#ifndef _GEN2ICONDECODER_H
#define _GEN2ICONDECODER_H

#include "gen2/Gen2Common.h"

#define GEN2_TILE_BITS_PER_PIXEL 2
#define GEN2_BYTES_PER_TILE 16
#define GEN2_ICON_WIDTH_IN_TILES 2
#define GEN2_ICON_HEIGHT_IN_TILES 2
#define GEN2_ICON_NUM_BYTES GEN2_BYTES_PER_TILE * 4

class IRomReader;

/**
 * @brief This class is responsible for decoding pokemon party menu icons for Gen 2 games
 */
class Gen2IconDecoder
{
public:
    Gen2IconDecoder(IRomReader& romReader, Gen2GameType gameType);

    /**
     * @brief Decodes the specified icon type. either frame 1 or frame 2.
     * This function returns a buffer containing gameboy tiles.
     * 
     * The result is supposed to be fed to an instance of the SpriteRenderer class to convert it into an actual usable format
     * 
     * @return Internal buffer containing the icon tiles in vertical tile order.
     */
    uint8_t* decode(Gen2PokemonIconType iconType, bool firstFrame);
protected:
private:
    uint8_t buffer_[GEN2_ICON_NUM_BYTES];
    IRomReader& romReader_;
    Gen2GameType gameType_;
};

#endif