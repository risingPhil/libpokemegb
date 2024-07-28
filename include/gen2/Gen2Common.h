#ifndef _GEN2COMMON_H
#define _GEN2COMMON_H

#include "common.h"

class Gen2GameReader;
class ISaveManager;

// source: https://bulbapedia.bulbagarden.net/wiki/List_of_items_by_index_number_(Generation_II)
#define CRYSTAL_ITEM_ID_GS_BALL 0x73

// Based on https://github.com/kwsch/PKHeX/blob/master/PKHeX.Core/Resources/text/script/gen2/flags_c_en.txt
// Look at this file to see more events
#define CRYSTAL_EVENTFLAG_KURT_CAN_CHECK_GSBALL 190
#define CRYSTAL_EVENTFLAG_KURT_READY_TO_RETURN_GSBALL 191
#define CRYSTAL_EVENTFLAG_GSBALL_USABLE_IN_ILEX_FOREST_SHRINE 192
#define CRYSTAL_EVENTFLAG_RECEIVED_GSBALL 832

enum class Gen2GameType
{
    INVALID,
    GOLD,
    SILVER,
    CRYSTAL
};

typedef struct Gen2PokeStats
{
    uint8_t pokedex_number;
    uint8_t base_hp;
    uint8_t base_attack;
    uint8_t base_defense;
    uint8_t base_speed;
    uint8_t base_special_attack;
    uint8_t base_special_defense;
    uint8_t type1;
    uint8_t type2;
    uint8_t catch_rate;
    uint8_t base_exp_yield;
    uint8_t wild_held_item1;
    uint8_t wild_held_item2;
    uint8_t gender_ratio;
    uint8_t unknown;
    uint8_t egg_cycles;
    uint8_t unknown2;
    uint8_t front_sprite_dimensions;
    uint8_t blank[4];
    uint8_t growth_rate;
    uint8_t egg_groups;
    uint8_t tm_hm_flags[8];
} Gen2PokeStats;

enum Gen2PokeType
{
    GEN2_PT_NORMAL = 0x0,
    GEN2_PT_FIGHTING,
    GEN2_PT_FLYING,
    GEN2_PT_POISON,
    GEN2_PT_GROUND,
    GEN2_PT_ROCK,
    GEN2_PT_BIRD,
    GEN2_PT_BUG,
    GEN2_PT_GHOST,
    GEN2_PT_STEEL,
    GEN2_PT_FIRE = 0x14,
    GEN2_PT_WATER = 0x15,
    GEN2_PT_GRASS = 0x16,
    GEN2_PT_ELECTRIC = 0x17,
    GEN2_PT_PSYCHIC = 0x18,
    GEN2_PT_ICE = 0x19,
    GEN2_PT_DRAGON = 0x1A,
    GEN2_PT_DARK = 0x1B
};

enum Gen2GrowthRate
{
    GEN2_GRW_MEDIUM_FAST = 0,
    GEN2_GRW_SLIGHTLY_FAST = 1,
    GEN2_GRW_SLIGHTLY_SLOW = 2,
    GEN2_GRW_MEDIUM_SLOW = 3,
    GEN2_GRW_FAST = 4,
    GEN2_GRW_SLOW = 5
};

enum Gen2ItemListType
{
    GEN2_ITEMLISTTYPE_INVALID,
    GEN2_ITEMLISTTYPE_ITEMPOCKET,
    GEN2_ITEMLISTTYPE_KEYITEMPOCKET,
    GEN2_ITEMLISTTYPE_BALLPOCKET,
    GEN2_ITEMLISTTYPE_PC,
    GEN2_ITEMLISTTYPE_MAX
};

typedef struct Gen2TrainerPokemon
{
    uint8_t poke_index;
    uint8_t held_item_index;
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
    uint8_t friendship_or_remaining_egg_cycles;
    uint8_t pokerus;
    uint16_t caught_data;
    uint8_t level;
    uint8_t status_condition;
    uint8_t unused_byte;
    uint16_t current_hp;
    uint16_t max_hp;
    uint16_t atk;
    uint16_t def;
    uint16_t speed;
    uint16_t special_atk;
    uint16_t special_def;
} Gen2TrainerPokemon;

class Gen2ItemList
{
public:
    Gen2ItemList(ISaveManager& saveManager, Gen2ItemListType type, bool isCrystal);

    uint8_t getCount();
    uint8_t getCapacity();
    
    /**
     * Gets the entry info at the specified index in the item list
     */
    bool getEntry(uint8_t index, uint8_t& outItemId, uint8_t& outCount);
    
    /**
     * @brief Gets the next entry's info in the item list.
     * The reason this exists is to avoid the seek that would be done on every call to getEntry()
     * 
     * If the previous function call doing stuff on saveManager_ was the getCount() function, you can
     * bypass a call to getEntry() and call this getNextEntry() function immediately
     */
    bool getNextEntry(uint8_t& outItemId, uint8_t& outCount);

    /**
     * @brief This function adds an item (or multiple of that specific item) to the item list and returns true if this was successful
     * WARNING: You must call finishSave() on the Gen2GameReader after calling this function. Not doing so will corrupt your save file!
     */
    bool add(uint8_t itemId, uint8_t itemCount);

    bool remove(uint8_t itemId);
protected:
private:
    bool seekToBasePos();

    ISaveManager& saveManager_;
    const Gen2ItemListType type_;
    const bool isCrystal_;
};

class Gen2Checksum
{
public:
    Gen2Checksum();

    void addByte(uint8_t byte);
    uint16_t get() const;
protected:
private:
    uint16_t checksum_;
};

/**
 * @brief Determines a Gen2GameType based on the given GameboyCartridgeHeader struct
 */
Gen2GameType gen2_determineGameType(const GameboyCartridgeHeader& cartridgeHeader);

void gen2_recalculatePokeStats(Gen2GameReader& gameReader, Gen2TrainerPokemon& poke);

/**
 * @brief This function decodes a text (This could be a name or something else) found in the rom.
 * @return the number of characters copied to the output buffer
 */
uint16_t gen2_decodePokeText(const uint8_t* inputBuffer, uint16_t inputBufferLength, char* outputBuffer, uint16_t outputBufferLength);
/**
 * @brief The opposite of gen1_decodePokeText()
 */
uint16_t gen2_encodePokeText(const char* inputBuffer, uint16_t inputBufferLength, uint8_t* outputBuffer, uint16_t outputBufferLength, uint8_t terminator);

/**
 * @brief This function determines whether the given trainer pokemon is shiny
 */
bool gen2_isPokemonShiny(Gen2TrainerPokemon &poke);

/**
 * @brief This function makes the necessary IV changes to make the pokemon shiny
 */
void gen2_makePokemonShiny(Gen2TrainerPokemon& poke);

const char* gen2_getItemListTypeString(Gen2ItemListType type);

#endif