#ifndef _COMMON_H
#define _COMMON_H

#include <cstdint>

enum class Endianness
{
    LITTLE,
    BIG
};

typedef struct GameboyCartridgeHeader
{
    uint8_t entry[4];
    char title[17];
    char manufacturer_code[5];
    uint8_t CGB_flag;
    char new_licensee_code[2];
    uint8_t SGB_flag;
    uint8_t cartridge_type;
    uint8_t rom_size;
    uint8_t ram_size;
    uint8_t destination_code;
    uint8_t old_licensee_code;
    uint8_t mask_rom_version_number;
    uint8_t header_checksum;
    uint8_t global_checksum[2];
} GameboyCartridgeHeader;

enum class PokeStat
{
    HP,
    ATK,
    DEF,
    SPEED,
    SPECIAL,
    SPECIAL_ATK,
    SPECIAL_DEF
};

enum GrowthRate
{
    GRW_MEDIUM_FAST = 0,
    GRW_SLIGHTLY_FAST = 1,
    GRW_SLIGHTLY_SLOW = 2,
    GRW_MEDIUM_SLOW = 3,
    GRW_FAST = 4,
    GRW_SLOW = 5
};

enum PokedexFlag
{
    POKEDEX_SEEN,
    POKEDEX_OWNED
};

typedef struct TextCodePair
{
	uint8_t code;
	const char* chars;
} TextCodePair;

extern uint16_t monochromeGBColorPalette[4];

/**
 * converts the given gameboy color palette to RGB values.
 * Will always return a uint8_t[12] array.
 * 
 * Note: the next call to this function overwrites the values in the returned pointer
 */
uint8_t* convertGBColorPaletteToRGB24(uint16_t palette[4]);

/**
 * @brief This function gets the specific IV value for the given PokeStat type
 */
uint8_t getStatIV(PokeStat type, const uint8_t iv_data[2]);

/**
 * @brief This function calculates the given PokeStat type using the given parameters.
 * 
 * @type the PokeStat type to calculate
 * @baseVal the base value for the given pokemon
 * @stat_iv the stat iv obtained using getStatIV()
 * 
 */
uint16_t calculatePokeStat(PokeStat type, uint16_t baseVal, uint8_t stat_iv, uint16_t stat_exp, uint8_t level);

uint32_t *getExpTableForGrowthRate(uint8_t growthRate);
uint32_t getExpNeededForLevel(uint8_t level, uint8_t growthRate);
uint8_t getLevelForExp(uint32_t exp, uint8_t growthRate);

void printGameboyCartridgeHeader(const GameboyCartridgeHeader& cartridgeHeader);

#endif