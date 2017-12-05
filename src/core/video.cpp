/**
 * @file video.cpp
 * @brief Implementation of Video.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-08-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/architecture.h"
#include "o3d/core/video.h"

using namespace o3d;

Video* Video::m_instance = 0;

// constructor
Video::Video() :
    m_appWindow(nullptr),
	m_desktopFreq(0)
#ifdef O3D_X11
    ,m_savedRotation(0),
	m_savedModeId(0),
	m_savedRate(0)
#endif
{
	m_desktop = m_modes.end();
	listDisplayModes();
	m_currentMode = m_desktop;
}

// destructor
Video::~Video()
{
	restoreDisplayMode();
}

// Singleton instantiation
Video* Video::instance()
{
    if (!m_instance) {
		m_instance = new Video();
    }

    return m_instance;
}

// Singleton destruction
void Video::destroy()
{
    if (m_instance) {
		delete m_instance;
        m_instance = nullptr;
	}
}

// Get current display ratio (width/height).
Float Video::getCoef()const
{
	return Float(getWidth()) / getHeight();
}

// get display mode string for n
String Video::getDisplayModeString(CIT_VideoModeList mode)
{
	return String::print("%ux%u %ubpp %uhz", mode->width, mode->height, mode->bpp, mode->freq);
}

// find a display mode by string
CIT_VideoModeList Video::findDisplayMode(const String &mode) const
{
	VideoMode videomode;

    if (mode.isValid()) {
		Int32 pos;

		videomode.width = mode.toUInt32(0);

		pos = mode.find('x',0);
        if (pos == -1) {
			return m_modes.end();
        } else {
			videomode.height = mode.toUInt32(pos+1);
        }

		pos = mode.find(' ',0);
        if (pos == -1) {
			return m_modes.end();
        } else {
			videomode.bpp = mode.toUInt32(pos+1);
        }

		pos = mode.reverseFind(' ');
        if (pos == -1) {
			videomode.freq = 0;
        } else {
			videomode.freq = mode.toUInt32(pos+1);
        }

		return findDisplayMode(videomode);
    } else {
		return m_modes.end();
    }
}

CIT_VideoModeList Video::findDisplayMode(const VideoMode &mode) const
{
    if (mode.freq == 0) {
		UInt32 freq = 0;
		CIT_VideoModeList result = m_modes.end();

        for (CIT_VideoModeList cit = m_modes.begin(); cit != m_modes.end(); ++cit) {
            if ((cit->width == mode.width) && (cit->height == mode.height) &&
                (cit->bpp == mode.bpp) && (cit->freq > freq)) {

				freq = cit->freq;
				result = cit;
			}
		}

		return result;
    } else {
        for (CIT_VideoModeList cit = m_modes.begin(); cit != m_modes.end(); ++cit) {
            if ((cit->width == mode.width) && (cit->height == mode.height) &&
                (cit->bpp == mode.bpp) && (cit->freq == mode.freq)) {

				return cit;
			}
		}
	}

	// not found !
	return m_modes.end();
}
