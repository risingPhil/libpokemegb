#include "gen2/Gen2MysteryGiftManager.h"
#include "gen2/Gen2GameReader.h"
#include "SaveManager.h"

#include <string.h>
#include <cstdlib>

#define MYSTERYGIFT_OFFSET_ITEM 1
#define MYSTERYGIFT_OFFSET_NUM_PARTNERS 2
#define MYSTERYGIFT_OFFSET_PARTNER_IDS 3
#define MYSTERYGIFT_OFFSET_DECORATIONS_RECEIVED 10
#define MYSTERYGIFT_OFFSET_TIMER 20
#define MYSTERYGIFT_OFFSET_TRAINER_HOUSE_FLAG 23

#define MYSTERYGIFT_POKEME64_TRAINER_ID 0xADA0

static const uint8_t commonItemSet[] = {
    0xAD, // BERRY
    0x4E, // PRZCUREBERRY
    0x54, // MINT BERRY
    0x50, // ICE BERRY
    0x4F, // BURNT BERRY
    0x4A, // PSNCUREBERRY
    0x29, // GUARD SPEC
    0x33, // X DEFEND
    0x31, // X ATTACK
    0x53, // BITTER BERRY
    0x2C, // DIRE HIT
    0x35, // X SPECIAL
    0x21, // X ACCURACY
    0xB9, // EON MAIL
    0xBA, // MORPH MAIL
    0xBC // MUSIC MAIL
};

static const uint8_t uncommonItemSet[] = {
    0x6D, // MIRACLEBERRY
    0xAE, // GOLD BERRY
    0x27, // REVIVE
    0x04, // GREAT BALL
    0x2A, // SUPER REPEL
    0x2B, // MAX REPEL
    0x41, // ELIXER
    0x3F, // ETHER
    0xBB, // BLUESKY MAIL
    0xBD  // MIRAGE MAIL
};

static const uint8_t rareItemSet[] = {
    0x18, // WATER STONE
    0x16, // FIRE STONE
    0x22, // LEAF STONE
    0x17, // THUNDER STONE
    0x40, // MAX ETHER
    0x15, // MAX ELIXER
    0x28, // MAX REVIVE
    0x8C // SCOPE LENS
};

static const uint8_t veryRareItemSet[] = {
    0x1A, // HP UP
    0x3E, // PP UP
    0x20, // RARE CANDY
};

// NOTE: the decoration sets are defined in the exact right order for the decorationsReceived flag array.
// Don't modify it, because we will make use of this order to figure out the index of a flag.

static const uint8_t commonDecorationSet[] = {
    33, // JIGGLYPUFF DOLL
    37, // POLIWAG DOLL
    38, // DIGLETT DOLL
    39, // STARYU DOLL
    40, // MAGIKARP DOLL
    41, // ODDISH DOLL
    42, // GENGAR DOLL
    43, // SHELLDER DOLL
    44, // GRIMER DOLL
    45, // VOLTORB DOLL
    18, // CLEFAIRY POSTER
    19, // JIGGLYPUFF POSTER
    22, // SUPER NES
    46, // WEEDLE DOLL
    48, // GEODUDE DOLL
    49 // MACHOP DOLL
};

static const uint8_t uncommonDecorationSet[] = {
    12, // MAGNAPLANT
    13, // TROPICPLANT
    21, // NES
    23, // NINTENDO64
    34, // BULBASAUR DOLL
    36, // SQUIRTLE DOLL
    3, // PINK BED
    4 // POLKADOT BED
};

static const uint8_t rareDecorationSet[] = {
    7, // RED CARPET
    8, // BLUE CARPET
    9, // YELLOW CARPET
    10, // GREEN CARPET
    14, // JUMBOPLANT
    24, // VIRTUAL BOY
    27, // BIG ONIX
    17 // PIKACHU POSTER
};

static const uint8_t veryRareDecorationSet[] = {
    28, // BIG LAPRAS
    31, // SURF PIKACHU DOLL
    5, // PIKACHU BED
    47, // UNOWN DOLL
    50, // TENTACOOL DOLL
};

static uint32_t getMysteryGiftBaseSRAMOffset(Gen2GameType gameType, Gen2LocalizationLanguage localization)
{
    return gen2_getSRAMOffsets(gameType, localization).mysteryGiftUnlocked;
}

static bool seekToMysteryGiftOffset(ISaveManager& saveManager, Gen2GameType gameType, Gen2LocalizationLanguage localization, uint8_t offset)
{
    const uint32_t baseSRAMOffset = getMysteryGiftBaseSRAMOffset(gameType, localization);
    if(!baseSRAMOffset)
    {
        // not supported by current game localization
        return false;
    }

    if(!saveManager.seek(baseSRAMOffset + offset))
    {
        // can't seek
        return false;
    }
    return true;
}

/**
 * searches the index of the given decorationID in the specified list
 */
static uint8_t getIndexOfDecorationInList(const uint8_t* decorationList, size_t listSize, uint8_t decorationID)
{
    for(uint8_t i=0; i < listSize; ++i)
    {
        if(decorationList[i] == decorationID)
        {
            return i;
        }
    }
    return 0xFF;
}

/**
 * @brief This function searches the various decoration sets for the given decorationID
 * and returns the index total. The decoration sets above were defined in the exact right order for this.
 */
static uint8_t getMysteryGiftDecorationReceivedFlagIndexFor(uint8_t decorationID)
{
    const uint8_t* decoSets[] = { commonDecorationSet, uncommonDecorationSet, rareDecorationSet, veryRareDecorationSet };
    const size_t listSizes[] = { sizeof(commonDecorationSet), sizeof(uncommonDecorationSet), sizeof(rareDecorationSet), sizeof(veryRareDecorationSet) };
    const uint8_t numSets = sizeof(listSizes) / sizeof(const size_t);
    uint8_t baseIndex = 0;
    uint8_t ret;

    for(uint8_t i=0; i < numSets; ++i)
    {
        ret = getIndexOfDecorationInList(decoSets[i], listSizes[i], decorationID);
        if(ret != 0xFF)
        {
            return baseIndex + ret;
        }
        // not found, try next set. We add the listSize to the baseIndex though
        baseIndex += listSizes[i];
    }

    return 0xFF;
}

static uint16_t constructDayTimer(uint8_t numberOfDays, uint8_t curDay)
{
    return (numberOfDays << 8) | curDay;
}

static void readDayTimer(uint16_t timer, uint8_t& numberOfDays, uint8_t& startDay)
{
    numberOfDays = static_cast<uint8_t>(timer >> 8);
    startDay = static_cast<uint8_t>(timer & 0xFF);
}

MysteryGiftSelection selectRandomGift()
{
    MysteryGiftSelection result;
    uint32_t randomValue = (uint32_t)rand();

    const uint8_t* selectedItemSet = commonItemSet;
    const uint8_t* selectedDecorationSet = commonDecorationSet;
    size_t selectedItemSetSize = sizeof(commonItemSet);
    size_t selectedDecorationSetSize = sizeof(commonDecorationSet);
    bool shouldReturnDecoration;
    uint8_t setIndex;

    // according to https://bulbapedia.bulbagarden.net/wiki/Mystery_Gift#Item_gift_set
    // we may need up to 3 8 bit random numbers
    // since rand() generates a 32 bit number, we just use every byte of it separately, instead of generating multiple random numbers
    if((randomValue & 0xFF) <= 25)
    {
        // uncommon set. next roll for rare
        randomValue >>= 8;

        if((randomValue & 0xFF) <= 50)
        {
            // rare set, next roll for very rare
            randomValue >>= 8;

            if((randomValue & 0xFF) <= 50)
            {
                //very rare set
                selectedItemSet = veryRareItemSet;
                selectedDecorationSet = veryRareDecorationSet;
                selectedItemSetSize = sizeof(veryRareItemSet);
                selectedDecorationSetSize = sizeof(veryRareDecorationSet);
            }
            else
            {
                // rare set
                selectedItemSet = rareItemSet;
                selectedDecorationSet = rareDecorationSet;
                selectedItemSetSize = sizeof(rareItemSet);
                selectedDecorationSetSize = sizeof(rareDecorationSet);
            }
        }
        else
        {
            selectedItemSet = uncommonItemSet;
            selectedDecorationSet = uncommonDecorationSet;
            selectedItemSetSize = sizeof(uncommonItemSet);
            selectedDecorationSetSize = sizeof(uncommonDecorationSet);
        }
    }

    // worst case, after the bitshift we only have the upper 8 bits left (well, at the bottom 8 bits now)
    randomValue >>= 8;
    // use the first bit of the resulting value to determine whether to return a decoration or an item
    shouldReturnDecoration = (randomValue & 0x1);
    // now generate a new random number for the index in the list
    randomValue = (uint32_t)rand();

    result.useDecoration = shouldReturnDecoration;

    // determine decoration to be used
    setIndex = (randomValue & 0xFF) % selectedDecorationSetSize;
    result.decorationID = selectedDecorationSet[setIndex];

    // now use the next 8 bits of the random value for the selected item.
    randomValue >>= 8;
    setIndex = (randomValue & 0xFF) % selectedItemSetSize;
    result.itemID = selectedItemSet[setIndex];

    return result;
}

Gen2MysteryGiftManager::Gen2MysteryGiftManager(ISaveManager& saveManager, Gen2GameType gameType, Gen2LocalizationLanguage language)
    : saveManager_(saveManager)
    , gameType_(gameType)
    , localization_(language)
    , mysteryGiftPartnerIdBuffer_()
{
}

Gen2MysteryGiftManager::~Gen2MysteryGiftManager()
{
}

bool Gen2MysteryGiftManager::isUnlocked() const
{
    uint8_t value;

    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, 0))
    {
        // can't seek or unsupported by game localization
        return false;
    }

    if(!saveManager_.readByte(value))
    {
        // can't read
        return false;
    }
    return (value != 255);
}

void Gen2MysteryGiftManager::unlock()
{
    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, 0))
    {
        // can't seek or unsupported by game localization
        return;
    }
    saveManager_.rewind();
    // sMysteryGiftItem to 0
    saveManager_.writeByte(0);
    // sMysteryGiftUnlocked to 0
    saveManager_.writeByte(0);
    // sBackupMysteryGiftItem to 0
    saveManager_.writeByte(0);
    setNumberOfDailyMysteryGiftPartners(0);
}

MysteryGiftResult Gen2MysteryGiftManager::obtain(Gen2GameReader& gameReader, IRTCReader& rtcReader, const MysteryGiftSelection& selectedMysteryGift)
{
    Gen2ItemListType itemListType;
    MysteryGiftResult result;
    uint8_t numMysteryGiftPartners;
    const uint8_t currentDay = (uint8_t)gameReader.getClockManager(rtcReader).getDay();
    uint8_t previousTimerStartDay;
    uint8_t previousTimerNumDays;

    if(!isUnlocked())
    {
        return MYSTERYGIFT_RESULT_ERROR_NOT_UNLOCKED;
    }

    // if the mystery gift day timer has expired, reset the number of mystery gift partners
    readDayTimer(getMysteryGiftTimerValue(), previousTimerNumDays, previousTimerStartDay);
    if(currentDay >= ((previousTimerStartDay + previousTimerNumDays) % 7))
    {
        // reset number of daily mystery gift partners
        setNumberOfDailyMysteryGiftPartners(0);
    }

    numMysteryGiftPartners = getNumberOfDailyMysteryGiftPartners();
    if(numMysteryGiftPartners >= 5)
    {
        return MYSTERYGIFT_RESULT_ERROR_TOO_MANY_GIFTS;
    }

    setDailyMysteryGiftPartnerAtIndex(numMysteryGiftPartners, MYSTERYGIFT_POKEME64_TRAINER_ID);
    ++numMysteryGiftPartners;
    setNumberOfDailyMysteryGiftPartners(numMysteryGiftPartners);
    setMysteryGiftTimerValue(constructDayTimer(1, currentDay));

    if(selectedMysteryGift.useDecoration)
    {
        const uint16_t flagNumber = gen2_convertDecorationIDIntoEventFlag(selectedMysteryGift.decorationID);
        const uint8_t mysteryGiftDecorationFlagIndex = getMysteryGiftDecorationReceivedFlagIndexFor(selectedMysteryGift.decorationID);
        if(!isDecorationsReceivedFlagSet(mysteryGiftDecorationFlagIndex))
        {
            gameReader.setEventFlag(flagNumber, true);
            setDecorationsReceivedFlag(mysteryGiftDecorationFlagIndex);
            return MYSTERYGIFT_RESULT_DECORATION;
        }
        else
        {
            // The player already has the decoration. Continue with the item procurement below!
        }
    }

    // GREAT BALL needs to go into the ball pocket
    if(selectedMysteryGift.itemID == 0x04)
    {
        itemListType = Gen2ItemListType::GEN2_ITEMLISTTYPE_BALLPOCKET;
        result = MYSTERYGIFT_RESULT_ITEM_BALLPOCKET;
    }
    else
    {
        itemListType = Gen2ItemListType::GEN2_ITEMLISTTYPE_ITEMPOCKET;
        result = MYSTERYGIFT_RESULT_ITEM_ITEMPOCKET;
    }

    Gen2ItemList itemList = gameReader.getItemList(itemListType);
    if(!itemList.add(selectedMysteryGift.itemID, 1))
    {
        // adding the item failed!
        // revert the addition of the mystery gift partner entry
        setDailyMysteryGiftPartnerAtIndex(numMysteryGiftPartners - 1, 0);
        setNumberOfDailyMysteryGiftPartners(numMysteryGiftPartners - 1);
        result = MYSTERYGIFT_RESULT_ERROR_NO_ROOM_FOR_ITEM;
    }

    return result;
}

uint8_t Gen2MysteryGiftManager::getMysteryGiftItem() const
{
    uint8_t value;

    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, MYSTERYGIFT_OFFSET_ITEM))
    {
        // can't seek or unsupported by game localization
        return false;
    }

    if(!saveManager_.readByte(value))
    {
        // can't read
        return 0;
    }
    return value;
}

void Gen2MysteryGiftManager::setMysteryGiftItem(uint8_t itemIndex)
{
    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, MYSTERYGIFT_OFFSET_ITEM))
    {
        // can't seek or unsupported by game localization
        return;
    }

    saveManager_.writeByte(itemIndex);
}

uint8_t Gen2MysteryGiftManager::getNumberOfDailyMysteryGiftPartners() const
{
    uint8_t value;

    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, MYSTERYGIFT_OFFSET_NUM_PARTNERS))
    {
        // can't seek or unsupported by game localization
        return 0xFF;
    }

    if(!saveManager_.readByte(value))
    {
        // can't read
        return 0xFF;
    }
    return value;
}

void Gen2MysteryGiftManager::setNumberOfDailyMysteryGiftPartners(uint8_t numPartners)
{
    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, MYSTERYGIFT_OFFSET_NUM_PARTNERS))
    {
        // can't seek or unsupported by game localization
        return;
    }

    saveManager_.writeByte(numPartners);
}

uint16_t* Gen2MysteryGiftManager::getDailyMysteryGiftPartnerIDs() const
{
    uint16_t trainerID;
    memset(mysteryGiftPartnerIdBuffer_, 0, sizeof(mysteryGiftPartnerIdBuffer_));

    const uint8_t numPartners = getNumberOfDailyMysteryGiftPartners();
    if(numPartners != 0xFF)
    {
        // we don't need to seek because after reading the number of partners
        // we should be exactly at the right sram offset
        for(uint8_t i=0; i < numPartners; ++i)
        {
            saveManager_.readUint16(trainerID, Endianness::BIG);
            mysteryGiftPartnerIdBuffer_[i] = trainerID;
        }
    }

    return mysteryGiftPartnerIdBuffer_;
}

bool Gen2MysteryGiftManager::setDailyMysteryGiftPartnerAtIndex(uint8_t index, uint16_t trainerID)
{
    if(index >= MYSTERYGIFT_MAX_PARTNERS)
    {
        return false;
    }

    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, MYSTERYGIFT_OFFSET_PARTNER_IDS + (index * sizeof(uint16_t))))
    {
        // can't seek or unsupported by game localization
        return false;
    }

    saveManager_.writeUint16(trainerID, Endianness::BIG);
    return true;
}

bool Gen2MysteryGiftManager::isDecorationsReceivedFlagSet(uint8_t flagIndex) const
{
    const uint8_t byteIndex = flagIndex / 8;
    const uint8_t flag = 1 << (flagIndex % 8);
    uint8_t byteVal;

    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, MYSTERYGIFT_OFFSET_DECORATIONS_RECEIVED + byteIndex))
    {
        // can't seek or unsupported by game localization
        return false;
    }

    if(!saveManager_.readByte(byteVal))
    {
        // Can't read
        return false;
    }

    return (byteVal & flag);
}

void Gen2MysteryGiftManager::setDecorationsReceivedFlag(uint8_t flagIndex)
{
    const uint8_t byteIndex = flagIndex / 8;
    const uint8_t flag = 1 << (flagIndex % 8);
    uint8_t byteVal;

    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, MYSTERYGIFT_OFFSET_DECORATIONS_RECEIVED + byteIndex))
    {
        // can't seek or unsupported by game localization
        return;
    }

    if(!saveManager_.readByte(byteVal))
    {
        // Can't read
        return;
    }

    byteVal |= flag;
    saveManager_.rewind();
    saveManager_.writeByte(byteVal);
}

uint16_t Gen2MysteryGiftManager::getMysteryGiftTimerValue() const
{
    uint16_t value;
    uint8_t byteVal;

    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, MYSTERYGIFT_OFFSET_TIMER))
    {
        // can't seek or unsupported by game localization
        return 0xFFFF;
    }

    if(!saveManager_.readByte(byteVal))
    {
        // can't read
        return 0xFFFF;
    }
    value = ((uint16_t)byteVal) << 8;
    if(!saveManager_.readByte(byteVal))
    {
        // can't read
        return 0xFFFF;
    }
    value |= byteVal;

    return value;
}

void Gen2MysteryGiftManager::setMysteryGiftTimerValue(uint16_t timerValue)
{
    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, MYSTERYGIFT_OFFSET_TIMER))
    {
        // can't seek or unsupported by game localization
        return;
    }

    saveManager_.writeByte((timerValue >> 8));
    saveManager_.writeByte((timerValue & 0xFF));
}

uint8_t Gen2MysteryGiftManager::getTrainerHouseFlag() const
{
    uint8_t value;

    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, MYSTERYGIFT_OFFSET_TRAINER_HOUSE_FLAG))
    {
        // can't seek or unsupported by game localization
        return false;
    }

    if(!saveManager_.readByte(value))
    {
        // can't read
        return 0;
    }
    return value;
}

void Gen2MysteryGiftManager::setTrainerHouseFlag(uint8_t value)
{
    if(!seekToMysteryGiftOffset(saveManager_, gameType_, localization_, MYSTERYGIFT_OFFSET_TRAINER_HOUSE_FLAG))
    {
        // can't seek or unsupported by game localization
        return;
    }

    saveManager_.writeByte(value);
}