#ifndef _GEN2DISTRIBUTIONPOKEMON_H
#define _GEN2DISTRIBUTIONPOKEMON_H

#include "gen2/Gen2Common.h"

typedef struct Gen2DistributionPokemon
{
    const char* name;
    // name of the original trainer as it came from the distribution machine or "" if irrelevant
    // ignored if setPlayerAsOriginalTrainer == true
    const char* originalTrainer;
    // if regenerateTrainerID = false, use it as is
    // if regenerateTrainerID = true, use it as max bounds or 0 if no bounds
    // ignored if setPlayerAsOriginalTrainer == true
    uint16_t originalTrainerID;
    // set the player name as the original trainer ID
    bool setPlayerAsOriginalTrainer;
    // generate a random trainer ID for the pokemon. see originalTrainerID above
    // ignored if setPlayerAsOriginalTrainer == true
    bool regenerateTrainerID;
    // value between 0-100 that indicates the chance for a shiny variant
    // if the value is set to 0xFF, this field should be ignored
    // this is mostly used for simulating the chances of the PKCNY pokemon
    uint8_t shinyChance;
    // a level to override the default level in Gen2TrainerPokemon with.
    // if set to 0, this field should be ignored
    uint8_t overrideLevel;
    // value indicates that this pokémon is given as an egg
    bool isEgg;
    // this iv_data will override the iv_data that is inside the poke member.
    // this is because we might have different distributions with slight differences of the same pokemon
    // ignored if randomizeIVs == true
    // could also randomly be ignored if shinyChance != 0xFF
    uint8_t iv_data[2];
    // this value indicates whether the iv_data field above should be used
    // or whether the IVs need to be randomized instead
    // shinyChance still applies here if set to a value != 0xFF
    bool randomizeIVs;
    // indicates whether this is a Japanese distribution event pokémon.
    // This is important because we can't support the Trainer OT characters of a Japanese one in other
    // localizations. When injecting a Japanese pokémon into a different localization, we replace the OT with POKEME64.
    // when injecting a US/EU pokémon into a japanese cartridge, we'll replace the OT with ポケ64
    bool isJapanese;
    // definition of the pokemon
    Gen2TrainerPokemon poke;
} Gen2DistributionPokemon;

// Note: see gen2_getMainDistributionPokemonList() and gen2_getPokemonCenterNewYorkDistributionPokemonList() below

/**
 * @brief Gets a list of const Gen2DistributionPokemon pointers of the distribution pokemon that were available worldwide
 * and stores it in the specified outList.
 * 
 * outSize will be updated with the size of the list
 */
void gen2_getMainDistributionPokemonList(const Gen2DistributionPokemon**& outList, uint32_t& outSize);

/**
 * @brief Gets a list of const Gen2DistributionPokemon pointers of the distribution pokemon that were available at
 * the Pokémon Center New York (PCNY) and stores it in the specified outList.
 * 
 * outSize will be updated with the size of the list
 */
void gen2_getPokemonCenterNewYorkDistributionPokemonList(const Gen2DistributionPokemon**& outList, uint32_t& outSize);
#endif