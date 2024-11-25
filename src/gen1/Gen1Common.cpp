#include "gen1/Gen1Common.h"
#include "gen1/Gen1GameReader.h"
#include "RomReader.h"
#include "utils.h"
#include "common.h"

#include <cstring>
#include <cstdlib>

#define POKEMON_BLUE_CARTRIDGE_TITLE "POKEMON BLUE"
#define POKEMON_RED_CARTRIDGE_TITLE "POKEMON RED"
#define POKEMON_YELLOW_CARTRIDGE_TITLE "POKEMON YEL"

static const uint8_t g1_indexNumberMapFingerprint[] = {
	0x70, 0x73, 0x20, 0x23, 0x15, 0x64, 0x22, 0x50,
	0x02, 0x67, 0x6C, 0x66, 0x58, 0x5E, 0x1D, 0x1F,
};

static const TextCodePair gen1TextCodesMain[] = {
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

static const TextCodePair gen1TextCodesJpn[] = {
	{0x01, "イ゙"},
	{0x02, "ヴ"},
	{0x03, "エ゙"},
	{0x04, "オ゙"},
	{0x05, "ガ"},
	{0x06, "ギ"},
	{0x07, "グ"},
	{0x08, "ゲ"},
	{0x09, "ゴ"},
	{0x0A, "ザ"},
	{0x0B, "ジ"},
	{0x0C, "ズ"},
	{0x0D, "ゼ"},
	{0x0E, "ゾ"},
	{0x0F, "ダ"},
	{0x10, "ヂ"},
	{0x11, "ヅ"},
	{0x12, "デ"},
	{0x13, "ド"},
	{0x14, "ナ゙"},
	{0x15, "ニ゙"},
	{0x16, "ヌ゙"},
	{0x17, "ネ゙"},
	{0x18, "ノ゙"},
	{0x19, "バ"},
	{0x1A, "ビ"},
	{0x1B, "ブ"},
	{0x1C, "ボ"},
	{0x1D, "マ゙"},
	{0x1E, "ミ゙"},
	{0x1F, "ム゙"},
	{0x20, "ィ゙"},
	{0x21, "あ゙"},
	{0x22, "い゙"},
	{0x23, "ゔ"},
	{0x24, "え゙"},
	{0x25, "お゙"},
	{0x26, "が"},
	{0x27, "ぎ"},
	{0x28, "ぐ"},
	{0x29, "げ"},
	{0x2A, "ご"},
	{0x2B, "ざ"},
	{0x2C, "じ"},
	{0x2D, "ず"},
	{0x2E, "ぜ"},
	{0x2F, "ぞ"},
	{0x30, "だ"},
	{0x31, "ぢ"},
	{0x32, "づ"},
	{0x33, "で"},
	{0x34, "ど"},
	{0x35, "な゙"},
	{0x36, "に゙"},
	{0x37, "ぬ゙"},
	{0x38, "ね゙"},
	{0x39, "の゙"},
	{0x3A, "ば"},
	{0x3B, "び"},
	{0x3C, "ぶ"},
	{0x3D, "べ"},
	{0x3E, "ぼ"},
	{0x3F, "ま゙"},
	{0x40, "パ"},
	{0x41, "ピ"},
	{0x42, "プ"},
	{0x43, "ポ"},
	{0x44, "ぱ"},
	{0x45, "ぴ"},
	{0x46, "ぷ"},
	{0x47, "ぺ"},
	{0x48, "ぽ"},
	{0x49, "ま゚"},
	{0x4D, "も゚"},
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
	{0x60, "A"},
	{0x61, "B"},
	{0x62, "C"},
	{0x63, "D"},
	{0x64, "E"},
	{0x65, "F"},
	{0x66, "G"},
	{0x67, "H"},
	{0x68, "I"},
	{0x69, "V"},
	{0x6A, "S"},
	{0x6B, "L"},
	{0x6C, "M"},
	{0x6D, "："},
	{0x6E, "ぃ"},
	{0x6F, "ぅ"},
	{0x70, "「"},
	{0x71, "」"},
	{0x72, "『"},
	{0x73, "』"},
	{0x74, "・"},
	{0x75, "…"},
	{0x76, "ぁ"},
	{0x77, "ぇ"},
	{0x78, "ぉ"},
	{0x80, "ア"},
	{0x81, "イ"},
	{0x82, "ウ"},
	{0x83, "エ"},
	{0x84, "オ"},
	{0x85, "カ"},
	{0x86, "キ"},
	{0x87, "ク"},
	{0x88, "ケ"},
	{0x89, "コ"},
	{0x8A, "サ"},
	{0x8B, "シ"},
	{0x8C, "ス"},
	{0x8D, "セ"},
	{0x8E, "ソ"},
	{0x8F, "タ"},
	{0x90, "チ"},
	{0x91, "ツ"},
	{0x92, "テ"},
	{0x93, "ト"},
	{0x94, "ナ"},
	{0x95, "ニ"},
	{0x96, "ヌ"},
	{0x97, "ネ"},
	{0x98, "ノ"},
	{0x99, "ハ"},
	{0x9A, "ヒ"},
	{0x9B, "フ"},
	{0x9C, "ホ"},
	{0x9D, "マ"},
	{0x9E, "ミ"},
	{0x9F, "ム"},
	{0xA0, "メ"},
	{0xA1, "モ"},
	{0xA2, "ヤ"},
	{0xA3, "ユ"},
	{0xA4, "ヨ"},
	{0xA5, "ラ"},
	{0xA6, "ル"},
	{0xA7, "レ"},
	{0xA8, "ロ"},
	{0xA9, "ワ"},
	{0xAA, "ヲ"},
	{0xAB, "ン"},
	{0xAC, "ッ"},
	{0xAD, "ャ"},
	{0xAE, "ュ"},
	{0xAF, "ョ"},
	{0xB0, "ィ"},
	{0xB1, "あ"},
	{0xB2, "い"},
	{0xB3, "う"},
	{0xB4, "え"},
	{0xB5, "お"},
	{0xB6, "か"},
	{0xB7, "き"},
	{0xB8, "く"},
	{0xB9, "け"},
	{0xBA, "こ"},
	{0xBB, "さ"},
	{0xBC, "し"},
	{0xBD, "す"},
	{0xBE, "せ"},
	{0xBF, "そ"},
	{0xC0, "た"},
	{0xC1, "ち"},
	{0xC2, "つ"},
	{0xC3, "て"},
	{0xC4, "と"},
	{0xC5, "な"},
	{0xC6, "に"},
	{0xC7, "ぬ"},
	{0xC8, "ね"},
	{0xC9, "の"},
	{0xCA, "は"},
	{0xCB, "ひ"},
	{0xCC, "ふ"},
	{0xCD, "へ"},
	{0xCE, "ほ"},
	{0xCF, "ま"},
	{0xD0, "み"},
	{0xD1, "む"},
	{0xD2, "め"},
	{0xD3, "も"},
	{0xD4, "や"},
	{0xD5, "ゆ"},
	{0xD6, "よ"},
	{0xD7, "ら"},
	{0xD8, "リ"},
	{0xD9, "る"},
	{0xDA, "れ"},
	{0xDB, "ろ"},
	{0xDC, "わ"},
	{0xDD, "を"},
	{0xDE, "ん"},
	{0xDF, "っ"},
	{0xE0, "ゃ"},
	{0xE1, "ゅ"},
	{0xE2, "ょ"},
	{0xE3, "ー"},
	{0xE4, "゜"},
	{0xE5, "゛"},
	{0xE6, "?"},
	{0xE7, "!"},
	{0xE8, "。"},
	{0xE9, "ァ"},
	{0xEA, "ゥ"},
	{0xEB, "ェ"},
	{0xEC, "▷"},
	{0xED, "▶"},
	{0xEE, "▼"},
	{0xEF, "♂"},
	{0xF0, "円"},
	{0xF1, "×"},
	{0xF2, "."},
	{0xF3, "/"},
	{0xF4, "ォ"},
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

Gen1LocalizationLanguage gen1_determineGameLanguage(IRomReader& romReader, Gen1GameType gameType)
{
	// The pokemon index-to-pokedex-number map has a unique rom offset in each of the game localizations.
	// It also should have the exact same data in all gen 1 games and all of their localizations.
	// Therefore we can use a fingerprint byte pattern to check these locations to figure out which localization we have.
	uint8_t buffer[sizeof(g1_indexNumberMapFingerprint)];

	for(uint8_t i=0; i < static_cast<uint8_t>(Gen1LocalizationLanguage::MAX); ++i)
	{
		const Gen1LocalizationRomOffsets& romOffsets = gen1_getRomOffsets(gameType, (Gen1LocalizationLanguage)i);
		romReader.seek(romOffsets.numbers);
		romReader.read(buffer, sizeof(g1_indexNumberMapFingerprint));
		if(memcmp(buffer, g1_indexNumberMapFingerprint, sizeof(g1_indexNumberMapFingerprint)) == 0)
		{
			// we found the fingerprint at the "numbers" offset of the current localization!
			// Therefore we know which game language we're dealing with!
			return (Gen1LocalizationLanguage)i;
		}
	}

	return Gen1LocalizationLanguage::MAX;
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

uint16_t gen1_decodePokeText(const uint8_t* inputBuffer, uint16_t inputBufferLength, char* outputBuffer, uint16_t outputBufferLength, Gen1LocalizationLanguage language)
{
	const TextCodePair* textCodes;
	uint16_t numEntries;

	if(language != Gen1LocalizationLanguage::JAPANESE)
	{
		textCodes = gen1TextCodesMain;
		numEntries = sizeof(gen1TextCodesMain) / sizeof(struct TextCodePair);
	}
	else
	{
		textCodes = gen1TextCodesJpn;
		numEntries = sizeof(gen1TextCodesJpn) / sizeof(struct TextCodePair);
	}

	return decodeText(textCodes, numEntries, inputBuffer, inputBufferLength, outputBuffer, outputBufferLength);
}

uint16_t gen1_encodePokeText(const char* inputBuffer, uint16_t inputBufferLength, uint8_t* outputBuffer, uint16_t outputBufferLength, uint8_t terminator, Gen1LocalizationLanguage language)
{
	const TextCodePair* textCodes;
	uint16_t numEntries;

	if(language != Gen1LocalizationLanguage::JAPANESE)
	{
		textCodes = gen1TextCodesMain;
		numEntries = sizeof(gen1TextCodesMain) / sizeof(struct TextCodePair);
	}
	else
	{
		textCodes = gen1TextCodesJpn;
		numEntries = sizeof(gen1TextCodesJpn) / sizeof(struct TextCodePair);
	}

	return encodeText(textCodes, numEntries, inputBuffer, inputBufferLength, outputBuffer, outputBufferLength, terminator);
}

void gen1_prepareDistributionPokemon(Gen1GameReader& gameReader, const Gen1DistributionPokemon& distributionPoke, Gen1TrainerPokemon& poke, const char*& originalTrainerName)
{
    if(distributionPoke.setPlayerAsOriginalTrainer)
    {
        originalTrainerName = gameReader.getTrainerName();
        poke.original_trainer_ID = gameReader.getTrainerID();
    }
    else
    {
        originalTrainerName = distributionPoke.originalTrainer;

        if(distributionPoke.regenerateTrainerID)
        {
            if(distributionPoke.originalTrainerID)
            {
                // limit set, apply it
                poke.original_trainer_ID = (uint16_t)(rand() % distributionPoke.originalTrainerID);
            }
            else
            {
                // no limit. The max is the max of the uint16_t type
                poke.original_trainer_ID =(uint16_t)(rand() % UINT16_MAX);
            }
        }
        else
        {
            poke.original_trainer_ID = distributionPoke.originalTrainerID;
        }
    }

    poke.iv_data[0] = distributionPoke.iv_data[0];
    poke.iv_data[1] = distributionPoke.iv_data[1];
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