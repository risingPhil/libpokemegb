#include "gen2/Gen2GameReader.h"
#include "SpriteRenderer.h"
#include "RomReader.h"
#include "SaveManager.h"
#include "utils.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <time.h>

using OutputFormat = SpriteRenderer::OutputFormat;

// This example application just does some random stuff with Gen 2 game + save.
// This was merely used during development of this library and is therefore unfocused/chaotic in nature.

//#define DECODE_ALL_FRONT_SPRITES 1

static void printTrainerPokemon(const char* prefix, const Gen2TrainerPokemon& poke, Gen2GameReader& gameReader)
{
    Gen2PokeStats stats;
    gameReader.readPokemonStatsForIndex(poke.poke_index, stats);

    printf("%s\tNumber: %hhu\n", prefix, poke.poke_index);
    printf("%s\tType 1: 0x%hhx\n", prefix, stats.type1);
    printf("%s\tType 2: 0x%hhx\n", prefix, stats.type2);
    printf("%s\tHeld item: 0x%hhx\n", prefix, poke.held_item_index);
    printf("%s\tMove 1: 0x%hhx\n", prefix, poke.index_move1);
    printf("%s\tMove 2: 0x%hhx\n", prefix, poke.index_move2);
    printf("%s\tMove 3: 0x%hhx\n", prefix, poke.index_move3);
    printf("%s\tMove 4: 0x%hhx\n", prefix, poke.index_move4);
    printf("%s\tOriginal Trainer ID: 0x%hx\n", prefix, poke.original_trainer_ID);
    printf("%s\tExp: %u\n", prefix, poke.exp);
    printf("%s\tHP EV: %hu\n", prefix, poke.hp_effort_value);
    printf("%s\tAtk EV: %hu\n", prefix, poke.atk_effort_value);
    printf("%s\tDef EV: %hu\n", prefix, poke.def_effort_value);
    printf("%s\tSpeed EV: %hu\n", prefix, poke.speed_effort_value);
    printf("%s\tSpecial EV: %hu\n", prefix, poke.special_effort_value);
    printf("%s\tIV: 0x%hhx 0x%hhx\n", prefix, poke.iv_data[0], poke.iv_data[1]);
    printf("%s\tPP Move 1: %hhu\n", prefix, poke.pp_move1);
    printf("%s\tPP Move 2: %hhu\n", prefix, poke.pp_move2);
    printf("%s\tPP Move 3: %hhu\n", prefix, poke.pp_move3);
    printf("%s\tPP Move 4: %hhu\n", prefix, poke.pp_move4);
    printf("%s\tFriendship/Egg cycles: %hhu\n", prefix, poke.friendship_or_remaining_egg_cycles);
    printf("%s\tPokerus: 0x%hhx\n", prefix, poke.pokerus);
    printf("%s\tCaught data: 0x%hx\n", prefix, poke.caught_data);
    printf("%s\tLevel: %hhu\n", prefix, poke.level);
    printf("%s\tStatus Condition: 0x%hhx\n", prefix, poke.status_condition);
    printf("%s\tCurrent HP: %hu\n", prefix, poke.current_hp);
    printf("%s\tMax HP: %hu\n", prefix, poke.max_hp);
    printf("%s\tAtk: %hu\n", prefix, poke.atk);
    printf("%s\tDef: %hu\n", prefix, poke.def);
    printf("%s\tSpeed: %hu\n", prefix, poke.speed);
    printf("%s\tSpecial Atk: %hu\n", prefix, poke.special_atk);
    printf("%s\tSpecial Def: %hu\n", prefix, poke.special_def);
}

static void printParty(const char* prefix, Gen2GameReader& reader)
{
    Gen2Party party = reader.getParty();
    Gen2TrainerPokemon poke;
    const uint8_t numPokemon = party.getNumberOfPokemon();

    printf("%sNumber: %hhu\n", prefix, numPokemon);

    for(uint8_t i=0; i < numPokemon; ++i)
    {
        party.getPokemon(i, poke);

        printf("%s%s (%s)\n", prefix, party.getPokemonNickname(i), reader.getPokemonName(poke.poke_index));
        printTrainerPokemon(prefix, poke, reader);
    }
}

static void printBox(const char* prefix, Gen2GameReader& reader, uint8_t boxIndex)
{
    Gen2Box box = reader.getBox(boxIndex);
    Gen2TrainerPokemon poke;
    const uint8_t numPokemon = box.getNumberOfPokemon();

    printf("%sBox %hhu:\n", prefix, boxIndex + 1);
    printf("%sNumber: %hhu\n", prefix, numPokemon);

    for(uint8_t i=0; i < numPokemon; ++i)
    {
        box.getPokemon(i, poke);

        printf("%s%s (%s)\n", prefix, box.getPokemonNickname(i), reader.getPokemonName(poke.poke_index));
        printTrainerPokemon(prefix, poke, reader);
    }
}

#ifdef DECODE_ALL_FRONT_SPRITES

static void printStats(const char* prefix, const Gen2PokeStats& stats)
{
    printf("%spokedex number:%hhu\n", prefix, stats.pokedex_number);
    printf("%sbase hp: %hhu\n", prefix, stats.base_hp);
    printf("%sbase atk: %hhu\n", prefix, stats.base_attack);
    printf("%sbase def: %hhu\n", prefix, stats.base_defense);
    printf("%sbase speed: %hhu\n", prefix, stats.base_speed);
    printf("%sbase Sp. atk: %hhu\n", prefix, stats.base_special_attack);
    printf("%sbase Sp. def: %hhu\n", prefix, stats.base_special_defense);
    printf("%sType 1: 0x%hhx\n", prefix, stats.type1);
    printf("%sType 2: 0x%hhx\n", prefix, stats.type2);
    printf("%sCatch rate: 0x%hhx\n", prefix, stats.catch_rate);
    printf("%sbase exp yield: 0x%hhx\n", prefix, stats.base_exp_yield);
    printf("%swild held item 1: 0x%hhx\n", prefix, stats.wild_held_item1);
    printf("%swild held item 2: 0x%hhx\n", prefix, stats.wild_held_item2);
    printf("%sgender ratio: 0x%hhx\n", prefix, stats.gender_ratio);
    printf("%sunknown 1: 0x%hhx\n", prefix, stats.unknown);
    printf("%segg cycles: %hhu\n", prefix, stats.egg_cycles);
    printf("%sunknown 2: 0x%hhx\n", prefix, stats.unknown2);
    printf("%sfront sprite dimensions: 0x%hhx\n", prefix, stats.front_sprite_dimensions);
    printf("%sblank: 0x%hhx 0x%hhx 0x%hhx 0x%hhx\n", prefix, stats.blank[0], stats.blank[1], stats.blank[2], stats.blank[3]);
    printf("%sgrowth rate: 0x%hhx\n", prefix, stats.growth_rate);
    printf("%segg groups: 0x%hhx\n", prefix, stats.egg_groups);
    printf("%sTM/HM flags: 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx\n", prefix, 
        stats.tm_hm_flags[0], stats.tm_hm_flags[1], stats.tm_hm_flags[2], stats.tm_hm_flags[3], stats.tm_hm_flags[4], 
        stats.tm_hm_flags[5], stats.tm_hm_flags[6], stats.tm_hm_flags[7]);
}

static void decodePokemonSpriteByIndex(Gen2GameReader& gameReader, uint8_t i)
{
    SpriteRenderer renderer;
    char fileNameBuf[50];
    Gen2PokeStats stats;
    uint8_t* rgbBuffer;
    const char* pokeName;
    const uint8_t* spriteBuffer;
    uint8_t bankIndex;
    uint16_t pointer;
    uint8_t spriteWidthInTiles;
    uint8_t spriteHeightInTiles;
    uint16_t colorPalette[4];

    pokeName = gameReader.getPokemonName(i);
    gameReader.readPokemonStatsForIndex(i, stats);
    gameReader.readSpriteDimensions(stats, spriteWidthInTiles, spriteHeightInTiles);

    printf("Index %hhu:\t%s -> %hhux%hhu\n", i, pokeName, spriteWidthInTiles, spriteHeightInTiles);
    printStats("\t", stats);

    gameReader.readFrontSpritePointer(i, bankIndex, pointer);

    spriteBuffer = gameReader.decodeSprite(bankIndex, pointer);
    if(!spriteBuffer)
    {
        printf("Error: Could not decode sprite for %s\n", pokeName);
        return;
    }
    
    gameReader.readColorPaletteForPokemon(i, false, colorPalette);
    rgbBuffer = renderer.draw(spriteBuffer, OutputFormat::RGB, colorPalette, spriteWidthInTiles, spriteHeightInTiles);

    snprintf(fileNameBuf, sizeof(fileNameBuf), "%s.png", pokeName);
    write_png(fileNameBuf, rgbBuffer, spriteWidthInTiles * 8, spriteHeightInTiles * 8);
}
#endif

static void print_usage()
{
    printf("Usage: do_stuff_gen2 <path/to/rom.gbc> <path/to/file.sav>\n");
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    if(argc != 3)
    {
        print_usage();
        return 1;
    }
    uint8_t* romBuffer;
    uint8_t* savBuffer;
    uint32_t romFileSize;
    uint32_t savFileSize;

    romBuffer = readFileIntoBuffer(argv[1], romFileSize);
    if(!romBuffer)
    {
        fprintf(stderr, "ERROR: Couldn't read file %s\n", argv[1]);
        return 1;
    }
    savBuffer = readFileIntoBuffer(argv[2], savFileSize);
    if(!savBuffer)
    {
        fprintf(stderr, "ERROR: Couldn't read file %s\n", argv[2]);
        free(romBuffer);
        romBuffer = nullptr;
        return 1;
    }

    GameboyCartridgeHeader cartridgeHeader;
    BufferBasedRomReader romReader(romBuffer, romFileSize);
    BufferBasedSaveManager saveManager(savBuffer, savFileSize);

    readGameboyCartridgeHeader(romReader, cartridgeHeader);
    printGameboyCartridgeHeader(cartridgeHeader);
    const Gen2GameType gameType = gen2_determineGameType(cartridgeHeader);

    if(gameType == Gen2GameType::INVALID)
    {
        fprintf(stderr, "ERROR: Not a valid Gen 2 Pokémon game!\n");
        return 1;
    }

    Gen2GameReader gameReader(romReader, saveManager, gameType);


#if DECODE_ALL_FRONT_SPRITES
    for(uint8_t i=1; i < 252; ++i)
    {
        decodePokemonSpriteByIndex(gameReader, i);
    }
#endif

    if(gameReader.isMainChecksumValid())
    {
        printf("Main checksum valid\n");
    }

    if(gameReader.isBackupChecksumValid())
    {
        printf("Backup checksum valid\n");
    }

    printf("Injecting Mew and Celebi\n");
    gameReader.addDistributionPokemon(g2_clubNintendoMexico_Mew);
    gameReader.addDistributionPokemon(g2_nintendoPowerCelebi);

    gameReader.addDistributionPokemon(g2_pcny_mistRemoraid);
    gameReader.addDistributionPokemon(g2_pcny_biteLapras);
    gameReader.addDistributionPokemon(g2_pcny_shinyZapdos);
    gameReader.addDistributionPokemon(g2_pcny_shinyMoltres);
    gameReader.addDistributionPokemon(g2_pcny_shinyArticuno);
    gameReader.addDistributionPokemon(g2_pcny_shinyEntei);
    gameReader.finishSave();

    if(gameReader.isMainChecksumValid())
    {
        printf("Main checksum valid\n");
    }

    if(gameReader.isBackupChecksumValid())
    {
        printf("Backup checksum valid\n");
    }

    printf("Trainer %s (0x%hx)\n", gameReader.getTrainerName(), gameReader.getTrainerID());
    printf("Rival %s\n", gameReader.getRivalName());
    printf("PKMN Seen: %hhu\n", gameReader.getPokedexCounter(POKEDEX_SEEN));
    printf("PKMN Owned: %hhu\n", gameReader.getPokedexCounter(POKEDEX_OWNED));
    printf("Party:\n");
    printParty("\t", gameReader);
    printBox("\t", gameReader, 0);
    printBox("\t", gameReader, 1);

    FILE* f = fopen("out.sav", "w");
    fwrite(savBuffer, 1, savFileSize, f);
    fclose(f);

    free(romBuffer);
    romBuffer = 0;
    free(savBuffer);
    savBuffer = 0;

    return 0;
}