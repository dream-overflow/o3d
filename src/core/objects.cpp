/**
 * @file objects.cpp
 * @brief Implementation of Objects.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2005-03-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/objects.h"

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

//---------------------------------------------------------------------------------------
// class Date
//---------------------------------------------------------------------------------------

Date* Date::sm_null = nullptr;
Date* Date::sm_startDate = nullptr;

void Date::init()
{
	if (!sm_null)
	{
		sm_null = new Date(False);
		sm_startDate = new Date(True);
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
	year(0),
	hour(0),
	minute(0),
	second(0),
	millisecond(0)
{
	if (setToday)
		setCurrent();
}

// Initialize manually.
Date::Date(
		UInt16 _year,
		Month _month,
		Day _day,
		UInt8 _dayofWeek,
		UInt8 _hour,
		UInt8 _minutes,
		UInt8 _seconds,
		UInt16 _millisecond) :
	month(_month),
	day(_day),
	mday(_dayofWeek),
	year(_year),
	hour(_hour),
	minute(_minutes),
	second(_seconds),
	millisecond(_millisecond)
{

}

Date::Date(const Date & _which) :
	month(_which.month),
	day(_which.day),
	mday(_which.mday),
	year(_which.year),
	hour(_which.hour),
	minute(_which.minute),
	second(_which.second),
	millisecond(_which.millisecond)
{
}

void Date::destroy()
{
	year = 0;
	month = Month(0);
	day = Day(0);
	hour = 0;
	minute = 0;
	millisecond = 0;
	mday = 0;
	second = 0;
}

Date & Date::operator = (const Date & _which)
{
	year = _which.year;
	month = _which.month;
	day = _which.day;
	hour = _which.hour;
	minute = _which.minute;
	second = _which.second;
	millisecond = _which.millisecond;
	mday = _which.mday;

	return *this;
}

Date::Date(time_t ltime)
{
#ifdef O3D_VC_COMPILER
	struct tm local;
	_localtime64_s(&local,&ltime);

	year = UInt16(local.tm_year + 1900);
	month = Month(local.tm_mon);
	day = Day(local.tm_wday);
	hour = UInt8(local.tm_hour);
	minute = UInt8(local.tm_min);
	second = UInt8(local.tm_sec);
	mday = UInt8(local.tm_mday);
	millisecond = 0;
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
	millisecond = 0;
#endif
}

Bool Date::operator == (const Date & _which)
{
	return ((year == _which.year) &&
			(month == _which.month) &&
			(day == _which.day) &&
			(hour == _which.hour) &&
			(minute == _which.minute) &&
			(second == _which.second) &&
			(millisecond == _which.millisecond) &&
			(mday == _which.mday));
}

Bool Date::operator != (const Date & _which)
{
	return !((*this) == _which);
}

Bool Date::writeToFile(OutStream &os) const
{
	// 10 bytes date
    os  << year
		<< UInt8(month)
		<< UInt8(day)
		<< mday
		<< hour
		<< minute
		<< second
		<< millisecond;

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

    is  >> mday
		>> hour
		>> minute
		>> second
		>> millisecond;

	return True;
}

String Date::buildString(const String & _arg) const
{
    // @see strftime http://www.cplusplus.com/reference/ctime/strftime/
	String result;

	UInt32 k = 0;

	while (k < _arg.length())
		if (_arg.getData()[k] == '%')
		{
			if ((++k) == _arg.length())
				break;

			switch (_arg.getData()[k])
			{
				case 'y':
					result << year;
				break;
				case 'm':
                    result << String(monthString[month]);
				break;
                case 'b':
                    result << String(shortMonthString[month]);
                break;
				case 'M':
					result << (month+1);
				break;
				case 'd':
                    result << String(dayString[day]);
				break;
                case 'a':
                    result << String(shortDayString[day]);
                break;
				case 'D':
					result << mday;
				break;
				case 'h':
					if (hour < 10) result << '0';
					result << hour;
				break;
				case 'i':
					if (minute < 10) result << '0';
					result << minute;
				break;
				case 's':
					if (second < 10) result << '0';
					result << second;
				break;
				case 'l':
					result << millisecond;
				break;
			}

			++k;
		}
		else
		{
			result += _arg.getData()[k];
			++k;
		}

	return result;
}

void Date::buildFromString(const String &_value, const String &_arg)
{
	StringTokenizer arg(_arg, L"%");
	StringTokenizer value(_value, L" -.:,;/^|");

	while (arg.hasMoreElements() && value.hasMoreElements())
	{
		String argu(arg.nextElement());
		String vals(value.nextElement());

		switch(argu.getData()[0])
		{
			case 'y':
				year = UInt16(vals.toUInt32());
			break;
			case 'm':
				month = Month(vals.toUInt32()-1);
			break;
			case 'M':
				month = Month(vals.toUInt32()-1);
			break;
			case 'd':
				//mday = ...
				day = Day(vals.toUInt32());
			break;
			case 'D':
				mday = UInt8(vals.toUInt32());
				//day = ...
			break;
			case 'h':
				hour = UInt8(vals.toUInt32());
			break;
			case 'i':
				minute = UInt8(vals.toUInt32());
			break;
			case 's':
				second = UInt8(vals.toUInt32());
			break;
			case 'l':
				millisecond = UInt8(vals.toUInt32());
			break;
		}
	}
}

Bool Date::setFromString(const String & _value, const String & _arg)
{
	String date(_value);
	String substring;

	UInt32 k = 0;

	Bool result = True;

	std::wistringstream iss;

	while (k < _arg.length())
	{
		if (_arg.getData()[k] == '%')
		{
			if (k == _arg.length() - 1)
				break;

			k++;

			if (k == _arg.length() - 1)
				substring = date;
			else
			{
				substring = date;
				substring.truncate(substring.find(_arg.getData()[k+1]));
				date = date.sub(date.find(_arg.getData()[k+1]));

				if (k+1 != _arg.length() - 1) date = date.sub(1);
			}

            iss.str(substring.getData());

            switch(_arg.getData()[k])
            {
            case 'y':
                iss >> year;
                break;
            case 'm':
            {
                Bool find = False;

                for (UInt32 i = 0 ; i < 12 ; ++i)
                    if (wcscmp(iss.str().c_str(), monthString[i]) == 0)
                    {
                        month = Month(i);
                        find = True;
                        break;
                    }

                if (!find) result = False;
            }
                break;
            case 'b':
            {
                Bool find = False;

                for (UInt32 i = 0 ; i < 12 ; ++i)
                    if (wcscmp(iss.str().c_str(), shortMonthString[i]) == 0)
                    {
                        month = Month(i);
                        find = True;
                        break;
                    }

                if (!find) result = False;
            }
                break;
            case 'M':
                Int32 lmonth;
                iss >> lmonth;
                month = Month(lmonth-1);
                break;
            case 'd':
            {
                Bool find = False;

                for (UInt32 i = 0 ; i < 7 ; ++i)
                    if (wcscmp(iss.str().c_str(), dayString[i]) == 0)
                    {
                        day = Day(i);
                        //mday = ...
                        find = True;
                        break;
                    }

                if (!find) result = False;
            }
                break;
            case 'a':
            {
                Bool find = False;

                for (UInt32 i = 0 ; i < 7 ; ++i)
                    if (wcscmp(iss.str().c_str(), shortDayString[i]) == 0)
                    {
                        day = Day(i);
                        //mday = ...
                        find = True;
                        break;
                    }

                if (!find) result = False;
            }
                break;
            case 'D':
            {
                Int32 lmday;
                iss >> lmday;
                mday = (UInt8)lmday;
                //day = ...
            }
                break;
            case 'h':
            {
                Int32 lhour;
                iss >> lhour;
                hour = (UInt8)lhour;
            }
                break;
            case 'i':
            {
                Int32 lminute;
                iss >> lminute;
                minute = (UInt8)lminute;
            }
                break;
            case 's':
            {
                Int32 lsecond;
                iss >> lsecond;
                second = (UInt8)lsecond;
            }
                break;
            case 'l':
            {
                Int32 lmillisecond;
                iss >> lmillisecond;
                millisecond = (UInt8)lmillisecond;
            }
                break;

                //				k++;
            }

            k++;
        }
		else
		{
			date = date.sub(1);
			k++;
		}
	}

	return result;
}

void Date::setCurrent()
{
#ifdef O3D_WINDOWS
	_tzset();

	__time64_t ltime;
   _time64(&ltime);

	tm local;
	_localtime64_s(&local,&ltime);

	year = UInt16(local.tm_year + 1900);
	month = Month(local.tm_mon);
	day = Day(local.tm_wday);
	hour = UInt8(local.tm_hour);
	minute = UInt8(local.tm_min);
	second = UInt8(local.tm_sec);
	mday = UInt8(local.tm_mday);
	millisecond = 0;
#else
	struct timeval ltime;
	struct tm *local;

	tzset();

	gettimeofday(&ltime, NULL);
	local = localtime(&ltime.tv_sec);
	year = local->tm_year + 1900;
	month = Month(local->tm_mon);
	day = Day(local->tm_wday);
	hour = local->tm_hour;
	minute = local->tm_min;
	second = local->tm_sec;
	mday = local->tm_mday;
	millisecond = ltime.tv_usec / 1000;
#endif
}

void Date::setMsTime(Int64 ms)
{
#ifdef O3D_WINDOWS
    _tzset();

    __time64_t ltime;
    ltime = ms / 1000;

    tm local;
    _localtime64_s(&local,&ltime);

    year = UInt16(local.tm_year + 1900);
    month = Month(local.tm_mon);
    day = Day(local.tm_wday);
    hour = UInt8(local.tm_hour);
    minute = UInt8(local.tm_min);
    second = UInt8(local.tm_sec);
    mday = UInt8(local.tm_mday);
    millisecond = 0;
#else
    struct timeval ltime;
    ltime.tv_sec = ms / 1000;
    ltime.tv_usec = (ms % 1000) * 1000;

    struct tm *local;

    tzset();

    local = localtime(&ltime.tv_sec);
    year = local->tm_year + 1900;
    month = Month(local->tm_mon);
    day = Day(local->tm_wday);
    hour = local->tm_hour;
    minute = local->tm_min;
    second = local->tm_sec;
    mday = local->tm_mday;
    millisecond = ltime.tv_usec / 1000;
#endif
}

time_t Date::toTime_t() const
{
#ifdef O3D_WINDOWS
	tm lObjectTime;

	lObjectTime.tm_sec = second;
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

	lObjectTime.tm_sec = second;
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

	struct tm startTM,endTM;

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
	if (elapsed > (days * 24 * 60 * 60))
		return True;

	return False;
}

