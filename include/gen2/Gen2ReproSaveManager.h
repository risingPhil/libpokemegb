#ifndef _GEN2REPROSAVEMANAGER_H
#define _GEN2REPROSAVEMANAGER_H

#include "RomReader.h"
#include "SaveManager.h"

enum class Gen2ReproType
{
    INVALID,
    ALIEXPRESS_TYPE1
};

typedef struct Gen2ReproSRAMRomOffsetMetadata
{
    // A list of rom offsets for each SRAM bank
    struct {
        uint8_t romBankIndex;
        uint16_t romBankOffset;
    } sramBankOffsets[4];
} Gen2ReproSRAMRomOffsetMetadata;

/**
 * @brief This class provides a way to read save data from a reproduction cart
 * Reproduction carts save their data inside the rom data.
 */
class Gen2ReproSaveManager : public BaseSaveManager
{
public:
    Gen2ReproSaveManager(IRomReader& romReader, Gen2ReproType reproType, bool writeEnabled);
    virtual ~Gen2ReproSaveManager();

    /**
     * @brief This function reads a byte, returns it and advances the internal pointer by 1 byte
     * 
     * @return uint8_t 
     */
    bool readByte(uint8_t& outByte) override;
    void writeByte(uint8_t byte) override;

    /**
     * @brief This function reads multiple bytes into the specified output buffer
     * @note: bytesToRead should be <= 0x2000 (SRAM bank size)
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

    /**
     * @brief This function rewinds the internal pointer with the specified numBytes
     * This can be considered a backward relative seek
     */
    bool rewind(uint32_t numBytes = 1) override;

    /**
     * @brief This function seeks to the specified absolute SRAM offset
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
    IRomReader& romReader_;
    Gen2ReproType reproType_;
    const Gen2ReproSRAMRomOffsetMetadata* reproRomOffsetMeta_;
    uint32_t absoluteOffset_;
    bool writeEnabled_;
};

#endif