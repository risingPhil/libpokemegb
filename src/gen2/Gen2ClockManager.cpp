#include "gen2/Gen2ClockManager.h"
#include "RTCReader.h"
#include "SaveManager.h"

#define RTCFIELDOFFSET_DAY 0
#define RTCFIELDOFFSET_HOUR 1
#define RTCFIELDOFFSET_MINUTE 2
#define RTCFIELDOFFSET_SECOND 3

static uint8_t readRTCOffsetField(ISaveManager& saveManager, Gen2GameType gameType, Gen2LocalizationLanguage localization, uint8_t fieldOffset)
{
    const uint32_t sramOffset = gen2_getSRAMOffsets(gameType, localization).rtcDayOffset + fieldOffset;
    uint8_t result = 0;

    saveManager.seek(sramOffset);
    saveManager.readByte(result);
    return result;
}

Gen2ClockManager::Gen2ClockManager(ISaveManager& saveManager, IRTCReader& rtcReader, Gen2GameType gameType, Gen2LocalizationLanguage localization)
    : saveManager_(saveManager)
    , rtcReader_(rtcReader)
    , gameType_(gameType)
    , localization_(localization)
    , dayCounter_(0)
    , hours_(0)
    , minutes_(0)
    , seconds_(0)
{
}

Gen2ClockManager::~Gen2ClockManager()
{
}

uint8_t Gen2ClockManager::getRTCDayOffset()
{
    return readRTCOffsetField(saveManager_, gameType_, localization_, RTCFIELDOFFSET_DAY);
}

uint8_t Gen2ClockManager::getRTCHourOffset()
{
    return readRTCOffsetField(saveManager_, gameType_, localization_, RTCFIELDOFFSET_HOUR);
}

uint8_t Gen2ClockManager::getRTCMinuteOffset()
{
    return readRTCOffsetField(saveManager_, gameType_, localization_, RTCFIELDOFFSET_MINUTE);
}

uint8_t Gen2ClockManager::getRTCSecondOffset()
{
    return readRTCOffsetField(saveManager_, gameType_, localization_, RTCFIELDOFFSET_SECOND);
}

void Gen2ClockManager::latchRTC()
{
    rtcReader_.latch();

    dayCounter_ = rtcReader_.getDaysCounter() + getRTCDayOffset();
    hours_ = rtcReader_.getHours() + getRTCHourOffset();
    minutes_ = rtcReader_.getMinutes() + getRTCMinuteOffset();
    seconds_ = rtcReader_.getSeconds() + getRTCSecondOffset();

    fixRTCCounters(seconds_, minutes_, hours_, dayCounter_);
    // we don't need/want the halt and carry bit flags here, so strip them
    dayCounter_ &= 0x3FFF;
}

DayOfWeek Gen2ClockManager::getDay()
{
    return static_cast<DayOfWeek>(dayCounter_ % 7);
}

uint16_t Gen2ClockManager::getDayCounter()
{
    return dayCounter_;
}

uint8_t Gen2ClockManager::getHours()
{
    return hours_;
}

uint8_t Gen2ClockManager::getMinutes()
{
    return minutes_;
}

uint8_t Gen2ClockManager::getSeconds()
{
    return seconds_;
}

void Gen2ClockManager::resetRTC()
{
    // The game checks bit 7 on the sRTCStatusFlags field in SRAM
    // this is set when the game detects wrong RTC register values.
    // In order to let the game prompt to reconfigure the RTC clock, we just have to set this bit
    // Based on sRTCStatusFlags, RecordRTCStatus, .set_bit_7 in
    // https://github.com/pret/pokecrystal
    // https://github.com/pret/pokegold
    const uint32_t saveOffset = gen2_getSRAMOffsets(gameType_, localization_).rtcFlags;
    const uint8_t rtcStatusFieldValue = 0xC0;

    saveManager_.seekToBankOffset(0, saveOffset);
    saveManager_.writeByte(rtcStatusFieldValue);
}

const char* toString(DayOfWeek weekDay)
{
    switch(weekDay)
    {
    case DayOfWeek::SUNDAY:
        return "Sunday";
    case DayOfWeek::MONDAY:
        return "Monday";
    case DayOfWeek::TUESDAY:
        return "Tuesday";
    case DayOfWeek::WEDNESDAY:
        return "Wednesday";
    case DayOfWeek::THURSDAY:
        return "Thursday";
    case DayOfWeek::FRIDAY:
        return "Friday";
    case DayOfWeek::SATURDAY:
        return "Saturday";
    default:
        return "Invalid";
    }
}