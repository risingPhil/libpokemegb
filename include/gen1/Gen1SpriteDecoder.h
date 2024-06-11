#ifndef GEN1SPRITEDECODER_H
#define GEN1SPRITEDECODER_H

#include <cstdint>

#define SPRITE_BUFFER_WIDTH_IN_PIXELS 56
#define SPRITE_BUFFER_HEIGHT_IN_PIXELS 56
#define SPRITE_BITPLANE_BUFFER_SIZE_IN_BYTES ((SPRITE_BUFFER_WIDTH_IN_PIXELS * SPRITE_BUFFER_HEIGHT_IN_PIXELS) / 8)
#define GEN1_DECODED_SPRITE_BUFFER_SIZE_IN_BYTES SPRITE_BITPLANE_BUFFER_SIZE_IN_BYTES * 2

class IRomReader;

/**
 * @brief based on https://www.youtube.com/watch?v=aF1Yw_wu2cM and https://www.youtube.com/watch?v=ZI50XUeN6QE
 * 
 */
class Gen1SpriteDecoder
{
public:
    Gen1SpriteDecoder(IRomReader& romReader);
    ~Gen1SpriteDecoder();

    /**
     * @brief This function decodes the sprite at the specified bank and pointer
     * and returns a pointer to the decoded sprite buffer.
     * 
     * NOTE: this decoded sprite may or may not be what you expect. The format of it is described here:
     * https://rgmechex.com/tech/gen1decompress.html and as the function level comment of mergeSpriteBitplanes() in the .cpp file
     * 
     * Careful: the returned buffer is reused/recycled on the next call to decode().
     * So if you want to keep the data around after the next decode() call, you should make a copy of it.
     * 
     * The size of the returned buffer is SPRITE_BITPLANE_BUFFER_SIZE_IN_BYTES * 2
     */
    uint8_t* decode(uint8_t bankIndex, uint16_t pointer);
protected:
private:
    IRomReader& romReader_;
    // 392 bytes per buffer (56x56 pixels at 1 bit per pixel) (1 tile is 8x8)
    uint8_t bigSpriteBuffer_[SPRITE_BITPLANE_BUFFER_SIZE_IN_BYTES * 3];
};

#endif