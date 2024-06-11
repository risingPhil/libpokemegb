#include "SpriteRenderer.h"
#include "common.h"

uint32_t SpriteRenderer::getNumRGBBytesFor(uint8_t numHTiles, uint8_t numVTiles) const
{
    const uint16_t numPixels = (numHTiles * 8) * (numVTiles * 8);
    return numPixels * 3;
}

uint8_t* SpriteRenderer::drawRGB(const uint8_t* spriteBuffer, uint16_t palette[4], uint8_t numHTiles, uint8_t numVTiles)
{
    uint8_t byte0;
    uint8_t byte1;
    uint16_t x;
    uint16_t y;

    // convert -size 56x56 -depth 8 rgb:sprite.rgb output.png

    const uint8_t* rgbPalette = convertGBColorPaletteToRGB24(palette);

    // We're working with a gameboy specific format here:
    // every 2 bytes contain the bits for 8 horizontal pixels
    // the first of those 2 bytes contain the least significant bits for every of those 8 horizontal pixels
    // the second of those 2 bytes contain the most significant bits for every of those 8 horizontal pixels.
    // however, the data is stored in column first order: that means that we're walking through the sprite vertically.
    // this is a bit confusing, but the gist of it is: you get 8 horizontal pixels (=2 bytes), then you move to the next vertical row.
    // then you get another 8 horizontal pixels of that row. And then you move again to the next vertical row.
    // you keep doing this until you reach the bottom of the sprite, after which the next column begins. And there you repeat the process

    uint32_t i = 0;
    uint32_t offset;
    uint8_t paletteIndex;
    uint8_t bit0;
    uint8_t bit1;
    uint8_t r, g, b;
    // every tile is 8x8
    const uint16_t spriteBufferHeightInPixels = numVTiles * 8;
    const uint16_t spriteBufferWidthInPixels = numHTiles * 8;
    const uint16_t spriteBufferSize = spriteBufferWidthInPixels * spriteBufferHeightInPixels / 8;
    while(i < spriteBufferSize)
    {
        byte0 = spriteBuffer[i * 2];
        byte1 = spriteBuffer[i * 2 + 1];

//      printf("0x%hhu 0x%hhu\n", byte0, byte1);

        x = (i / (spriteBufferHeightInPixels)) * 8;
        y = (i % (spriteBufferHeightInPixels));

        for(uint8_t bitIndex = 0; bitIndex < 8; ++bitIndex)
        {
            offset = (y * spriteBufferWidthInPixels + x + bitIndex) * 3;

            bit0 = (byte0 >> (7 - bitIndex)) & 0x1;
            bit1 = (byte1 >> (7 - bitIndex)) & 0x1;

            paletteIndex = (bit1 << 1) | bit0;

            r = rgbPalette[paletteIndex * 3 + 0];
            g = rgbPalette[paletteIndex * 3 + 1];
            b = rgbPalette[paletteIndex * 3 + 2];

            rgbBuffer_[offset] = r;
            rgbBuffer_[offset + 1] = g;
            rgbBuffer_[offset + 2] = b;
        }
        ++i;
    }
    return rgbBuffer_;
}