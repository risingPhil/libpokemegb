#include "RomReader.h"
#include "utils.h"

#include <cstring>

static const uint8_t bitIndexMaskLookUpTable[] = {
    0xFF,
    0x40,
    0x20,
    0x10,
    0x8,
    0x4,
    0x2,
    0x1
};

static uint8_t readBit(IRomReader& romReader, uint8_t& bitIndex)
{
    const uint8_t result = (romReader.peek() & bitIndexMaskLookUpTable[bitIndex]) >> (7 - bitIndex);
    
    if(bitIndex == 7)
    {
        romReader.advance();
        bitIndex = 0;
    }
    else
    {
        ++bitIndex;
    }
    return result;
}

IRomReader::~IRomReader()
{
}

BaseRomReader::BaseRomReader()
    : cpuLittleEndian_(isCurrentCPULittleEndian())
{
}

BaseRomReader::~BaseRomReader()
{
}

bool BaseRomReader::readUint16(uint16_t& outByte, Endianness fieldEndianness)
{
    if(!read((uint8_t*)&outByte, 2))
    {
        return false;
    }

    if(fieldEndianness == Endianness::LITTLE && !cpuLittleEndian_)
    {
        outByte = byteSwapUint16(outByte);
    }
    else if(fieldEndianness == Endianness::BIG && cpuLittleEndian_)
    {
        outByte = byteSwapUint16(outByte);
    }

    return true;
}

bool BaseRomReader::readUint32(uint32_t& outByte, Endianness fieldEndianness)
{
    if(!read((uint8_t*)&outByte, 4))
    {
        return false;
    }

    if(fieldEndianness == Endianness::LITTLE && !cpuLittleEndian_)
    {
        outByte = byteSwapUint32(outByte);
    }
    else if(fieldEndianness == Endianness::BIG && cpuLittleEndian_)
    {
        outByte = byteSwapUint32(outByte);
    }

    return true;
}

bool BaseRomReader::seekToBank(uint8_t bankIndex)
{
    // 0x4000 represents 16 KB. Each gameboy rom bank has a size of 16 KB
    return seek(((uint32_t)bankIndex) * 0x4000);
}

bool BaseRomReader::seekToRomPointer(uint16_t pointer, uint8_t bankIndex)
{
    if(bankIndex == 0xFF)
    {
        bankIndex = getCurrentBankIndex();
    }
    if(!seekToBank(bankIndex))
    {
        return false;
    }
    
    // This needs a bit of background: 
    // The gameboy can address 32 KB of rom data at one time.
    // To be able to support a bigger rom, a bank switching mechanism exists in most cartridges.
    // a ROM is divided into blocks (=banks) of 16 KB.
    // The first 16 KB of gameboy address space (0x0000 - 0x4000) and will ALWAYS be mapped to bank 0 of the ROM data on the cartridge. 
    // It is not possible to switch this to a different bank
    // The next 16 KB is switchable on command and is mapped from 0x4000 - 0x8000
    // When we read pointers from the ROM, it can be either 2 bytes (little endian) or 3 bytes. In the case of 3 bytes, the additional 1 byte refers to the bank index.
    // In any case, the 2 bytes refer to an absolute pointer in mapped gameboy memory when the correct bank has been mapped/mounted.
    // Since the first 16 KB of the address space in that scenario ALWAYS refers to bank 0, we need to subtract it from the pointer (0x4000) in order to get an offset relative to the start of the bank it is referring to.
    // source: https://www.smogon.com/smog/issue27/glitch
    pointer -= 0x4000;
    return advance(pointer);
}

uint32_t BaseRomReader::readUntil(uint8_t* outBuffer, uint8_t terminator, uint32_t maxRead)
{
    return readUntil(outBuffer, &terminator, 1, maxRead);
}

uint32_t BaseRomReader::readUntil(uint8_t* outBuffer, const uint8_t* terminatorList, uint8_t numTerminators, uint32_t maxRead)
{
    uint8_t* cur = outBuffer;
    uint8_t i;
    while(static_cast<uint32_t>(cur - outBuffer) < maxRead && read(cur, 1))
    {
        for(i=0; i < numTerminators; ++i)
        {
            if((*cur) == terminatorList[i])
            {
                return (cur - outBuffer);
            }
        }
        ++cur;
    }
    return (cur - outBuffer);
}

BufferBasedRomReader::BufferBasedRomReader(const uint8_t* buffer, uint32_t bufferSize)
    : buffer_(buffer)
    , end_(buffer + bufferSize)
    , cur_(buffer_)
{
}

BufferBasedRomReader::~BufferBasedRomReader()
{
}

bool BufferBasedRomReader::readByte(uint8_t& outByte)
{
    return read(&outByte, 1);
}

bool BufferBasedRomReader::read(uint8_t* outBuffer, uint32_t bytesToRead)
{
    const uint8_t* newPos = cur_ + bytesToRead;
    if(newPos <= end_)
    {
        memcpy(outBuffer, cur_, bytesToRead);
        cur_ = newPos;
        return true;
    }
    return false;
}

uint8_t BufferBasedRomReader::peek()
{
    return (*cur_);
}

bool BufferBasedRomReader::advance(uint32_t numBytes)
{
    return seek((cur_ - buffer_) + numBytes);
}

bool BufferBasedRomReader::seek(uint32_t absoluteOffset)
{
    const uint8_t* newPos = buffer_ + absoluteOffset;
    if(newPos < end_)
    {
        cur_ = newPos;
        return true;
    }
    return false;
}

bool BufferBasedRomReader::searchFor(const uint8_t* needle, uint32_t needleLength)
{
    const uint8_t* ret = memSearch(cur_, (end_ - cur_), needle, needleLength);
    if(ret)
    {
        cur_ = ret;
        return true;
    }
    return false;
}

uint8_t BufferBasedRomReader::getCurrentBankIndex() const
{
    // every bank is 16 KB (=0x4000)
    return (uint8_t)((cur_ - buffer_) % 0x4000);
}

BitReader::BitReader(IRomReader& romReader)
    : romReader_(romReader)
    , bitIndex_(0)
{
}

uint8_t BitReader::read(uint8_t numBits)
{
    uint8_t result = 0;
    uint8_t i;
    
    for(i=0; i < numBits; ++i)
    {
        result <<= 1;
        result |= readBit(romReader_, bitIndex_);
    }
    return result;
}

bool BitReader::seek(uint32_t byteOffset, uint8_t bitOffset)
{
    if(!romReader_.seek(byteOffset))
    {
        return false;
    }
    bitIndex_ = bitOffset;
    return true;
}

bool readGameboyCartridgeHeader(IRomReader& romReader, GameboyCartridgeHeader& cartridgeHeader)
{
    memset(&cartridgeHeader, 0, sizeof(struct GameboyCartridgeHeader));

    romReader.seek(0x100);
    romReader.read(cartridgeHeader.entry, 4);
    romReader.seek(0x134);
    romReader.read((uint8_t*)cartridgeHeader.title, 16);
    romReader.seek(0x13F);
    romReader.read((uint8_t*)cartridgeHeader.manufacturer_code, 4);
    romReader.seek(0x143);
    romReader.readByte(cartridgeHeader.CGB_flag);
    romReader.read((uint8_t*)cartridgeHeader.new_licensee_code, 2);
    romReader.readByte(cartridgeHeader.SGB_flag);
    romReader.readByte(cartridgeHeader.cartridge_type);
    romReader.readByte(cartridgeHeader.rom_size);
    romReader.readByte(cartridgeHeader.ram_size);
    romReader.readByte(cartridgeHeader.destination_code);
    romReader.readByte(cartridgeHeader.old_licensee_code);
    romReader.readByte(cartridgeHeader.mask_rom_version_number);
    romReader.readByte(cartridgeHeader.header_checksum);
    romReader.read(cartridgeHeader.global_checksum, 2);

    return true;
}
