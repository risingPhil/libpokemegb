#include "utils.h"
#include "RomReader.h"
#include "gen1/Gen1Common.h"
#include "gen2/Gen2Common.h"
#include "gen1/Gen1Localization.h"
#include "gen2/Gen2Localization.h"

#include <cstdio>
#include <cstring>

/**
 * This tool tries to find rom offsets of a game localization based on certain byte patterns in the international english games.
 */

/*
 * The next variables need to be modified in a hardcoded way right now. They affect the findNames() function by specifying the localization for the
 * text encode/decode process. It therefore affects the character set table used to encode the pokémon names.
 * Therefore set these accordingly for the localization you're trying to search a pokémon name for.
 *
 * Also note that the pokémon name is hardcoded below. To search for a region-specific name, you'll have to modify it in the code as well.
 *
 * Yes, I'm aware this is a bit unclean, but once the relevant rom offsets are found once, you don't typically need this tool anymore.
 * Still, I'm leaving this application in the codebase in case I need more rom offsets later (or in case Korean localization support would get implemented at some point)
*/
static const Gen1LocalizationLanguage g1_localization = Gen1LocalizationLanguage::ENGLISH;
static const Gen2LocalizationLanguage g2_localization = Gen2LocalizationLanguage::ENGLISH;

static uint32_t findBinaryPattern(uint8_t* buffer, size_t bufferSize, uint8_t* pattern, size_t patternSize)
{
    uint32_t lastResult = 0;
    uint8_t* cur = buffer;
    while(cur)
    {
        cur = (uint8_t*)memmem(cur, bufferSize - (cur - buffer), pattern, patternSize);
        if(cur)
        {
            lastResult = (uint32_t)(cur - buffer);
            printf("%s: found pattern at offset 0x%08x\n", __FUNCTION__, lastResult);
            cur += patternSize;
        }
    }
    return lastResult;
}

static uint32_t findNames(uint8_t* buffer, size_t bufferSize, const char** nameList, uint8_t nameListSize, uint8_t gen)
{
    uint8_t nameBuffer[0xB];
    uint8_t i;
    uint16_t nameLength;
    const uint8_t nameEntrySize = 0xA;
    uint8_t* firstEntry;
    uint8_t* cur = buffer;
    bool match;

    if(gen == 1)
    {
        nameLength = gen1_encodePokeText(nameList[0], strlen(nameList[0]), nameBuffer, sizeof(nameBuffer), 0x50, g1_localization);
    }
    else if(gen == 2)
    {
        nameLength = gen2_encodePokeText(nameList[0], strlen(nameList[0]), nameBuffer, sizeof(nameBuffer), 0x50, g2_localization);
    }
    firstEntry = (uint8_t*)memmem(cur, bufferSize - (cur - buffer), nameBuffer, nameLength - 1);

    while(firstEntry)
    {
        cur = firstEntry + nameEntrySize;
        match = true;
        for(i = 1; i < nameListSize; ++i)
        {
            if(gen == 1)
            {
                nameLength = gen1_encodePokeText(nameList[i], strlen(nameList[i]), nameBuffer, sizeof(nameBuffer), 0x50, g1_localization);
            }
            else if(gen == 2)
            {
                nameLength = gen2_encodePokeText(nameList[i], strlen(nameList[i]), nameBuffer, sizeof(nameBuffer), 0x50, g2_localization);
            }

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
        if(gen == 1)
        {
            nameLength = gen1_encodePokeText(nameList[0], strlen(nameList[0]), nameBuffer, sizeof(nameBuffer), 0x50, g1_localization);
        }
        else if(gen == 2)
        {
            nameLength = gen2_encodePokeText(nameList[0], strlen(nameList[0]), nameBuffer, sizeof(nameBuffer), 0x50, g2_localization);
        }
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

static void processGen1(Gen1GameType gameType, uint8_t* romBuffer, uint8_t* localizedRomBuffer, size_t localizedRomBufferSize)
{
    // compare with the english/international release
    const Gen1LocalizationRomOffsets& eng = gen1_getRomOffsets(gameType, Gen1LocalizationLanguage::ENGLISH);

    printf("Stats:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng.stats, 11);
    printf("Stats Mew:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng.statsMew, 11);
    printf("Numbers:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng.numbers, 11);

    // WARNING: Modify this variable with the localized name of the pokémon we're trying to look up. (RHYDON, because it's index 1)
    const char* pokeNames[] = {"RHYDON"};
    printf("Names: 0x%08x\n", findNames(localizedRomBuffer, localizedRomBufferSize, pokeNames, 1, 1));
    printf("IconTypes:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng.iconTypes, 11);
    printf("Icons:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng.icons, 6);
    printf("PaletteIndices:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng.paletteIndices, 11);
    printf("Palettes:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng.palettes, 11);
}

static void processGen2(Gen2GameType gameType, uint8_t* romBuffer, uint8_t* localizedRomBuffer, size_t localizedRomBufferSize)
{
    // compare with the english/international release
    const Gen2LocalizationRomOffsets& eng = gen2_getRomOffsets(gameType, Gen2LocalizationLanguage::ENGLISH);

    printf("Stats:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng.stats, 11);
    // WARNING: Modify this variable with the localized name of the pokémon we're trying to look up. (BULBASAUR, because it's index 1)
    const char* pokeNames[] = {"BULBASAUR"};
    printf("Names: 0x%08x\n", findNames(localizedRomBuffer, localizedRomBufferSize, pokeNames, 1, 2));
    printf("IconTypes:\n");
    const uint32_t iconTypesOffset = findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng.iconTypes, 44);

    // Because the data shifts around between localizations and because eng.icons is a pointer (+ bank) list, we can't do an exact binary pattern search to figure out the icons offset
    // Luckily, the https://github.com/pret/pokegold project's pokegold.sym file tells us that the "Icons" offset is 78 bytes after the "IconPointers".
    // Therefore if we find the iconTypes offset, we should in theory be able to calculate the Icons offset based on that
    printf("Icons: 0x%08x\n", iconTypesOffset + 0xFB);
    printf("SpritePointers:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng.spritePointers, 11);
    printf("SpritePalettes:\n");
    findBinaryPattern(localizedRomBuffer, localizedRomBufferSize, romBuffer + eng.spritePalettes, 11);
}

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage: findLocalizationOffsets <path/to/english_rom.gbc> <path/to/localized_rom.gbc>\n");
        return 1;
    }

    uint8_t* romBuffer;
    uint8_t* localizedRomBuffer;
    uint32_t romFileSize;
    uint32_t localizedRomBufferSize;

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

    readGameboyCartridgeHeader(romReader, cartridgeHeader);

    //check if we're dealing with gen 1
    const Gen1GameType gen1Type = gen1_determineGameType(cartridgeHeader);
    const Gen2GameType gen2Type = gen2_determineGameType(cartridgeHeader);
    if(gen1Type != Gen1GameType::INVALID)
    {
       processGen1(gen1Type, romBuffer, localizedRomBuffer, localizedRomBufferSize);
       return 0;
    }
    else if(gen2Type != Gen2GameType::INVALID)
    {
        processGen2(gen2Type, romBuffer, localizedRomBuffer, localizedRomBufferSize);
        return 0;
    }

    fprintf(stderr, "ERROR: No valid gen I/II rom found!\n");
    return 1;
}