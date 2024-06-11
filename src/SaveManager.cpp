#include "SaveManager.h"
#include "utils.h"

#include <cstring>

ISaveManager::~ISaveManager()
{
}

BaseSaveManager::BaseSaveManager()
    : cpuLittleEndian_(isCurrentCPULittleEndian())
{
}

BaseSaveManager::~BaseSaveManager()
{
}

bool BaseSaveManager::readUint16(uint16_t& outBytes, Endianness fieldEndianness)
{
    if(!read((uint8_t*)&outBytes, 2))
    {
        return false;
    }

    if(fieldEndianness == Endianness::LITTLE && !cpuLittleEndian_)
    {
        outBytes = byteSwapUint16(outBytes);
    }
    else if(fieldEndianness == Endianness::BIG && cpuLittleEndian_)
    {
        outBytes = byteSwapUint16(outBytes);
    }
    return true;
}

void BaseSaveManager::writeUint16(uint16_t bytes, Endianness fieldEndianness)
{
    // when reading the data before, we might have done a byte swap. To undo it while writing, we just need to repeat it.
    if(fieldEndianness == Endianness::LITTLE && !cpuLittleEndian_)
    {
        bytes = byteSwapUint16(bytes);
    }
    else if(fieldEndianness == Endianness::BIG && cpuLittleEndian_)
    {
        bytes = byteSwapUint16(bytes);
    }
    write((uint8_t*)&bytes, 2);
}

bool BaseSaveManager::readUint24(uint32_t& outByte, Endianness fieldEndianness)
{
    uint8_t bytes[3];

    if(!read(bytes, 3))
    {
        return false;
    }

    if(fieldEndianness == Endianness::LITTLE)
    {
        outByte = (((uint32_t)bytes[2]) << 16) | (((uint32_t)bytes[1]) << 8) | bytes[0];
    }
    else
    {
        outByte = (((uint32_t)bytes[0]) << 16) | (((uint32_t)bytes[1]) << 8) | bytes[2];
    }

    return true;
}

void BaseSaveManager::writeUint24(uint32_t bytes, Endianness fieldEndianness)
{
    uint8_t bytesToWrite[3];

    if(fieldEndianness == Endianness::LITTLE)
    {
        bytesToWrite[0] = (uint8_t)(bytes & 0xFF);
        bytesToWrite[1] = (uint8_t)((bytes & 0xFF00) >> 8);
        bytesToWrite[2] = (uint8_t)((bytes & 0xFF0000) >> 16);
    }
    else
    {
        bytesToWrite[0] = (uint8_t)((bytes & 0xFF0000) >> 16);
        bytesToWrite[1] = (uint8_t)((bytes & 0xFF00) >> 8);
        bytesToWrite[2] = (uint8_t)(bytes & 0xFF);   
    }

    write(bytesToWrite, sizeof(bytesToWrite));
}

bool BaseSaveManager::readUint32(uint32_t& outByte, Endianness fieldEndianness)
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

bool BaseSaveManager::seekToBank(uint8_t bankIndex)
{
    // 0x2000 represents 8KB. Each gameboy sram bank has a size of 8KB.
    return seek(((uint32_t)bankIndex) * 0x2000);
}

bool BaseSaveManager::seekToBankOffset(uint8_t bankIndex, uint16_t offset)
{
    if(!seekToBank(bankIndex))
    {
        return false;
    }
    return advance(offset);
}

uint32_t BaseSaveManager::readUntil(uint8_t* outBuffer, uint8_t terminator, uint32_t maxRead)
{
    uint8_t* cur = outBuffer;
    while(static_cast<uint32_t>(cur - outBuffer) < maxRead && read(cur, 1))
    {
        if((*cur) == terminator)
        {
            break;
        }
        ++cur;
    }
    return (cur - outBuffer);
}

void BaseSaveManager::copyRegion(uint32_t absoluteSrcOffset, uint32_t absoluteDstOffsetStart, uint32_t numBytes)
{
    uint8_t buffer[128];
    uint32_t srcOffset = absoluteSrcOffset;
    // include the end byte of the range
    uint32_t bytesRemaining = numBytes;
    uint8_t bytesToRead;
    while(bytesRemaining > 0)
    {
        if(bytesRemaining > sizeof(buffer))
        {
            bytesToRead = sizeof(buffer);
        }
        else
        {
            bytesToRead = bytesRemaining;
        }

        seek(srcOffset);
        read(buffer, bytesToRead);

        seek(absoluteDstOffsetStart + numBytes - bytesRemaining);
        write(buffer, bytesToRead);

        srcOffset += bytesToRead;
        bytesRemaining -= bytesToRead;
    }
}

BufferBasedSaveManager::BufferBasedSaveManager(uint8_t* buffer, uint32_t bufferSize)
    : buffer_(buffer)
    , end_(buffer + bufferSize)
    , cur_(buffer)
{
}

BufferBasedSaveManager::~BufferBasedSaveManager()
{
}

bool BufferBasedSaveManager::readByte(uint8_t& outByte)
{
    return read(&outByte, 1);
}

void BufferBasedSaveManager::writeByte(uint8_t byte)
{
    write(&byte, 1);
}

bool BufferBasedSaveManager::read(uint8_t* outBuffer, uint32_t bytesToRead)
{
    uint8_t* newPos = cur_ + bytesToRead;
    if(newPos < end_)
    {
        memcpy(outBuffer, cur_, bytesToRead);
        cur_ = newPos;
        return true;
    }
    return false;
}

void BufferBasedSaveManager::write(const uint8_t* buffer, uint32_t bytesToWrite)
{
    uint8_t* newPos = cur_ + bytesToWrite;
    if(newPos < end_)
    {
        memcpy(cur_, buffer, bytesToWrite);
        cur_ = newPos;
    }
}

uint8_t BufferBasedSaveManager::peek()
{
    return (*cur_);
}

bool BufferBasedSaveManager::advance(uint32_t numBytes)
{
    uint8_t* newPos = cur_ + numBytes;
    if(newPos < end_)
    {
        cur_ = newPos;
        return true;
    }
    return false;
}

bool BufferBasedSaveManager::rewind(uint32_t numBytes)
{
    uint8_t* newPos = cur_ - numBytes;

    if(newPos >= buffer_)
    {
        cur_ = newPos;
        return true;
    }

    return false;
}

bool BufferBasedSaveManager::seek(uint32_t absoluteOffset)
{
    uint8_t* newPos = buffer_ + absoluteOffset;
    if(newPos < end_)
    {
        cur_ = newPos;
        return true;
    }
    return false;
}

uint8_t BufferBasedSaveManager::getCurrentBankIndex() const
{
    // 0x2000 represents the bank size of 8 KB
    return (uint8_t)((cur_ - buffer_) / 0x2000);
}