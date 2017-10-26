/**
 * @file mousesmoother.cpp
 * @brief Implementation of MouseSmoother.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-11-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/vector2.h"
#include "o3d/core/mousesmoother.h"

using namespace o3d;

// build some spline value, with the given points and tangents
inline static Vector2f buildHermiteVector(
	const Vector2f &P0,
	const Vector2f &P1,
	const Vector2f &T0,
	const Vector2f &T1,
	Float lambda)
{
	Float lambda2 = lambda * lambda;
	Float lambda3 = lambda2 * lambda;
	Float h1 = 2 * lambda3 - 3 * lambda2 + 1;
	Float h2 = - 2 * lambda3 + 3 * lambda2;
	Float h3 = lambda3 - 2 * lambda2 + lambda;
	Float h4 = lambda3 - lambda2;

	return Vector2f(
                h1 * P0.x() + h2 * P1.x() + h3 * T0.x() + h4 * T1.x(),
                h1 * P0.y() + h2 * P1.y() + h3 * T0.y() + h4 * T1.y());
}

// default constructor
MouseSmoother::MouseSmoother(Float samplingPeriod)
{
	O3D_ASSERT(samplingPeriod > 0.f);

	m_samplingPeriod = samplingPeriod;
	m_isInit = False;
}

// Set the sampling period
void MouseSmoother::setSamplingPeriod(Float period)
{
	if (period == m_samplingPeriod)
		return;

	O3D_ASSERT(period > 0.f);

	reset();
	m_samplingPeriod = period;
}

// Sample pos, and return smoothed position
Vector2f MouseSmoother::samplePos(const Vector2f &realPos, Float date)
{
	if (!m_isInit)
	{
        m_sample[0] = m_sample[1] = m_sample[2] = m_sample[3] = Sample(date, realPos);
		m_deltaPos = m_oldPos = realPos;
		m_isInit = True;
	}
	else
	{
		// see if enough time has elapsed since last sample
		if (date - m_sample[3].Date >= m_samplingPeriod)
		{
			UInt32 numSamples = (UInt32)floor((date - m_sample[3].Date) / m_samplingPeriod);
            numSamples = o3d::min(numSamples, (UInt32)4);

			for (UInt32 i = 0; i < numSamples; ++i)
			{
				// add a new sample
				m_sample[0] = m_sample[1];
				m_sample[1] = m_sample[2];
				m_sample[2] = m_sample[3];
                m_sample[3] = Sample(date, realPos);
			}
		}
		else if (date == m_sample[3].Date)
		{
			// update current pos
            m_sample[3] = Sample(date, realPos);
		}
	}

    if ((m_sample[1].Pos.x() == m_sample[2].Pos.x()) && (m_sample[1].Pos.y() == m_sample[2].Pos.y()))
    {
        // if the pointer hasn't moved, allow a discontinuity of speed
        m_deltaPos.set(0.f, 0.f);
        m_oldPos = m_sample[2].Pos;
        return m_sample[2].Pos;
    }

    Float dt = m_sample[2].Date - m_sample[1].Date;
    if (dt == 0)
    {
        m_deltaPos = m_sample[2].Pos - m_oldPos;
        m_oldPos = m_sample[2].Pos;
        return m_sample[2].Pos;
    }

    Vector2f t0;
    if (m_sample[2].Date != m_sample[0].Date)
    {
        t0 = dt * (m_sample[2].Pos - m_sample[0].Pos) / (m_sample[2].Date - m_sample[0].Date);
    }

    Vector2f t1;
    if (m_sample[3].Date != m_sample[1].Date)
    {
        t1 = dt * (m_sample[3].Pos - m_sample[1].Pos) / (m_sample[3].Date - m_sample[1].Date);
    }

    Float evalDate = date - 2.f * m_samplingPeriod;
    o3d::clamp(evalDate, m_sample[1].Date, m_sample[2].Date);

    Vector2f result = buildHermiteVector(
        m_sample[1].Pos,
        m_sample[2].Pos,
        t0,
        t1,
        ((evalDate - m_sample[1].Date) / dt));

    m_deltaPos = result - m_oldPos;
	m_oldPos = result;

	return result;
}

