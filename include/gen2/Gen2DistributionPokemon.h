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
    // definition of the pokemon
    Gen2TrainerPokemon poke;
} Gen2DistributionPokemon;

// Note: see gen2_getMainDistributionPokemonList() and gen2_getPokemonCenterNewYorkDistributionPokemonList() below

extern const Gen2DistributionPokemon g2_clubNintendoMexico_Mew;
extern const Gen2DistributionPokemon g2_clubNintendoMexico_Celebi;

extern const Gen2DistributionPokemon g2_swedenCelebi;
extern const Gen2DistributionPokemon g2_westfieldShopping;
extern const Gen2DistributionPokemon g2_celebiTour;
extern const Gen2DistributionPokemon g2_pokemonFunFestCelebi;
extern const Gen2DistributionPokemon g2_nintendoPowerCelebi;
extern const Gen2DistributionPokemon g2_celebiSweepstakes;

// PCNY (Pokemon Center New York) Gotta Catch 'em all! station distribution pokémon below
extern const Gen2DistributionPokemon g2_pcny_ancientpowerBulbasaur;
extern const Gen2DistributionPokemon g2_pcny_shinyVenusaur;
extern const Gen2DistributionPokemon g2_pcny_crunchCharmander;
extern const Gen2DistributionPokemon g2_pcny_shinyCharizard;
extern const Gen2DistributionPokemon g2_pcny_zapCannonSquirtle;
extern const Gen2DistributionPokemon g2_pcny_shinyBlastoise;
extern const Gen2DistributionPokemon g2_pcny_sonicboomSpearow;
extern const Gen2DistributionPokemon g2_pcny_lovelyKissNidoranFem;
extern const Gen2DistributionPokemon g2_pcny_moonlightNidoranFem;
extern const Gen2DistributionPokemon g2_pcny_sweetKissNidoranFem;
extern const Gen2DistributionPokemon g2_pcny_lovelyKissNidoranMale;
extern const Gen2DistributionPokemon g2_pcny_morningSunNidoranMale;
extern const Gen2DistributionPokemon g2_pcny_sweetKissNidoranMale;
extern const Gen2DistributionPokemon g2_pcny_flailZubat;
extern const Gen2DistributionPokemon g2_pcny_leechSeedOddish;
extern const Gen2DistributionPokemon g2_pcny_synthesisParas;
extern const Gen2DistributionPokemon g2_pcny_petalDancePsyduck;
extern const Gen2DistributionPokemon g2_pcny_triAttackPsyduck;
extern const Gen2DistributionPokemon g2_pcny_growthPoliwag;
extern const Gen2DistributionPokemon g2_pcny_lovelyKissPoliwag;
extern const Gen2DistributionPokemon g2_pcny_sweetKissPoliwag;
extern const Gen2DistributionPokemon g2_pcny_foresightAbra;
extern const Gen2DistributionPokemon g2_pcny_falseSwipeMachop;
extern const Gen2DistributionPokemon g2_pcny_thrashMachop;
extern const Gen2DistributionPokemon g2_pcny_lovelyKissBellsprout;
extern const Gen2DistributionPokemon g2_pcny_sweetKissBellsprout;
extern const Gen2DistributionPokemon g2_pcny_confuseRayTentacool;
extern const Gen2DistributionPokemon g2_pcny_rapidSpinGeodude;
extern const Gen2DistributionPokemon g2_pcny_lowKickPonyta;
extern const Gen2DistributionPokemon g2_pcny_agilityMagnemite;
extern const Gen2DistributionPokemon g2_pcny_furyCutterFarfetchd;
extern const Gen2DistributionPokemon g2_pcny_lowKickDoduo;
extern const Gen2DistributionPokemon g2_pcny_flailSeel;
extern const Gen2DistributionPokemon g2_pcny_sharpenOnix;
extern const Gen2DistributionPokemon g2_pcny_amnesiaDrowsee;
extern const Gen2DistributionPokemon g2_pcny_metalClawKrabby;
extern const Gen2DistributionPokemon g2_pcny_agilityVoltorb;
extern const Gen2DistributionPokemon g2_pcny_sweetScentExeggcute;
extern const Gen2DistributionPokemon g2_pcny_furyAttackCubone;
extern const Gen2DistributionPokemon g2_pcny_doubleSlapLickitung;
extern const Gen2DistributionPokemon g2_pcny_sweetScentChansey;
extern const Gen2DistributionPokemon g2_pcny_synthesisTangela;
extern const Gen2DistributionPokemon g2_pcny_faintAttackKangaskhan;
extern const Gen2DistributionPokemon g2_pcny_hazeHorsea;
extern const Gen2DistributionPokemon g2_pcny_swordsDanceGoldeen;
extern const Gen2DistributionPokemon g2_pcny_twisterStaryu;
extern const Gen2DistributionPokemon g2_pcny_mindReaderMrMime;
extern const Gen2DistributionPokemon g2_pcny_sonicBoomScyther;
extern const Gen2DistributionPokemon g2_pcny_rockThrowPinsir;
extern const Gen2DistributionPokemon g2_pcny_quickAttackTauros;
extern const Gen2DistributionPokemon g2_pcny_bubbleMagikarp;
extern const Gen2DistributionPokemon g2_pcny_reversalMagikarp;
extern const Gen2DistributionPokemon g2_pcny_biteLapras;
extern const Gen2DistributionPokemon g2_pcny_futureSightLapras;
extern const Gen2DistributionPokemon g2_pcny_growthEevee;
extern const Gen2DistributionPokemon g2_pcny_barrierPorygon;
extern const Gen2DistributionPokemon g2_pcny_rockThrowOmanyte;
extern const Gen2DistributionPokemon g2_pcny_rockThrowKabuto;
extern const Gen2DistributionPokemon g2_pcny_rockThrowAerodactyl;
extern const Gen2DistributionPokemon g2_pcny_lovelyKissSnorlax;
extern const Gen2DistributionPokemon g2_pcny_splashSnorlax;
extern const Gen2DistributionPokemon g2_pcny_sweetKissSnorlax;
extern const Gen2DistributionPokemon g2_pcny_shinyArticuno;
extern const Gen2DistributionPokemon g2_pcny_shinyZapdos;
extern const Gen2DistributionPokemon g2_pcny_shinyMoltres;
extern const Gen2DistributionPokemon g2_pcny_hydroPumpDratini;
extern const Gen2DistributionPokemon g2_pcny_shinyMewtwo;
extern const Gen2DistributionPokemon g2_pcny_shinyMew;
extern const Gen2DistributionPokemon g2_pcny_petalDanceChikorita;
extern const Gen2DistributionPokemon g2_pcny_shinyMeganium;
extern const Gen2DistributionPokemon g2_pcny_doubleEdgeCyndaquil;
extern const Gen2DistributionPokemon g2_pcny_shinyTyphlosion;
extern const Gen2DistributionPokemon g2_pcny_submissionTotodile;
extern const Gen2DistributionPokemon g2_pcny_shinyFeraligatr;
extern const Gen2DistributionPokemon g2_pcny_dizzyPunchSentret;
extern const Gen2DistributionPokemon g2_pcny_nightShadeHoothoot;
extern const Gen2DistributionPokemon g2_pcny_barrierLedyba;
extern const Gen2DistributionPokemon g2_pcny_growthSpinarak;
extern const Gen2DistributionPokemon g2_pcny_lightScreenChinchou;
extern const Gen2DistributionPokemon g2_pcny_dizzyPunchPichu;
extern const Gen2DistributionPokemon g2_pcny_petalDancePichu;
extern const Gen2DistributionPokemon g2_pcny_scaryFacePichu;
extern const Gen2DistributionPokemon g2_pcny_singPichu;
extern const Gen2DistributionPokemon g2_pcny_petalDanceCleffa;
extern const Gen2DistributionPokemon g2_pcny_scaryFaceCleffa;
extern const Gen2DistributionPokemon g2_pcny_swiftCleffa;
extern const Gen2DistributionPokemon g2_pcny_mimicIgglybuff;
extern const Gen2DistributionPokemon g2_pcny_petalDanceIgglybuff;
extern const Gen2DistributionPokemon g2_pcny_scaryFaceIgglybuff;
extern const Gen2DistributionPokemon g2_pcny_safeguardNatu;
extern const Gen2DistributionPokemon g2_pcny_dizzyPunchMarill;
extern const Gen2DistributionPokemon g2_pcny_hydroPumpMarill;
extern const Gen2DistributionPokemon g2_pcny_scaryFaceMarill;
extern const Gen2DistributionPokemon g2_pcny_substituteSudowoodo;
extern const Gen2DistributionPokemon g2_pcny_agilityHoppip;
extern const Gen2DistributionPokemon g2_pcny_mimicAipom;
extern const Gen2DistributionPokemon g2_pcny_splashSunkern;
extern const Gen2DistributionPokemon g2_pcny_steelWingYanma;
extern const Gen2DistributionPokemon g2_pcny_sweetKissYanma;
extern const Gen2DistributionPokemon g2_pcny_bellyDrumWooper;
extern const Gen2DistributionPokemon g2_pcny_scaryFaceWooper;
extern const Gen2DistributionPokemon g2_pcny_beatUpMurkrow;
extern const Gen2DistributionPokemon g2_pcny_hypnosisMisdreavus;
extern const Gen2DistributionPokemon g2_pcny_mimicWobbuffet;
extern const Gen2DistributionPokemon g2_pcny_substitutePineco;
extern const Gen2DistributionPokemon g2_pcny_furyAttackDunsparce;
extern const Gen2DistributionPokemon g2_pcny_hornDrillDunsparce;
extern const Gen2DistributionPokemon g2_pcny_lovelyKissSnubbull;
extern const Gen2DistributionPokemon g2_pcny_doubleEdgeQwilfish;
extern const Gen2DistributionPokemon g2_pcny_seismicTossHeracross;
extern const Gen2DistributionPokemon g2_pcny_moonlightSneasel;
extern const Gen2DistributionPokemon g2_pcny_sweetScentTeddiursa;
extern const Gen2DistributionPokemon g2_pcny_whirlwindSwinub;
extern const Gen2DistributionPokemon g2_pcny_amnesiaRemoraid;
extern const Gen2DistributionPokemon g2_pcny_mistRemoraid;
extern const Gen2DistributionPokemon g2_pcny_payDayDelibird;
extern const Gen2DistributionPokemon g2_pcny_spikesDelibird;
extern const Gen2DistributionPokemon g2_pcny_gustMantine;
extern const Gen2DistributionPokemon g2_pcny_furyCutterSkarmory;
extern const Gen2DistributionPokemon g2_pcny_absorbPhanpy;
extern const Gen2DistributionPokemon g2_pcny_safeguardStantler;
extern const Gen2DistributionPokemon g2_pcny_rageTyrogue;
extern const Gen2DistributionPokemon g2_pcny_metronomeSmoochum;
extern const Gen2DistributionPokemon g2_pcny_petalDanceSmoochum;
extern const Gen2DistributionPokemon g2_pcny_dizzyPunchElekid;
extern const Gen2DistributionPokemon g2_pcny_pursuitElekid;
extern const Gen2DistributionPokemon g2_pcny_faintAttackMagby;
extern const Gen2DistributionPokemon g2_pcny_megaKickMiltank;
extern const Gen2DistributionPokemon g2_pcny_shinyRaikou;
extern const Gen2DistributionPokemon g2_pcny_shinyEntei;
extern const Gen2DistributionPokemon g2_pcny_shinySuicune;

// The regular (non-shiny) suicune on https://bulbapedia.bulbagarden.net/wiki/List_of_PCNY_event_Pok%C3%A9mon_distributions_(Generation_II)#Suicune
// has too many question marks and not even any moves. So I'm ignoring that one.
//extern const Gen2DistributionPokemon g2_pcny_suicune;

extern const Gen2DistributionPokemon g2_pcny_rageLarvitar;
extern const Gen2DistributionPokemon g2_pcny_shinyLugia;
extern const Gen2DistributionPokemon g2_pcny_shinyHoOh;
extern const Gen2DistributionPokemon g2_pcny_Celebi;

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