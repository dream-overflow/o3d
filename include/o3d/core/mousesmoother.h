/**
 * @file mousesmoother.h
 * @brief Mouse input smoother using spline
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-11-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MOUSESMOOTHER_H
#define _O3D_MOUSESMOOTHER_H

#include "base.h"
#include "memorydbg.h"

#include "vector2.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class MouseSmoother
//-------------------------------------------------------------------------------------
//! Mouse input smoother, based on a spline.
//! The mouse coordinates are sampled at the given period. Increase the period to obtain
//! a greater smoothing.
//! A delay of 2*m_SamplingPeriod between the user moving the mouse and the pointer
//! reaching the wanted position.
//---------------------------------------------------------------------------------------
class O3D_API MouseSmoother
{
public:

	//! default constructor. take a sampling period
	MouseSmoother(Float samplingPeriod = 0.05f);

	//! Set the sampling period
	//! @note It reset the smoother
	void setSamplingPeriod(Float period);

	//! Get the sampling period
	inline Float getSamplingPeriod() const { return m_samplingPeriod; }

	//! Reset smoother. The next returned position will be the exact position of mouse (no smoothing is done)
	inline void reset() { m_isInit = False; }

	//! Return true if no sampling has occurred
	inline Bool isReseted() const { return !m_isInit; }

	//! Sample pos, and return smoothed position
	Vector2f samplePos(const Vector2f &realPos, Float date);

	//! Get the delta smoothed position
	inline const Vector2f& getDeltaPos() const { return m_deltaPos; }

	//! Get the last smoothed position
	inline const Vector2f& getLastPos() const { return m_oldPos; }

private:

	//! A sample of mouse position
	class Sample
	{
	public:

		Float Date;
		Vector2f Pos;

		//! Default ctor
		Sample() {}

		//! ctor with pos and date
		Sample(Float date, const Vector2f &pos) :
			Date(date), Pos(pos) {}
	};

	Float m_samplingPeriod;
	Bool m_isInit;

	Sample m_sample[4];   //!< 4 samples needed (0&2 for tangent at sample 1, 1&3 for tangent at sample 2)

	Vector2f m_oldPos;
	Vector2f m_deltaPos;
};

} // namespace o3d

#endif // _O3D_MOUSESMOOTHER_H

