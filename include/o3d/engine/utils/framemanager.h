/**
 * @file framemanager.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FRAMEMANAGER_H
#define _O3D_FRAMEMANAGER_H

#include "o3d/core/base.h"
#include "o3d/core/memorydbg.h"
#include "../enginetype.h"

namespace o3d {

class Scene;

/**
 * @brief Frame manager measurement.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2003-01-07
 * Compute statistics on scene update and display.
 */
class O3D_API FrameManager
{
public:

	//! Number of frame time to record to compute an FPS.
	static const UInt32 FPS_INTERVAL_SIZE = 20;
	//! Number of FPS to record to done the ReportLog.
	static const UInt32 FPS_MAX_INTERVAL = 60;

	//! Default constructor.
    FrameManager();

	//! Update information (need to be called on each frame display).
	void update(Float prevDisplayDuration);

	//! Log information about the last frames.
	void logInfo() const;

	//! Get the total number of rendered frames.
	inline UInt64 getTotalFrame() const { return m_totalFrame; }

	//! Compute the time elapsed since the last call of this method.
	//! Get the result with GetFrameDuration().
	void computeFrameDuration();

	//! Get the last frame duration in seconds.
	inline Float getFrameDuration() { return m_frameDuration; }

	//! Add a number of drawn primitive to this frame.
	//! @param primitive Type of the drawn primitives.
	//! @param count Number of drawn indices.
	//! @note The number of triangles or lines is correctly computed.
	void addPrimitives(PrimitiveFormat primitive, UInt32 num);

	//! Get the number of drawn triangles for the last frame.
	UInt32 getNumTriangles() const;

	//! Get the number of drawn lines for the last frame.
	UInt32 getNumLines() const;

	//! Get the number of drawn point for the last frame.
	UInt32 getNumPoints() const;

protected:

	struct Frame
	{
		Float duration;
		UInt32 numTris;
		UInt32 numLines;
		UInt32 numPoints;
	};

	Frame m_framesList[FPS_MAX_INTERVAL];  //!< Last computed frames.

	UInt32 m_numTris;       //!< Current number of triangles.
	UInt32 m_numLines;      //!< Current number of lines.
	UInt32 m_numPoints;     //!< Current number of points.

	UInt32 m_interval;		//!< Number of frame elapsed in the interval.

	UInt32 m_frame;			//!< Number of elapsed frame.
	Int64 m_lastTime;		//!< Time since the last frame.

	UInt64 m_totalFrame;    //!< Number of rendered frames.

	Float m_frameDuration;	//!< Duration of the last frame in seconds.
    Int64 m_frameLastTime;	//!< Time at the previous computeFrameDuration().
    Int64 m_frameNewTime;	//!< Time at the last computeFrameDuration().
};

} // namespace o3d

#endif // _O3D_FRAMEMANAGER_H

