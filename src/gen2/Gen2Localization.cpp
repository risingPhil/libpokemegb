#include "gen2/Gen2Localization.h"
#include "gen2/Gen2Common.h"

static const Gen2LocalizationRomOffsets g2_dummyROMOffsets = {
    .stats = 0,
    .names = 0,
    .iconTypes = 0,
    .icons = 0,
    .spritePointers = 0,
    .spritePalettes = 0
};

/**
 * @brief Pokemon Gold rom offsets
 */
static const Gen2LocalizationRomOffsets g2_localizationROMOffsetsG[] = {
    // ENGLISH
    {
        .stats = 0x51B0B,
        .names = 0x1B0B74,
        .iconTypes = 0x8E975,
        .icons = 0x8EA70,
        .spritePointers = 0x48000,
        .spritePalettes = 0xAD45
    },
    // FRENCH
    {
        .stats = 0x51B10,
        .names = 0x1B0BC5,
        .iconTypes = 0x8E977,
        .icons = 0x8EA72,
        .spritePointers = 0x48000,
        .spritePalettes = 0xAD45
    },
    // ITALIAN
    {
        .stats = 0x51B19,
        .names = 0x1B0BD2,
        .iconTypes = 0x8E973,
        .icons = 0x8EA6E,
        .spritePointers = 0x48000,
        .spritePalettes = 0xAD45
    },
    // SPANISH
    {
        .stats = 0x51B19,
        .names = 0x1B0BB8,
        .iconTypes = 0x8E973,
        .icons = 0x8EA6E,
        .spritePointers = 0x48000,
        .spritePalettes = 0xAD45
    },
    // GERMAN
    {
        .stats = 0x51B00,
        .names = 0x1b0b6b,
        .iconTypes = 0x8E977,
        .icons = 0x8EA72,
        .spritePointers = 0x48000,
        .spritePalettes = 0xAD45
    },
    // KOREAN
    {
        .stats = 0x51BDF,
        .names = 0x1B0C4A,
        .iconTypes = 0x8E96D,
        .icons = 0x8EA68,
        .spritePointers = 0x48000,
        .spritePalettes = 0xAD15
    },
    // JAPANESE
    {
        .stats = 0x51AA9,
        .names = 0x53A09,
        .iconTypes = 0x8E795,
        .icons = 0x8E890,
        .spritePointers = 0x48000,
        .spritePalettes = 0xACCB
    }
};

/**
 * @brief Pokemon Silver rom offsets
 */
static const Gen2LocalizationRomOffsets g2_localizationROMOffsetsS[] = {
    // ENGLISH
    {
        .stats = 0x51B0B,
        .names = 0x1B0B74,
        .iconTypes = 0x8E95B,
        .icons = 0x8EA56,
        .spritePointers = 0x48000,
        .spritePalettes = 0xAD45
    },
    // FRENCH
    {
        .stats = 0x51B10,
        .names = 0x1B0BC5,
        .iconTypes = 0x8E95D,
        .icons = 0x8EA58,
        .spritePointers = 0x48000,
        .spritePalettes = 0xAD45
    },
    // ITALIAN
    {
        .stats = 0x51B19,
        .names = 0x1B0BD2,
        .iconTypes = 0x8E959,
        .icons = 0x8EA54,
        .spritePointers = 0x48000,
        .spritePalettes = 0xAD45
    },
    // SPANISH
    {
        .stats = 0x51B19,
        .names = 0x1B0BB8,
        .iconTypes = 0x8E959,
        .icons = 0x8EA54,
        .spritePointers = 0x48000,
        .spritePalettes = 0xAD45
    },
    // GERMAN
    {
        .stats = 0x51B00,
        .names = 0x1B0B6B,
        .iconTypes = 0x8E95D,
        .icons = 0x8EA58,
        .spritePointers = 0x48000,
        .spritePalettes = 0xAD45
    },
    // KOREAN
    {
        .stats = 0x51BDF,
        .names = 0x1B0C4A,
        .iconTypes = 0x8E953,
        .icons = 0x8EA4E,
        .spritePointers = 0x48000,
        .spritePalettes = 0xAD15
    },
    // JAPANESE
    {
        .stats = 0x51AA9,
        .names = 0x53A09,
        .iconTypes = 0x8E780,
        .icons = 0x8E87B,
        .spritePointers = 0x48000,
        .spritePalettes = 0xACCB
    }
};

/**
 * @brief Pokemon Crystal rom offsets
 */
static const Gen2LocalizationRomOffsets g2_localizationROMOffsetsC[] = {
    // ENGLISH
    {
        .stats = 0x051424,
        .names = 0x53384,
        .iconTypes = 0x8EAC4,
        .icons = 0x8EBBF,
        .spritePointers = 0x120000,
        .spritePalettes = 0xA8D6
    },
    // FRENCH
    {
        .stats = 0x051417,
        .names = 0x53377,
        .iconTypes = 0x8EAC4,
        .icons = 0x8EBBF,
        .spritePointers = 0x120000,
        .spritePalettes = 0xA8D6
    },
    // ITALIAN
    {
        .stats = 0x051433,
        .names = 0x53393,
        .iconTypes = 0x8EAC4,
        .icons = 0x8EBBF,
        .spritePointers = 0x120000,
        .spritePalettes = 0xA8D6
    },
    // SPANISH
    {
        .stats = 0x5142D,
        .names = 0x5338D,
        .iconTypes = 0x8EAC4,
        .icons = 0x8EBBF,
        .spritePointers = 0x120000,
        .spritePalettes = 0xA8D6
    },
    // GERMAN
    {
        .stats = 0x5140E,
        .names = 0x5336E,
        .iconTypes = 0x8EAC4,
        .icons = 0x8EBBF,
        .spritePointers = 0x120000,
        .spritePalettes = 0xA8D6
    },
    // KOREAN
    { // NOT SUPPORTED! Apparently Pokémon Crystal was never released in Korea!
        .stats = 0,
        .names = 0,
        .iconTypes = 0,
        .icons = 0,
        .spritePointers = 0,
        .spritePalettes = 0
    },
    // JAPANESE
    {
        .stats = 0x514BA,
        .names = 0x5341A,
        .iconTypes = 0x8EAC4,
        .icons = 0x8EBBF,
        .spritePointers = 0x120000,
        .spritePalettes = 0xA893
    }
};

static const Gen2LocalizationSRAMOffsets g2_dummySRAMOffsets = {
    .currentBoxIndex = 0,
    .currentBox = 0,
    .dexSeen = 0,
    .dexOwned = 0,
    .party = 0,
    .eventFlags = 0,
    .mainChecksum = 0,
    .mainChecksumDataEndPos = 0,
    .backupChecksum = 0,
    .rivalName = 0,
    .gsBallMain = 0,
    .gsBallBackup = 0,
    .itemPocketTMHM = 0,
    .itemPocketItem = 0,
    .itemPocketKey = 0,
    .itemPocketBall = 0,
    .itemPocketPC = 0,
    .rtcFlags = 0,
    .mysteryGiftUnlocked = 0,
    .rtcDayOffset = 0
};

static const Gen2LocalizationSRAMOffsets g2_internationalSRAMOffsetsGS = {
    .currentBoxIndex = 0x2724,
    .currentBox = 0x2D6C,
    .dexSeen = 0x2A6C,
    .dexOwned = 0x2A4C,
    .party = 0x288A,
    .eventFlags = 0x261F,
    .mainChecksum = 0x2D69,
    .mainChecksumDataEndPos = 0x2D68,
    .backupChecksum = 0x7E6D,
    .rivalName = 0x2021,
    .gsBallMain = 0, // N/A for Gold & Silver
    .gsBallBackup = 0, // N/A for Gold & Silver
    .itemPocketTMHM = 0x23E6,
    .itemPocketItem = 0x241F,
    .itemPocketKey = 0x2449,
    .itemPocketBall = 0x2464,
    .itemPocketPC = 0x247E,
    .rtcFlags = 0xC60,
    .mysteryGiftUnlocked = 0xBE3,
    .rtcDayOffset = 0x2044
};

static const Gen2LocalizationSRAMOffsets g2_internationalSRAMOffsetsC = {
    .currentBoxIndex = 0x2700,
    .currentBox = 0x2D10,
    .dexSeen = 0x2A47,
    .dexOwned = 0x2A27,
    .party = 0x2865,
    .eventFlags = 0x2600,
    .mainChecksum = 0x2D0D,
    .mainChecksumDataEndPos = 0x2B82,
    .backupChecksum = 0x1F0D,
    .rivalName = 0x2021,
    .gsBallMain = 0x3E3C,
    .gsBallBackup = 0x3E44,
    .itemPocketTMHM = 0x23E7,
    .itemPocketItem = 0x2420,
    .itemPocketKey = 0x244A,
    .itemPocketBall = 0x2465,
    .itemPocketPC = 0x247F,
    .rtcFlags = 0xC60,
    .mysteryGiftUnlocked = 0xBE3,
    .rtcDayOffset = 0x2044
};

static const Gen2LocalizationSRAMOffsets g2_localizationSRAMOffsetsGS[] = {
    // ENGLISH
    g2_internationalSRAMOffsetsGS,
    // FRENCH
    g2_internationalSRAMOffsetsGS,
    // ITALIAN
    g2_internationalSRAMOffsetsGS,
    // SPANISH
    g2_internationalSRAMOffsetsGS,
    // GERMAN
    g2_internationalSRAMOffsetsGS,
    // KOREAN
    {
        .currentBoxIndex = 0x26FC,
        .currentBox = 0x2DAE,
        .dexSeen = 0x2AAE,
        .dexOwned = 0x2A8E,
        .party = 0x28CC,
        .eventFlags = 0x25F7,
        .mainChecksum = 0x2DAB,
        .mainChecksumDataEndPos = 0x2DAA,
        .backupChecksum = 0x7E6B,
        .rivalName = 0x2021,
        .gsBallMain = 0, // N/A for Gold & Silver
        .gsBallBackup = 0, // N/A for Gold & Silver
        .itemPocketTMHM = 0x23DE,
        .itemPocketItem = 0x2417,
        .itemPocketKey = 0x2441,
        .itemPocketBall = 0x245C,
        .itemPocketPC = 0x2476,
        .rtcFlags = 0x1060,
        .mysteryGiftUnlocked = 0x0,
        .rtcDayOffset = 0x0 // TODO
    },
    // JAPANESE
    {
        .currentBoxIndex = 0x2705,
        .currentBox = 0x2D10,
        .dexSeen = 0x29EE,
        .dexOwned = 0x29CE,
        .party = 0x283E,
        .eventFlags = 0x2600,
        .mainChecksum = 0x2D0D,
        .mainChecksumDataEndPos = 0x2C8B,
        .backupChecksum = 0x7F0D,
        .rivalName = 0x2017,
        .gsBallMain = 0, // N/A for Gold & Silver
        .gsBallBackup = 0, // N/A for Gold & Silver
        .itemPocketTMHM = 0x23C7,
        .itemPocketItem = 0x2400,
        .itemPocketKey = 0x242A,
        .itemPocketBall = 0x2445,
        .itemPocketPC = 0x245F,
        .rtcFlags = 0x1000,
        .mysteryGiftUnlocked = 0xB51,
        .rtcDayOffset = 0x0 // TODO
    }
};

static const Gen2LocalizationSRAMOffsets g2_localizationSRAMOffsetsC[] = {
    // ENGLISH
    g2_internationalSRAMOffsetsC,
    // FRENCH
    g2_internationalSRAMOffsetsC,
    // ITALIAN
    g2_internationalSRAMOffsetsC,
    // SPANISH
    g2_internationalSRAMOffsetsC,
    // GERMAN
    g2_internationalSRAMOffsetsC,
    // KOREAN
    g2_dummySRAMOffsets, // Crystal was never released in Korea!!
    // JAPANESE
    {
        .currentBoxIndex = 0x26E2,
        .currentBox = 0x2D10,
        .dexSeen = 0x29CA,
        .dexOwned = 0x29AA,
        .party = 0x281A,
        .eventFlags = 0x25E2,
        .mainChecksum = 0x2D0D,
        .mainChecksumDataEndPos = 0x2AE2,
        .backupChecksum = 0x7F0D,
        .rivalName = 0x2017,
        .gsBallMain = 0xA000,
        .gsBallBackup = 0xA083,
        .itemPocketTMHM = 0x23C9,
        .itemPocketItem = 0x2402,
        .itemPocketKey = 0x242C,
        .itemPocketBall = 0x2447,
        .itemPocketPC = 0x2461,
        .rtcFlags = 0x0C80,
        .mysteryGiftUnlocked = 0xB51,
        .rtcDayOffset = 0x0 // TODO
    }
};

const Gen2LocalizationRomOffsets& gen2_getRomOffsets(Gen2GameType gameType, Gen2LocalizationLanguage language)
{
    if(language == Gen2LocalizationLanguage::MAX)
    {
        return g2_dummyROMOffsets;
    }

    switch(gameType)
    {
    case Gen2GameType::GOLD:
        return g2_localizationROMOffsetsG[(uint8_t)language];
    case Gen2GameType::SILVER:
        return g2_localizationROMOffsetsS[(uint8_t)language];
    case Gen2GameType::CRYSTAL:
        return g2_localizationROMOffsetsC[(uint8_t)language];
    default:
        return g2_dummyROMOffsets;
    }
}

const Gen2LocalizationSRAMOffsets& gen2_getSRAMOffsets(Gen2GameType gameType, Gen2LocalizationLanguage language)
{
    if(language == Gen2LocalizationLanguage::MAX)
    {
        return g2_dummySRAMOffsets;
    }

    switch(gameType)
    {
    case Gen2GameType::GOLD:
    case Gen2GameType::SILVER:
        return g2_localizationSRAMOffsetsGS[(uint8_t)language];
    case Gen2GameType::CRYSTAL:
        return g2_localizationSRAMOffsetsC[(uint8_t)language];
    default:
        return g2_dummySRAMOffsets;
    }
}