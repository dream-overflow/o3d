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

enum Month
{
    MONTH_JANUARY = 0,
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
    MONTH_DECEMBER
};

enum Day
{
    DAY_SUNDAY = 0,
    DAY_MONDAY,
    DAY_TUESDAY,
    DAY_WEDNESDAY,
    DAY_THURSDAY,
    DAY_FRIDAY,
    DAY_SATURDAY
};

/**
 * @brief The Date class
 * Process date.
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
    Date(UInt16 _year, Month _month, Day _day, UInt8 _dayofWeek);

    //! Copy ctor
    Date(const Date & _which);

    //! Destructor
    virtual ~Date() {}

    //! Clear the date object.
    void destroy();

    //! Copy.
    Date & operator = (const Date & _which);

    //! Compare to another date object for equality.
    Bool operator == (const Date & _which) const;

    //! Compare to another date object for inequality.
    Bool operator != (const Date & _which) const;

    //! Compare to another date object for lesser.
    Bool operator < (const Date & _which) const;

    //! Build a string containing the date with a specified format and return it.
    String buildString(const String & _arg = String("%d, %m %y")) const;

    //! Set the date of the object from a string and a specified format.
    Bool setFromString(const String & _value, const String & _arg = String("%d, %m %y"));

    //! Build the date of the object from a string and a specified format.
    //! @note More sophisticated than SetFromString. It can use day and month from letters.
    void buildFromString(const String & _value, const String &_arg = String("%y-%m-%d"));

    //! Set with the current date and time.
    void setCurrent();

    //! Set the time from a time_t struct.
    void setTime(time_t ltime) { operator =(Date(ltime)); }

    //! Set the time from a time in milliseconds since epoch.
    void setMsTime(Int64 ms);

    //! Convert the time to a time_t struct.
    time_t toTime_t() const;

    //! Check if the date is older than XX days.
    Bool isOlderThan(const Date &compare, UInt32 days);

    Bool writeToFile(OutStream &os) const;
    Bool readFromFile(InStream &is);

public: // public members

    Month month;          //!< month 0..11
    Day day;              //!< day of the week 0..6
    UInt8 mday;       //!< day of the month 1..31

    UInt16 year;      //!< full year (not since 1900 like ctime)

private:

    static Date *sm_null;
    static Date *sm_startDate;

    static void init();
    static void quit();
};

} // namespace o3d

#endif // _O3D_DATE_H
