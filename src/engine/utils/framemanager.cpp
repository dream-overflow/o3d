/**
 * @file framemanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/utils/framemanager.h"

#include <math.h>

#include "o3d/core/debug.h"
#include "o3d/core/string.h"
#include "o3d/core/architecture.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/enginetype.h"

using namespace o3d;

// Constructor
FrameManager::FrameManager() :
	m_numTris(0),
	m_numLines(0),
	m_numPoints(0),
	m_interval(0),
	m_frame(0),
	m_lastTime(0),
	m_totalFrame(0),
	m_frameDuration(0),
	m_frameLastTime(0)
{
}

// update frame information
void FrameManager::update(Float prevDisplayDuration)
{
	if (m_frame == 0 && m_interval == 0)
	{
		// initialization
		m_totalFrame = 0;
		m_lastTime = System::getTime();
	}

	++m_frame;
	++m_totalFrame;

	if (m_frame == FPS_INTERVAL_SIZE)
	{
		Int64 time = System::getTime();

		m_framesList[m_interval].duration = prevDisplayDuration;
		m_framesList[m_interval].numLines = m_numLines;
		m_framesList[m_interval].numPoints = m_numPoints;
		m_framesList[m_interval].numTris = m_numTris;

		m_lastTime = time;
		++m_interval;
		m_frame = 0;

		if (m_interval == FPS_MAX_INTERVAL)
		{
			m_interval = 1;
			m_framesList[0].duration = m_framesList[FPS_MAX_INTERVAL-1].duration;
			m_framesList[0].numLines = m_framesList[FPS_MAX_INTERVAL-1].numLines;
			m_framesList[0].numPoints = m_framesList[FPS_MAX_INTERVAL-1].numPoints;
			m_framesList[0].numTris = m_framesList[FPS_MAX_INTERVAL-1].numTris;
		}
	}

	// start with 0 triangles, 0 lines, and 0 points
	m_numTris = 0;
	m_numLines = 0;
	m_numPoints = 0;
}

// Log information about the last frames.
void FrameManager::logInfo() const
{
    String str("The last ");

	str.concat(m_interval);
	str += " frames :\n";

	for (UInt32 i = 0; i < m_interval; ++i)
	{
		str += String::print
                ("    |- %i -> duration(%.4f ms) tris(%i)/lines(%i)/points(%i)\n",
				i,
				m_framesList[i].duration,
				m_framesList[i].numTris,
				m_framesList[i].numLines,
				m_framesList[i].numPoints);
	}

	O3D_MESSAGE(str);
}

// Compute the duration of the frame in seconds.
void FrameManager::computeFrameDuration()
{
 	Int64 time = System::getTime();
	
	if (m_frameLastTime == 0)
		m_frameDuration = 0.0f;
	else
		m_frameDuration = Float(time - m_frameLastTime) / System::getTimeFrequency();

	m_frameLastTime = time;
}

// Add a number of drawn primitive to this frame.
void FrameManager::addPrimitives(PrimitiveFormat primitive, UInt32 count)
{
	switch (primitive)
	{
		case P_TRIANGLES:
			m_numTris += count / 3;
			break;
		case P_TRIANGLE_STRIP:
			m_numTris += count - 2;
			break;
		case P_TRIANGLE_FAN:
			m_numTris += count - 2;
			break;
		case P_LINES:
			m_numLines += count / 2;
			break;
		case P_LINE_LOOP:
			m_numLines += count;
			break;
		case P_LINE_STRIP:
			m_numLines += count - 1;
			break;
		case P_POINTS:
			m_numPoints += 1;
			break;
		default:
			break;
	}
}

// Get the number of drawn triangles for the last frame.
UInt32 FrameManager::getNumTriangles() const
{
	if (m_interval != 0)
		return m_framesList[m_interval-1].numTris;
	else
		return 0;
}

// Get the number of drawn lines for the last frame.
UInt32 FrameManager::getNumLines() const
{
	if (m_interval != 0)
		return m_framesList[m_interval-1].numLines;
	else
		return 0;
}

// Get the number of drawn point for the last frame.
UInt32 FrameManager::getNumPoints() const
{
	if (m_interval != 0)
		return m_framesList[m_interval-1].numPoints;
	else
		return 0;
}
