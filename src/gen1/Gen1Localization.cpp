#include "gen1/Gen1Localization.h"

static const Gen1LocalizationRomOffsets g1_localizationROMOffsetsRB[] = {
    // English
    {
        .stats = 0x383DE,
        .statsMew = 0x425B,
        .numbers = 0x41024,
        .names = 0x1c21e,
        .iconTypes = 0x7190D,
        .icons = 0x717C0,
        .paletteIndices = 0x725C9,
        .palettes = 0x72660
    },
    // French
    {
        .stats = 0x383DE,
        .statsMew = 0x425B,
        .numbers = 0x40FAA,
        .names = 0x1C21E,
        .iconTypes = 0x718DE,
        .icons = 0x71791,
        .paletteIndices = 0x7259A,
        .palettes = 0x72631
    },
    // Spanish
    {
        .stats = 0x383DE,
        .statsMew = 0x425B,
        .numbers = 0x40FB4,
        .names = 0x1C21E,
        .iconTypes = 0x718FD,
        .icons = 0x717B0,
        .paletteIndices = 0x725B9,
        .palettes = 0x72650
    },
    // German
    {
        .stats = 0x383DE,
        .statsMew = 0x425B,
        .numbers = 0x40F96,
        .names = 0x1C21E,
        .iconTypes = 0x718E7,
        .icons = 0x7179A,
        .paletteIndices = 0x725A3,
        .palettes = 0x7263A
    },
    // Italian
    {
        .stats = 0x383DE,
        .statsMew = 0x425B,
        .numbers = 0x40FB6,
        .names = 0x1C21E,
        .iconTypes = 0x7194D,
        .icons = 0x71800,
        .paletteIndices = 0x72609,
        .palettes = 0x726A0
    },
    // Japanese
    {
        .stats = 0x38000,
        .statsMew = 0x4200,
        .numbers = 0x4279A,
        .names = 0x39068,
        .iconTypes = 0x71DD1,
        .icons = 0x71C84,
        .paletteIndices = 0x72A1F,
        .palettes = 0x72AB6
    }
};

static const Gen1LocalizationRomOffsets g1_localizationROMOffsetsY[] = {
    // English
    {
        .stats = 0x383DE,
        .statsMew = 0x0, // IRRELEVANT because mew is included in the stats list for Yellow
        .numbers = 0x410B1,
        .names = 0xE8000,
        .iconTypes = 0x719BA,
        .icons = 0x7184D,
        .paletteIndices = 0x72922,
        .palettes = 0x72AF9
    },
    // French
    {
        .stats = 0x383DE,
        .statsMew = 0x0, // IRRELEVANT because mew is included in the stats list for Yellow
        .numbers = 0x41036,
        .names = 0xE8000,
        .iconTypes = 0x7198B,
        .icons = 0x7181E,
        .paletteIndices = 0x728F3,
        .palettes = 0x72ACA
    },
    // Spanish
    {
        .stats = 0x383DE,
        .statsMew = 0x0, // IRRELEVANT because mew is included in the stats list for Yellow
        .numbers = 0x41041,
        .names = 0xE8000,
        .iconTypes = 0x719AA,
        .icons = 0x7183D,
        .paletteIndices = 0x72912,
        .palettes = 0x72AE9
    },
    // German
    {
        .stats = 0x383DE,
        .statsMew = 0x0, // IRRELEVANT because mew is included in the stats list for Yellow
        .numbers = 0x41023,
        .names = 0xE8000,
        .iconTypes = 0x71999,
        .icons = 0x7182c,
        .paletteIndices = 0x72901,
        .palettes = 0x72AD8
    },
    // Italian
    {
        .stats = 0x383DE,
        .statsMew = 0x0, // IRRELEVANT because mew is included in the stats list for Yellow
        .numbers = 0x41043,
        .names = 0xE8000,
        .iconTypes = 0x719FA,
        .icons = 0x7188D,
        .paletteIndices = 0x72962,
        .palettes = 0x72B39
    },
    // Japanese
    {
        .stats = 0x383DE,
        .statsMew = 0x0, // IRRELEVANT because mew is included in the stats list for Yellow
        .numbers = 0x4282D,
        .names = 0x39462,
        .iconTypes = 0x71911,
        .icons = 0x717A4,
        .paletteIndices = 0x72650,
        .palettes = 0x726E7
    }
};

static const Gen1LocalizationSRAMOffsets g1_sRAMOffsetsInternational = {
    .trainerID = 0x2605,
    .dexSeen = 0x25B6,
    .dexOwned = 0x25A3,
    .rivalName = 0x25F6,
    .party = 0x2F2C,
    .currentBoxIndex = 0x284C,
    .checksum = 0x3523
};

static const Gen1LocalizationSRAMOffsets g1_sRAMOffsetsJapan = {
    .trainerID = 0x25FB,
    .dexSeen = 0x25B1,
    .dexOwned = 0x259E,
    .rivalName = 0x25F1,
    .party = 0x2ED5,
    .currentBoxIndex = 0x2842,
    .checksum = 0x3594
};

const Gen1LocalizationRomOffsets& gen1_getRomOffsets(Gen1GameType gameType, Gen1LocalizationLanguage language)
{
    if(gameType == Gen1GameType::YELLOW)
    {
        return g1_localizationROMOffsetsY[(uint8_t)language];
    }
    else
    {
        return g1_localizationROMOffsetsRB[(uint8_t)language];
    }
}

const Gen1LocalizationSRAMOffsets& gen1_getSRAMOffsets(Gen1LocalizationLanguage language)
{
    return (language != Gen1LocalizationLanguage::JAPANESE) ? g1_sRAMOffsetsInternational : g1_sRAMOffsetsJapan;
}