#ifndef _GEN1LOCALIZATION_H
#define _GEN1LOCALIZATION_H

#include "gen1/Gen1Common.h"

/**
 * @brief This enum defines the supported localization languages for gen 1.
 * MAX is invalid. It is defined to be used in a for loop if needed.
 * It can also be used as an invalid value to force Gen1GameReader to identify
 * the localization language
 */
enum class Gen1LocalizationLanguage
{
    ENGLISH,
    FRENCH,
    SPANISH,
    GERMAN,
    ITALIAN,
    JAPANESE,
    MAX
};

/**
 * @brief Different pokémon localizations unfortunately also store data at different locations in the rom.
 * This is because stuff shifts around because of the different text.
 *
 * So to deal with different localizations of the same game, we need to keep track of the various rom offsets we need
 * for each of them.
 *
 */
typedef struct Gen1LocalizationRomOffsets
{
    /**
     * @brief The rom offset at which the species structs can be found containing the base stats
     * of the various pokémon. https://bulbapedia.bulbagarden.net/wiki/Pok%C3%A9mon_species_data_structure_(Generation_I)
     */
    uint32_t stats;
    /**
     * @brief For some reason, in the Pokémon Blue and Red versions, the mew species(stats) struct is stored separately.
     * This does not apply to Pokémon Yellow
     */
    uint32_t statsMew;
    /**
     * @brief This is the rom offset at which the pokedex numbers of pokémon are stored in pokemon index order
     */
    uint32_t numbers;
    /**
     * @brief This is the rom offset at which we can find the encoded pokémon names.
     * These can be decoded with gen1_decodePokeText()
     */
    uint32_t names;

    /**
     * @brief This is the rom offset at which we can find the party menu icon type for each pokémon in pokedex number order.
     */
    uint32_t iconTypes;
    /**
     * @brief This is the rom offset at which we can find the MonPartySpritePointers pointer table
     * This defines where the icon sprites are stored in the rom (and the number of tiles)
     *
     * see https://github.com/pret/pokered/blob/master/data/icon_pointers.asm
     */
    uint32_t icons;

    /**
     * @brief This is the rom offset at which a palette index is stored for each pokémon in pokedex number order.
     */
    uint32_t paletteIndices;

    /**
     * @brief This is the rom offset at which each pokémon sprite color palette is stored in palette index order
     */
    uint32_t palettes;
} Gen1LocalizationRomOffsets;

typedef struct Gen1LocalizationSRAMOffsets
{
    uint32_t trainerID;
    uint32_t currentMap;
    uint32_t dexSeen;
    uint32_t dexOwned;
    uint32_t rivalName;
    uint32_t party;
    uint32_t currentBoxIndex;
    uint32_t checksum;
} Gen1LocalizationSRAMOffsets;

const Gen1LocalizationRomOffsets& gen1_getRomOffsets(Gen1GameType gameType, Gen1LocalizationLanguage language);
const Gen1LocalizationSRAMOffsets& gen1_getSRAMOffsets(Gen1LocalizationLanguage language);
#endif