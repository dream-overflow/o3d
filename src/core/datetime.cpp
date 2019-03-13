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
    L"December" };

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
    L"Dec" };

static const WChar * dayString[] = {
    L"Sunday",
    L"Monday",
    L"Tuesday",
    L"Wednesday",
    L"Thursday",
    L"Friday",
    L"Saturday" };

static const WChar * shortDayString[] = {
    L"Sun",
    L"Mon",
    L"Tue",
    L"Wed",
    L"Thu",
    L"Fri",
    L"Sat" };

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
    month(Month(0)),
    day(Day(0)),
    mday(0),
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
DateTime::DateTime(
        UInt16 _year,
        Month _month,
        Day _day,
        UInt8 _dayofWeek,
        UInt8 _hour,
        UInt8 _minutes,
        UInt8 _seconds,
        UInt32 _microsecond) :
    month(_month),
    day(_day),
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
    day(_which.day),
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
    month = Month(0);
    day = Day(0);
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
    day = _which.day;
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
    month = Month(local.tm_mon);
    day = Day(local.tm_wday);
    hour = UInt8(local.tm_hour);
    minute = UInt8(local.tm_min);
    second = UInt8(local.tm_sec);
    mday = UInt8(local.tm_mday);
    microsecond = 0;
#else
    struct tm *local;
    local = localtime(&ltime);

    year = local->tm_year + 1900;
    month = Month(local->tm_mon);
    day = Day(local->tm_wday);
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
            (day == _which.day) &&
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
    // no mday in comparison
    return ((year < _which.year) ||
            (month < _which.month) ||
            (day < _which.day) ||
            (hour < _which.hour) ||
            (minute < _which.minute) ||
            (second < _which.second) ||
            (microsecond < _which.microsecond));
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

    if (day > _which.day) {
        return False;
    } else if (day < _which.day) {
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
    // no mday in comparison
    return ((year > _which.year) ||
            (month > _which.month) ||
            (day > _which.day) ||
            (hour > _which.hour) ||
            (minute > _which.minute) ||
            (second > _which.second) ||
            (microsecond > _which.microsecond));
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

    if (day < _which.day) {
        return False;
    } else if (day > _which.day) {
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
        << UInt8(month)
        << UInt8(day)
        << mday
        << hour
        << minute
        << second
        << microsecond;

    return True;
}

Bool DateTime::readFromFile(InStream &is)
{
    UInt8 tmp;

    // 12 bytes date
    is  >> year
        >> tmp;
    month = Month(tmp);

    is >> tmp;
    day = Day(tmp);

    is  >> mday
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
                    result << String(monthString[month]);
                    break;

                case 'b':
                    // letter short month
                    result << String(shortMonthString[month]);
                    break;

                case 'm':
                    // 2 digits month
                    if (month+1 < 10) {
                        // leading 0
                        result << '0' << (month+1);
                    } else {
                        result << (month+1);
                    }
                    break;

                case 'A':
                    // letter day of week
                    result << String(dayString[day]);
                    break;

                case 'a':
                    // letter short day of week
                    result << String(shortDayString[day]);
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
                for (int i = 0; i < 12; ++i) {
                    if (vals == monthString[i]) {
                        month = Month(i);
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
                for (int i = 0; i < 12; ++i) {
                    if (vals == shortMonthString[i]) {
                        month = Month(i);
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
                    month = Month(vals.toUInt32(1)-1);
                } else {
                    month = Month(vals.toUInt32()-1);
                }
                if (month > 13) {
                    return False;
                }
                break;

            case 'A':
                // letter day
                for (int i = 0; i < 6; ++i) {
                    if (vals == dayString[i]) {
                        day = Day(i);
                        found = True;
                        break;
                    }
                }
                if (!found) {
                    return False;
                }
                // mday = ...
                break;

            case 'a':
                // letter short day
                for (int i = 0; i < 6; ++i) {
                    if (vals == shortDayString[i]) {
                        day = Day(i);
                        found = True;
                        break;
                    }
                }
                if (!found) {
                    return False;
                }
                // mday = ...
                break;

            case 'd':
                // 2 digits month day
                if (vals[0] == '0') {
                    // ignore leading 0
                    mday = UInt8(vals.toUInt32(1));
                    // day = ...
                } else {
                    mday = UInt8(vals.toUInt32());
                    // day = ...
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

//Bool DateTime::setFromString(const String & _value, const String & _arg)
//{
//    String date(_value);
//    String substring;

//    UInt32 k = 0;

//    Bool result = True;

//    std::wistringstream iss;

//    while (k < _arg.length()) {
//        if (_arg.getData()[k] == '%') {
//            if (k == _arg.length() - 1) {
//                break;
//            }

//            k++;

//            if (k == _arg.length() - 1) {
//                substring = date;
//            } else {
//                substring = date;
//                substring.truncate(substring.find(_arg.getData()[k+1]));
//                date = date.sub(date.find(_arg.getData()[k+1]));

//                if (k+1 != _arg.length() - 1) {
//                    date = date.sub(1);
//                }
//            }

//            iss.str(substring.getData());

//            switch(_arg.getData()[k]) {
//                case 'y':
//                    iss >> year;
//                    break;
//                case 'm':
//                {
//                    Bool find = False;

//                    for (UInt32 i = 0 ; i < 12 ; ++i) {
//                        if (wcscmp(iss.str().c_str(), monthString[i]) == 0) {
//                            month = Month(i);
//                            find = True;
//                            break;
//                        }
//                    }

//                    if (!find) {
//                        result = False;
//                    }
//                }
//                    break;
//                case 'b':
//                {
//                    Bool find = False;

//                    for (UInt32 i = 0 ; i < 12 ; ++i) {
//                        if (wcscmp(iss.str().c_str(), shortMonthString[i]) == 0) {
//                            month = Month(i);
//                            find = True;
//                            break;
//                        }
//                    }

//                    if (!find) {
//                        result = False;
//                    }
//                }
//                    break;
//                case 'M':
//                    Int32 lmonth;
//                    iss >> lmonth;
//                    month = Month(lmonth-1);
//                    break;
//                case 'd':
//                {
//                    Bool find = False;

//                    for (UInt32 i = 0 ; i < 7 ; ++i) {
//                        if (wcscmp(iss.str().c_str(), dayString[i]) == 0) {
//                            day = Day(i);
//                            //mday = ...
//                            find = True;
//                            break;
//                        }
//                    }

//                    if (!find) {
//                        result = False;
//                    }
//                }
//                    break;
//                case 'a':
//                {
//                    Bool find = False;

//                    for (UInt32 i = 0 ; i < 7 ; ++i) {
//                        if (wcscmp(iss.str().c_str(), shortDayString[i]) == 0) {
//                            day = Day(i);
//                            //mday = ...
//                            find = True;
//                            break;
//                        }
//                    }

//                    if (!find) {
//                        result = False;
//                    }
//                }
//                    break;
//                case 'D':
//                {
//                    Int32 lmday;
//                    iss >> lmday;
//                    mday = (UInt8)lmday;
//                    //day = ...
//                }
//                    break;
//                case 'h':
//                {
//                    Int32 lhour;
//                    iss >> lhour;
//                    hour = (UInt8)lhour;
//                }
//                    break;
//                case 'i':
//                {
//                    Int32 lminute;
//                    iss >> lminute;
//                    minute = (UInt8)lminute;
//                }
//                    break;
//                case 's':
//                {
//                    Int32 lsecond;
//                    iss >> lsecond;
//                    second = (UInt8)lsecond;
//                }
//                    break;
//                case 'l':
//                {
//                    Int32 lmicrosecond;
//                    iss >> lmicrosecond;
//                    microsecond = (UInt8)lmicrosecond;
//                }
//                    break;

//                // k++;
//            }

//            k++;
//        } else {
//            date = date.sub(1);
//            k++;
//        }
//    }

//    return result;
//}

void DateTime::setCurrent()
{
#ifdef O3D_WINDOWS
    __time64_t ltime;
   _time64(&ltime);

    tm local;
    _localtime64_s(&local, &ltime);

    year = UInt16(local.tm_year + 1900);
    month = Month(local.tm_mon);
    day = Day(local.tm_wday);
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
    month = Month(local->tm_mon);
    day = Day(local->tm_wday);
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
    month = Month(local.tm_mon);
    day = Day(local.tm_wday);
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
    month = Month(local->tm_mon);
    day = Day(local->tm_wday);
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
    month = Month(local.tm_mon);
    day = Day(local.tm_wday);
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
    month = Month(local->tm_mon);
    day = Day(local->tm_wday);
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
    lObjectTime.tm_mon = Int32(month);
    lObjectTime.tm_year = year - 1900;
    lObjectTime.tm_wday = 0;
    lObjectTime.tm_yday = 0;
    lObjectTime.tm_isdst = 0;

    return _mktime64(&lObjectTime);
#else
    struct tm lObjectTime;

    lObjectTime.tm_sec = second + (UTC ? sm_localTz : 0);
    lObjectTime.tm_min = minute;
    lObjectTime.tm_hour = hour;
    lObjectTime.tm_mday = mday;
    lObjectTime.tm_mon = Int32(month);
    lObjectTime.tm_year = year - 1900;
    lObjectTime.tm_wday = 0;
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

Bool DateTime::isOlderThan(const DateTime& today, UInt32 days)
{
#ifdef O3D_WINDOWS
    __time64_t start,end;
    double elapsed;

    // set start/end times structures
    tm startTM,endTM;

    // info : tm_wday and tm_yday are ignored for mktime
    startTM.tm_year = year - 1900;
    startTM.tm_mon = month;
    startTM.tm_wday = day;
    startTM.tm_hour = hour;
    startTM.tm_min = minute;
    startTM.tm_sec = second;
    startTM.tm_mday = mday;
    startTM.tm_isdst = 0;

    endTM.tm_year = today.year - 1900;
    endTM.tm_mon = today.month;
    endTM.tm_wday = today.day;
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
    startTM.tm_year = year - 1900;
    startTM.tm_mon = month;
    startTM.tm_wday = day;
    startTM.tm_hour = hour;
    startTM.tm_min = minute;
    startTM.tm_sec = second;
    startTM.tm_mday = mday;
    startTM.tm_isdst = 0;

    endTM.tm_year = today.year - 1900;
    endTM.tm_mon = today.month;
    endTM.tm_wday = today.day;
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
