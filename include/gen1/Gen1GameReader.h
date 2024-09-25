#ifndef GEN1GAMEREADER_H
#define GEN1GAMEREADER_H

#include "gen1/Gen1SpriteDecoder.h"
#include "gen1/Gen1IconDecoder.h"
#include "gen1/Gen1PlayerPokemonStorage.h"
#include "gen1/Gen1DistributionPokemon.h"

class IRomReader;
class ISaveManager;

class Gen1GameReader
{
public:
    Gen1GameReader(IRomReader& romReader, ISaveManager& saveManager, Gen1GameType gameType);

    /**
     * @brief get the name of a pokémon based on an index number
     * Note: you don't own the returned pointer. The data will get overwritten on the next call to this function,
     * so make sure to strdup() it if you need to store it for later
     * 
     * @return const char* 
     */
    const char* getPokemonName(uint8_t index) const;

    /**
     * @brief Gets the pokedex number of a pokemon at the given internal index
     */
    uint8_t getPokemonNumber(uint8_t index) const;

    /**
     * @brief This function returns the Gen1PokemonIconType for a pokemon at the given index
     */
    Gen1PokemonIconType getPokemonIconType(uint8_t index) const;

    /**
     * @brief With this function, you can check if the index is valid and not referring to missingno
     */
    bool isValidIndex(uint8_t index) const;

    /**
     * @brief This function reads the pokemon stats at the given index
     * 
     * @return whether or not this operation was successful
     */
    bool readPokemonStatsForIndex(uint8_t index, Gen1PokeStats& stats) const;

    /**
     * @brief Get the index of the color palette based on the pokedex number of a certain pokemon
     */
    uint8_t getColorPaletteIndexByPokemonNumber(uint8_t pokeNumber);

    /**
     * @brief This function reads the color palette with the given ID into outColorPalette
     * outColorPalette should be a pointer to an uint16_t array with 4 elements
     * 
     * Based on: https://bulbapedia.bulbagarden.net/wiki/List_of_color_palettes_by_index_number_(Generation_I)
     * 
     * @param outColorPalette 
     */
    void readColorPalette(uint8_t paletteId, uint16_t* outColorPalette);

    /**
     * @brief Get the trainer name from the save file
     * Note: the resulting const char* does not need to be free'd.
     * However, it needs to be either used rightaway or strdup()'d, because the data will get overwritten on the next call to this function.
     */
    const char* getTrainerName() const;

    /**
     * @brief Get the rival name from the save file
     * Note: the resulting const char* does not need to be free'd.
     * However, it needs to be either used rightaway or strdup()'d, because the data will get overwritten on the next call to this function.
     */
    const char* getRivalName() const;

    /**
     * @brief Get the player ID from the save file
     */
    uint16_t getTrainerID() const;

    /**
     * @brief Returns a Gen1Party instance, which can be used to retrieve the information about the pokemon currently in the trainers' party
     */
    Gen1Party getParty();

    /**
     * @brief Returns a Gen1Box instance, which can be used to retrieve information about the pokemon currently in the Trainers' PC box at the specified index
     */
    Gen1Box getBox(uint8_t boxIndex);

    /**
     * @brief Returns the 0-based index of the currently selected PC box for pokemon
     */
    uint8_t getCurrentBoxIndex();

    /**
     * @brief This function returns whether the pokemon with the given pokedexNumber has been SEEN/OWNED.
     */
    bool getPokedexFlag(PokedexFlag dexFlag, uint8_t pokedexNumber) const;

    /**
     * @brief This function sets the pokedex flag for the given pokedexNumber
     */
    void setPokedexFlag(PokedexFlag dexFlag, uint8_t pokedexNumber) const;

    /**
     * @brief This function returns the counter value of the given dex flag
     */
    uint8_t getPokedexCounter(PokedexFlag dexFlag) const;

    /** 
     * @brief This function decodes a sprite at the given bank and pointer.
     * Returns a pointer to a buffer containing the decoded gameboy sprite in gameboy sprite format.
     * The size of the returned buffer is SPRITE_BITPLANE_BUFFER_SIZE_IN_BYTES * 2
     * 
     * WARNING: this function returns a pointer to an internal buffer of the Gen1SpriteDecoder. It will get overwritten on the next call to this function.
     * If you want to keep the content around for longer, make a copy of this data.
     */
    uint8_t* decodeSprite(uint8_t bankIndex, uint16_t pointer);

    /**
     * @brief This function decodes the given pokemon icon and returns an internal buffer
     * Note that this returns a 16x16 buffer in gameboy format with tiles in vertical order.
     * You need to feed it to an instance of SpriteRenderer to convert it to a useful format
     * 
     * WARNING: this function returns a buffer to an internal buffer of Gen1IconDecoder. That means it will get overwritten on the next call to this function.
     * If you want to keep the content around for longer, make a copy of this data
     */
    uint8_t* decodePokemonIcon(Gen1PokemonIconType iconType, bool firstFrame = true);

    /**
     * @brief Adds a pokemon to the save. Tries to add it to the party first. If there's no more room there, it tries to add it to the
     * first ingame PC box with open slots
     * 
     * @param originalTrainerID optional parameter. If not specified (=null), the original trainer id string will be set to the players'
     * @param nickname optional parameter. If not specified (= null), the pokemon species name will be used instead.
     * 
     * @return 
     * 0xFF - Could not add pokemon (probably no space left)
     * 0xFE - Added to players' party
     * 0x0 - 0xB - Added to box at index <value>
     */
    uint8_t addPokemon(Gen1TrainerPokemon& poke, const char* originalTrainerID = 0, const char* nickname = 0);

    /**
     * @brief Adds a distribution pokemon to the save. Tries to add it to the party first. If there's no more room there, it tries to add it to the
     * first ingame PC box with open slots
     * 
     * @param nickname optional parameter. If not specified (= null), the pokemon species name will be used instead.
     * 
     * @return 
     * 0xFF - Could not add pokemon (probably no space left)
     * 0xFE - Added to players' party
     * 0x0 - 0xB - Added to box at index <value>
     */
    uint8_t addDistributionPokemon(const Gen1DistributionPokemon& distributionPoke, const char* nickname = 0);

    /**
     * @brief This function checks whether the main checksum of bank 1 is valid
     */
    bool isMainChecksumValid();
    
    /**
     * @brief Updates the main data checksum in bank 1
     */
    void updateMainChecksum();
    /**
     * @brief Checks whether the whole bank checksum of the given memory bank is valid.
     * As you may or may not know, bank 2 and bank 3 contain the PC Pokemon boxes of the player
     * These memory banks have 2 types of checksums: a whole memory bank checksum (which is this one)
     * And a checksum per box. (which will be dealt with in the Gen1Box class)
     * 
     * So yeah, this function is about the whole memory bank checksums in memory bank 2 and 3
     */
    bool isWholeBoxBankValid(uint8_t bankIndex);

    /**
     * @brief This function updates the checksum value of the whole box bank (bank 2 or 3)
     */
    void updateWholeBoxBankChecksum(uint8_t bankIndex);
protected:
private:
    IRomReader& romReader_;
    ISaveManager& saveManager_;
    Gen1SpriteDecoder spriteDecoder_;
    Gen1IconDecoder iconDecoder_;
    Gen1GameType gameType_;
};

#endif