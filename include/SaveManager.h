#ifndef _SAVEMANAGER_H
#define _SAVEMANAGER_H

#include "common.h"
#include <cstdint>

class ISaveManager
{
public:
    virtual ~ISaveManager();

    /**
     * @brief This function reads a byte, returns it and advances the internal pointer by 1 byte
     * 
     * @return uint8_t 
     */
    virtual bool readByte(uint8_t& outByte) = 0;
    virtual void writeByte(uint8_t byte) = 0;

    /**
     * @brief This function reads multiple bytes into the specified output buffer
     * 
     * @return whether the operation was successful
     */
    virtual bool read(uint8_t* outBuffer, uint32_t bytesToRead) = 0;
    virtual void write(const uint8_t* buffer, uint32_t bytesToWrite) = 0;

    /**
     * @brief This function keeps reading until either maxRead number of bytes have been read or the terminator has been encountered.
     */
    virtual uint32_t readUntil(uint8_t* outBuffer, uint8_t terminator, uint32_t maxRead) = 0;

    /**
     * @brief Reads a 16 bit unsigned integer from the SRAM at the current position.
     * The Endianness enum indicates whether the field we're trying to read is actually stored in little endian or big endian form.
     * Turns out there's a mix of them in the rom/SRAM even though most fields are in little endian.
     * 
     * We need to use this value to determine whether we should do a byteswap for the CPU we're running this program on.
     */
    virtual bool readUint16(uint16_t& outBytes, Endianness fieldEndianness = Endianness::LITTLE) = 0;

    /**
     * @brief Writes a 16 bit unsigned integer to the SRAM at the current position.
     * The Endianness enum indicates whether the field we're trying to read is actually stored in little endian or big endian form.
     * Turns out there's a mix of them in the rom/SRAM even though most fields are in little endian.
     * 
     * We need to use this value to determine whether we should do a byteswap for the CPU we're running this program on.
     */
    virtual void writeUint16(uint16_t bytes, Endianness fieldEndianness = Endianness::LITTLE) = 0;

    /**
     * @brief See @readUint16. This is the same but then for reading 3 bytes and returning it as 32 bit uint
     */
    virtual bool readUint24(uint32_t& outBytes, Endianness fieldEndianness = Endianness::LITTLE) = 0;

    /**
     * @brief See @writeUint16. This is the same, but for writing 3 bytes in the right endianness for the SRAM
     */
    virtual void writeUint24(uint32_t bytes, Endianness fieldEndianness = Endianness::LITTLE) = 0;

    /**
     * @brief See @readUint16. This is the same but then for uint32_t
     */
    virtual bool readUint32(uint32_t& outBytes, Endianness fieldEndianness = Endianness::LITTLE) = 0;

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
     * @brief This function rewinds the internal pointer with the specified numBytes
     * This can be considered a backward relative seek
     */
    virtual bool rewind(uint32_t numBytes = 1) = 0;

    /**
     * @brief This function seeks to the specified absolute SRAM offset
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
     */
    virtual bool seekToBank(uint8_t bankIndex) = 0;

    /**
     * @brief This function seeks to the specified offset in the specified save bank.
     */
    virtual bool seekToBankOffset(uint8_t bankIndex, uint16_t offset) = 0;

    /**
     * @brief Copies a region from the save to another region. 
     */
    virtual void copyRegion(uint32_t absoluteSrcOffset, uint32_t absoluteDstOffsetStart, uint32_t numBytes) = 0;
protected:
private:
};

class BaseSaveManager : public ISaveManager
{
public:
    BaseSaveManager();
    virtual ~BaseSaveManager();

    bool readUint16(uint16_t& outBytes, Endianness fieldEndianness = Endianness::LITTLE) override;
    void writeUint16(uint16_t bytes, Endianness fieldEndianness = Endianness::LITTLE) override;
    bool readUint24(uint32_t& outBytes, Endianness fieldEndianness = Endianness::LITTLE) override;
    void writeUint24(uint32_t bytes, Endianness fieldEndianness = Endianness::LITTLE) override;
    bool readUint32(uint32_t& outBytes, Endianness fieldEndianness = Endianness::LITTLE) override;

    bool seekToBank(uint8_t bankIndex) override;
    bool seekToBankOffset(uint8_t bankIndex, uint16_t offset);

    uint32_t readUntil(uint8_t* outBuffer, uint8_t terminator, uint32_t maxRead) override;

    void copyRegion(uint32_t absoluteSrcOffset, uint32_t absoluteDstOffsetStart, uint32_t absoluteDstOffsetEnd) override;
protected:
private:
    const bool cpuLittleEndian_;
};

class BufferBasedSaveManager : public BaseSaveManager
{
public:
    BufferBasedSaveManager(uint8_t* buffer, uint32_t bufferSize);
    virtual ~BufferBasedSaveManager();
    /**
     * @brief This function reads a byte, returns it and advances the internal pointer by 1 byte
     * 
     * @return uint8_t 
     */
    bool readByte(uint8_t& outByte) override;
    virtual void writeByte(uint8_t byte) override;

    /**
     * @brief This function reads multiple bytes into the specified output buffer
     * 
     * @return whether the operation was successful
     */
    bool read(uint8_t* outBuffer, uint32_t bytesToRead) override;
    void write(const uint8_t* buffer, uint32_t bytesToWrite) override;

    /**
     * @brief This function reads the current byte without advancing the internal pointer by 1 byte
     * 
     * @return uint8_t 
     */
    uint8_t peek() override;

    /**
     * @brief This function advances the internal pointer by the specified numBytes.
     * This can be considered a relative seek
     * 
     */
    bool advance(uint32_t numBytes = 1) override;
    bool rewind(uint32_t numBytes = 1) override;

    /**
     * @brief This function seeks to the specified absolute save file/buffer offset
     * 
     * @param absoluteOffset 
     */
    bool seek(uint32_t absoluteOffset) override;

    /**
     * @brief Returns the index of the current bank
     */
    uint8_t getCurrentBankIndex() const override;
protected:
private:
    uint8_t* const buffer_;
    uint8_t* const end_;
    uint8_t* cur_;
};

#endif