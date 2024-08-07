#include "RomReader.h"
#include "SaveManager.h"
#include "gen1/Gen1GameReader.h"
#include "SpriteRenderer.h"
#include "utils.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <time.h>

using OutputFormat = SpriteRenderer::OutputFormat;

// This example application just does some random stuff with Gen 1 game + save.
// This was merely used during development of this library and is therefore unfocused/chaotic in nature.

#define PRINT_POKESTATS 1
#define DECODE_SPRITES 1

#ifdef PRINT_POKESTATS
static void printPokestats(const char* prefix, Gen1PokeStats& stats)
{
    printf("%snumber: %hhu\n", prefix, stats.pokedex_number);
    printf("%sBase HP: %hhu\n", prefix, stats.base_hp);
    printf("%sBase Atk: %hhu\n", prefix, stats.base_attack);
    printf("%sBase Def: %hhu\n", prefix, stats.base_defense);
    printf("%sBase Spd: %hhu\n", prefix, stats.base_speed);
    printf("%sBase Spec: %hhu\n", prefix, stats.base_special);
    printf("%sType1: 0x%hhx\n", prefix, stats.type1);
    printf("%sType2: 0x%hhx\n", prefix, stats.type2);
    printf("%sCatch rate: %hhu\n", prefix, stats.catch_rate);
    printf("%sBase Exp Yield: %hhu\n", prefix, stats.base_exp_yield);
    printf("%sFront Sprite Dimensions: 0x%hhx\n", prefix, stats.front_sprite_dimensions);
    printf("%sSprite bank: %hhx\n", prefix, stats.sprite_bank);
    printf("%sFront Sprite Ptr: 0x%hx\n", prefix, stats.pointer_to_frontsprite);
    printf("%sBack Sprite Ptr: %hx\n", prefix, stats.pointer_to_backsprite);
    printf("%sBase Attacks: 0x%hhx 0x%hhx 0x%hhx 0x%hhx\n", prefix, stats.lvl1_attacks[0], stats.lvl1_attacks[1], stats.lvl1_attacks[2], stats.lvl1_attacks[3]);
    printf("%sGrowth Rate: %hhu\n", prefix, stats.growth_rate);
    printf("%sTM/HM Flags: 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx 0x%hhx\n", prefix, stats.tm_hm_flags[0], stats.tm_hm_flags[1], stats.tm_hm_flags[2], stats.tm_hm_flags[3], stats.tm_hm_flags[4], stats.tm_hm_flags[5], stats.tm_hm_flags[6]);

}
#endif

static void printTrainerPokemon(const char* prefix, const Gen1TrainerPokemon& poke, Gen1GameReader& reader)
{
    printf("%s\tNumber: %hhu\n", prefix, reader.getPokemonNumber(poke.poke_index));
    printf("%s\tIndex: %hhu\n", prefix, poke.poke_index);
    printf("%s\tCurrent HP: %hu\n", prefix, poke.current_hp);
    printf("%s\tLevel: %hhu\n", prefix, poke.level);
    printf("%s\tStatus Condition: 0x%hhx\n", prefix, poke.status_condition);
    printf("%s\tType 1: 0x%hhx\n", prefix, poke.type1);
    printf("%s\tType 2: 0x%hhx\n", prefix, poke.type2);
    printf("%s\tCatch Rate/Held Item: 0x%hhx\n", prefix, poke.catch_rate_or_held_item);
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
    printf("%s\tMax HP: %hu\n", prefix, poke.max_hp);
    printf("%s\tAtk: %hu\n", prefix, poke.atk);
    printf("%s\tDef: %hu\n", prefix, poke.def);
    printf("%s\tSpeed: %hu\n", prefix, poke.speed);
    printf("%s\tSpecial: %hu\n", prefix, poke.special);

    // test our implementation of calculatePokeStat()
    Gen1PokeStats stats;
    reader.readPokemonStatsForIndex(poke.poke_index, stats);
    printf("%s\tMax HP calculated: %hu\n", prefix, calculatePokeStat(PokeStat::HP, stats.base_hp, getStatIV(PokeStat::HP, poke.iv_data), poke.hp_effort_value, poke.level));
    printf("%s\tAtk calculated: %hu\n", prefix, calculatePokeStat(PokeStat::ATK, stats.base_attack, getStatIV(PokeStat::ATK, poke.iv_data), poke.atk_effort_value, poke.level));
    printf("%s\tDef calculated: %hu\n", prefix, calculatePokeStat(PokeStat::DEF, stats.base_defense, getStatIV(PokeStat::DEF, poke.iv_data), poke.def_effort_value, poke.level));
    printf("%s\tSpeed calculated: %hu\n", prefix, calculatePokeStat(PokeStat::SPEED, stats.base_speed, getStatIV(PokeStat::SPEED, poke.iv_data), poke.speed_effort_value, poke.level));
    printf("%s\tSpecial calculated: %hu\n", prefix, calculatePokeStat(PokeStat::SPECIAL, stats.base_special, getStatIV(PokeStat::SPECIAL, poke.iv_data), poke.special_effort_value, poke.level));
}

static void printParty(const char* prefix, Gen1GameReader& reader)
{
    Gen1Party party = reader.getParty();
    Gen1TrainerPokemon poke;
    const uint8_t numPokemon = party.getNumberOfPokemon();

    printf("%sNumber: %hhu\n", prefix, numPokemon);

    for(uint8_t i=0; i < numPokemon; ++i)
    {
        party.getPokemon(i, poke);

        printf("%s%s (%s)\n", prefix, party.getPokemonNickname(i), reader.getPokemonName(poke.poke_index));
        printTrainerPokemon(prefix, poke, reader);
    }
}

static void printBox(const char* prefix, Gen1GameReader& reader, uint8_t boxIndex)
{
    Gen1Box box = reader.getBox(boxIndex);
    Gen1TrainerPokemon poke;
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

static void print_usage()
{
    printf("Usage: do_stuff_gen1 <path/to/rom.gbc> <path/to/file.sav>\n");
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

    const Gen1GameType gameType = gen1_determineGameType(cartridgeHeader);
    if(gameType == Gen1GameType::INVALID)
    {
        fprintf(stderr, "ERROR: Not a valid Gen 1 Pokémon game!\n");
        return 1;
    }

    Gen1GameReader gen1Reader(romReader, saveManager, gameType);
    Gen1PokeStats stats;
#ifdef PRINT_POKESTATS
    printf("Pokémon names:\n");
    // Note: indices are 1-based: https://bulbapedia.bulbagarden.net/wiki/List_of_Pok%C3%A9mon_by_index_number_(Generation_I)
    for(uint8_t i = 1; i < 191; ++i)
    {
        gen1Reader.readPokemonStatsForIndex(i, stats);
        printf("\t%hhu: %s\n", i, gen1Reader.getPokemonName(i));
        printPokestats("\t\t", stats);
    }
    printf("end\n");
#endif

    printf("Save game:\n");
    printf("\tTrainer: %s (0x%hx)\n", gen1Reader.getTrainerName(), gen1Reader.getTrainerID());
    printf("\tRival: %s\n", gen1Reader.getRivalName());
    printf("\tSeen: %hhu\n", gen1Reader.getPokedexCounter(POKEDEX_SEEN));
    printf("\tOwned: %hhu\n", gen1Reader.getPokedexCounter(POKEDEX_OWNED));
    printf("\tParty:\n");
    printParty("\t\t", gen1Reader);
    printBox("\t\t", gen1Reader, 0);
    printBox("\t\t", gen1Reader, 1);

#if 0
    printf("Main checksum valid: %d\n", gen1Reader.isMainChecksumValid());
    printf("Bank 2 valid: %d\n", gen1Reader.isWholeBoxBankValid(2));
    printf("Bank 3 valid: %d\n", gen1Reader.isWholeBoxBankValid(3));

    uint8_t currentBoxIndex = gen1Reader.getCurrentBoxIndex();
    for(uint8_t i = 0; i < 13; ++i)
    {
        Gen1Box box = gen1Reader.getBox(i);
        printf("Box %hhu valid: %d, currentBoxIndex=%hhu\n", (i + 1), box.isChecksumValid(currentBoxIndex), currentBoxIndex);
    }
#endif

#if DECODE_SPRITES
    SpriteRenderer renderer;
    uint8_t* spriteBuffer;
    uint8_t* rgbBuffer;
    char fileNameBuf[50];
    uint16_t colorPalette[4];
    const char* pokeName;
    for(uint8_t i = 1; i < 191; ++i)
    {
        if(!gen1Reader.isValidIndex(i))
        {
            continue;
        }
        pokeName = gen1Reader.getPokemonName(i);

        printf("Decoding %s\n", pokeName);

        gen1Reader.readPokemonStatsForIndex(i, stats);
        gen1Reader.readColorPalette(gen1Reader.getColorPaletteIndexByPokemonNumber(stats.pokedex_number), colorPalette);
        spriteBuffer = gen1Reader.decodeSprite(stats.sprite_bank, stats.pointer_to_frontsprite);
        rgbBuffer = renderer.draw(spriteBuffer, OutputFormat::RGB, colorPalette, 7, 7);
        
        snprintf(fileNameBuf, sizeof(fileNameBuf), "%s.png", pokeName);
        write_png(fileNameBuf, rgbBuffer, 56, 56);
    }
#endif


#if 0
    Gen1TrainerPokemon bulby;
    Gen1Party party = gen1Reader.getParty();
    party.getPokemon(0, bulby);
    
    gen1Reader.addPokemon(bulby, nullptr, "bulma");
    gen1Reader.addPokemon(bulby, nullptr, "bumma");
    gen1Reader.addPokemon(bulby, nullptr, "bulva");
    gen1Reader.addPokemon(bulby, nullptr, "bulla");
    gen1Reader.addPokemon(bulby, nullptr, "bulbaba");
    gen1Reader.addPokemon(bulby, nullptr, "bulpapa");
    gen1Reader.addPokemon(bulby, nullptr, "bulmama");
    gen1Reader.addPokemon(bulby);
#endif
    gen1Reader.addDistributionPokemon(g1_coraChatelineauMew);
    gen1Reader.addDistributionPokemon(g1_nintendoPowerPikachu);

    FILE* f = fopen("out.sav", "w");
    fwrite(savBuffer, 1, savFileSize, f);
    fclose(f);

    free(romBuffer);
    romBuffer = 0;
    free(savBuffer);
    savBuffer = 0;
    
    return 0;
}
