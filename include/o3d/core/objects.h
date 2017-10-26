/**
 * @file objects.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_OBJECTS_H
#define _O3D_OBJECTS_H

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

//---------------------------------------------------------------------------------------
//! @class Date
//-------------------------------------------------------------------------------------
//! Date accessor and helper.
//---------------------------------------------------------------------------------------
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
	Date(
			UInt16 _year,
			Month _month,
			Day _day,
			UInt8 _dayofWeek,
			UInt8 _hour,
			UInt8 _minutes,
			UInt8 _seconds,
			UInt16 _millisecond);

	//! Copy ctor
	Date(const Date & _which);

	//! Destructor
	virtual ~Date() {}

	//! Clear the date object.
	void destroy();

	//! Copy.
	Date & operator = (const Date & _which);

	//! Compare to another date object for equality.
	Bool operator == (const Date & _which);

	//! Compare to another date object for inequality.
	Bool operator != (const Date & _which);

	//! Build a string containing the date with a specified format and return it.
	String buildString(const String & _arg = String("%d, %m %y  %h:%i")) const;

	//! Set the date of the object from a string and a specified format.
	Bool setFromString(const String & _value, const String & _arg = String("%d, %m %y  %h:%i"));

	//! Build the date of the object from a string and a specified format.
	//! @note More sophisticated than SetFromString. It can use day and month from letters.
	void buildFromString(const String & _value, const String &_arg = String("%y-%m-%d %h:%i:%s"));

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

	UInt8 hour;          //!< hours 0..23
	UInt8 minute;        //!< minutes 0..59
	UInt8 second;        //!< second 0..59
	UInt16 millisecond;  //!< millisecond

private:

	static Date *sm_null;
	static Date *sm_startDate;

	static void init();
	static void quit();
};


//---------------------------------------------------------------------------------------
//! @class TimeCounter
//-------------------------------------------------------------------------------------
//! A precise time counter object. It count as precise as the hardware can.
//---------------------------------------------------------------------------------------
class O3D_API TimeCounter
{
public:

	//! Default ctor
	//! @param check The time to check at each update, in seconds.
	TimeCounter(Float check) :
		m_lastTime(0),
		m_curTime(0),
		m_isUpdated(False),
		m_delta(0.f),
		m_checkTime(check) {}

	//! Update the time counter
	inline void update()
	{
		m_curTime = System::getTime();

		if (m_lastTime != 0)
			m_delta = (Float)(m_curTime - m_lastTime) / (Float)System::getTimeFrequency();
		else
			m_lastTime = m_curTime;

		if (m_delta >= m_checkTime)
		{
			m_lastTime = m_curTime;
			m_isUpdated = True;
		}
		else
			m_isUpdated = False;
	}

	//! Reset the timer
	inline void reset()
	{
		m_lastTime = m_curTime = 0;
		m_delta = 0.f;
		m_isUpdated = False;
	}

	//! return the time since the last update in seconds
	inline Float elapsed() const { return m_delta; }

	//! return the time since the last update in ms
	inline UInt32 elapsedMs() const { return UInt32(m_delta*1000.f); }

	//! set/get checking time (in seconds)
	inline void setCheckTime(Float check) { m_checkTime = check; }
	inline Float getCheckTime() const { return m_checkTime; }

	//! check if the elapsed time is greater or equal to check time
	inline Bool check() const { return m_isUpdated; }

protected:

	Int64 m_lastTime;
	Int64 m_curTime;
	Bool m_isUpdated;
	Float m_delta;
	Float m_checkTime;
};


//---------------------------------------------------------------------------------------
//! @class TimeCounterMs
//-------------------------------------------------------------------------------------
//! A millisecond precise time counter object. For more precision use O3DTimeCounter.
//---------------------------------------------------------------------------------------
class O3D_API TimeCounterMs
{
public:

	//! Default ctor
	//! @param check The time to check at each update, in milliseconds.
	TimeCounterMs(UInt32 check) :
		m_lastTime(0),
		m_curTime(0),
		m_isUpdated(False),
		m_delta(0),
		m_checkTime(check) {}

	//! Update the time counter
	inline void update()
	{
		m_curTime = System::getMsTime();

		if (m_lastTime != 0)
			m_delta = m_curTime - m_lastTime;
		else
			m_lastTime = m_curTime;

		if (m_delta >= m_checkTime)
		{
			m_lastTime = m_curTime;
			m_isUpdated = True;
		}
		else
			m_isUpdated = False;
	}

	//! Reset the timer
	inline void reset()
	{
		m_lastTime = m_curTime = 0;
		m_delta = 0;
		m_isUpdated = False;
	}

	//! return the time since the last update in seconds
	inline Float elapsed() const { return m_delta / 1000.f; }

	//! return the time since the last update in ms
	inline UInt32 elapsedMs() const { return m_delta; }

	//! set checking time (in millisecond)
	inline void setCheckTime(UInt32 check) { m_checkTime = check; }
	//! get checking time (in millisecond)
	inline UInt32 getCheckTime() const { return m_checkTime; }

	//! check if the elapsed time is greater or equal to check time
	inline Bool check() const { return m_isUpdated; }

protected:

	UInt32 m_lastTime;
	UInt32 m_curTime;
	Bool m_isUpdated;
	UInt32 m_delta;
	UInt32 m_checkTime;
};

//---------------------------------------------------------------------------------------
//! @class TimeMesure
//-------------------------------------------------------------------------------------
//! Helper to compute the duration of a block or function
//---------------------------------------------------------------------------------------
class O3D_API TimeMesure
{
public:

	//! Constructor.
	//! @param result Reference on a float, set to the duration of the block
	//! at the destructor of this class.
	TimeMesure(Float &result) :
		m_result(&result)
	{
		m_startTime = System::getTime();
	}

	//! Destructor.
	~TimeMesure()
	{
		*m_result = Float(System::getTime() - m_startTime) / System::getTimeFrequency();
	}

	//! Get the start timestamp in high precision.
	inline UInt64 getStartTime() const { return m_startTime; }

private:

	Int64 m_startTime;
	Float *m_result;
};

//---------------------------------------------------------------------------------------
//! @class Elapsed
//-------------------------------------------------------------------------------------
//! Helper to compute the elapsed time since a previous update with a precise time 
//! counter.
//---------------------------------------------------------------------------------------
class O3D_API Elapsed
{
public:

	//! Default constructor.
	Elapsed() :
		m_previousTime(0),
		m_elapsed(0.f)
	{
	}

	//! Update the elapsed time.
	inline Float update()
	{
		Int64 curTime = System::getTime();

		if (m_previousTime == 0)
			m_previousTime = curTime;

		m_elapsed = Float(curTime - m_previousTime) / System::getTimeFrequency();
		m_previousTime = curTime;

		return m_elapsed;
	}

	//! Reset the last time and elapsed to zero.
	inline void reset()
	{
		m_elapsed = 0.f;
		m_previousTime = 0;
	}

	//! Get the elapsed time since the last update.
	inline Float getElapsed() const { return m_elapsed; }

private:

	Int64 m_previousTime;
	Float m_elapsed;
};

} // namespace o3d

#endif // _O3D_OBJECTS_H

