#ifndef _SPRITERENDERER_H
#define _SPRITERENDERER_H

#include <cstdint>

#define MAX_SPRITE_TILES_WIDTH 7
#define MAX_SPRITE_PIXEL_WIDTH MAX_SPRITE_TILES_WIDTH * 8
#define MAX_SPRITE_TILES_HEIGHT 7
#define MAX_SPRITE_PIXEL_HEIGHT MAX_SPRITE_TILES_HEIGHT * 8

class SpriteRenderer
{
public:
    /**
     * Returns the number of RGB bytes to store RGB values for a sprite with the given number of horizontal and vertical tiles
     */
    uint32_t getNumRGBBytesFor(uint8_t numHTiles, uint8_t numVTiles) const;

    /**
     * @brief This function draw the given spriteBuffer to the internal rgbBuffer_ and returns this rgbBuffer_ pointer
     * 
     * NOTE: the next call to this function overwrites the data inside the returned buffer. So if you want to keep this data around, copy it.
     * You don't need to free() the data.
     */
    uint8_t* drawRGB(const uint8_t* spriteBuffer, uint16_t palette[4], uint8_t numHTiles, uint8_t numVTiles);
protected:
private:
    uint8_t rgbBuffer_[MAX_SPRITE_PIXEL_WIDTH * MAX_SPRITE_PIXEL_HEIGHT * 3];
};

#endif