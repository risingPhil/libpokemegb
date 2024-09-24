#include "SpriteRenderer.h"
#include "common.h"

#include <cstdio>

using OutputFormat = SpriteRenderer::OutputFormat;

static const uint8_t TILE_WIDTH = 8;
static const uint8_t TILE_HEIGHT = 8;
static const uint8_t BITS_PER_PIXEL = 2;
static const uint8_t BYTES_PER_TILE = ((TILE_WIDTH * TILE_HEIGHT) / 8) * BITS_PER_PIXEL;

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
    const uint32_t colorVal = *((uint32_t *)(buffer + srcOffset));
    // generic white: 0xFFFFFFFF
    // gen 1 pokemon colorpalette white: 0xFFEEFFFF or 0xFFF6FFFF (after convertGBColorPaletteToRGBA24() and adding alpha)
    return (colorVal == 0xFFFFFFFF || (colorVal == 0xFFFFEEFF) || (colorVal == 0xFFF6FFFF));
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

SpriteRenderer::SpriteRenderer()
    : buffer_()
    , format_(OutputFormat::RGB)
    , rgbPalette_(nullptr)
    , rgba16Palette_(nullptr)
    , numColorComponents_(0)
{
}

uint32_t SpriteRenderer::getNumRGBBytesFor(uint8_t numHTiles, uint8_t numVTiles) const
{
    const uint16_t numPixels = (numHTiles * 8) * (numVTiles * 8);
    return numPixels * 3;
}

uint8_t* SpriteRenderer::draw(const uint8_t* spriteBuffer, OutputFormat format, uint16_t palette[4], uint8_t numHTiles, uint8_t numVTiles, TileOrder tileOrder)
{
    const uint16_t spriteBufferHeightInPixels = numVTiles * 8;
    const uint16_t spriteBufferWidthInPixels = numHTiles * 8;
    const uint32_t spriteBufferSize = (spriteBufferWidthInPixels / 8) * spriteBufferHeightInPixels * BITS_PER_PIXEL;
    uint32_t i = 0;
    uint16_t x = 0;
    uint16_t y = 0;

    setFormatAndPalette(format, palette);

    while(i < spriteBufferSize)
    {
        internalDrawTile(spriteBuffer + i, x, y, spriteBufferWidthInPixels, false);

        if(tileOrder == TileOrder::VERTICAL)
        {
            y += 8;
            if(y >= spriteBufferHeightInPixels)
            {
                x += 8;
                y = 0;
            }
        }
        else
        {
            x += 8;
            if(x >= spriteBufferWidthInPixels)
            {
                x = 0;
                y += 8;
            }
        }

        i += BYTES_PER_TILE;
    }

    return buffer_;
}

uint8_t* SpriteRenderer::drawTile(const uint8_t* tileBuffer, OutputFormat format, uint16_t palette[4], uint16_t xOffset, uint16_t yOffset, uint8_t outputBufferHTiles, bool mirrorHorizontally)
{
    const uint16_t spriteBufferWidthInPixels = outputBufferHTiles * 8;

    setFormatAndPalette(format, palette);
    internalDrawTile(tileBuffer, xOffset, yOffset, spriteBufferWidthInPixels, mirrorHorizontally);

    return buffer_;
}

uint8_t* SpriteRenderer::removeWhiteBackground(uint8_t numHTiles, uint8_t numVTiles)
{
    if(format_ == OutputFormat::RGB)
    {
        fprintf(stderr, "[SpriteRenderer]: %s: ERROR: removeBackground is not supported for OutputFormat::RGB!", __FUNCTION__);
        return buffer_;
    }

    const uint16_t spriteBufferHeightInPixels = numVTiles * 8;
    const uint16_t spriteBufferWidthInPixels = numHTiles * 8;

    removeBackground(buffer_, format_, spriteBufferWidthInPixels, spriteBufferHeightInPixels);
    return buffer_;
}

void SpriteRenderer::internalDrawTile(const uint8_t* tileBuffer, uint16_t xOffset, uint16_t yOffset, uint16_t spriteBufferWidthInPixels, bool mirrorHorizontally)
{
    uint32_t offset;
    uint32_t i = 0;
    uint16_t x;
    uint16_t y = 0;
    uint8_t byte0;
    uint8_t byte1;
    uint8_t bit0;
    uint8_t bit1;
    uint8_t paletteIndex;

    while(i < BYTES_PER_TILE)
    {
        byte0 = tileBuffer[i];
        byte1 = tileBuffer[i + 1];
        
        for(x = 0; x < 8; ++x)
        {
            offset = ((yOffset + y) * spriteBufferWidthInPixels + xOffset + x) * numColorComponents_;

            if(!mirrorHorizontally)
            {
                bit0 = (byte0 >> (7 - x)) & 0x1;
                bit1 = (byte1 >> (7 - x)) & 0x1;
            }
            else
            {
                bit0 = (byte0 >> x) & 0x1;
                bit1 = (byte1 >> x) & 0x1;
            }

            paletteIndex = (bit1 << 1) | bit0;

            if(format_ == OutputFormat::RGBA16)
            {
                (*((uint16_t*)(buffer_ + offset))) = rgba16Palette_[paletteIndex];
            }
            else
            {
                // R
                buffer_[offset] = rgbPalette_[paletteIndex * 3 + 0];
                // G
                buffer_[offset + 1] = rgbPalette_[paletteIndex * 3 + 1];
                // B
                buffer_[offset + 2] = rgbPalette_[paletteIndex * 3 + 2];

                if(format_ == OutputFormat::RGBA32)
                {
                    // A
                    buffer_[offset + 3] = 0xFF;
                }
            }
        }
        ++y;

        i += 2;
    }
}

void SpriteRenderer::setFormatAndPalette(OutputFormat format, uint16_t palette[4])
{
    format_ = format;
    switch(format)
    {
        case OutputFormat::RGB:
            rgbPalette_ = convertGBColorPaletteToRGB24(palette);
            rgba16Palette_ = nullptr;
            numColorComponents_ = 3;
            break;
        case OutputFormat::RGBA32:
            rgbPalette_ = convertGBColorPaletteToRGB24(palette);
            rgba16Palette_ = nullptr;
            numColorComponents_ = 4;
            break;
        case OutputFormat::RGBA16:
            rgbPalette_ = nullptr;
            rgba16Palette_ = convertGBColorPaletteToRGBA16(palette);
            numColorComponents_ = 2;
            break;
        default:
            break;
    }
}