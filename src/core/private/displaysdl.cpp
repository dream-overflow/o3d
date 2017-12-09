/**
 * @file displaysdl.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/base.h"

/* ONLY IF O3D_SDL2 IS SELECTED */
#ifdef O3D_SDL2

#include "o3d/core/appwindow.h"
#include "o3d/core/display.h"
#include "o3d/core/debug.h"

#include <SDL2/SDL.h>

#include "o3d/core/architecture.h"

using namespace o3d;

// change display mode
void Display::setDisplayMode(AppWindow *appWindow, CIT_VideoModeList mode)
{
    if (mode == m_modes.end()) {
		O3D_ERROR(E_InvalidParameter("Invalid display mode"));
    }

    if (!appWindow || !appWindow->isSet()) {
		O3D_ERROR(E_InvalidParameter("Invalid application window"));
    }

	SDL_DisplayMode displayMode;
	int display = 0;

    if (SDL_GetDisplayMode(display, mode->index, &displayMode) != 0) {
		String str;
		str << mode->width << 'x' << mode->height << ' ' <<
			mode->bpp << String("bpp ") << mode->freq << String("Hz");

		O3D_ERROR(E_InvalidParameter(String("Unsupported display mode (") + str + ")"));
	}

	SDL_Window *appWindowSDL = reinterpret_cast<SDL_Window*>(appWindow->getHDC());

    if (m_appWindow != appWindow) {
        if (m_appWindow) {
			SDL_Window *windowSDL = reinterpret_cast<SDL_Window*>(m_appWindow->getHDC());
			SDL_SetWindowFullscreen(windowSDL, SDL_FALSE);
		}

		SDL_SetWindowFullscreen(appWindowSDL, SDL_TRUE);
	}

    if (SDL_SetWindowDisplayMode(appWindowSDL, &displayMode) != 0) {
		String str;
		str << mode->width << 'x' << mode->height << ' ' <<
			mode->bpp << String("bpp ") << mode->freq << String("Hz");

		SDL_SetWindowFullscreen(appWindowSDL, SDL_FALSE);
		O3D_ERROR(E_InvalidParameter(String("Unable to set display mode (") + str + ")"));
	}

	m_appWindow = appWindow;
	m_currentMode = mode;
}

// restore to desktop display mode
void Display::restoreDisplayMode()
{
    if (m_currentMode != m_desktop) {
		m_currentMode = m_desktop;
	}

    if (m_appWindow) {
		SDL_Window *windowSDL = reinterpret_cast<SDL_Window*>(m_appWindow->getHDC());
		SDL_SetWindowFullscreen(windowSDL, SDL_FALSE);
        m_appWindow = nullptr;
	}
}

// list all display mode for selected device
void Display::listDisplayModes()
{
	SDL_DisplayMode mode;
	int display = 0;
	Int32 numMode = SDL_GetNumDisplayModes(display);

    for (Int32 i = 0; i < numMode; ++i) {
		SDL_GetDisplayMode(display, i, &mode);

		// list only display mode with 16 or 32 bpp and height greater than pixels
		/*if ((mode.format == UInt32(SDL_PIXELFORMAT_ARGB8888)) ||
			 (mode.format == UInt32(SDL_PIXELFORMAT_RGBA8888)) ||
			 (mode.format == UInt32(SDL_PIXELFORMAT_ABGR8888)) ||
			 (mode.format == UInt32(SDL_PIXELFORMAT_BGRA8888)) ||
			 (mode.format == UInt32(SDL_PIXELFORMAT_ARGB4444)) ||
             (mode.format == UInt32(SDL_PIXELFORMAT_ABGR4444)))*/ {
			UInt32 bpp = SDL_BITSPERPIXEL(mode.format);

			VideoMode videoMode;

			videoMode.index = i;
			videoMode.width = mode.w;
			videoMode.height = mode.h;
			videoMode.bpp = bpp;
			videoMode.freq = (UInt32)mode.refresh_rate;

			m_modes.push_back(videoMode);
		}
	}

	// desktop mode
	SDL_GetDesktopDisplayMode(display, &mode);

	VideoMode desktop;

	desktop.width = mode.w;
	desktop.height = mode.h;
	desktop.bpp = SDL_BITSPERPIXEL(mode.format);
	desktop.freq = (UInt32)mode.refresh_rate;

	m_desktop = m_currentMode = findDisplayMode(desktop);
}

#endif // O3D_SDL2
