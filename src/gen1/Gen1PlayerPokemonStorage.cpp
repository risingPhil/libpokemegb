#include "gen1/Gen1PlayerPokemonStorage.h"
#include "gen1/Gen1GameReader.h"
#include "gen1/Gen1Common.h"
#include "SaveManager.h"
#include "utils.h"

#include <cstring>

typedef struct Gen1TrainerPartyMeta
{
    uint8_t number_of_pokemon;
    uint8_t species_index_list[7];
} Gen1TrainerPartyMeta;

typedef struct Gen1TrainerBoxMeta
{
    uint8_t number_of_pokemon;
    uint8_t species_index_list[21];
} Gen1TrainerBoxMeta;

static const uint8_t NICKNAME_SIZE = 0xB;
static const uint8_t OT_NAME_SIZE = 0xB;

/**
 * @brief This function will load the metadata of the trainer party into the specified outPartyMeta variable.
 * Note that it won't contain the detailed data about the pokemon in the party.
 */
static bool getPartyMetadata(ISaveManager& saveManager, Gen1TrainerPartyMeta &outPartyMeta, Gen1LocalizationLanguage localization)
{
    const uint16_t savOffset = (localization != Gen1LocalizationLanguage::JAPANESE) ? 0x2F2C : 0x2ED5;
    saveManager.seek(savOffset);
    saveManager.readByte(outPartyMeta.number_of_pokemon);
    saveManager.read(outPartyMeta.species_index_list, 6);
    outPartyMeta.species_index_list[6] = 0xFF;
    return true;
}

static void writePartyMetadata(ISaveManager& saveManager, Gen1TrainerPartyMeta& partyMeta, Gen1LocalizationLanguage localization)
{
    const uint16_t savOffset = (localization != Gen1LocalizationLanguage::JAPANESE) ? 0x2F2C : 0x2ED5;
    saveManager.seek(savOffset);
    saveManager.writeByte(partyMeta.number_of_pokemon);
    saveManager.write(partyMeta.species_index_list, 6);
    saveManager.writeByte(0xFF);
}

uint8_t getGen1BoxBankIndex(uint8_t boxIndex, uint8_t currentBoxIndex)
{
    // NOTE: important: current box data to the "longterm" bank (in bank 2 or 3) until you switch the current box in the pc
    // found this while experimenting with a Pkmn Blue save saved close to the start of the game by transferring a ratata to box 1
    // the current box is stored in bank 1
    if(boxIndex == currentBoxIndex)
    {
        return 1;
    }
    return (boxIndex < 6) ? 2 : 3;
}

static uint16_t getBoxBankOffset(uint8_t boxIndex, uint8_t currentBoxIndex)
{
    const uint16_t boxSize = 0x462;

    // NOTE: important: current box data to the "longterm" bank (in bank 2 or 3) until you switch the current box in the pc
    // found this while experimenting with a Pkmn Blue save saved close to the start of the game by transferring a ratata to box 1
    // the current box is stored at this offset
    if(boxIndex == currentBoxIndex)
    {
        return 0x10C0;
    }

    return (boxIndex % 6) * boxSize;
}

static void readCommonPokeData(ISaveManager& saveManager, Gen1TrainerPokemon& outTrainerPokemon)
{
    memset(&outTrainerPokemon, 0, sizeof(Gen1TrainerPokemon));
    saveManager.readByte(outTrainerPokemon.poke_index);

    // stored in big endian form
    saveManager.readUint16(outTrainerPokemon.current_hp, Endianness::BIG);
    saveManager.readByte(outTrainerPokemon.level);
    saveManager.readByte(outTrainerPokemon.status_condition);
    saveManager.readByte(outTrainerPokemon.type1);
    saveManager.readByte(outTrainerPokemon.type2);
    saveManager.readByte(outTrainerPokemon.catch_rate_or_held_item);
    saveManager.readByte(outTrainerPokemon.index_move1);
    saveManager.readByte(outTrainerPokemon.index_move2);
    saveManager.readByte(outTrainerPokemon.index_move3);
    saveManager.readByte(outTrainerPokemon.index_move4);
    saveManager.readUint16(outTrainerPokemon.original_trainer_ID);
    // Testing with a save revealed that these fields are stored in Big endian form
    saveManager.readUint24(outTrainerPokemon.exp, Endianness::BIG);
    saveManager.readUint16(outTrainerPokemon.hp_effort_value, Endianness::BIG);
    saveManager.readUint16(outTrainerPokemon.atk_effort_value, Endianness::BIG);
    saveManager.readUint16(outTrainerPokemon.def_effort_value, Endianness::BIG);
    saveManager.readUint16(outTrainerPokemon.speed_effort_value, Endianness::BIG);
    saveManager.readUint16(outTrainerPokemon.special_effort_value, Endianness::BIG);
    // no endianness stuff for IV. Make sure to keep as is
    saveManager.read(outTrainerPokemon.iv_data, 2);
    saveManager.readByte(outTrainerPokemon.pp_move1);
    saveManager.readByte(outTrainerPokemon.pp_move2);
    saveManager.readByte(outTrainerPokemon.pp_move3);
    saveManager.readByte(outTrainerPokemon.pp_move4);
}

static void writeCommonPokeData(ISaveManager& saveManager, const Gen1TrainerPokemon& trainerPokemon)
{
    saveManager.writeByte(trainerPokemon.poke_index);
    saveManager.writeUint16(trainerPokemon.current_hp, Endianness::BIG);
    saveManager.writeByte(trainerPokemon.level);
    saveManager.writeByte(trainerPokemon.status_condition);
    saveManager.writeByte(trainerPokemon.type1);
    saveManager.writeByte(trainerPokemon.type2);
    saveManager.writeByte(trainerPokemon.catch_rate_or_held_item);
    saveManager.writeByte(trainerPokemon.index_move1);
    saveManager.writeByte(trainerPokemon.index_move2);
    saveManager.writeByte(trainerPokemon.index_move3);
    saveManager.writeByte(trainerPokemon.index_move4);
    saveManager.writeUint16(trainerPokemon.original_trainer_ID);
    // Testing with a save revealed that these fields are stored in Big endian form
    saveManager.writeUint24(trainerPokemon.exp, Endianness::BIG);
    saveManager.writeUint16(trainerPokemon.hp_effort_value, Endianness::BIG);
    saveManager.writeUint16(trainerPokemon.atk_effort_value, Endianness::BIG);
    saveManager.writeUint16(trainerPokemon.def_effort_value, Endianness::BIG);
    saveManager.writeUint16(trainerPokemon.speed_effort_value, Endianness::BIG);
    saveManager.writeUint16(trainerPokemon.special_effort_value, Endianness::BIG);
    // no endianness stuff for IV. Make sure to keep as is
    saveManager.write(trainerPokemon.iv_data, 2);
    saveManager.writeByte(trainerPokemon.pp_move1);
    saveManager.writeByte(trainerPokemon.pp_move2);
    saveManager.writeByte(trainerPokemon.pp_move3);
    saveManager.writeByte(trainerPokemon.pp_move4);
}

/**
 * @brief This function will load the metadata of the trainer box into the specified outBoxMeta variable.
 * Note that it won't contain the detailed data about the pokemon in the party.
 */
static bool getBoxMetadata(ISaveManager& saveManager, uint8_t boxIndex, uint8_t currentBoxIndex, Gen1TrainerBoxMeta& outBoxMeta)
{
    saveManager.seekToBankOffset(getGen1BoxBankIndex(boxIndex, currentBoxIndex), getBoxBankOffset(boxIndex, currentBoxIndex));
    saveManager.readByte(outBoxMeta.number_of_pokemon);
    saveManager.read(outBoxMeta.species_index_list, 20);
    outBoxMeta.species_index_list[20] = 0xFF;

    // if seems the number_of_pokemon field read is unreliable if the box hasn't been used before.
    // fortunately we can also use the species index list to determine the number
    // note, you may wonder why we do the readByte for that field in the first place then, don't you?
    // well, we still need to advance the pointer in the buffer in SaveManager. So we might as well
    // 0xFF is used as a terminator after the last entry in the list
    outBoxMeta.number_of_pokemon = 0;
    uint8_t* cur = outBoxMeta.species_index_list;
    const uint8_t* const end = cur + sizeof(outBoxMeta.species_index_list);
    while(cur < end)
    {
        if((*cur) == 0xFF)
        {
            //terminator encountered. Stop looping
            break;
        }
        ++outBoxMeta.number_of_pokemon;
        ++cur;
    }

    return true;
}

static void writeBoxMetadata(ISaveManager& saveManager, uint8_t boxIndex, uint8_t currentBoxIndex, const Gen1TrainerBoxMeta& boxMeta)
{
    saveManager.seekToBankOffset(getGen1BoxBankIndex(boxIndex, currentBoxIndex), getBoxBankOffset(boxIndex, currentBoxIndex));
    saveManager.writeByte(boxMeta.number_of_pokemon);
    saveManager.write(boxMeta.species_index_list, 20);
    saveManager.writeByte(0xFF);
}

static uint8_t calculateBoxChecksum(ISaveManager& saveManager, uint8_t boxIndex, uint8_t currentBoxIndex)
{
    Gen1Checksum checksum;
    const uint16_t boxSize = 0x462;
    uint16_t i;
    uint8_t byte;

    saveManager.seekToBankOffset(getGen1BoxBankIndex(boxIndex, currentBoxIndex), getBoxBankOffset(boxIndex, currentBoxIndex));
    
    for(i = 0; i < boxSize; ++i)
    {
        saveManager.readByte(byte);
        checksum.addByte(byte);
    }
    return checksum.get();
}

Gen1Party::Gen1Party(Gen1GameReader& gameReader, ISaveManager& savManager, Gen1LocalizationLanguage language)
    : gameReader_(gameReader)
    , saveManager_(savManager)
    , localization_(language)
{   
}

Gen1Party::~Gen1Party()
{
}

uint8_t Gen1Party::getSpeciesAtIndex(uint8_t partyIndex)
{
    Gen1TrainerPartyMeta partyMeta;
    getPartyMetadata(saveManager_, partyMeta, localization_);

    if(partyIndex >= partyMeta.number_of_pokemon)
    {
        return 0;
    }
    return partyMeta.species_index_list[partyIndex];
}

bool Gen1Party::getPokemon(uint8_t partyIndex, Gen1TrainerPokemon& outTrainerPokemon, bool shouldRecalculateLevel)
{
    Gen1PokeStats stats;
    const uint8_t PARTY_POKEMON_NUM_BYTES = 44;
    const uint8_t FIRST_POKE_STRUCT_OFFSET = 8;

    saveManager_.seek(0x2F2C + FIRST_POKE_STRUCT_OFFSET + ((partyIndex)*PARTY_POKEMON_NUM_BYTES));

    readCommonPokeData(saveManager_, outTrainerPokemon);

    // according to https://bulbapedia.bulbagarden.net/wiki/Pok%C3%A9mon_data_structure_(Generation_I)
    // the level is stored a second time here for some reason. 
    saveManager_.advance();

    saveManager_.readUint16(outTrainerPokemon.max_hp, Endianness::BIG);
    saveManager_.readUint16(outTrainerPokemon.atk, Endianness::BIG);
    saveManager_.readUint16(outTrainerPokemon.def, Endianness::BIG);
    saveManager_.readUint16(outTrainerPokemon.speed, Endianness::BIG);
    saveManager_.readUint16(outTrainerPokemon.special, Endianness::BIG);

    // the level field above is kinda unreliable. the only reliable way is to base it on the exp field
    // doing the recalculation causes a performance hit though, because the stats data is being read.
    // shouldRecalculateLevel == false lets you avoid this.
    if(shouldRecalculateLevel)
    {
        gameReader_.readPokemonStatsForIndex(outTrainerPokemon.poke_index, stats);
        outTrainerPokemon.level = getLevelForExp(outTrainerPokemon.exp, stats.growth_rate);
    }
    return true;
}

bool Gen1Party::setPokemon(uint8_t partyIndex, Gen1TrainerPokemon& poke)
{
    const uint8_t PARTY_POKEMON_NUM_BYTES = 44;
    const uint8_t FIRST_POKE_STRUCT_OFFSET = 8;
    Gen1TrainerPartyMeta partyMeta;
    getPartyMetadata(saveManager_, partyMeta, localization_);

    if(partyIndex >= partyMeta.number_of_pokemon)
    {
        // You can't use this function to add a pokemon. Only to replace one.
        return false;
    }

    partyMeta.species_index_list[partyIndex] = poke.poke_index;

    if(partyMeta.number_of_pokemon != getMaxNumberOfPokemon())
    {
        // we need to add a terminator
        partyMeta.species_index_list[partyMeta.number_of_pokemon] = 0xFF;
    }

    writePartyMetadata(saveManager_, partyMeta, localization_);

    // make sure the stat fields are filled in by recalculating them.
    // this is the same as what happens when withdrawing them from an ingame PC box
    gen1_recalculatePokeStats(gameReader_, poke);
    poke.current_hp = poke.max_hp;

    saveManager_.seek(0x2F2C + FIRST_POKE_STRUCT_OFFSET + ((partyIndex)*PARTY_POKEMON_NUM_BYTES));
    writeCommonPokeData(saveManager_, poke);

    // according to https://bulbapedia.bulbagarden.net/wiki/Pok%C3%A9mon_data_structure_(Generation_I)
    // the level is stored a second time here for some reason.
    saveManager_.writeByte(poke.level);

    saveManager_.writeUint16(poke.max_hp, Endianness::BIG);
    saveManager_.writeUint16(poke.atk, Endianness::BIG);
    saveManager_.writeUint16(poke.def, Endianness::BIG);
    saveManager_.writeUint16(poke.speed, Endianness::BIG);
    saveManager_.writeUint16(poke.special, Endianness::BIG);

    return true;
}

uint8_t Gen1Party::getNumberOfPokemon()
{
    Gen1TrainerPartyMeta partyMeta;
    if(!getPartyMetadata(saveManager_, partyMeta, localization_))
    {
        return 0;
    }
    return partyMeta.number_of_pokemon;
}

uint8_t Gen1Party::getMaxNumberOfPokemon()
{
    return 6;
}

const char* Gen1Party::getPokemonNickname(uint8_t partyIndex)
{
    static char result[20];

    uint8_t encodedNickName[NICKNAME_SIZE];
    const uint16_t FIRST_NICKNAME_NAME_OFFSET = 0x152;

    saveManager_.seek(0x2F2C + FIRST_NICKNAME_NAME_OFFSET + (partyIndex * NICKNAME_SIZE));
    saveManager_.readUntil(encodedNickName, 0x50, NICKNAME_SIZE);

    gen1_decodePokeText(encodedNickName, sizeof(encodedNickName), result, sizeof(result), (Gen1LocalizationLanguage)localization_);

    return result;
}

void Gen1Party::setPokemonNickname(uint8_t partyIndex, const char* name)
{
    const uint16_t FIRST_NICKNAME_NAME_OFFSET = 0x152;
    uint8_t encodedNickName[NICKNAME_SIZE];
    if(!name)
    {
        Gen1TrainerPokemon poke;
        getPokemon(partyIndex, poke);
        name = gameReader_.getPokemonName(poke.poke_index);
    }
    
    const uint16_t encodedLength = gen1_encodePokeText(name, strlen(name), encodedNickName, NICKNAME_SIZE, 0x50, (Gen1LocalizationLanguage)localization_);
    saveManager_.seek(0x2F2C + FIRST_NICKNAME_NAME_OFFSET + (partyIndex * NICKNAME_SIZE));
    saveManager_.write(encodedNickName, encodedLength);
}

const char* Gen1Party::getOriginalTrainerOfPokemon(uint8_t partyIndex)
{
    static char result[20];
    uint8_t encodedOTName[OT_NAME_SIZE];
    const uint16_t FIRST_OT_NAME_OFFSET = 0x110;

    saveManager_.seek(0x2F2C + FIRST_OT_NAME_OFFSET + (partyIndex * OT_NAME_SIZE));
    saveManager_.readUntil(encodedOTName, 0x50, OT_NAME_SIZE);

    gen1_decodePokeText(encodedOTName, sizeof(encodedOTName), result, sizeof(result), (Gen1LocalizationLanguage)localization_);

    return result;
}

void Gen1Party::setOriginalTrainerOfPokemon(uint8_t partyIndex, const char* originalTrainer)
{
    uint8_t encodedOTName[OT_NAME_SIZE];
    const uint16_t FIRST_OT_NAME_OFFSET = 0x110;

    const uint16_t encodedLength = gen1_encodePokeText(originalTrainer, strlen(originalTrainer), encodedOTName, OT_NAME_SIZE, 0x50, (Gen1LocalizationLanguage)localization_);

    saveManager_.seek(0x2F2C + FIRST_OT_NAME_OFFSET + (partyIndex * OT_NAME_SIZE));
    saveManager_.write(encodedOTName, encodedLength);
}

bool Gen1Party::add(Gen1TrainerPokemon& poke, const char* originalTrainerID, const char* nickname)
{
    Gen1TrainerPartyMeta partyMeta;
    getPartyMetadata(saveManager_, partyMeta, localization_);
    const uint8_t partyIndex = partyMeta.number_of_pokemon;

    if(partyIndex >= getMaxNumberOfPokemon())
    {
        // no room for this pokemon
        return false;
    }
    ++partyMeta.number_of_pokemon;

    if(partyMeta.number_of_pokemon != getMaxNumberOfPokemon())
    {
        // we need to add a terminator
        partyMeta.species_index_list[partyMeta.number_of_pokemon] = 0xFF;
    }

    writePartyMetadata(saveManager_, partyMeta, localization_);
    if(!setPokemon(partyIndex, poke))
    {
        return false;
    }

    // now store the original trainer string and name/nickname
    setOriginalTrainerOfPokemon(partyIndex, originalTrainerID);
    setPokemonNickname(partyIndex, nickname);

    return true;
}

Gen1Box::Gen1Box(Gen1GameReader& gameReader, ISaveManager& saveManager, uint8_t boxIndex, Gen1LocalizationLanguage language)
    : gameReader_(gameReader)
    , saveManager_(saveManager)
    , boxIndex_(boxIndex)
    , localization_(language)
{
}

Gen1Box::~Gen1Box()
{
}

uint8_t Gen1Box::getSpeciesAtIndex(uint8_t index)
{
    Gen1TrainerBoxMeta boxMeta;
    const uint8_t currentBoxIndex = gameReader_.getCurrentBoxIndex();
    getBoxMetadata(saveManager_, boxIndex_, currentBoxIndex, boxMeta);

    if(index >= boxMeta.number_of_pokemon)
    {
        return 0;
    }

    return boxMeta.species_index_list[index];
}

bool Gen1Box::getPokemon(uint8_t index, Gen1TrainerPokemon& outTrainerPokemon, bool shouldRecalculateLevel)
{
    Gen1PokeStats stats;
    const uint8_t BOX_POKEMON_NUM_BYTES = 33;
    const uint8_t FIRST_POKE_STRUCT_OFFSET = 22;

    const uint8_t currentBoxIndex = gameReader_.getCurrentBoxIndex();
    const uint8_t bankIndex = getGen1BoxBankIndex(boxIndex_, currentBoxIndex);
    const uint16_t boxOffset = getBoxBankOffset(boxIndex_, currentBoxIndex);

    saveManager_.seekToBankOffset(bankIndex, boxOffset + FIRST_POKE_STRUCT_OFFSET + (index * BOX_POKEMON_NUM_BYTES));
    
    readCommonPokeData(saveManager_, outTrainerPokemon);

    // the level field above is kinda unreliable. the only reliable way is to base it on the exp field
    // doing the recalculation causes a performance hit though, because the stats data is being read.
    // shouldRecalculateLevel == false lets you avoid this.
    if(shouldRecalculateLevel)
    {
        gameReader_.readPokemonStatsForIndex(outTrainerPokemon.poke_index, stats);
        outTrainerPokemon.level = getLevelForExp(outTrainerPokemon.exp, stats.growth_rate);
    }
    
    return true;
}

bool Gen1Box::setPokemon(uint8_t index, Gen1TrainerPokemon& poke)
{
    const uint8_t BOX_POKEMON_NUM_BYTES = 33;
    const uint8_t FIRST_POKE_STRUCT_OFFSET = 22;
    Gen1TrainerBoxMeta boxMeta;
    const uint8_t currentBoxIndex = gameReader_.getCurrentBoxIndex();
    getBoxMetadata(saveManager_, boxIndex_, currentBoxIndex, boxMeta);

    if(index >= boxMeta.number_of_pokemon)
    {
        // You can't use this function to add a pokemon. Only to replace one.
        return false;
    }

    boxMeta.species_index_list[index] = poke.poke_index;

    writeBoxMetadata(saveManager_, boxIndex_, currentBoxIndex, boxMeta);

    // make sure the stat fields are filled in by recalculating them.
    // this is the same as what happens when withdrawing them from an ingame PC box
    gen1_recalculatePokeStats(gameReader_, poke);
    poke.current_hp = poke.max_hp;

    const uint8_t bankIndex = getGen1BoxBankIndex(boxIndex_, currentBoxIndex);
    const uint16_t boxOffset = getBoxBankOffset(boxIndex_, currentBoxIndex);
    saveManager_.seekToBankOffset(bankIndex, boxOffset + FIRST_POKE_STRUCT_OFFSET + (index * BOX_POKEMON_NUM_BYTES));

    writeCommonPokeData(saveManager_, poke);

    // now update the checksum. Note that this is NOT the only checksum that will need to be updated. There's also the whole bank checksum.
    // this needs to be updated by Gen1GameReader once it is done manipulating the boxes
    updateChecksum(currentBoxIndex);

    return true;
}

uint8_t Gen1Box::getNumberOfPokemon()
{
    Gen1TrainerBoxMeta boxMeta;
    const uint8_t currentBoxIndex = gameReader_.getCurrentBoxIndex();
    
    if(!getBoxMetadata(saveManager_, boxIndex_, currentBoxIndex, boxMeta))
    {
        return 0;
    }
    return boxMeta.number_of_pokemon;
}

uint8_t Gen1Box::getMaxNumberOfPokemon()
{
    return 20;
}

const char* Gen1Box::getPokemonNickname(uint8_t index)
{
    static char result[20];

    uint8_t encodedNickName[NICKNAME_SIZE];
    const uint16_t nicknameOffset = 0x386;
    const uint8_t currentBoxIndex = gameReader_.getCurrentBoxIndex();
    const uint8_t bankIndex = getGen1BoxBankIndex(boxIndex_, currentBoxIndex);
    const uint16_t boxOffset = getBoxBankOffset(boxIndex_, currentBoxIndex);

    saveManager_.seekToBankOffset(bankIndex, boxOffset + nicknameOffset + (index * NICKNAME_SIZE));
    saveManager_.readUntil(encodedNickName, 0x50, NICKNAME_SIZE);

    gen1_decodePokeText(encodedNickName, sizeof(encodedNickName), result, sizeof(result), (Gen1LocalizationLanguage)localization_);

    return result;
}

void Gen1Box::setPokemonNickname(uint8_t index, const char* name)
{
    uint8_t encodedNickName[NICKNAME_SIZE];
    const uint16_t nicknameOffset = 0x386;
    const uint8_t currentBoxIndex = gameReader_.getCurrentBoxIndex();
    const uint8_t bankIndex = getGen1BoxBankIndex(boxIndex_, currentBoxIndex);
    const uint16_t boxOffset = getBoxBankOffset(boxIndex_, currentBoxIndex);
    if(!name)
    {
        Gen1TrainerPokemon poke;
        getPokemon(index, poke);
        name = gameReader_.getPokemonName(poke.poke_index);
    }
    
    const uint16_t encodedLength = gen1_encodePokeText(name, strlen(name), encodedNickName, NICKNAME_SIZE, 0x50, (Gen1LocalizationLanguage)localization_);
    saveManager_.seekToBankOffset(bankIndex, boxOffset + nicknameOffset + (index * NICKNAME_SIZE));
    saveManager_.write(encodedNickName, encodedLength);
}

const char* Gen1Box::getOriginalTrainerOfPokemon(uint8_t index)
{
    static char result[20];
    uint8_t encodedOTName[OT_NAME_SIZE];

    const uint16_t otOffset = 0x2AA;
    const uint8_t currentBoxIndex = gameReader_.getCurrentBoxIndex();
    const uint8_t bankIndex = getGen1BoxBankIndex(boxIndex_, currentBoxIndex);
    const uint16_t boxOffset = getBoxBankOffset(boxIndex_, currentBoxIndex);

    saveManager_.seekToBankOffset(bankIndex, boxOffset + otOffset + (index * OT_NAME_SIZE));
    saveManager_.readUntil(encodedOTName, 0x50, OT_NAME_SIZE);

    gen1_decodePokeText(encodedOTName, sizeof(encodedOTName), result, sizeof(result), (Gen1LocalizationLanguage)localization_);

    return result;
}

void Gen1Box::setOriginalTrainerOfPokemon(uint8_t index, const char* originalTrainer)
{
    uint8_t encodedOTName[OT_NAME_SIZE];
    const uint16_t otOffset = 0x2AA;
    const uint8_t currentBoxIndex = gameReader_.getCurrentBoxIndex();
    const uint8_t bankIndex = getGen1BoxBankIndex(boxIndex_, currentBoxIndex);
    const uint16_t boxOffset = getBoxBankOffset(boxIndex_, currentBoxIndex);


    const uint16_t encodedLength = gen1_encodePokeText(originalTrainer, strlen(originalTrainer), encodedOTName, OT_NAME_SIZE, 0x50, (Gen1LocalizationLanguage)localization_);
    
    saveManager_.seekToBankOffset(bankIndex, boxOffset + otOffset + (index * OT_NAME_SIZE));
    saveManager_.write(encodedOTName, encodedLength);
}

bool Gen1Box::add(Gen1TrainerPokemon& poke, const char* originalTrainerID, const char* nickname)
{
    Gen1TrainerBoxMeta boxMeta;
    const uint8_t currentBoxIndex = gameReader_.getCurrentBoxIndex();
    getBoxMetadata(saveManager_, boxIndex_, currentBoxIndex, boxMeta);
    const uint8_t index = boxMeta.number_of_pokemon;

    if(index >= getMaxNumberOfPokemon())
    {
        // no room for this pokemon
        return false;
    }
    
    boxMeta.species_index_list[index] = poke.poke_index;
    ++boxMeta.number_of_pokemon;

    if(boxMeta.number_of_pokemon != getMaxNumberOfPokemon())
    {
        // we need to add a terminator
        boxMeta.species_index_list[boxMeta.number_of_pokemon] = 0xFF;
    }

    writeBoxMetadata(saveManager_, boxIndex_, currentBoxIndex, boxMeta);
    if(!setPokemon(index, poke))
    {
        return false;
    }

    // now store the original trainer string and name/nickname
    setOriginalTrainerOfPokemon(index, originalTrainerID);
    setPokemonNickname(index, nickname);

    // now update the checksum. Note that this is NOT the only checksum that will need to be updated. There's also the whole bank checksum.
    // this needs to be updated by Gen1GameReader once it is done manipulating the boxes
    updateChecksum(currentBoxIndex);

    return true;
}

bool Gen1Box::isChecksumValid(uint8_t currentBoxIndex)
{
    uint8_t storedChecksum;
    uint8_t calculatedBoxChecksum;
    if(boxIndex_ == currentBoxIndex)
    {
        return true;
    }

    const uint8_t bankIndex = getGen1BoxBankIndex(boxIndex_, currentBoxIndex);
    const uint16_t offset = 0x1A4D + (boxIndex_ % 6);

    saveManager_.seekToBankOffset(bankIndex, offset);
    saveManager_.readByte(storedChecksum);

    calculatedBoxChecksum = calculateBoxChecksum(saveManager_, boxIndex_, currentBoxIndex);

    return (storedChecksum == calculatedBoxChecksum);
}

void Gen1Box::updateChecksum(uint8_t currentBoxIndex)
{
    if(boxIndex_ == currentBoxIndex)
    {
        return;
    }
    const uint8_t calculatedBoxChecksum = calculateBoxChecksum(saveManager_, boxIndex_, currentBoxIndex);

    const uint8_t bankIndex = getGen1BoxBankIndex(boxIndex_, currentBoxIndex);
    const uint16_t offset = 0x1A4D + (boxIndex_ % 6);

    saveManager_.seekToBankOffset(bankIndex, offset);
    saveManager_.writeByte(calculatedBoxChecksum);
}