#include "gen2/Gen2PlayerPokemonStorage.h"
#include "gen2/Gen2GameReader.h"
#include "SaveManager.h"

#include <cstring>

static const uint8_t ORIGINAL_TRAINER_NAME_SIZE = 0xB;
static const uint8_t ORIGINAL_TRAINER_NAME_SIZE_JPN = 0x6;
static const uint8_t NICKNAME_SIZE = 0xB;
static const uint8_t NICKNAME_SIZE_JPN = 0x6;

static const uint8_t PARTY_LIST_CAPACITY = 6;
static const uint8_t PARTY_LIST_PKMN_ENTRY_SIZE = 48;
static const uint8_t PC_BOX_LIST_CAPACITY = 20;
static const uint8_t PC_BOX_LIST_CAPACITY_JPN = 30;
static const uint8_t PC_BOX_LIST_PKMN_ENTRY_SIZE = 32;

Gen2PokemonList::Gen2PokemonList(Gen2GameReader& gameReader, ISaveManager& saveManager, uint8_t listCapacity, uint8_t entrySize, Gen2LocalizationLanguage language)
    : gameReader_(gameReader)
    , saveManager_(saveManager)
    , localization_(language)
    , listCapacity_(listCapacity)
    , entrySize_(entrySize)
{
}

Gen2PokemonList::~Gen2PokemonList()
{
}

uint8_t Gen2PokemonList::getNumberOfPokemon()
{
    uint8_t result;
    if(!saveManager_.seek(getSaveOffset()))
    {
        return 0;
    }
    saveManager_.readByte(result);
    return result;
}

uint8_t Gen2PokemonList::getMaxNumberOfPokemon() const
{
    return listCapacity_;
}

bool Gen2PokemonList::setNumberOfPokemon(uint8_t count)
{
    if(!saveManager_.seek(getSaveOffset()))
    {
        return false;
    }
    saveManager_.writeByte(count);
    return true;
}

uint8_t Gen2PokemonList::getSpeciesAtIndex(uint8_t index)
{
    if(!saveManager_.seek(getSaveOffset() + 1 + index))
    {
        return 0;
    }
    return getNextSpecies();
}

bool Gen2PokemonList::setSpeciesAtIndex(uint8_t index, uint8_t speciesIndex)
{
    if(!saveManager_.seek(getSaveOffset() + 1 + index))
    {
        return false;
    }
    saveManager_.writeByte(speciesIndex);
    return true;
}

uint8_t Gen2PokemonList::getNextSpecies()
{
    uint8_t result;
    saveManager_.readByte(result);

    return result;
}

bool Gen2PokemonList::getPokemon(uint8_t index, Gen2TrainerPokemon& outPoke)
{
    if(!saveManager_.seek(getSaveOffset() + listCapacity_ + 2 + (index * entrySize_)))
    {
        return false;
    }
    return getNextPokemon(outPoke);
}

bool Gen2PokemonList::isEgg(uint8_t index)
{
    return (getSpeciesAtIndex(index) == 0xFD);
}

bool Gen2PokemonList::getNextPokemon(Gen2TrainerPokemon& outPoke)
{
    saveManager_.readByte(outPoke.poke_index);
    saveManager_.readByte(outPoke.held_item_index);
    saveManager_.readByte(outPoke.index_move1);
    saveManager_.readByte(outPoke.index_move2);
    saveManager_.readByte(outPoke.index_move3);
    saveManager_.readByte(outPoke.index_move4);
    saveManager_.readUint16(outPoke.original_trainer_ID, Endianness::BIG);
    saveManager_.readUint24(outPoke.exp, Endianness::BIG);
    saveManager_.readUint16(outPoke.hp_effort_value, Endianness::BIG);
    saveManager_.readUint16(outPoke.atk_effort_value, Endianness::BIG);
    saveManager_.readUint16(outPoke.def_effort_value, Endianness::BIG);
    saveManager_.readUint16(outPoke.speed_effort_value, Endianness::BIG);
    saveManager_.readUint16(outPoke.special_effort_value, Endianness::BIG);
    // no endianness stuff for IV. Make sure to keep as is
    saveManager_.read(outPoke.iv_data, 2);
    saveManager_.readByte(outPoke.pp_move1);
    saveManager_.readByte(outPoke.pp_move2);
    saveManager_.readByte(outPoke.pp_move3);
    saveManager_.readByte(outPoke.pp_move4);
    saveManager_.readByte(outPoke.friendship_or_remaining_egg_cycles);
    saveManager_.readByte(outPoke.pokerus);
    saveManager_.readUint16(outPoke.caught_data, Endianness::BIG);
    saveManager_.readByte(outPoke.level);

    if(entrySize_ == 48)
    {
        // party pokemon -> read "temporary" field values
        saveManager_.readByte(outPoke.status_condition);
        saveManager_.readByte(outPoke.unused_byte);
        saveManager_.readUint16(outPoke.current_hp, Endianness::BIG);
        saveManager_.readUint16(outPoke.max_hp, Endianness::BIG);
        saveManager_.readUint16(outPoke.atk, Endianness::BIG);
        saveManager_.readUint16(outPoke.def, Endianness::BIG);
        saveManager_.readUint16(outPoke.speed, Endianness::BIG);
        saveManager_.readUint16(outPoke.special_atk, Endianness::BIG);
        saveManager_.readUint16(outPoke.special_def, Endianness::BIG);
    }
    else
    {
        // box pokemon -> recalculate "temporary" field values
        outPoke.status_condition = 0;
        outPoke.unused_byte = 0;
        gen2_recalculatePokeStats(gameReader_, outPoke);
    }
    return false;
}

bool Gen2PokemonList::setPokemon(uint8_t index, const Gen2TrainerPokemon& orig, bool isEgg)
{
    Gen2TrainerPokemon poke;
    Gen2PokeStats stats;
    uint8_t remainingEggCycles;

    // work on a copy of the incoming pokemon
    // this is needed to keep it const on passing, but modify it during writing (for the temp stats)
    memcpy(&poke, &orig, sizeof(Gen2TrainerPokemon));

    // if the pokemon is supposed to be an egg, the species index needs to be set as 0xFD
    if(!setSpeciesAtIndex(index, (isEgg) ? 0xFD : poke.poke_index))
    {
        return false;
    }

    if(!saveManager_.seek(getSaveOffset() + listCapacity_ + 2 + (index * entrySize_)))
    {
        return false;
    }

    if(isEgg)
    {
        //if the pokemon is supposed to be an egg, the remaining egg cycles field needs to be set accordingly
        gameReader_.readPokemonStatsForIndex(poke.poke_index, stats);
        remainingEggCycles = stats.egg_cycles;
    }
    else
    {
        remainingEggCycles = poke.friendship_or_remaining_egg_cycles;
    }

    saveManager_.writeByte(poke.poke_index);
    saveManager_.writeByte(poke.held_item_index);
    saveManager_.writeByte(poke.index_move1);
    saveManager_.writeByte(poke.index_move2);
    saveManager_.writeByte(poke.index_move3);
    saveManager_.writeByte(poke.index_move4);
    saveManager_.writeUint16(poke.original_trainer_ID, Endianness::BIG);
    saveManager_.writeUint24(poke.exp, Endianness::BIG);
    saveManager_.writeUint16(poke.hp_effort_value, Endianness::BIG);
    saveManager_.writeUint16(poke.atk_effort_value, Endianness::BIG);
    saveManager_.writeUint16(poke.def_effort_value, Endianness::BIG);
    saveManager_.writeUint16(poke.speed_effort_value, Endianness::BIG);
    saveManager_.writeUint16(poke.special_effort_value, Endianness::BIG);
    // no endianness stuff for IV. Make sure to keep as is
    saveManager_.write(poke.iv_data, 2);
    saveManager_.writeByte(poke.pp_move1);
    saveManager_.writeByte(poke.pp_move2);
    saveManager_.writeByte(poke.pp_move3);
    saveManager_.writeByte(poke.pp_move4);
    saveManager_.writeByte(remainingEggCycles);
    saveManager_.writeByte(poke.pokerus);
    saveManager_.writeUint16(poke.caught_data, Endianness::BIG);
    saveManager_.writeByte(poke.level);

    if(entrySize_ == 48)
    {
        // party pokemon -> write "temporary" field values

        gen2_recalculatePokeStats(gameReader_, poke);
        poke.current_hp = poke.max_hp;

        saveManager_.writeByte(poke.status_condition);
        saveManager_.writeByte(poke.unused_byte);
        saveManager_.writeUint16(poke.current_hp, Endianness::BIG);
        saveManager_.writeUint16(poke.max_hp, Endianness::BIG);
        saveManager_.writeUint16(poke.atk, Endianness::BIG);
        saveManager_.writeUint16(poke.def, Endianness::BIG);
        saveManager_.writeUint16(poke.speed, Endianness::BIG);
        saveManager_.writeUint16(poke.special_atk, Endianness::BIG);
        saveManager_.writeUint16(poke.special_def, Endianness::BIG);
    }
    return true;
}

bool Gen2PokemonList::add(const Gen2TrainerPokemon& poke, bool isEgg, const char* originalTrainerID, const char* nickname)
{
    const uint8_t oldNumberOfPokemon = getNumberOfPokemon();
    const uint8_t speciesListTerminator = 0xFF;
    if(oldNumberOfPokemon + 1 > getMaxNumberOfPokemon())
    {
        return false;
    }

    // first add a terminator on the next entry
    if(!setSpeciesAtIndex(oldNumberOfPokemon + 1, speciesListTerminator))
    {
        // failure
        return false;
    }

    //now add the species index of the new pokemon at the end of the list
    if(!setSpeciesAtIndex(oldNumberOfPokemon, poke.poke_index))
    {
        // failure
        return false;
    }

    if(!setPokemon(oldNumberOfPokemon, poke, isEgg))
    {
        //failure: undo species index update by replacing it with the terminator
        setSpeciesAtIndex(oldNumberOfPokemon, speciesListTerminator);
        return false;
    }

    setOriginalTrainerOfPokemon(oldNumberOfPokemon, originalTrainerID);
    if(isEgg)
    {
        setPokemonNickname(oldNumberOfPokemon, "EGG");
    }
    else
    {
        setPokemonNickname(oldNumberOfPokemon, nickname);
    }

    // if everything was successful, update the counter of the list
    return setNumberOfPokemon(oldNumberOfPokemon + 1);
}

const char* Gen2PokemonList::getPokemonNickname(uint8_t index)
{
    static char result[20];
    const uint8_t pokeTextTerminator = 0x50;
    const uint8_t otSize = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? ORIGINAL_TRAINER_NAME_SIZE : ORIGINAL_TRAINER_NAME_SIZE_JPN;
    const uint8_t nickNameSize = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? NICKNAME_SIZE : NICKNAME_SIZE_JPN;

    uint8_t encodedNickName[NICKNAME_SIZE];
    const uint16_t nicknameOffset = listCapacity_ + 2 + (listCapacity_ * entrySize_) + (listCapacity_ * otSize) + (index * nickNameSize);

    if(!saveManager_.seek(getSaveOffset() + nicknameOffset))
    {
        return nullptr;
    }

    saveManager_.readUntil(encodedNickName, pokeTextTerminator, nickNameSize);

    gen2_decodePokeText(encodedNickName, sizeof(encodedNickName), result, sizeof(result), localization_);

    return result;
}

bool Gen2PokemonList::setPokemonNickname(uint8_t index, const char* nickname)
{
    uint8_t encodedNickName[NICKNAME_SIZE];
    const uint8_t pokeTextTerminator = 0x50;
    const uint8_t otSize = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? ORIGINAL_TRAINER_NAME_SIZE : ORIGINAL_TRAINER_NAME_SIZE_JPN;
    const uint8_t nickNameSize = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? NICKNAME_SIZE : NICKNAME_SIZE_JPN;

    if(!nickname)
    {
        Gen2TrainerPokemon poke;
        getPokemon(index, poke);
        nickname = gameReader_.getPokemonName(poke.poke_index);
    }
    
    const uint16_t encodedLength = gen2_encodePokeText(nickname, strlen(nickname), encodedNickName, nickNameSize, pokeTextTerminator, localization_);
    const uint16_t nicknameOffset = listCapacity_ + 2 + (listCapacity_ * entrySize_) + (listCapacity_ * otSize) + (index * nickNameSize);

    if(!saveManager_.seek(getSaveOffset() + nicknameOffset))
    {
        return false;
    }

    saveManager_.write(encodedNickName, encodedLength);
    return true;
}

const char* Gen2PokemonList::getOriginalTrainerOfPokemon(uint8_t index)
{
    static char result[20];
    const uint8_t pokeTextTerminator = 0x50;
    const uint8_t otSize = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? ORIGINAL_TRAINER_NAME_SIZE : ORIGINAL_TRAINER_NAME_SIZE_JPN;

    uint8_t encodedOTName[ORIGINAL_TRAINER_NAME_SIZE];
    const uint16_t originalTrainerOffset = listCapacity_ + 2 + (listCapacity_ * entrySize_) + (index * otSize);

    if(!saveManager_.seek(getSaveOffset() + originalTrainerOffset))
    {
        return nullptr;
    }

    saveManager_.readUntil(encodedOTName, pokeTextTerminator, otSize);

    gen2_decodePokeText(encodedOTName, sizeof(encodedOTName), result, sizeof(result), localization_);

    return result;
}

bool Gen2PokemonList::setOriginalTrainerOfPokemon(uint8_t index, const char* originalTrainerID)
{
    uint8_t encodedOTName[ORIGINAL_TRAINER_NAME_SIZE];
    const uint8_t pokeTextTerminator = 0x50;
    const uint8_t otSize = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? ORIGINAL_TRAINER_NAME_SIZE : ORIGINAL_TRAINER_NAME_SIZE_JPN;

    const uint16_t encodedLength = gen2_encodePokeText(originalTrainerID, strlen(originalTrainerID), encodedOTName, otSize, pokeTextTerminator, localization_);
    const uint16_t originalTrainerOffset = listCapacity_ + 2 + (listCapacity_ * entrySize_) + (index * otSize);

    if(!saveManager_.seek(getSaveOffset() + originalTrainerOffset))
    {
        return false;
    }
    saveManager_.write(encodedOTName, encodedLength);
    return true;
}

Gen2Party::Gen2Party(Gen2GameReader& gameReader, ISaveManager& saveManager, Gen2LocalizationLanguage language)
    : Gen2PokemonList(gameReader, saveManager, PARTY_LIST_CAPACITY, PARTY_LIST_PKMN_ENTRY_SIZE, language)
{
}

Gen2Party::~Gen2Party()
{
}

uint32_t Gen2Party::getSaveOffset()
{
    return gen2_getSRAMOffsets(gameReader_.getGameType(), localization_).party;
}

Gen2Box::Gen2Box(Gen2GameReader& gameReader, ISaveManager& saveManager, uint8_t boxIndex, Gen2LocalizationLanguage language)
    : Gen2PokemonList(gameReader, saveManager, (language != Gen2LocalizationLanguage::JAPANESE) ? PC_BOX_LIST_CAPACITY : PC_BOX_LIST_CAPACITY_JPN, PC_BOX_LIST_PKMN_ENTRY_SIZE, language)
    , boxIndex_(boxIndex)
{
}

Gen2Box::~Gen2Box()
{
}

uint32_t Gen2Box::getSaveOffset()
{
    uint32_t result;

    switch(boxIndex_)
    {
        case 0:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x4000 : 0x4000;
            break;
        case 1:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x4450 : 0x454A;
            break;
        case 2:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x48A0 : 0x4A94;
            break;
        case 3:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x4CF0 : 0x4FDE;
            break;
        case 4:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x5140 : 0x5528;
            break;
        case 5:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x5590 : 0x5A72;
            break;
        case 6:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x59E0 : 0x6000;
            break;
        case 7:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x6000 : 0x654A;
            break;
        case 8:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x6450 : 0x6A94;
            break;
        case 9:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x68A0 : 0x0;
            break;
        case 10:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x6CF0 : 0x0;
            break;
        case 11:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x7140 : 0x0;
            break;
        case 12:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x7590 : 0x0;
            break;
        case 13:
            result = (localization_ != Gen2LocalizationLanguage::JAPANESE) ? 0x79E0 : 0x0;
            break;
        default:
            result = 0;
            break;
    }
    return result;
}