#ifndef GEN1DISTRIBUTIONPOKEMON_H
#define GEN1DISTRIBUTIONPOKEMON_H

#include "Gen1Common.h"

typedef struct Gen1DistributionPokemon
{
    const char* name;
    // name of the original trainer as it came from the distribution machine or "" if irrelevant
    const char* originalTrainer;
    // if regenerateTrainerID = false, use it as is
    // if regenerateTrainerID = true, use it as max bounds or 0 if no bounds
    uint16_t originalTrainerID;
    // this iv_data will override the iv_data that is inside the poke member.
    // this is because we might have different distributions with slight differences of the same pokemon
    uint8_t iv_data[2];
    // set the player name as the original trainer ID
    bool setPlayerAsOriginalTrainer;
    // generate a random trainer ID for the pokemon. see originalTrainerID above
    bool regenerateTrainerID;
    // indicates whether this is a Japanese distribution event pokémon.
    // This is important because we can't support the Trainer OT characters of a Japanese one in other
    // localizations. When injecting a Japanese pokémon into a different localization, we replace the OT with POKEME64.
    // when injecting a US/EU pokémon into a japanese cartridge, we'll replace the OT with ポケ64
    bool isJapanese;
    // overrides the held item field
    uint8_t held_item_override;
    // definition of the pokemon
    Gen1TrainerPokemon poke;
} Gen1DistributionPokemon;

/**
 * @brief Gets a list of const Gen1DistributionPokemon pointers of the distribution pokemon that were available worldwide
 * and stores it in the specified outList.
 * 
 * outSize will be updated with the size of the list
 */
void gen1_getMainDistributionPokemonList(const Gen1DistributionPokemon**& outList, uint32_t& outSize);

#endif