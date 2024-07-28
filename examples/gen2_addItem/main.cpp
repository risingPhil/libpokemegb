#include "gen2/Gen2GameReader.h"
#include "gen1/Gen1Common.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "utils.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

static Gen2ItemListType selectOption()
{
    char input[100];
    uint8_t userChoice;
    uint8_t listSize = static_cast<uint8_t>(GEN2_ITEMLISTTYPE_MAX);

    printf("Select item list:\n");

    for(uint8_t i=0; i < listSize; ++i)
    {
        printf("\t%hhu.) %s\n", i, gen2_getItemListTypeString(static_cast<Gen2ItemListType>(i)));
    }

    printf("Choose: (0-%hhu): ", listSize - 1);
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        fprintf(stderr, "Error reading input\n");
        return Gen2ItemListType::GEN2_ITEMLISTTYPE_INVALID;
    }
    // Remove the newline character if it exists
    input[strcspn(input, "\n")] = '\0';

    userChoice = (uint8_t)strtoul(input, 0, 10);

    if(userChoice > listSize - 1)
    {
        fprintf(stderr, "Invalid choice, try again!");
        return Gen2ItemListType::GEN2_ITEMLISTTYPE_INVALID;
    }

    return static_cast<Gen2ItemListType>(userChoice);
}

static uint8_t selectItem()
{
    char input[100];
    printf("Enter item id: ");
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        fprintf(stderr, "Error reading input\n");
        return 0;
    }
    // Remove the newline character if it exists
    input[strcspn(input, "\n")] = '\0';

    return (uint8_t)strtoul(input, 0, 10);
}

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage: gen2_addItem <path/to/rom.gbc> <path/to/file.sav>\n");
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

    Gen2ItemListType listType = selectOption();

    if(listType == Gen2ItemListType::GEN2_ITEMLISTTYPE_INVALID)
    {
        return 1;
    }
    Gen2GameReader gameReader(romReader, saveManager, gen2Type);
    Gen2ItemList itemList = gameReader.getItemList(listType);
    const uint8_t itemId = selectItem();
    itemList.add(itemId, 1);

    gameReader.finishSave();

    FILE* f = fopen(argv[2], "w");
    fwrite(saveBuffer, 1, saveFileSize, f);
    fclose(f);

    free(romBuffer);
    free(saveBuffer);
    romBuffer = 0;
    saveBuffer = 0;
    
    return 0;
}