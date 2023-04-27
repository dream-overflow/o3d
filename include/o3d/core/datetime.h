/**
 * @file datetime.h
 * @brief Date and time object
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_DATETIME_H
#define _O3D_DATETIME_H

#include "string.h"
#include "date.h"

namespace o3d {

/**
 * @brief The DateTime class.
 * Date accessor and helper.
 * @todo complete tz support.
 */
class O3D_API DateTime
{
    friend class Application;

public:

    //! Return a null date.
    static const DateTime& nullDate();

    //! Return the date of the start of the program.
    static const DateTime& startDate();

    //! default ctor
    DateTime(Bool setToday = False);

    //! Initialize from time_t
    DateTime(time_t ltime);

    //! Initialize manually.
    DateTime(
            Int16 _year,
            Int8 _month,
            Int8 _day,
            Int8 _dayofWeek,
            Int8 _hour,
            Int8 _minutes,
            Int8 _seconds,
            Int32 _microsecond,
            Int8 _tz = 0);

    //! Copy ctor
    DateTime(const DateTime & _which);

    //! Destructor
    ~DateTime() {}

    //! Clear the date object.
    void destroy();

    //! Copy.
    DateTime & operator = (const DateTime & _which);

    //! Compare to another date object for equality.
    Bool operator == (const DateTime & _which) const;

    //! Compare to another date object for inequality.
    Bool operator != (const DateTime & _which) const;

    //! Compare to another date object for lesser.
    Bool operator < (const DateTime & _which) const;

    //! Compare to another date object for lesser or equal.
    Bool operator <= (const DateTime & _which) const;

    //! Compare to another date object for greater.
    Bool operator > (const DateTime & _which) const;

    //! Compare to another date object for greater or equal.
    Bool operator >= (const DateTime & _which) const;

    //! Build a string containing the date with a specified format and return it.
    String buildString(const String & _arg = String("%d, %m %Y  %H:%M")) const;

    //! Build the date of the object from a string and a specified format.
    //! Allowed format extra characters list: -.:,;/^|TZ
    //! @return False if format or value error.
    Bool buildFromString(const String & _value, const String &_arg = String("%Y-%m-%d %H:%M:%S"));

    //! Set with the current date and time.
    void setCurrent(Bool UTC=False);

    //! Set the time from a time_t struct.
    void setTime(time_t ltime) { operator =(DateTime(ltime)); }

    //! Set the time from a time in second with a microsecond precision.
    void fromTime(o3d::Float time, Bool UTC=True);

    //! Set the time from a time in second with a microsecond precision.
    void fromTime(o3d::Double time, Bool UTC=True);

    //! Set the time from a timestamp in milliseconds since epoch.
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

    //! Get the day of week for the current datetime. It is 0 based on sunday.
    Int8 getDayOfWeek() const;

    //! Check if the date is older than n days.
    Bool isOlderThan(const DateTime &compare, Int32 days);

    Bool writeToFile(OutStream &os) const;
    Bool readFromFile(InStream &is);

public: // public members

    Int8 month;         //!< month 1..12
    Int8 wday;          //!< day of the week 1..7
    Int8 mday;          //!< day of the month 1..31

    Int16 year;         //!< full year (not since 1900 like ctime)

    Int8 hour;          //!< hours 0..23
    Int8 minute;        //!< minutes 0..59
    Int8 second;        //!< second 0..59
    Int32 microsecond;  //!< microsecond

    Int8 tz;            //!< 0 mean UTC

private:

    static DateTime *sm_null;
    static DateTime *sm_startDate;
    static Int32 sm_localTz;
    static Int32 sm_localDst;

    static void init();
    static void quit();
};

} // namespace o3d

#endif // _O3D_DATETIME_H
