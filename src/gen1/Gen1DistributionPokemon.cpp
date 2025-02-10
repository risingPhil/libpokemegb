#include "gen1/Gen1DistributionPokemon.h"
#include "common.h"

static const Gen1TrainerPokemon commonMewDefinition = {
    .poke_index = 0x15,
    .current_hp = 1, // I don't want to bother with figuring out the max hp here, so I set current hp to 1
    .level = 5,
    .status_condition = 0,
    .type1 = 0x18,
    .type2 = 0x18,
    .catch_rate_or_held_item = 45, // based on Gen1PokeStats
    .index_move1 = 0x1, // pound
    .index_move2 = 0x0,
    .index_move3 = 0x0,
    .index_move4 = 0x0,
    .original_trainer_ID = 0,
    .exp = getExpNeededForLevel(5, GRW_MEDIUM_SLOW),
    .hp_effort_value = 0,
    .atk_effort_value = 0,
    .def_effort_value = 0,
    .speed_effort_value = 0,
    .special_effort_value = 0,
    .iv_data = {0xA1, 0xC5},
    .pp_move1 = 35,
    .pp_move2 = 0,
    .pp_move3 = 0,
    .pp_move4 = 0,
    .max_hp = 0,
    .atk = 0,
    .def = 0,
    .speed = 0,
    .special = 0
};

static const Gen1TrainerPokemon surfingPikachu = {
    .poke_index = 0x54,
    .current_hp = 1,
    .level = 5,
    .status_condition = 0,
    .type1 = 0x17,
    .type2 = 0x17,
    .catch_rate_or_held_item = 190,
    .index_move1 = 84,
    .index_move2 = 45,
    .index_move3 = 57,
    .index_move4 = 0,
    .original_trainer_ID = 0,
    .exp = getExpNeededForLevel(5, GRW_MEDIUM_FAST),
    .hp_effort_value = 0,
    .atk_effort_value = 0,
    .def_effort_value = 0,
    .speed_effort_value = 0,
    .special_effort_value = 0,
    .iv_data = {0, 0},
    .pp_move1 = 30,
    .pp_move2 = 40,
    .pp_move3 = 15,
    .pp_move4 = 0,
    .max_hp = 0,
    .atk = 0,
    .def = 0,
    .speed = 0,
    .special = 0
};

// https://bulbapedia.bulbagarden.net/wiki/List_of_European_language_event_Pok%C3%A9mon_distributions_(Generation_I)
static const Gen1DistributionPokemon g1_nintendoUKPokemonFestivalMew2016 = {
    "Nintendo UK Pokemon Festival Mew 2016",
    "GF",
    22796,
    {0xFF, 0xFF},
    false,
    false,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_clubNintendoMexicoGiveawayMew = {
    "Club Nintendo Mexico Giveaway Mew",
    "ASH",
    45515,
    {0xA1, 0xC5},
    false,
    false,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_farMorBornMew = {
    "Far, Mor & Born Mew",
    "",
    0,
    {0xA1, 0xC5},
    true,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_christmasPresentMew = {
    "Christmas Present Mew",
    "",
    0,
    {0xA1, 0xC5},
    true,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_swedenMewOnTour = {
    "Sweden Mew On Tour",
    "",
    0,
    {0xA1, 0xC5},
    true,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_tennispalatsiMew = {
    "Tennispalatsi Mew",
    "FINLAND",
    1400,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_norwayPokemonTourMew = {
    "Norway Pokémon Tour Mew",
    "EUROPE",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_clubNintendoMew = {
    "Club Nintendo Mew",
    "EUROPE",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_pokemon2000ChampionshipMew = {
    "Pokémon 2000 World Championship Mew",
    "",
    0,
    {0xA1, 0xC5},
    true,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_millenniumDomeMew = {
    "Millennium Dome Mew",
    "",
    0,
    {0xA1, 0xC5},
    true,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_austriaMew = {
    "Austria Mew",
    "AUSTRIA",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_ukIrelandPokemonChampionship2000Mew = {
    "UK and Ireland Pokémon Championship 2000 Mew",
    "UK",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_coraChatelineauMew = {
    "Cora Châtelineau Mew",
    "BENELUX",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_francePokemonTournamentMew = {
    "France Pokémon Tournament Mew",
    "",
    0,
    {0xA1, 0xC5},
    true,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_spainPokemonTournamentMew = {
    "Spain Pokémon Tournament Mew",
    "EUROPE",
    0,
    {0x64, 0x38},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_mewsFlashMew = {
    "Mews Flash Mew",
    "UK",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_pokemonPatrolMew = {
    "Pokémon Patrol Mew",
    "YOSHIBB",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_nintendoOfficialMagazineTourMew = {
    "Nintendo Official Magazine Tour Mew",
    "UK",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_canadianPokemonStadiumTour2000Mew = {
    "Canadian Pokémon Stadium Tour 2000 Mew",
    "LUIGIC",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_pokemonStadiumTour2000Mew = {
    "Pokémon 2000 Stadium Tour Mew",
    "LUIGE",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_canadaToysRUsMew = {
    "Canada Toys \"R\" Us Mew",
    "YOSHIRA",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_usToysRUsMew = {
    "U.S. Toys \"R\" Us Mew",
    "YOSHIBA",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_nintendoPowerMew = {
    "Nintendo Power Mew",
    "YOSHIRB",
    1000,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_pokemonLeagueNintendoTrainingTour99Mew = {
    "Pokémon League Nintendo Training Tour '99 Mew",
    "YOSHIRA",
    0,
    {0xA1, 0xC5},
    false,
    true,
    false,
    0,
    commonMewDefinition
};

static const Gen1DistributionPokemon g1_nintendoPowerPikachu = {
    "Nintendo Power Pikachu",
    "",
    1000,
    {0xA1, 0xC5}, // according to this thread, same IVs as distribution mew: 
                  // https://projectpokemon.org/home/forums/topic/37431-gen-i-v-event-contributions-thread/page/5/
    true,
    true,
    false,
    0,
    surfingPikachu
};

static const Gen1DistributionPokemon g1_pokeTourMew = {
    .name = "Poké Tour Mew",
    .originalTrainer = "AUS",
    .originalTrainerID = 0,
    .iv_data = {0xA1, 0xC5},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = false,
    .held_item_override = 0,
    .poke = commonMewDefinition
};

static const Gen1DistributionPokemon g1_pokemonPowerMew = {
    .name = "Pokémon Power Mew",
    .originalTrainer = "MARIO",
    .originalTrainerID = 151,
    .iv_data = {0xA1, 0xC5},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = false,
    .held_item_override = 0,
    .poke = commonMewDefinition
};

static const Gen1DistributionPokemon g1_jpn_gamefreakMew = {
    .name = "Game Freak Mew 2016 (JPN)",
    .originalTrainer = "ゲーフリ",
    .originalTrainerID = 22796,
    .iv_data = {0xFF, 0xFF},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = false,
    .isJapanese = true,
    .held_item_override = 0,
    .poke = commonMewDefinition
};

static const Gen1DistributionPokemon g1_jpn_spaceworld99Mew = {
    .name = "Nintendo Space World '99 Mew (JPN)",
    .originalTrainer = "マクハリ",
    .originalTrainerID = 0,
    .iv_data = {0xFF, 0xFF},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0x53,
    .poke = commonMewDefinition
};

static const Gen1DistributionPokemon g1_jpn_pokemonStampFearow = {
    .name = "Pokémon Stamp Fearow (JPN)",
    .originalTrainer = "",
    .originalTrainerID = 0,
    .iv_data = {0, 0},
    .setPlayerAsOriginalTrainer = true,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0,
    .poke = {
        .poke_index = 0x23,
        .current_hp = 1,
        .level = 25,
        .status_condition = 0,
        .type1 = 0, // NORMAL
        .type2 = 2, // FLYING
        .catch_rate_or_held_item = 0xAD,
        .index_move1 = 45, // GROWL
        .index_move2 = 43, // LEER
        .index_move3 = 31, // FURY ATTACK
        .index_move4 = 6, // PAY DAY
        .original_trainer_ID = 0,
        .exp = getExpNeededForLevel(25, GRW_MEDIUM_FAST),
        .hp_effort_value = 0,
        .atk_effort_value = 0,
        .def_effort_value = 0,
        .speed_effort_value = 0,
        .special_effort_value = 0,
        .iv_data = {0, 0},
        .pp_move1 = 40,
        .pp_move2 = 30,
        .pp_move3 = 20,
        .pp_move4 = 20,
        .max_hp = 0,
        .atk = 0,
        .def = 0,
        .speed = 0,
        .special = 0
    }
};

static const Gen1DistributionPokemon g1_jpn_pokemonStampRapidash = {
    .name = "Pokémon Stamp Rapidash (JPN)",
    .originalTrainer = "",
    .originalTrainerID = 0,
    .iv_data = {0, 0},
    .setPlayerAsOriginalTrainer = true,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0,
    .poke = {
        .poke_index = 0xA4,
        .current_hp = 1,
        .level = 40,
        .status_condition = 0,
        .type1 = 20, // FIRE
        .type2 = 20, // FIRE
        .catch_rate_or_held_item = 0x3C,
        .index_move1 = 52, // EMBER
        .index_move2 = 83, // FIRE SPIN
        .index_move3 = 23, // STOMP
        .index_move4 = 6, // PAY DAY
        .original_trainer_ID = 0,
        .exp = getExpNeededForLevel(40, GRW_MEDIUM_FAST),
        .hp_effort_value = 0,
        .atk_effort_value = 0,
        .def_effort_value = 0,
        .speed_effort_value = 0,
        .special_effort_value = 0,
        .iv_data = {0, 0},
        .pp_move1 = 25,
        .pp_move2 = 15,
        .pp_move3 = 20,
        .pp_move4 = 20,
        .max_hp = 0,
        .atk = 0,
        .def = 0,
        .speed = 0,
        .special = 0
    }
};

static const Gen1DistributionPokemon g1_jpn_summer98PokemonBattleTourPikachu = {
    .name = "Summer '98 Pokémon Battle Tour Pikachu (JPN)",
    .originalTrainer = "イマクニ",
    .originalTrainerID = 0,
    .iv_data = {0, 0},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0,
    .poke = {
        .poke_index = 0x54,
        .current_hp = 1,
        .level = 5,
        .status_condition = 0,
        .type1 = 23, // ELECTRIC
        .type2 = 23, // ELECTRIC
        .catch_rate_or_held_item = 0xAD,
        .index_move1 = 84, // THUNDERSHOCK
        .index_move2 = 45, // GROWL
        .index_move3 = 57, // SURF
        .index_move4 = 0,
        .original_trainer_ID = 0,
        .exp = getExpNeededForLevel(5, GRW_MEDIUM_FAST),
        .hp_effort_value = 0,
        .atk_effort_value = 0,
        .def_effort_value = 0,
        .speed_effort_value = 0,
        .special_effort_value = 0,
        .iv_data = {0, 0},
        .pp_move1 = 30,
        .pp_move2 = 40,
        .pp_move3 = 15,
        .pp_move4 = 0,
        .max_hp = 0,
        .atk = 0,
        .def = 0,
        .speed = 0,
        .special = 0
    }
};

static const Gen1DistributionPokemon g1_jpn_universityMagikarp = {
    .name = "University Magikarp (JPN)",
    .originalTrainer = "",
    .originalTrainerID = 0,
    .iv_data = {0, 0},
    .setPlayerAsOriginalTrainer = true,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0,
    .poke = {
        .poke_index = 0x85,
        .current_hp = 1,
        .level = 15,
        .status_condition = 0,
        .type1 = 21, // WATER
        .type2 = 21, // WATER
        .catch_rate_or_held_item = 0xAD,
        .index_move1 = 150, // SPLASH
        .index_move2 = 82, // DRAGON RAGE
        .index_move3 = 0,
        .index_move4 = 0,
        .original_trainer_ID = 0,
        .exp = getExpNeededForLevel(15, GRW_SLOW),
        .hp_effort_value = 0,
        .atk_effort_value = 0,
        .def_effort_value = 0,
        .speed_effort_value = 0,
        .special_effort_value = 0,
        .iv_data = {0, 0},
        .pp_move1 = 40,
        .pp_move2 = 10,
        .pp_move3 = 0,
        .pp_move4 = 0,
        .max_hp = 0,
        .atk = 0,
        .def = 0,
        .speed = 0,
        .special = 0
    }
};

static const Gen1DistributionPokemon g1_jpn_ng_worldHobbyFairMew = {
    .name = "NG World Hobby Fair Mew (JPN)",
    .originalTrainer = "トウキョー",
    .originalTrainerID = 0,
    .iv_data = {0xA1, 0xC5},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0x53,
    .poke = commonMewDefinition
};

static const Gen1DistributionPokemon g1_jpn_nintendoSpaceWorld97Mew = {
    .name = "Nintendo Space World '97 Mew (JPN)",
    .originalTrainer = "クッパ",
    .originalTrainerID = 0,
    .iv_data = {0xA1, 0xC5},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0x53,
    .poke = commonMewDefinition
};

static const Gen1DistributionPokemon g1_jpn_nintendo64SurfingPikachu = {
    .name = "Nintendo 64 Surfing Pikachu (JPN)",
    .originalTrainer = "ニンテン",
    .originalTrainerID = 0,
    .iv_data = {0, 0},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0xAD, // BERRY
    .poke = {
        .poke_index = 0x54,
        .current_hp = 1,
        .level = 5,
        .status_condition = 0,
        .type1 = 23, // ELECTRIC
        .type2 = 23, // ELECTRIC
        .catch_rate_or_held_item = 0xAD,
        .index_move1 = 84, // THUNDERSHOCK
        .index_move2 = 45, // GROWL
        .index_move3 = 57, // SURF
        .index_move4 = 0,
        .original_trainer_ID = 0,
        .exp = getExpNeededForLevel(5, GRW_MEDIUM_FAST),
        .hp_effort_value = 0,
        .atk_effort_value = 0,
        .def_effort_value = 0,
        .speed_effort_value = 0,
        .special_effort_value = 0,
        .iv_data = {0, 0},
        .pp_move1 = 30,
        .pp_move2 = 40,
        .pp_move3 = 15,
        .pp_move4 = 0,
        .max_hp = 0,
        .atk = 0,
        .def = 0,
        .speed = 0,
        .special = 0
    }
};

static const Gen1DistributionPokemon g1_jpn_flyingPikachu = {
    .name = "Flying Pikachu (JPN)",
    .originalTrainer = "コロコロ",
    .originalTrainerID = 0,
    .iv_data = {0, 0},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0xAD, // BERRY
    .poke = {
        .poke_index = 0x54,
        .current_hp = 1,
        .level = 5,
        .status_condition = 0,
        .type1 = 23, // ELECTRIC
        .type2 = 23, // ELECTRIC
        .catch_rate_or_held_item = 0xAD,
        .index_move1 = 84, // THUNDERSHOCK
        .index_move2 = 45, // GROWL
        .index_move3 = 19, // FLY
        .index_move4 = 0,
        .original_trainer_ID = 0,
        .exp = getExpNeededForLevel(5, GRW_MEDIUM_FAST),
        .hp_effort_value = 0,
        .atk_effort_value = 0,
        .def_effort_value = 0,
        .speed_effort_value = 0,
        .special_effort_value = 0,
        .iv_data = {0, 0},
        .pp_move1 = 30,
        .pp_move2 = 40,
        .pp_move3 = 15,
        .pp_move4 = 0,
        .max_hp = 0,
        .atk = 0,
        .def = 0,
        .speed = 0,
        .special = 0
    }
};

static const Gen1DistributionPokemon g1_jpn_surfingPikachu = {
    .name = "Surfing Pikachu (JPN)",
    .originalTrainer = "コロコロ",
    .originalTrainerID = 0,
    .iv_data = {0, 0},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0xAD, // BERRY
    .poke = {
        .poke_index = 0x54,
        .current_hp = 1,
        .level = 5,
        .status_condition = 0,
        .type1 = 23, // ELECTRIC
        .type2 = 23, // ELECTRIC
        .catch_rate_or_held_item = 0xAD,
        .index_move1 = 84, // THUNDERSHOCK
        .index_move2 = 45, // GROWL
        .index_move3 = 57, // SURF
        .index_move4 = 0,
        .original_trainer_ID = 0,
        .exp = getExpNeededForLevel(5, GRW_MEDIUM_FAST),
        .hp_effort_value = 0,
        .atk_effort_value = 0,
        .def_effort_value = 0,
        .speed_effort_value = 0,
        .special_effort_value = 0,
        .iv_data = {0, 0},
        .pp_move1 = 30,
        .pp_move2 = 40,
        .pp_move3 = 15,
        .pp_move4 = 0,
        .max_hp = 0,
        .atk = 0,
        .def = 0,
        .speed = 0,
        .special = 0
    }
};

static const Gen1DistributionPokemon g1_jpn_pkmn2IdeaContestSurfingPikachu = {
    .name = "PKMN 2 Idea Contest Surfing Pikachu (JPN)",
    .originalTrainer = "コロコロ",
    .originalTrainerID = 0,
    .iv_data = {0, 0},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0xAD, // BERRY
    .poke = {
        .poke_index = 0x54,
        .current_hp = 1,
        .level = 5,
        .status_condition = 0,
        .type1 = 23, // ELECTRIC
        .type2 = 23, // ELECTRIC
        .catch_rate_or_held_item = 0xAD,
        .index_move1 = 84, // THUNDERSHOCK
        .index_move2 = 45, // GROWL
        .index_move3 = 57, // SURF
        .index_move4 = 0,
        .original_trainer_ID = 0,
        .exp = getExpNeededForLevel(5, GRW_MEDIUM_FAST),
        .hp_effort_value = 0,
        .atk_effort_value = 0,
        .def_effort_value = 0,
        .speed_effort_value = 0,
        .special_effort_value = 0,
        .iv_data = {0, 0},
        .pp_move1 = 30,
        .pp_move2 = 40,
        .pp_move3 = 15,
        .pp_move4 = 0,
        .max_hp = 0,
        .atk = 0,
        .def = 0,
        .speed = 0,
        .special = 0
    }
};

static const Gen1DistributionPokemon g1_jpn_corocoro20thAnniversaryMew = {
    .name = "CoroCoro 20th Anniversary Mew (JPN)",
    .originalTrainer = "コロコロ",
    .originalTrainerID = 0,
    .iv_data = {0, 0},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0x53, // Bitter Berry
    .poke = commonMewDefinition
};

static const Gen1DistributionPokemon g1_jpn_4thNGWorldHobbyFairMew = {
    .name = "4th NG World Hobby Fair Mew (JPN)",
    .originalTrainer = "コロコロ",
    .originalTrainerID = 0,
    .iv_data = {0, 0},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0x53, // Bitter Berry
    .poke = commonMewDefinition
};

static const Gen1DistributionPokemon g1_jpn_pkmn2ProdStartMew = {
    .name = "PKMN2 Production Start Mew (JPN)",
    .originalTrainer = "コロコロ",
    .originalTrainerID = 0,
    .iv_data = {0, 0},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0x53, // Bitter Berry
    .poke = commonMewDefinition
};

static const Gen1DistributionPokemon g1_jpn_legendaryPokemonOfferMew = {
    .name = "Legendary Pokémon Offer Mew (JPN)",
    .originalTrainer = "コロコロ",
    .originalTrainerID = 21,
    .iv_data = {0, 0},
    .setPlayerAsOriginalTrainer = false,
    .regenerateTrainerID = true,
    .isJapanese = true,
    .held_item_override = 0x53, // Bitter Berry
    .poke = commonMewDefinition
};

// TODO: add more japanese distro pkmn

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
    &g1_pokemonPowerMew,
    &g1_jpn_gamefreakMew,
    &g1_jpn_spaceworld99Mew,
    &g1_jpn_pokemonStampFearow,
    &g1_jpn_pokemonStampRapidash,
    &g1_jpn_summer98PokemonBattleTourPikachu,
    &g1_jpn_universityMagikarp,
    &g1_jpn_ng_worldHobbyFairMew,
    &g1_jpn_nintendoSpaceWorld97Mew,
    &g1_jpn_nintendo64SurfingPikachu,
    &g1_jpn_flyingPikachu,
    &g1_jpn_surfingPikachu,
    &g1_jpn_pkmn2IdeaContestSurfingPikachu,
    &g1_jpn_corocoro20thAnniversaryMew,
    &g1_jpn_4thNGWorldHobbyFairMew,
    &g1_jpn_pkmn2ProdStartMew,
    &g1_jpn_legendaryPokemonOfferMew
};

void gen1_getMainDistributionPokemonList(const Gen1DistributionPokemon**& outList, uint32_t& outSize)
{
    outList = mainList;
    outSize = sizeof(mainList) / sizeof(mainList[0]);
}