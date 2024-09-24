#ifndef _SPRITERENDERER_H
#define _SPRITERENDERER_H

#include <cstdint>

#define MAX_SPRITE_TILES_WIDTH 7
#define MAX_SPRITE_PIXEL_WIDTH MAX_SPRITE_TILES_WIDTH * 8
#define MAX_SPRITE_TILES_HEIGHT 7
#define MAX_SPRITE_PIXEL_HEIGHT MAX_SPRITE_TILES_HEIGHT * 8
#define MAX_NUM_BYTES_PER_COLOR_COMPONENT 4

class SpriteRenderer
{
public:

    enum class OutputFormat
    {
        RGB,
        RGBA16,
        RGBA32
    };

    /**
     * @brief This enum indicates the order the tiles appear in the input data
     */
    enum class TileOrder
    {
        HORIZONTAL,
        VERTICAL
    };

    SpriteRenderer();

    /**
     * Returns the number of RGB bytes to store RGB values for a sprite with the given number of horizontal and vertical tiles
     */
    uint32_t getNumRGBBytesFor(uint8_t numHTiles, uint8_t numVTiles) const;

    /**
     * @brief This function draw the given spriteBuffer to the internal rgbBuffer_ and returns this rgbBuffer_ pointer
     * 
     * NOTE: the next call to this function or drawTile() overwrites the data inside the returned buffer. So if you want to keep this data around, copy it.
     * You don't need to free() the data.
     */
    uint8_t* draw(const uint8_t* spriteBuffer, OutputFormat format, uint16_t palette[4], uint8_t numHTiles, uint8_t numVTiles, TileOrder tileOrder = TileOrder::VERTICAL);

    /**
     * @brief This function draws the given buffer that supposedly contains a tile (16 bytes at 2BPP) to the internal buffer_ at the given [xOffset, yOffset] and 
     * returns this internal buffer_
     * 
     * NOTE: the next call to this function or draw() overwrites the data inside the returned buffer. So if you want to keep this data around, copy it.
     * You don't need to free() the data.
     */
    uint8_t* drawTile(const uint8_t* tileBuffer, OutputFormat format, uint16_t palette[4], uint16_t xOffset, uint16_t yOffset, uint8_t outputBufferHTiles, bool mirrorHorizontally = false);

    /**
     * @brief This function removes the white background in the current internal buffer_ and returns this buffer
     * 
     * NOTE: the next call to draw() or drawTile() overwrites the data inside the returned buffer. So if you want to keep this data around, copy it.
     * You don't need to free() the data.
     */
    uint8_t* removeWhiteBackground(uint8_t numHTiles, uint8_t numVTiles);
protected:
private:
    void internalDrawTile(const uint8_t* tileBuffer, uint16_t xOffset, uint16_t yOffset, uint16_t spriteBufferWidthInPixels, bool mirrorHorizontally);

    void setFormatAndPalette(OutputFormat format, uint16_t palette[4]);

    uint8_t buffer_[MAX_SPRITE_PIXEL_WIDTH * MAX_SPRITE_PIXEL_HEIGHT * MAX_NUM_BYTES_PER_COLOR_COMPONENT];
    OutputFormat format_;
    const uint8_t* rgbPalette_;
    const uint16_t* rgba16Palette_;
    uint8_t numColorComponents_;
};

#endif