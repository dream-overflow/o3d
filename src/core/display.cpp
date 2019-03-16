/**
 * @file video.cpp
 * @brief Implementation of display.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-08-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/architecture.h"
#include "o3d/core/display.h"

using namespace o3d;

Display* Display::m_instance = nullptr;

// constructor
Display::Display() :
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
Display::~Display()
{
	restoreDisplayMode();
}

// Singleton instantiation
Display* Display::instance()
{
    if (!m_instance) {
        m_instance = new Display();
    }

    return m_instance;
}

// Singleton destruction
void Display::destroy()
{
    if (m_instance) {
		delete m_instance;
        m_instance = nullptr;
	}
}

// Get current display ratio (width/height).
Float Display::getCoef()const
{
	return Float(getWidth()) / getHeight();
}

// get display mode string for n
String Display::getDisplayModeString(CIT_DisplayModeList mode)
{
	return String::print("%ux%u %ubpp %uhz", mode->width, mode->height, mode->bpp, mode->freq);
}

// find a display mode by string
CIT_DisplayModeList Display::findDisplayMode(const String &mode) const
{
    DisplayMode videomode;

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

CIT_DisplayModeList Display::findDisplayMode(const DisplayMode &mode) const
{
    if (mode.freq == 0) {
		UInt32 freq = 0;
        CIT_DisplayModeList result = m_modes.end();

        for (CIT_DisplayModeList cit = m_modes.begin(); cit != m_modes.end(); ++cit) {
            if ((cit->width == mode.width) && (cit->height == mode.height) &&
                (cit->bpp == mode.bpp) && (cit->freq > freq)) {

				freq = cit->freq;
				result = cit;
			}
		}

		return result;
    } else {
        for (CIT_DisplayModeList cit = m_modes.begin(); cit != m_modes.end(); ++cit) {
            if ((cit->width == mode.width) && (cit->height == mode.height) &&
                (cit->bpp == mode.bpp) && (cit->freq == mode.freq)) {

				return cit;
			}
		}
	}

	// not found !
	return m_modes.end();
}
