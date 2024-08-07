#include "common.h"
#include <cmath>
#include <cstdio>

uint16_t monochromeGBColorPalette[4] = {
    0x7FFF,
    0x56B6,
    0x2D6B,
    0x421
};

uint32_t mediumFastExpTable[] = {
    0,
    8,
    27,
    64,
    125,
    216,
    343,
    512,
    729,
    1000,
    1331,
    1728,
    2197,
    2744,
    3375,
    4096,
    4913,
    5832,
    6859,
    8000,
    9261,
    10648,
    12167,
    13824,
    15625,
    17576,
    19683,
    21952,
    24389,
    27000,
    29791,
    32768,
    35937,
    39304,
    42875,
    46656,
    50653,
    54872,
    59319,
    64000,
    68921,
    74088,
    79507,
    85184,
    91125,
    97336,
    103823,
    110592,
    117649,
    125000,
    132651,
    140608,
    148877,
    157464,
    166375,
    175616,
    185193,
    195112,
    205379,
    216000,
    226981,
    238328,
    250047,
    262144,
    274625,
    287496,
    300763,
    314432,
    328509,
    343000,
    357911,
    373248,
    389017,
    405224,
    421875,
    438976,
    456533,
    474552,
    493039,
    512000,
    531441,
    551368,
    571787,
    592704,
    614125,
    636056,
    658503,
    681472,
    704969,
    729000,
    753571,
    778688,
    804357,
    830584,
    857375,
    884736,
    912673,
    941192,
    970299,
    1000000,
};

uint32_t mediumSlowExpTable[] = {
    0,
    9,
    57,
    96,
    135,
    179,
    236,
    314,
    419,
    560,
    742,
    973,
    1261,
    1612,
    2035,
    2535,
    3120,
    3798,
    4575,
    5460,
    6458,
    7577,
    8825,
    10208,
    11735,
    13411,
    15244,
    17242,
    19411,
    21760,
    24294,
    27021,
    29949,
    33084,
    36435,
    40007,
    43808,
    47846,
    52127,
    56660,
    61450,
    66505,
    71833,
    77440,
    83335,
    89523,
    96012,
    102810,
    109923,
    117360,
    125126,
    133229,
    141677,
    150476,
    159635,
    169159,
    179056,
    189334,
    199999,
    211060,
    222522,
    234393,
    246681,
    259392,
    272535,
    286115,
    300140,
    314618,
    329555,
    344960,
    360838,
    377197,
    394045,
    411388,
    429235,
    447591,
    466464,
    485862,
    505791,
    526260,
    547274,
    568841,
    590969,
    613664,
    636935,
    660787,
    685228,
    710266,
    735907,
    762160,
    789030,
    816525,
    844653,
    873420,
    902835,
    932903,
    963632,
    995030,
    1027103,
    1059860,
};

uint32_t fastExpTable[] = {
    0,
    6,
    21,
    51,
    100,
    172,
    274,
    409,
    583,
    800,
    1064,
    1382,
    1757,
    2195,
    2700,
    3276,
    3930,
    4665,
    5487,
    6400,
    7408,
    8518,
    9733,
    11059,
    12500,
    14060,
    15746,
    17561,
    19511,
    21600,
    23832,
    26214,
    28749,
    31443,
    34300,
    37324,
    40522,
    43897,
    47455,
    51200,
    55136,
    59270,
    63605,
    68147,
    72900,
    77868,
    83058,
    88473,
    94119,
    100000,
    106120,
    112486,
    119101,
    125971,
    133100,
    140492,
    148154,
    156089,
    164303,
    172800,
    181584,
    190662,
    200037,
    209715,
    219700,
    229996,
    240610,
    251545,
    262807,
    274400,
    286328,
    298598,
    311213,
    324179,
    337500,
    351180,
    365226,
    379641,
    394431,
    409600,
    425152,
    441094,
    457429,
    474163,
    491300,
    508844,
    526802,
    545177,
    563975,
    583200,
    602856,
    622950,
    643485,
    664467,
    685900,
    707788,
    730138,
    752953,
    776239,
    800000,
};

uint32_t slowExpTable[] = {
    0,
    10,
    33,
    80,
    156,
    270,
    428,
    640,
    911,
    1250,
    1663,
    2160,
    2746,
    3430,
    4218,
    5120,
    6141,
    7290,
    8573,
    10000,
    11576,
    13310,
    15208,
    17280,
    19531,
    21970,
    24603,
    27440,
    30486,
    33750,
    37238,
    40960,
    44921,
    49130,
    53593,
    58320,
    63316,
    68590,
    74148,
    80000,
    86151,
    92610,
    99383,
    106480,
    113906,
    121670,
    129778,
    138240,
    147061,
    156250,
    165813,
    175760,
    186096,
    196830,
    207968,
    219520,
    231491,
    243890,
    256723,
    270000,
    283726,
    297910,
    312558,
    327680,
    343281,
    359370,
    375953,
    393040,
    410636,
    428750,
    447388,
    466560,
    486271,
    506530,
    527343,
    548720,
    570666,
    593190,
    616298,
    640000,
    664301,
    689210,
    714733,
    740880,
    767656,
    795070,
    823128,
    851840,
    881211,
    911250,
    941963,
    973360,
    1005446,
    1038230,
    1071718,
    1105920,
    1140841,
    1176490,
    1212873,
    1250000,
};

uint8_t* convertGBColorPaletteToRGB24(uint16_t palette[4])
{
    static uint8_t result[12];
    uint16_t work;

    uint8_t *cur = result;
    for(uint8_t i=0; i < 4; ++i)
    {
        // format is 15 bit BGR (https://www.huderlem.com/demos/gameboypalette.html)
        const uint8_t b5 = (palette[i] >> 10) & 0x1F;
        const uint8_t g5 = (palette[i] >> 5) & 0x1F;
        const uint8_t r5 = palette[i] & 0x1F;
        
        work = (static_cast<uint32_t>(r5) * 255) / 31;
        (*cur) = (uint8_t)work;
        ++cur;
        work = (static_cast<uint32_t>(g5) * 255) / 31;
        (*cur) = (uint8_t)work;
        ++cur;
        work = (static_cast<uint32_t>(b5) * 255) / 31;
        (*cur) = (uint8_t)work;
        ++cur;
    }

    return result;
}

uint16_t* convertGBColorPaletteToRGBA16(uint16_t palette[4])
{
    static uint16_t result[4];

    uint16_t *cur = result;
    for(uint8_t i=0; i < 4; ++i)
    {
        // format is 15 bit BGR (https://www.huderlem.com/demos/gameboypalette.html)
        const uint16_t b5 = (palette[i] >> 10) & 0x1F;
        const uint16_t g5 = (palette[i] >> 5) & 0x1F;
        const uint16_t r5 = palette[i] & 0x1F;

        (*cur) = (r5 << 11) | (g5 << 6) | (b5 << 1) | 0x1;
        ++cur;
    }
    return result;
}

uint8_t getStatIV(PokeStat type, const uint8_t iv_data[2])
{
    // https://bulbapedia.bulbagarden.net/wiki/Individual_values
    switch(type)
    {
        case PokeStat::ATK:
            return (iv_data[0] >> 4);
        case PokeStat::DEF:
            return (iv_data[0] & 0xF);
        case PokeStat::SPEED:
            return (iv_data[1] >> 4);
        case PokeStat::SPECIAL:
        case PokeStat::SPECIAL_ATK:
        case PokeStat::SPECIAL_DEF:
            return (iv_data[1] & 0xF);
        case PokeStat::HP:
            // HP is based on the least significant bit of the other 4 stats
            return (((getStatIV(PokeStat::ATK, iv_data) & 0x1) << 3) |
                ((getStatIV(PokeStat::DEF, iv_data) & 0x1) << 2) |
                ((getStatIV(PokeStat::SPEED, iv_data) & 0x1) << 1) |
                ((getStatIV(PokeStat::SPECIAL, iv_data) & 0x1)));
        default:
            return 0;
    }
}

// https://bulbapedia.bulbagarden.net/wiki/Stat
// Note that there may be a difference between the stats in the Gen1TrainerPokemon stats field and the ones calculated here
// This is because stats are being recalculated when withdrawing the pokemon from the pc.
// look into "The box trick"
// If you store your pokemon in an ingame pc and take it out again, you'll see that now the stats match with the ones calculated here.
uint16_t calculatePokeStat(PokeStat type, uint16_t baseVal, uint8_t stat_iv, uint16_t stat_exp, uint8_t level)
{
    uint16_t result;
    // Terminology: For Gen 1 & 2, IVs are also referred to as Determinant Values (DVs)

    // first part: The fraction only
    // this part is common for all PokeStats including HP
    const uint16_t ivPart = (baseVal + stat_iv) * 2;
    const uint16_t statExpPart = static_cast<uint16_t>(ceil(sqrt(stat_exp)) / 4);

    result = ((ivPart + statExpPart) * level) / 100;

    if(type == PokeStat::HP)
    {
        // now add the second part of the formula, which is specific for HP
        result += level + 10;
    }
    else
    {
        // now add the second part of the formula specific to all stats that aren't HP.
        result += 5;
    }
    return static_cast<uint16_t>(result);
}

uint32_t *getExpTableForGrowthRate(uint8_t growthRate)
{
    uint32_t *expTable;
    switch (growthRate)
    {
    case GRW_FAST:
        expTable = fastExpTable;
        break;
    case GRW_MEDIUM_FAST:
        expTable = mediumFastExpTable;
        break;
    case GRW_MEDIUM_SLOW:
        expTable = mediumSlowExpTable;
        break;
    case GRW_SLOW:
        expTable = slowExpTable;
        break;
    default:
        expTable = 0;
        break;
    }
    return expTable;
}

uint32_t getExpNeededForLevel(uint8_t level, uint8_t growthRate)
{
    uint32_t *expTable;

    if(level < 1 || level > 100)
    {
        return 0;
    }
    expTable = getExpTableForGrowthRate(growthRate);

    return (expTable) ? expTable[level - 1] : 0;
}

uint8_t getLevelForExp(uint32_t exp, uint8_t growthRate)
{
    const uint32_t *expTable = getExpTableForGrowthRate(growthRate);
    if (!expTable)
    {
        return 1;
    }

    for (uint8_t i = 1; i < 100; ++i)
    {
        // to check which level we have, we need to find the first entry that is more than the exp we have
        // when we find that, i would refer to the next level, not the current one.
        // However, since we're doing an array index, i is zero-based, whereas the pkmn level is 1 -based.
        // so it turns out that we can just return i here instead of subtracting by 1
        if (exp < expTable[i])
        {
            return i;
        }
    }
    return 100;
}

void printGameboyCartridgeHeader(const GameboyCartridgeHeader& h)
{
    printf("Entry:\t%hhx %hhx %hhx %hhx\n", h.entry[0], h.entry[1], h.entry[2], h.entry[3]);
    printf("Title:\t%s\n", h.title);
    printf("Manufacturer code:\t%s\n", h.manufacturer_code);
    printf("CGB flag:\t%hhx\n", h.CGB_flag);
    printf("New Licensee code:\t%hhx %hhx\n", h.new_licensee_code[0], h.new_licensee_code[1]);
    printf("SGB flag:\t%hhx\n", h.SGB_flag);
    printf("Cartridge type:\t%hhx\n", h.cartridge_type);
    printf("Rom size:\t%hhx\n", h.rom_size);
    printf("Ram size:\t%hhx\n", h.ram_size);
    printf("Destination code:\t%hhx\n", h.destination_code);
    printf("Old Licensee code:\t%hhx\n", h.old_licensee_code);
    printf("Mask Rom version:\t%hhx\n", h.mask_rom_version_number);
    printf("Header checksum:\t%hhx\n", h.header_checksum);
    printf("Global checksum: \t%hhx %hhx\n", h.global_checksum[0], h.global_checksum[1]);
}