#include "gen1/Gen1Common.h"
#include "gen1/Gen1GameReader.h"
#include "utils.h"
#include "common.h"

#include <cstring>

#define POKEMON_BLUE_CARTRIDGE_TITLE "POKEMON BLUE"
#define POKEMON_RED_CARTRIDGE_TITLE "POKEMON RED"
#define POKEMON_YELLOW_CARTRIDGE_TITLE "POKEMON YELLOW"


static TextCodePair gen1TextCodes[] = {
	{0x4F, " "},
	{0x57, "#"},
	{0x51, "*"},
	{0x52, "A1"},
	{0x53, "A2"},
	{0x54, "POKé"},
	{0x55, "+"},
	{0x58, "$"},
	{0x5D, "TRAINER"},
	{0x75, "…"},
	{0x7F, " "},
	{0x80, "A"},
	{0x81, "B"},
	{0x82, "C"},
	{0x83, "D"},
	{0x84, "E"},
	{0x85, "F"},
	{0x86, "G"},
	{0x87, "H"},
	{0x88, "I"},
	{0x89, "J"},
	{0x8A, "K"},
	{0x8B, "L"},
	{0x8C, "M"},
	{0x8D, "N"},
	{0x8E, "O"},
	{0x8F, "P"},
	{0x90, "Q"},
	{0x91, "R"},
	{0x92, "S"},
	{0x93, "T"},
	{0x94, "U"},
	{0x95, "V"},
	{0x96, "W"},
	{0x97, "X"},
	{0x98, "Y"},
	{0x99, "Z"},
	{0x9A, "("},
	{0x9B, ")"},
	{0x9C, ","},
	{0x9D, ";"},
	{0x9E, "["},
	{0x9F, "]"},
	{0xA0, "a"},
	{0xA1, "b"},
	{0xA2, "c"},
	{0xA3, "d"},
	{0xA4, "e"},
	{0xA5, "f"},
	{0xA6, "g"},
	{0xA7, "h"},
	{0xA8, "i"},
	{0xA9, "j"},
	{0xAA, "k"},
	{0xAB, "l"},
	{0xAC, "m"},
	{0xAD, "n"},
	{0xAE, "o"},
	{0xAF, "p"},
	{0xB0, "q"},
	{0xB1, "r"},
	{0xB2, "s"},
	{0xB3, "t"},
	{0xB4, "u"},
	{0xB5, "v"},
	{0xB6, "w"},
	{0xB7, "x"},
	{0xB8, "y"},
	{0xB9, "z"},
	{0xBA, "é"},
	{0xBB, "\'d"},
	{0xBC, "\'l"},
	{0xBD, "\'s"},
	{0xBE, "\'t"},
	{0xBF, "\'v"},
	{0xE0, "\'"},
	{0xE1, "PK"},
	{0xE2, "MN"},
	{0xE3, "-"},
	{0xE4, "\'r"},
	{0xE5, "\'m"},
	{0xE6, "?"},
	{0xE7, "!"},
	{0xE8, "."},
	{0xED, "→"},
	{0xEE, "↓"},
	{0xEF, "♂"},
	{0xF0, "¥"},
	{0xF1, "×"},
	{0xF3, "/"},
	{0xF4, ","},
	{0xF5, "♀"},
	{0xF6, "0"},
	{0xF7, "1"},
	{0xF8, "2"},
	{0xF9, "3"},
	{0xFA, "4"},
	{0xFB, "5"},
	{0xFC, "6"},
	{0xFD, "7"},
	{0xFE, "8"},
	{0xFF, "9"}
};

Gen1GameType gen1_determineGameType(const GameboyCartridgeHeader& cartridgeHeader)
{
	Gen1GameType result;
	if (strncmp(cartridgeHeader.title, POKEMON_BLUE_CARTRIDGE_TITLE, sizeof(POKEMON_BLUE_CARTRIDGE_TITLE) - 1) == 0)
    {
        result = Gen1GameType::BLUE;
    }
    else if (strncmp(cartridgeHeader.title, POKEMON_RED_CARTRIDGE_TITLE, sizeof(POKEMON_RED_CARTRIDGE_TITLE) - 1) == 0)
    {
        result = Gen1GameType::RED;
    }
    else if (strncmp(cartridgeHeader.title, POKEMON_YELLOW_CARTRIDGE_TITLE, sizeof(POKEMON_YELLOW_CARTRIDGE_TITLE) - 1) == 0)
    {
        result = Gen1GameType::YELLOW;
    }
	else
	{
		result = Gen1GameType::INVALID;
	}
	return result;
}

void gen1_recalculatePokeStats(Gen1GameReader& reader, Gen1TrainerPokemon& poke)
{
    Gen1PokeStats stats;
    reader.readPokemonStatsForIndex(poke.poke_index, stats);

	poke.level = getLevelForExp(poke.exp, stats.growth_rate);
    poke.max_hp = calculatePokeStat(PokeStat::HP, stats.base_hp, getStatIV(PokeStat::HP, poke.iv_data), poke.hp_effort_value, poke.level);
    poke.atk = calculatePokeStat(PokeStat::ATK, stats.base_attack, getStatIV(PokeStat::ATK, poke.iv_data), poke.atk_effort_value, poke.level);
    poke.def = calculatePokeStat(PokeStat::DEF, stats.base_defense, getStatIV(PokeStat::DEF, poke.iv_data), poke.def_effort_value, poke.level);
    poke.speed = calculatePokeStat(PokeStat::SPEED, stats.base_speed, getStatIV(PokeStat::SPEED, poke.iv_data), poke.speed_effort_value, poke.level);
    poke.special = calculatePokeStat(PokeStat::SPECIAL, stats.base_special, getStatIV(PokeStat::SPECIAL, poke.iv_data), poke.special_effort_value, poke.level);
}

uint16_t gen1_decodePokeText(const uint8_t* inputBuffer, uint16_t inputBufferLength, char* outputBuffer, uint16_t outputBufferLength)
{
	const uint16_t numEntries = sizeof(gen1TextCodes) / sizeof(struct TextCodePair);
	return decodeText(gen1TextCodes, numEntries, inputBuffer, inputBufferLength, outputBuffer, outputBufferLength);
}

uint16_t gen1_encodePokeText(const char* inputBuffer, uint16_t inputBufferLength, uint8_t* outputBuffer, uint16_t outputBufferLength, uint8_t terminator)
{
	const uint16_t numEntries = sizeof(gen1TextCodes) / sizeof(struct TextCodePair);
	return encodeText(gen1TextCodes, numEntries, inputBuffer, inputBufferLength, outputBuffer, outputBufferLength, terminator);
}

Gen1Checksum::Gen1Checksum()
    : checksum_(0)
{
}

void Gen1Checksum::addByte(uint8_t byteVal)
{
    checksum_ += byteVal;
}

uint8_t Gen1Checksum::get() const
{
    return ~checksum_;
}