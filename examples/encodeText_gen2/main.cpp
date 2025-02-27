#include "gen2/Gen2Common.h"

#include <cstdio>
#include <cstring>

static void print_hex(const unsigned char* buffer, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        fprintf(stderr, "Usage: encodeText_gen2 <textToEncode>\n");
        return 1;
    }

    uint8_t outputBuffer[4096];

    const uint16_t size = gen2_encodePokeText(argv[1], strlen(argv[1]), outputBuffer, sizeof(outputBuffer), 0x50, Gen2LocalizationLanguage::ENGLISH);
    print_hex(outputBuffer, size);
}