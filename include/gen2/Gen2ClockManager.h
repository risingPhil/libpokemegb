#ifndef _GEN2CLOCKMANAGER_H
#define _GEN2CLOCKMANAGER_H

#include "gen2/Gen2Common.h"

class ISaveManager;
class IRTCReader;

// see https://github.com/pret/pokegold/blob/master/constants/ram_constants.asm
enum class DayOfWeek
{
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
};

/**
 * This class helps manage the RTC clock in gen II games.
 */
class Gen2ClockManager
{
public:
    Gen2ClockManager(ISaveManager& saveManager, IRTCReader& rtcReader, Gen2GameType gameType, Gen2LocalizationLanguage localization);
    ~Gen2ClockManager();

    /**
     * This function reads the rtc day offset, set by the user on a new game based on the current rtc value from SaveManager.
     * This is needed to interpret the RTC values correctly
     */
    uint8_t getRTCDayOffset();

    /**
     * This function reads the rtc hour offset, set by the user on a new game based on the current rtc value from SaveManager.
     * This is needed to interpret the RTC values correctly
     */
    uint8_t getRTCHourOffset();

    /**
     * This function reads the rtc minute offset, set by the user on a new game based on the current rtc value from SaveManager.
     * This is needed to interpret the RTC values correctly
     */
    uint8_t getRTCMinuteOffset();

    /**
     * This function reads rtc second offset, set by the user on a new game based on the current rtc value from SaveManager.
     * This is needed to interpret the RTC values correctly
     */
    uint8_t getRTCSecondOffset();

    /**
     * This function reads all the RTC fields at once from the RTCReader and apply the rtc offsets from the save file internally.
     * You need to call this function before calling any of the functions to retrieve the current day/time values.
     *
     * You also need to call this to refresh the data from the RTC chip.
     */
    void latchRTC();

    /**
     * returns the day of the week. (rtcoffset from save applied)
     *
     * Make sure to call latchRTC() to initialize/refresh the day/time values before calling this function!
     */
    DayOfWeek getDay();

    /**
     * Returns the actual day counter (rtcoffset from save applied)
     */
    uint16_t getDayCounter();

    /**
     * Returns the current hour (rtcoffset from save applied)
     *
     * Make sure to call latchRTC() to initialize/refresh the day/time values before calling this function!
     */
    uint8_t getHours();

    /**
     * Returns the current minute (rtcoffset from save applied)
     *
     * Make sure to call latchRTC() to initialize/refresh the day/time values before calling this function!
     */
    uint8_t getMinutes();

    /**
     * Returns the current second (rtcoffset from save applied)
     *
     * Make sure to call latchRTC() to initialize/refresh the day/time values before calling this function!
     */
    uint8_t getSeconds();

    /**
     * @brief This function will change an SRAM field to let gen 2 games prompt you
     * to reconfigure the game clock
     */
    void resetRTC();
protected:
private:
    ISaveManager& saveManager_;
    IRTCReader& rtcReader_;
    Gen2GameType gameType_;
    Gen2LocalizationLanguage localization_;
    uint16_t dayCounter_;
    uint8_t hours_;
    uint8_t minutes_;
    uint8_t seconds_;
};

/**
 * Converts the given WeekDay to string
 */
const char* toString(DayOfWeek weekDay);

#endif