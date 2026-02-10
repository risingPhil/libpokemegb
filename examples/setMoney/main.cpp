#include "gen1/Gen1GameReader.h"
#include "gen2/Gen2GameReader.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "utils.h"

#include <cstdio>
#include <cstdint>
#include <cstdlib>

static void print_usage()
{
    printf("Usage: setMoney <path/to/rom.gbc> <path/to/file.sav> <amount>\n");
}

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        print_usage();
        return 1;
    }
    uint8_t* romBuffer;
    uint8_t* savBuffer;
    uint32_t romFileSize;
    uint32_t savFileSize;
    uint32_t amount = std::atoi(argv[3]);

    printf("rom: %s, save: %s, amount: %u\n", argv[1], argv[2], amount);

    romBuffer = readFileIntoBuffer(argv[1], romFileSize);
    if(!romBuffer)
    {
        fprintf(stderr, "ERROR: Couldn't read file %s\n", argv[1]);
        return 1;
    }
    savBuffer = readFileIntoBuffer(argv[2], savFileSize);
    if(!savBuffer)
    {
        fprintf(stderr, "ERROR: Couldn't read file %s\n", argv[2]);
        free(romBuffer);
        romBuffer = nullptr;
        return 1;
    }

    GameboyCartridgeHeader cartridgeHeader;
    BufferBasedRomReader romReader(romBuffer, romFileSize);
    BufferBasedSaveManager saveManager(savBuffer, savFileSize);

    readGameboyCartridgeHeader(romReader, cartridgeHeader);

    // check if we're dealing with gen 1
    const Gen1GameType gen1Type = gen1_determineGameType(cartridgeHeader);
    const Gen2GameType gen2Type = gen2_determineGameType(cartridgeHeader);
    if (gen1Type != Gen1GameType::INVALID)
    {
        Gen1GameReader gameReader(romReader, saveManager, gen1Type);
        gameReader.setTrainerMoney(amount);
        gameReader.updateMainChecksum();
    }
    else if (gen2Type != Gen2GameType::INVALID)
    {
        Gen2GameReader gameReader(romReader, saveManager, gen2Type);
        gameReader.setTrainerMoney(amount);
        gameReader.finishSave();
    }

    FILE* f = fopen(argv[2], "w");
    fwrite(savBuffer, 1, savFileSize, f);
    fclose(f);

    free(romBuffer);
    romBuffer = nullptr;

    free(savBuffer);
    savBuffer = nullptr;

    return 0;
}