#include "gen1/Gen1IconDecoder.h"
#include "RomReader.h"

#include <cstring>

static uint8_t reverseByte(uint8_t incoming)
{
    uint8_t result = 0;
    uint8_t maskIncoming = 0x80;
    uint8_t flagOutgoing = 1;
    while(maskIncoming)
    {
        if(incoming & maskIncoming)
        {
            result |= flagOutgoing;
        }
        maskIncoming >>= 1;
        flagOutgoing <<= 1;
    }
    return result;
}

Gen1IconDecoder::Gen1IconDecoder(IRomReader& romReader, Gen1GameType gameType)
    : buffer_()
    , romReader_(romReader)
    , gameType_(gameType)
{

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
uint8_t* Gen1IconDecoder::decode(Gen1LocalizationLanguage language, Gen1PokemonIconType iconType, bool firstFrame)
{
    const uint32_t romOffset = gen1_getRomOffsets(gameType_, language).icons;
    const uint32_t ENTRY_SIZE = 6;
    const uint8_t MAX_NUM_TILES = 8;

    uint8_t iconSrcBuffer[GEN1_BYTES_PER_TILE * MAX_NUM_TILES];
    uint16_t pointer;
    uint16_t vSpritesTileOffset;
    uint8_t numTiles; // total number of tiles of the sprite (to indicate byte size)
    uint8_t bankIndex;
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
    romReader_.read(iconSrcBuffer, GEN1_BYTES_PER_TILE * numTiles);

    if(isSymmetric)
    {
        const uint8_t numRowBytes = GEN1_BYTES_PER_TILE * GEN1_ICON_HEIGHT_IN_TILES;
        const uint8_t* const dataEnd = iconSrcBuffer + numRowBytes;
        const uint8_t* cur = iconSrcBuffer;
        uint8_t* outCur = buffer_ + numRowBytes;
        // in this scenario, the tiles are stored in vertical order, but only the left half (2 tiles)
        // for the right half, we're supposed to mirror the bytes
        memcpy(buffer_, iconSrcBuffer, numRowBytes);

        while(cur < dataEnd)
        {
            (*outCur) = reverseByte((*cur));

            ++cur;
            ++outCur;
        }
    }
    else
    {
        // even though we read the entire icon at once, they are stored in the rom/cartridge in horizontal tile order.
        // But WE want to return the data in vertical order
        // (this keeps rendering with SpriteRenderer simple)
        // so we need to reorder the tiles from horizontal to vertical
        memcpy(buffer_, iconSrcBuffer, GEN1_BYTES_PER_TILE);
        memcpy(buffer_ + GEN1_BYTES_PER_TILE, iconSrcBuffer + (2 * GEN1_BYTES_PER_TILE), GEN1_BYTES_PER_TILE);
        memcpy(buffer_ + (2 * GEN1_BYTES_PER_TILE), iconSrcBuffer + GEN1_BYTES_PER_TILE, GEN1_BYTES_PER_TILE);
        memcpy(buffer_ + (3 * GEN1_BYTES_PER_TILE), iconSrcBuffer + (3 * GEN1_BYTES_PER_TILE), GEN1_BYTES_PER_TILE);
    }

    return buffer_;
}