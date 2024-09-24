#include "gen1/Gen1GameReader.h"
#include "gen1/Gen1SpriteDecoder.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "utils.h"

#include <cstdlib>
/**
 * @brief This function calculates the main data checksum
 */
uint8_t calculateMainDataChecksum(ISaveManager& saveManager)
{
    Gen1Checksum checksum;
    const uint16_t checksummedDataStart = 0x598;
    const uint16_t checksummedDataEnd = 0x1523;
    const uint16_t numBytes = checksummedDataEnd - checksummedDataStart;
    uint16_t i;
    uint8_t byte;

    saveManager.seekToBankOffset(1, checksummedDataStart);

    for(i=0; i < numBytes; ++i)
    {
        saveManager.readByte(byte);
        checksum.addByte(byte);
    }

    return checksum.get();
}

uint8_t calculateWholeBoxBankChecksum(ISaveManager& saveManager, uint8_t bankIndex)
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

Gen1GameReader::Gen1GameReader(IRomReader &romReader, ISaveManager &saveManager, Gen1GameType gameType)
    : romReader_(romReader)
    , saveManager_(saveManager)
    , spriteDecoder_(romReader_)
    , gameType_(gameType)
{
}

const char *Gen1GameReader::getPokemonName(uint8_t index) const
{
    // based on: https://github.com/seanmorris/pokemon-parser/blob/master/source/PokemonRom.js#L493
    static char result[20];
    uint8_t encodedText[0xA];
    uint32_t numRead;
    uint16_t pointer;

    if(gameType_ == Gen1GameType::BLUE || gameType_ == Gen1GameType::RED)
    {
        romReader_.seek(0x2FA3);
        const uint8_t bankByte = romReader_.peek();

        romReader_.seek(0x2FAE);
        romReader_.readUint16(pointer);

        // seek to the right location
        romReader_.seekToRomPointer(pointer, bankByte);
    }
    else
    {   
        // Pkmn Yellow
        romReader_.seek(0xE8000);
    }

    romReader_.advance((index - 1) * 0xA);

    // max 10 bytes
    numRead = romReader_.readUntil(encodedText, 0x50, 0xA);

    gen1_decodePokeText(encodedText, numRead, result, sizeof(result));
    return result;
}

uint8_t Gen1GameReader::getPokemonNumber(uint8_t index) const
{
    // Based on https://github.com/seanmorris/pokemon-parser/blob/master/source/PokemonRom.js#L509
    uint8_t result = 0xFF;
    const uint32_t romOffset = (gameType_ == Gen1GameType::YELLOW) ? 0x410B1 : 0x41024;
    romReader_.seek(romOffset + (index - 1));
    romReader_.readByte(result);
    return result;
}

Gen1PokemonIconType Gen1GameReader::getPokemonIconType(uint8_t index) const
{
    //MonPartyData in pret/pokered and pret/pokeyellow
    // strangely, this array is in pokemon _number_ order, not index
    uint8_t number = getPokemonNumber(index);
    const uint32_t romOffset = (gameType_ == Gen1GameType::YELLOW) ? 0x719BA : 0x7190D;
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
    uint8_t statsBank;
    uint16_t statsPointer;

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
            romReader_.seek(0x153B);
            romReader_.readByte(statsBank);

            romReader_.seek(0x1578);
            romReader_.readUint16(statsPointer);
        }
        else
        {
            // mew stats
            romReader_.seek(0x159C);
            romReader_.readByte(statsBank);

            romReader_.seek(0x1593);
            romReader_.readUint16(statsPointer);
        }

        romReader_.seekToRomPointer(statsPointer, statsBank);
        if (pokeNumber != 151)
        {
            // the number is 1-based.
            romReader_.advance(statsStructSize * (pokeNumber - 1));
        }
    }
    else
    {
        // Dealing with Pokemon yellow
        romReader_.seek(0x383DE);
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
    const uint32_t romOffset = (gameType_ == Gen1GameType::YELLOW) ?  0x72922 : 0x725C9;
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
    const uint32_t romOffset = (gameType_ == Gen1GameType::YELLOW) ? 0x72AF9 : 0x72660;
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
    gen1_decodePokeText(encodedPlayerName, sizeof(encodedPlayerName), result, sizeof(result));
    return result;
}

const char *Gen1GameReader::getRivalName() const
{
    static char result[20];
    uint8_t encodedRivalName[0xB];
    saveManager_.seek(0x25F6);

    saveManager_.readUntil(encodedRivalName, 0x50, sizeof(encodedRivalName));
    gen1_decodePokeText(encodedRivalName, sizeof(encodedRivalName), result, sizeof(result));
    return result;
}

uint16_t Gen1GameReader::getTrainerID() const
{
    uint16_t result;

    saveManager_.seek(0x2605);
    saveManager_.readUint16(result);

    return result;
}

Gen1Party Gen1GameReader::getParty()
{
    return Gen1Party((*this), saveManager_);
}

Gen1Box Gen1GameReader::getBox(uint8_t boxIndex)
{
    return Gen1Box((*this), saveManager_, boxIndex);
}

uint8_t Gen1GameReader::getCurrentBoxIndex()
{
    uint8_t byte;

    saveManager_.seek(0x284C);
    saveManager_.readByte(byte);

    return byte & 0x3F;   
}

bool Gen1GameReader::getPokedexFlag(PokedexFlag dexFlag, uint8_t pokedexNumber) const
{
    const uint16_t saveOffset = (dexFlag == POKEDEX_SEEN)  ? 0x25B6 : 0x25A3;
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
    const uint16_t saveOffset = (dexFlag == POKEDEX_SEEN)  ? 0x25B6 : 0x25A3;
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
    const uint16_t saveOffset = (dexFlag == POKEDEX_SEEN)  ? 0x25B6 : 0x25A3;
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

/*
 * In gen 1, the behaviour of the party icons is a bit messy.
 * There's the MonPartySpritePointers of the various icon sprites. But while some of these sprites are stored fully,
 * for some only the left half is stored. (and the code is supposed to mirror this half while rendering)
 * In fact, for the latter, the top and bottom left tiles are separate entries in the table. This is not the case for the icons that are
 * stored fully.
 * 
 * There's also no real flag to indicate this.
 * 
 * 2 icons don't even have a second frame stored. As a matter of fact, the HELIX icon doesn't even have an entry in the table at all!
 * 
 * Anyway, we need to deal with this mess and that makes our code below a bit messy as well.
 */
uint8_t* Gen1GameReader::decodePokemonIcon(Gen1PokemonIconType iconType, SpriteRenderer& renderer, SpriteRenderer::OutputFormat outputFormat, bool firstFrame)
{
    const uint32_t romOffset = (gameType_ == Gen1GameType::YELLOW) ? 0x7184D : 0x717C0;
    const uint8_t MAX_NUM_TILES = 8;
    const uint8_t TILE_WIDTH = 8;
    const uint8_t TILE_HEIGHT = 8;
    const uint8_t BITS_PER_PIXEL = 2;
    const uint8_t BYTES_PER_TILE = ((TILE_WIDTH * TILE_HEIGHT) / 8) * BITS_PER_PIXEL;
    const uint32_t ENTRY_SIZE = 6;
    uint8_t iconSrcBuffer[MAX_NUM_TILES * BYTES_PER_TILE];
    uint16_t pointer;
    uint8_t numTiles; // total number of tiles of the sprite (to indicate byte size)
    uint8_t bankIndex;
    uint16_t vSpritesTileOffset;
    uint8_t entryIndex;
    // this boolean indicates that the sprite only stores the left half, but it's symmetric
    bool isSymmetric;

    switch(iconType)
    {
    case GEN1_ICONTYPE_MON:
        entryIndex = (firstFrame) ? 14 : 0;
        isSymmetric = false;
        break;
    case GEN1_ICONTYPE_BALL:
        entryIndex = (firstFrame) ? 1 : 0xFF;
        isSymmetric = false;
        break;
    case GEN1_ICONTYPE_FAIRY:
        entryIndex = (firstFrame) ? 16 : 2;
        isSymmetric = false;
        break;
    case GEN1_ICONTYPE_BIRD:
        entryIndex = (firstFrame) ? 17 : 3;
        isSymmetric = false;
        break;
    case GEN1_ICONTYPE_WATER:
        entryIndex = (firstFrame) ? 4 : 18;
        isSymmetric = false;
        break;
    case GEN1_ICONTYPE_BUG:
        entryIndex = (firstFrame) ? 19 : 5;
        isSymmetric = true;
        break;
    case GEN1_ICONTYPE_GRASS:
        entryIndex = (firstFrame) ? 21 : 7;
        isSymmetric = true;
        break;
    case GEN1_ICONTYPE_SNAKE:
        entryIndex = (firstFrame) ? 23 : 9;
        isSymmetric = true;
        break;
    case GEN1_ICONTYPE_QUADRUPED:
        entryIndex = (firstFrame) ? 25 : 11;
        isSymmetric = true;
        break;
    case GEN1_ICONTYPE_PIKACHU:
    {
        if(gameType_ != Gen1GameType::YELLOW)
        {
            entryIndex = 0xFF;
        }
        else
        {
            entryIndex = (firstFrame) ? 13 : 27;
        }
        isSymmetric = false;
        break;
    }
    default:
        entryIndex = 0xFF;
        isSymmetric = false;
        break;
    }

    // HACK: In pokemon yellow, there's an additional PIKACHU entry in the MonPartySpritePointers table.
    // This shifts every entryIndex by 1 after this new entry.
    // In practice, this means that mostly the second frame of icons are affected.
    // so we need to increase the entryIndex by one in this case.
    if(gameType_ == Gen1GameType::YELLOW && entryIndex != 0xFF && entryIndex > 13)
    {
        ++entryIndex;
    }

    if(entryIndex == 0xFF)
    {
        if(iconType == Gen1PokemonIconType::GEN1_ICONTYPE_HELIX && firstFrame)
        {
            // The helix sprite is not part of the MonPartySpritePointers table for some reason
            bankIndex = 4;
            pointer = (gameType_ == Gen1GameType::YELLOW) ? 0x7525 : 0x5180;
            numTiles = 4;
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        romReader_.seek(romOffset);
        romReader_.advance(ENTRY_SIZE * entryIndex);

        romReader_.readUint16(pointer);
        romReader_.readByte(numTiles);
        romReader_.readByte(bankIndex);
        romReader_.readUint16(vSpritesTileOffset);

        // HACK: so, in the case that only the left half of the icon is stored, every tile (top left + bottom left)
        // gets a separate entry in the table. But that's a bit annoying to work with. Since both tiles are stored
        // contiguously on the cartridge, let's just manipulate the numTiles field because we're going to render the tiles
        // separately anyway because we're aware that we're in this scenario.
        if(numTiles == 1 && isSymmetric)
        {
            numTiles = 2;
        }
    }

    romReader_.seekToRomPointer(pointer, bankIndex);
    romReader_.read(iconSrcBuffer, BYTES_PER_TILE * numTiles);

    if(isSymmetric)
    {
        // only the left half is stored. We're supposed to mirror it to get the right half.
        const uint8_t* srcTop = iconSrcBuffer;
        const uint8_t* srcBottom = iconSrcBuffer + BYTES_PER_TILE;
        // top left corner
        renderer.drawTile(srcTop, outputFormat, monochromeGBColorPalette, 0, 0, 2, false);
        // top right corner (mirrored)
        renderer.drawTile(srcTop, outputFormat, monochromeGBColorPalette, TILE_WIDTH, 0, 2, true);
        // bottom left corner
        renderer.drawTile(srcBottom, outputFormat, monochromeGBColorPalette, 0, TILE_HEIGHT, 2, false);
        // bottom right corner (mirrored)
        return renderer.drawTile(srcBottom, outputFormat, monochromeGBColorPalette, TILE_WIDTH, TILE_HEIGHT, 2, true);
    }
    else
    {
        // full sprite is stored in a horizontal tile order. So we can decode them this way.
        return renderer.draw(iconSrcBuffer, outputFormat, monochromeGBColorPalette, 2, 2, SpriteRenderer::TileOrder::HORIZONTAL);
    }
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
        for(uint8_t i = 0; i < 12; ++i)
        {
            Gen1Box box = getBox(i);
            if(box.getNumberOfPokemon() == box.getMaxNumberOfPokemon())
            {
                continue;
            }

            box.add(poke, originalTrainerID, nickname);
            result = i;

            updateWholeBoxBankChecksum(getGen1BoxBankIndex(i, currentBoxIndex));
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
    const uint16_t mainDataChecksumOffset = 0x3523;
    uint8_t storedChecksum;
    uint8_t calculatedChecksum;

    saveManager_.seek(mainDataChecksumOffset);
    saveManager_.readByte(storedChecksum);

    calculatedChecksum = calculateMainDataChecksum(saveManager_);

    return (storedChecksum == calculatedChecksum);
}

void Gen1GameReader::updateMainChecksum()
{
    const uint16_t mainDataChecksumOffset = 0x3523;
    const uint8_t calculatedChecksum = calculateMainDataChecksum(saveManager_);

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