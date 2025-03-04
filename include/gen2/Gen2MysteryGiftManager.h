#ifndef _GEN2MYSTERYGIFT_H
#define _GEN2MYSTERYGIFT_H

#include <cstdint>

#define MYSTERYGIFT_MAX_PARTNERS 5
#define NUM_NON_TROPHY_DECORATIONS 42

class Gen2GameReader;
class IRTCReader;
class ISaveManager;
enum class Gen2GameType;
enum class Gen2LocalizationLanguage;

typedef struct MysteryGiftSelection
{
    uint8_t itemID;
    uint8_t decorationID;
    bool useDecoration;
} MysteryGiftSelection;

typedef enum MysteryGiftResult
{
    MYSTERYGIFT_RESULT_DECORATION,
    MYSTERYGIFT_RESULT_ITEM_ITEMPOCKET,
    MYSTERYGIFT_RESULT_ITEM_BALLPOCKET,
    MYSTERYGIFT_RESULT_ERROR_NOT_UNLOCKED,
    MYSTERYGIFT_RESULT_ERROR_TOO_MANY_GIFTS,
    MYSTERYGIFT_RESULT_ERROR_NO_ROOM_FOR_ITEM
} MysteryGiftResult;

/**
 * @brief This function rolls some dices and returns a random mystery gift.
 *
 * It is NOT affected/restricted by trainerIDs, unlike the official mystery gift implementations.
 */
MysteryGiftSelection selectRandomGift();

/**
 * @brief This class implements support for Gen2MysteryGift WITHOUT the trainerID
 * restrictions.
 *
 * Items will be sent to the players' item/ball pocket instead of to the delivery agent in the pokémon center.
 * This ensures that you don't have to collect the gift between every attempt.
 *
 * The player will be able to use Mystery Gift 5 times in total per day.
 *
 * It is mostly based on:
 * - https://bulbapedia.bulbagarden.net/wiki/Mystery_Gift
 * - https://bulbapedia.bulbagarden.net/wiki/List_of_decorations_in_Generation_II
 * - https://github.com/pret/pokegold/blob/master/engine/rtc/rtc.asm
 * - https://github.com/pret/pokegold/blob/master/home/time.asm
 * - https://github.com/pret/pokegold/blob/master/ram/wram.asm
 * - https://github.com/pret/pokegold/blob/master/ram/sram.asm
 * - https://github.com/pret/pokegold/blob/master/data/decorations/mystery_gift_decos.asm
 * - https://github.com/pret/pokegold/blob/master/engine/link/mystery_gift.asm
 * - https://github.com/pret/pokegold/blob/master/engine/link/mystery_gift_2.asm
 */
class Gen2MysteryGiftManager
{
public:
    Gen2MysteryGiftManager(ISaveManager& saveManager, Gen2GameType gameType, Gen2LocalizationLanguage language);
    ~Gen2MysteryGiftManager();

    /**
     * @brief This function returns a boolean indicating whether the mystery gift feature has been unlocked
     */
    bool isUnlocked() const;

    /**
     * @brief This function unlocks mystery gift in the game save.
     */
    void unlock();

    /**
     * @brief This function applies the selectedMysteryGift as mystery gift in the save file.
     * For items, we push them straight into the players' item/ball pocket (depending on the item)
     * This prevents the need to collect the item from the Pokémon Center's attendant for every mystery gift.
     *
     * You can insert up to 5 mystery gifts per day this way.
     *
     * WARNING: Make sure to call gameReader.finishSave() after calling this to avoid corrupting the save!
     */
    MysteryGiftResult obtain(Gen2GameReader& gameReader, IRTCReader& rtcReader, const MysteryGiftSelection& selectedMysteryGift);

    /**
     * @brief This function returns the index of a mystery gift item that is pending to be collected over mystery gift.
     */
    uint8_t getMysteryGiftItem() const;

    /**
     * @brief Sets an item index that can get collected by the player with the mystery gift
     */
    void setMysteryGiftItem(uint8_t itemIndex);

    /**
     * @brief returns the number of mystery gift partners the player had today
     */
    uint8_t getNumberOfDailyMysteryGiftPartners() const;

    /**
     * @brief Sets the Number Of Daily Mystery Gift Partners
     */
    void setNumberOfDailyMysteryGiftPartners(uint8_t numPartners);

    /**
     * This function returns an array of MYSTERYGIFT_MAX_PARTNERS elements
     * in which each element is a trainerID of a mystery gift partner.
     * or 0 if none.
     *
     * Note: an internal buffer is returned. The next call to this function
     * will overwrite its values. If you need the values to persist, make sure
     * to copy them to a separate buffer!
     */
    uint16_t* getDailyMysteryGiftPartnerIDs() const;

    /**
     * @brief This function replaces the value of the mystery gift partner id
     * at the given index.
     *
     * @param index should be < MYSTERYGIFT_MAX_PARTNERS
     * @param trainerID the trainer id of the partner we want to write
     *
     * @return returns true if completed normally or false when failed!
     */
    bool setDailyMysteryGiftPartnerAtIndex(uint8_t index, uint16_t trainerID);

    /**
     * @brief This function returns whether the decorationsReceived flag at the specified
     * flagIndex is set
     */
    bool isDecorationsReceivedFlagSet(uint8_t flagIndex) const;

    /**
     * @brief Makes use of the isDecorationsReceivedFlagSet() function to determine whether
     * the player already received the decoration with given decorationID through mystery gift.
     */
    bool isDecorationIDReceived(uint8_t decorationID) const;

    /**
     * @brief This function sets the given flag in the mysteryGiftDecorationReceived field
     */
    void setDecorationsReceivedFlag(uint8_t flagIndex);

    /**
     * @brief This function makes use of setDecorationsReceivedFlag() to flag
     * the given decorationID as received through mystery gift.
     */
    void setDecorationIDReceivedFlag(uint8_t decorationID);

    /**
     * @brief Gets the mystery gift timer value
     */
    uint16_t getMysteryGiftTimerValue() const;

    /**
     * @brief Sets a mystery gift timer value
     */
    void setMysteryGiftTimerValue(uint16_t timerValue);

    /**
     * @brief Retrieves the value of the Mystery Gift Trainer House Flag field
     */
    uint8_t getTrainerHouseFlag() const;

    /**
     * @brief Sets the value of the Mystery Gift Trainer House Flag field
     */
    void setTrainerHouseFlag(uint8_t value);
protected:
private:
    ISaveManager& saveManager_;
    Gen2GameType gameType_;
    Gen2LocalizationLanguage localization_;
    mutable uint16_t mysteryGiftPartnerIdBuffer_[MYSTERYGIFT_MAX_PARTNERS];
    mutable uint8_t mysteryGiftDecorationsReceivedBuffer_[(NUM_NON_TROPHY_DECORATIONS / 8) + 1];
};

#endif