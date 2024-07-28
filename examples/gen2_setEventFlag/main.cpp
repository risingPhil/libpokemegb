#include "gen2/Gen2GameReader.h"
#include "gen1/Gen1Common.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "utils.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

static uint16_t getFlagNumber()
{
    char input[100];
    printf("Enter event flag number: ");
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        fprintf(stderr, "Error reading input\n");
        return 0;
    }
    // Remove the newline character if it exists
    input[strcspn(input, "\n")] = '\0';

    return (uint16_t)strtoul(input, 0, 10);
}

static uint16_t getFlagValue()
{
    char input[100];
    printf("Enter new value (0-1): ");
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        fprintf(stderr, "Error reading input\n");
        return 0;
    }
    // Remove the newline character if it exists
    input[strcspn(input, "\n")] = '\0';

    return (uint16_t)strtoul(input, 0, 10);
}

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage: gen2_setEventFlag <path/to/rom.gbc> <path/to/file.sav>\n");
        return 1;
    }

    uint8_t* romBuffer;
    uint8_t* saveBuffer;
    uint32_t romFileSize;
    uint32_t saveFileSize;

    romBuffer = readFileIntoBuffer(argv[1], romFileSize);
    if(!romBuffer)
    {
        fprintf(stderr, "ERROR: Couldn't read file %s\n", argv[1]);
        return 1;
    }
    saveBuffer = readFileIntoBuffer(argv[2], saveFileSize);
    if(!saveBuffer)
    {
        fprintf(stderr, "ERROR: Couldn't read file %s\n", argv[2]);
        return 1;
    }

    GameboyCartridgeHeader cartridgeHeader;
    BufferBasedRomReader romReader(romBuffer, romFileSize);
    BufferBasedSaveManager saveManager(saveBuffer, saveFileSize);

    readGameboyCartridgeHeader(romReader, cartridgeHeader);

    //check if we're dealing with gen 1
    const Gen1GameType gen1Type = gen1_determineGameType(cartridgeHeader);
    const Gen2GameType gen2Type = gen2_determineGameType(cartridgeHeader);
    if(gen1Type != Gen1GameType::INVALID)
    {
        fprintf(stderr, "ERROR: sorry, this tool only supports gen 2!\n");
        return 1;
    }
    else if(gen2Type == Gen2GameType::INVALID)
    {
        fprintf(stderr, "ERROR: this is not a Gen 2 pokémon game!\n");
        return 1;
    }

    const uint16_t flagNumber = getFlagNumber();
    const uint16_t flagValue = getFlagValue();

    if(flagValue != 0 && flagValue != 1)
    {
        fprintf(stderr, "ERROR: wrong value %hu\n", flagValue);
        return 1;
    }

    Gen2GameReader gameReader(romReader, saveManager, gen2Type);
    gameReader.setEventFlag(flagNumber, static_cast<bool>(flagValue));
    gameReader.finishSave();
    printf("Flag %hu => %s\n", flagNumber, (static_cast<bool>(flagValue)) ? "true" : "false");
    
    FILE* f = fopen(argv[2], "w");
    fwrite(saveBuffer, 1, saveFileSize, f);
    fclose(f);

    free(romBuffer);
    free(saveBuffer);
    romBuffer = 0;
    saveBuffer = 0;

    return 0;
}