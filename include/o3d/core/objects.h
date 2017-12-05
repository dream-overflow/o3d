/**
 * @file objects.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_OBJECTS_H
#define _O3D_OBJECTS_H

#include "string.h"

namespace o3d {

/**
 * @brief A precise time counter object. It count as precise as the hardware can.
 */
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

        if (m_lastTime != 0) {
			m_delta = (Float)(m_curTime - m_lastTime) / (Float)System::getTimeFrequency();
        } else {
			m_lastTime = m_curTime;
        }

        if (m_delta >= m_checkTime) {
			m_lastTime = m_curTime;
			m_isUpdated = True;
        } else {
			m_isUpdated = False;
        }
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

/**
 * @brief A millisecond precise time counter object. For more precision use TimeCounter.
 */
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

        if (m_lastTime != 0) {
			m_delta = m_curTime - m_lastTime;
        } else {
			m_lastTime = m_curTime;
        }

        if (m_delta >= m_checkTime) {
			m_lastTime = m_curTime;
			m_isUpdated = True;
        } else {
			m_isUpdated = False;
        }
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

/**
 * @brief  Helper to compute the elapsed time since a previous update with a precise time
 * counter.
 */
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

        if (m_previousTime == 0) {
			m_previousTime = curTime;
        }

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
