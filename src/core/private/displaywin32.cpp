/**
 * @file displaywin32.cpp
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

#include "o3d/core/display.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/debug.h"
#include "o3d/core/architecture.h"

using namespace o3d;

// change display mode
void Display::setDisplayMode(AppWindow *appWindow, CIT_DisplayModeList mode)
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
void Display::restoreDisplayMode()
{
    if (m_currentMode != m_desktop) {
		ChangeDisplaySettings(NULL, 0);
		m_currentMode = m_desktop;
	}

    m_appWindow = nullptr;
}

// list all display mode for selected device
void Display::listDisplayModes()
{
	// store desktop video mode
	DEVMODE videoMode;
    videoMode.dmSize = sizeof(DEVMODE);

	// list display mode
	Int32 i = 0;

    while (EnumDisplaySettings(NULL,i++,&videoMode)) {
        // list only 16 or 32bpp video mode
        if (((videoMode.dmBitsPerPel == 16) || (videoMode.dmBitsPerPel == 32))) {
            DisplayMode displayMode;

            displayMode.index = i-1;
            displayMode.width = (UInt32)videoMode.dmPelsWidth;
            displayMode.height = (UInt32)videoMode.dmPelsHeight;
            displayMode.bpp = (UInt32)videoMode.dmBitsPerPel;
            displayMode.freq = (UInt32)videoMode.dmDisplayFrequency;

            IT_DisplayModeList comp_it = m_modes.begin();

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

            m_modes.insert(comp_it, displayMode);
		}
	}

	// desktop mode
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &videoMode);

    DisplayMode desktop;

	desktop.width = videoMode.dmPelsWidth;
	desktop.height = videoMode.dmPelsHeight;
	desktop.bpp = videoMode.dmBitsPerPel;
	desktop.freq = (UInt32)videoMode.dmDisplayFrequency;

	m_desktop = m_currentMode = findDisplayMode(desktop);
}

#endif // O3D_WIN32
