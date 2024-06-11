#ifndef _GEN2SPRITEDECODER_H
#define _GEN2SPRITEDECODER_H

#include <cstdint>

#define SPRITE_BUFFER_WIDTH_IN_PIXELS 56
#define SPRITE_BUFFER_HEIGHT_IN_PIXELS 56
#define GEN2_SPRITE_BUFFER_SIZE_IN_BYTES ((SPRITE_BUFFER_WIDTH_IN_PIXELS / 4) * SPRITE_BUFFER_HEIGHT_IN_PIXELS)

// Pokemon crystal has animated sprites. So a front sprite could consist of several frames.
// Some of the commands are therefore also referencing data that could be outside the bounds of a single frame.
// we need to ensure our buffer is large enough to cope with this.
// Essentially, the buffer should be large enough to contain the maximum number of frames used in pokémon crystal for a single sprite
// the number assigned to this define == the max number of frames you're expecting.
#define POKEMON_CRYSTAL_SAFETY_FACTOR 5
class IRomReader;

class Gen2SpriteDecoder
{
public:
    Gen2SpriteDecoder(IRomReader& romReader);
    ~Gen2SpriteDecoder();

    /**
     * @brief Decodes the sprite that starts at the current position inside the romReader specified in the constructor.
     * 
     * @return uint8_t* returns a pointer to an internal buffer containing the decoded sprite in gameboy format. If you need to convert to RGB, please use SpriteDecoder
     * You don't own the returned buffer. In fact, the content will get overwritten on the next call to the decode() function. If you need to keep the data around, consider
     * copying it to a separate buffer.
     */
    uint8_t* decode();

    /**
     * @brief Decodes the sprite that starts in the given bank at the given pointer address. The pointer address should be a value between 0x4000 - 0x8000 because it refers
     * to the address of bank 1 after it has been mapped to the address range of the gameboy.
     * 
     * @return uint8_t* returns a pointer to an internal buffer containing the decoded sprite in gameboy format. If you need to convert to RGB, please use SpriteDecoder
     * You don't own the returned buffer. In fact, the content will get overwritten on the next call to the decode() function. If you need to keep the data around, consider
     * copying it to a separate buffer.
     */
    uint8_t* decode(uint8_t bankIndex, uint16_t pointer);
protected:
private:
    void decodeCommand(uint8_t command, uint16_t count);

    /**
     * @brief This function processes one of the 3 commands
     * that look back into the data that was already filled in our buffer
     * and reuses it
     * 
     * There's some common code for these commands, that's why this function exists
     */
    void processReuseCommand(uint8_t command, uint16_t count);

    IRomReader& romReader_;
    uint8_t buffer_[GEN2_SPRITE_BUFFER_SIZE_IN_BYTES * POKEMON_CRYSTAL_SAFETY_FACTOR];
    uint8_t* cur_;
};

#endif