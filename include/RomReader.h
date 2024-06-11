#ifndef ROMREADER_H
#define ROMREADER_H

#include "common.h"

#include <cstdint>

class IRomReader
{
public:
    virtual ~IRomReader();

    /**
     * @brief This function reads a byte, returns it and advances the internal pointer by 1 byte
     * 
     * @return uint8_t 
     */
    virtual bool readByte(uint8_t& outByte) = 0;

    /**
     * @brief This function reads multiple bytes into the specified output buffer
     * 
     * @return whether the operation was successful
     */
    virtual bool read(uint8_t* outBuffer, uint32_t bytesToRead) = 0;

    /**
     * @brief This function keeps reading until either maxRead number of bytes have been read or the terminator has been encountered.
     */
    virtual uint32_t readUntil(uint8_t* outBuffer, uint8_t terminator, uint32_t maxRead) = 0;

    /**
     * @brief Reads a 16 bit unsigned integer from the rom at the current position.
     * The Endianness enum indicates whether the field we're trying to read is actually stored in little endian or big endian form.
     * Turns out there's a mix of them in the roms even though most fields are in little endian.
     * 
     * We need to use this value to determine whether we should do a byteswap for the CPU we're running this program on.
     */
    virtual bool readUint16(uint16_t& outByte, Endianness fieldEndianness = Endianness::LITTLE) = 0;

    /**
     * @brief See @readUint16. This is the same but then for uint32_t
     */
    virtual bool readUint32(uint32_t& outByte, Endianness fieldEndianness = Endianness::LITTLE) = 0;

    /**
     * @brief This function reads the current byte without advancing the internal pointer by 1 byte
     * 
     * @return uint8_t 
     */
    virtual uint8_t peek() = 0;

    /**
     * @brief This function advances the internal pointer by the specified numBytes.
     * This can be considered a relative seek
     * 
     */
    virtual bool advance(uint32_t numBytes = 1) = 0;

    /**
     * @brief This function seeks to the specified absolute rom offset
     * 
     * @param absoluteOffset 
     */
    virtual bool seek(uint32_t absoluteOffset) = 0;

    /**
     * @brief Returns the index of the current bank
     */
    virtual uint8_t getCurrentBankIndex() const = 0;

    /**
     * @brief This function seeks to the start of the specified bank index
     * 
     * @param bankIndex 
     * @return true 
     * @return false 
     */
    virtual bool seekToBank(uint8_t bankIndex) = 0;

    /**
     * @brief This function seeks to the specified rom pointer. Note this should be an unprocessed pointer as stored in the rom (except for endianness conversions done by readUint16)
     * The reason is that the pointers stored in the rom assume the memory address of the rom location AFTER it has been mounted/mapped in the gameboy memory.
     * Therefore this function will subtract 0x4000 from it (because 0x0000 - 0x4000 in gameboy address space is ALWAYS occupied by bank 0)
     * 
     * If bankIndex is not specified (= set to 0xFF), we will seek to the pointer in the current bank
     */
    virtual bool seekToRomPointer(uint16_t pointer, uint8_t bankIndex = 0xFF) = 0;

    /**
     * @brief This function searches for a sequence of bytes (the needle) in the buffer starting from
     * the current internal position

     * @return true we found the sequence of bytes and we've seeked toward this point
     * @return false we didn't find the sequence of bytes anywhere
     */
    virtual bool searchFor(const uint8_t* needle, uint32_t needleLength) = 0;
protected:
private:
};

class BaseRomReader : public IRomReader
{
public:
    BaseRomReader();
    virtual ~BaseRomReader();

    bool readUint16(uint16_t& outByte, Endianness fieldEndianness = Endianness::LITTLE) override;
    bool readUint32(uint32_t& outByte, Endianness fieldEndianness = Endianness::LITTLE) override;

    bool seekToBank(uint8_t bankIndex) override;
    bool seekToRomPointer(uint16_t pointer, uint8_t bankIndex = 0xFF) override;

    uint32_t readUntil(uint8_t* outBuffer, uint8_t terminator, uint32_t maxRead) override;
protected:
private:
    const bool cpuLittleEndian_;
};

class BufferBasedRomReader : public BaseRomReader
{
public:
    BufferBasedRomReader(const uint8_t* buffer, uint32_t bufferSize);
    virtual ~BufferBasedRomReader();

    /**
     * @brief This function reads a byte, returns it and advances the internal pointer by 1 byte
     * 
     * @return uint8_t 
     */
    bool readByte(uint8_t& outByte) override;

    /**
     * @brief This function reads multiple bytes into the specified output buffer
     * 
     * @return whether the operation was successful
     */
    bool read(uint8_t* outBuffer, uint32_t bytesToRead) override;

    /**
     * @brief This function reads the current byte without advancing the internal pointer by 1 byte
     * 
     * @return uint8_t 
     */
    uint8_t peek() override;

    /**
     * @brief This function advances the internal pointer by 1 byte
     * 
     */
    bool advance(uint32_t numBytes = 1) override;

    /**
     * @brief This function seeks to the specified absolute rom offset
     * 
     * @param absoluteOffset 
     */
    bool seek(uint32_t absoluteOffset) override;

    /**
     * @brief This function searches for a sequence of bytes (the needle) in the buffer starting from
     * the current internal position

     * @return true we found the sequence of bytes and we've seeked toward this point
     * @return false we didn't find the sequence of bytes anywhere
     */
    bool searchFor(const uint8_t* needle, uint32_t needleLength) override;

    uint8_t getCurrentBankIndex() const override;
protected:
private:
    const uint8_t* const buffer_;
    const uint8_t* const end_;
    const uint8_t* cur_;
};

class BitReader
{
public:
    BitReader(IRomReader& romReader);

    /**
     * @brief Read the specified number of bits
     * 
     * @param numBits 
     * @return uint8_t 
     */
    uint8_t read(uint8_t numBits);

    bool seek(uint32_t byteOffset, uint8_t bitOffset = 0);
protected:
private:
    IRomReader& romReader_;
    uint8_t bitIndex_;
};

bool readGameboyCartridgeHeader(IRomReader& romReader, GameboyCartridgeHeader& cartridgeHeader);

#endif