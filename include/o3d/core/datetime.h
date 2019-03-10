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
            UInt16 _year,
            Month _month,
            Day _day,
            UInt8 _dayofWeek,
            UInt8 _hour,
            UInt8 _minutes,
            UInt8 _seconds,
            UInt32 _microsecond);

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

    //! Set the date of the object from a string and a specified format.
    //! @deprecated
    // Bool setFromString(const String & _value, const String & _arg = String("%d, %m %Y  %H:%M"));

    //! Build the date of the object from a string and a specified format.
    //! @note More advanced format possible than with setFromString. It can use day and month from letters.
    void buildFromString(const String & _value, const String &_arg = String("%Y-%m-%d %H:%M:%S"));

    //! Set with the current date and time.
    void setCurrent();

    //! Set the time from a time_t struct.
    void setTime(time_t ltime) { operator =(DateTime(ltime)); }

    //! Set the time from a timestamp in milliseconds since epoch.
    void setMsTime(Int64 ms);

    //! Convert the time to a time_t struct.
    time_t toTime_t() const;

    //! Check if the date is older than n days.
    Bool isOlderThan(const DateTime &compare, UInt32 days);

    Bool writeToFile(OutStream &os) const;
    Bool readFromFile(InStream &is);

public: // public members

    Month month;         //!< month 0..11
    Day day;             //!< day of the week 0..6
    UInt8 mday;          //!< day of the month 1..31

    UInt16 year;         //!< full year (not since 1900 like ctime)

    UInt8 hour;          //!< hours 0..23
    UInt8 minute;        //!< minutes 0..59
    UInt8 second;        //!< second 0..59
    UInt32 microsecond;  //!< microsecond

private:

    static DateTime *sm_null;
    static DateTime *sm_startDate;

    static void init();
    static void quit();
};

} // namespace o3d

#endif // _O3D_DATETIME_H
