#include "gen1/Gen1GameReader.h"
#include "gen1/Gen1SpriteDecoder.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "utils.h"

#include <cstdlib>
#include <cstdio>

/**
 * @brief This function calculates the main data checksum
 */
static uint8_t calculateMainDataChecksum(ISaveManager& saveManager, Gen1LocalizationLanguage localization)
{
    Gen1Checksum checksum;
    const uint32_t checksummedDataStart = 0x2598;
    const uint32_t checksummedDataEnd = gen1_getSRAMOffsets(localization).checksum;
    const uint32_t numBytes = checksummedDataEnd - checksummedDataStart;
    uint32_t i;
    uint8_t byte;

    saveManager.seek(checksummedDataStart);

    for(i=0; i < numBytes; ++i)
    {
        saveManager.readByte(byte);
        checksum.addByte(byte);
    }

    return checksum.get();
}

static uint8_t calculateWholeBoxBankChecksum(ISaveManager& saveManager, uint8_t bankIndex)
{
    Gen1Checksum checksum;
    const uint16_t checksummedDataStart = 0x0;
    const uint16_t checksummedDataEnd = 0x1A4C;
    const uint16_t numBytes = checksummedDataEnd - checksummedDataStart;
    uint16_t i;
    uint8_t byte;

    saveManager.seekToBankOffset(bankIndex, checksummedDataStart);
    for(i=0; i < numBytes; ++i)
    {
        saveManager.readByte(byte);
        checksum.addByte(byte);
    }

    return checksum.get();
}

/**
 * @brief This function decodes a binary coded decimal number stored in big endian format.
 * (see: https://en.wikipedia.org/wiki/Binary-coded_decimal)
 * It assumes 4 bits per digit and all digits are decimal (0-9)
 * 
 * @param bcdData the buffer containing the Binary coded decimal data
 * @param numDigits number of digits in the buffer
 */
static uint32_t decodeBigEndianBinaryCodedDecimalNumber(const uint8_t *bcdData, uint8_t numDigits)
{
    uint32_t result = 0;
    // The + 1 is because the division always rounds down.
    // but we want to make sure that if there's an odd number of digits, we still process the last one
    for(uint8_t i=0; i < (numDigits + 1) / 2; ++i)
    {
        const uint8_t upperNibble = (bcdData[i] >> 4);
        const uint8_t lowerNibble = (bcdData[i] & 0xF);
        result = (result * 100) + (upperNibble * 10) + lowerNibble;
    }
    return result;
}

/**
 * @brief This function encodes a binary coded decimal number in big endian format.
 * (see: https://en.wikipedia.org/wiki/Binary-coded_decimal)
 * The output will use 4 bits per digit and all digits are decimal (0-9)
 * 
 * @param bcdData a buffer with sufficient size to hold the binary encoded decimal data
 * @param numDigits number of digits to encode
 */
static void encodeBigEndianBinaryCodedDecimalNumber(uint32_t value, uint8_t *outBcdData, uint8_t numDigits)
{
    // since we're doing big endian, but we're extracting the digits from least significant to most significant,
    // we need to fill the output buffer backwards
    for(int8_t i = numDigits - 1; i >= 0; i -= 2)
    {
        const uint8_t lowerNibble = value % 10;
        value /= 10;
        const uint8_t upperNibble = value % 10;
        value /= 10;
        outBcdData[i] = (upperNibble << 4) | lowerNibble;
    }
}

Gen1GameReader::Gen1GameReader(IRomReader &romReader, ISaveManager &saveManager, Gen1GameType gameType, Gen1LocalizationLanguage language)
    : romReader_(romReader)
    , saveManager_(saveManager)
    , spriteDecoder_(romReader_)
    , iconDecoder_(romReader, gameType)
    , gameType_(gameType)
    , localization_(language)
{
    if(language == Gen1LocalizationLanguage::MAX)
    {
        localization_ = gen1_determineGameLanguage(romReader, gameType);
    }
}

Gen1LocalizationLanguage Gen1GameReader::getGameLanguage() const
{
    return localization_;
}

const char *Gen1GameReader::getPokemonName(uint8_t index) const
{
    // based on: https://github.com/seanmorris/pokemon-parser/blob/master/source/PokemonRom.js#L493
    static char result[20];
    uint8_t encodedText[0xA];
    uint32_t numRead;
    const uint16_t entrySize = (localization_ != Gen1LocalizationLanguage::JAPANESE) ? 0xA : 0x5;
    const uint32_t romOffset = gen1_getRomOffsets(gameType_, localization_).names;

    if(!romOffset)
    {
        snprintf(result, sizeof(result) - 1, "poke-%hhu", index);
        return result;
    }

    romReader_.seek(romOffset);
    romReader_.advance((index - 1) * entrySize);

    // max 10 bytes
    numRead = romReader_.readUntil(encodedText, 0x50, entrySize);

    gen1_decodePokeText(encodedText, numRead, result, sizeof(result), localization_);
    return result;
}

uint8_t Gen1GameReader::getPokemonNumber(uint8_t index) const
{
    // Based on https://github.com/seanmorris/pokemon-parser/blob/master/source/PokemonRom.js#L509
    uint8_t result = 0xFF;
    const uint32_t romOffset = gen1_getRomOffsets(gameType_, localization_).numbers;
    romReader_.seek(romOffset + (index - 1));
    romReader_.readByte(result);
    return result;
}

Gen1PokemonIconType Gen1GameReader::getPokemonIconType(uint8_t index) const
{
    //MonPartyData in pret/pokered and pret/pokeyellow
    // strangely, this array is in pokemon _number_ order, not index
    uint8_t number = getPokemonNumber(index);
    const uint32_t romOffset = gen1_getRomOffsets(gameType_, localization_).iconTypes;
    uint8_t byteVal;
    Gen1PokemonIconType result;

    if(number == 0xFF)
    {
        // invalid number!
        return Gen1PokemonIconType::GEN1_ICONTYPE_MAX;
    }
    // number is 1-based, but the array/offset is obviously 0-based. So subtract one
    --number;

    romReader_.seek(romOffset);

    // MonPartyData is a nybble (4 bit) array.
    romReader_.advance((number / 2));
    romReader_.readByte(byteVal);

    if((number & 0x1) == 0)
    {
        // if the number is even, the upper 4 bits need to be used
        result = (Gen1PokemonIconType)(byteVal >> 4);
    }
    else
    {
        // if odd, the lower 4 bits need to be used
        result = (Gen1PokemonIconType)(byteVal & 0x0F);
    }
    return result;
}

bool Gen1GameReader::isValidIndex(uint8_t index) const
{
    switch(index)
    {
        case 0x0:
        case 0x1F:
        case 0x20:
        case 0x32:
        case 0x34:
        case 0x38:
        case 0x3D:
        case 0x3E:
        case 0x3F:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x4F:
        case 0x50:
        case 0x51:
        case 0x56:
        case 0x57:
        case 0x5E:
        case 0x5F:
        case 0x73:
        case 0x79:
        case 0x7A:
        case 0x7F:
        case 0x86:
        case 0x87:
        case 0x89:
        case 0x8C:
        case 0x92:
        case 0x9C:
        case 0x9F:
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xAC:
        case 0xAE:
        case 0xAF:
        case 0xB5:
        case 0xB6:
        case 0xB7:
        case 0xB8:
            // Invalid (MISSINGNO) entry
            return false;
        default:
            break;
    }
    return (index < 192);
}

bool Gen1GameReader::readPokemonStatsForIndex(uint8_t index, Gen1PokeStats &outStats) const
{
    // Strangely enough, the pokemon stats data is not stored by index, but by pokedex number.
    // So we need to do a conversion here
    // Based on: https://bulbapedia.bulbagarden.net/wiki/Pok%C3%A9mon_species_data_structure_(Generation_I)#:~:text=The%20Pok%C3%A9mon%20species%20data%20structure,order%20rather%20than%20index%20number.
    // https://github.com/seanmorris/pokemon-parser/blob/master/source/PokemonRom.js#L516
    const uint8_t statsStructSize = 28;
    const uint8_t pokeNumber = getPokemonNumber(index);

    uint8_t spriteBank;

    if (gameType_ != Gen1GameType::YELLOW && index == 0x15)
    {
        spriteBank = 0x1;
    }
    else if (index == 0xB6)
    {
        spriteBank = 0xB;
    }
    else if (index < 0x1F)
    {
        spriteBank = 0x9;
    }
    else if (index < 0x4A)
    {
        spriteBank = 0xA;
    }
    else if (index < 0x74)
    {
        spriteBank = 0xB;
    }
    else if (index < 0x99)
    {
        spriteBank = 0xC;
    }
    else
    {
        spriteBank = 0xD;
    }

    if(gameType_ != Gen1GameType::YELLOW)
    {
        // Mew (of which the pokenumber is 151) stats are stored at a completely different location in the rom than the rest
        if (pokeNumber != 151)
        {
            romReader_.seek(gen1_getRomOffsets(gameType_, localization_).stats);
        }
        else
        {
            // mew stats
            romReader_.seek(gen1_getRomOffsets(gameType_, localization_).statsMew);
        }

        if (pokeNumber != 151)
        {
            // the number is 1-based.
            romReader_.advance(statsStructSize * (pokeNumber - 1));
        }
    }
    else
    {
        // Dealing with Pokemon yellow
        romReader_.seek(gen1_getRomOffsets(gameType_, localization_).stats);
        // the number is 1-based.
        romReader_.advance(statsStructSize * (pokeNumber - 1));
    }

    romReader_.readByte(outStats.pokedex_number);
    romReader_.readByte(outStats.base_hp);
    romReader_.readByte(outStats.base_attack);
    romReader_.readByte(outStats.base_defense);
    romReader_.readByte(outStats.base_speed);
    romReader_.readByte(outStats.base_special);
    romReader_.readByte(outStats.type1);
    romReader_.readByte(outStats.type2);
    romReader_.readByte(outStats.catch_rate);
    romReader_.readByte(outStats.base_exp_yield);
    romReader_.readByte(outStats.front_sprite_dimensions);
    outStats.sprite_bank = spriteBank;
    romReader_.readUint16(outStats.pointer_to_frontsprite);
    romReader_.readUint16(outStats.pointer_to_backsprite);
    romReader_.read(outStats.lvl1_attacks, 4);
    romReader_.readByte(outStats.growth_rate);
    romReader_.read(outStats.tm_hm_flags, 7);
    return true;
}

uint8_t Gen1GameReader::getColorPaletteIndexByPokemonNumber(uint8_t pokeNumber)
{
    uint8_t result;
    // pokeyellow.map from https://github.com/pret/pokeyellow (after compilation)
    const uint32_t romOffset = gen1_getRomOffsets(gameType_, localization_).paletteIndices;
    if(!romReader_.seek(romOffset + (pokeNumber - 1)))
    {
        return 0xFF;
    }

    if(!romReader_.readByte(result))
    {
        result = 0xFF;
    }

    return result;
}

void Gen1GameReader::readColorPalette(uint8_t paletteId, uint16_t* outColorPalette)
{
    uint16_t* cur = outColorPalette;
    const uint16_t* end = cur + 4;

    // based on https://datacrystal.romhacking.net/wiki/Pok%C3%A9mon_Red_and_Blue/ROM_map
    // and https://bulbapedia.bulbagarden.net/wiki/List_of_color_palettes_by_index_number_(Generation_I)
    const uint32_t romOffset = gen1_getRomOffsets(gameType_, localization_).palettes;
    romReader_.seek(romOffset + (paletteId * 8));
    while(cur < end)
    {
        romReader_.readUint16((*cur), Endianness::LITTLE);
        ++cur;
    }

}

const char *Gen1GameReader::getTrainerName() const
{
    static char result[20];
    uint8_t encodedPlayerName[0xB];
    saveManager_.seek(0x2598);

    saveManager_.readUntil(encodedPlayerName, 0x50, 0xB);
    gen1_decodePokeText(encodedPlayerName, sizeof(encodedPlayerName), result, sizeof(result), localization_);
    return result;
}

const char *Gen1GameReader::getRivalName() const
{
    static char result[20];
    uint8_t encodedRivalName[0xB];
    const uint32_t savOffset = gen1_getSRAMOffsets(localization_).rivalName;

    saveManager_.seek(savOffset);

    saveManager_.readUntil(encodedRivalName, 0x50, sizeof(encodedRivalName));
    gen1_decodePokeText(encodedRivalName, sizeof(encodedRivalName), result, sizeof(result), localization_);
    return result;
}

uint16_t Gen1GameReader::getTrainerID() const
{
    uint16_t result;
    const uint32_t savOffset = gen1_getSRAMOffsets(localization_).trainerID;
    saveManager_.seek(savOffset);
    saveManager_.readUint16(result);

    return result;
}

uint32_t Gen1GameReader::getTrainerMoney() const
{
    const uint32_t savOffset = gen1_getSRAMOffsets(localization_).trainerMoney;
    saveManager_.seek(savOffset);

    uint8_t encodedAmount[3];
    saveManager_.read(encodedAmount, 3);

    // for gen 1, money is stored as a big-endian BCD number (source: https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_I)#Main_Data)
    return decodeBigEndianBinaryCodedDecimalNumber(encodedAmount, 6);
}

void Gen1GameReader::setTrainerMoney(uint32_t amount)
{
    const uint32_t savOffset = gen1_getSRAMOffsets(localization_).trainerMoney;
    saveManager_.seek(savOffset);

    // for gen 1, money is stored as a big-endian BCD number (source: https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_I)#Main_Data)
    uint8_t encodedAmount[3];
    encodeBigEndianBinaryCodedDecimalNumber(amount, encodedAmount, 6);

    saveManager_.write(encodedAmount, 3);
}

Gen1Maps Gen1GameReader::getCurrentMap() const
{
    uint8_t result;
    const uint32_t savOffset = gen1_getSRAMOffsets(localization_).currentMap;
    saveManager_.seek(savOffset);

    saveManager_.readByte(result);
    return static_cast<Gen1Maps>(result);
}

Gen1Party Gen1GameReader::getParty()
{
    return Gen1Party((*this), saveManager_, localization_);
}

Gen1Box Gen1GameReader::getBox(uint8_t boxIndex)
{
    return Gen1Box((*this), saveManager_, boxIndex, localization_);
}

uint8_t Gen1GameReader::getCurrentBoxIndex()
{
    uint8_t byte;
    const uint32_t savOffset = gen1_getSRAMOffsets(localization_).currentBoxIndex;

    saveManager_.seek(savOffset);
    saveManager_.readByte(byte);

    return byte & 0x3F;   
}

bool Gen1GameReader::getPokedexFlag(PokedexFlag dexFlag, uint8_t pokedexNumber) const
{
    const Gen1LocalizationSRAMOffsets& sramOffsets = gen1_getSRAMOffsets(localization_);
    const uint32_t saveOffset = (dexFlag == POKEDEX_SEEN) ? sramOffsets.dexSeen : sramOffsets.dexOwned;
    uint8_t byte;

    if(pokedexNumber < 1 || pokedexNumber > 151)
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

void Gen1GameReader::setPokedexFlag(PokedexFlag dexFlag, uint8_t pokedexNumber) const
{
    const Gen1LocalizationSRAMOffsets& sramOffsets = gen1_getSRAMOffsets(localization_);
    const uint32_t saveOffset = (dexFlag == POKEDEX_SEEN) ? sramOffsets.dexSeen : sramOffsets.dexOwned;
    uint8_t byte;
    
    if(pokedexNumber < 1 || pokedexNumber > 151)
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

uint8_t Gen1GameReader::getPokedexCounter(PokedexFlag dexFlag) const
{
    const Gen1LocalizationSRAMOffsets& sramOffsets = gen1_getSRAMOffsets(localization_);
    const uint32_t saveOffset = (dexFlag == POKEDEX_SEEN) ? sramOffsets.dexSeen : sramOffsets.dexOwned;
    uint8_t bytes[19];
    uint8_t result = 0;

    saveManager_.seek(saveOffset);
    saveManager_.read(bytes, sizeof(bytes));

    const uint8_t *cur = bytes;
    const uint8_t *const end = bytes + sizeof(bytes);
    uint8_t bit = 0;

    while(cur < end)
    {
        bit = 0;
        // while we're not at the last bit yet, walk through all the bits and add to counter if set
        while(bit != 0x80)
        {
            if(bit == 0)
            {
                bit = 1;
            }
            else
            {
                bit <<= 1;
            }

            if((*cur) & bit)
            {
                ++result;
            }
        }
        
        ++cur;
    }
    return result;
}

uint8_t* Gen1GameReader::decodeSprite(uint8_t bankIndex, uint16_t pointer)
{
    return spriteDecoder_.decode(bankIndex, pointer);
}

uint8_t* Gen1GameReader::decodePokemonIcon(Gen1PokemonIconType iconType, bool firstFrame)
{
    return iconDecoder_.decode(localization_, iconType, firstFrame);
}

uint8_t Gen1GameReader::addPokemon(Gen1TrainerPokemon& poke, const char* originalTrainerID, const char* nickname)
{
    Gen1Party party = getParty();
    uint8_t result = 0xFF;
    const uint8_t pokedexNumber = getPokemonNumber(poke.poke_index);
    
    if(!originalTrainerID)
    {
        originalTrainerID = getTrainerName();
    }

    if(party.getNumberOfPokemon() < party.getMaxNumberOfPokemon())
    {
        party.add(poke, originalTrainerID, nickname);
        result = 0xFE;
    }
    else
    {
        const uint8_t currentBoxIndex = getCurrentBoxIndex();
        const uint8_t numBoxes = (localization_ != Gen1LocalizationLanguage::JAPANESE)? 12 : 8;
        for(uint8_t i = 0; i < numBoxes; ++i)
        {
            Gen1Box box = getBox(i);
            if(box.getNumberOfPokemon() == box.getMaxNumberOfPokemon())
            {
                continue;
            }

            box.add(poke, originalTrainerID, nickname);
            result = i;

            updateWholeBoxBankChecksum(getGen1BoxBankIndex(i, currentBoxIndex, localization_));
            break;
        }   
    }
    setPokedexFlag(PokedexFlag::POKEDEX_SEEN, pokedexNumber);
    setPokedexFlag(PokedexFlag::POKEDEX_OWNED, pokedexNumber);
    updateMainChecksum();
    return result;
}

uint8_t Gen1GameReader::addDistributionPokemon(const Gen1DistributionPokemon& distributionPoke, const char* nickname)
{
    Gen1TrainerPokemon poke = distributionPoke.poke;
    const char* originalTrainerName;

    // apply the attributes of Gen1DistributionPokemon to the actual Gen2TrainerPokemon instance
    gen1_prepareDistributionPokemon((*this), distributionPoke, poke, originalTrainerName);

    return addPokemon(poke, originalTrainerName, nickname);
}

bool Gen1GameReader::isMainChecksumValid()
{
    const uint32_t mainDataChecksumOffset = gen1_getSRAMOffsets(localization_).checksum;
    uint8_t storedChecksum;
    uint8_t calculatedChecksum;

    saveManager_.seek(mainDataChecksumOffset);
    saveManager_.readByte(storedChecksum);

    calculatedChecksum = calculateMainDataChecksum(saveManager_, localization_);

    return (storedChecksum == calculatedChecksum);
}

void Gen1GameReader::updateMainChecksum()
{
    const uint32_t mainDataChecksumOffset = gen1_getSRAMOffsets(localization_).checksum;
    const uint8_t calculatedChecksum = calculateMainDataChecksum(saveManager_, localization_);

    saveManager_.seek(mainDataChecksumOffset);
    saveManager_.writeByte(calculatedChecksum);
}

bool Gen1GameReader::isWholeBoxBankValid(uint8_t bankIndex)
{
    const uint16_t wholeBankChecksumOffset = 0x1A4C;
    uint8_t storedChecksum;
    uint8_t calculatedChecksum;

    saveManager_.seekToBankOffset(bankIndex, wholeBankChecksumOffset);
    saveManager_.readByte(storedChecksum);

    calculatedChecksum = calculateWholeBoxBankChecksum(saveManager_, bankIndex);

    return (storedChecksum == calculatedChecksum);
}

void Gen1GameReader::updateWholeBoxBankChecksum(uint8_t bankIndex)
{
    const uint16_t wholeBankChecksumOffset = 0x1A4C;
    if(bankIndex < 2 || bankIndex > 3)
    {
        // only valid/needed for bank 2 and 3
        return;
    }

    const uint8_t calculatedChecksum = calculateWholeBoxBankChecksum(saveManager_, bankIndex);
    saveManager_.seekToBankOffset(bankIndex, wholeBankChecksumOffset);
    saveManager_.writeByte(calculatedChecksum);
}