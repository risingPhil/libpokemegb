#ifndef _GEN2GAMEREADER_H
#define _GEN2GAMEREADER_H

#include "gen2/Gen2SpriteDecoder.h"
#include "gen2/Gen2PlayerPokemonStorage.h"
#include "gen2/Gen2DistributionPokemon.h"

class IRomReader;
class ISaveManager;


// Note: for GEN2 there is no difference between index and pokedex number: the indexes are in national dex order

class Gen2GameReader
{
public:
    Gen2GameReader(IRomReader &romReader, ISaveManager &saveManager, Gen2GameType gameType);
    ~Gen2GameReader();

    Gen2GameType getGameType() const;

    /**
     * @brief get the name of a pokémon based on an index number
     * Note: you don't own the returned pointer. The data will get overwritten on the next call to this function,
     * so make sure to strdup() it if you need to store it for later
     *
     * @return const char*
     */
    const char *getPokemonName(uint8_t index) const;

    /**
     * @brief With this function, you can check if the index is valid and not referring to garbage data
     */
    bool isValidIndex(uint8_t index) const;

    /**
     * @brief This function reads the pokemon stats at the given index
     *
     * @return whether or not this operation was successful
     */
    bool readPokemonStatsForIndex(uint8_t index, Gen2PokeStats &outStats) const;

    /**
     * @brief This function reads the pointer to the front sprite of the pokémon at the given index
     */
    bool readFrontSpritePointer(uint8_t index, uint8_t &outBankIndex, uint16_t &outPointer);

    /**
     * @brief Converts the sprite_dimensions field in Gen2PokeStats into values we can actually use
     */
    void readSpriteDimensions(const Gen2PokeStats &poke, uint8_t &outWidthInTiles, uint8_t &outHeightInTiles);

    /**
     * @brief This function reads a color palette for the given pokemon
     * @param index the index of the pokemon
     * @param shiny whether or not we need to get the shiny color palette
     * @param outColorPalette the output variable in which the color palette will be read. Please make sure this array has a length of 4 items
     */
    bool readColorPaletteForPokemon(uint8_t index, bool shiny, uint16_t *outColorPalette);

    /**
     * @brief This function decodes a sprite at the given bank and pointer.
     * Returns a pointer to a buffer containing the decoded gameboy sprite in gameboy sprite format.
     * You need to look at the sprite dimension to see how much data there is. ()
     *
     * WARNING: this function returns a pointer to an internal buffer of the Gen1SpriteDecoder. It will get overwritten on the next call to this function.
     * If you want to keep the content around for longer, make a copy of this data.
     */
    uint8_t *decodeSprite(uint8_t bankIndex, uint16_t pointer);

    /**
     * @brief Adds a pokemon to the save. Tries to add it to the party first. If there's no more room there, it tries to add it to the
     * first ingame PC box with open slots.
     *
     * WARNING: When you're done adding pokemon, make sure to call finishSave() !!! Not doing so will corrupt the save file!
     *
     * @param isEgg adds the pokemon as an egg
     * @param originalTrainerID optional parameter. If not specified (=null), the original trainer id string will be set to the players'
     * @param nickname optional parameter. If not specified (= null), the pokemon species name will be used instead.
     *
     * @return
     * 0xFF - Could not add pokemon (probably no space left)
     * 0xFE - Added to players' party
     * 0x0 - 0xB - Added to box at index <value>
     */
    uint8_t addPokemon(Gen2TrainerPokemon &poke, bool isEgg, const char *originalTrainerID = 0, const char *nickname = 0);

    /**
     * @brief Adds a distribution pokemon to the save. Tries to add it to the party first. If there's no more room there, it tries to add it to the
     * first ingame PC box with open slots
     *
     * WARNING: When you're done adding pokemon, make sure to call finishSave() !!! Not doing so will corrupt the save file!
     *
     * @param nickname optional parameter. If not specified (= null), the pokemon species name will be used instead.
     *
     * @return
     * 0xFF - Could not add pokemon (probably no space left)
     * 0xFE - Added to players' party
     * 0x0 - 0xB - Added to box at index <value>
     */
    uint8_t addDistributionPokemon(const Gen2DistributionPokemon& distributionPoke, const char *nickname = 0);

    /**
     * @brief Get the player ID from the save file
     */
    uint16_t getTrainerID() const;

    /**
     * @brief Get the trainer name from the save file
     * Note: the resulting const char* does not need to be free'd.
     * However, it needs to be either used rightaway or strdup()'d, because the data will get overwritten on the next call to this function.
     */
    const char *getTrainerName() const;

    /**
     * @brief Get the rival name from the save file
     * Note: the resulting const char* does not need to be free'd.
     * However, it needs to be either used rightaway or strdup()'d, because the data will get overwritten on the next call to this function.
     */
    const char *getRivalName() const;

    /**
     * @brief Returns the 0-based index of the currently selected PC box for pokemon
     */
    uint8_t getCurrentBoxIndex();

    /**
     * @brief Returns a Gen2Party instance, which can be used to retrieve the information about the pokemon currently in the trainers' party
     */
    Gen2Party getParty();

    /**
     * @brief Returns a Gen2Box instance, which can be used to retrieve information about the pokemon currently in the Trainers' PC box at the specified index
     */
    Gen2Box getBox(uint8_t boxIndex);

    /**
     * Returns a Gen2ItemList instance, which can be used to retrieve the items currently in the players' specified inventory type
     */
    Gen2ItemList getItemList(Gen2ItemListType type);

    /**
     * @brief This function returns whether the pokemon with the given pokedexNumber has been SEEN/OWNED.
     */
    bool getPokedexFlag(PokedexFlag dexFlag, uint8_t pokedexNumber);

    /**
     * @brief This function sets the pokedex flag for the given pokedexNumber
     */
    void setPokedexFlag(PokedexFlag dexFlag, uint8_t pokedexNumber);

    /**
     * @brief This function returns the counter value of the given dex flag
     */
    uint8_t getPokedexCounter(PokedexFlag dexFlag);

    /**
     * @brief The save data is stored twice in gen 2. There's a main and a backup region.
     * This function returns whether the main regions' checksum is valid.
     */
    bool isMainChecksumValid();
    /**
     * @brief The save data is stored twice in gen 2. There's a main and a backup region.
     * This function returns whether the backup regions' checksum is valid.
     */
    bool isBackupChecksumValid();

    /**
     * @brief This function finishes the save file. Make sure to call this at the end after you're done doing changes
     * This function will:
     * - update the current pc box section of the save
     * - update the main checksum
     * - copy the main save to the backup save
     */
    void finishSave();

    /**
     * @brief Checks whether we're currently dealing with Pokémon Crystal
     */
    bool isGameCrystal() const;
protected:
private:
    IRomReader &romReader_;
    ISaveManager &saveManager_;
    Gen2SpriteDecoder spriteDecoder_;
    Gen2GameType gameType_;
};

#endif