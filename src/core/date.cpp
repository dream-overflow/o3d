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
    month(Month(0)),
    day(Day(0)),
    mday(0),
    year(0)
{
    if (setToday) {
        setCurrent();
    }
}

// Initialize manually.
Date::Date(UInt16 _year,
        Month _month,
        Day _day,
        UInt8 _dayofWeek) :
    month(_month),
    day(_day),
    mday(_dayofWeek),
    year(_year)
{

}

Date::Date(const Date & _which) :
    month(_which.month),
    day(_which.day),
    mday(_which.mday),
    year(_which.year)
{
}

void Date::destroy()
{
    year = 0;
    month = Month(0);
    day = Day(0);
    mday = 0;
}

Date & Date::operator = (const Date & _which)
{
    year = _which.year;
    month = _which.month;
    day = _which.day;
    mday = _which.mday;

    return *this;
}

Date::Date(time_t ltime)
{
#ifdef _MSC_VER  // O3D_WINDOWS
    struct tm local;
    _localtime64_s(&local,&ltime);

    year = UInt16(local.tm_year + 1900);
    month = Month(local.tm_mon);
    day = Day(local.tm_wday);
    mday = UInt8(local.tm_mday);
#else
    struct tm *local;
    local = localtime(&ltime);

    year = local->tm_year + 1900;
    month = Month(local->tm_mon);
    day = Day(local->tm_wday);
    mday = local->tm_mday;
#endif
}

Bool Date::operator == (const Date & _which) const
{
    return ((year == _which.year) &&
            (month == _which.month) &&
            (day == _which.day) &&
            (mday == _which.mday));
}

Bool Date::operator != (const Date & _which) const
{
    return !((*this) == _which);
}

Bool Date::operator <(const Date &_which) const
{
    // no mday in comparison
    return ((year < _which.year) ||
            (month < _which.month) ||
            (day < _which.day));
}

Bool Date::writeToFile(OutStream &os) const
{
    // 10 bytes date
    os  << year
        << UInt8(month)
        << UInt8(day)
        << mday;

    return True;
}

Bool Date::readFromFile(InStream &is)
{
    UInt8 tmp;

    // 10 bytes date
    is  >> year
        >> tmp;
    month = Month(tmp);

    is >> tmp;
    day = Day(tmp);

    is >> mday;

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
                    result << String(monthString[month]);
                    break;

                case 'b':
                    // letter short month
                    result << String(shortMonthString[month]);
                    break;

                case 'm':
                    // 2 digits month
                    if (month+1 < 10) {
                        result << '0' << (month+1);  // leading 0
                    } else {
                        result << (month+1);
                    }
                    break;

                case 'A':
                    // letter day
                    result << String(dayString[day]);
                    break;

                case 'a':
                    // letter short day
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
            }

            ++k;
        } else {
            result += _arg.getData()[k];
            ++k;
        }
    }

    return result;
}

void Date::buildFromString(const String &_value, const String &_arg)
{
    StringTokenizer arg(_arg, L"%");
    StringTokenizer value(_value, L" -.:,;/^|TZ");

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
                break;

            case 'm':
                // 2 digits month
                if (vals[0] == '0') {
                    // ignore leading 0
                    month = Month(vals.toUInt32(1)-1);
                } else {
                    month = Month(vals.toUInt32()-1);
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
                // mday = ...
                break;

            case 'd':
                // 2 digits month day
                if (vals[0] == '0') {
                    // ignore leading 0
                    mday = UInt8(vals.toUInt32(1));
                    //day = ...
                } else {
                    mday = UInt8(vals.toUInt32());
                    //day = ...
                }
                break;
        }
    }
}

//Bool Date::setFromString(const String & _value, const String & _arg)
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

//                if (k+1 != _arg.length() - 1) date = date.sub(1);
//            }

//            iss.str(substring.getData());

//            switch(_arg.getData()[k]) {
//                case 'y':
//                    // 4 digits year
//                    iss >> year;
//                    break;

//                case 'm':
//                {
//                    // letter month
//                    Bool find = False;

//                    for (UInt32 i = 0 ; i < 12 ; ++i)
//                        if (wcscmp(iss.str().c_str(), monthString[i]) == 0)
//                        {
//                            month = Month(i);
//                            find = True;
//                            break;
//                        }

//                    if (!find) result = False;
//                }
//                    break;

//                case 'b':
//                {
//                    // letter short month
//                    Bool find = False;

//                    for (UInt32 i = 0 ; i < 12 ; ++i)
//                        if (wcscmp(iss.str().c_str(), shortMonthString[i]) == 0)
//                        {
//                            month = Month(i);
//                            find = True;
//                            break;
//                        }

//                    if (!find) result = False;
//                }
//                    break;

//                case 'M':
//                    // 2 digits month
//                    Int32 lmonth;
//                    iss >> lmonth;
//                    month = Month(lmonth-1);
//                    break;

//                case 'd':
//                {
//                    // letter day of week
//                    Bool find = False;

//                    for (UInt32 i = 0 ; i < 7 ; ++i)
//                        if (wcscmp(iss.str().c_str(), dayString[i]) == 0)
//                        {
//                            day = Day(i);
//                            //mday = ...
//                            find = True;
//                            break;
//                        }

//                    if (!find) result = False;
//                }
//                    break;

//                case 'a':
//                {
//                    // letter short day of week
//                    Bool find = False;

//                    for (UInt32 i = 0 ; i < 7 ; ++i)
//                        if (wcscmp(iss.str().c_str(), shortDayString[i]) == 0)
//                        {
//                            day = Day(i);
//                            //mday = ...
//                            find = True;
//                            break;
//                        }

//                    if (!find) result = False;
//                }
//                    break;

//                case 'D':
//                {
//                    // 2 digits day of month
//                    Int32 lmday;
//                    iss >> lmday;
//                    mday = (UInt8)lmday;
//                    //day = ...
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

void Date::setCurrent()
{
#ifdef O3D_WINDOWS
    __time64_t ltime;
   _time64(&ltime);

    tm local;
    _localtime64_s(&local,&ltime);

    year = UInt16(local.tm_year + 1900);
    month = Month(local.tm_mon);
    day = Day(local.tm_wday);
    mday = UInt8(local.tm_mday);
#else
    struct timeval ltime;
    struct tm *local;

    gettimeofday(&ltime, nullptr);
    local = localtime(&ltime.tv_sec);
    year = local->tm_year + 1900;
    month = Month(local->tm_mon);
    day = Day(local->tm_wday);
    mday = local->tm_mday;
#endif
}

void Date::setMsTime(Int64 ms)
{
#ifdef O3D_WINDOWS
    __time64_t ltime;
    ltime = ms / 1000;

    tm local;
    _localtime64_s(&local,&ltime);

    year = UInt16(local.tm_year + 1900);
    month = Month(local.tm_mon);
    day = Day(local.tm_wday);
    mday = UInt8(local.tm_mday);
#else
    struct timeval ltime;
    ltime.tv_sec = ms / 1000;
    ltime.tv_usec = (ms % 1000) * 1000;

    struct tm *local;

    local = localtime(&ltime.tv_sec);
    year = local->tm_year + 1900;
    month = Month(local->tm_mon);
    day = Day(local->tm_wday);
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
    lObjectTime.tm_mon = Int32(month);
    lObjectTime.tm_year = year - 1900;
    lObjectTime.tm_wday = 0;
    lObjectTime.tm_yday = 0;
    lObjectTime.tm_isdst = 0;

    return _mktime64(&lObjectTime);
#else
    struct tm lObjectTime;

    lObjectTime.tm_sec = (UTC ? sm_localTz : 0);
    lObjectTime.tm_min = 0;
    lObjectTime.tm_hour = 0;
    lObjectTime.tm_mday = mday;
    lObjectTime.tm_mon = Int32(month);
    lObjectTime.tm_year = year - 1900;
    lObjectTime.tm_wday = 0;
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
    return static_cast<Float>(toTime_t(UTC)) / 1000.f;
}

Double Date::toDoubleTimestamp(Bool UTC) const
{
    // time_t is in second unit
    return static_cast<Double>(toTime_t(UTC)) / 1000.0;
}

Bool Date::isOlderThan(const Date& today, UInt32 days)
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
    startTM.tm_hour = 0;
    startTM.tm_min = 0;
    startTM.tm_sec = 0;
    startTM.tm_mday = mday;
    startTM.tm_isdst = 0;

    endTM.tm_year = today.year - 1900;
    endTM.tm_mon = today.month;
    endTM.tm_wday = today.day;
    endTM.tm_hour = 0;
    endTM.tm_min = 0;
    endTM.tm_sec = 0;
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
    startTM.tm_hour = 0;
    startTM.tm_min = 0;
    startTM.tm_sec = 0;
    startTM.tm_mday = mday;
    startTM.tm_isdst = 0;

    endTM.tm_year = today.year - 1900;
    endTM.tm_mon = today.month;
    endTM.tm_wday = today.day;
    endTM.tm_hour = 0;
    endTM.tm_min = 0;
    endTM.tm_sec = 0;
    endTM.tm_mday = 0;
    endTM.tm_isdst = 0;

    // make start/end times
    start = mktime(&startTM);
    end   = mktime(&endTM);

    // diff the two times
    elapsed = difftime(end,start);

#endif

    // compare time in seconds (transform days in seconds)
    if (elapsed > (days * 24 * 60 * 60))
        return True;

    return False;
}
