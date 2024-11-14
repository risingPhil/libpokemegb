#include "gen1/Gen1GameReader.h"
#include "gen2/Gen2GameReader.h"
#include "SpriteRenderer.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "utils.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

using OutputFormat = SpriteRenderer::OutputFormat;

static const LocalizationLanguage g_localization = LocalizationLanguage::ENGLISH;

static void decodeGen1Icon(IRomReader& romReader, ISaveManager& saveManager, Gen1GameType gen1Type, Gen1PokemonIconType iconType, bool firstFrame)
{
    Gen1GameReader gameReader(romReader, saveManager, gen1Type, g_localization);
    SpriteRenderer renderer;
    char fileNameBuf[100];
    uint8_t* spriteBuffer;
    uint8_t* outputBuffer;

    spriteBuffer = gameReader.decodePokemonIcon(iconType, firstFrame);
    if(!spriteBuffer)
    {
        fprintf(stderr, "ERROR: Could not decode icon for icon type %d and firstFrame %d!\n", (int)iconType, firstFrame);
        return;
    }
    outputBuffer = renderer.draw(spriteBuffer, OutputFormat::RGB, monochromeGBColorPalette, GEN1_ICON_WIDTH_IN_TILES, GEN1_ICON_HEIGHT_IN_TILES);
    snprintf(fileNameBuf, sizeof(fileNameBuf), "%d_frame%u.png", (int)iconType, (firstFrame) ? 1 : 2);
    write_png(fileNameBuf, outputBuffer, 2 * 8, 2 * 8, false);
}

static void decodeGen2Icon(IRomReader& romReader, ISaveManager& saveManager, Gen2GameType gen2Type, Gen2PokemonIconType iconType, bool firstFrame)
{
    Gen2GameReader gameReader(romReader, saveManager, gen2Type);
    SpriteRenderer renderer;
    char fileNameBuf[100];
    uint8_t* spriteBuffer;
    uint8_t* outputBuffer;

    spriteBuffer = gameReader.decodePokemonIcon(iconType, firstFrame);
    if(!spriteBuffer)
    {
        fprintf(stderr, "ERROR: Could not decode icon for icon type %d and firstFrame %d!\n", (int)iconType, firstFrame);
        return;
    }
    outputBuffer = renderer.draw(spriteBuffer, OutputFormat::RGB, gen2_iconColorPalette, GEN2_ICON_WIDTH_IN_TILES, GEN2_ICON_HEIGHT_IN_TILES);
    snprintf(fileNameBuf, sizeof(fileNameBuf), "%d_frame%u.png", (int)iconType, (firstFrame) ? 1 : 2);
    write_png(fileNameBuf, outputBuffer, 2 * 8, 2 * 8, false);
}

static void decodeGen1Icons(IRomReader& romReader, ISaveManager& saveManager, Gen1GameType gen1Type)
{
    for(int i=0; i < GEN1_ICONTYPE_MAX; ++i)
    {
        decodeGen1Icon(romReader, saveManager, gen1Type, (Gen1PokemonIconType)i, true);
        decodeGen1Icon(romReader, saveManager, gen1Type, (Gen1PokemonIconType)i, false);
    }
}

static void decodeGen2Icons(IRomReader& romReader, ISaveManager& saveManager, Gen2GameType gen2Type)
{
    for(int i=1; i < GEN2_ICONTYPE_MAX; ++i)
    {
        decodeGen2Icon(romReader, saveManager, gen2Type, (Gen2PokemonIconType)i, true);
        decodeGen2Icon(romReader, saveManager, gen2Type, (Gen2PokemonIconType)i, false);
    }
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <path/to/rom.gbc>\n", argv[0]);
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
    BufferBasedSaveManager saveManager(nullptr, 0);

    readGameboyCartridgeHeader(romReader, cartridgeHeader);

    //check if we're dealing with gen 1
    const Gen1GameType gen1Type = gen1_determineGameType(cartridgeHeader);
    const Gen2GameType gen2Type = gen2_determineGameType(cartridgeHeader);
    if(gen1Type != Gen1GameType::INVALID)
    {
        decodeGen1Icons(romReader, saveManager, gen1Type);
    }
    else if(gen2Type != Gen2GameType::INVALID)
    {
        decodeGen2Icons(romReader, saveManager, gen2Type);
    }
    else
    {
        fprintf(stderr, "ERROR: The specified rom is not a gen 1 or gen 2 pokémon game!\n");
    }

    free(romBuffer);
    romBuffer = 0;

    return 0;
}