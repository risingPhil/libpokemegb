#ifndef _GEN2LOCALIZATION_H
#define _GEN2LOCALIZATION_H

#include <cstdint>

enum class Gen2GameType;

enum class Gen2LocalizationLanguage
{
    ENGLISH,
    FRENCH,
    ITALIAN,
    SPANISH,
    GERMAN,
    /**
     * Warning: pokémon name decoding is not supported for Korean games!
     * It has a much more difficult character encoding scheme than the other languages (including Japanese)
     * https://bulbapedia.bulbagarden.net/wiki/Korean_character_encoding_(Generation_II)
     *
     * I'm personally not that interested in supporting that. Sorry. I'd welcome contributions for it though!
     */
    KOREAN,
    JAPANESE,
    /**
     * This value indicates that we haven't found a supported localization. It can also be used as a marker to
     * indicate that the game has not been identified yet.
     */
    MAX
};

/**
 * This struct contains the relevant rom offsets for a specific Gen II game localization.
 * Because of the localization process, data has shifted to different positions inside the rom
 * So we need separate rom offsets for each of these localization+game type variation.
 */
typedef struct Gen2LocalizationRomOffsets
{
    /**
     * Rom offset of the species structs which define the base stats of a pokémon in the pokedex number order
     */
    uint32_t stats;
    /**
     * Rom offset of the pokémon name definitions in pokedex number order
     */
    uint32_t names;
    /**
     * Rom offset of the list of icon types per pokémon in the pokedex number order
     */
    uint32_t iconTypes;
    /**
     * Rom offset for the icon pointer table for the party mini-sprite icons
     */
    uint32_t icons;
    /**
     * Rom offset for the list of pokémon sprite pointers. (front + back)
     */
    uint32_t spritePointers;
    /**
     * Rom offset for the list of pokémon sprite palettes. Each pokémon has 2 palettes: non-shiny and shiny.
     * For each of the palettes, only 2 16-bit colors are defined. The other two of the 4-color gameboy colorpalette
     * are always black and white, so they don't need to be stored in this list
     */
    uint32_t spritePalettes;
} Gen2LocalizationRomOffsets;

typedef struct Gen2LocalizationSRAMOffsets
{
    uint32_t currentBoxIndex;
    uint32_t currentBox;
    uint32_t dexSeen;
    uint32_t dexOwned;
    uint32_t party;
    uint32_t eventFlags;
    uint32_t mainChecksum;
    uint32_t mainChecksumDataEndPos;
    uint32_t backupChecksum;
    uint32_t rivalName;
    uint32_t gsBallMain;
    uint32_t gsBallBackup;
    uint32_t itemPocketTMHM;
    uint32_t itemPocketItem;
    uint32_t itemPocketKey;
    uint32_t itemPocketBall;
    uint32_t itemPocketPC;
    uint32_t rtcFlags;
    uint32_t mysteryGiftUnlocked;
    /**
     * This field indicates the offset of the player's day settings in relation to
     * the rtc chips' value at the time.
     *
     * Similar fields for hours, minutes and seconds follow right after this one.
     *
     * Based on the wStartDay in pokegold https://github.com/pret/pokegold/blob/symbols/pokegold.sym
     * if you look at TryLoadSaveData in save.asm, you'll see that
     * the sram offset is sPlayerData + (wStartDay - wPlayerData)
     */
    uint32_t rtcDayOffset;
} Gen2LocalizationSRAMOffsets;

const Gen2LocalizationRomOffsets& gen2_getRomOffsets(Gen2GameType gameType, Gen2LocalizationLanguage language);
const Gen2LocalizationSRAMOffsets& gen2_getSRAMOffsets(Gen2GameType gameType, Gen2LocalizationLanguage language);

#endif