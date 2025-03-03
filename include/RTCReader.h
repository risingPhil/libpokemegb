#ifndef _RTCREADER_H
#define _RTCREADER_H

#include <cstdint>
#include <ctime>

/**
 * This interface abstracts a concrete implementation to access the RTC register data.
 *
 * I abstracted it with an interface so I can have 2 implementations:
 * - .rtc file based
 * - actual RTC reading with the N64 transfer pak in PokeMe64
 *
 * based on https://gbdev.io/pandocs/MBC3.html
 */
class IRTCReader
{
public:
    virtual ~IRTCReader();

    /**
     * This function will grab the current values of the RTC chip and store them internally
     * in the IRTCReader implementation.
     *
     * Use the getXYZ() functions to actually retrieve this data
     */
    virtual void latch() = 0;

    /**
     * Retrieves the current "hours" value.
     * Note: to do the initial read of/refresh the RTC data, you must first call latch()
     */
    virtual uint8_t getHours() const = 0;

    /**
     * Retrieves the current "minutes" value.
     * Note: to do the initial read of/refresh the RTC data, you must first call latch()
     */
    virtual uint8_t getMinutes() const = 0;

    /**
     * Retrieves the current "seconds" value.
     * Note: to do the initial read of/refresh the RTC data, you must first call latch()
     */
    virtual uint8_t getSeconds() const = 0;

    /**
     * Retrieves the current value of the "days" counter, as it is tracked in the RTC chip.
     * This is the raw value, excluding the halt and day counter carry bit
     */
    virtual uint16_t getDaysCounter() const = 0;

    /**
     * This returns the current value of the halt bit.
     * Note: to do the initial read of/refresh the RTC data, you must first call latch()
     */
    virtual bool isRTCHalted() const = 0;

    /**
     * This returns whether the days counter carry bit is set.
     * Note: to do the initial read of/refresh the RTC data, you must first call latch()
     */
    virtual bool isDaysCounterCarryBitSet() const = 0;
protected:
private:
};

class BaseRTCReader : public IRTCReader
{
public:
    BaseRTCReader();
    virtual ~BaseRTCReader();

    /**
     * Retrieves the current "hours" value.
     * Note: to do the initial read of/refresh the RTC data, you must first call latch()
     */
    uint8_t getHours() const override;

    /**
     * Retrieves the current "minutes" value.
     * Note: to do the initial read of/refresh the RTC data, you must first call latch()
     */
    uint8_t getMinutes() const override;

    /**
     * Retrieves the current "seconds" value.
     * Note: to do the initial read of/refresh the RTC data, you must first call latch()
     */
    uint8_t getSeconds() const override;

    /**
     * Retrieves the current value of the "days" counter, as it is tracked in the RTC chip.
     * This is the raw value, excluding the halt and day counter carry bit
     */
    uint16_t getDaysCounter() const override;

    /**
     * This returns the current value of the halt bit.
     * Note: to do the initial read of/refresh the RTC data, you must first call latch()
     */
    bool isRTCHalted() const override;

    /**
     * This returns whether the days counter carry bit is set.
     * Note: to do the initial read of/refresh the RTC data, you must first call latch()
     */
    bool isDaysCounterCarryBitSet() const override;
protected:
    uint8_t seconds_;
    uint8_t minutes_;
    uint8_t hours_;
    uint16_t daysRaw_;
private:
};

/**
 * This class reads RTC data from a buffer.
 * The data in this buffer is expected to be in the visualboyadvance-m .rtc file format.
 * Therefore it must be at least 9 bytes long
 * (seconds, minutes, hours, 16 bit day field and 32 bit unix timestamp)
 */
class BufferBasedRTCReader : public BaseRTCReader
{
public:
    BufferBasedRTCReader(uint8_t *buffer, uint8_t bufferSize, time_t currentTime = 0);
    virtual ~BufferBasedRTCReader();

    /**
     * This implementation of the latch function will
     * read a .rtc file
     */
    void latch() override;
protected:
private:
    uint8_t *buffer_;
    uint8_t bufferSize_;
    time_t currentTime_;
};

/**
 * This is a helper function to fix up the rtc fields after applying an offset to it.
 * (for example: if hours > 23, increase the day counter)
 *
 * This can be useful in various contexts
 */
void fixRTCCounters(uint8_t& seconds, uint8_t& minutes, uint8_t& hours, uint16_t& daysRaw);

#endif