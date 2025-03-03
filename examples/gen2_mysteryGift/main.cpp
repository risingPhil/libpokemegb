#include "gen2/Gen2GameReader.h"
#include "gen2/Gen2Items.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "RTCReader.h"
#include "utils.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        fprintf(stderr, "Usage: gen2_mysteryGift <path/to/rom.gbc> <path/to/file.sav> <path/to/file.rtc>\n");
        return 1;
    }

    uint8_t* romBuffer;
    uint8_t* saveBuffer;
    uint8_t* rtcBuffer;
    uint32_t romFileSize;
    uint32_t saveFileSize;
    uint32_t rtcFileSize;

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

    rtcBuffer = readFileIntoBuffer(argv[3], rtcFileSize);
    if(!rtcBuffer)
    {
        fprintf(stderr, "ERROR: Couldn't read file %s\n", argv[3]);
        return 1;
    }

    GameboyCartridgeHeader cartridgeHeader;
    BufferBasedRomReader romReader(romBuffer, romFileSize);
    BufferBasedSaveManager saveManager(saveBuffer, saveFileSize);
    BufferBasedRTCReader rtcReader(rtcBuffer, rtcFileSize, time(NULL));

    readGameboyCartridgeHeader(romReader, cartridgeHeader);

    //check if we're dealing with gen 2
    const Gen2GameType gen2Type = gen2_determineGameType(cartridgeHeader);
    if(gen2Type == Gen2GameType::INVALID)
    {
        fprintf(stderr, "ERROR: not a gen 2 game!\n");
        return 1;
    }

    srand(time(NULL));

    Gen2GameReader gameReader(romReader, saveManager, gen2Type);
    Gen2MysteryGiftManager mysteryGiftManager = gameReader.getMysteryGiftManager();
    const auto gift = selectRandomGift();

    if(!mysteryGiftManager.isUnlocked())
    {
        printf("Mystery Gift feature was unlocked!\n");
        mysteryGiftManager.unlock();
    }
    const MysteryGiftResult mgResult = mysteryGiftManager.obtain(gameReader, rtcReader, gift);
    const char* trainerName = gameReader.getTrainerName();

    if(gift.useDecoration && mgResult == MYSTERYGIFT_RESULT_DECORATION)
    {
        printf("Mystery gift: %s obtained %s decoration! It was sent to your PC!\n", trainerName, getGen2DecorationString((Gen2Decoration)gift.decorationID));
    }
    else if(mgResult == MYSTERYGIFT_RESULT_ITEM_ITEMPOCKET || mgResult == MYSTERYGIFT_RESULT_ITEM_BALLPOCKET)
    {
        printf("Mystery gift: %s obtained %s!\n", trainerName, getGen2ItemString((Gen2Items)gift.itemID));
    }
    else
    {
        printf("Mystery gift: error %d\n", (int)mgResult);
    }

    gameReader.finishSave();

    FILE* f = fopen(argv[2], "w");
    fwrite(saveBuffer, 1, saveFileSize, f);
    fclose(f);

    free(romBuffer);
    free(saveBuffer);
    free(rtcBuffer);
    romBuffer = 0;
    saveBuffer = 0;
    rtcBuffer = 0;

    return 0;
}
