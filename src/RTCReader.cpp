#include "RTCReader.h"

#include <cstring>

IRTCReader::~IRTCReader()
{
}

BaseRTCReader::BaseRTCReader()
    : seconds_(0)
    , minutes_(0)
    , hours_(0)
    , daysRaw_(0)
{
}

BaseRTCReader::~BaseRTCReader()
{
}

uint8_t BaseRTCReader::getHours() const
{
    return hours_;
}

uint8_t BaseRTCReader::getMinutes() const
{
    return minutes_;
}

uint8_t BaseRTCReader::getSeconds() const
{
    return seconds_;
}

uint16_t BaseRTCReader::getDaysCounter() const
{
    return (daysRaw_ & 0x1FF);
}

bool BaseRTCReader::isRTCHalted() const
{
    return (daysRaw_ & 0x4000);
}

bool BaseRTCReader::isDaysCounterCarryBitSet() const
{
    return (daysRaw_ & 0x8000);
}

BufferBasedRTCReader::BufferBasedRTCReader(uint8_t *buffer, uint8_t bufferSize, time_t currentTime)
    : buffer_(buffer)
    , bufferSize_(bufferSize)
    , currentTime_(currentTime)
{
}

BufferBasedRTCReader::~BufferBasedRTCReader()
{
}

void BufferBasedRTCReader::latch()
{
    time_t previousTime;
    time_t timeDiff;
    time_t temp;
    uint16_t newDaysCounter;

    if(currentTime_)
    {
        // If a timestamp is provided, the bufferSize should be at least 9 bytes long!
        if(bufferSize_ < 9)
        {
            return;
        }
    }
    else
    {
        // If no timestamp is provided, the bufferSize should be at least 5 bytes
        if(bufferSize_ < 5)
        {
            return;
        }
    }

    seconds_ = buffer_[0];
    minutes_ = buffer_[1];
    hours_ = buffer_[2];
    daysRaw_ = (((uint16_t)buffer_[4]) << 8) | ((uint16_t)buffer_[3]);

    if(!currentTime_)
    {
        // if no currentTime was specified as an optional constructor parameter
        // then we won't apply any correction based on the time passed since the .rtc file was saved.
        return;
    }

    // a current time was specified, that means we need to read a timestamp and apply the difference
    previousTime = (time_t)buffer_[5];
    previousTime |= ((time_t)buffer_[6]) << 8;
    previousTime |= ((time_t)buffer_[7]) << 16;
    previousTime |= ((time_t)buffer_[8]) << 24;

    timeDiff = currentTime_ - previousTime;

    // first apply the days difference
    temp = timeDiff / (24 * 3600);
    newDaysCounter = getDaysCounter() + temp;
    // this is a little bit more involved because of the carry and halt flags, which we want to preserve
    // we need to be careful not overwriting these flags though, if our modification ends up overflowing
    // the counters intended bits
    if(newDaysCounter & 0xC000)
    {
        // overflow into the carry and halt flags.
        // so, reset counter to zero and set both flags
        daysRaw_ = 0xC000;
    }
    else
    {
        // at this point there could be overflow, but not into the carry and halt flags
        // the fixRTCCounters() function call below will clean up any such overflow.
        daysRaw_ = (daysRaw_ & 0xC000) | newDaysCounter;
    }
    // subtract the day difference
    timeDiff -= (temp * 24 * 3600);

    // now apply the hour difference
    temp = timeDiff / 3600;
    hours_ += temp;
    timeDiff -= (temp * 3600);

    // now apply the minutes difference
    temp = timeDiff / 60;
    minutes_ += temp;
    timeDiff -= temp * 60;

    // now apply the seconds difference
    seconds_ += temp;

    fixRTCCounters(seconds_, minutes_, hours_, daysRaw_);
}

void fixRTCCounters(uint8_t& seconds, uint8_t& minutes, uint8_t& hours, uint16_t& daysRaw)
{
    uint16_t newDaysCounter = daysRaw & 0x3FFF; // all bits, except for the carry and halt flags
    bool daysOverflow;
    // start fixing up the new values. (some of them might have gone over their ranges)
    if(seconds > 59)
    {
        ++minutes;
        seconds %= 60;
    }

    if(minutes > 59)
    {
        ++hours;
        minutes %= 60;
    }

    if(hours > 23)
    {
        ++newDaysCounter;
        hours %= 24;
    }

    // check if the days counter overflowed by checking bits that should never be set in any condition
    daysOverflow = (newDaysCounter & 0x3E00);

    // now apply the new days counter value
    // we AND with 0xC0 to preserve the upper 2 flag bits
    if(daysOverflow)
    {
        daysRaw = 0xC000; // this sets both the carry and halt flags and resets the counter to zero.
    }
    else
    {
        // no overflow, so we can apply the actual value
        daysRaw = (daysRaw & 0xC000) | (newDaysCounter & 0x1FF);
    }
}