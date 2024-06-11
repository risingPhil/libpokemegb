#include "gen1/Gen1GameReader.h"
#include "gen1/Gen1DistributionPokemon.h"
#include "gen2/Gen2GameReader.h"
#include "gen2/Gen2DistributionPokemon.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "SpriteRenderer.h"
#include "utils.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

static void showAvailableGen1DistributionPokeMenu(Gen1GameReader& gameReader)
{
    const Gen1DistributionPokemon** mainList;
    uint32_t listSize;
    char input[100];
    uint8_t userChoice;

    gen1_getMainDistributionPokemonList(mainList, listSize);

    printf("Available distribution pokémon:\n");
    for(uint8_t i = 0; i < listSize; ++i)
    {
        printf("\t%hhu.) %s\n", i, mainList[i]->name);
    }

    printf("Choose: (0-%hhu): ", listSize - 1);
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        fprintf(stderr, "Error reading input\n");
        return;
    }
    // Remove the newline character if it exists
    input[strcspn(input, "\n")] = '\0';

    userChoice = (uint8_t)strtoul(input, 0, 10);

    if(userChoice > listSize - 1)
    {
        fprintf(stderr, "Invalid choice, try again!");
        return;
    }

    printf("Congratulations! You got a %s!\n", gameReader.getPokemonName(mainList[userChoice]->poke.poke_index));
    gameReader.addDistributionPokemon((*mainList[userChoice]));
}

static void showAvailableGen2DistributionPokeMenu(Gen2GameReader& gameReader)
{
    const Gen2DistributionPokemon** mainList;
    const Gen2DistributionPokemon** pcnyList;
    const Gen2DistributionPokemon** listToUse;
    uint32_t mainListSize;
    uint32_t pcnyListSize;
    char input[100];
    uint8_t userChoice;
    uint8_t i;

    gen2_getMainDistributionPokemonList(mainList, mainListSize);
    gen2_getPokemonCenterNewYorkDistributionPokemonList(pcnyList, pcnyListSize);

    printf("Available distribution pokémon:\n");
    for(i = 0; i < mainListSize; ++i)
    {
        printf("\t%hhu.) %s\n", i, mainList[i]->name);
    }

    for(i = 0; i < pcnyListSize; ++i)
    {
        printf("\t%hhu.) PCNY %s\n", i + mainListSize, pcnyList[i]->name);
    }

    printf("Choose: (0-%hhu): ", mainListSize + pcnyListSize - 1);
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        fprintf(stderr, "Error reading input\n");
        return;
    }
    // Remove the newline character if it exists
    input[strcspn(input, "\n")] = '\0';

    userChoice = (uint8_t)strtoul(input, 0, 10);

    if(userChoice >= mainListSize + pcnyListSize)
    {
        fprintf(stderr, "Invalid choice, try again!");
        return;
    }

    if(userChoice >= mainListSize)
    {
        listToUse = pcnyList;
        userChoice -= mainListSize;
    }
    else
    {
        listToUse = mainList;
    }

    if(listToUse[userChoice]->isEgg)
    {
        printf("Congratulations! You got an %s EGG! Take good care of it!\n", gameReader.getPokemonName(listToUse[userChoice]->poke.poke_index));
    }
    else
    {
        printf("Congratulations! You got a %s!\n", gameReader.getPokemonName(listToUse[userChoice]->poke.poke_index));
    }
    gameReader.addDistributionPokemon((*listToUse[userChoice]));
    gameReader.finishSave();
}

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage: addDistributionPoke <path/to/rom.gbc> <path/to/file.sav>\n");
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
        Gen1GameReader gameReader(romReader, saveManager, gen1Type);

        showAvailableGen1DistributionPokeMenu(gameReader);
    }
    else if(gen2Type != Gen2GameType::INVALID)
    {
        Gen2GameReader gameReader(romReader, saveManager, gen2Type);
        showAvailableGen2DistributionPokeMenu(gameReader);
    }

    FILE* f = fopen(argv[2], "w");
    fwrite(saveBuffer, 1, saveFileSize, f);
    fclose(f);

    free(romBuffer);
    free(saveBuffer);
    romBuffer = 0;
    saveBuffer = 0;
    
    return 0;
}