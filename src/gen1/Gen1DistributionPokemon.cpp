#include "gen1/Gen1DistributionPokemon.h"
#include "common.h"

static const Gen1TrainerPokemon commonMewDefinition = {
    0x15,
    1, // I don't want to bother with figuring out the max hp here, so I set current hp to 1
    5,
    0,
    0x18,
    0x18,
    45, // based on Gen1PokeStats
    0x1, // pound
    0x0,
    0x0,
    0x0,
    0,
    getExpNeededForLevel(5, 3),
    0,
    0,
    0,
    0,
    0,
    {0xA1, 0xC5},
    35,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static const Gen1TrainerPokemon surfingPikachu = {
    0x54,
    1,
    5,
    0,
    0x17,
    0x17,
    190,
    84,
    45,
    57,
    0,
    0,
    getExpNeededForLevel(5, 0),
    0,
    0,
    0,
    0,
    0,
    {0, 0},
    30,
    40,
    15,
    0,
    0,
    0,
    0,
    0,
    0
};

const Gen1DistributionPokemon g1_nintendoUKPokemonFestivalMew2016 = {
    "Nintendo UK Pokemon Festival Mew 2016",
    "GF",
    22796,
    {0xFF, 0xFF},
    false,
    false,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_clubNintendoMexicoGiveawayMew = {
    "Club Nintendo Mexico Giveaway Mew",
    "ASH",
    45515,
    {0xA1, 0xC5},
    false,
    false,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_farMorBornMew = {
    "Far, Mor & Born Mew",
    "",
    0,
    {0xA1, 0xC5},
    true,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_christmasPresentMew = {
    "Christmas Present Mew",
    "",
    0,
    {0xA1, 0xC5},
    true,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_swedenMewOnTour = {
    "Sweden Mew On Tour",
    "",
    0,
    {0xA1, 0xC5},
    true,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_tennispalatsiMew = {
    "Tennispalatsi Mew",
    "FINLAND",
    1400,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_norwayPokemonTourMew = {
    "Norway Pokémon Tour Mew",
    "EUROPE",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_clubNintendoMew = {
    "Club Nintendo Mew",
    "EUROPE",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_pokemon2000ChampionshipMew = {
    "Pokémon 2000 World Championship Mew",
    "",
    0,
    {0xA1, 0xC5},
    true,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_millenniumDomeMew = {
    "Millennium Dome Mew",
    "",
    0,
    {0xA1, 0xC5},
    true,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_austriaMew = {
    "Austria Mew",
    "AUSTRIA",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_ukIrelandPokemonChampionship2000Mew = {
    "UK and Ireland Pokémon Championship 2000 Mew",
    "UK",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_coraChatelineauMew = {
    "Cora Châtelineau Mew",
    "BENELUX",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_francePokemonTournamentMew = {
    "France Pokémon Tournament Mew",
    "",
    0,
    {0xA1, 0xC5},
    true,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_spainPokemonTournamentMew = {
    "Spain Pokémon Tournament Mew",
    "EUROPE",
    0,
    {0x64, 0x38},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_mewsFlashMew = {
    "Mews Flash Mew",
    "UK",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_pokemonPatrolMew = {
    "Pokémon Patrol Mew",
    "YOSHIBB",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_nintendoOfficialMagazineTourMew = {
    "Nintendo Official Magazine Tour Mew",
    "UK",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_canadianPokemonStadiumTour2000Mew = {
    "Canadian Pokémon Stadium Tour 2000 Mew",
    "LUIGIC",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_pokemonStadiumTour2000Mew = {
    "Pokémon 2000 Stadium Tour Mew",
    "LUIGE",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_canadaToysRUsMew = {
    "Canada Toys \"R\" Us Mew",
    "YOSHIRA",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_usToysRUsMew = {
    "U.S. Toys \"R\" Us Mew",
    "YOSHIBA",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_nintendoPowerMew = {
    "Nintendo Power Mew",
    "YOSHIRB",
    1000,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_pokemonLeagueNintendoTrainingTour99Mew = {
    "Pokémon League Nintendo Training Tour '99 Mew",
    "YOSHIRA",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_nintendoPowerPikachu = {
    "Nintendo Power Pikachu",
    "",
    1000,
    {0xA1, 0xC5}, // according to this thread, same IVs as distribution mew: 
                  // https://projectpokemon.org/home/forums/topic/37431-gen-i-v-event-contributions-thread/page/5/
    true,
    true,
    surfingPikachu
};

const Gen1DistributionPokemon g1_pokeTourMew = {
    "Poké Tour Mew",
    "AUS",
    0,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

const Gen1DistributionPokemon g1_pokemonPowerMew = {
    "Pokémon Power Mew",
    "MARIO",
    151,
    {0xA1, 0xC5},
    false,
    true,
    commonMewDefinition
};

static const Gen1DistributionPokemon* mainList[] = {
    &g1_nintendoUKPokemonFestivalMew2016,
    &g1_clubNintendoMexicoGiveawayMew,
    &g1_farMorBornMew,
    &g1_christmasPresentMew,
    &g1_swedenMewOnTour,
    &g1_tennispalatsiMew,
    &g1_norwayPokemonTourMew,
    &g1_clubNintendoMew,
    &g1_pokemon2000ChampionshipMew,
    &g1_millenniumDomeMew,
    &g1_austriaMew,
    &g1_ukIrelandPokemonChampionship2000Mew,
    &g1_coraChatelineauMew,
    &g1_francePokemonTournamentMew,
    &g1_spainPokemonTournamentMew,
    &g1_mewsFlashMew,
    &g1_pokemonPatrolMew,
    &g1_nintendoOfficialMagazineTourMew,
    &g1_canadianPokemonStadiumTour2000Mew,
    &g1_pokemonStadiumTour2000Mew,
    &g1_canadaToysRUsMew,
    &g1_usToysRUsMew,
    &g1_nintendoPowerMew,
    &g1_pokemonLeagueNintendoTrainingTour99Mew,
    &g1_nintendoPowerPikachu,
    &g1_pokeTourMew,
    &g1_pokemonPowerMew
};

void gen1_getMainDistributionPokemonList(const Gen1DistributionPokemon**& outList, uint32_t& outSize)
{
    outList = mainList;
    outSize = sizeof(mainList) / sizeof(mainList[0]);
}