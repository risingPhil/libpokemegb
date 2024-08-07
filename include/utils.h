#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

struct TextCodePair;

/**
 * @brief Searches the specified buffer (haystack) for a sequence of bytes (needle)
 * @return uint8_t* NULL if not found, non-null if found
 */
const uint8_t* memSearch(const uint8_t* haystack, uint32_t hayStackLength, const uint8_t* needle, uint32_t needleLength);

/**
 * @brief This function decodes a text (This could be a name or something else) found in the rom based on the textCodes list you pass in.
 * @return the number of characters copied to the output buffer
 */
uint16_t decodeText(struct TextCodePair* textCodes, uint16_t numTextCodes, const uint8_t* inputBuffer, uint16_t inputBufferLength, char* outputBuffer, uint16_t outputBufferLength);

/**
 * @brief The opposite of decodeText()
 */
uint16_t encodeText(struct TextCodePair* textCodes, uint16_t numTextCodes, const char* inputBuffer, uint16_t inputBufferLength, uint8_t* outputBuffer, uint16_t outputBufferLength, uint8_t terminator);

bool isCurrentCPULittleEndian();

uint16_t byteSwapUint16(uint16_t val);
uint32_t byteSwapUint32(uint32_t val);

/**
 * Function to write an RGB24 image to a PNG file
 */
bool write_png(const char *filename, unsigned char *rgb_data, int width, int height, bool hasAlpha = false);

/**
 * @brief This function reads an entire file into a buffer.
 * The caller needs to free() it after using it.
 * 
 * @return a pointer to the allocated buffer. If something failed, NULL will be returned.
 */
uint8_t* readFileIntoBuffer(const char* pathToFile, uint32_t& outFileSize);
#endif