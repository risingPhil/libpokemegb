#include "gen1/Gen1GameReader.h"
#include "gen2/Gen2GameReader.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "SpriteRenderer.h"
#include "utils.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

static void decodeGen1Sprite(Gen1GameReader& gameReader, uint8_t pokeIndex)
{
    SpriteRenderer renderer;
    Gen1PokeStats stats;
    uint8_t* spriteBuffer;
    uint8_t* rgbBuffer;
    char fileNameBuf[50];
    uint16_t colorPalette[4];
    const char* pokeName;

    if(!gameReader.isValidIndex(pokeIndex))
    {
        fprintf(stderr, "ERROR: index %hhu is not valid!\n", pokeIndex);
        return;
    }
    pokeName = gameReader.getPokemonName(pokeIndex);

    printf("Decoding %s\n", pokeName);

    gameReader.readPokemonStatsForIndex(pokeIndex, stats);
    gameReader.readColorPalette(gameReader.getColorPaletteIndexByPokemonNumber(stats.pokedex_number), colorPalette);
    
    spriteBuffer = gameReader.decodeSprite(stats.sprite_bank, stats.pointer_to_frontsprite);

    // we'll now write the decoded sprite in binary form. This is in gameboy format essentially.
    snprintf(fileNameBuf, sizeof(fileNameBuf), "%s.bin", pokeName);
    FILE* f = fopen(fileNameBuf, "w");
    fwrite(spriteBuffer, 1, GEN1_DECODED_SPRITE_BUFFER_SIZE_IN_BYTES, f);
    fclose(f);

    // convert it to RGB
    rgbBuffer = renderer.drawRGB(spriteBuffer, colorPalette, 7, 7);
    
    // now convert the RGB data and output it as PNG
    snprintf(fileNameBuf, sizeof(fileNameBuf), "%s.png", pokeName);
    write_png(fileNameBuf, rgbBuffer, 56, 56);
}

static void decodeGen2Sprite(Gen2GameReader& gameReader, uint8_t pokeIndex)
{
    SpriteRenderer renderer;
    Gen2PokeStats stats;
    uint8_t* spriteBuffer;
    uint8_t* rgbBuffer;
    char fileNameBuf[50];
    uint16_t colorPalette[4];
    const char* pokeName;
    uint8_t spriteWidthInTiles;
    uint8_t spriteHeightInTiles;
    uint8_t bankIndex;
    uint16_t pointer;

    if(!gameReader.isValidIndex(pokeIndex))
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
    
    // we'll now write the decoded sprite in binary form. This is in gameboy format essentially.
    snprintf(fileNameBuf, sizeof(fileNameBuf), "%s.bin", pokeName);
    FILE* f = fopen(fileNameBuf, "w");
    fwrite(spriteBuffer, 1, GEN2_SPRITE_BUFFER_SIZE_IN_BYTES, f);
    fclose(f);

    // convert it to RGB
    rgbBuffer = renderer.drawRGB(spriteBuffer, colorPalette, spriteWidthInTiles, spriteHeightInTiles);
    
    // now convert the RGB data and output it as PNG
    snprintf(fileNameBuf, sizeof(fileNameBuf), "%s.png", pokeName);
    write_png(fileNameBuf, rgbBuffer, spriteWidthInTiles * 8, spriteHeightInTiles * 8);
}

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage: decodeSprite <path/to/rom.gbc> <poke_index>\n");
        return 1;
    }

    uint8_t* romBuffer;
    uint32_t romFileSize;

    romBuffer = readFileIntoBuffer(argv[1], romFileSize);
    if(!romBuffer)
    {
        fprintf(stderr, "ERROR: Couldn't read file %s\n", argv[1]);
        return 1;
    }

    GameboyCartridgeHeader cartridgeHeader;
    BufferBasedRomReader romReader(romBuffer, romFileSize);
    BufferBasedSaveManager saveManager(0, 0);

    readGameboyCartridgeHeader(romReader, cartridgeHeader);

    //check if we're dealing with gen 1
    const Gen1GameType gen1Type = gen1_determineGameType(cartridgeHeader);
    const Gen2GameType gen2Type = gen2_determineGameType(cartridgeHeader);
    if(gen1Type != Gen1GameType::INVALID)
    {
        Gen1GameReader gameReader(romReader, saveManager, gen1Type);
        decodeGen1Sprite(gameReader, strtoul(argv[2], 0, 0));
    }
    else if(gen2Type != Gen2GameType::INVALID)
    {
        Gen2GameReader gameReader(romReader, saveManager, gen2Type);
        decodeGen2Sprite(gameReader, strtoul(argv[2], 0, 0));
    }

    free(romBuffer);
    romBuffer = 0;
    
    return 0;
}