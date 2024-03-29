/**
 * @file date.h
 * @brief Date object
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_DATE_H
#define _O3D_DATE_H

#include "string.h"

namespace o3d {

/**
 * @brief The Month enum ISO8601. Index start to 1.
 */
enum Month
{
    MONTH_JANUARY = 1,
    MONTH_FEBRUARY,
    MONTH_MARCH,
    MONTH_APRIL,
    MONTH_MAY,
    MONTH_JUNE,
    MONTH_JULY,
    MONTH_AUGUST,
    MONTH_SEPTEMBER,
    MONTH_OCTOBER,
    MONTH_NOVEMBER,
    MONTH_DECEMBER,
    MONTH_UNDECEMBER,
    MONTH_DUODECEMBER
};

/**
 * @brief The Day enum ISO8601 : Monday is first day, and index start to 1.
 */
enum Day
{
    DAY_MONDAY = 1,
    DAY_TUESDAY,
    DAY_WEDNESDAY,
    DAY_THURSDAY,
    DAY_FRIDAY,
    DAY_SATURDAY,
    DAY_SUNDAY
};

/**
 * @brief The Date class
 * Process date.
 * @todo complete tz support.
 */
class O3D_API Date
{
    friend class Application;

public:

    //! Return a null date.
    static const Date& nullDate();

    //! Return the date of the start of the program.
    static const Date& startDate();

    //! default ctor
    Date(Bool setToday = False);

    //! Initialize from time_t
    Date(time_t ltime);

    //! Initialize manually.
    Date(Int16 _year, Int8 _month, Int8 _day, Int8 _dayofWeek, Int8 _tz=0);

    //! Copy ctor
    Date(const Date & _which);

    //! Destructor
    ~Date() {}

    //! Clear the date object.
    void destroy();

    //! Copy.
    Date& operator = (const Date & _which);

    //! Compare to another date object for equality.
    Bool operator == (const Date & _which) const;

    //! Compare to another date object for inequality.
    Bool operator != (const Date & _which) const;

    //! Compare to another date object for lesser.
    Bool operator < (const Date & _which) const;

    //! Compare to another date object for lesser or equal.
    Bool operator <= (const Date & _which) const;

    //! Compare to another date object for greater.
    Bool operator > (const Date & _which) const;

    //! Compare to another date object for greater or equal.
    Bool operator >= (const Date & _which) const;

    //! Build a string containing the date with a specified format and return it.
    String buildString(const String & _arg = String("%d, %m %Y")) const;

    //! Build the date of the object from a string and a specified format.
    //! Allowed format extra characters list: -.:,;/^|TZ
    //! @return False if format or value error.
    Bool buildFromString(const String & _value, const String &_arg = String("%Y-%m-%d"));

    //! Set with the current date and time.
    void setCurrent(Bool UTC=False);

    //! Set the time from a time_t struct.
    void setTime(time_t ltime) { operator =(Date(ltime)); }

    //! Set the time from a time in second with a microsecond precision.
    void fromTime(o3d::Float time, Bool UTC=True);

    //! Set the time from a time in second with a microsecond precision.
    void fromTime(o3d::Double time, Bool UTC=True);

    //! Set the time from a time in milliseconds since epoch.
    void fromTimeMs(Int64 ms, Bool UTC=True);

    //! Set the time from a time in microsecond since epoch.
    void fromTimeUs(Int64 us, Bool UTC=True);

    //! Convert the time to a time_t struct.
    time_t toTime_t(Bool UTC=True) const;

    //! Convert to an integer epoch timestamp in ms.
    Int32 toTimestamp(Bool UTC=True) const;

    //! Convert to a float epoch timestamp in second unit with microsecond precision.
    Float toFloatTimestamp(Bool UTC=True) const;

    //! Convert to a double epoch timestamp in second unit with microsecond precision.
    Double toDoubleTimestamp(Bool UTC=True) const;

    //! Check if the date is older than XX days.
    Bool isOlderThan(const Date &compare, Int32 days);

    //! Get the day of week for the current datetime. It is 0 based on sunday.
    Int8 getDayOfWeek() const;

    Bool writeToFile(OutStream &os) const;
    Bool readFromFile(InStream &is);

public: // public members

    Int8 month;      //!< month 1..12
    Int8 wday;       //!< day of the week 1..7
    Int8 mday;       //!< day of the month 1..31

    Int16 year;      //!< full year (not since 1900 like ctime)

    Int8 tz;         //!< 0 mean UTC

private:

    static Date *sm_null;
    static Date *sm_startDate;
    static Int32 sm_localTz;
    static Int32 sm_localDst;

    static void init();
    static void quit();
};

} // namespace o3d

#endif // _O3D_DATE_H
