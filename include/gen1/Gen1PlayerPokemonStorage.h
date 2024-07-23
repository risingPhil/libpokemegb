#ifndef GEN1_PLAYERPOKEMONSTORAGE_H
#define GEN1_PLAYERPOKEMONSTORAGE_H

#include "gen1/Gen1Common.h"

class Gen1GameReader;
class ISaveManager;

uint8_t getGen1BoxBankIndex(uint8_t boxIndex, uint8_t currentBoxIndex);

/**
 * @brief This class represents the trainer party in the GEN1 games
 */
class Gen1Party
{
public:
    Gen1Party(Gen1GameReader& gameReader, ISaveManager& saveManager);
    ~Gen1Party();

    bool getPokemon(uint8_t index, Gen1TrainerPokemon& outTrainerPokemon);
    bool setPokemon(uint8_t index, Gen1TrainerPokemon& poke);
    uint8_t getNumberOfPokemon();
    uint8_t getMaxNumberOfPokemon();
    const char* getPokemonNickname(uint8_t index);
    void setPokemonNickname(uint8_t index, const char* nickname = 0);
    const char* getOriginalTrainerOfPokemon(uint8_t index);
    void setOriginalTrainerOfPokemon(uint8_t index, const char* originalTrainer);
    bool add(Gen1TrainerPokemon& poke, const char* originalTrainerID, const char* nickname = 0);
protected:
private:
    Gen1GameReader& gameReader_;
    ISaveManager& saveManager_;
};

/**
 * @brief This class represents a PC Box of the player in the GEN1 games
 */
class Gen1Box
{
public:
    Gen1Box(Gen1GameReader& gameReader, ISaveManager& saveManager, uint8_t boxIndex);
    ~Gen1Box();

    bool getPokemon(uint8_t index, Gen1TrainerPokemon& outTrainerPokemon);
    bool setPokemon(uint8_t index, Gen1TrainerPokemon& poke);
    uint8_t getNumberOfPokemon();
    uint8_t getMaxNumberOfPokemon();
    const char* getPokemonNickname(uint8_t index);
    void setPokemonNickname(uint8_t index, const char* nickname = 0);
    const char* getOriginalTrainerOfPokemon(uint8_t index);
    void setOriginalTrainerOfPokemon(uint8_t index, const char* originalTrainer);
    bool add(Gen1TrainerPokemon& poke, const char* originalTrainerID, const char* nickname = 0);

    bool isChecksumValid(uint8_t currentBoxIndex);
    void updateChecksum(uint8_t currentBoxIndex);
protected:
private:
    Gen1GameReader& gameReader_;
    ISaveManager& saveManager_;
    uint8_t boxIndex_;
};

#endif