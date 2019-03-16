/**
 * @file datetime.cpp
 * @brief Implementation of datetime.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-03-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/datetime.h"

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

static const WChar * monthString[] = {
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
    L"December",
    L"Undecember",
    L"Duodecember"
};

static const WChar * shortMonthString[] = {
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

static const WChar * dayString[] = {
    L"Monday",
    L"Tuesday",
    L"Wednesday",
    L"Thursday",
    L"Friday",
    L"Saturday",
    L"Sunday"
};

static const WChar * shortDayString[] = {
    L"Mon",
    L"Tue",
    L"Wed",
    L"Thu",
    L"Fri",
    L"Sat",
    L"Sun"
};

DateTime* DateTime::sm_null = nullptr;
DateTime* DateTime::sm_startDate = nullptr;
Int32 DateTime::sm_localTz = 0;

void DateTime::init()
{
    if (!sm_null) {
        sm_null = new DateTime(False);
        sm_startDate = new DateTime(True);

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

void DateTime::quit()
{
    o3d::deletePtr(sm_null);
    o3d::deletePtr(sm_startDate);
}

// Return a null date.
const DateTime& DateTime::nullDate()
{
    return *sm_null;
}

// Return the date of the start of the program.
const DateTime& DateTime::startDate()
{
    return *sm_startDate;
}

DateTime::DateTime(Bool setToday) :
    month(1),
    wday(1),
    mday(1),
    year(0),
    hour(0),
    minute(0),
    second(0),
    microsecond(0)
{
    if (setToday) {
        setCurrent();
    }
}

// Initialize manually.
DateTime::DateTime(UInt16 _year,
        UInt8 _month,
        UInt8 _day,
        UInt8 _dayofWeek,
        UInt8 _hour,
        UInt8 _minutes,
        UInt8 _seconds,
        UInt32 _microsecond) :
    month(_month),
    wday(_day),
    mday(_dayofWeek),
    year(_year),
    hour(_hour),
    minute(_minutes),
    second(_seconds),
    microsecond(_microsecond)
{

}

DateTime::DateTime(const DateTime & _which) :
    month(_which.month),
    wday(_which.wday),
    mday(_which.mday),
    year(_which.year),
    hour(_which.hour),
    minute(_which.minute),
    second(_which.second),
    microsecond(_which.microsecond)
{
}

void DateTime::destroy()
{
    year = 0;
    month = 1;
    wday = 1;
    mday = 1;
    hour = 0;
    minute = 0;
    microsecond = 0;
    mday = 0;
    second = 0;
}

DateTime & DateTime::operator = (const DateTime & _which)
{
    year = _which.year;
    month = _which.month;
    wday = _which.wday;
    hour = _which.hour;
    minute = _which.minute;
    second = _which.second;
    microsecond = _which.microsecond;
    mday = _which.mday;

    return *this;
}

DateTime::DateTime(time_t ltime)
{
#ifdef _MSC_VER  // O3D_WINDOWS
    struct tm local;
    _localtime64_s(&local,&ltime);

    year = UInt16(local.tm_year + 1900);
    month = UInt8(local.tm_mon) + 1;
    wday = UInt8(local.tm_wday) + 1;
    hour = UInt8(local.tm_hour);
    minute = UInt8(local.tm_min);
    second = UInt8(local.tm_sec);
    mday = UInt8(local.tm_mday);
    microsecond = 0;
#else
    struct tm *local;
    local = localtime(&ltime);

    year = local->tm_year + 1900;
    month = UInt8(local->tm_mon) + 1;
    wday = UInt8(local->tm_wday) + 1;
    hour = local->tm_hour;
    minute = local->tm_min;
    second = local->tm_sec;
    mday = local->tm_mday;
    microsecond = 0;
#endif
}

Bool DateTime::operator == (const DateTime & _which) const
{
    return ((year == _which.year) &&
            (month == _which.month) &&
            (mday == _which.mday) &&
            (hour == _which.hour) &&
            (minute == _which.minute) &&
            (second == _which.second) &&
            (microsecond == _which.microsecond) &&
            (mday == _which.mday));
}

Bool DateTime::operator != (const DateTime & _which) const
{
    return !((*this) == _which);
}

Bool DateTime::operator <(const DateTime &_which) const
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

    if (hour > _which.hour) {
        return False;
    } else if (hour < _which.hour) {
        return True;
    }

    if (minute > _which.minute) {
        return False;
    } else if (minute < _which.minute) {
        return True;
    }

    if (second > _which.second) {
        return False;
    } else if (second < _which.second) {
        return True;
    }

    if (microsecond > _which.microsecond) {
        return False;
    } else if (microsecond < _which.microsecond) {
        return True;
    }

    // or ms equal
    return False;
}

Bool DateTime::operator <=(const DateTime &_which) const
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

    if (hour > _which.hour) {
        return False;
    } else if (hour < _which.hour) {
        return True;
    }

    if (minute > _which.minute) {
        return False;
    } else if (minute < _which.minute) {
        return True;
    }

    if (second > _which.second) {
        return False;
    } else if (second < _which.second) {
        return True;
    }

    if (microsecond > _which.microsecond) {
        return False;
    } else if (microsecond < _which.microsecond) {
        return True;
    }

    // or ms equal
    return True;
}

Bool DateTime::operator >(const DateTime &_which) const
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

    if (hour < _which.hour) {
        return False;
    } else if (hour > _which.hour) {
        return True;
    }

    if (minute < _which.minute) {
        return False;
    } else if (minute > _which.minute) {
        return True;
    }

    if (second < _which.second) {
        return False;
    } else if (second > _which.second) {
        return True;
    }

    if (microsecond < _which.microsecond) {
        return False;
    } else if (microsecond > _which.microsecond) {
        return True;
    }

    // or ms equal
    return False;
}

Bool DateTime::operator >=(const DateTime &_which) const
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

    if (hour < _which.hour) {
        return False;
    } else if (hour > _which.hour) {
        return True;
    }

    if (minute < _which.minute) {
        return False;
    } else if (minute > _which.minute) {
        return True;
    }

    if (second < _which.second) {
        return False;
    } else if (second > _which.second) {
        return True;
    }

    if (microsecond < _which.microsecond) {
        return False;
    } else if (microsecond > _which.microsecond) {
        return True;
    }

    // or ms equal
    return True;
}

Bool DateTime::writeToFile(OutStream &os) const
{
    // 12 bytes date
    os  << year
        << month
        << wday
        << mday
        << hour
        << minute
        << second
        << microsecond;

    return True;
}

Bool DateTime::readFromFile(InStream &is)
{
    // 12 bytes date
    is  >> year
        >> month
        >> wday
        >> mday
        >> hour
        >> minute
        >> second
        >> microsecond;

    return True;
}

String DateTime::buildString(const String & _arg) const
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
                        // leading 0
                        result << '0' << month;
                    } else {
                        result << month;
                    }
                    break;

                case 'A':
                    // letter day of week
                    result << String(dayString[wday-1]);
                    break;

                case 'a':
                    // letter short day of week
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

                case 'H':
                    // 2 digits hour
                    if (hour < 10) {
                        result << '0' << hour;  // leading 0
                    } else {
                        result << hour;
                    }
                    break;

                case 'M':
                    // 2 digits minute
                    if (minute < 10) {
                        result << '0' << minute;  // leading 0
                    } else {
                        result << minute;
                    }
                    break;

                case 'S':
                    // 2 digits second
                    if (second < 10) {
                        result << '0' << second;  // leading 0
                    } else {
                        result << second;
                    }
                    break;

                case 'f':
                    // 1 to 6 digits microsecond
                    result << microsecond;
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

Bool DateTime::buildFromString(const String &_value, const String &_arg)
{
    StringTokenizer arg(_arg, L"%");
    StringTokenizer value(_value, L" -.:,;/^|TZ");

    if (arg.countTokens() != value.countTokens()) {
        return False;
    }

//    WChar arg;

//    for (o3d::Int32 i = 0; i < _value.length(); ++i) {
//        if (_value[i] == '%' && i < _value.length()-1) {
//            arg = _value[i+1];
//        } else {
//            continue;
//        }

    while (arg.hasMoreElements() && value.hasMoreElements()) {
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

            case 'H':
                if (vals[0] == '0') {
                    // ignore leading 0
                    hour = UInt8(vals.toUInt32(1));
                } else {
                    hour = UInt8(vals.toUInt32());
                }
                if (hour > 23) {
                    return False;
                }
                break;

            case 'M':
                if (vals[0] == '0') {
                    // ignore leading 0
                    minute = UInt8(vals.toUInt32(1));
                } else {
                    minute = UInt8(vals.toUInt32());
                }
                if (minute > 59) {
                    return False;
                }
                break;

            case 'S':
                if (vals[0] == '0') {
                    // ignore leading 0
                    second = UInt8(vals.toUInt32(1));
                } else {
                    second = UInt8(vals.toUInt32());
                }
                if (second > 59) {
                    return False;
                }
                break;

            case 'f':
                vals.trimLeft('0', True);  // remove eventual leading 0
                if (vals.isValid()) {
                    microsecond = UInt8(vals.toUInt32());
                } else {
                    microsecond = 0;
                }
                if (microsecond > 999999) {
                    return False;
                }
                break;
        }
    }

    return True;
}

void DateTime::setCurrent()
{
#ifdef O3D_WINDOWS
    __time64_t ltime;
   _time64(&ltime);

    tm local;
    _localtime64_s(&local, &ltime);

    year = UInt16(local.tm_year + 1900);
    month = UInt8(local.tm_mon) + 1;
    day = UInt8(local.tm_wday) + 1;
    hour = UInt8(local.tm_hour);
    minute = UInt8(local.tm_min);
    second = UInt8(local.tm_sec);
    mday = UInt8(local.tm_mday);
    microsecond = 0;  // @todo howto ?
#else
    struct timeval ltime;
    struct tm *local;

    gettimeofday(&ltime, nullptr);
    local = localtime(&ltime.tv_sec);
    year = local->tm_year + 1900;
    month = UInt8(local->tm_mon) + 1;
    wday = UInt8(local->tm_wday) + 1;
    hour = local->tm_hour;
    minute = local->tm_min;
    second = local->tm_sec;
    mday = local->tm_mday;
    microsecond = o3d::UInt32(ltime.tv_usec);
#endif
}

void DateTime::fromTime(Float time, Bool UTC)
{
    fromTimeUs(Int64(time * 1000000.f), UTC);
}

void DateTime::fromTime(Double time, Bool UTC)
{
    fromTimeUs(Int64(time * 1000000.0), UTC);
}

void DateTime::fromTimeMs(Int64 ms, Bool UTC)
{
#ifdef O3D_WINDOWS
    __time64_t ltime = static_cast<time_t>(ms / 1000) - (UTC ? sm_localTz : 0);

    tm local;
    _localtime64_s(&local, &ltime);

    year = UInt16(local.tm_year + 1900);
    month = UInt8(local.tm_mon) + 1;
    day = UInt8(local.tm_wday) + 1;
    hour = UInt8(local.tm_hour);
    minute = UInt8(local.tm_min);
    second = UInt8(local.tm_sec);
    mday = UInt8(local.tm_mday);
    microsecond = static_cast<o3d::UInt32>((ms * 1000) - (static_cast<o3d::Int64>(ms / 1000) * 1000000));
#else
    time_t ts = static_cast<time_t>(ms / 1000) - (UTC ? sm_localTz : 0);
    struct tm *local;

    local = localtime(&ts);
    year = local->tm_year + 1900;
    month = UInt8(local->tm_mon) + 1;
    wday = UInt8(local->tm_wday) + 1;
    hour = local->tm_hour;
    minute = local->tm_min;
    second = local->tm_sec;
    mday = local->tm_mday;
    microsecond = static_cast<o3d::UInt32>((ms * 1000) - (static_cast<o3d::Int64>(ms / 1000) * 1000000));
#endif
}

void DateTime::fromTimeUs(Int64 us, Bool UTC)
{
#ifdef O3D_WINDOWS
    __time64_t ltime = static_cast<time_t>(us / 1000000) - (UTC ? sm_localTz : 0);

    tm local;
    _localtime64_s(&local, &ltime);

    year = UInt16(local.tm_year + 1900);
    month = UInt8(local.tm_mon) + 1;
    day = UInt8(local.tm_wday) + 1;
    hour = UInt8(local.tm_hour);
    minute = UInt8(local.tm_min);
    second = UInt8(local.tm_sec);
    mday = UInt8(local.tm_mday);
    microsecond = static_cast<o3d::UInt32>(us - (static_cast<o3d::Int64>(us / 1000000) * 1000000));
#else
    time_t ts = static_cast<time_t>(us / 1000000) - (UTC ? sm_localTz : 0);
    struct tm *local;

    local = localtime(&ts);
    year = local->tm_year + 1900;
    month = UInt8(local->tm_mon) + 1;
    wday = UInt8(local->tm_wday) + 1;
    hour = local->tm_hour;
    minute = local->tm_min;
    second = local->tm_sec;
    mday = local->tm_mday;
    microsecond = static_cast<o3d::UInt32>(us - (static_cast<o3d::Int64>(us / 1000000) * 1000000));
#endif
}

time_t DateTime::toTime_t(Bool UTC) const
{
#ifdef O3D_WINDOWS
    tm lObjectTime;

    lObjectTime.tm_sec = second + (UTC ? sm_localTz : 0);
    lObjectTime.tm_min = minute;
    lObjectTime.tm_hour = hour;
    lObjectTime.tm_mday = mday;
    lObjectTime.tm_mon = month-1;
    lObjectTime.tm_year = year-1900;
    lObjectTime.tm_wday = 0;  // wday-1;
    lObjectTime.tm_yday = 0;
    lObjectTime.tm_isdst = 0;

    return _mktime64(&lObjectTime);
#else
    struct tm lObjectTime;

    lObjectTime.tm_sec = second + (UTC ? sm_localTz : 0);
    lObjectTime.tm_min = minute;
    lObjectTime.tm_hour = hour;
    lObjectTime.tm_mday = mday;
    lObjectTime.tm_mon = month-1;
    lObjectTime.tm_year = year-1900;
    lObjectTime.tm_wday = 0;  // wday-1;
    lObjectTime.tm_yday = 0;
    lObjectTime.tm_isdst = 0;

    return mktime(&lObjectTime);
#endif
}

Int32 DateTime::toTimestamp(Bool UTC) const
{
    // time_t is in ms, adding the microsecond part but want it in millisecond
    return static_cast<Int32>(toTime_t(UTC)) + (static_cast<Int32>(microsecond) / 1000);
}

Float DateTime::toFloatTimestamp(Bool UTC) const
{
    // time_t is in second unit, adding the microsecond part but want it in second
    return static_cast<Float>(toTime_t(UTC)) + (static_cast<Float>(microsecond) / 1000000.f);
}

Double DateTime::toDoubleTimestamp(Bool UTC) const
{
    // time_t is in second unit, adding the microsecond part but want it in second
    return static_cast<Double>(toTime_t(UTC)) + (static_cast<Double>(microsecond) / 1000000.0);
}

UInt8 DateTime::getDayOfWeek() const
{
    static Int32 t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    Int32 y = year - (month < 3 ? 1 : 0);

    return ((y + y/4 - y/100 + y/400 + t[month-1] + (mday-1)) % 7) + 1;
}

Bool DateTime::isOlderThan(const DateTime& today, UInt32 days)
{
#ifdef O3D_WINDOWS
    __time64_t start,end;
    double elapsed;

    // set start/end times structures
    tm startTM, endTM;

    // info : tm_wday and tm_yday are ignored for mktime
    startTM.tm_year = year-1900;
    startTM.tm_mon = month-1;
    startTM.tm_wday = wday-1;
    startTM.tm_hour = hour;
    startTM.tm_min = minute;
    startTM.tm_sec = second;
    startTM.tm_mday = mday;
    startTM.tm_isdst = 0;

    endTM.tm_year = today.year-1900;
    endTM.tm_mon = today.month-1;
    endTM.tm_wday = today.wday-1;
    endTM.tm_hour = today.hour;
    endTM.tm_min = today.minute;
    endTM.tm_sec = today.second;
    endTM.tm_mday = today.mday;
    endTM.tm_isdst = 0;

    // make start/end times
    start = _mktime64(&startTM);
    end   = _mktime64(&endTM);

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
    startTM.tm_hour = hour;
    startTM.tm_min = minute;
    startTM.tm_sec = second;
    startTM.tm_mday = mday;
    startTM.tm_isdst = 0;

    endTM.tm_year = today.year-1900;
    endTM.tm_mon = today.month-1;
    endTM.tm_wday = today.wday-1;
    endTM.tm_hour = today.hour;
    endTM.tm_min = today.minute;
    endTM.tm_sec = today.second;
    endTM.tm_mday = today.mday;
    endTM.tm_isdst = 0;

    // make start/end times
    start = mktime(&startTM);
    end   = mktime(&endTM);

    // diff the two times
    elapsed = difftime(end,start);

#endif

    // compare time in seconds (transform days in seconds)
    if (elapsed > (days * 24 * 60 * 60)) {
        return True;
    }

    return False;
}
