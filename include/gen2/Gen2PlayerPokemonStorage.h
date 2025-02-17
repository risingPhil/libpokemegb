#ifndef _GEN2PLAYERPOKEMONSTORAGE_H
#define _GEN2PLAYERPOKEMONSTORAGE_H

#include "gen2/Gen2Common.h"
#include "gen2/Gen2Localization.h"

class ISaveManager;
class Gen2GameReader;

// the numbers are based on the PC_BOX_LIST_CAPACITY and PC_BOX_LIST_PKMN_ENTRY_SIZE in combination with the formula you can find here:
// https://bulbapedia.bulbagarden.net/wiki/Save_data_structure_(Generation_II)#Pok%C3%A9mon_lists
const int GEN2_PC_BOX_SIZE_IN_BYTES = 20 * (32 + 23) + 2;

/**
 * @brief The storage of the trainer party and pc boxes is very similar in Gen 2, 
 * aside from the listCapacity and the fields to read for a pokemon entry.
 * 
 * Therefore this class abstracts them both
 * 
 */
class Gen2PokemonList
{
public:
    Gen2PokemonList(Gen2GameReader& gameReader, ISaveManager& saveManager, uint8_t listCapacity, uint8_t entrySize, Gen2LocalizationLanguage language);
    virtual ~Gen2PokemonList();

    virtual uint32_t getSaveOffset() = 0;

    /**
     * @brief Seeks to the beginning of the PokemonList with saveManager_ and returns the number of entries in the list
     */
    uint8_t getNumberOfPokemon();
    uint8_t getMaxNumberOfPokemon() const;
    bool setNumberOfPokemon(uint8_t count);

    /**
     * @brief Seeks to the given index and returns the species index at that position
     * If the last call on the saveManager instance was done with PokemonList::getCount(), you can avoid the seek by using getNextSpecies() instead.
     */
    uint8_t getSpeciesAtIndex(uint8_t index);

    /**
     * @brief Modifies the species index at the given list index
     */
    bool setSpeciesAtIndex(uint8_t index, uint8_t speciesIndex);
    
    /**
     * @brief Get the next species index in the list. This should only be used if the last call on saveManager was done with 
     * PokemonList::getCount(), PokemonList::getSpeciesAtIndex() or getNextSpecies()
     */
    uint8_t getNextSpecies();

    /**
     * @brief Seeks to the data entry of the pokemon at the given index and reads its contents into Gen2TrainerPokemon
     * if accessing subsequent entries, you can avoid the seek by using getNextPokemon() if the last call on the saveManager_ 
     * instance was PokemonList::getPokemonAtIndex(), PokemonList::getNextPokemon(),or the terminator value of the species list obtained through
     * getNextSpecies()
     */
    bool getPokemon(uint8_t index, Gen2TrainerPokemon& outPoke);

    /**
     * @brief Checks whether the pokemon at the given index is an egg
     */
    bool isEgg(uint8_t index);
    
    /**
     * @brief Gets the next pokemon entry in the list
     */
    bool getNextPokemon(Gen2TrainerPokemon& outPoke);
    
    bool setPokemon(uint8_t index, const Gen2TrainerPokemon& poke, bool isEgg);
    bool add(const Gen2TrainerPokemon& poke, bool isEgg, const char* originalTrainerID, const char* nickname = nullptr);

    const char* getPokemonNickname(uint8_t index);
    bool setPokemonNickname(uint8_t, const char* nickname = nullptr);

    const char* getOriginalTrainerOfPokemon(uint8_t index);
    bool setOriginalTrainerOfPokemon(uint8_t index, const char* originalTrainerID);
    
protected:
    Gen2GameReader& gameReader_;
    ISaveManager& saveManager_;
    Gen2LocalizationLanguage localization_;
private:
    const uint8_t listCapacity_;
    const uint8_t entrySize_;
};

class Gen2Party : public Gen2PokemonList
{
public:
    Gen2Party(Gen2GameReader& gameReader, ISaveManager& saveManager, Gen2LocalizationLanguage language);
    virtual ~Gen2Party();

    uint32_t getSaveOffset() override;
protected:
private:
};

class Gen2Box : public Gen2PokemonList
{
public:
    Gen2Box(Gen2GameReader& gameReader, ISaveManager& saveManager, uint8_t boxIndex, Gen2LocalizationLanguage language);
    virtual ~Gen2Box();

    uint32_t getSaveOffset() override;
protected:
private:
    const uint8_t boxIndex_;
};

#endif