#include "gen2/Gen2ReproSaveManager.h"

static const Gen2ReproSRAMRomOffsetMetadata aliexpressReproRomOffsets = {
    .sramBankOffsets = {
        {
            .romBankIndex = 0x74,
            .romBankOffset = 0x6000
        },
        {
            .romBankIndex = 0x75,
            .romBankOffset = 0x6000
        },
        {
            .romBankIndex = 0x76,
            .romBankOffset = 0x6000
        },
        {
            .romBankIndex = 0x77,
            .romBankOffset = 0x6000
        }
    }
};

static uint8_t calculateBankIndex(uint32_t absoluteSRAMOffset)
{
    return (absoluteSRAMOffset / 0x2000);
}

static const Gen2ReproSRAMRomOffsetMetadata* getReproSaveRomOffsets(Gen2ReproType reproType)
{
    switch(reproType)
    {
        case Gen2ReproType::ALIEXPRESS_TYPE1:
            return &aliexpressReproRomOffsets;
        default:
            return nullptr;
    }
}

Gen2ReproSaveManager::Gen2ReproSaveManager(IRomReader& romReader, Gen2ReproType reproType, bool writeEnabled)
    : romReader_(romReader)
    , reproType_(reproType)
    , reproRomOffsetMeta_(getReproSaveRomOffsets(reproType))
    , absoluteOffset_(0)
    , writeEnabled_(writeEnabled)
{
    seek(0);
}

Gen2ReproSaveManager::~Gen2ReproSaveManager()
{
}

bool Gen2ReproSaveManager::readByte(uint8_t& outByte)
{
    return read(&outByte, 1);
}

void Gen2ReproSaveManager::writeByte(uint8_t byte)
{
    write(&byte, 1);
}

bool Gen2ReproSaveManager::read(uint8_t* outBuffer, uint32_t bytesToRead)
{
    bool ret = romReader_.read(outBuffer, bytesToRead);
    if(!ret)
    {
        return ret;
    }

    ret &= advance(bytesToRead);

    return ret;
}

void Gen2ReproSaveManager::write(const uint8_t* buffer, uint32_t bytesToWrite)
{
    (void)buffer;
    (void)bytesToWrite;
    if(!writeEnabled_)
    {
        return;
    }

    //TODO:
}

uint8_t Gen2ReproSaveManager::peek()
{
    return romReader_.peek();
}

bool Gen2ReproSaveManager::advance(uint32_t numBytes)
{
    absoluteOffset_ += numBytes;
    return seek(absoluteOffset_);
}

bool Gen2ReproSaveManager::rewind(uint32_t numBytes)
{
    absoluteOffset_ -= numBytes;
    return seek(absoluteOffset_);
}

bool Gen2ReproSaveManager::seek(uint32_t absoluteOffset)
{
    const uint8_t bankIndex = calculateBankIndex(absoluteOffset);

    if(!reproRomOffsetMeta_ || bankIndex > 3)
    {
        return false;
    }

    absoluteOffset_ = absoluteOffset;

    const uint16_t localOffset = absoluteOffset % 0x2000;
    return romReader_.seekToRomPointer(reproRomOffsetMeta_->sramBankOffsets[bankIndex].romBankOffset + localOffset, reproRomOffsetMeta_->sramBankOffsets[bankIndex].romBankIndex);
}

uint8_t Gen2ReproSaveManager::getCurrentBankIndex() const
{
    return calculateBankIndex(absoluteOffset_);
}