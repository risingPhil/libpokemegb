#include "SpriteRenderer.h"
#include "common.h"

#include <cstdio>

using OutputFormat = SpriteRenderer::OutputFormat;

// The next set of functions are defined to remove the white background from a decoded RGBA pokémon sprite by doing edge detection.
// Both RGBA32 and RGBA16 formats are supported

/**
 * @brief Returns the byte offset for the pixel at the specified row and column in a RGBA buffer
 */
static uint32_t getRGBA32Offset(int row, int column, int spriteWidthInPixels)
{
    return row * spriteWidthInPixels * 4 + column * 4;
}

static uint32_t getRGBA16Offset(int row, int column, int spriteWidthInPixels)
{
    return row * spriteWidthInPixels * 2 + column * 2;   
}

/**
 * @brief Returns whether the pixel at the specified buffer offset is white
 */
static bool rgba32_is_white(uint8_t* buffer, uint32_t srcOffset)
{
    return ((*((uint32_t *)(buffer + srcOffset))) == 0xFFFFFFFF);
}

static bool rgba16_is_white(uint8_t* buffer, uint32_t srcOffset)
{
    const uint16_t colorVal = *((uint16_t *)(buffer + srcOffset));
    // generic white: 0xFFFF
    // gen 1 pokemon colorpalette white: 0xFF7F or 0xFFFD (after convertGBColorPaletteToRGBA16())
    return (colorVal == 0xFFFF || colorVal == 0xFF7F || colorVal == 0xFFFD);
}

/**
 * @brief Makes the pixel at the specified buffer offset transparent
 */
static void rgba32_make_transparent(uint8_t* buffer, uint32_t srcOffset)
{
    (*((uint32_t *)(buffer + srcOffset))) = 0;
}

/**
 * @brief Makes the pixel at the specified buffer offset transparent
 */
static void rgba16_make_transparent(uint8_t* buffer, uint32_t srcOffset)
{
    (*((uint16_t *)(buffer + srcOffset))) &= 0xFFFE;
}


/**
 * @brief returns the alpha component of the RGBA color at the specified buffer offset
 */
static uint8_t rgba32_alpha(uint8_t* buffer, uint32_t srcOffset)
{
    return buffer[srcOffset + 3];
}

/**
 * @brief returns the alpha component of the RGBA16 color at the specified buffer offset
 */
static uint8_t rgba16_alpha(uint8_t* buffer, uint32_t srcOffset)
{
    return (*((uint16_t *)(buffer + srcOffset))) & 0x1;
}

/**
 * @brief This function makes the pixel at the specified row and column index transparent if it's white
 */
static bool transformWhiteBackgroundPixel(uint8_t *buffer, OutputFormat format, int row, int column, int spriteWidthInPixels)
{
    uint32_t srcOffset;
    uint8_t alpha;
    bool white;
    
    switch(format)
    {
    case OutputFormat::RGBA16:
        srcOffset = getRGBA16Offset(row, column, spriteWidthInPixels);
        alpha = rgba16_alpha(buffer, srcOffset);
        white = rgba16_is_white(buffer, srcOffset);
        break;
    case OutputFormat::RGBA32:
        srcOffset = getRGBA32Offset(row, column, spriteWidthInPixels);
        alpha = rgba32_alpha(buffer, srcOffset);
        white = rgba32_is_white(buffer, srcOffset);
        break;
    default:
        return false;
    }

    if (alpha == 0)
    {
        return true;
    }

    if (white)
    {
        // make transparent
        if(format == OutputFormat::RGBA32)
        {
            rgba32_make_transparent(buffer, srcOffset);
        }
        else
        {
            rgba16_make_transparent(buffer, srcOffset);
        }
        return true;
    }
    else
    {
        // found edge. break inner loop and go to the next row
        return false;
    }
}

/**
 * @brief This function will make the pixel at the given row and column index and its white neighbours transparent recursively.
 */
static void removeBackground_Phase2_MakeNeighbourPixelsTransparent(uint8_t *buffer, OutputFormat format, int row, int column, int spriteWidthInPixels, int spriteHeightInPixels)
{
    uint32_t srcOffset;
    uint32_t neighbourOffset;
    uint8_t alpha;
    bool white;
    
    if(format == OutputFormat::RGBA32)
    {
        srcOffset = getRGBA32Offset(row, column, spriteWidthInPixels);
        alpha = rgba32_alpha(buffer, srcOffset);
    }
    else
    {
        srcOffset = getRGBA16Offset(row, column, spriteWidthInPixels);
        alpha = rgba16_alpha(buffer, srcOffset);
    }
    if (alpha == 0)
    {
        return;
    }

    // make transparent
    if(format == OutputFormat::RGBA32)
    {
        rgba32_make_transparent(buffer, srcOffset);
    }
    else
    {
        rgba16_make_transparent(buffer, srcOffset);
    }

    // now start making neighbouring white pixels transparant recursively
    // top neighbour
    if (row)
    {
        if(format == OutputFormat::RGBA32)
        {
            neighbourOffset = getRGBA32Offset(row - 1, column, spriteWidthInPixels);
            white = rgba32_is_white(buffer, neighbourOffset);
        }
        else
        {
            neighbourOffset = getRGBA16Offset(row - 1, column, spriteWidthInPixels);
            white = rgba16_is_white(buffer, neighbourOffset);
        }
        if (white)
        {
            removeBackground_Phase2_MakeNeighbourPixelsTransparent(buffer, format, row - 1, column, spriteWidthInPixels, spriteHeightInPixels);
        }
    }

    // right neighbour
    if (column < spriteWidthInPixels - 1)
    {
        if(format == OutputFormat::RGBA32)
        {
            neighbourOffset = getRGBA32Offset(row, column + 1, spriteWidthInPixels);
            white = rgba32_is_white(buffer, neighbourOffset);
        }
        else
        {
            neighbourOffset = getRGBA16Offset(row, column + 1, spriteWidthInPixels);
            white = rgba16_is_white(buffer, neighbourOffset);
        }
        if (white)
        {
            removeBackground_Phase2_MakeNeighbourPixelsTransparent(buffer, format, row, column + 1, spriteWidthInPixels, spriteHeightInPixels);
        }
    }

    // bottom neighbour
    if (row < spriteHeightInPixels - 1)
    {
        if(format == OutputFormat::RGBA32)
        {
            neighbourOffset = getRGBA32Offset(row + 1, column, spriteWidthInPixels);
            white = rgba32_is_white(buffer, neighbourOffset);
        }
        else
        {
            neighbourOffset = getRGBA16Offset(row + 1, column, spriteWidthInPixels);
            white = rgba16_is_white(buffer, neighbourOffset);
        }

        if (white)
        {
            removeBackground_Phase2_MakeNeighbourPixelsTransparent(buffer, format, row + 1, column, spriteWidthInPixels, spriteHeightInPixels);
        }
    }

    // left neighbour
    if (column)
    {
        if(format == OutputFormat::RGBA32)
        {
            neighbourOffset = getRGBA32Offset(row, column - 1, spriteWidthInPixels);
            white = rgba32_is_white(buffer, neighbourOffset);
        }
        else
        {
            neighbourOffset = getRGBA16Offset(row, column - 1, spriteWidthInPixels);
            white = rgba16_is_white(buffer, neighbourOffset);
        }

        if (white)
        {
            removeBackground_Phase2_MakeNeighbourPixelsTransparent(buffer, format, row, column - 1, spriteWidthInPixels, spriteHeightInPixels);
        }
    }
}

/**
 * @brief This function is called whenever we detect a white pixel after removing the white background from every edge.
 * We will check the pixel neighbours to figure out whether we can connect this pixel with a transparent one.
 * If we can find a transparent pixel in our direct neighbouring pixels, we'll turn this pixel and all its white neighbours transparant (using removeBackground_Phase2_MakeNeighbourPixelsTransparent).
 */
static void removeBackground_Phase2_dealWithWhitePixel(uint8_t *buffer, OutputFormat format, int row, int column, int spriteWidthInPixels, int spriteHeightInPixels)
{
    uint32_t neighBourOffset;
    bool foundTransparentPixel = false;
    uint8_t alpha;
    // allright, we currently have a white pixel.
    // now we're going to search if we can find a transparent pixel around this one.
    // if we can find one, we transform any white pixels around us recursively. If we just find a non-white, non-transparent pixel, we'll ignore it

    // check alpha value of the left neighbour (if any)
    if (column)
    {
        if(format == OutputFormat::RGBA32)
        {
            neighBourOffset = getRGBA32Offset(row, column - 1, spriteWidthInPixels);
            alpha = rgba32_alpha(buffer, neighBourOffset);
        }
        else
        {
            neighBourOffset = getRGBA16Offset(row, column - 1, spriteWidthInPixels);
            alpha = rgba16_alpha(buffer, neighBourOffset);
        }
        if (alpha == 0)
        {
            foundTransparentPixel = true;
        }
    }

    // now check the alpha value of the bottom neighbour (if any)
    if (!foundTransparentPixel && row < spriteHeightInPixels)
    {
        if(format == OutputFormat::RGBA32)
        {
            neighBourOffset = getRGBA32Offset(row + 1, column, spriteWidthInPixels);
            alpha = rgba32_alpha(buffer, neighBourOffset);
        }
        else
        {
            neighBourOffset = getRGBA16Offset(row + 1, column, spriteWidthInPixels);
            alpha = rgba16_alpha(buffer, neighBourOffset);
        }
        if (alpha == 0)
        {
            foundTransparentPixel = true;
        }
    }

    // now check the alpha value of the right neighbour (if any)
    if (!foundTransparentPixel && column < spriteWidthInPixels - 1)
    {
        if(format == OutputFormat::RGBA32)
        {
            neighBourOffset = getRGBA32Offset(row, column + 1, spriteWidthInPixels);
            alpha = rgba32_alpha(buffer, neighBourOffset);
        }
        else
        {
            neighBourOffset = getRGBA16Offset(row, column + 1, spriteWidthInPixels);
            alpha = rgba16_alpha(buffer, neighBourOffset);
        }

        if (alpha == 0)
        {
            foundTransparentPixel = true;
        }
    }

    // now check the alpha value of the top neighbour (if any)
    if(!foundTransparentPixel && row)
    {
        if(format == OutputFormat::RGBA32)
        {
            neighBourOffset = getRGBA32Offset(row - 1, column, spriteWidthInPixels);
            alpha = rgba32_alpha(buffer, neighBourOffset);
        }
        else
        {
            neighBourOffset = getRGBA16Offset(row - 1, column, spriteWidthInPixels);
            alpha = rgba16_alpha(buffer, neighBourOffset);
        }

        if (alpha == 0)
        {
            foundTransparentPixel = true;
        }
    }

    // allright, we found at least 1 transparent neighbour.
    // we should turn the current pixel and its white neighbours transparent
    if (foundTransparentPixel)
    {
        removeBackground_Phase2_MakeNeighbourPixelsTransparent(buffer, format, row, column, spriteWidthInPixels, spriteHeightInPixels);
    }
}

/**
 * @brief This function implements an algorithm to remove the white background from a pokémon sprite.
 * 
 * There are 2 phases:
 * - scan from left-to-right, right-to-left, top-to-bottom, bottom-to-top and remove every white pixel until we hit a non-white one
 * - for the remaining white pixels, try to connect them to a transparent pixel using the pixel neighbours. If we find a transparent neighbour,
 * turn the white pixel and its white neighbours transparent recursively until all the connected white pixels are transparent.
 */
static void removeBackground(uint8_t *buffer, OutputFormat format, int spriteWidthInPixels, int spriteHeightInPixels)
{
    uint32_t srcOffset;
    int row;
    int column;
    bool white;
    
    // edge detection: scan from left-to-right, right-to-left, top-to-bottom, bottom-to-top on a half image each.
    // keep removing white pixels until you encounter a non-white pixel (if you do, move to the next row/column)
    // let's start with horizontal left-to-right.
    for (row = 0; row < spriteHeightInPixels; ++row)
    {
        for (column = 0; column < spriteWidthInPixels / 2; ++column)
        {
            if (!transformWhiteBackgroundPixel(buffer, format, row, column, spriteWidthInPixels))
            {
                // found edge. break inner loop and go to the next row
                break;
            }
        }
    }

    // now do the same, but right to left (right half)
    for (row = 0; row < spriteHeightInPixels; ++row)
    {
        for (column = spriteWidthInPixels - 1; column >= spriteWidthInPixels / 2; --column)
        {
            if (!transformWhiteBackgroundPixel(buffer, format, row, column, spriteWidthInPixels))
            {
                // found edge. break inner loop and go to the next row
                break;
            }
        }
    }

    // now do the same, but top to bottom, left to right (top half)
    for (column = 0; column < spriteWidthInPixels; ++column)
    {
        for (row = 0; row < spriteHeightInPixels / 2; ++row)
        {
            if (!transformWhiteBackgroundPixel(buffer, format, row, column, spriteWidthInPixels))
            {
                // found edge. break inner loop and go to the next row
                break;
            }
        }
    }

    // now do the same, bottom to top, left to right (bottom half)
    for (column = 0; column < spriteWidthInPixels; ++column)
    {
        for (row = spriteHeightInPixels - 1; row >= spriteHeightInPixels / 2; --row)
        {
            if (!transformWhiteBackgroundPixel(buffer, format, row, column, spriteWidthInPixels))
            {
                // found edge. break inner loop and go to the next row
                break;
            }
        }
    }

    // allright, now comes the most difficult part: let's go over the entire buffer
    // and when we find white pixels (which are either inside the sprite (belly? eyes?) or between 2 edges of a sprite (between hand and foot?))
    // we try to find either a transparent pixel in our neighbour pixels or an edge
    // we take into account that we are navigating from top to bottom, left to right
    // if it's still white, the alpha will always be set to 0xFF always, so we don't particularly care about reading the alpha
    // now do the same, but top to bottom, left to right
    for (column = 0; column < spriteWidthInPixels; ++column)
    {
        for (row = 0; row < spriteHeightInPixels; ++row)
        {
            if(format == OutputFormat::RGBA32)
            {
                srcOffset = getRGBA32Offset(row, column, spriteWidthInPixels);
                white = rgba32_is_white(buffer, srcOffset);
            }
            else
            {
                srcOffset = getRGBA16Offset(row, column, spriteWidthInPixels);
                white = rgba16_is_white(buffer, srcOffset);
            }

            if (white)
            {
                removeBackground_Phase2_dealWithWhitePixel(buffer, format, row, column, spriteWidthInPixels, spriteHeightInPixels);
            }
        }
    }
}

// end of the removeBackground specific code

uint32_t SpriteRenderer::getNumRGBBytesFor(uint8_t numHTiles, uint8_t numVTiles) const
{
    const uint16_t numPixels = (numHTiles * 8) * (numVTiles * 8);
    return numPixels * 3;
}

uint8_t* SpriteRenderer::draw(const uint8_t* spriteBuffer, OutputFormat format, uint16_t palette[4], uint8_t numHTiles, uint8_t numVTiles, bool removeWhiteBackground)
{
    const uint8_t* rgbPalette;
    const uint16_t* rgba16Palette;
    uint8_t byte0;
    uint8_t byte1;
    uint16_t x;
    uint16_t y;
    uint8_t numColorComponents;

    // convert -size 56x56 -depth 8 rgb:sprite.rgb output.png

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
    // every tile is 8x8
    const uint16_t spriteBufferHeightInPixels = numVTiles * 8;
    const uint16_t spriteBufferWidthInPixels = numHTiles * 8;
    const uint16_t spriteBufferSize = spriteBufferWidthInPixels * spriteBufferHeightInPixels / 8;

    switch(format)
    {
        case OutputFormat::RGB:
            rgbPalette = convertGBColorPaletteToRGB24(palette);
            rgba16Palette = nullptr;
            numColorComponents = 3;
            break;
        case OutputFormat::RGBA32:
            rgbPalette = convertGBColorPaletteToRGB24(palette);
            rgba16Palette = nullptr;
            numColorComponents = 4;
            break;
        case OutputFormat::RGBA16:
            rgbPalette = nullptr;
            rgba16Palette = convertGBColorPaletteToRGBA16(palette);
            numColorComponents = 2;
            break;
        default:
            return nullptr;
    }

    while(i < spriteBufferSize)
    {
        byte0 = spriteBuffer[i * 2];
        byte1 = spriteBuffer[i * 2 + 1];

//      printf("0x%hhu 0x%hhu\n", byte0, byte1);

        x = (i / (spriteBufferHeightInPixels)) * 8;
        y = (i % (spriteBufferHeightInPixels));

        for(uint8_t bitIndex = 0; bitIndex < 8; ++bitIndex)
        {
            offset = (y * spriteBufferWidthInPixels + x + bitIndex) * numColorComponents;

            bit0 = (byte0 >> (7 - bitIndex)) & 0x1;
            bit1 = (byte1 >> (7 - bitIndex)) & 0x1;

            paletteIndex = (bit1 << 1) | bit0;

            if(format == OutputFormat::RGBA16)
            {
                (*((uint16_t*)(buffer_ + offset))) = rgba16Palette[paletteIndex];
            }
            else
            {
                // R
                buffer_[offset] = rgbPalette[paletteIndex * 3 + 0];
                // G
                buffer_[offset + 1] = rgbPalette[paletteIndex * 3 + 1];
                // B
                buffer_[offset + 2] = rgbPalette[paletteIndex * 3 + 2];

                if(format == OutputFormat::RGBA32)
                {
                    // A
                    buffer_[offset + 3] = 0xFF;
                }
            }
        }
        ++i;
    }

    if(removeWhiteBackground)
    {
        if(format == OutputFormat::RGB)
        {
            fprintf(stderr, "[SpriteRenderer]: %s: ERROR: removeBackground is not supported for OutputFormat::RGB!", __FUNCTION__);
            return buffer_;
        }

        removeBackground(buffer_, format, spriteBufferWidthInPixels, spriteBufferHeightInPixels);
    }

    return buffer_;
}