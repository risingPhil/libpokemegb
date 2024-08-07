#include "gen1/Gen1GameReader.h"
#include "gen2/Gen2GameReader.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "SpriteRenderer.h"
#include "utils.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

static bool g_writeSpriteBufferBinFile = false;
static bool g_removeBackground = false;

// The next set of functions are defined to remove the white background from a decoded RGB pokémon sprite by doing edge detection
// This is example code. I decided to not include it in the library because you'd need a separate -yet very similar- implementation
// for RGBA16 buffers and I didn't feel up to rewrite the code in such a way that the usage of RGBA32/RGBA16 can be abstracted. I also couldn't be bothered to add both to the lib.
// The code below is specific for RGBA32, NOT RGBA16.
// For PokeMe64, however, I'll need RGBA16.
// So... I just left this RGBA32 implementation here as an example, but I'll define a RGBA16 one inside of PokeMe64 separately.

/**
 * @brief Returns the byte offset for the pixel at the specified row and column in a RGBA buffer
 */
static uint32_t getRGBA32Offset(int row, int column, int spriteWidthInPixels)
{
    return row * spriteWidthInPixels * 4 + column * 4;
}

/**
 * @brief Returns whether the pixel at the specified buffer offset is white
 */
static bool rgba32_is_white(uint8_t* buffer, uint32_t srcOffset)
{
    return ((*(uint32_t *)(buffer + srcOffset)) == 0xFFFFFFFF);
}

/**
 * @brief Makes the pixel at the specified buffer offset transparent
 */
static void rgba32_make_transparent(uint8_t* buffer, uint32_t srcOffset)
{
    (*(uint32_t *)(buffer + srcOffset)) = 0;
}

/**
 * @brief returns the alpha component of the RGBA color at the specified buffer offset
 */
static uint8_t rgba32_alpha(uint8_t* buffer, uint32_t srcOffset)
{
    return buffer[srcOffset + 3];
}

/**
 * @brief This function makes the pixel at the specified row and column index transparent if it's white
 */
static bool transformWhiteBackgroundPixel(uint8_t *rgbaBuffer, int row, int column, int spriteWidthInPixels)
{
    const uint32_t srcOffset = getRGBA32Offset(row, column, spriteWidthInPixels);

    if (rgba32_alpha(rgbaBuffer, srcOffset) == 0)
    {
        return true;
    }

    if (rgba32_is_white(rgbaBuffer, srcOffset))
    {
        // make transparent
        rgba32_make_transparent(rgbaBuffer, srcOffset);
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
static void removeBackground_Phase2_MakeNeighbourPixelsTransparent(uint8_t *rgbaBuffer, int row, int column, int spriteWidthInPixels)
{
    const uint32_t srcOffset = getRGBA32Offset(row, column, spriteWidthInPixels);
    if (rgba32_alpha(rgbaBuffer, srcOffset) == 0)
    {
        return;
    }

    // make transparent
    rgba32_make_transparent(rgbaBuffer, srcOffset);

    // now start making neighbouring white pixels transparant recursively
    // top neighbour
    if (row)
    {
        const uint32_t neighbourOffset = getRGBA32Offset(row - 1, column, spriteWidthInPixels);
        if (rgba32_is_white(rgbaBuffer, neighbourOffset))
        {
            removeBackground_Phase2_MakeNeighbourPixelsTransparent(rgbaBuffer, row - 1, column, spriteWidthInPixels);
        }
    }

    // right neighbour
    if (column < SPRITE_BUFFER_WIDTH_IN_PIXELS - 1)
    {
        const uint32_t neighbourOffset = getRGBA32Offset(row, column + 1, spriteWidthInPixels);
        if (rgba32_is_white(rgbaBuffer, neighbourOffset))
        {
            removeBackground_Phase2_MakeNeighbourPixelsTransparent(rgbaBuffer, row, column + 1, spriteWidthInPixels);
        }
    }

    // bottom neighbour
    if (row < SPRITE_BUFFER_HEIGHT_IN_PIXELS - 1)
    {
        const uint32_t neighbourOffset = getRGBA32Offset(row + 1, column, spriteWidthInPixels);
        if (rgba32_is_white(rgbaBuffer, neighbourOffset))
        {
            removeBackground_Phase2_MakeNeighbourPixelsTransparent(rgbaBuffer, row + 1, column, spriteWidthInPixels);
        }
    }

    // left neighbour
    if (column)
    {
        const uint32_t neighbourOffset = getRGBA32Offset(row, column - 1, spriteWidthInPixels);
        if (rgba32_is_white(rgbaBuffer, neighbourOffset))
        {
            removeBackground_Phase2_MakeNeighbourPixelsTransparent(rgbaBuffer, row, column - 1, spriteWidthInPixels);
        }
    }
}

/**
 * @brief This function is called whenever we detect a white pixel after removing the white background from every edge.
 * We will check the pixel neighbours to figure out whether we can connect this pixel with a transparent one.
 * If we can find a transparent pixel in our direct neighbouring pixels, we'll turn this pixel and all its white neighbours transparant (using removeBackground_Phase2_MakeNeighbourPixelsTransparent).
 */
static void removeBackground_Phase2_dealWithWhitePixel(uint8_t *rgbaBuffer, int row, int column, int spriteWidthInPixels, int spriteHeightInPixels)
{
    uint32_t neighBourOffset;
    bool foundTransparentPixel = false;
    // allright, we currently have a white pixel.
    // now we're going to search if we can find a transparent pixel around this one.
    // if we can find one, we transform any white pixels around us recursively. If we just find a non-white, non-transparent pixel, we'll ignore it

    // check alpha value of the left neighbour (if any)
    if (column)
    {
        neighBourOffset = getRGBA32Offset(row, column - 1, spriteWidthInPixels);
        if (rgba32_alpha(rgbaBuffer, neighBourOffset) == 0)
        {
            foundTransparentPixel = true;
        }
    }

    // now check the alpha value of the bottom neighbour (if any)
    if (!foundTransparentPixel && row < spriteHeightInPixels)
    {
        neighBourOffset = getRGBA32Offset(row + 1, column, spriteWidthInPixels);
        if (rgba32_alpha(rgbaBuffer, neighBourOffset) == 0)
        {
            foundTransparentPixel = true;
        }
    }

    // now check the alpha value of the right neighbour (if any)
    if (!foundTransparentPixel && column < spriteWidthInPixels - 1)
    {
        neighBourOffset = getRGBA32Offset(row, column + 1, spriteWidthInPixels);
        if (rgba32_alpha(rgbaBuffer, neighBourOffset) == 0)
        {
            foundTransparentPixel = true;
        }
    }

    // now check the alpha value of the top neighbour (if any)
    if(!foundTransparentPixel && row)
    {
        neighBourOffset = getRGBA32Offset(row - 1, column, spriteWidthInPixels);
        if (rgba32_alpha(rgbaBuffer, neighBourOffset) == 0)
        {
            foundTransparentPixel = true;
        }
    }

    // allright, we found at least 1 transparent neighbour.
    // we should turn the current pixel and its white neighbours transparent
    if (foundTransparentPixel)
    {
        removeBackground_Phase2_MakeNeighbourPixelsTransparent(rgbaBuffer, row, column, spriteWidthInPixels);
    }
}

static void convertRGBToRGBA(uint8_t* rgbBuffer, uint8_t* rgbaBuffer, int spriteWidthInPixels, int spriteHeightInPixels)
{
    uint32_t srcOffset, dstOffset;
    int row, column;
    // convert to RGBA first
    for (row = 0; row < spriteHeightInPixels; ++row)
    {
        for (column = 0; column < spriteWidthInPixels; ++column)
        {
            srcOffset = (row * spriteWidthInPixels * 3) + (column * 3);
            dstOffset = getRGBA32Offset(row, column, spriteWidthInPixels);
            rgbaBuffer[dstOffset] = rgbBuffer[srcOffset];
            rgbaBuffer[dstOffset + 1] = rgbBuffer[srcOffset + 1];
            rgbaBuffer[dstOffset + 2] = rgbBuffer[srcOffset + 2];
            rgbaBuffer[dstOffset + 3] = 0xFF; // set alpha to max
        }
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
static void removeBackground(uint8_t *rgbaBuffer, int spriteWidthInPixels, int spriteHeightInPixels)
{
    int row;
    int column;
    
    // edge detection: scan from left-to-right, right-to-left, top-to-bottom, bottom-to-top on a half image each.
    // keep removing white pixels until you encounter a non-white pixel (if you do, move to the next row/column)
    // let's start with horizontal left-to-right.
    for (row = 0; row < spriteHeightInPixels; ++row)
    {
        for (column = 0; column < spriteWidthInPixels / 2; ++column)
        {
            if (!transformWhiteBackgroundPixel(rgbaBuffer, row, column, spriteWidthInPixels))
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
            if (!transformWhiteBackgroundPixel(rgbaBuffer, row, column, spriteWidthInPixels))
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
            if (!transformWhiteBackgroundPixel(rgbaBuffer, row, column, spriteWidthInPixels))
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
            if (!transformWhiteBackgroundPixel(rgbaBuffer, row, column, spriteWidthInPixels))
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
            const uint32_t srcOffset = getRGBA32Offset(row, column, spriteWidthInPixels);
            const bool white = rgba32_is_white(rgbaBuffer, srcOffset);
            if (white)
            {
                removeBackground_Phase2_dealWithWhitePixel(rgbaBuffer, row, column, spriteWidthInPixels, spriteHeightInPixels);
            }
        }
    }
}

// end of the removeBackground RGBA32 specific code

static void decodeGen1Sprite(Gen1GameReader &gameReader, uint8_t pokeIndex)
{
    SpriteRenderer renderer;
    Gen1PokeStats stats;
    uint8_t *spriteBuffer;
    uint8_t *rgbBuffer;
    uint8_t rgbaBuffer[SPRITE_BUFFER_WIDTH_IN_PIXELS * SPRITE_BUFFER_HEIGHT_IN_PIXELS * 4];
    char fileNameBuf[50];
    uint16_t colorPalette[4];
    const char *pokeName;

    if (!gameReader.isValidIndex(pokeIndex))
    {
        fprintf(stderr, "ERROR: index %hhu is not valid!\n", pokeIndex);
        return;
    }
    pokeName = gameReader.getPokemonName(pokeIndex);

    printf("Decoding %s\n", pokeName);

    gameReader.readPokemonStatsForIndex(pokeIndex, stats);
    gameReader.readColorPalette(gameReader.getColorPaletteIndexByPokemonNumber(stats.pokedex_number), colorPalette);

    spriteBuffer = gameReader.decodeSprite(stats.sprite_bank, stats.pointer_to_frontsprite);

    if(g_writeSpriteBufferBinFile)
    {
        // we'll now write the decoded sprite in binary form. This is in gameboy format essentially.
        snprintf(fileNameBuf, sizeof(fileNameBuf), "%s.bin", pokeName);
        FILE *f = fopen(fileNameBuf, "w");
        fwrite(spriteBuffer, 1, GEN1_DECODED_SPRITE_BUFFER_SIZE_IN_BYTES, f);
        fclose(f);
    }

    // convert it to RGB
    rgbBuffer = renderer.drawRGB(spriteBuffer, colorPalette, 7, 7);
    
    snprintf(fileNameBuf, sizeof(fileNameBuf), "%s.png", pokeName);
    // depending on the cmdline arguments, we try to remove the white background or not
    if(g_removeBackground)
    {
        convertRGBToRGBA(rgbBuffer, rgbaBuffer, SPRITE_BUFFER_WIDTH_IN_PIXELS, SPRITE_BUFFER_HEIGHT_IN_PIXELS);
        removeBackground(rgbaBuffer, SPRITE_BUFFER_WIDTH_IN_PIXELS, SPRITE_BUFFER_HEIGHT_IN_PIXELS);
        
        // now convert the RGB data and output it as PNG
        write_png(fileNameBuf, rgbaBuffer, SPRITE_BUFFER_WIDTH_IN_PIXELS, SPRITE_BUFFER_HEIGHT_IN_PIXELS, true);
    }
    else
    {
        // now convert the RGB data and output it as PNG
        write_png(fileNameBuf, rgbBuffer, SPRITE_BUFFER_WIDTH_IN_PIXELS, SPRITE_BUFFER_HEIGHT_IN_PIXELS);
    }
}

static void decodeGen2Sprite(Gen2GameReader &gameReader, uint8_t pokeIndex)
{
    SpriteRenderer renderer;
    Gen2PokeStats stats;
    uint8_t *spriteBuffer;
    uint8_t *rgbBuffer;
    uint8_t rgbaBuffer[SPRITE_BUFFER_WIDTH_IN_PIXELS * SPRITE_BUFFER_HEIGHT_IN_PIXELS * 4];
    char fileNameBuf[50];
    uint16_t colorPalette[4];
    const char *pokeName;
    uint8_t spriteWidthInTiles;
    uint8_t spriteHeightInTiles;
    uint8_t bankIndex;
    uint16_t pointer;

    if (!gameReader.isValidIndex(pokeIndex))
    {
        fprintf(stderr, "ERROR: index %hhu is not valid!\n", pokeIndex);
        return;
    }
    pokeName = gameReader.getPokemonName(pokeIndex);

    printf("Decoding %s\n", pokeName);

    gameReader.readPokemonStatsForIndex(pokeIndex, stats);
    gameReader.readSpriteDimensions(stats, spriteWidthInTiles, spriteHeightInTiles);
    gameReader.readColorPaletteForPokemon(pokeIndex, false, colorPalette);
    gameReader.readFrontSpritePointer(pokeIndex, bankIndex, pointer);

    spriteBuffer = gameReader.decodeSprite(bankIndex, pointer);

    if(g_writeSpriteBufferBinFile)
    {
        // we'll now write the decoded sprite in binary form. This is in gameboy format essentially.
        snprintf(fileNameBuf, sizeof(fileNameBuf), "%s.bin", pokeName);
        FILE *f = fopen(fileNameBuf, "w");
        fwrite(spriteBuffer, 1, GEN2_SPRITE_BUFFER_SIZE_IN_BYTES, f);
        fclose(f);
    }

    // convert it to RGB
    rgbBuffer = renderer.drawRGB(spriteBuffer, colorPalette, spriteWidthInTiles, spriteHeightInTiles);
    
    snprintf(fileNameBuf, sizeof(fileNameBuf), "%s.png", pokeName);
    // depending on the cmdline arguments, we try to remove the white background or not
    if(g_removeBackground)
    {
        convertRGBToRGBA(rgbBuffer, rgbaBuffer, spriteWidthInTiles * 8, spriteHeightInTiles * 8);
        removeBackground(rgbaBuffer, spriteWidthInTiles * 8, spriteHeightInTiles * 8);
        
        // now convert the RGB data and output it as PNG
        write_png(fileNameBuf, rgbaBuffer, spriteWidthInTiles * 8, spriteHeightInTiles * 8, true);
    }
    else
    {
        // now convert the RGB data and output it as PNG
        write_png(fileNameBuf, rgbBuffer, spriteWidthInTiles * 8, spriteHeightInTiles * 8);
    }
}

static void print_usage()
{
    fprintf(stderr, "Usage: decodeSprite [OPTIONS] <path/to/rom.gbc> <poke_index>\n");
    fprintf(stderr, "\tOptions:\n");
    fprintf(stderr, "\t\t-b\tAlso output the decoded sprite buffer (gb format) as a bin file\n");
    fprintf(stderr, "\t\t-B\tRemoves the white background from the sprite\n");
}

int main(int argc, char **argv)
{
    int option;
    while ((option = getopt(argc, argv, "bB")) != -1) 
    {
        switch (option) {
        case 'b':
            printf("Writing decoded sprite buffer enabled!\n");
            g_writeSpriteBufferBinFile = true;
            break;
        case 'B':
            printf("Removing background enabled!\n");
            g_removeBackground = true;
            break;
        default:                          /* '?' */
            fprintf(stderr, "ERROR: Unknown option!");
            print_usage();
            return 1;
        }
    }
    
    if (argc - optind < 2)
    {
        fprintf(stderr, "ERROR: Not enough arguments!\n");
        print_usage();
        return 1;
    }

    uint8_t *romBuffer;
    uint32_t romFileSize;

    romBuffer = readFileIntoBuffer(argv[optind], romFileSize);
    if (!romBuffer)
    {
        fprintf(stderr, "ERROR: Couldn't read file %s\n", argv[1]);
        return 1;
    }

    GameboyCartridgeHeader cartridgeHeader;
    BufferBasedRomReader romReader(romBuffer, romFileSize);
    BufferBasedSaveManager saveManager(0, 0);

    readGameboyCartridgeHeader(romReader, cartridgeHeader);

    // check if we're dealing with gen 1
    const Gen1GameType gen1Type = gen1_determineGameType(cartridgeHeader);
    const Gen2GameType gen2Type = gen2_determineGameType(cartridgeHeader);
    if (gen1Type != Gen1GameType::INVALID)
    {
        Gen1GameReader gameReader(romReader, saveManager, gen1Type);
        decodeGen1Sprite(gameReader, strtoul(argv[optind + 1], 0, 0));
    }
    else if (gen2Type != Gen2GameType::INVALID)
    {
        Gen2GameReader gameReader(romReader, saveManager, gen2Type);
        decodeGen2Sprite(gameReader, strtoul(argv[optind + 1], 0, 0));
    }

    free(romBuffer);
    romBuffer = 0;

    return 0;
}
