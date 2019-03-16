/**
 * @file date.cpp
 * @brief Implementation of date.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-03-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/date.h"

#include "o3d/core/error.h"
#include "o3d/core/stringtokenizer.h"

#include "o3d/core/instream.h"
#include "o3d/core/outstream.h"

#ifdef O3D_WINDOWS
#include <time.h>
#else
#include <sys/time.h>
#endif

#include <sstream>
using namespace o3d;

static const WChar* monthString[] = {
    L"January",
    L"February",
    L"March",
    L"April",
    L"May",
    L"June",
    L"July",
    L"August",
    L"September",
    L"October",
    L"November",
    L"December" ,
    L"Undecember",
    L"Duodecember"
};

static const WChar* shortMonthString[] = {
    L"Jan",
    L"Feb",
    L"Mar",
    L"Apr",
    L"May",
    L"Jun",
    L"Jul",
    L"Aug",
    L"Sep",
    L"Oct",
    L"Nov",
    L"Dec",
    L"Und",
    L"Dud"
};

static const WChar* dayString[] = {
    L"Monday",
    L"Tuesday",
    L"Wednesday",
    L"Thursday",
    L"Friday",
    L"Saturday",
    L"Sunday"
};

static const WChar* shortDayString[] = {
    L"Mon",
    L"Tue",
    L"Wed",
    L"Thu",
    L"Fri",
    L"Sat",
    L"Sun"
};

Date* Date::sm_null = nullptr;
Date* Date::sm_startDate = nullptr;
Int32 Date::sm_localTz = 0;

void Date::init()
{
    if (!sm_null) {
        sm_null = new Date(False);
        sm_startDate = new Date(True);

#ifdef O3D_WINDOWS
        _tzset();
#else
        tzset();
#endif
        time_t gmt, rawtime = time(NULL);
        struct tm *ptm;

#ifdef O3D_WINDOWS
        struct tm gbuf;
        ptm = gmtime_r(&rawtime, &gbuf);
#else
        ptm = gmtime(&rawtime);
#endif
        // Request that mktime() looksup dst in timezone database
        ptm->tm_isdst = -1;
        gmt = mktime(ptm);

        sm_localTz = static_cast<Int32>(difftime(rawtime, gmt));
    }
}

void Date::quit()
{
    o3d::deletePtr(sm_null);
    o3d::deletePtr(sm_startDate);
}

// Return a null date.
const Date& Date::nullDate()
{
    return *sm_null;
}

// Return the date of the start of the program.
const Date& Date::startDate()
{
    return *sm_startDate;
}

Date::Date(Bool setToday) :
    month(1),
    wday(1),
    mday(1),
    year(0)
{
    if (setToday) {
        setCurrent();
    }
}

// Initialize manually.
Date::Date(UInt16 _year, UInt8 _month, UInt8 _day, UInt8 _dayofWeek) :
    month(_month),
    wday(_day),
    mday(_dayofWeek),
    year(_year)
{

}

Date::Date(const Date &_which) :
    month(_which.month),
    wday(_which.wday),
    mday(_which.mday),
    year(_which.year)
{
}

void Date::destroy()
{
    year = 0;
    month = 1;
    wday = 1;
    mday = 1;
}

Date & Date::operator = (const Date &_which)
{
    year = _which.year;
    month = _which.month;
    wday = _which.wday;
    mday = _which.mday;

    return *this;
}

Date::Date(time_t ltime)
{
#ifdef _MSC_VER  // O3D_WINDOWS
    struct tm local;
    _localtime64_s(&local,&ltime);

    year = UInt16(local.tm_year + 1900);
    month = UInt8(local.tm_mon) + 1;
    wday = UInt8(local.tm_wday) + 1;
    mday = UInt8(local.tm_mday);
#else
    struct tm *local;
    local = localtime(&ltime);

    year = local->tm_year + 1900;
    month = UInt8(local->tm_mon) + 1;
    wday = UInt8(local->tm_wday) + 1;
    mday = local->tm_mday;
#endif
}

Bool Date::operator == (const Date & _which) const
{
    return ((year == _which.year) &&
            (month == _which.month) &&
            (mday == _which.mday) &&
            (mday == _which.mday));
}

Bool Date::operator != (const Date & _which) const
{
    return !((*this) == _which);
}

Bool Date::operator <(const Date &_which) const
{
    if (year > _which.year) {
        return False;
    } else if (year < _which.year) {
        return True;
    }

    if (month > _which.month) {
        return False;
    } else if (month < _which.month) {
        return True;
    }

    if (mday > _which.mday) {
        return False;
    } else if (mday < _which.mday) {
        return True;
    }

    // or ms equal
    return False;
}

Bool Date::operator <=(const Date &_which) const
{
    if (year > _which.year) {
        return False;
    } else if (year < _which.year) {
        return True;
    }

    if (month > _which.month) {
        return False;
    } else if (month < _which.month) {
        return True;
    }

    if (mday > _which.mday) {
        return False;
    } else if (mday < _which.mday) {
        return True;
    }

    // or ms equal
    return True;
}

Bool Date::operator >(const Date &_which) const
{
    if (year < _which.year) {
        return False;
    } else if (year > _which.year) {
        return True;
    }

    if (month < _which.month) {
        return False;
    } else if (month > _which.month) {
        return True;
    }

    if (mday < _which.mday) {
        return False;
    } else if (mday > _which.mday) {
        return True;
    }

    // or ms equal
    return False;
}

Bool Date::operator >=(const Date &_which) const
{
    if (year < _which.year) {
        return False;
    } else if (year > _which.year) {
        return True;
    }

    if (month < _which.month) {
        return False;
    } else if (month > _which.month) {
        return True;
    }

    if (mday < _which.mday) {
        return False;
    } else if (mday > _which.mday) {
        return True;
    }

    // or ms equal
    return True;
}

Bool Date::writeToFile(OutStream &os) const
{
    // 5 bytes date
    os  << year
        << month
        << wday
        << mday;

    return True;
}

Bool Date::readFromFile(InStream &is)
{
    // 5 bytes date
    is  >> year
        >> month
        >> wday
        >> mday;

    return True;
}

String Date::buildString(const String & _arg) const
{
    // @see strftime http://www.cplusplus.com/reference/ctime/strftime/
    String result;

    UInt32 k = 0;

    while (k < _arg.length()) {
        if (_arg.getData()[k] == '%') {
            if ((++k) == _arg.length()) {
                break;
            }

            switch (_arg.getData()[k]) {
                case 'Y':
                    // 4 digits year
                    result << year;
                    break;

                case 'y':
                {
                    // 2 digits year
                    String y; y.concat(year);
                    result << y.slice(y.length()-2, -1);
                }
                    break;

                case 'B':
                    // letter month
                    result << String(monthString[month-1]);
                    break;

                case 'b':
                    // letter short month
                    result << String(shortMonthString[month-1]);
                    break;

                case 'm':
                    // 2 digits month
                    if (month < 10) {
                        result << '0' << month;  // leading 0
                    } else {
                        result << month;
                    }
                    break;

                case 'A':
                    // letter day
                    result << String(dayString[wday-1]);
                    break;

                case 'a':
                    // letter short day
                    result << String(shortDayString[wday-1]);
                    break;

                case 'd':
                    // 2 digits month day
                    if (mday < 10) {
                        result << '0' << mday;  // leading 0
                    } else {
                        result << mday;
                    }
                    break;
            }

            ++k;
        } else {
            result += _arg.getData()[k];
            ++k;
        }
    }

    return result;
}

Bool Date::buildFromString(const String &_value, const String &_arg)
{
    StringTokenizer arg(_arg, L"%");
    StringTokenizer value(_value, L" -.:,;/^|TZ");

    if (arg.countTokens() != value.countTokens()) {
        return False;
    }

    while (arg.hasMoreElements() && value.hasMoreElements()) {
        // @todo check value and return False if error
        String argu(arg.nextElement());
        String vals(value.nextElement());

        Bool found = False;

        switch(argu.getData()[0]) {
            case 'Y':
                // 4 digits year
                year = UInt16(vals.toUInt32());
                break;

            case 'y':
                // 2 digits year
                year = UInt16(vals.toUInt32()) + 2000;
                break;

            case 'B':
                // letter month
                for (UInt8 i = 0; i < 14; ++i) {
                    if (vals == monthString[i]) {
                        month = i+1;
                        found = True;
                        break;
                    }
                }
                if (!found) {
                    return False;
                }
                break;

            case 'b':
                // letter short month
                for (UInt8 i = 0; i < 14; ++i) {
                    if (vals == shortMonthString[i]) {
                        month = i+1;
                        found = True;
                        break;
                    }
                }
                if (!found) {
                    return False;
                }
                break;

            case 'm':
                // 2 digits month
                if (vals[0] == '0') {
                    // ignore leading 0
                    month = vals.toUInt32(1);
                } else {
                    month = vals.toUInt32();
                }
                if (month > 14) {
                    return False;
                }
                break;

            case 'A':
                // letter day
                for (UInt8 i = 0; i < 6; ++i) {
                    if (vals == dayString[i]) {
                        wday = i+1;
                        found = True;
                        break;
                    }
                }
                if (!found) {
                    return False;
                }
                // mday = ... need week of year/month
                break;

            case 'a':
                // letter short day
                for (UInt8 i = 0; i < 6; ++i) {
                    if (vals == shortDayString[i]) {
                        wday = i+1;
                        found = True;
                        break;
                    }
                }
                if (!found) {
                    return False;
                }
                // mday = ... need week of year/month
                break;

            case 'd':
                // 2 digits month day
                if (vals[0] == '0') {
                    // ignore leading 0
                    mday = UInt8(vals.toUInt32(1));
                    wday = getDayOfWeek();
                } else {
                    mday = UInt8(vals.toUInt32());
                    wday = getDayOfWeek();
                }
                if (mday > 31) {
                    // @todo but might check 29 feb, and 30 day month...
                    return False;
                }
                break;
        }
    }

    return True;
}

void Date::setCurrent()
{
#ifdef O3D_WINDOWS
    __time64_t ltime;
   _time64(&ltime);

    tm local;
    _localtime64_s(&local,&ltime);

    year = UInt16(local.tm_year + 1900);
    month = UInt8(local.tm_mon) + 1;
    day = UInt8(local.tm_wday) + 1;
    mday = UInt8(local.tm_mday);
#else
    struct timeval ltime;
    struct tm *local;

    gettimeofday(&ltime, nullptr);
    local = localtime(&ltime.tv_sec);
    year = local->tm_year + 1900;
    month = UInt8(local->tm_mon) + 1;
    wday = UInt8(local->tm_wday) + 1;
    mday = local->tm_mday;
#endif
}

void Date::fromTime(Float time, Bool UTC)
{
    fromTimeUs(Int64(time * 1000000), UTC);
}

void Date::fromTime(Double time, Bool UTC)
{
    fromTimeUs(Int64(time * 1000000), UTC);
}

void Date::fromTimeMs(Int64 ms, Bool UTC)
{
#ifdef O3D_WINDOWS
    __time64_t ltime = static_cast<time_t>(ms / 1000) - (UTC ? sm_localTz : 0);

    tm local;
    _localtime64_s(&local,&ltime);

    year = UInt16(local.tm_year + 1900);
    month = UInt8(local.tm_mon) + 1;
    day = UInt8(local.tm_wday) + 1;
    mday = UInt8(local.tm_mday);
#else
    time_t ts = static_cast<time_t>(ms / 1000) - (UTC ? sm_localTz : 0);
    struct tm *local;

    local = localtime(&ts);
    year = local->tm_year + 1900;
    month = UInt8(local->tm_mon) + 1;
    wday = UInt8(local->tm_wday) + 1;
    mday = local->tm_mday;
#endif
}

void Date::fromTimeUs(Int64 us, Bool UTC)
{
#ifdef O3D_WINDOWS
    __time64_t ltime = static_cast<time_t>(us / 1000000) - (UTC ? sm_localTz : 0);

    tm local;
    _localtime64_s(&local,&ltime);

    year = UInt16(local.tm_year + 1900);
    month = UInt8(local.tm_mon) + 1;
    day = UInt8(local.tm_wday) + 1;
    mday = UInt8(local.tm_mday);
#else
    time_t ts = static_cast<time_t>(us / 1000000) - (UTC ? sm_localTz : 0);
    struct tm *local;

    local = localtime(&ts);
    year = local->tm_year + 1900;
    month = UInt8(local->tm_mon) + 1;
    wday = UInt8(local->tm_wday) + 1;
    mday = local->tm_mday;
#endif
}

time_t Date::toTime_t(Bool UTC) const
{
#ifdef O3D_WINDOWS
    tm lObjectTime;

    lObjectTime.tm_sec = (UTC ? sm_localTz : 0);
    lObjectTime.tm_min = 0;
    lObjectTime.tm_hour = 0;
    lObjectTime.tm_mday = mday;
    lObjectTime.tm_mon = month-1;
    lObjectTime.tm_year = year-1900;
    lObjectTime.tm_wday = 0;  // wday-1;
    lObjectTime.tm_yday = 0;
    lObjectTime.tm_isdst = 0;

    return _mktime64(&lObjectTime);
#else
    struct tm lObjectTime;

    lObjectTime.tm_sec = (UTC ? sm_localTz : 0);
    lObjectTime.tm_min = 0;
    lObjectTime.tm_hour = 0;
    lObjectTime.tm_mday = mday;
    lObjectTime.tm_mon = month-1;
    lObjectTime.tm_year = year-1900;
    lObjectTime.tm_wday = 0;  // wday-1;
    lObjectTime.tm_yday = 0;
    lObjectTime.tm_isdst = 0;

    return mktime(&lObjectTime);
#endif
}

Int32 Date::toTimestamp(Bool UTC) const
{
    // time_t is in ms, adding the microsecond part but want it in millisecond
    return static_cast<Int32>(toTime_t(UTC));
}

Float Date::toFloatTimestamp(Bool UTC) const
{
    // time_t is in second unit
    return static_cast<Float>(toTime_t(UTC));
}

Double Date::toDoubleTimestamp(Bool UTC) const
{
    // time_t is in second unit
    return static_cast<Double>(toTime_t(UTC));
}

Bool Date::isOlderThan(const Date& today, UInt32 days)
{
#ifdef O3D_WINDOWS
    __time64_t start, end;
    double elapsed;

    // set start/end times structures
    tm startTM,endTM;

    // info : tm_wday and tm_yday are ignored for mktime
    startTM.tm_year = year-1900;
    startTM.tm_mon = month-1;
    startTM.tm_wday = day-1;
    startTM.tm_hour = 0;
    startTM.tm_min = 0;
    startTM.tm_sec = 0;
    startTM.tm_mday = mday;
    startTM.tm_isdst = 0;

    endTM.tm_year = today.year-1900;
    endTM.tm_mon = today.month-1;
    endTM.tm_wday = today.wday-1;
    endTM.tm_hour = 0;
    endTM.tm_min = 0;
    endTM.tm_sec = 0;
    endTM.tm_mday = today.mday;
    endTM.tm_isdst = 0;

    // make start/end times
    start = _mktime64(&startTM);
    end = _mktime64(&endTM);

    // diff the two times
    elapsed = _difftime64(end,start);
#else
    time_t start, end;
    double elapsed;

    struct tm startTM, endTM;

    // info : tm_wday and tm_yday are ignored for mktime
    startTM.tm_year = year-1900;
    startTM.tm_mon = month-1;
    startTM.tm_wday = wday-1;
    startTM.tm_hour = 0;
    startTM.tm_min = 0;
    startTM.tm_sec = 0;
    startTM.tm_mday = mday;
    startTM.tm_isdst = 0;

    endTM.tm_year = today.year-1900;
    endTM.tm_mon = today.month-1;
    endTM.tm_wday = today.wday-1;
    endTM.tm_hour = 0;
    endTM.tm_min = 0;
    endTM.tm_sec = 0;
    endTM.tm_mday = 0;
    endTM.tm_isdst = 0;

    // make start/end times
    start = mktime(&startTM);
    end = mktime(&endTM);

    // diff the two times
    elapsed = difftime(end, start);

#endif

    // compare time in seconds (transform days in seconds)
    if (elapsed > (days * 24 * 60 * 60))
        return True;

    return False;
}

UInt8 Date::getDayOfWeek() const
{
    static Int32 t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    Int32 y = year - (month < 3 ? 1 : 0);

    return ((y + y/4 - y/100 + y/400 + t[month-1] + (mday-1)) % 7) + 1;
}
