#include "utils.h"
#include "common.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>

//#define PNG_SUPPORT

#ifdef PNG_SUPPORT
#include <png.h>
#endif

const char* findCharsByTextCode(const TextCodePair* textCodes, uint16_t numEntries, uint8_t code)
{
	const TextCodePair* const end = textCodes + numEntries;
	const TextCodePair* cur = textCodes;

	while(cur < end)
	{
		if(cur->code == code)
		{
			return cur->chars;
		}
		++cur;
	}
	return nullptr;
}

bool findTextcodeByString(const TextCodePair* textCodes, uint16_t numEntries, const char* input, uint16_t inputLength, uint8_t& outCode, uint16_t& needleLength)
{
	const TextCodePair* const end = textCodes + numEntries;
	const TextCodePair* cur = textCodes;

	while(cur < end)
	{
		needleLength = strlen(cur->chars);

		if(needleLength > inputLength)
		{
			++cur;
			continue;
		}

		if(strncmp(input, cur->chars, needleLength) == 0)
		{
			outCode = cur->code;
			return true;
		}
		++cur;
	}

	needleLength = 0;
	outCode = 0;
    return false;
}

const uint8_t* memSearch(const uint8_t* haystack, uint32_t haystackLength, const uint8_t* needle, uint32_t needleLength)
{
    const uint8_t* cur = haystack;
    const uint8_t* const end = haystack + haystackLength - needleLength;
    const uint8_t* ret;
    while(cur < end)
    {
        ret = (const uint8_t*)memchr(haystack, needle[0], (end - cur));
        if(!ret)
        {
            // the first byte of the needle is nowhere to be found.
            return NULL;
        }
        
        if(!memcmp(ret, needle, needleLength))
        {
            // found the needle -> we can return this pointer as a result
            return ret;
        }

        cur = ret + 1;
    }
    return NULL;
}

uint16_t decodeText(const struct TextCodePair* textCodes, uint16_t numTextCodes, const uint8_t* inputBuffer, uint16_t inputBufferLength, char* outputBuffer, uint16_t outputBufferLength)
{
    uint16_t result = 0;
    if(inputBufferLength > outputBufferLength)
    {
        return result;
    }

    const uint8_t* curInput = (const uint8_t*)inputBuffer;
    const uint8_t* endInput = curInput + inputBufferLength;
    uint8_t* curOutput = (uint8_t*)outputBuffer;
    uint16_t remainingBufferSpace = outputBufferLength;

    while(curInput < endInput)
    {
        if((*curInput) == 0x50)
        {
            break;
        }
        else
        {
			const char* outputText = findCharsByTextCode(textCodes, numTextCodes, (*curInput));
            const size_t outputTextLength = strlen(outputText);

            if(remainingBufferSpace < outputTextLength + 1)
            {
                // not enough space left: truncated
                memcpy(curOutput, outputText, remainingBufferSpace - 1);
                outputBuffer[outputBufferLength -1] = '\0';
                return outputBufferLength - 1;
            }

            memcpy(curOutput, outputText, outputTextLength);
            curOutput += outputTextLength;
            result += outputTextLength;
            remainingBufferSpace -= outputTextLength;

            ++curInput;
        }
    }
	// terminator byte
	(*curOutput) = '\0';
    return result;
}

uint16_t encodeText(const struct TextCodePair* textCodes, uint16_t numTextCodes, const char* inputBuffer, uint16_t inputBufferLength, uint8_t* outputBuffer, uint16_t outputBufferLength, uint8_t terminator)
{
	uint16_t needleLength;
	uint8_t code;
	const char* cur = inputBuffer;
	const char* const inputBufferEnd = inputBuffer + inputBufferLength;
	uint8_t* outputCur = outputBuffer;
	uint8_t* const outputBufferEnd = outputBuffer + outputBufferLength;
	bool match;

	while(cur < inputBufferEnd)
	{
		match = false;
		findTextcodeByString(textCodes, numTextCodes, cur, inputBufferLength - (cur - inputBuffer), code, needleLength);
		if(code)
		{
			// match: every match results in 1 byte being added to the outputbuffer. That's the whole point of encoding this stuff
			(*outputCur) = code;
			if(outputCur == outputBufferEnd - 1)
			{
				// reached the end of the outputbuffer. So truncate
				(*outputCur) = terminator;
				// for consistency, we need to include the terminator in the byte count returned
				++outputCur;
				return static_cast<uint16_t>(outputCur - outputBuffer);
			}
			else
			{
				match = true;
				++outputCur;
				cur += needleLength;
			}
		}

		if(!match)
		{
			printf("Error trying to encode text!\n");
			break;
		}
	}
	(*outputCur) = terminator;
	// for consistency, we need to include the terminator in the byte count returned
	++outputCur;
	return static_cast<uint16_t>(outputCur - outputBuffer);
}

// the attribute stuff is to make sure the compiler doesn't optimize this code away.
__attribute__((optimize("O0")))
bool isCurrentCPULittleEndian()
{
	short int word = 0x0001;
   char *byte = (char *) &word;
   return (byte[0] ? true : false);
}

uint16_t byteSwapUint16(uint16_t val)
{
    return __builtin_bswap16(val);
}

uint32_t byteSwapUint32(uint32_t val)
{
	return __builtin_bswap32(val);
}

bool write_png(const char *filename, unsigned char *rgb_data, int width, int height, bool hasAlpha)
{
#ifdef PNG_SUPPORT
    const int numColorComponents = (hasAlpha) ? 4 : 3;
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        return false;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(fp);
        return false;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        return false;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return false;
    }

    png_init_io(png, fp);

    const int colorType = (hasAlpha) ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB;
    // Set the image information
    png_set_IHDR(png, info, width, height, 8, colorType,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, info);

    // Write the image data
    for (int y = 0; y < height; y++) {
        png_bytep row_pointer = (png_bytep)(rgb_data + y * width * numColorComponents);
        png_write_row(png, row_pointer);
    }

    png_write_end(png, NULL);

    png_destroy_write_struct(&png, &info);
    fclose(fp);

    return true;
#else
	(void)rgb_data;
	(void)width;
	(void)height;
    (void)hasAlpha;
	
	fprintf(stderr, "Error: Can't write %s! PNG support was not enabled at build time!\n", filename);
	return false;
#endif
}

uint8_t* readFileIntoBuffer(const char* pathToFile, uint32_t& outFileSize)
{
    FILE* file = fopen(pathToFile, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    // Seek to the end of the file to get the file size
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("Failed to seek to end of file");
        fclose(file);
        return NULL;
    }
    
    const long size = ftell(file);
    if (size == -1) {
        perror("Failed to get file size");
        fclose(file);
        return NULL;
    }

    // Return to the beginning of the file
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("Failed to seek to beginning of file");
        fclose(file);
        return NULL;
    }

    // Allocate buffer to hold the file contents
    uint8_t* buffer = (uint8_t*)malloc(size);
    if (!buffer) {
        perror("Failed to allocate buffer");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    size_t bytesRead = fread(buffer, 1, size, file);
    if (bytesRead != static_cast<uint32_t>(size)) {
        perror("Failed to read file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    // Close the file and return the buffer
    fclose(file);
    outFileSize = static_cast<uint32_t>(size);
    return buffer;
}
