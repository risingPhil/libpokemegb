#include "gen2/Gen2Common.h"
#include "gen2/Gen2GameReader.h"
#include "SaveManager.h"
#include "RomReader.h"
#include "utils.h"
#include "common.h"

#include <cstdlib>
#include <cstring>

#define POKEMON_GOLD_CARTRIDGE_TITLE "POKEMON_GLD"
#define POKEMON_SILVER_CARTRIDGE_TITLE "POKEMON_SLV"
#define POKEMON_CRYSTAL_CARTRIDGE_TITLE "PM_CRYSTAL"

uint16_t gen2_iconColorPalette[4] = {
	0x7FFF,
	0x2A5E,
	0x10FE,
	0x0
};

static uint8_t gen2_statsFingerPrint[] = {
	0x01, 0x2D, 0x31, 0x31, 0x2D, 0x41, 0x41, 0x16,
	0x03, 0x2D, 0x40, 0x00, 0x00, 0x1F, 0x64, 0x14
};

static TextCodePair gen2TextCodesMain[] = {
	{0x56, "……"},
	{0x5D, "TRAINER"},
	{0x60, "█"},
	{0x61, "▲"},
	{0x74, "・"},
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
	{0x9C, ":"},
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
	{0xBA, "à"},
	{0xBB, "è"},
	{0xBD, "ù"},
	{0xBE, "ß"},
	{0xBF, "ç"},
	{0xC0, "Ä"},
	{0xC1, "Ö"},
	{0xC2, "Ü"},
	{0xC3, "ä"},
	{0xC4, "ö"},
	{0xC5, "ü"},
	{0xC6, "ë"},
	{0xC7, "ï"},
	{0xC8, "â"},
	{0xC9, "ô"},
	{0xCA, "û"},
	{0xCB, "ê"},
	{0xCC, "î"},
	{0xD0, "'d"},
	{0xD1, "'l"},
	{0xD2, "'m"},
	{0xD3, "'r"},
	{0xD4, "'s"},
	{0xD5, "'t"},
	{0xD6, "'v"},
	{0xE0, "\'"},
	{0xE1, "PK"},
	{0xE2, "MN"},
	{0xE3, "-"},
	{0xE6, "?"},
	{0xE7, "!"},
	{0xE8, "."},
	{0xE9, "&"},
	{0xEA, "é"},
	{0xEB, "🡆"},
	{0xEC, "▷"},
	{0xED, "▶"},
	{0xEE, "▼"},
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
	{0xFF, "9"},
	{0x51, " "}
};

static const TextCodePair gen2TextCodesJpn[] = {
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
	{0x61, "▲"},
	{0x62, "?"},
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

Gen2ItemList::Gen2ItemList(ISaveManager &saveManager, Gen2ItemListType type, bool isCrystal)
	: saveManager_(saveManager)
	, type_(type)
	, isCrystal_(isCrystal)
{
}

uint8_t Gen2ItemList::getCount()
{
	uint8_t result;
	if (!seekToBasePos())
	{
		return 0;
	}

	if (!saveManager_.readByte(result))
	{
		result = 0;
	}
	return result;
}

uint8_t Gen2ItemList::getCapacity()
{
	return (type_ == Gen2ItemListType::GEN2_ITEMLISTTYPE_KEYITEMPOCKET) ? 26 : 20;
}

bool Gen2ItemList::getEntry(uint8_t index, uint8_t &outItemId, uint8_t &outCount)
{
	if (!seekToBasePos())
	{
		return false;
	}

	// It looks like the key item pocket doesn't have a count value.
	// During my test if I try to write a GS ball with count 1 to crystal and store the count value, I get a random master ball showing up (which has itemId 1)
	// before the gs ball in the list.
	const uint32_t advanceAmount = (type_ == Gen2ItemListType::GEN2_ITEMLISTTYPE_KEYITEMPOCKET) ? 1 + index : 1 + (index * 2);
	if (!saveManager_.advance(advanceAmount))
	{
		return false;
	}

	return getNextEntry(outItemId, outCount);
}

bool Gen2ItemList::getNextEntry(uint8_t &outItemId, uint8_t &outCount)
{
	const uint8_t peekByte = saveManager_.peek();

	// 0xFF is the terminator byte
	if(peekByte == 0xFF)
	{
		return false;
	}

	// 2 things here:
	// - The bulbapedia article: https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_II)#Item_lists
	//   gets the order of count vs itemId wrong. At least during my tests with a Pokémon crystal AND gold save
	// - The key item pocket doesn't appear to store a count value at all.
	//   see my other comment at ::getEntry() for more info
	saveManager_.readByte(outItemId);
	if(type_ != Gen2ItemListType::GEN2_ITEMLISTTYPE_KEYITEMPOCKET)
	{
		saveManager_.readByte(outCount);
	}
	else
	{
		outCount = 1;
	}

	return true;
}

bool Gen2ItemList::add(uint8_t itemId, uint8_t itemCount)
{
	uint8_t curItemId;
	uint8_t curItemCount;
	const uint8_t numItems = getCount();
	
	// search for an existing item entry
	// if found, we'll increase the itemcount
	// if not found, the internal position of the savemanager will have conveniently moved
	// to the position we'd actually need to write the new entry to
	while(getNextEntry(curItemId, curItemCount))
	{
		if(curItemId == itemId)
		{
			// the key item pocket doesn't appear to have a count field per entry.
			// For more info see the related comment @getEntry()
			if(type_ != Gen2ItemListType::GEN2_ITEMLISTTYPE_KEYITEMPOCKET)
			{
				uint8_t newCount = curItemCount + itemCount;
				if(newCount > 99)
				{
					newCount = 99;
				}
				saveManager_.rewind(1); // count is the second field of each entry (yes, Bulbapedia appears to get it wrong here)
				saveManager_.writeByte(newCount);
				saveManager_.advance();
			}
			return true;
		}
	}

	// no existing entry found in the itemlist
	// first check if there's any room left for the new item
	if(numItems >= getCapacity())
	{
		return false;
	}
	
	// Bulbapedia gets the order wrong here. itemId should go first, then itemCount
	// at least this is the case during my tests with a Pokémon Crystal AND Gold save
	saveManager_.writeByte(itemId);
	// in the while loop, getNextEntry has returned false on the last iteration, so that means
	// we've arrived at the terminator of the list. This is the exact position at which we need to write our new entry.
	// convenient, isn't it?
	// so let's write the new entry...
	if(type_ != Gen2ItemListType::GEN2_ITEMLISTTYPE_KEYITEMPOCKET)
	{
		// the key item pocket doesn't appear to have a count field per entry.
		// For more info see the related comment @getEntry()
		saveManager_.writeByte(itemCount);
	}
	// now write the new terminator
	saveManager_.writeByte(0xFF);

	// the only thing left is to increase the list counter
	seekToBasePos();
	saveManager_.writeByte(numItems + 1);
	
	return true;
}

bool Gen2ItemList::remove(uint8_t itemId)
{
	uint8_t curItemId;
	uint8_t curItemCount;

	const uint8_t numItems = getCount();

	// search for an existing item entry
	// if found, we'll increase the itemcount
	// if not found, the internal position of the savemanager will have conveniently moved
	// to the position we'd actually need to write the new entry to
	while(getNextEntry(curItemId, curItemCount))
	{
		if(curItemId == itemId)
		{
			break;
		}
	}

	if(curItemId != itemId)
	{
		return false;
	}

	while(getNextEntry(curItemId, curItemCount))
	{
		// okay, we need to rewind to the item we want to replace first. right now we're just past the item we just read to replace it with
		if(type_ != Gen2ItemListType::GEN2_ITEMLISTTYPE_KEYITEMPOCKET)
		{
			saveManager_.rewind(4);
		}
		else
		{
			// the key item pocket doesn't appear to have a count field per entry.
			saveManager_.rewind(2);
		}
		saveManager_.writeByte(curItemId);
		if(type_ != Gen2ItemListType::GEN2_ITEMLISTTYPE_KEYITEMPOCKET)
		{
			// the key item pocket doesn't appear to have a count field per entry.
			// For more info see the related comment @getEntry()
			saveManager_.writeByte(curItemCount);
		}

		// now we need to advance 1 item to make sure we're not reading the same entry again on the next loop
		if(type_ != Gen2ItemListType::GEN2_ITEMLISTTYPE_KEYITEMPOCKET)
		{
			saveManager_.advance(2);
		}
		else
		{
			// the key item pocket doesn't appear to have a count field per entry.
			saveManager_.advance(1);
		}
	}

	if(type_ != Gen2ItemListType::GEN2_ITEMLISTTYPE_KEYITEMPOCKET)
	{
		saveManager_.rewind(2);
	}
	else
	{
		saveManager_.rewind(1);
	}

	// now write the new terminator
	saveManager_.writeByte(0xFF);

	// the only thing left is to increase the list counter
	seekToBasePos();
	saveManager_.writeByte(numItems - 1);

	return true;
}

bool Gen2ItemList::seekToBasePos()
{
	uint32_t offset;
	switch (type_)
	{
	case GEN2_ITEMLISTTYPE_ITEMPOCKET:
		offset = (isCrystal_) ? 0x2420 : 0x241F;
		break;
	case GEN2_ITEMLISTTYPE_KEYITEMPOCKET:
		offset = (isCrystal_) ? 0x244A : 0x2449;
		break;
	case GEN2_ITEMLISTTYPE_BALLPOCKET:
		offset = (isCrystal_) ? 0x2465 : 0x2464;
		break;
	case GEN2_ITEMLISTTYPE_PC:
		offset = (isCrystal_) ? 0x247F : 0x247E;
		break;
	default:
		return false;
	}

	return saveManager_.seek(offset);
}

Gen2Checksum::Gen2Checksum()
	: checksum_(0)
{
}

void Gen2Checksum::addByte(uint8_t byte)
{
	checksum_ += byte;
}

uint16_t Gen2Checksum::get() const
{
	return checksum_;
}

Gen2GameType gen2_determineGameType(const GameboyCartridgeHeader& cartridgeHeader)
{
	Gen2GameType result;
	if (strncmp(cartridgeHeader.title, POKEMON_GOLD_CARTRIDGE_TITLE, sizeof(POKEMON_GOLD_CARTRIDGE_TITLE) - 1) == 0)
    {
        result = Gen2GameType::GOLD;
    }
    else if (strncmp(cartridgeHeader.title, POKEMON_SILVER_CARTRIDGE_TITLE, sizeof(POKEMON_SILVER_CARTRIDGE_TITLE) - 1) == 0)
    {
        result = Gen2GameType::SILVER;
    }
    else if (strncmp(cartridgeHeader.title, POKEMON_CRYSTAL_CARTRIDGE_TITLE, sizeof(POKEMON_CRYSTAL_CARTRIDGE_TITLE) - 1) == 0)
    {
        result = Gen2GameType::CRYSTAL;
    }
	else
	{
		result = Gen2GameType::INVALID;
	}
	return result;
}

Gen2LocalizationLanguage gen2_determineGameLanguage(IRomReader& romReader, Gen2GameType gameType)
{
	// The pokemon index-to-pokedex-number map has a unique rom offset in each of the game localizations.
	// It also should have the exact same data in all gen 1 games and all of their localizations.
	// Therefore we can use a fingerprint byte pattern to check these locations to figure out which localization we have.
	uint8_t buffer[sizeof(gen2_statsFingerPrint)];

	for(uint8_t i=0; i < static_cast<uint8_t>(Gen2LocalizationLanguage::MAX); ++i)
	{
		const Gen2LocalizationRomOffsets& romOffsetList = gen2_getRomOffsets(gameType, (Gen2LocalizationLanguage)i);
		romReader.seek(romOffsetList.stats);
		romReader.read(buffer, sizeof(gen2_statsFingerPrint));
		if(memcmp(buffer, gen2_statsFingerPrint, sizeof(gen2_statsFingerPrint)) == 0)
		{
			// stats fingerprint match at the current languages' stats offset.

			// For pokémon gold & pokémon silver, however, the spanish and italian localizations use the exact same stats rom offset.
			// The only difference we can really check for -given the rom offsets we possess right now- is the names offset.
			// that's the only one that is unique for these
			// both of these games use the original English names though. So it's just a matter of the offsets being different
			if(gameType != Gen2GameType::CRYSTAL && (i == (uint8_t)Gen2LocalizationLanguage::ITALIAN || i == (uint8_t)Gen2LocalizationLanguage::SPANISH))
			{
				uint8_t nameBuffer[0xB];
				const char* firstPokemon = "BULBASAUR";
				// both languages are luckily also using our main character set. So we don't have to worry about that
				const uint16_t nameLength = gen2_encodePokeText(firstPokemon, strlen(firstPokemon), nameBuffer, sizeof(nameBuffer), 0x50);

				romReader.seek(romOffsetList.names);
				romReader.read(buffer, nameLength);

				const int ret = memcmp(buffer, nameBuffer, nameLength - 1);
				if(ret != 0)
				{
					// no match at the current offset. Skip to the next language
					continue;
				}
				// if we get here, we have a full match. So we know which language we're dealing with now!
				// Therefore we will end up at the return statement below
			}
			return (Gen2LocalizationLanguage)i;
		}
	}

	return Gen2LocalizationLanguage::MAX;
}

void gen2_recalculatePokeStats(Gen2GameReader &reader, Gen2TrainerPokemon &poke)
{
	Gen2PokeStats stats;
	reader.readPokemonStatsForIndex(poke.poke_index, stats);

	poke.max_hp = calculatePokeStat(PokeStat::HP, stats.base_hp, getStatIV(PokeStat::HP, poke.iv_data), poke.hp_effort_value, poke.level);
	poke.atk = calculatePokeStat(PokeStat::ATK, stats.base_attack, getStatIV(PokeStat::ATK, poke.iv_data), poke.atk_effort_value, poke.level);
	poke.def = calculatePokeStat(PokeStat::DEF, stats.base_defense, getStatIV(PokeStat::DEF, poke.iv_data), poke.def_effort_value, poke.level);
	poke.speed = calculatePokeStat(PokeStat::SPEED, stats.base_speed, getStatIV(PokeStat::SPEED, poke.iv_data), poke.speed_effort_value, poke.level);
	poke.special_atk = calculatePokeStat(PokeStat::SPECIAL_ATK, stats.base_special_attack, getStatIV(PokeStat::SPECIAL_ATK, poke.iv_data), poke.special_effort_value, poke.level);
	poke.special_def = calculatePokeStat(PokeStat::SPECIAL_DEF, stats.base_special_defense, getStatIV(PokeStat::SPECIAL_DEF, poke.iv_data), poke.special_effort_value, poke.level);
}

uint16_t gen2_decodePokeText(const uint8_t *inputBuffer, uint16_t inputBufferLength, char *outputBuffer, uint16_t outputBufferLength, Gen2LocalizationLanguage language)
{
	const TextCodePair* textCodes;
	uint16_t numEntries;

	switch(language)
	{
	case Gen2LocalizationLanguage::JAPANESE:
		textCodes = gen2TextCodesJpn;
		numEntries = sizeof(gen2TextCodesJpn) / sizeof(struct TextCodePair);
		break;
	default:
		textCodes = gen2TextCodesMain;
		numEntries = sizeof(gen2TextCodesMain) / sizeof(struct TextCodePair);
	}

	return decodeText(textCodes, numEntries, inputBuffer, inputBufferLength, outputBuffer, outputBufferLength);
}

uint16_t gen2_encodePokeText(const char *inputBuffer, uint16_t inputBufferLength, uint8_t *outputBuffer, uint16_t outputBufferLength, uint8_t terminator, Gen2LocalizationLanguage language)
{
	const TextCodePair* textCodes;
	uint16_t numEntries;

	switch(language)
	{
	case Gen2LocalizationLanguage::JAPANESE:
		textCodes = gen2TextCodesJpn;
		numEntries = sizeof(gen2TextCodesJpn) / sizeof(struct TextCodePair);
		break;
	default:
		textCodes = gen2TextCodesMain;
		numEntries = sizeof(gen2TextCodesMain) / sizeof(struct TextCodePair);
		break;
	}

	return encodeText(textCodes, numEntries, inputBuffer, inputBufferLength, outputBuffer, outputBufferLength, terminator);
}

bool gen2_isPokemonShiny(Gen2TrainerPokemon& poke)
{
    // based on https://bulbapedia.bulbagarden.net/wiki/Shiny_Pok%C3%A9mon#Determining_Shininess
    const PokeStat statsToCheckFor10[] = {PokeStat::SPEED, PokeStat::DEF, PokeStat::SPECIAL};

    for(uint8_t i=0; i < (sizeof(statsToCheckFor10) / sizeof(statsToCheckFor10[0])); ++i)
    {
        if(getStatIV(statsToCheckFor10[i], poke.iv_data) != 10)
        {
            return false;
        }
    }

    switch(getStatIV(PokeStat::ATK, poke.iv_data))
    {
        case 2:
        case 3:
        case 6:
        case 7:
        case 10:
        case 11:
        case 14:
        case 15:
            return true;
        default:
            break;
    }
    return false;
}

void gen2_makePokemonShiny(Gen2TrainerPokemon& poke)
{
	const uint8_t validAtkValues[] = { 2, 3, 6, 7, 10, 11, 14, 15 };
	uint8_t i;
	bool atkValid = false;

	uint8_t atkIV = getStatIV(PokeStat::ATK, poke.iv_data);
	
	for(i=0; i < sizeof(validAtkValues); ++i)
	{
		if(atkIV == validAtkValues[i])
		{
			atkValid = true;
			break;
		}
	}

	if(!atkValid)
	{
		i = rand() % sizeof(validAtkValues);
		atkIV = validAtkValues[i];
	}

	// the first 4 bits need to be the valid atk IV (one of the validAtkValues list values)
	// all the other groups of 4 bits need to be set to 10 (SPEED, DEFENSE, SPECIAL)
	poke.iv_data[0] = (atkIV << 4) | 0xA;
	poke.iv_data[1] = 0xAA;
}

const char* gen2_getItemListTypeString(Gen2ItemListType type)
{
	switch(type)
	{
		case Gen2ItemListType::GEN2_ITEMLISTTYPE_ITEMPOCKET:
			return "Item Pocket";
		case Gen2ItemListType::GEN2_ITEMLISTTYPE_KEYITEMPOCKET:
			return "Key Item Pocket";
		case Gen2ItemListType::GEN2_ITEMLISTTYPE_BALLPOCKET:
			return "Ball Pocket";
		case Gen2ItemListType::GEN2_ITEMLISTTYPE_PC:
			return "PC";
		default:
			return "Invalid";
	}
}

void gen2_prepareDistributionPokemon(Gen2GameReader& gameReader, const Gen2DistributionPokemon& distributionPoke, Gen2TrainerPokemon& poke, const char*& originalTrainerName)
{
    if (distributionPoke.setPlayerAsOriginalTrainer)
    {
        originalTrainerName = gameReader.getTrainerName();
        poke.original_trainer_ID = gameReader.getTrainerID();
    }
    else
    {
		const Gen2LocalizationLanguage gameLang = gameReader.getGameLanguage();

		if(gameLang == Gen2LocalizationLanguage::JAPANESE && !distributionPoke.isJapanese)
		{
			// The Japanese games don't have all the latin characters in their character set.
			// So if transferring a non-japanese distribution event pokémon to a japanese cartridge,
			// we need to replace the OT.
			// If Google Translate is any reliable, ポケメ64 should translate to "Pokeme 64".
			originalTrainerName = "ポケメ64";
		}
		else if(gameLang != Gen2LocalizationLanguage::JAPANESE && distributionPoke.isJapanese)
		{
			// Obviously non-japanese cartridges/roms don't have the japanese characters in their character set.
			// So if transferring a japanese distribution event pokémon to a non-japanese cartridge,
			// we need to replace the OT.
			// We'll replace it with PokeMe64
			originalTrainerName = "PokeMe64";
		}
		else
		{
			originalTrainerName = distributionPoke.originalTrainer;
		}

        if (distributionPoke.regenerateTrainerID)
        {
            if (distributionPoke.originalTrainerID)
            {
                // limit set, apply it
                poke.original_trainer_ID = (uint16_t)(rand() % distributionPoke.originalTrainerID);
            }
            else
            {
                // no limit. The max is the max of the uint16_t type
                poke.original_trainer_ID = (uint16_t)(rand() % UINT16_MAX);
            }
        }
        else
        {
            poke.original_trainer_ID = distributionPoke.originalTrainerID;
        }
    }

    if (distributionPoke.shinyChance != 0xFF && (rand() % 100) <= distributionPoke.shinyChance)
    {
        // the pokemon will be shiny
        gen2_makePokemonShiny(poke);
    }
    else if (distributionPoke.randomizeIVs)
    {
        const uint16_t randomVal = (uint16_t)rand();
        poke.iv_data[0] = (uint8_t)(randomVal >> 8);
        poke.iv_data[1] = (uint8_t)(randomVal & 0xFF);
    }
    else
    {
        poke.iv_data[0] = distributionPoke.iv_data[0];
        poke.iv_data[1] = distributionPoke.iv_data[1];
    }
}