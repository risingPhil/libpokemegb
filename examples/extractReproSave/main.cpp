#include "gen1/Gen1GameReader.h"
#include "gen2/Gen2GameReader.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "utils.h"

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>

const static uint32_t GEN2_REPRO_SAVE_OFFSET_IN_ROM = 0x1D0000;
const static uint16_t GB_ROM_BANK_SIZE = 0x4000;
const static uint16_t GB_SRAM_BANK_SIZE = 0x2000;

static void print_usage(const char* progName)
{
    printf("Usage: %s <path/to/rom.gbc>\n", progName);
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        print_usage(argv[0]);
        return 1;
    }
    uint8_t* romBuffer;
    uint32_t romFileSize;

    printf("rom: %s\n", argv[1]);

    romBuffer = readFileIntoBuffer(argv[1], romFileSize);
    if(!romBuffer)
    {
        fprintf(stderr, "ERROR: Couldn't read file %s\n", argv[1]);
        return 1;
    }

    GameboyCartridgeHeader cartridgeHeader;
    BufferBasedRomReader romReader(romBuffer, romFileSize);

    readGameboyCartridgeHeader(romReader, cartridgeHeader);

    // check if we're dealing with gen 1
    const Gen1GameType gen1Type = gen1_determineGameType(cartridgeHeader);
    const Gen2GameType gen2Type = gen2_determineGameType(cartridgeHeader);

    if(gen2Type == Gen2GameType::INVALID)
    {
        fprintf(stderr, "Not a valid gen 2 rom!\n");
        free(romBuffer);
        romBuffer = 0;
        return 1;
    }

    FILE* outFile = fopen("out.sav", "w");
    if(!outFile)
    {
        perror("Could not open file out.sav for writing: ");
        free(romBuffer);
        romBuffer = 0;
        return 1;
    }

    // So, turns out that these reproductions dedicate an entire rom bank
    // for every SRAM bank.
    // As a matter of fact, each SRAM bank is located at an offset of 0x2000 within
    // said rom bank.
    uint8_t savBankBuffer[GB_SRAM_BANK_SIZE];
    const uint8_t* cur = romBuffer + GEN2_REPRO_SAVE_OFFSET_IN_ROM;
    const uint8_t* const end = cur + (GB_ROM_BANK_SIZE * 4);
    while(cur < end)
    {
        memcpy(savBankBuffer, cur + GB_SRAM_BANK_SIZE, GB_SRAM_BANK_SIZE);
        fwrite(savBankBuffer, sizeof(uint8_t), GB_SRAM_BANK_SIZE, outFile);
        cur += GB_ROM_BANK_SIZE;
    }

    fclose(outFile);

    free(romBuffer);
    romBuffer = 0;

    return 0;
}