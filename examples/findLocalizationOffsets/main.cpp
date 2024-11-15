#include "utils.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "gen1/Gen1Common.h"
#include "gen1/Gen1Localization.h"
#include "gen2/Gen2Common.h"

#include <cstdio>
#include <cstring>

static const LocalizationLanguage g_localization = LocalizationLanguage::ENGLISH;

static void findBinaryPattern(uint8_t* buffer, size_t bufferSize, uint8_t* pattern, size_t patternSize)
{
    uint8_t* cur = buffer;
    while(cur)
    {
        cur = (uint8_t*)memmem(cur, bufferSize - (cur - buffer), pattern, patternSize);
        if(cur)
        {
            printf("%s: found pattern at offset 0x%08x\n", __FUNCTION__, (uint32_t)(cur - buffer));
            cur += patternSize;
        }
    }
}

static uint32_t findNames(uint8_t* buffer, size_t bufferSize, const char** nameList, uint8_t nameListSize)
{
    uint8_t nameBuffer[0xB];
    uint8_t i;
    uint16_t nameLength;
    const uint8_t nameEntrySize = 0xA;
    uint8_t* firstEntry;
    uint8_t* cur = buffer;
    bool match;

    nameLength = gen1_encodePokeText(nameList[0], strlen(nameList[0]), nameBuffer, sizeof(nameBuffer), 0x50, g_localization);
    firstEntry = (uint8_t*)memmem(cur, bufferSize - (cur - buffer), nameBuffer, nameLength);

    while(firstEntry)
    {
        cur = firstEntry + nameEntrySize;
        match = true;
        for(i = 1; i < nameListSize; ++i)
        {
            nameLength = gen1_encodePokeText(nameList[i], strlen(nameList[i]), nameBuffer, sizeof(nameBuffer), 0x50, g_localization);
            if(memcmp(cur, nameBuffer, nameLength))
            {
                //not equal, so no match at this offset
                match = false;
                break;
            }
            cur += nameEntrySize;
        }

        if(match)
        {
            return (uint32_t)(firstEntry - buffer);
        }
        // re-encode first name
        nameLength = gen1_encodePokeText(nameList[0], strlen(nameList[0]), nameBuffer, sizeof(nameBuffer), 0x50, g_localization);
        // continue searching for another occurrence of the first name starting from the bytes directly after
        // the previously found location
        cur = firstEntry + nameEntrySize;
        firstEntry = (uint8_t*)memmem(cur, bufferSize - (cur - buffer), nameBuffer, nameLength);
    }

    return 0xFFFFFFFF;
}

#if 0
static void dumpHex(const uint8_t* buffer, uint16_t numBytes, const char* prefix)
{
    uint16_t i = 0;
    while(i < numBytes)
    {
        if(i % 8 == 0)
        {
            printf("\n%s", prefix);
        }
        printf("%02hhX ", buffer[i]);
        ++i;
    }
    printf("\n");
}
#endif

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage: findLocalizationOffsets <path/to/english_rom.gbc> <path/to/localized_rom.gbc>\n");
        return 1;
    }

    uint8_t* romBuffer;
    uint8_t* localizedRomBuffer;
    uint8_t* saveBuffer = nullptr;
    uint32_t romFileSize;
    uint32_t localizedRomBufferSize;
    uint32_t saveFileSize = 0;

    romBuffer = readFileIntoBuffer(argv[1], romFileSize);
    if(!romBuffer)
    {
        fprintf(stderr, "ERROR: Couldn't read file %s\n", argv[1]);
        return 1;
    }

    localizedRomBuffer = readFileIntoBuffer(argv[2], localizedRomBufferSize);
    if(!localizedRomBuffer)
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
    if(gen2Type != Gen2GameType::INVALID)
    {
        fprintf(stderr, "ERROR: sorry, this tool only supports gen 1!\n");
        return 1;
    }
    else if(gen1Type == Gen1GameType::INVALID)
    {
        fprintf(stderr, "ERROR: this is not a Gen 2 pokémon game!\n");
        return 1;
    }

    // When searching for Red/Blue offsets, use this line.
    const Gen1LocalizationRomOffsets* eng = g1_localizationOffsetsRB;
    // For yellow, use this one instead:
//  const Gen1LocalizationRomOffsets* eng = g1_localizationOffsetsY;
    printf("Stats:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng->stats, 11);
    printf("Stats Mew:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng->statsMew, 11);
    printf("Numbers:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng->numbers, 11);
    const char* pokeNames[] = {"RHYDON"};
    printf("Names: 0x%08x\n", findNames(localizedRomBuffer, localizedRomBufferSize, pokeNames, 1));
    printf("IconTypes:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng->iconTypes, 11);
    printf("Icons:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng->icons, 6);
    printf("PaletteIndices:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng->paletteIndices, 11);
    printf("Palettes:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng->palettes, 11);

    return 0;
}