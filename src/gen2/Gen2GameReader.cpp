#include "gen2/Gen2GameReader.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "utils.h"

#include <cstdlib>
#include <cstdio>

// needed for crystal_fixPicBank() below
#define CRYSTAL_PICS_FIX 0x36
#define CRYSTAL_BANK_PICS_1 72

#define SAVE_CORRUPTION_CHECK1_EXPECTED_VALUE 99
#define SAVE_CORRUPTION_CHECK2_EXPECTED_VALUE 127

static const uint8_t crystalPicsBanks[] = {
    CRYSTAL_BANK_PICS_1 + 0,
    CRYSTAL_BANK_PICS_1 + 1,
    CRYSTAL_BANK_PICS_1 + 2,
    CRYSTAL_BANK_PICS_1 + 3,
    CRYSTAL_BANK_PICS_1 + 4,
    CRYSTAL_BANK_PICS_1 + 5,
    CRYSTAL_BANK_PICS_1 + 6,
    CRYSTAL_BANK_PICS_1 + 7,
    CRYSTAL_BANK_PICS_1 + 8,
    CRYSTAL_BANK_PICS_1 + 9,
    CRYSTAL_BANK_PICS_1 + 10,
    CRYSTAL_BANK_PICS_1 + 11,
    CRYSTAL_BANK_PICS_1 + 12,
    CRYSTAL_BANK_PICS_1 + 13,
    CRYSTAL_BANK_PICS_1 + 14,
    CRYSTAL_BANK_PICS_1 + 15,
    CRYSTAL_BANK_PICS_1 + 16,
    CRYSTAL_BANK_PICS_1 + 17,
    CRYSTAL_BANK_PICS_1 + 18,
    CRYSTAL_BANK_PICS_1 + 19,
    CRYSTAL_BANK_PICS_1 + 20,
    CRYSTAL_BANK_PICS_1 + 21,
    CRYSTAL_BANK_PICS_1 + 22,
    CRYSTAL_BANK_PICS_1 + 23
};

/**
 * @brief This function was created based on engine/gfx/load_pics.asm in https://github.com/pret/pokecrystal/tree/master (FixPicBank)
 * For some reason the bankByte is being "fixed" in Pokémon Crystal
 * 
 * ChatGPT assisted me with this function, because I'm not great at interpreting assembly.
 */
static uint8_t crystal_fixPicBank(uint8_t bankIndex)
{
    // Calculate the offset in the PicsBanks array
    const int16_t offset = static_cast<int16_t>(bankIndex) - (CRYSTAL_BANK_PICS_1 - CRYSTAL_PICS_FIX);

    // Ensure the offset is within the bounds of the PicsBanks array
    if (offset < 0 || offset >= static_cast<int16_t>(sizeof(crystalPicsBanks)))
    {
        // Handle error: invalid offset
        return 0; // or some error code
    }

    // Return the bank number from the PicsBanks array
    return crystalPicsBanks[offset];
}

/**
 * @brief Based on docs/gen2.txt line 104
 */
static uint8_t gold_silver_fixPicBank(uint8_t bankIndex)
{
    uint8_t result;
    // remap bankbyte according to docs/gen2.txt
    switch (bankIndex)
    {
    case 0x13:
        result = 0x1F;
        break;
    case 0x14:
        result = 0x20;
        break;
    case 0x1F:
        result = 0x2E;
        break;
    default:
        result = bankIndex;
        break;
    }
    return result;
}

static void addBytesToChecksum(ISaveManager &saveManager, uint32_t savOffsetStart, uint32_t savOffsetEnd, Gen2Checksum &checksum)
{
    if (!saveManager.seek(savOffsetStart))
    {
        return;
    }

    uint8_t byte;
    for (uint16_t i = savOffsetStart; i < savOffsetEnd + 1; ++i)
    {
        saveManager.readByte(byte);
        checksum.addByte(byte);
    }
}

// based on https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_II)#Gold_and_Silver
static uint16_t calculateMainRegionChecksum(ISaveManager &saveManager, Gen2GameType gameType, Gen2LocalizationLanguage localization)
{
    Gen2Checksum checksum;
    const uint32_t savOffsetStart = 0x2009;
    const uint32_t savOffsetEnd = gen2_getSRAMOffsets(gameType, localization).mainChecksumDataEndPos;

    addBytesToChecksum(saveManager, savOffsetStart, savOffsetEnd, checksum);

    return checksum.get();
}

// based on https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_II)#Gold_and_Silver
static uint16_t calculateBackupRegionChecksum(ISaveManager &saveManager, Gen2GameType gameType, Gen2LocalizationLanguage localization)
{
    Gen2Checksum checksum;

    if(localization == Gen2LocalizationLanguage::JAPANESE)
    {
        if(gameType == Gen2GameType::CRYSTAL)
        {
            addBytesToChecksum(saveManager, 0x7209, 0x7CE3, checksum);
        }
        else
        {
            addBytesToChecksum(saveManager, 0x7209, 0x7E8C, checksum);
        }
    }
    else if(localization == Gen2LocalizationLanguage::KOREAN)
    {
        addBytesToChecksum(saveManager, 0x106B, 0x1533, checksum); // 1224 bytes
        addBytesToChecksum(saveManager, 0x1534, 0x1A12, checksum); // 1246 bytes
        addBytesToChecksum(saveManager, 0x1A13, 0x1C38, checksum); // 549 bytes
        addBytesToChecksum(saveManager, 0x3DD8, 0x3F79, checksum); // 417 bytes
        addBytesToChecksum(saveManager, 0x7E39, 0x7E6A, checksum); // 49 bytes
    }
    else
    {
        if (gameType == Gen2GameType::CRYSTAL)
        {
            // for crystal, the backup save region is a contiguous address range
            addBytesToChecksum(saveManager, 0x1209, 0x1D82, checksum);
        }
        else
        {
            // for Gold/Silver the backup save region is split up into 5 blocks
            addBytesToChecksum(saveManager, 0x15C7, 0x17EC, checksum); //  550 bytes
            addBytesToChecksum(saveManager, 0x3D96, 0x3F3F, checksum); //  426 bytes
            addBytesToChecksum(saveManager, 0x0C6B, 0x10E7, checksum); // 1149 bytes
            addBytesToChecksum(saveManager, 0x7E39, 0x7E6C, checksum); //   52 bytes
            addBytesToChecksum(saveManager, 0x10E8, 0x15C6, checksum); // 1247 bytes
        }
    }
    return checksum.get();
}

static uint16_t readMainChecksum(ISaveManager &saveManager, Gen2GameType gameType, Gen2LocalizationLanguage localization)
{
    uint16_t result;
    const uint32_t savOffset = gen2_getSRAMOffsets(gameType, localization).mainChecksum;

    saveManager.seek(savOffset);
    saveManager.readUint16(result, Endianness::LITTLE);

    return result;
}

static uint16_t readBackupChecksum(ISaveManager &saveManager, Gen2GameType gameType, Gen2LocalizationLanguage localization)
{
    uint16_t result;
    const uint32_t savOffset = gen2_getSRAMOffsets(gameType, localization).backupChecksum;

    saveManager.seek(savOffset);
    saveManager.readUint16(result, Endianness::LITTLE);

    return result;
}

static void writeMainChecksum(ISaveManager &saveManager, Gen2GameType gameType, Gen2LocalizationLanguage localization, uint16_t checksum)
{
    const uint32_t savOffset = gen2_getSRAMOffsets(gameType, localization).mainChecksum;

    saveManager.seek(savOffset);
    saveManager.writeUint16(checksum, Endianness::LITTLE);
}

static void writeBackupChecksum(ISaveManager &saveManager, Gen2GameType gameType, Gen2LocalizationLanguage localization, uint16_t checksum)
{
    const uint32_t savOffset = gen2_getSRAMOffsets(gameType, localization).backupChecksum;

    saveManager.seek(savOffset);
    saveManager.writeUint16(checksum, Endianness::LITTLE);
}

static void updateMainChecksum(ISaveManager &saveManager, Gen2GameType gameType, Gen2LocalizationLanguage localization)
{
    const uint16_t calculatedChecksum = calculateMainRegionChecksum(saveManager, gameType, localization);
    writeMainChecksum(saveManager, gameType, localization, calculatedChecksum);
}

/**
 * This function copies the main save region to the backup save region.
 * It therefore overwrites the current backup save region.
 */
static void copyMainSaveRegionToBackupSaveRegion(ISaveManager& saveManager, Gen2GameType gameType, Gen2LocalizationLanguage localization)
{
    if(localization == Gen2LocalizationLanguage::JAPANESE)
    {
        if(gameType == Gen2GameType::CRYSTAL)
        {
            saveManager.copyRegion(0x2009, 0x7209, 2778);
        }
        else
        {
            saveManager.copyRegion(0x2009, 0x7209, 3203);
        }
    }
    else if(localization == Gen2LocalizationLanguage::KOREAN)
    {
        saveManager.copyRegion(0x2009, 0x106B, 1224); // 1224 bytes
        saveManager.copyRegion(0x24D1, 0x1534, 1246); // 1246 bytes
        saveManager.copyRegion(0x29AF, 0x1A13, 549); // 549 bytes
        saveManager.copyRegion(0x2BD4, 0x3DD8, 417); // 417 bytes
        saveManager.copyRegion(0x2D75, 0x7E39, 49); // 49 bytes
    }
    else
    {
        if (gameType == Gen2GameType::CRYSTAL)
        {
            // for crystal, the backup save region is a contiguous address range
            saveManager.copyRegion(0x2009, 0x1209, 2938);
        }
        else
        {
            // for Gold/Silver the backup save region is split up into 3 blocks
            saveManager.copyRegion(0x2009, 0x15C7, 550);  //  550 bytes
            saveManager.copyRegion(0x222F, 0x3D96, 426);  //  426 bytes
            saveManager.copyRegion(0x23D9, 0x0C6B, 1149); // 1149 bytes
            saveManager.copyRegion(0x2856, 0x7E39, 52);   //   52 bytes
            saveManager.copyRegion(0x288A, 0x10E8, 1247); // 1247 bytes
        }
    }

    // read main data checksum and copy that to the backup checksum
    writeBackupChecksum(saveManager, gameType, localization, readMainChecksum(saveManager, gameType, localization));
}

/**
 * Gen 2 has 2 canary values for the main save to check whether SRAM actually contains a save
 */
static bool hasMainSave(ISaveManager& saveManager, Gen2LocalizationLanguage localization, bool isCrystal)
{
    // check first canary value
    uint8_t saveCorruptionCheckValue1;
    uint8_t saveCorruptionCheckValue2;

    if(localization == Gen2LocalizationLanguage::JAPANESE || localization == Gen2LocalizationLanguage::KOREAN)
    {
        // I can't check for the canary values in a japanese or Korean save because I don't know the save offset nor the expected value.
        // I can't rely on the decompilation projects (Pret's pokegold/pokecrystal) because they only do the international version.
        // I also can't rely on PkHex's source code because they're not checking this.
        return true;
    }

    // this value is at the same offset for Gold/Silver/Crystal
    saveManager.seekToBankOffset(1, 0x8);
    saveManager.readByte(saveCorruptionCheckValue1);

    // check second canary
    // for this one the offset differs in crystal
    if(isCrystal)
    {
        saveManager.seekToBankOffset(1, 0xD0F);
    }
    else
    {
        saveManager.seekToBankOffset(1, 0xD6B);
    }
    saveManager.readByte(saveCorruptionCheckValue2);

    // for there to be an actual save, the canary values need to be set to these exact values.
    // Taken from the PRET pokecrystal and pokegold projects
    // https://github.com/pret/pokecrystal/blob/symbols/pokecrystal.sym
    // https://github.com/pret/pokegold/blob/symbols/pokegold.sym
    return (saveCorruptionCheckValue1 == SAVE_CORRUPTION_CHECK1_EXPECTED_VALUE) 
        && (saveCorruptionCheckValue2 == SAVE_CORRUPTION_CHECK2_EXPECTED_VALUE);

}

/**
 * Gen 2 has 2 canary values for the main save to check whether SRAM actually contains a save
 */
static bool hasBackupSave(ISaveManager& saveManager, Gen2LocalizationLanguage localization, bool isCrystal)
{
    // check first canary value
    uint8_t saveCorruptionCheckValue1;
    uint8_t saveCorruptionCheckValue2;

    if(localization == Gen2LocalizationLanguage::JAPANESE || localization == Gen2LocalizationLanguage::KOREAN)
    {
        // I can't check for the canary values in a japanese or Korean save because I don't know the save offset nor the expected value.
        // I can't rely on the decompilation projects (Pret's pokegold/pokecrystal) because they only do the international version.
        // I also can't rely on PkHex's source code because they're not checking this.
        return true;
    }

    if(isCrystal)
    {
        saveManager.seekToBankOffset(0, 0x1208);
    }
    else
    {
        saveManager.seekToBankOffset(3, 0x1E38);
    }
    saveManager.readByte(saveCorruptionCheckValue1);

    // check second canary
    // for this one the offset differs in crystal
    if(isCrystal)
    {
        saveManager.seekToBankOffset(0, 0x1F0F);
    }
    else
    {
        saveManager.seekToBankOffset(3, 0x1E6F);
    }
    saveManager.readByte(saveCorruptionCheckValue2);

    // for there to be an actual save, the canary values need to be set to these exact values.
    // Taken from the PRET pokecrystal and pokegold projects
    // https://github.com/pret/pokecrystal/blob/symbols/pokecrystal.sym
    // https://github.com/pret/pokegold/blob/symbols/pokegold.sym
    return (saveCorruptionCheckValue1 == SAVE_CORRUPTION_CHECK1_EXPECTED_VALUE) 
        && (saveCorruptionCheckValue2 == SAVE_CORRUPTION_CHECK2_EXPECTED_VALUE);

}

Gen2GameReader::Gen2GameReader(IRomReader &romReader, ISaveManager &saveManager, Gen2GameType gameType, Gen2LocalizationLanguage language)
    : romReader_(romReader)
    , saveManager_(saveManager)
    , spriteDecoder_(romReader)
    , iconDecoder_(romReader, gameType)
    , gameType_(gameType)
    , localization_(language)
{
    if(language == Gen2LocalizationLanguage::MAX)
    {
        localization_ = gen2_determineGameLanguage(romReader, gameType);
//      printf("Detected localization=%d\n", (int)localization_);
    }
}

Gen2GameReader::~Gen2GameReader()
{
}

Gen2GameType Gen2GameReader::getGameType() const
{
    return gameType_;
}

Gen2LocalizationLanguage Gen2GameReader::getGameLanguage() const
{
    return localization_;
}

const char *Gen2GameReader::getPokemonName(uint8_t index) const
{
    static char result[20];
    uint8_t encodedText[0xA];
    uint32_t numRead;
    uint8_t maxNumBytesPerName;

    const uint32_t romOffset = gen2_getRomOffsets(gameType_, localization_).names;
    if(!romOffset)
    {
        snprintf(result, sizeof(result) - 1, "poke-%hhu", index);
        return result;
    }

    // The max name length is only 5 chars (and therefore bytes) in Japanese roms.
    // But it's 10 for every other language
    switch(localization_)
    {
    case Gen2LocalizationLanguage::JAPANESE:
        maxNumBytesPerName = 5;
        break;
    default:
        maxNumBytesPerName = 10;
        break;
    }

    romReader_.seek(romOffset + (maxNumBytesPerName * (index - 1)));

    // based on what I encountered so far. 0x50 is default, however it turns out 0x75 (…) is used too!
    const uint8_t nameTerminators[] = {0x50, 0x75};
    numRead = romReader_.readUntil(encodedText, nameTerminators, sizeof(nameTerminators), maxNumBytesPerName);

    gen2_decodePokeText(encodedText, numRead, result, sizeof(result) - 1, localization_);
    return result;
}

Gen2PokemonIconType Gen2GameReader::getPokemonIconType(uint8_t index) const
{
    const uint32_t romOffset = gen2_getRomOffsets(gameType_, localization_).iconTypes;
    uint8_t byteVal;

    if(!romOffset)
    {
        return Gen2PokemonIconType::GEN2_ICONTYPE_MAX;
    }

    romReader_.seek(romOffset);
    romReader_.advance((index - 1));
    romReader_.readByte(byteVal);

    return (Gen2PokemonIconType)byteVal;
}

bool Gen2GameReader::isValidIndex(uint8_t index) const
{
    return (index != 0) && (index < 252);
}

bool Gen2GameReader::readPokemonStatsForIndex(uint8_t index, Gen2PokeStats &outStats) const
{
    const uint8_t statsStructSize = 32;
    const uint32_t romOffset = gen2_getRomOffsets(gameType_, localization_).stats;

    romReader_.seek(romOffset);

    // now move to the specific pokemon stats entry
    // this is 1-based
    if (!romReader_.advance(statsStructSize * (index - 1)))
    {
        return false;
    }

    romReader_.readByte(outStats.pokedex_number);
    romReader_.readByte(outStats.base_hp);
    romReader_.readByte(outStats.base_attack);
    romReader_.readByte(outStats.base_defense);
    romReader_.readByte(outStats.base_speed);
    romReader_.readByte(outStats.base_special_attack);
    romReader_.readByte(outStats.base_special_defense);
    romReader_.readByte(outStats.type1);
    romReader_.readByte(outStats.type2);
    romReader_.readByte(outStats.catch_rate);
    romReader_.readByte(outStats.base_exp_yield);
    romReader_.readByte(outStats.wild_held_item1);
    romReader_.readByte(outStats.wild_held_item2);
    romReader_.readByte(outStats.gender_ratio);
    romReader_.readByte(outStats.unknown);
    romReader_.readByte(outStats.egg_cycles);
    romReader_.readByte(outStats.unknown2);
    romReader_.readByte(outStats.front_sprite_dimensions);
    romReader_.read(outStats.blank, 4);
    romReader_.readByte(outStats.growth_rate);
    romReader_.readByte(outStats.egg_groups);
    romReader_.read(outStats.tm_hm_flags, 8);

    return true;
}

bool Gen2GameReader::readFrontSpritePointer(uint8_t index, uint8_t &outBankIndex, uint16_t &outPointer)
{
    const uint8_t bytesPerPokemon = 6;

    // For Crystal, this offset was really hard to find.
    // I eventually found it with these 2 files:
    // https://raw.githubusercontent.com/pret/pokecrystal/symbols/pokecrystal.map
    // https://github.com/pret/pokecrystal/blob/master/data/pokemon/pic_pointers.asm
    //
    const uint32_t romOffset = gen2_getRomOffsets(gameType_, localization_).spritePointers;

    // I don't support Unown sprite decoding right now, because unown is stored separately based on the variant.
    if (index == 201)
    {
        return false;
    }

    if (!romReader_.seek(romOffset))
    {
        return false;
    }
    if (!romReader_.advance((index - 1) * bytesPerPokemon))
    {
        return false;
    }

    romReader_.readByte(outBankIndex);

    // for some reason, the bank byte doesn't contain the actual bank index
    // it's a remapped version of it. So we need to convert it.
    if (isGameCrystal())
    {
        outBankIndex = crystal_fixPicBank(outBankIndex);
    }
    else
    {
        outBankIndex = gold_silver_fixPicBank(outBankIndex);
    }

    // NOTE: the pointer returned is a pointer to the memory address once the bank is mapped.
    // what you need to know here is that bank 0 is always mapped in a gameboy
    // but the memory addresses directly after those of bank 0 are the memory addresses at which the switchable bank is mapped to.
    // Therefore, once mapped, an offset within the switchable bank will ALWAYS be mapped to an address between 0x4000 and 0x8000
    // This is why the read pointer will also always be a value between 0x4000 and 0x8000, as it is a pointer within the mapped address space
    // to convert to an actual offset within the relevant bank, we just need to subtract 0x4000 from this pointer
    // We deal with this in our IRomReader implementation. See seekToRomPointer()
    romReader_.readUint16(outPointer, Endianness::LITTLE);

    return true;
}

void Gen2GameReader::readSpriteDimensions(const Gen2PokeStats &poke, uint8_t &outWidthInTiles, uint8_t &outHeightInTiles)
{
    outWidthInTiles = poke.front_sprite_dimensions >> 4;
    outHeightInTiles = poke.front_sprite_dimensions & 0xF;
}

bool Gen2GameReader::readColorPaletteForPokemon(uint8_t index, bool shiny, uint16_t *outColorPalette)
{
    // index is 1-based.
    // 2 color palettes are offered per pokemon: non-shiny (4 bytes), shiny (4 bytes)
    // each stored color palette only has 2 out of 4 colors actually stored in the rom.
    // that's because for every color palette, the first color is white and the last color is black.
    // so the rom only stores the 2nd and 3rd colors
    // each of these colors is an uint16_t
    const uint32_t romOffset = gen2_getRomOffsets(gameType_, localization_).spritePalettes;

    if (!romReader_.seek(romOffset + ((index - 1) * 8)))
    {
        return false;
    }

    if (shiny)
    {
        if (!romReader_.advance(4))
        {
            return false;
        }
    }

    uint16_t *curColor = outColorPalette;

    // first color is always white
    (*curColor) = 0x7FFF;
    ++curColor;
    // only the second and third color are actually stored in gen 2
    romReader_.readUint16(*curColor, Endianness::LITTLE);
    ++curColor;
    romReader_.readUint16(*curColor, Endianness::LITTLE);
    ++curColor;
    // last color is always black
    (*curColor) = 0;

    return true;
}

uint8_t *Gen2GameReader::decodeSprite(uint8_t bankIndex, uint16_t pointer)
{
    return spriteDecoder_.decode(bankIndex, pointer);
}

uint8_t *Gen2GameReader::decodePokemonIcon(Gen2PokemonIconType iconType, bool firstFrame)
{
    return iconDecoder_.decode(localization_, iconType, firstFrame);
}

uint8_t Gen2GameReader::addPokemon(Gen2TrainerPokemon &poke, bool isEgg, const char *originalTrainerID, const char *nickname)
{
    Gen2Party party = getParty();
    uint8_t result = 0xFF;

    if (!originalTrainerID)
    {
        originalTrainerID = getTrainerName();
    }

    if (party.getNumberOfPokemon() < party.getMaxNumberOfPokemon())
    {
        party.add(poke, isEgg, originalTrainerID, nickname);
        result = 0xFE;
    }
    else
    {
        for (uint8_t i = 0; i < 14; ++i)
        {
            Gen2Box box = getBox(i);
            if (box.getNumberOfPokemon() == box.getMaxNumberOfPokemon())
            {
                continue;
            }

            box.add(poke, isEgg, originalTrainerID, nickname);
            result = i;
            break;
        }
    }
    setPokedexFlag(PokedexFlag::POKEDEX_SEEN, poke.poke_index);
    setPokedexFlag(PokedexFlag::POKEDEX_OWNED, poke.poke_index);
    return result;
}

uint8_t Gen2GameReader::addDistributionPokemon(const Gen2DistributionPokemon &distributionPoke, const char *nickname)
{
    Gen2TrainerPokemon poke = distributionPoke.poke;
    const char *originalTrainerName;

    // apply the attributes of Gen2DistributionPokemon to the actual Gen2TrainerPokemon instance
    gen2_prepareDistributionPokemon((*this), distributionPoke, poke, originalTrainerName);

    return addPokemon(poke, distributionPoke.isEgg, originalTrainerName, nickname);
}

uint16_t Gen2GameReader::getTrainerID() const
{
    uint16_t result = 0;
    if (!saveManager_.seek(0x2009))
    {
        return result;
    }

    saveManager_.readUint16(result, Endianness::BIG);
    return result;
}

const char *Gen2GameReader::getTrainerName() const
{
    static char result[20];
    uint8_t encodedPlayerName[0xB];

    saveManager_.seek(0x200B);

    saveManager_.readUntil(encodedPlayerName, 0x50, 0xB);
    gen2_decodePokeText(encodedPlayerName, sizeof(encodedPlayerName), result, sizeof(result), localization_);
    return result;
}

const char *Gen2GameReader::getRivalName() const
{
    static char result[20];
    uint8_t encodedPlayerName[0xB];
    const uint32_t savOffset = gen2_getSRAMOffsets(gameType_, localization_).rivalName;

    saveManager_.seek(savOffset);
    saveManager_.readUntil(encodedPlayerName, 0x50, 0xB);

    gen2_decodePokeText(encodedPlayerName, sizeof(encodedPlayerName), result, sizeof(result), localization_);
    return result;
}

uint8_t Gen2GameReader::getCurrentBoxIndex()
{
    uint8_t result;
    const uint16_t saveOffset = gen2_getSRAMOffsets(gameType_, localization_).currentBoxIndex;
    if (!saveManager_.seek(saveOffset))
    {
        return 0;
    }

    saveManager_.readByte(result);
    // only the lowest 4 bits of this value are the box index
    return (result & 0xF);
}

Gen2Party Gen2GameReader::getParty()
{
    return Gen2Party((*this), saveManager_, localization_);
}

Gen2Box Gen2GameReader::getBox(uint8_t boxIndex)
{
    return Gen2Box((*this), saveManager_, boxIndex, localization_);
}

Gen2ItemList Gen2GameReader::getItemList(Gen2ItemListType type)
{
    return Gen2ItemList(saveManager_, type, gameType_, localization_);
}

Gen2ClockManager Gen2GameReader::getClockManager(IRTCReader& rtcReader)
{
    return Gen2ClockManager(saveManager_, rtcReader, gameType_, localization_);
}

Gen2MysteryGiftManager Gen2GameReader::getMysteryGiftManager()
{
    return Gen2MysteryGiftManager(saveManager_, gameType_, localization_);
}

bool Gen2GameReader::getPokedexFlag(PokedexFlag dexFlag, uint8_t pokedexNumber)
{
    uint32_t saveOffset;

    if(dexFlag == POKEDEX_SEEN)
    {
        saveOffset = gen2_getSRAMOffsets(gameType_, localization_).dexSeen;
    }
    else
    {
        saveOffset = gen2_getSRAMOffsets(gameType_, localization_).dexOwned;
    }

    uint8_t byte;
    if (pokedexNumber < 1 || pokedexNumber > 251)
    {
        return false;
    }
    // for the next operations, the pokedexNumber will be used as a zero-based value
    --pokedexNumber;

    saveManager_.seek(saveOffset + (pokedexNumber / 8));
    saveManager_.readByte(byte);

    // in this case, bits are ordered within bytes from lowest to highest
    // source: https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_I)#bank1_main_pokedex
    return (byte >> (pokedexNumber % 8)) & 0x1;
}

void Gen2GameReader::setPokedexFlag(PokedexFlag dexFlag, uint8_t pokedexNumber)
{
    uint32_t saveOffset;

    if(dexFlag == POKEDEX_SEEN)
    {
        saveOffset = gen2_getSRAMOffsets(gameType_, localization_).dexSeen;
    }
    else
    {
        saveOffset = gen2_getSRAMOffsets(gameType_, localization_).dexOwned;
    }

    uint8_t byte;
    if (pokedexNumber < 1 || pokedexNumber > 251)
    {
        return;
    }
    // for the next operations, the pokedexNumber will be used as a zero-based value
    --pokedexNumber;

    saveManager_.seek(saveOffset + (pokedexNumber / 8));
    byte = saveManager_.peek();

    // in this case, bits are ordered within bytes from lowest to highest
    // source: https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_I)#bank1_main_pokedex
    byte |= (1 << (pokedexNumber % 8));
    saveManager_.writeByte(byte);
}

uint8_t Gen2GameReader::getPokedexCounter(PokedexFlag dexFlag)
{
    uint32_t saveOffset;

    if(dexFlag == POKEDEX_SEEN)
    {
        saveOffset = gen2_getSRAMOffsets(gameType_, localization_).dexSeen;
    }
    else
    {
        saveOffset = gen2_getSRAMOffsets(gameType_, localization_).dexOwned;
    }

    uint8_t bytes[32];
    uint8_t result = 0;

    saveManager_.seek(saveOffset);
    saveManager_.read(bytes, sizeof(bytes));

    const uint8_t *cur = bytes;
    const uint8_t *const end = bytes + sizeof(bytes);
    uint8_t bit = 0;

    while (cur < end)
    {
        bit = 0;
        // while we're not at the last bit yet, walk through all the bits and add to counter if set
        while (bit != 0x80)
        {
            // this if may look a bit strange here, but this was an easy way to evaluate the first bit
            // without having to duplicate the bit check below
            if (bit == 0)
            {
                bit = 1;
            }
            else
            {
                bit <<= 1;
            }

            if ((*cur) & bit)
            {
                ++result;
            }
        }

        ++cur;
    }
    return result;
}

bool Gen2GameReader::isMainChecksumValid()
{
    const bool isCrystal = isGameCrystal();

    if(!hasMainSave(saveManager_, localization_, isCrystal))
    {
        return false;
    }

    const uint16_t storedChecksum = readMainChecksum(saveManager_, gameType_, localization_);
    const uint16_t calculatedChecksum = calculateMainRegionChecksum(saveManager_, gameType_, localization_);

    return (storedChecksum == calculatedChecksum);
}

bool Gen2GameReader::isBackupChecksumValid()
{
    const bool isCrystal = isGameCrystal();

    if(!hasBackupSave(saveManager_, localization_, isCrystal))
    {
        return false;
    }

    const uint16_t storedChecksum = readBackupChecksum(saveManager_, gameType_, localization_);
    const uint16_t calculatedChecksum = calculateBackupRegionChecksum(saveManager_, gameType_, localization_);

    return (storedChecksum == calculatedChecksum);
}

void Gen2GameReader::finishSave()
{
    const uint16_t currentPcBoxSaveOffset = gen2_getSRAMOffsets(gameType_, localization_).currentBox;
    Gen2Box currentBox = getBox(getCurrentBoxIndex());

#if 1
    // we may have modified or current PC box. However, Gen2GameReader doesn't modify this section in the save directly/immediately.
    // instead we write directly to the intended PC box area when we're doign changes.
    // and we rely on this function (finishSave) to copy the changed PC box to the current PC box save region.
    saveManager_.copyRegion(currentBox.getSaveOffset(), currentPcBoxSaveOffset, GEN2_PC_BOX_SIZE_IN_BYTES);
#endif
    updateMainChecksum(saveManager_, gameType_, localization_);

    // now start copying the main save region to the backup save region(s)
    copyMainSaveRegionToBackupSaveRegion(saveManager_, gameType_, localization_);
}

bool Gen2GameReader::isGameCrystal() const
{
    return (gameType_ == Gen2GameType::CRYSTAL);
}

void Gen2GameReader::unlockGsBallEvent()
{
    if(!isGameCrystal())
    {
        return;
    }
    const Gen2LocalizationSRAMOffsets& sramOffsets = gen2_getSRAMOffsets(gameType_, localization_);

    // let's remove the GS ball from the players' inventory first (if it exists)
    // this is to cover up my earlier fuck up with PokeMe64 which only handed out the item.
    // it also is part of making the event repeatable.
    Gen2ItemList itemList = getItemList(Gen2ItemListType::GEN2_ITEMLISTTYPE_KEYITEMPOCKET);
    itemList.remove(CRYSTAL_ITEM_ID_GS_BALL);

    // now let's reset the GS Ball related event flags (to make the event repeatable)
    setEventFlag(CRYSTAL_EVENTFLAG_RECEIVED_GSBALL, false);
    setEventFlag(CRYSTAL_EVENTFLAG_KURT_CAN_CHECK_GSBALL, false);
    setEventFlag(CRYSTAL_EVENTFLAG_KURT_READY_TO_RETURN_GSBALL, false);
    setEventFlag(CRYSTAL_EVENTFLAG_GSBALL_USABLE_IN_ILEX_FOREST_SHRINE, false);

    // unlock the event itself. This triggers the NPC to stop you when trying to leave the Golden Rod pokémon center
    // to give you the GS Ball.
    // to be honest I have no clue what the exact meaning of this byte is. But hey, it works!
    saveManager_.seek(sramOffsets.gsBallMain);
    saveManager_.writeByte(CRYSTAL_GS_BALL_ENABLE_VALUE);
    // this is a mirror of the previous byte. It's supposedly not needed. But let's just set it to be safe.
    saveManager_.seek(sramOffsets.gsBallBackup);
    saveManager_.writeByte(CRYSTAL_GS_BALL_ENABLE_VALUE);
}

bool Gen2GameReader::getEventFlag(uint16_t flagNumber)
{
    const uint32_t saveOffset = gen2_getSRAMOffsets(gameType_, localization_).eventFlags;
    uint8_t byteVal;
    const uint8_t flag = 1 << (flagNumber % 8);

    saveManager_.seek(saveOffset + (flagNumber / 8));
    saveManager_.readByte(byteVal);

    return (byteVal & flag);
}

void Gen2GameReader::setEventFlag(uint16_t flagNumber, bool enabled)
{
    const uint32_t saveOffset = gen2_getSRAMOffsets(gameType_, localization_).eventFlags;
    uint8_t byteVal;
    uint8_t resultVal;
    const uint8_t flag = 1 << (flagNumber % 8);

    saveManager_.seek(saveOffset + (flagNumber / 8));
    saveManager_.readByte(byteVal);
    saveManager_.rewind(1);

    // first calculate the value of the byte with the bit reset to 0
    resultVal = byteVal & (~flag);

    // now apply the flag if enabled == true
    if(enabled)
    {
        resultVal |= flag;
    }

    saveManager_.writeByte(resultVal);
}
