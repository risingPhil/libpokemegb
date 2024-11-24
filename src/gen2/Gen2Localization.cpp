#include "gen2/Gen2Localization.h"
#include "gen2/Gen2Common.h"

static const Gen2LocalizationRomOffsets dummy = {
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
static const Gen2LocalizationRomOffsets g2_localizationOffsetsG[] = {
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
        // names NOT SUPPORTED! (because of difficult character set). We will instead show poke-<number> for each pokémon
        .names = 0x0,
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
static const Gen2LocalizationRomOffsets g2_localizationOffsetsS[] = {
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
        // names NOT SUPPORTED! (because of difficult character set). We will instead show poke-<number> for each pokémon
        .names = 0x0,
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
static const Gen2LocalizationRomOffsets g2_localizationOffsetsC[] = {
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

const Gen2LocalizationRomOffsets& gen2_getRomOffsets(Gen2GameType gameType, Gen2LocalizationLanguage language)
{
    if(language == Gen2LocalizationLanguage::MAX)
    {
        return dummy;
    }

    switch(gameType)
    {
    case Gen2GameType::GOLD:
        return g2_localizationOffsetsG[(uint8_t)language];
    case Gen2GameType::SILVER:
        return g2_localizationOffsetsS[(uint8_t)language];
    case Gen2GameType::CRYSTAL:
        return g2_localizationOffsetsC[(uint8_t)language];
    default:
        return dummy;
    }
}