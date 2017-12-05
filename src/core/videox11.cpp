/**
 * @file videox11.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/base.h"

/* ONLY IF O3D_X11 IS SELECTED */
#ifdef O3D_X11

#include "o3d/core/application.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/video.h"
#include "o3d/core/debug.h"

#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/extensions/Xrandr.h>

using namespace o3d;

XF86VidModeModeInfo deskMode;

//void setFullScreen(Display *display, Window window, Bool fullScreen)
//{
//	XEvent event;
		
//	Atom _NET_WM_STATE = XInternAtom(
//			display,
//			"_NET_WM_STATE",
//			False);

//	Atom _NET_WM_STATE_FULLSCREEN = XInternAtom(
//			display,
//			"_NET_WM_STATE_FULLSCREEN",
//			False);

//	event.type = ClientMessage;
//	event.xclient.window = window;
//	event.xclient.message_type = _NET_WM_STATE;
//	event.xclient.format = 32;
//	event.xclient.data.l[0] = fullScreen ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
//	event.xclient.data.l[1] = _NET_WM_STATE_FULLSCREEN;
//	event.xclient.data.l[2] = 0;	// no second property to toggle
//	event.xclient.data.l[3] = 1;	// source indication: application
//	event.xclient.data.l[4] = 0;	// unused

//	XSendEvent(
//		display,
//		RootWindow(display, defaultScreen(display)),
//		0,
//		SubstructureRedirectMask | SubstructureNotifyMask,
//		&event);
//}

// change display mode
void Video::setDisplayMode(AppWindow *appWindow, CIT_VideoModeList mode)
{
    if (mode == m_modes.end()) {
		O3D_ERROR(E_InvalidParameter("Invalid display mode"));
    }

    if (!appWindow || !appWindow->isSet()) {
		O3D_ERROR(E_InvalidParameter("Invalid application window"));
    }

	Display *display = reinterpret_cast<Display*>(Application::getDisplay());
    int screen = DefaultScreen(display);

    XRRScreenConfiguration *screenConfig = XRRGetScreenInfo(display, RootWindow(display, screen));

    // first try with set screen config XRR 1.2
    Status result = XRRSetScreenConfigAndRate(
                 display,
                 screenConfig,
                 RootWindow(display, screen),
                 static_cast<int>(mode->index),
                 m_savedRotation,
                 static_cast<short>(mode->freq),
                 CurrentTime);

    XRRFreeScreenConfigInfo(screenConfig);

    // second try with set screen size XRR 1.1 and compatible with moderns WM
    if (result != Success) {
        XRRSetScreenSize(
                    display,
                    RootWindow(display, screen),
                    mode->width,
                    mode->height,
                    m_desktop->mmWidth,
                    m_desktop->mmHeight);

        // see ApplicationX11 : X11 error handler
        XSync(display, False);
    }

    // all tentatives failed
    if (Application::isDisplayError()) {
		String str;
		str << mode->width << 'x' << mode->height << ' ' <<
			mode->bpp << String("bpp ") << mode->freq << String("Hz");

		//SetFullScreen(display, static_cast<Window>(appWindow->getHWND()), False);
        O3D_ERROR(E_InvalidVideoMode(String("Unable to set display mode (") + str + ")"));
    }

//    if (appWindow != m_appWindow) {
//		if (m_appWindow) {
//			setFullScreen(display, static_cast<Window>(m_appWindow->getHWND()), False);
//      }
//		setFullScreen(display, static_cast<Window>(appWindow->getHWND()), True);
//	}

//	UInt32 modeRate;
//	XF86VidModeModeInfo **modes;
//	XF86VidModeGetAllModeLines(display, screen, &numSizes, &modes);

//	if (freq == 0) {
//		freq = *m_desktop.freqList.begin();
//  }

//	int size = -1
//  for (i = 0; i < numSizes; ++i) {
//      bpp = (UInt32)DefaultDepth(display, screen);

//    	// Compute the displays refresh rate, dotclock comes in kHz.
//    	modeRate = UInt32((modes[i]->dotclock * 1000 ) / (modes[i]->htotal * modes[i]->vtotal));

//		if ((modes[i]->hdisplay == mode->width) && (modes[i]->vdisplay == mode->height) &&
//			(modeRate == freq) && (mode->bpp == bpp)) {
//			break;
//		}
//	}

//    if (size < numSizes) {
//    	Bool result = XF86VidModeSwitchToMode(display, screen, modes[size]);
//    	if (result == True) {
//    		result = XF86VidModeSetViewPort(display, screen, 0, 0);
//      }

//    	if (result == False) {
//    		String str;
//    		str << mode->width << 'x' << mode->height << ' ' <<
//    			mode->bpp << O3DString("bpp ") << freq << O3DString("Hz");

//    		XFree(modes);
//          O3D_ERROR(E_InvalidVideoMode(O3DString("Unable to set display mode (") + str + ")"));
//    	}
//    } else {
//		String str;
//		str << mode->width << 'x' << mode->height << ' ' <<
//			mode->bpp << O3DString("bpp ") << freq << O3DString("Hz");

//		XFree(modes);
//      O3D_ERROR(E_InvalidVideoMode(String("Unsupported display mode (") + str + ")"));
//	}

//    XFree(modes);

	XMapRaised(display, static_cast<Window>(appWindow->getHWND()));

	m_appWindow = appWindow;
	m_currentMode = mode;
}

// restore to desktop display mode
void Video::restoreDisplayMode()
{
    if (m_currentMode != m_desktop) {
        Display *display = reinterpret_cast<Display*>(Application::getDisplay());
        int screen = DefaultScreen(display);

//        XF86VidModeSwitchToMode(display, screen, &deskMode);
//        XF86VidModeSetViewPort(display, screen, 0, 0);
//        XFlush(display);

        XRRScreenConfiguration *screenConfig = XRRGetScreenInfo(display, RootWindow(display, screen));
        Status result;

        // first try with set screen config XRR 1.2
        result = XRRSetScreenConfigAndRate(
                     display,
                     screenConfig,
                     RootWindow(display, screen),
                     m_savedModeId,
                     m_savedRotation,
                     m_savedRate,
                     CurrentTime);

        XRRFreeScreenConfigInfo(screenConfig);

        // second try with set screen size XRR 1.1 and compatible with moderns WM
        if (result != Success) {
            XRRSetScreenSize(
                        display,
                        RootWindow(display, screen),
                        m_desktop->width,
                        m_desktop->height,
                        m_desktop->mmWidth,
                        m_desktop->mmHeight);

            // see ApplicationX11 : X11 error handler
            XSync(display, False);
        }

        // all tentatives failed
        if (Application::isDisplayError()) {
            String str;
            str << m_desktop->width << 'x' << m_desktop->height << ' ' <<
                   m_desktop->bpp << String("bpp ") << m_desktop->freq << String("Hz");

            O3D_ERROR(E_InvalidVideoMode(String("Unable to restore desktop display mode (") + str + ")"));
        }

		m_currentMode = m_desktop;
	}

    if (m_appWindow) {
        // setFullScreen(display, static_cast<Window>(m_appWindow->getHWND()), False);
        m_appWindow = nullptr;
	}
}

// list all display mode for selected device
void Video::listDisplayModes()
{
	Display *display = reinterpret_cast<Display*>(Application::getDisplay());

	int majorV, minorV;
    if (!XRRQueryVersion(display, &majorV, &minorV)) {
		O3D_ERROR(E_InvalidPrecondition("XRandR is not available"));
    }

    int screen = DefaultScreen(display);
    XRRScreenConfiguration *screenConfig = XRRGetScreenInfo(display, RootWindow(display, screen));

    int numSizes, numRates;
    XRRScreenSize *modes;
    short *rates;

	// retrieve the list of resolutions
    modes = XRRConfigSizes(screenConfig, &numSizes);
    if (numSizes > 0) {
		int i, j;
        for (i = 0; i < numSizes; ++i) {
			VideoMode videoMode;

			videoMode.index = i;
			videoMode.width = (UInt32)modes[i].width;
			videoMode.height = (UInt32)modes[i].height;
            videoMode.mmWidth = (UInt32)modes[i].mwidth;
            videoMode.mmHeight = (UInt32)modes[i].mheight;
			videoMode.bpp = (UInt32)DefaultDepth(display, screen);

            rates = XRRConfigRates(screenConfig, i, &numRates);
            for (j = 0; j < numRates; ++j) {
				videoMode.freq = (UInt32)rates[j];
				m_modes.push_back(videoMode);
			}
		}

		// desktop mode
        m_savedModeId = XRRConfigCurrentConfiguration(screenConfig, &m_savedRotation);
		m_savedRate = XRRConfigCurrentRate(screenConfig);

		VideoMode desktop;

		desktop.width = modes[m_savedModeId].width;
		desktop.height = modes[m_savedModeId].height;
        desktop.mmWidth = modes[m_savedModeId].mwidth;
        desktop.mmHeight = modes[m_savedModeId].mheight;
		desktop.bpp = (UInt32)DefaultDepth(display, screen);
		desktop.freq = (UInt32)m_savedRate;

		m_desktop = m_currentMode = findDisplayMode(desktop);
	}

    XRRFreeScreenConfigInfo(screenConfig);
/*
	XF86VidModeModeInfo **modes;

	XF86VidModeGetAllModeLines(display, screen, &numSizes, &modes);

	// save desktop-resolution before switching modes
    deskMode = *modes[0];

	m_desktop.width = deskMode.hdisplay;
	m_desktop.height = deskMode.vdisplay;
	m_desktop.bpp = (UInt32)DefaultDepth(display, screen);

	// Compute the displays refresh rate, dotclock comes in kHz for the desktop mode
	m_desktop.freqList.insert(UInt32((deskMode.dotclock * 1000 ) /
			(deskMode.htotal * deskMode.vtotal)));

    for (int i = 0; i < numSizes; ++i)
	{
		if (modes[i]->vdisplay >= 400)
		{
			O3D_IT_VideoModeList it = m_modes.begin();
			while (it != m_modes.end())
			{
				if ((it->width == modes[i]->hdisplay) &&
				    (it->height == modes[i]->vdisplay))
					break;
                ++it;
			}

			// Compute the displays refresh rate, dotclock comes in kHz.
			UInt32 freq =  UInt32((modes[i]->dotclock * 1000 ) /
					(modes[i]->htotal * modes[i]->vtotal));

			if (it != m_modes.end())
			{
				it->freqList.insert(freq);
			}
			else
			{
				O3DVideoMode videoMode;

				videoMode.width = modes[i]->hdisplay;
				videoMode.height = modes[i]->vdisplay;
				videoMode.bpp = (UInt32)DefaultDepth(display, screen);

				videoMode.freqList.insert(freq);

				m_modes.push_back(videoMode);
			}
		}
	}

    XFree(modes);*/
}

#endif // O3D_X11
