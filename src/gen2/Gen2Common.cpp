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
	{0x56, u8"……"},
	{0x5D, u8"TRAINER"},
	{0x60, u8"█"},
	{0x61, u8"▲"},
	{0x74, u8"・"},
	{0x75, u8"…"},
	{0x7F, u8" "},
	{0x80, u8"A"},
	{0x81, u8"B"},
	{0x82, u8"C"},
	{0x83, u8"D"},
	{0x84, u8"E"},
	{0x85, u8"F"},
	{0x86, u8"G"},
	{0x87, u8"H"},
	{0x88, u8"I"},
	{0x89, u8"J"},
	{0x8A, u8"K"},
	{0x8B, u8"L"},
	{0x8C, u8"M"},
	{0x8D, u8"N"},
	{0x8E, u8"O"},
	{0x8F, u8"P"},
	{0x90, u8"Q"},
	{0x91, u8"R"},
	{0x92, u8"S"},
	{0x93, u8"T"},
	{0x94, u8"U"},
	{0x95, u8"V"},
	{0x96, u8"W"},
	{0x97, u8"X"},
	{0x98, u8"Y"},
	{0x99, u8"Z"},
	{0x9A, u8"("},
	{0x9B, u8")"},
	{0x9C, u8":"},
	{0x9D, u8";"},
	{0x9E, u8"["},
	{0x9F, u8"]"},
	{0xA0, u8"a"},
	{0xA1, u8"b"},
	{0xA2, u8"c"},
	{0xA3, u8"d"},
	{0xA4, u8"e"},
	{0xA5, u8"f"},
	{0xA6, u8"g"},
	{0xA7, u8"h"},
	{0xA8, u8"i"},
	{0xA9, u8"j"},
	{0xAA, u8"k"},
	{0xAB, u8"l"},
	{0xAC, u8"m"},
	{0xAD, u8"n"},
	{0xAE, u8"o"},
	{0xAF, u8"p"},
	{0xB0, u8"q"},
	{0xB1, u8"r"},
	{0xB2, u8"s"},
	{0xB3, u8"t"},
	{0xB4, u8"u"},
	{0xB5, u8"v"},
	{0xB6, u8"w"},
	{0xB7, u8"x"},
	{0xB8, u8"y"},
	{0xB9, u8"z"},
	{0xBA, u8"à"},
	{0xBB, u8"è"},
	{0xBD, u8"ù"},
	{0xBE, u8"ß"},
	{0xBF, u8"ç"},
	{0xC0, u8"Ä"},
	{0xC1, u8"Ö"},
	{0xC2, u8"Ü"},
	{0xC3, u8"ä"},
	{0xC4, u8"ö"},
	{0xC5, u8"ü"},
	{0xC6, u8"ë"},
	{0xC7, u8"ï"},
	{0xC8, u8"â"},
	{0xC9, u8"ô"},
	{0xCA, u8"û"},
	{0xCB, u8"ê"},
	{0xCC, u8"î"},
	{0xD0, u8"'d"},
	{0xD1, u8"'l"},
	{0xD2, u8"'m"},
	{0xD3, u8"'r"},
	{0xD4, u8"'s"},
	{0xD5, u8"'t"},
	{0xD6, u8"'v"},
	{0xE0, u8"\'"},
	{0xE1, u8"PK"},
	{0xE2, u8"MN"},
	{0xE3, u8"-"},
	{0xE6, u8"?"},
	{0xE7, u8"!"},
	{0xE8, u8"."},
	{0xE9, u8"&"},
	{0xEA, u8"é"},
	{0xEB, u8"🡆"},
	{0xEC, u8"▷"},
	{0xED, u8"▶"},
	{0xEE, u8"▼"},
	{0xEF, u8"♂"},
	{0xF0, u8"¥"},
	{0xF1, u8"×"},
	{0xF3, u8"/"},
	{0xF4, u8","},
	{0xF5, u8"♀"},
	{0xF6, u8"0"},
	{0xF7, u8"1"},
	{0xF8, u8"2"},
	{0xF9, u8"3"},
	{0xFA, u8"4"},
	{0xFB, u8"5"},
	{0xFC, u8"6"},
	{0xFD, u8"7"},
	{0xFE, u8"8"},
	{0xFF, u8"9"},
	{0x51, u8" "}
};

static const TextCodePair gen2TextCodesJpn[] = {
	{0x01, u8"イ゙"},
	{0x02, u8"ヴ"},
	{0x03, u8"エ゙"},
	{0x04, u8"オ゙"},
	{0x05, u8"ガ"},
	{0x06, u8"ギ"},
	{0x07, u8"グ"},
	{0x08, u8"ゲ"},
	{0x09, u8"ゴ"},
	{0x0A, u8"ザ"},
	{0x0B, u8"ジ"},
	{0x0C, u8"ズ"},
	{0x0D, u8"ゼ"},
	{0x0E, u8"ゾ"},
	{0x0F, u8"ダ"},
	{0x10, u8"ヂ"},
	{0x11, u8"ヅ"},
	{0x12, u8"デ"},
	{0x13, u8"ド"},
	{0x14, u8"ナ゙"},
	{0x15, u8"ニ゙"},
	{0x16, u8"ヌ゙"},
	{0x17, u8"ネ゙"},
	{0x18, u8"ノ゙"},
	{0x19, u8"バ"},
	{0x1A, u8"ビ"},
	{0x1B, u8"ブ"},
	{0x1C, u8"ボ"},
	{0x1D, u8"マ゙"},
	{0x1E, u8"ミ゙"},
	{0x1F, u8"ム゙"},
	{0x20, u8"ィ゙"},
	{0x21, u8"あ゙"},
	{0x22, u8"い゙"},
	{0x23, u8"ゔ"},
	{0x24, u8"え゙"},
	{0x25, u8"お゙"},
	{0x26, u8"が"},
	{0x27, u8"ぎ"},
	{0x28, u8"ぐ"},
	{0x29, u8"げ"},
	{0x2A, u8"ご"},
	{0x2B, u8"ざ"},
	{0x2C, u8"じ"},
	{0x2D, u8"ず"},
	{0x2E, u8"ぜ"},
	{0x2F, u8"ぞ"},
	{0x30, u8"だ"},
	{0x31, u8"ぢ"},
	{0x32, u8"づ"},
	{0x33, u8"で"},
	{0x34, u8"ど"},
	{0x35, u8"な゙"},
	{0x36, u8"に゙"},
	{0x37, u8"ぬ゙"},
	{0x38, u8"ね゙"},
	{0x39, u8"の゙"},
	{0x3A, u8"ば"},
	{0x3B, u8"び"},
	{0x3C, u8"ぶ"},
	{0x3D, u8"べ"},
	{0x3E, u8"ぼ"},
	{0x3F, u8"ま゙"},
	{0x40, u8"パ"},
	{0x41, u8"ピ"},
	{0x42, u8"プ"},
	{0x43, u8"ポ"},
	{0x44, u8"ぱ"},
	{0x45, u8"ぴ"},
	{0x46, u8"ぷ"},
	{0x47, u8"ぺ"},
	{0x48, u8"ぽ"},
	{0x49, u8"ま゚"},
	{0x4D, u8"も゚"},
	{0x4F, u8" "},
	{0x57, u8"#"},
	{0x51, u8"*"},
	{0x52, u8"A1"},
	{0x53, u8"A2"},
	{0x54, u8"POKé"},
	{0x55, u8"+"},
	{0x58, u8"$"},
	{0x5D, u8"TRAINER"},
	{0x75, u8"…"},
	{0x7F, u8" "},
	{0x60, u8"A"},
	{0x61, u8"▲"},
	{0x62, u8"?"},
	{0x63, u8"D"},
	{0x64, u8"E"},
	{0x65, u8"F"},
	{0x66, u8"G"},
	{0x67, u8"H"},
	{0x68, u8"I"},
	{0x69, u8"V"},
	{0x6A, u8"S"},
	{0x6B, u8"L"},
	{0x6C, u8"M"},
	{0x6D, u8"："},
	{0x6E, u8"ぃ"},
	{0x6F, u8"ぅ"},
	{0x70, u8"「"},
	{0x71, u8"」"},
	{0x72, u8"『"},
	{0x73, u8"』"},
	{0x74, u8"・"},
	{0x75, u8"…"},
	{0x76, u8"ぁ"},
	{0x77, u8"ぇ"},
	{0x78, u8"ぉ"},
	{0x80, u8"ア"},
	{0x81, u8"イ"},
	{0x82, u8"ウ"},
	{0x83, u8"エ"},
	{0x84, u8"オ"},
	{0x85, u8"カ"},
	{0x86, u8"キ"},
	{0x87, u8"ク"},
	{0x88, u8"ケ"},
	{0x89, u8"コ"},
	{0x8A, u8"サ"},
	{0x8B, u8"シ"},
	{0x8C, u8"ス"},
	{0x8D, u8"セ"},
	{0x8E, u8"ソ"},
	{0x8F, u8"タ"},
	{0x90, u8"チ"},
	{0x91, u8"ツ"},
	{0x92, u8"テ"},
	{0x93, u8"ト"},
	{0x94, u8"ナ"},
	{0x95, u8"ニ"},
	{0x96, u8"ヌ"},
	{0x97, u8"ネ"},
	{0x98, u8"ノ"},
	{0x99, u8"ハ"},
	{0x9A, u8"ヒ"},
	{0x9B, u8"フ"},
	{0x9C, u8"ホ"},
	{0x9D, u8"マ"},
	{0x9E, u8"ミ"},
	{0x9F, u8"ム"},
	{0xA0, u8"メ"},
	{0xA1, u8"モ"},
	{0xA2, u8"ヤ"},
	{0xA3, u8"ユ"},
	{0xA4, u8"ヨ"},
	{0xA5, u8"ラ"},
	{0xA6, u8"ル"},
	{0xA7, u8"レ"},
	{0xA8, u8"ロ"},
	{0xA9, u8"ワ"},
	{0xAA, u8"ヲ"},
	{0xAB, u8"ン"},
	{0xAC, u8"ッ"},
	{0xAD, u8"ャ"},
	{0xAE, u8"ュ"},
	{0xAF, u8"ョ"},
	{0xB0, u8"ィ"},
	{0xB1, u8"あ"},
	{0xB2, u8"い"},
	{0xB3, u8"う"},
	{0xB4, u8"え"},
	{0xB5, u8"お"},
	{0xB6, u8"か"},
	{0xB7, u8"き"},
	{0xB8, u8"く"},
	{0xB9, u8"け"},
	{0xBA, u8"こ"},
	{0xBB, u8"さ"},
	{0xBC, u8"し"},
	{0xBD, u8"す"},
	{0xBE, u8"せ"},
	{0xBF, u8"そ"},
	{0xC0, u8"た"},
	{0xC1, u8"ち"},
	{0xC2, u8"つ"},
	{0xC3, u8"て"},
	{0xC4, u8"と"},
	{0xC5, u8"な"},
	{0xC6, u8"に"},
	{0xC7, u8"ぬ"},
	{0xC8, u8"ね"},
	{0xC9, u8"の"},
	{0xCA, u8"は"},
	{0xCB, u8"ひ"},
	{0xCC, u8"ふ"},
	{0xCD, u8"へ"},
	{0xCE, u8"ほ"},
	{0xCF, u8"ま"},
	{0xD0, u8"み"},
	{0xD1, u8"む"},
	{0xD2, u8"め"},
	{0xD3, u8"も"},
	{0xD4, u8"や"},
	{0xD5, u8"ゆ"},
	{0xD6, u8"よ"},
	{0xD7, u8"ら"},
	{0xD8, u8"リ"},
	{0xD9, u8"る"},
	{0xDA, u8"れ"},
	{0xDB, u8"ろ"},
	{0xDC, u8"わ"},
	{0xDD, u8"を"},
	{0xDE, u8"ん"},
	{0xDF, u8"っ"},
	{0xE0, u8"ゃ"},
	{0xE1, u8"ゅ"},
	{0xE2, u8"ょ"},
	{0xE3, u8"ー"},
	{0xE4, u8"゜"},
	{0xE5, u8"゛"},
	{0xE6, u8"?"},
	{0xE7, u8"!"},
	{0xE8, u8"。"},
	{0xE9, u8"ァ"},
	{0xEA, u8"ゥ"},
	{0xEB, u8"ェ"},
	{0xEC, u8"▷"},
	{0xED, u8"▶"},
	{0xEE, u8"▼"},
	{0xEF, u8"♂"},
	{0xF0, u8"円"},
	{0xF1, u8"×"},
	{0xF2, u8"."},
	{0xF3, u8"/"},
	{0xF4, u8"ォ"},
	{0xF5, u8"♀"},
	{0xF6, u8"0"},
	{0xF7, u8"1"},
	{0xF8, u8"2"},
	{0xF9, u8"3"},
	{0xFA, u8"4"},
	{0xFB, u8"5"},
	{0xFC, u8"6"},
	{0xFD, u8"7"},
	{0xFE, u8"8"},
	{0xFF, u8"9"}
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
        originalTrainerName = distributionPoke.originalTrainer;

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