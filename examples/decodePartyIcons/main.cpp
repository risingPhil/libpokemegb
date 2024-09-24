#include "gen1/Gen1GameReader.h"
#include "gen1/Gen1SpriteDecoder.h"
#include "SpriteRenderer.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "utils.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

using OutputFormat = SpriteRenderer::OutputFormat;
using TileOrder = SpriteRenderer::TileOrder;

void decodeGen1Icon(IRomReader& romReader, ISaveManager& saveManager, Gen1GameType gen1Type, Gen1PokemonIconType iconType, bool firstFrame)
{
    Gen1GameReader gameReader(romReader, saveManager, gen1Type);
    SpriteRenderer renderer;
    char fileNameBuf[100];
    uint8_t *outputBuffer;

    outputBuffer = gameReader.decodePokemonIcon(iconType, renderer, OutputFormat::RGB, firstFrame);
    if(!outputBuffer)
    {
        fprintf(stderr, "ERROR: Could not decode icon for icon type %d and firstFrame %d!\n", (int)iconType, firstFrame);
        return;
    }
    snprintf(fileNameBuf, sizeof(fileNameBuf), "%d_frame%u.png", (int)iconType, (firstFrame) ? 1 : 2);
    write_png(fileNameBuf, outputBuffer, 2 * 8, 2 * 8, false);
}


void decodeGen1Icons(IRomReader& romReader, ISaveManager& saveManager, Gen1GameType gen1Type)
{
    for(int i=0; i < GEN1_ICONTYPE_MAX; ++i)
    {
        decodeGen1Icon(romReader, saveManager, gen1Type, (Gen1PokemonIconType)i, true);
        decodeGen1Icon(romReader, saveManager, gen1Type, (Gen1PokemonIconType)i, false);
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
    if(gen1Type == Gen1GameType::INVALID)
    {
        fprintf(stderr, "ERROR: sorry, this tool only supports gen 1!\n");
        return 1;
    }

    decodeGen1Icons(romReader, saveManager, gen1Type);

    free(romBuffer);
    romBuffer = 0;

    return 0;
}