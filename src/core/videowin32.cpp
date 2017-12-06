/**
 * @file videowin32.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/base.h"

/* ONLY IF O3D_WIN32 IS SELECTED */
#ifdef O3D_WIN32

#include "o3d/core/video.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/debug.h"
#include "o3d/core/architecture.h"

using namespace o3d;

// change display mode
void Video::setDisplayMode(AppWindow *appWindow, CIT_VideoModeList mode)
{
    if (mode == m_modes.end()) {
		O3D_ERROR(E_InvalidParameter("Invalid video mode"));
    }

    if (!appWindow || !appWindow->isSet()) {
		O3D_ERROR(E_InvalidParameter("Invalid application window"));
    }

	RECT rect;
	DEVMODE VideoMode;

	rect.top    = 0;
	rect.left   = 0;
	rect.right  = mode->width;
	rect.bottom = mode->height;

	VideoMode.dmSize = sizeof(VideoMode);

	EnumDisplaySettings(NULL, mode->index, &VideoMode);
	//VideoMode.dmFields = DM_BITSPERPEL | DM_PELSHEIGHT | DM_PELSWIDTH;
	//VideoMode.dmFields |= DM_DISPLAYFREQUENCY;

    if (ChangeDisplaySettings(&VideoMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
		String str;
		str << mode->width << 'x' << mode->height << ' ' <<
			mode->bpp << String("bpp ") << mode->freq << String("Hz");

		O3D_ERROR(E_InvalidParameter("Unsupported video mode (" + str + ")"));
	}

	AdjustWindowRectEx(&rect, WS_POPUP, FALSE, WS_EX_APPWINDOW);

	m_appWindow = appWindow;
	m_currentMode = mode;
}

// restore to desktop display mode
void Video::restoreDisplayMode()
{
    if (m_currentMode != m_desktop) {
		ChangeDisplaySettings(NULL, 0);
		m_currentMode = m_desktop;
	}

	m_appWindow = NULL;
}

// list all display mode for selected device
void Video::listDisplayModes()
{
	// store desktop video mode
	DEVMODE videoMode;
	videoMode.dmSize = sizeof(VideoMode);

	// list display mode
	Int32 i = 0;

    while (EnumDisplaySettings(NULL,i++,&videoMode)) {
        // list only 16 or 32bpp video mode
        if (((videoMode.dmBitsPerPel == 16) || (videoMode.dmBitsPerPel == 32))) {
			VideoMode videomode;

			videomode.index = i-1;
			videomode.width = (UInt32)videoMode.dmPelsWidth;
			videomode.height = (UInt32)videoMode.dmPelsHeight;
			videomode.bpp = (UInt32)videoMode.dmBitsPerPel;
			videomode.freq = (UInt32)videoMode.dmDisplayFrequency;

			IT_VideoModeList comp_it = m_modes.begin();

			// sort by BPP (32>16)
            while ((comp_it != m_modes.end()) && (comp_it->bpp > videoMode.dmBitsPerPel)) {
				++comp_it;
			}

			// sort by resolution (1024x768>800x600)
			while ((comp_it != m_modes.end()) &&
                   ((comp_it->width > (UInt32)(videoMode.dmPelsWidth)) &&
                   (comp_it->height > (UInt32)(videoMode.dmPelsHeight)))) {

					++comp_it;
			}

			m_modes.insert(comp_it,videomode);
		}
	}

	// desktop mode
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &videoMode);

	VideoMode desktop;

	desktop.width = videoMode.dmPelsWidth;
	desktop.height = videoMode.dmPelsHeight;
	desktop.bpp = videoMode.dmBitsPerPel;
	desktop.freq = (UInt32)videoMode.dmDisplayFrequency;

	m_desktop = m_currentMode = findDisplayMode(desktop);
}

#endif // O3D_WIN32
