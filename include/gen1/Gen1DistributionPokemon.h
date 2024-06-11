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
    // definition of the pokemon
    Gen1TrainerPokemon poke;
} Gen1DistributionPokemon;

// https://bulbapedia.bulbagarden.net/wiki/List_of_European_language_event_Pok%C3%A9mon_distributions_(Generation_I)

extern const Gen1DistributionPokemon g1_nintendoUKPokemonFestivalMew2016;
extern const Gen1DistributionPokemon g1_clubNintendoMexicoGiveawayMew;
extern const Gen1DistributionPokemon g1_farMorBornMew;
extern const Gen1DistributionPokemon g1_christmasPresentMew;
extern const Gen1DistributionPokemon g1_swedenMewOnTour;
extern const Gen1DistributionPokemon g1_tennispalatsiMew;
extern const Gen1DistributionPokemon g1_norwayPokemonTourMew;
extern const Gen1DistributionPokemon g1_clubNintendoMew;
extern const Gen1DistributionPokemon g1_pokemon2000ChampionshipMew;
extern const Gen1DistributionPokemon g1_millenniumDomeMew;
extern const Gen1DistributionPokemon g1_austriaMew;
extern const Gen1DistributionPokemon g1_ukIrelandPokemonChampionship2000Mew;
extern const Gen1DistributionPokemon g1_coraChatelineauMew;
extern const Gen1DistributionPokemon g1_francePokemonTournamentMew;
extern const Gen1DistributionPokemon g1_spainPokemonTournamentMew;
extern const Gen1DistributionPokemon g1_mewsFlashMew;
extern const Gen1DistributionPokemon g1_pokemonPatrolMew;
extern const Gen1DistributionPokemon g1_nintendoOfficialMagazineTourMew;
extern const Gen1DistributionPokemon g1_canadianPokemonStadiumTour2000Mew;
extern const Gen1DistributionPokemon g1_pokemonStadiumTour2000Mew;
extern const Gen1DistributionPokemon g1_canadaToysRUsMew;
extern const Gen1DistributionPokemon g1_usToysRUsMew;
extern const Gen1DistributionPokemon g1_nintendoPowerMew;
extern const Gen1DistributionPokemon g1_pokemonLeagueNintendoTrainingTour99Mew;
extern const Gen1DistributionPokemon g1_nintendoPowerPikachu;
extern const Gen1DistributionPokemon g1_pokeTourMew;
extern const Gen1DistributionPokemon g1_pokemonPowerMew;

/**
 * @brief Gets a list of const Gen1DistributionPokemon pointers of the distribution pokemon that were available worldwide
 * and stores it in the specified outList.
 * 
 * outSize will be updated with the size of the list
 */
void gen1_getMainDistributionPokemonList(const Gen1DistributionPokemon**& outList, uint32_t& outSize);

#endif