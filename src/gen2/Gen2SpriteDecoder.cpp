#include "gen2/Gen2SpriteDecoder.h"
#include "RomReader.h"

// Lookup table for reversing bits of a byte
static const uint8_t reverse_table[256] = {
    0,  128, 64, 192, 32, 160,  96, 224, 16, 144, 80, 208, 48, 176, 112, 240,
    8,  136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248,
    4,  132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244,
    12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252,
    2,  130, 66, 194, 34, 162,  98, 226, 18, 146, 82, 210, 50, 178, 114, 242,
    10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250,
    6,  134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246,
    14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254,
    1,  129, 65, 193, 33, 161,  97, 225, 17, 145, 81, 209, 49, 177, 113, 241,
    9,  137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249,
    5,  133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245,
    13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253,
    3,  131, 67, 195, 35, 163,  99, 227, 19, 147, 83, 211, 51, 179, 115, 243,
    11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251,
    7,  135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215, 55, 183, 119, 247,
    15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255
};

// Function to reverse bits in a uint8_t
static uint8_t reverse_bits(uint8_t byte)
{
    return reverse_table[byte];
}

Gen2SpriteDecoder::Gen2SpriteDecoder(IRomReader& romReader)
    : romReader_(romReader)
    , buffer_()
    , cur_(buffer_)
{
}

Gen2SpriteDecoder::~Gen2SpriteDecoder()
{
}

uint8_t* Gen2SpriteDecoder::decode()
{
    uint8_t byte;
    uint8_t command;
    uint16_t count;

    cur_ = buffer_;

    romReader_.readByte(byte);
    while(byte != 0xFF)
    {
        command = (byte >> 5);
        count = (byte & 0x1f);
        decodeCommand(command, count);
        romReader_.readByte(byte);
    }

    return buffer_;
}

uint8_t* Gen2SpriteDecoder::decode(uint8_t bankIndex, uint16_t pointer)
{
    if (!romReader_.seekToRomPointer(pointer, bankIndex))
    {
        return nullptr;
    }

    return decode();
}

static void swap(uint8_t& b1, uint8_t& b2)
{
    const uint8_t temp = b1;
    b1 = b2;
    b2 = temp;
}

/**
 * Based on docs/gen2.txt and https://www.nesdev.org/wiki/Tile_compression
 */
void Gen2SpriteDecoder::decodeCommand(uint8_t command, uint16_t count)
{
    uint8_t* end;
    uint8_t byte1;
    uint8_t byte2;

    switch(command)
    {
        case 0:
            // copy the next c + 1 bytes to the buffer
            romReader_.read(cur_, count + 1);
            cur_ += (count + 1);
            break;
        case 1:
        {
            // Read another byte, and write it to the output c + 1 times.
            romReader_.readByte(byte1);
            end = cur_ + (count + 1);
            while(cur_ < end)
            {
                (*cur_) = byte1;
                ++cur_;
            }
            break;
        }
        case 2:
        {
            // Read another byte b1 and byte b2, and write byte b1 to the output c + 1 times, swapping b1 and b2 after each write.
            romReader_.readByte(byte1);
            romReader_.readByte(byte2);
            end = cur_ + (count + 1);
            while(cur_ < end)
            {
                (*cur_) = byte1;
                ++cur_;
                swap(byte1, byte2);
            }

            break;
        }
        case 3:
            // Write a zero byte (0x00) to the output c + 1 times.
            end = cur_ + (count + 1);
            while(cur_ < end)
            {
                (*cur_) = 0;
                ++cur_;
            }
            break;
        case 4:
        case 5:
        case 6:
            // process commands that reuse already decoded data
            processReuseCommand(command, count);
            break;
        case 7:
            romReader_.readByte(byte1);
            command = (count >> 2);
            count = ((count & 3) << 8) | byte1;
            decodeCommand(command, count);
            break;
    }
}

/**
 * Based on docs/gen2.txt and https://www.nesdev.org/wiki/Tile_compression
 */
void Gen2SpriteDecoder::processReuseCommand(uint8_t command, uint16_t count)
{
    uint32_t offset;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t* readCur;
    uint8_t* end;

    // Read byte b. If b < 0x80, then read byte b2; offset is b×256+b2 bytes from the output stream beginning. 
    // Else offset = b bytes behind from the current output stream end.
    romReader_.readByte(byte1);
    if(byte1 < 0x80)
    {
        romReader_.readByte(byte2);
        offset = (((uint16_t)byte1) * 256) + byte2;
        readCur = buffer_ + offset;
    }
    else
    {
        offset = (byte1 & 0x7F);
        readCur = cur_ - offset - 1;
    }

    switch(command)
    {
        case 4:
            // Copy c + 1 bytes from the output stream at offset to the output.
            // WARNING: this copy needs to be done byte per byte, as you can have an overlapping range with readCur and cur_
            //  (dugtrio, jigglypuff, ...). Using a single memcpy() instead causes graphical artefacts.
            end = cur_ + (count + 1);
            while(cur_ < end)
            {
                (*cur_) = (*readCur);
                ++readCur;
                ++cur_;
            }
            break;
        case 5:
        {
            // Copy c + 1 bytes from the output stream at offset to the output, reversing the bits in each byte.
            end = cur_ + (count + 1);
            while(cur_ < end)
            {
                (*cur_) = reverse_bits((*readCur));
                ++readCur;
                ++cur_;
            }
            break;
        }
        case 6:
            // Copy c + 1 bytes from the output stream at offset to the output, decrementing the read position after each write (i.e. reverse the data)
            end = cur_ + (count + 1);
            while(cur_ < end)
            {
                (*cur_) = (*readCur);
                --readCur;
                ++cur_;
            }
            break;
    }
}