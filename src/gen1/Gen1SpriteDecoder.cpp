#include "gen1/Gen1SpriteDecoder.h"
#include "SpriteRenderer.h"
#include "RomReader.h"

#include <cstring>
#include <cstdio>

// #define DUMP_SPRITEBITPLANES 1

// if you want to compare with something, you can use the chrome devtools on https://rgmechex.com/tech/gen1decompress.html
// to get the resulting bitplane, you can do this command in the console:
// writer.buffer.slice(writer.base, writer.base + 392)
// then you can use this approach to download the arraydata as a file: https://stackoverflow.com/questions/43026521/how-can-i-save-an-array-object-to-file-at-run-time-by-chrome-debug-tools

/**
 * @brief This class writes to a buffer in a vertical, column-first manner.
 * Supposedly sprites are being written to the buffer vertically during decoding, which is peculiar.
 */
class VerticalBitWriter
{
public:
    VerticalBitWriter(uint8_t *buffer, uint8_t spriteWidthInBits, uint8_t spriteHeightInBits)
        : buffer_(buffer)
        , curByte_(buffer)
        , bitIndex_(0)
        , spriteWidthInBits_(spriteWidthInBits)
        , spriteHeightInBits_(spriteHeightInBits)
        , bitsWritten_(0)
        , rowIndex_(0)
        , columnIndex_(0)
    {
    }

    void writePair(uint8_t pair)
    {
        writeBit((pair >> 1));
        writeBit((pair & 0x1));
    }

    void writeBit(uint8_t bit)
    {
        (*curByte_) |= (bit << (7 - bitIndex_));
        ++bitsWritten_;

        advance();
    }

    uint16_t getBitsWritten() const
    {
        return bitsWritten_;
    }

protected:
    void advance()
    {
        ++columnIndex_;
        if((columnIndex_ % 2) == 0)
        {
            ++rowIndex_;
            if(rowIndex_ == spriteHeightInBits_)
            {
                rowIndex_ = 0;
            }
            else
            {
                columnIndex_ -= 2;
            }

        }

        // storage in column major -> each next byte is the next row
        curByte_ = buffer_ + ((columnIndex_ / 8) * spriteHeightInBits_) +  rowIndex_;
//      printf("bitsWritten: %hu, columnIndex=%hhu, rowIndex=%hhu\n", bitsWritten_, columnIndex_, rowIndex_);

        bitIndex_ = (columnIndex_ % 8);
    }

private:
    uint8_t *buffer_;
    uint8_t *curByte_;
    uint8_t bitIndex_;
    uint8_t spriteWidthInBits_;
    uint8_t spriteHeightInBits_;
    uint16_t bitsWritten_;
    uint8_t rowIndex_;
    uint8_t columnIndex_;
};

/**
 * @brief based on https://www.youtube.com/watch?v=aF1Yw_wu2cM&t=237s
 */
static uint16_t decodeRLELVIntoN(BitReader &reader)
{
    uint16_t l = 0;
    uint16_t v = 0;
    uint16_t numLBits = 0;

    // first read L
    do
    {
        l <<= 1;
        l |= reader.read(1);
        ++numLBits;
    } while ((l & 0x1) == 1);

    // now read V
    for (uint16_t i = 0; i < numLBits; ++i)
    {
        // first iteration, v = 0, so it would be a NOOP.
        // last iteration, we shouldn't shift AFTER doing the read
        // so this is why the code is written like this.
        v <<= 1;
        v |= reader.read(1);
    }

    return l + v + 1;
}

static void decodeRLE(BitReader &reader, uint16_t outputLengthInBits, uint8_t *outBuf, uint8_t initialPacketType, uint8_t widthInPixels, uint8_t heightInPixels)
{
    VerticalBitWriter writer(outBuf, widthInPixels, heightInPixels);
    uint8_t bitPair;
    // N is the number of 00 pairs to write to the output buffer
    uint16_t N;
    bool isRLEPacket = (initialPacketType == 0);

    while (writer.getBitsWritten() < outputLengthInBits)
    {
        if (isRLEPacket)
        {
            // now determine how many 00 pairs to write by decoding LV into N
            N = decodeRLELVIntoN(reader);
//            printf("\n Writing %hu 00 pairs. bitsWritten=%hu\n", N, writer.getBitsWritten());
            for (uint16_t i = 0; i < N; ++i)
            {
                if(writer.getBitsWritten() < outputLengthInBits)
                {
                    writer.writePair(00);
                }
                else
                {
                  printf("WARN: can't write 00\n");
                }
            }
        }
        else
        {
            bitPair = reader.read(2);
            while(bitPair != 0)
            {
//              printf("%hhu%hhu ", bitPair >> 1, (bitPair & 0x1));
                writer.writePair(bitPair);

                if(writer.getBitsWritten() >= outputLengthInBits)
                {
                    break;
                }
                bitPair = reader.read(2);
            }
        }
        isRLEPacket = !isRLEPacket;
    }
//  printf("\nend RLE\n");
}

static void decodeDelta(uint8_t *buffer, uint8_t spriteWidthInPixels, uint8_t spriteHeightInPixels)
{
    uint8_t lastBit;
    uint8_t originalValue;
    uint8_t newValue;
    uint8_t bitIndex;
    uint8_t i;
    uint8_t j;
    uint16_t bufferOffset;
    const uint8_t spriteWidthInBytes = spriteWidthInPixels / 8;

    // delta decoding works horizontally. However, the incoming sprite data is stored in a different way:
    // the data is stored column first, meaning that contiguous bytes are not horizontal, but vertical.
    // Another important detail here is that every byte stores 8 horizontal pixels.
    // so that leaves us with some work to calculate the right buffer index in order to work horizontally for delta decoding.

    // now why does the order matter? Well the whole algorithm is based on repeating the value of the last bit we encountered
    // if a 0 is encountered in the input data (at the right position/location that is), then the last bit will be repeated.
    // if a 1 is encountered in the input data, we flip the lastBit
    // so the direction matters for the value of the lastBit at any point in time.
    for(j=0; j < spriteHeightInPixels; ++j)
    {
        // at the start of each row, the state is reset to 0
        lastBit = 0;
        for(i=0; i < spriteWidthInBytes; ++i)
        {
            bufferOffset = (i * spriteHeightInPixels) + j;
            originalValue = (*(buffer + bufferOffset));
            newValue = 0;

            for(bitIndex=0; bitIndex < 8; ++bitIndex)
            {
                newValue <<= 1;
                // if we encounter a 0 bit, we repeat the lastBit
                // if we encounter a 1 bit, we flip the lastBit and output it
                if(originalValue & (1 << (7 - bitIndex)))
                {
                    // flip the lastBit
                    lastBit ^= 0x1;
                }
                newValue |= lastBit;
            }

            (*(buffer + bufferOffset)) = newValue;
        }
    }
}

/**
 * @brief With encodingMode 2 or 3, you need to XOR bitplane 0 and bitplane 1 and store the contents into bitplane1
 */
static void xorBitplanes(uint8_t* bitPlane0, uint8_t* bitPlane1)
{
    uint16_t i = 0;

    while(i < SPRITE_BITPLANE_BUFFER_SIZE_IN_BYTES)
    {
        bitPlane1[i] ^= bitPlane0[i];
        ++i;
    }
}

/**
 * @brief This function does a valign: bottom, halign: center operation on the src buffer content and writes it to the destination buffer
 */
static void centerSprite(uint8_t* src, uint8_t* dst, uint8_t widthInTiles, uint8_t heightInTiles)
{
    const uint8_t maxTiles = 7;
    const uint8_t verticalOffset = maxTiles - heightInTiles;
    const uint8_t horizontalOffset = ((maxTiles - widthInTiles) / 2);
    
    // 8 vertical bytes per tile
    const uint16_t spriteHeightInBytes = heightInTiles * 8;
    const uint16_t outputStartOffset = (horizontalOffset * maxTiles + verticalOffset) * 8;

    uint16_t dstColumnStartOffset;
    uint16_t dstCurOffset;
    uint16_t x;
    uint16_t y;

    memset(dst, 0, SPRITE_BITPLANE_BUFFER_SIZE_IN_BYTES);

    // copy column by column
    // 1 tile horizontally is 1 byte (8 bits per byte -> 1 byte for 8 pixels)
    dstColumnStartOffset = outputStartOffset;
    for(x=0; x < widthInTiles; ++x)
    {
        for(y = 0; y < spriteHeightInBytes; ++y)
        {
            dstCurOffset = dstColumnStartOffset + y;
            dst[dstCurOffset] = src[(x * spriteHeightInBytes + y)];
        }
        dstColumnStartOffset += SPRITE_BUFFER_HEIGHT_IN_PIXELS;
    }
}

/**
 * @brief This function merges the 2 bitplanes into a single buffer.
 * Note that plane1 likely will overlap with destination.
 * This is why the function actually works backwards to avoid overwriting content that still needs to be read.
 * 
 * Also: it is expected that starting from plane1, the buffer at least has SPRITE_BUFFER_SIZE_IN_BYTES* 2 bytes available
 * In other words: the function relies on the fact that plane0, plane1 and plane2 are all part of the same big buffer
 * Because plane1 will be used as if it is SPRITE_BUFFER_SIZE_IN_BYTES * 2 bytes big (thereby occupying the space of plane2 as well)
 * 
 * But we need to talk about the format here. As you may expect, the result is a buffer in which every pixel is 2 bits.
 * But what's unexpected is how this is actually realized:
 * Instead of putting the relevant bits next to eachother, they actually end up in different bytes.
 * 
 * So byte0 contains the least significant bit of the pixel and byte1 contains the most significant bit of the same pixel
 * This is how the gameboy hardware reads it, so this format likely won't be useful as is if you don't intend to display this on a gameboy (emulator?)
 * 
 * Regardless, because of this format, merging the bitplanes is actually quite simple: you just interleave the bytes, not the bits
 * source: https://gbdev.io/gb-asm-tutorial/part1/tiles.html#:~:text=The%20concept%20of%20a%20%E2%80%9Ctile,referred%20to%20as%20meta%2Dtiles. 
 * and: https://rgmechex.com/tech/gen1decompress.html
 */
static void mergeSpriteBitplanes(uint8_t* plane0, uint8_t* plane1, uint8_t* destination)
{
    // using an int in the for loop because otherwise the i >=0 condition won't work (because of underflow)
    for(int i=SPRITE_BITPLANE_BUFFER_SIZE_IN_BYTES - 1; i >= 0; --i)
    {
        destination[i * 2 + 1] = plane1[i];
        destination[i * 2 + 0] = plane0[i];
    }
}

static uint8_t *getSpriteBuffer(uint8_t *bigBuffer, uint8_t index)
{
    return bigBuffer + (index * SPRITE_BITPLANE_BUFFER_SIZE_IN_BYTES);
}

static void dumpSpriteBitplane(uint8_t* spriteBuffer, const char* binPath, const char* rawPath, uint8_t spriteWidthInBytes, uint8_t spriteHeightInPixels)
{
#ifdef DUMP_SPRITEBITPLANES
    uint16_t bufferOffset;

    // convert to PNG like this: 
    // convert -size 56x56 -depth 8 gray:spriteBuffer1.raw output.png
    // by the way, for some reason, imagemagick doesn't properly handle the image if you'd not convert the bitplane to a 8 bit one (and use -depth 8)

    FILE* f = fopen(binPath, "w");
    
    fwrite(spriteBuffer, 1, SPRITE_BITPLANE_BUFFER_SIZE_IN_BYTES, f);
    fclose(f);

    f = fopen(rawPath, "w");

    // so weird thing here: because of the way the gameboy works, the data is stored as 8 horizontal pixels per byte,
    // but every byte is column major order: so after byte 1, the next byte is the next 8 pixels of the next (vertical) row and so on until we reach the bottom of the sprite
    // then the same process continues starting from the top of the next column.
    // so in order to construct an image that can be shown by "normal" programs in a left-to-right, top-to-bottom way
    // we need to seek within spriteBuffer a lot.
    // based on: https://glitchcity.wiki/wiki/Sprite_decompression_(Generation_I)#Bit_order
    for(uint8_t j=0; j < spriteHeightInPixels; ++j)
    {
        for(uint8_t i=0; i < spriteWidthInBytes; ++i)
        {
            bufferOffset = (i * spriteHeightInPixels) + j;

            for(uint8_t bitIndex=0; bitIndex < 8; ++bitIndex)
            {
                const uint8_t byteVal = (*(spriteBuffer + bufferOffset)) & (1 << (7- bitIndex));
                fputc((byteVal) ? 0 : 255, f);
            }
        }
    }
    fclose(f);

    printf("Dumped sprite bitplane buffer to %s and %s\n", binPath, rawPath);
#else
    (void)spriteBuffer;
    (void)binPath;
    (void)rawPath;
    (void)spriteWidthInBytes;
    (void)spriteHeightInPixels;
#endif
}

static void dumpDecodedSprite(uint8_t* spriteBuffer, const char* binPath, const char* rawPath)
{
#ifdef DUMP_SPRITEBITPLANES
    SpriteRenderer renderer;

    const uint32_t rgbBufferSize = renderer.getNumRGBBytesFor(7, 7);
    uint8_t* rgbBuffer;
    
    FILE* f = fopen(binPath, "w"); 
    fwrite(spriteBuffer, 1, SPRITE_BITPLANE_BUFFER_SIZE_IN_BYTES * 2, f);
    fclose(f);

    rgbBuffer = renderer.drawRGB(spriteBuffer, monochromeGBColorPalette, 7, 7);
    
    f = fopen(rawPath, "w");
    fwrite(rgbBuffer, 1, rgbBufferSize, f);
    fclose(f);
#else
    (void)spriteBuffer;
    (void)binPath;
    (void)rawPath;
#endif
}

Gen1SpriteDecoder::Gen1SpriteDecoder(IRomReader &romReader)
    : romReader_(romReader), bigSpriteBuffer_()
{
}

Gen1SpriteDecoder::~Gen1SpriteDecoder()
{
}

uint8_t* Gen1SpriteDecoder::decode(uint8_t bankIndex, uint16_t pointer)
{
    if (!romReader_.seekToRomPointer(pointer, bankIndex))
    {
        return nullptr;
    }

    memset(bigSpriteBuffer_, 0, SPRITE_BITPLANE_BUFFER_SIZE_IN_BYTES * 3);
    BitReader bitReader(romReader_);

    const uint8_t widthInTiles = bitReader.read(4);
    const uint8_t heightInTiles = bitReader.read(4);
    const uint8_t widthInPixels = widthInTiles * 8;
    const uint8_t heightInPixels = heightInTiles * 8;
    const uint8_t primaryBuffer = bitReader.read(1);
    uint8_t initial_packet_type = bitReader.read(1); // first packet is either RLE or data packet

    const uint16_t bitPlaneLengthInBits = widthInPixels * heightInPixels;

    uint8_t* bufferA = getSpriteBuffer(bigSpriteBuffer_, 0);
    uint8_t* bufferB = getSpriteBuffer(bigSpriteBuffer_, 1);
    uint8_t* bufferC = getSpriteBuffer(bigSpriteBuffer_, 2);
    uint8_t* buffer0;
    uint8_t* buffer1;

    // read compressed bitplane
    if(primaryBuffer)
    {
        buffer0 = bufferC;
        buffer1 = bufferB;
    }
    else
    {
        buffer0 = bufferB;
        buffer1 = bufferC;
    }
    decodeRLE(bitReader, bitPlaneLengthInBits, buffer0, initial_packet_type, widthInPixels, heightInPixels);

    uint8_t encodingMode = bitReader.read(1);
    if(encodingMode == 1)
    {
        encodingMode = (encodingMode << 1) | bitReader.read(1);
    }
    else
    {
        encodingMode = 1;
    }
    initial_packet_type = bitReader.read(1); // first packet is either RLE or data packet
    decodeRLE(bitReader, bitPlaneLengthInBits, buffer1, initial_packet_type, widthInPixels, heightInPixels);

    // NOTE: dumpSpriteBitplane does nothing if DUMP_SPRITEBITPLANES is not defined at the top of this source file
    dumpSpriteBitplane(buffer0, "spriteBitplane1_RLE.bin", "spriteBitplane1_RLE.raw", SPRITE_BUFFER_WIDTH_IN_PIXELS / 8, SPRITE_BUFFER_HEIGHT_IN_PIXELS);
    dumpSpriteBitplane(buffer1, "spriteBitplane2_RLE.bin", "spriteBitplane2_RLE.raw", SPRITE_BUFFER_WIDTH_IN_PIXELS / 8, SPRITE_BUFFER_HEIGHT_IN_PIXELS);

    // Okay, phase 1 is done. Now we need to do the operations involving the 2 bitplanes
    switch(encodingMode)
    {
        case 1:
        case 3:
            decodeDelta(buffer1, widthInPixels, heightInPixels);
            decodeDelta(buffer0, widthInPixels, heightInPixels);
            break;
        case 2:
            decodeDelta(buffer0, widthInPixels, heightInPixels);
            break;
        default:
            break;
    }

    dumpSpriteBitplane(buffer0, "spriteBitplane1_delta.bin", "spriteBitplane1_delta.raw", SPRITE_BUFFER_WIDTH_IN_PIXELS / 8, SPRITE_BUFFER_HEIGHT_IN_PIXELS);
    dumpSpriteBitplane(buffer1, "spriteBitplane2_delta.bin", "spriteBitplane2_delta.raw", SPRITE_BUFFER_WIDTH_IN_PIXELS / 8, SPRITE_BUFFER_HEIGHT_IN_PIXELS);

    if((encodingMode & 0x2))
    {
        xorBitplanes(buffer0, buffer1);
    }

    dumpSpriteBitplane(buffer0, "spriteBitplane1_encmode.bin", "spriteBitplane1_encmode.raw", SPRITE_BUFFER_WIDTH_IN_PIXELS / 8, SPRITE_BUFFER_HEIGHT_IN_PIXELS);
    dumpSpriteBitplane(buffer1, "spriteBitplane2_encmode.bin", "spriteBitplane2_encmode.raw", SPRITE_BUFFER_WIDTH_IN_PIXELS / 8, SPRITE_BUFFER_HEIGHT_IN_PIXELS);

    centerSprite(bufferB, bufferA, widthInTiles, heightInTiles);
    centerSprite(bufferC, bufferB, widthInTiles, heightInTiles);

    dumpSpriteBitplane(bufferA, "spriteBitplane1_centered.bin", "spriteBitplane1_centered.raw", SPRITE_BUFFER_WIDTH_IN_PIXELS / 8, SPRITE_BUFFER_HEIGHT_IN_PIXELS);
    dumpSpriteBitplane(bufferB, "spriteBitplane2_centered.bin", "spriteBitplane2_centered.raw", SPRITE_BUFFER_WIDTH_IN_PIXELS / 8, SPRITE_BUFFER_HEIGHT_IN_PIXELS);

    mergeSpriteBitplanes(bufferA, bufferB, bufferB);

    dumpDecodedSprite(bufferB, "sprite.bin", "sprite.rgb");

    return bufferB;
}
