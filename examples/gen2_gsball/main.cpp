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

#define POKEMON_CRYSTAL_ITEM_ID_GS_BALL 0x73

static void gen2ReceiveGSBall(Gen2GameReader& gameReader)
{
    bool alreadyHasOne = false;

    const char* trainerName = gameReader.getTrainerName();
    Gen2ItemList keyItemPocket = gameReader.getItemList(Gen2ItemListType::GEN2_ITEMLISTTYPE_KEYITEMPOCKET);
    if(keyItemPocket.getCount() > 0)
    {
        uint8_t itemId;
        uint8_t itemCount;
        bool gotEntry = keyItemPocket.getEntry(0, itemId, itemCount);

        while(gotEntry)
        {
            if(itemId == POKEMON_CRYSTAL_ITEM_ID_GS_BALL)
            {
                alreadyHasOne = true;
                break;
            }
            gotEntry = keyItemPocket.getNextEntry(itemId, itemCount);
        }
    }
    
    if(alreadyHasOne)
    {
        fprintf(stderr, "ERROR: It appears you already have one!");
    }
    else
    {
        keyItemPocket.add(POKEMON_CRYSTAL_ITEM_ID_GS_BALL, 1);
        gameReader.finishSave();
        fprintf(stdout, "%s obtained a GS Ball!", trainerName);
    }
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

    //check if we're dealing with gen 2
    const Gen2GameType gen2Type = gen2_determineGameType(cartridgeHeader);
    if(gen2Type == Gen2GameType::INVALID)
    {
        fprintf(stderr, "ERROR: not a gen 2 game!\n");
        return 1;
    }
    if(gen2Type != Gen2GameType::CRYSTAL)
    {
        fprintf(stderr, "ERROR: not Pokémon Crystal!\n");
//        return 1;
    }
    
    Gen2GameReader gameReader(romReader, saveManager, gen2Type);
    gen2ReceiveGSBall(gameReader);

    // Test for adding rare candy
#if 0
    Gen2ItemList itemPocket = gameReader.getItemList(Gen2ItemListType::GEN2_ITEMLISTTYPE_ITEMPOCKET);
    itemPocket.add(0x20, 27);
    gameReader.finishSave();
#endif

    FILE* f = fopen(argv[2], "w");
    fwrite(saveBuffer, 1, saveFileSize, f);
    fclose(f);

    free(romBuffer);
    free(saveBuffer);
    romBuffer = 0;
    saveBuffer = 0;
    
    return 0;
}
