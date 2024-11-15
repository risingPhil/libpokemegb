#ifndef _GEN1COMMON_H
#define _GEN1COMMON_H

#include "common.h"
#include "Gen1Localization.h"

class Gen1GameReader;
class IRomReader;
typedef struct Gen1DistributionPokemon Gen1DistributionPokemon;

enum class Gen1GameType
{
    INVALID,
    BLUE,
    RED,
    YELLOW
};

typedef struct Gen1PokeStats
{
    uint8_t pokedex_number;
    uint8_t base_hp;
    uint8_t base_attack;
    uint8_t base_defense;
    uint8_t base_speed;
    uint8_t base_special;
    uint8_t type1;
    uint8_t type2;
    uint8_t catch_rate;
    uint8_t base_exp_yield;
    uint8_t front_sprite_dimensions;
    uint8_t sprite_bank;
    uint16_t pointer_to_frontsprite;
    uint16_t pointer_to_backsprite;
    uint8_t lvl1_attacks[4];
    uint8_t growth_rate;
    uint8_t tm_hm_flags[7];
} Gen1PokeStats;

typedef struct Gen1TrainerPokemon
{
    uint8_t poke_index;
    uint16_t current_hp;
    uint8_t level;
    uint8_t status_condition;
    uint8_t type1;
    uint8_t type2;
    uint8_t catch_rate_or_held_item;
    uint8_t index_move1;
    uint8_t index_move2;
    uint8_t index_move3;
    uint8_t index_move4;
    uint16_t original_trainer_ID;
    uint32_t exp;
    uint16_t hp_effort_value;
    uint16_t atk_effort_value;
    uint16_t def_effort_value;
    uint16_t speed_effort_value;
    uint16_t special_effort_value;
    uint8_t iv_data[2];
    uint8_t pp_move1;
    uint8_t pp_move2;
    uint8_t pp_move3;
    uint8_t pp_move4;
    uint16_t max_hp;
    uint16_t atk;
    uint16_t def;
    uint16_t speed;
    uint16_t special;
} Gen1TrainerPokemon;

enum Gen1PokeType
{
    GEN1_PT_NORMAL = 0x0,
    GEN1_PT_FIGHTING,
    GEN1_PT_FLYING,
    GEN1_PT_POISON,
    GEN1_PT_GROUND,
    GEN1_PT_ROCK,
    GEN1_PT_BIRD,
    GEN1_PT_BUG,
    GEN1_PT_GHOST,
    GEN1_PT_FIRE = 0x14,
    GEN1_PT_WATER = 0x15,
    GEN1_PT_GRASS = 0x16,
    GEN1_PT_ELECTRIC = 0x17,
    GEN1_PT_PSYCHIC = 0x18,
    GEN1_PT_ICE = 0x19,
    GEN1_PT_DRAGON = 0x1A
};

enum Gen1PokemonIconType
{
    GEN1_ICONTYPE_MON = 0,
    GEN1_ICONTYPE_BALL,
    GEN1_ICONTYPE_HELIX,
    GEN1_ICONTYPE_FAIRY,
    GEN1_ICONTYPE_BIRD,
    GEN1_ICONTYPE_WATER,
    GEN1_ICONTYPE_BUG,
    GEN1_ICONTYPE_GRASS,
    GEN1_ICONTYPE_SNAKE,
    GEN1_ICONTYPE_QUADRUPED,
    GEN1_ICONTYPE_PIKACHU,
    GEN1_ICONTYPE_MAX
};

/**
 * @brief Implementation based on https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_I)#Checksum
 */
class Gen1Checksum
{
public:
    Gen1Checksum();

    void addByte(uint8_t byte);
    uint8_t get() const;
protected:
private:
    uint8_t checksum_;
};

/**
 * @brief Determines a Gen1GameType based on the given GameboyCartridgeHeader struct
 */
Gen1GameType gen1_determineGameType(const GameboyCartridgeHeader& cartridgeHeader);

/**
 * @brief this function determines the games' language
 */
LocalizationLanguage gen1_determineGameLanguage(IRomReader& romReader, Gen1GameType gameType);

void gen1_recalculatePokeStats(Gen1GameReader& gameReader, Gen1TrainerPokemon& poke);

/**
 * @brief This function decodes a text (This could be a name or something else) found in the rom.
 * @return the number of characters copied to the output buffer
 */
uint16_t gen1_decodePokeText(const uint8_t* inputBuffer, uint16_t inputBufferLength, char* outputBuffer, uint16_t outputBufferLength, LocalizationLanguage language);
/**
 * @brief The opposite of gen1_decodePokeText()
 */
uint16_t gen1_encodePokeText(const char* inputBuffer, uint16_t inputBufferLength, uint8_t* outputBuffer, uint16_t outputBufferLength, uint8_t terminator, LocalizationLanguage language);

/**
 * Prepares the gen 1 distribution pokemon for injection with Gen1GameReader::addPokemon()
 */
void gen1_prepareDistributionPokemon(Gen1GameReader& gameReader, const Gen1DistributionPokemon& distributionPoke, Gen1TrainerPokemon& poke, const char*& outTrainerName);

#endif