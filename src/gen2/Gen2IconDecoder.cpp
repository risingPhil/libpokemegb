#include "gen2/Gen2IconDecoder.h"
#include "RomReader.h"

#include <cstring>

Gen2IconDecoder::Gen2IconDecoder(IRomReader& romReader, Gen2GameType gameType)
    : buffer_()
    , romReader_(romReader)
    , gameType_(gameType)
{
}

uint8_t* Gen2IconDecoder::decode(Gen2LocalizationLanguage language, Gen2PokemonIconType iconType, bool firstFrame)
{
    const uint32_t romOffset = gen2_getRomOffsets(gameType_, language).icons;
    const uint8_t MAX_NUM_TILES = 4;
    const uint8_t TILE_WIDTH = 8;
    const uint8_t TILE_HEIGHT = 8;
    const uint8_t BITS_PER_PIXEL = 2;
    const uint8_t BYTES_PER_TILE = ((TILE_WIDTH * TILE_HEIGHT) / 8) * BITS_PER_PIXEL;
    const uint8_t TILES_PER_ICON = 4;
    const uint32_t ENTRY_SIZE = 2;
    uint8_t iconSrcBuffer[MAX_NUM_TILES * BYTES_PER_TILE];
    const uint8_t bankIndex = 0x23;
    uint16_t pointer;

    if(!romOffset)
    {
        return buffer_;
    }

    // read IconPointers table entry
    romReader_.seek(romOffset);
    romReader_.advance(((uint32_t)iconType) * ENTRY_SIZE);
    romReader_.readUint16(pointer);

    // now jump to the pointer to find the sprite
    romReader_.seekToRomPointer(pointer, bankIndex);

    // every sprite contains 2 frames.
    // if we want the second one, we need to skip 1 16x16 sprite
    if(!firstFrame)
    {
        romReader_.advance(TILES_PER_ICON * BYTES_PER_TILE);
    }

    romReader_.read(iconSrcBuffer, TILES_PER_ICON * BYTES_PER_TILE);

    // even though we read the entire icon at once, they are stored in the rom/cartridge in horizontal tile order.
    // But WE want to return the data in vertical order
    // (this keeps rendering with SpriteRenderer simple)
    // so we need to reorder the tiles from horizontal to vertical
    memcpy(buffer_, iconSrcBuffer, GEN2_BYTES_PER_TILE);
    memcpy(buffer_ + GEN2_BYTES_PER_TILE, iconSrcBuffer + (2 * GEN2_BYTES_PER_TILE), GEN2_BYTES_PER_TILE);
    memcpy(buffer_ + (2 * GEN2_BYTES_PER_TILE), iconSrcBuffer + GEN2_BYTES_PER_TILE, GEN2_BYTES_PER_TILE);
    memcpy(buffer_ + (3 * GEN2_BYTES_PER_TILE), iconSrcBuffer + (3 * GEN2_BYTES_PER_TILE), GEN2_BYTES_PER_TILE);

    return buffer_;
}