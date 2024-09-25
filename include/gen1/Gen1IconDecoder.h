#ifndef _GEN1POKEMONICONDECODER_H
#define _GEN1POKEMONICONDECODER_H

#include "gen1/Gen1Common.h"

#define GEN1_TILE_BITS_PER_PIXEL 2
#define GEN1_BYTES_PER_TILE 16
#define GEN1_ICON_WIDTH_IN_TILES 2
#define GEN1_ICON_HEIGHT_IN_TILES 2
#define GEN1_ICON_NUM_BYTES GEN1_BYTES_PER_TILE * 4

class IRomReader;

/**
 * @brief This class is responsible for decoding pokemon party menu icons for Gen 1 games
 */
class Gen1IconDecoder
{
public:
    Gen1IconDecoder(IRomReader& romReader, Gen1GameType gameType);

    /**
     * @brief Decodes the specified icon type. either frame 1 or frame 2.
     * This function returns a buffer containing gameboy tiles.
     * 
     * The result is supposed to be fed to an instance of the SpriteRenderer class to convert it into an actual usable format
     * 
     * @return Internal buffer containing the icon tiles in vertical tile order.
     */
    uint8_t* decode(Gen1PokemonIconType iconType, bool firstFrame);
protected:
private:
    uint8_t buffer_[GEN1_ICON_NUM_BYTES];
    IRomReader& romReader_;
    Gen1GameType gameType_;
};

#endif