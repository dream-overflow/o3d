/**
 * @file appwindowx11.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/appwindow.h"

// ONLY IF O3D_X11 IS SELECTED
#ifdef O3D_X11

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include "o3d/core/gl.h"

#include "o3d/core/private/glxdefines.h"
#include "o3d/core/private/glx.h"

#include "o3d/core/debug.h"
#include "o3d/core/application.h"
#include "o3d/core/display.h"

#ifdef O3D_EGL
  #include "o3d/core/private/egldefines.h"
  #include "o3d/core/private/egl.h"
#endif

#undef Bool

using namespace o3d;

// Atoms
// WM_DELETE_WINDOW
// _NET_WM_STATE
// _NET_WM_STATE_HIDDEN
// _NET_WM_STATE_MAXIMIZED_VERT
// _NET_WM_STATE_MAXIMIZED_HORZ
// _NET_WM_STATE_FULLSCREEN
// _NET_WM_NAME
// _NET_WM_ICON_NAME
// _NET_WM_ICON
// UTF8_STRING

static Atom XA_UTF8_STRING = 0;
static Atom XA_CLIPBOARD = 0;
static Atom XA_COMPOUND_TEXT = 0;
static Atom XA_TARGETS = 0;
static Atom _NET_WM_STATE = 0;
static Atom _NET_WM_STATE_FULLSCREEN = 0;
static Atom _NET_WM_STATE_MAXIMIZED_VERT = 0;
static Atom _NET_WM_STATE_MAXIMIZED_HORZ = 0;
static Atom WM_HINTS = 0;
static Atom _NET_WM_WINDOW_TYPE = 0;
static Atom _NET_WM_WINDOW_TYPE_NORMAL = 0;
static Atom _NET_WM_ICON = 0;
static Atom _NET_WM_PID = 0;
static Atom WM_DELETE_WINDOW = 0;

static void initAtoms(::Display *display)
{
    Bool atomInitialized = False;

    if (atomInitialized) {
        return;
    }

    XA_UTF8_STRING = XInternAtom(display, "UTF8_STRING", True);
    XA_CLIPBOARD = XInternAtom(display, "CLIPBOARD", True);
    XA_COMPOUND_TEXT = XInternAtom(display, "COMPOUND_TEXT", True);
    XA_TARGETS = XInternAtom(display, "TARGETS", True);

    _NET_WM_STATE = XInternAtom(display, "_NET_WM_STATE", False);
    _NET_WM_STATE_FULLSCREEN = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
    _NET_WM_STATE_MAXIMIZED_VERT = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
    _NET_WM_STATE_MAXIMIZED_HORZ = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    _NET_WM_WINDOW_TYPE = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    _NET_WM_WINDOW_TYPE_NORMAL = XInternAtom(display, "_NET_WM_WINDOW_TYPE_NORMAL", False);
    _NET_WM_ICON = XInternAtom(display, "_NET_WM_ICON", False);
    _NET_WM_PID = XInternAtom(display, "_NET_WM_PID", False);

    WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", True);
    WM_HINTS = XInternAtom(display, "_MOTIF_WM_HINTS", True);

}

static void setWindowState(::Display *display, Window window, Bool fullScreen)
{
    // Set the window manager state
	Atom atoms[3];
	int numAtoms = 0;

    if (fullScreen) {
		atoms[0] = _NET_WM_STATE_FULLSCREEN;
        atoms[1] = _NET_WM_STATE_MAXIMIZED_VERT;
        atoms[2] = _NET_WM_STATE_MAXIMIZED_HORZ;

		numAtoms = 1;
	}

    if (numAtoms > 0) {
		XChangeProperty(
				display,
				window,
				_NET_WM_STATE,
				XA_ATOM,
				32,
				PropModeReplace,
				(unsigned char *) atoms,
				numAtoms);
    } else {
		XDeleteProperty(display, window, _NET_WM_STATE);
	}
}

/*static void setSizeHint(
        Display *display,
        Window window,
        Int32 width,
        Int32 height,
        Int32 x,
        Int32 y,
        Bool fullScreen,
        Bool resizable,
        const Vector2i &minSize,
        const Vector2i &maxSize)
{
    XSizeHints *sizeHints = XAllocSizeHints();
    if (sizeHints)
    {
        if (!resizable || fullScreen)
        {
            sizeHints->min_width = sizeHints->max_width = width;
            sizeHints->min_height = sizeHints->max_height = height;
            sizeHints->flags = PMaxSize | PMinSize;

            // to fix if window is not resizable
            sizeHints->x = x;
            sizeHints->y = y;
            sizeHints->width  = width;
            sizeHints->height = height;
            sizeHints->flags |= USPosition | PSize;
        }
        if (!fullScreen)
        {
            sizeHints->x = x;
            sizeHints->y = y;
            sizeHints->width  = width;
            sizeHints->height = height;
            //sizeHints->flags = USPosition | USSize;
            sizeHints->flags = PPosition | PSize;

            if (minSize.x() >= 0 && minSize.y() >= 0)
            {
                sizeHints->min_width = minSize.x();
                sizeHints->min_height = minSize.y();

                sizeHints->flags |= PMinSize;
            }

            if (maxSize.x() >= 0 && maxSize.y() >= 0)
            {
                sizeHints->max_width = maxSize.x();
                sizeHints->max_height = maxSize.y();

                sizeHints->flags |= PMaxSize;
            }
        }
        XSetWMNormalHints(display, window, sizeHints);
        XFree(sizeHints);
    }
}*/

//static void setClassHint(
//		Display *display,
//		Window window,
//		const String &_resName,
//		const String &_className)
//{
//	// set the class hints so we can get an icon
//	XClassHint *classhints = XAllocClassHint();
//	if (classhints)
//	{
//		CString resName = _resName.toUtf8().getData();
//		CString className = _className.toUtf8().getData();
//
//		classhints->res_name = resName.getData();
//		classhints->res_class = className.getData();
//
//		XSetClassHint(display, window, classhints);
//		XFree(classhints);
//	}
//}

//static Bool getPropertyValue(
//		Display *display,
//		Window window,
//		const Char *propname,
//		long maxLen,
//		unsigned long *numItemRet,
//		unsigned char **propRet)
//{
//	int result;
//	Atom property;
//	Atom actualTypeReturn;
//	int actualFormatReturn;
//	unsigned long bytesAfterReturn;
//
//	property = XInternAtom(display, propname, True);
//	if (property == None)
//	{
//		// atom does not exists
//		return False;
//	}
//
//	result = XGetWindowProperty(
//			display,
//			window,
//			property,
//			0,
//			maxLen,
//			False,
//			AnyPropertyType,
//			&actualTypeReturn,
//			&actualFormatReturn,
//			numItemRet,
//			&bytesAfterReturn,
//			propRet);
//
//	if (result != Success)
//	{
//		// XGetWindowProperty failed
//		return False;
//	}
//
//	if ((actualTypeReturn == None) || (actualFormatReturn == 0))
//	{
//		// "Window is missing property propname
//		return False;
//	}
//
//	if (bytesAfterReturn)
//	{
//		// propname is too big for me
//		return False;
//	}
//
//	if (actualFormatReturn != 32)
//	{
//		// propname has bad format
//		return False;
//	}
//
//	return True;
//}

static void setWMHints(::Display *display, Window window, Bool fullScreen)
{
    // WM Hints
    if (fullScreen) {
        Bool set;
        Atom WM_HINTS;

        // We haven't modified the window manager hints yet
        set = False;

        // First try to set MWM hints
        WM_HINTS = XInternAtom(display, "_MOTIF_WM_HINTS", True);
        if (WM_HINTS != None) {
            // Hints used by Motif compliant window managers
            struct
            {
                unsigned long flags;
                unsigned long functions;
                unsigned long decorations;
                long input_mode;
                unsigned long status;
            } MWMHints = { (1L << 1), 0, 0, 0, 0 };
            // third value is bordered

            XChangeProperty(
                    display,
                    window,
                    WM_HINTS,
                    WM_HINTS,
                    32,
                    PropModeReplace,
                    (unsigned char *) &MWMHints,
                    sizeof(MWMHints) / 4);
            set = True;
        }
        // Now try to set KWM hints
        WM_HINTS = XInternAtom(display, "KWM_WIN_DECORATION", True);
        if (WM_HINTS != None) {
            long KWMHints = 0;

            XChangeProperty(
                    display,
                    window,
                    WM_HINTS,
                    WM_HINTS,
                    32,
                    PropModeReplace,
                    (unsigned char *) &KWMHints,
                    sizeof(KWMHints) / 4);
            set = True;
        }
        // Now try to set GNOME hints
        WM_HINTS = XInternAtom(display, "_WIN_HINTS", True);
        if (WM_HINTS != None) {
            long GNOMEHints = 0;

            XChangeProperty(
                    display,
                    window,
                    WM_HINTS,
                    WM_HINTS,
                    32,
                    PropModeReplace,
                    (unsigned char *) &GNOMEHints,
                    sizeof(GNOMEHints) / 4);
            set = True;
        }
        // Finally set the transient hints if necessary
        if (!set) {
            XSetTransientForHint(display, window, RootWindow(display, DefaultScreen(display)));
        }
    } else {
        Bool set;
        Atom WM_HINTS;

        // We haven't modified the window manager hints yet
        set = False;

        // First try to unset MWM hints
        WM_HINTS = XInternAtom(display, "_MOTIF_WM_HINTS", True);
        if (WM_HINTS != None) {
            XDeleteProperty(display, window, WM_HINTS);
            set = True;
        }
        // Now try to unset KWM hints
        WM_HINTS = XInternAtom(display, "KWM_WIN_DECORATION", True);
        if (WM_HINTS != None) {
            XDeleteProperty(display, window, WM_HINTS);
            set = True;
        }
        // Now try to unset GNOME hints
        WM_HINTS = XInternAtom(display, "_WIN_HINTS", True);
        if (WM_HINTS != None) {
            XDeleteProperty(display, window, WM_HINTS);
            set = True;
        }
        // Finally unset the transient hints if necessary
        if (!set) {
            XDeleteProperty(display, window, XA_WM_TRANSIENT_FOR);
        }
    }
}

// Window settings
void AppWindow::setTitle(const String &title)
{
	// if window created
	m_title = title;

    if (m_hWnd) {
		CString cstr = m_title.toUtf8();
        Char *title = cstr.getData();

		XTextProperty titleProp;
		Xutf8TextListToTextProperty(
                reinterpret_cast<::Display*>(Application::getDisplay()),
				&title,
				1,
				XUTF8StringStyle,
				&titleProp);
		XSetWMName(
                reinterpret_cast<::Display*>(Application::getDisplay()),
				static_cast<Window>(m_hWnd),
				&titleProp);

        XFree(titleProp.value);
	}
}

void AppWindow::setIcon(const Image &icon)
{
    if ((icon.getPixelFormat() != PF_RGB_U8) && (icon.getPixelFormat() != PF_RGBA_U8)) {
		O3D_ERROR(E_InvalidFormat("Icon must be RGB888 or RGBA8888"));
    }

    ::Display *display = reinterpret_cast<::Display*> (Application::getDisplay());
	Window window = static_cast<Window> (m_hWnd);

	// set for any sizes
    if (m_hWnd && icon.isValid()) {
		Image localPicture(icon);

		// get the first pixel as color key
		Rgb colorKey = icon.getRgbPixel(0, 0);
		localPicture.convertToRGBA8(colorKey);

		UInt32 i;
		int numItems;

        long *newSet = nullptr;
		long *icon;

		newSet = new long[(localPicture.getWidth() * localPicture.getHeight() + 2)];
		icon = newSet;
		numItems = (int)(localPicture.getWidth() * localPicture.getHeight() + 2);

		// Set the _NET_WM_ICON property
		icon[0] = (long)localPicture.getWidth();
		icon[1] = (long)localPicture.getHeight();

		const UInt8 *data = localPicture.getData();

		// change RGBA to ARGB
        for (i = 0; i < localPicture.getWidth() * localPicture.getHeight(); ++i) {
			icon[i+2] =  (data[i*4+3] << 24) |
						((data[i*4+0] << 16) & 0x00FF0000)|
						((data[i*4+1] << 8) & 0x0000FF00) |
						((data[i*4+2] << 0) & 0x000000FF);
		}

		XChangeProperty(
				display,
				window,
				_NET_WM_ICON,
				XA_CARDINAL,
				32,
				PropModeReplace,
				(unsigned char *)newSet,
				numItems);

		XFlush(display);

		deleteArray(newSet);
    } else if (m_hWnd) {
		XDeleteProperty(display, window, _NET_WM_ICON);
		XFlush(display);
	}

	m_icon = icon;
}

void AppWindow::setSize(Int32 width, Int32 height)
{
	m_width = width;
	m_height = height;

	m_clientWidth = width;
	m_clientHeight = height;
}

// Resize window
void AppWindow::resize(Int32 clientWidth, Int32 clientHeight)
{
    if (m_hWnd) {
		// window is in fullscreen state
        if (Display::instance()->getAppWindow() == this) {
            DisplayMode displayMode;
            displayMode.width = clientWidth;
            displayMode.height = clientHeight;
            displayMode.bpp = 24;// or 16
            displayMode.freq = 0;

            CIT_DisplayModeList cit = Display::instance()->findDisplayMode(displayMode);
            if (cit == Display::instance()->end()) {
                O3D_ERROR(E_InvalidParameter("Cannot resize a fullscreen window with this width/height"));
            }

            Display::instance()->setDisplayMode(this, cit);

            ::Display *display = reinterpret_cast<::Display*> (Application::getDisplay());
            Window window = static_cast<Window> (m_hWnd);

            XSizeHints *sizeHints = XAllocSizeHints();
            long hints;

            XGetWMNormalHints(display, window, sizeHints, &hints);

            sizeHints->flags &= ~(PMaxSize | PMinSize);

            // to fix if window is not resizable
            sizeHints->x = 0;
            sizeHints->y = 0;
            sizeHints->width  = clientWidth;
            sizeHints->height = clientHeight;
            sizeHints->flags |= PPosition | PSize;

            XSetWMNormalHints(display, window, sizeHints);
            XFree(sizeHints);

            // fix
            XResizeWindow(display, window, clientWidth, clientHeight);
            XMoveWindow(display, window, 0, 0);
            XRaiseWindow(display, window);

            XMapRaised(display, window);

            XFlush(display);

			m_width = clientWidth;
			m_height = clientHeight;

			m_clientWidth = clientWidth;
			m_clientHeight = clientHeight;

			// override_redirect is True so need to manually callback resize
			callBackResize();
        } else {
            ::Display *display = reinterpret_cast<::Display*> (Application::getDisplay());
            Window window = static_cast<Window> (m_hWnd);

            // TODO check min/max size
            XSizeHints *sizeHints = XAllocSizeHints();
            long hints;

            XGetWMNormalHints(display, window, sizeHints, &hints);

            if (!m_resizable) {
                sizeHints->flags &= ~(PMaxSize | PMinSize);
            }

            // to fix if window is not resizable
            sizeHints->width  = clientWidth;
            sizeHints->height = clientHeight;
            sizeHints->flags |= PSize;

            XSetWMNormalHints(display, window, sizeHints);
            XFree(sizeHints);

            XResizeWindow(display, window, clientWidth, clientHeight);
            XFlush(display);

			m_width = clientWidth;
			m_height = clientHeight;

			m_clientWidth = clientWidth;
			m_clientHeight = clientHeight;
		}
    } else {
		m_clientWidth = clientWidth;
		m_clientHeight = clientHeight;
	}
}

static XIM ms_IM = nullptr;

// Apply window settings
void AppWindow::applySettings(Bool fullScreen)
{
    if (!isSet()) {
		O3D_ERROR(E_InvalidPrecondition("Window not set"));
    }

    ::Display *display = reinterpret_cast<::Display*> (Application::getDisplay());

    // init X atoms on the first window creation
    initAtoms(display);

    int r = 8, g = 8, b = 8, a = 8;

    switch (m_colorFormat) {
        case COLOR_RGBA4:
            r = g = b = 4;
            a = 4;
            break;
        case COLOR_RGB5A1:
            r = g = b = 5;
            a = 1;
            break;
        case COLOR_RGB565:
            r = b = 5;
            g = 6;
            a = 0;
            break;
        case COLOR_RGB8:
            r = g = b = 8;
            a = 0;
            break;
        case COLOR_RGBA8:
            r = g = b = 8;
            a = 8;
            break;
        case COLOR_RGB16:
            r = g = b = 16;
            a = 0;
            break;
        case COLOR_RGBA16:
            r = g = b = 16;
            a = 16;
            break;
        case COLOR_RGB32:
            r = g = b = 32;
            a = 0;
            break;
        case COLOR_RGBA32:
            r = g = b = 32;
            a = 32;
            break;
        default:
            break;
    }

    Window window = 0;

    CString resourceName = m_title.toUtf8().getData();
    CString className = m_title.toUtf8().getData();

    if (GL::getImplementation() == GL::IMPL_GLX_14) {
        // Get a matching FB config
        int visualAttribs[] = {
            GLX_X_RENDERABLE, True,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
            GLX_RED_SIZE, r,
            GLX_GREEN_SIZE, g,
            GLX_BLUE_SIZE, b,
            GLX_ALPHA_SIZE, a,
            GLX_DEPTH_SIZE, (int)getDepth(),
            GLX_STENCIL_SIZE, (int)getStencil(),
            GLX_DOUBLEBUFFER, True,
            GLX_SAMPLE_BUFFERS, m_samples == NO_MSAA ? 0 : 1,
            GLX_SAMPLES, (int)m_samples,
            None
        };

        int fbCount;
        GLXFBConfig *glxFBConfig = GLX::chooseFBConfig(
                                       display,
                                       DefaultScreen(display),
                                       visualAttribs,
                                       &fbCount);

        if (!glxFBConfig) {
            O3D_ERROR(E_InvalidResult("Failed to retrieve a frame buffer config"));
        }

        // Pick the FB config/visual with the most samples per pixel
        int bestFBC = -1, worstFBC = -1, bestNumSamp = -1, worstNumSamp = 999;

        for (int i = 0; i < fbCount; i++) {
            XVisualInfo *vi = GLX::getVisualFromFBConfig(display, glxFBConfig[i]);
            if (vi) {
                int sampBuf, samples;

                GLX::getFBConfigAttrib(display, glxFBConfig[i], GLX_SAMPLE_BUFFERS, &sampBuf);
                GLX::getFBConfigAttrib(display, glxFBConfig[i], GLX_SAMPLES, &samples);

                O3D_MESSAGE(String::print(
                                "Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d, SAMPLES = %d",
                                i,
                                vi->visualid,
                                sampBuf,
                                samples));

                if (((bestFBC < 0) || sampBuf) && (samples > bestNumSamp)) {
                    bestFBC = i;
                    bestNumSamp = samples;
                }

                if ((worstFBC < 0) || !sampBuf || (samples < worstNumSamp)) {
                    worstFBC = i;
                    worstNumSamp = samples;
                }
            }
            XFree(vi);
        }

        bestFBC = 0;
        GLXFBConfig bestFbc = glxFBConfig[bestFBC];

        // be sure to free the FBConfig list allocated by glXChooseFBConfig()
        XFree(glxFBConfig);

        // Get a visual info from the chosen FB config
        XVisualInfo *visualInfo = GLX::getVisualFromFBConfig(display, bestFbc);
        O3D_MESSAGE(String::print("Chosen visual ID = 0x%x", visualInfo->visualid));

        XSetWindowAttributes windowAttr;
        Colormap colorMap;
        windowAttr.colormap = colorMap = XCreateColormap(
                                             display,
                                             RootWindow(display, visualInfo->screen),
                                             visualInfo->visual,
                                             AllocNone);

        // set to True is no necessary with modern WM, causes issue
        windowAttr.override_redirect = fullScreen ? False/*True*/ : False;

        windowAttr.background_pixel = 0;
        windowAttr.background_pixmap = None;
        windowAttr.border_pixel = 0;
        windowAttr.event_mask = 0;//ExposureMask | KeyPressMask | KeyReleaseMask |
        //		ButtonPressMask | ButtonReleaseMask |
        //		StructureNotifyMask | PropertyChangeMask | VisibilityChangeMask |
        //		FocusChangeMask | FocusChangeMask | SubstructureNotifyMask;
        windowAttr.cursor = None;

        unsigned long flags = CWCursor | CWBorderPixel | CWColormap | CWOverrideRedirect;
        // flag |= CWEventMask;
        // flag |= CWBackPixel;

        // compute the window position
        if (fullScreen) {
            m_posX = m_posY = 0;
        } else {
            CIT_DisplayModeList videoMode = Display::instance()->getCurrentDisplayMode();
            m_posX = Int32(videoMode->width - /*m_width*/m_clientWidth) / 2;
            m_posY = Int32(videoMode->height - /*m_height*/m_clientHeight) / 2;
        }

        window = XCreateWindow(
                     display,
                     RootWindow(display, visualInfo->screen),
                     m_posX,
                     m_posY,
                     m_clientWidth,
                     m_clientHeight,
                     0,
                     visualInfo->depth,
                     InputOutput,
                     visualInfo->visual,
                     flags,
                     &windowAttr);

        // Done with the visual info data
        XFree(visualInfo);

        if (!window) {
            O3D_ERROR(E_InvalidResult("Failed to create window"));
        }

        m_hWnd = static_cast<_HWND>(window);
        m_HDC = static_cast<_HDC>(window);
        m_PF = reinterpret_cast<_PF>(bestFbc);

    } else if (GL::getImplementation() == GL::IMPL_EGL_15) {
    #ifdef O3D_EGL
        EGLint apiType = EGL_OPENGL_BIT;
        if (GL::getType() == GL::API_GLES_3) {
            apiType = EGL_OPENGL_ES3_BIT;
        }

        // Get a matching config, double buffer is a default with EGL on Surface
        EGLint configAttributes[] = {
            EGL_BUFFER_SIZE, 0,
            EGL_RED_SIZE, r,
            EGL_GREEN_SIZE, g,
            EGL_BLUE_SIZE, b,
            EGL_ALPHA_SIZE, a,
            EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
            EGL_DEPTH_SIZE, (EGLint)getDepth(),
            EGL_LEVEL, 0,
            EGL_RENDERABLE_TYPE, apiType,
            EGL_SAMPLE_BUFFERS, m_samples == NO_MSAA ? 0 : 1,
            EGL_SAMPLES, (EGLint)m_samples,
            EGL_STENCIL_SIZE, (EGLint)getStencil(),
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_TRANSPARENT_TYPE, EGL_NONE,
            EGL_TRANSPARENT_RED_VALUE, EGL_DONT_CARE,
            EGL_TRANSPARENT_GREEN_VALUE, EGL_DONT_CARE,
            EGL_TRANSPARENT_BLUE_VALUE, EGL_DONT_CARE,
            EGL_CONFIG_CAVEAT, EGL_DONT_CARE,
            EGL_CONFIG_ID, EGL_DONT_CARE,
            EGL_MAX_SWAP_INTERVAL, EGL_DONT_CARE,
            EGL_MIN_SWAP_INTERVAL, EGL_DONT_CARE,
            EGL_NATIVE_RENDERABLE, EGL_DONT_CARE,
            EGL_NATIVE_VISUAL_TYPE, EGL_DONT_CARE,
            EGL_NONE
        };

        EGLDisplay eglDisplay = EGL::getDisplay(display);

        // Selection of the visual config
        EGLint numConfigs;

        if (!EGL::chooseConfig(eglDisplay, configAttributes, nullptr, 1, &numConfigs) || !numConfigs) {
            O3D_ERROR(E_InvalidResult("Failed to retrieve a frame buffer config"));
        }

        EGLConfig *eglConfigs = new EGLConfig[numConfigs];

        if (!EGL::chooseConfig(eglDisplay, configAttributes, eglConfigs, numConfigs, &numConfigs)) {
            deleteArray(eglConfigs);
            O3D_ERROR(E_InvalidResult("EGL get configs returns false"));
        }

        // Pick the FB config with the most samples per pixel
        int bestFBC = -1, worstFBC = -1, bestNumSamp = -1, worstNumSamp = 999;

        for (EGLint i = 0; i < numConfigs; i++) {
            EGLint sampBuf, samples, visualId;

            EGL::getConfigAttrib(eglDisplay, eglConfigs[i], EGL_NATIVE_VISUAL_ID, &visualId);
            EGL::getConfigAttrib(eglDisplay, eglConfigs[i], EGL_SAMPLE_BUFFERS, &sampBuf);
            EGL::getConfigAttrib(eglDisplay, eglConfigs[i], EGL_SAMPLES, &samples);

            O3D_MESSAGE(String::print(
                            "Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d, SAMPLES = %d",
                            i,
                            visualId,
                            sampBuf,
                            samples));

            if (((bestFBC < 0) || sampBuf) && (samples > bestNumSamp)) {
                bestFBC = i;
                bestNumSamp = samples;
            }

            if ((worstFBC < 0) || !sampBuf || (samples < worstNumSamp)) {
                worstFBC = i;
                worstNumSamp = samples;
            }
        }

        bestFBC = 0;
        EGLint visualId;
        EGLConfig eglConfig = eglConfigs[bestFBC];
        EGL::getConfigAttrib(eglDisplay, eglConfigs[bestFBC], EGL_NATIVE_VISUAL_ID, &visualId);
        deleteArray(eglConfigs);

        O3D_MESSAGE(String::print("Chosen visual ID = 0x%x", visualId));

        // Window attributes
        XSetWindowAttributes windowAttr;

        // set to True is no necessary with modern WM, causes issue
        windowAttr.override_redirect = fullScreen ? False/*True*/ : False;

        windowAttr.background_pixel = 0;
        windowAttr.background_pixmap = None;
        windowAttr.border_pixel = 0;
        windowAttr.event_mask = 0;
        windowAttr.cursor = None;

        unsigned long flags = CWCursor | CWBorderPixel | CWColormap | CWOverrideRedirect;
        // flag |= CWEventMask;
        // flag |= CWBackPixel;

        // compute the window position
        if (fullScreen) {
            m_posX = m_posY = 0;
        } else {
            CIT_DisplayModeList videoMode = Display::instance()->getCurrentDisplayMode();
            m_posX = Int32(videoMode->width - /*m_width*/m_clientWidth) / 2;
            m_posY = Int32(videoMode->height - /*m_height*/m_clientHeight) / 2;
        }

        // Create the window
        window = XCreateWindow(
                     display,
                     DefaultRootWindow(display), // RootWindow(display, visualInfo->screen),
                     m_posX,
                     m_posY,
                     m_clientWidth,
                     m_clientHeight,
                     0,
                     CopyFromParent,
                     InputOutput,
                     CopyFromParent,
                     flags,
                     &windowAttr);

        if (!window) {
            O3D_ERROR(E_InvalidResult("Failed to create window"));
        }

        // Create a window surface (EGL_RENDER_BUFFER is default to EGL_BACK_BUFFER if possible)
        EGLint surfaceAttributes[] = { EGL_NONE };
        EGLSurface eglSurface = EGL::createWindowSurface(eglDisplay, eglConfig, window, surfaceAttributes);

        if (eglSurface == EGL_NO_SURFACE) {
            O3D_ERROR(E_InvalidResult("Failed to create a GL surface"));
        }

        m_hWnd = static_cast<_HWND>(window);
        m_HDC = reinterpret_cast<_HDC>(eglSurface);
        m_PF = reinterpret_cast<_PF>(eglConfig);
    #else
        O3D_ERROR(E_UnsuportedFeature("Support for EGL is missing"));
    #endif
    } else {
        O3D_ERROR(E_UnsuportedFeature("Support for GLX or EGL only"));
    }

    XMapWindow(display, window);

    // WM Hints
    setWMHints(display, window, fullScreen);

    // Hints
    XWMHints *wmHints = XAllocWMHints();
    wmHints->input = True;
    wmHints->flags = InputHint | StateHint;

    XClassHint *classHints = XAllocClassHint();

    classHints->res_name = resourceName.getData();
    classHints->res_class = className.getData();

    XSizeHints *sizeHints = XAllocSizeHints();
    if (!m_resizable || fullScreen)	{
        sizeHints->min_width = sizeHints->max_width = m_clientWidth;
        sizeHints->min_height = sizeHints->max_height = m_clientHeight;
        sizeHints->flags = PMaxSize | PMinSize;
    } else if (!fullScreen) {
        sizeHints->x = m_posX;
        sizeHints->y = m_posY;
        sizeHints->width  = m_width;
        sizeHints->height = m_height;
        sizeHints->flags = PPosition | PSize;

        if (m_minSize.x() >= 0 && m_minSize.y() >= 0) {
            sizeHints->min_width = m_minSize.x();
            sizeHints->min_height = m_minSize.y();

            sizeHints->flags |= PMinSize;
        }

        if (m_maxSize.x() >= 0 && m_maxSize.y() >= 0) {
            sizeHints->max_width = m_maxSize.x();
            sizeHints->max_height = m_maxSize.y();

            sizeHints->flags |= PMaxSize;
        }
    }

    // Set the size, input and class hints, and define WM_CLIENT_MACHINE and WM_LOCALE_NAME
    XSetWMProperties(display, window, nullptr, nullptr, nullptr, 0, sizeHints, wmHints, classHints);
    XFree(sizeHints);
    XFree(classHints);
    XFree(wmHints);

    // set the window manager state
    setWindowState(display, window, fullScreen);

    // let the window manager know its a "normal" window
    XChangeProperty(
                display,
                window,
                _NET_WM_WINDOW_TYPE,
                XA_ATOM,
                32,
                PropModeReplace,
                (unsigned char *) &_NET_WM_WINDOW_TYPE_NORMAL,
                1);

    // set the window PID
    Int32 pid = Application::getPID();

    XChangeProperty(display, window, _NET_WM_PID, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&pid, 1);

    // let the window manager delete the window
    if (WM_DELETE_WINDOW != None) {
        XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1);
    }

    XMapRaised(display, window);

    if (!fullScreen) {
		XMoveWindow(
				display,
				window,
				m_posX,
				m_posY);
    }

	// window size
	m_width = m_clientWidth;
	m_height = m_clientHeight;

	// Input method

	// TODO to display
	XSetLocaleModifiers("");

    if (ms_IM == nullptr) {
        ms_IM = XOpenIM(display, nullptr, resourceName.getData(), className.getData());
    }

	m_ic = (void*)XCreateIC(
                ms_IM,
				XNClientWindow, window,
				XNFocusWindow, window,
				XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                XNResourceName, resourceName.getData(),
				XNResourceClass, className.getData(),
                nullptr);

	XSelectInput(
			display,
			window,
			(FocusChangeMask | EnterWindowMask | LeaveWindowMask
			| ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask
			| KeyPressMask | KeyReleaseMask | PropertyChangeMask | StructureNotifyMask
			| KeymapStateMask));

	// set title and icon
	setTitle(m_title);
	setIcon(m_icon);

	Application::addAppWindow(this);

	// create event
	callBackCreate();

	m_running = True;
}

// destroy the window.
void AppWindow::destroy()
{
	// delete inputs
	m_inputManager.destroy();

	m_icon.destroy();

    if (m_hWnd != NULL_HWND) {
        ::Display *display = reinterpret_cast<::Display*> (Application::getDisplay());

        if (isFullScreen()) {
            Display::instance()->restoreDisplayMode();
        }

        if (m_ic != nullptr) {
            XDestroyIC((XIC)m_ic);
        }

        // @todo to display to be done once
        if (ms_IM != nullptr) {
            XCloseIM(ms_IM);
        }

        // destroy surface
        if (GL::getImplementation() == GL::IMPL_EGL_15) {
        #ifdef O3D_EGL
            EGLDisplay eglDisplay = EGL::getDisplay(display);
            EGL::destroySurface(eglDisplay, reinterpret_cast<EGLSurface>(m_HDC));
        #endif
        }

		// get the colormap from attribute
		XWindowAttributes windowAttr;
		XGetWindowAttributes(display, static_cast<Window> (m_hWnd), &windowAttr);

		XFreeColormap(display, windowAttr.colormap);
		XDestroyWindow(display, static_cast<Window> (m_hWnd));
	}

    m_ic = nullptr;
    m_hWnd = NULL_HWND;
    m_PF = NULL_PF;
    m_HDC = NULL_HDC;

	m_running = False;
}

Bool AppWindow::pasteToClipboard(const String &text, Bool primary)
{
    if (!m_hWnd) {
        O3D_ERROR(E_InvalidOperation("The window must be valid"));
    }

    // nothing to paste
    if (text.isEmpty()) {
        return False;
    }

    ::Display *display = reinterpret_cast<::Display*>(Application::getDisplay());
    Window window =	static_cast<Window>(m_hWnd);

    CString utf8 = text.toUtf8();

    XLockDisplay(display);

    // Copy the text into the root windows cut buffer (for Xterm compatibility)
    XStoreBytes(display, utf8.getData(), utf8.length()+1);

    // Set ourself as the owner of the CLIPBOARD atom
    XSetSelectionOwner(display, primary ? XA_PRIMARY : XA_CLIPBOARD, window, CurrentTime);

    Window selectionOwner = XGetSelectionOwner(display, XA_CLIPBOARD);

    Bool result = False;

    if (selectionOwner == window) {
        // We got ownership
        result = True;
    } else {
        // We did not get ownership
        result = False;
    }

    XUnlockDisplay(display);

    return result;
}

static Int32 pending(::Display *display, UInt32 timeout)
{
    UInt32 sTime = System::getMsTime();

    for(;;) {
        XFlush(display);
        if (XEventsQueued(display, QueuedAlready)) {
            return 1;
        }

        // More drastic measures are required -- see if X is ready to talk
        {
            static struct timeval zero_time;        // static == 0
            int x11_fd;
            fd_set fdset;

            x11_fd = ConnectionNumber(display);
            FD_ZERO(&fdset);
            FD_SET(x11_fd, &fdset);
            if (select(x11_fd + 1, &fdset, nullptr, nullptr, &zero_time) == 1) {
                return (XPending(display));
            }
        }

        if (System::getMsTime() - sTime >= timeout) {
            return 0;
        }

        System::waitMs(2);
    }
}

//static int predicate(Display *display, XEvent *event, XPointer arg)
//{
//    return (event->type == SelectionNotify);
//}

String AppWindow::copyFromClipboard(Bool primary)
{
    if (!m_hWnd) {
        O3D_ERROR(E_InvalidOperation("The window must be valid"));
    }

    ::Display *display = reinterpret_cast<::Display*>(Application::getDisplay());
    Window window =	static_cast<Window>(m_hWnd);

    XLockDisplay(display);

    Atom selection = primary ? XA_PRIMARY : XA_CLIPBOARD;

    // Get the owner of the clipboard selection
    Window selectionOwner = XGetSelectionOwner(display, selection);

    String text; // result

    // owner is propagator
    if (selectionOwner == window) {
        int len;

        char *xdata = XFetchBytes(display, &len);
        text.fromUtf8(xdata, len-1);

        XFree(xdata);
    } else if (selectionOwner != None) {
        // If there is a selection (and therefore owner) fetch it
        XEvent event;
        Bool response = False;

        unsigned char *data = nullptr;
        Atom type;
        int format, result;
        unsigned long len, bytesLeft, dummy;

        // Ask the window whom current owns the clipboard to convert it to an
        // XA_UTF8_STRING and place it into the XA_CLIPBOARD property of our window.
        XConvertSelection(display, selection, XA_UTF8_STRING,
                          selection, window, CurrentTime);
        XFlush(display);

        XUnlockDisplay(display);

        // We now need to wait for a response from the window that owns the clipboard.
        while (!response) {
            // Wait for an event
            // if (XPending(display) > 0) {
            if (pending(display, 2) > 0) {
                // check and consume if found
                if (XCheckTypedWindowEvent(display, window, SelectionNotify, &event)) {
                    response = True;
                }

                // TODO is better than : ?

                /*// check and peek if found
                XPeekIfEvent(display, &event, predicate, nullptr);

                if (event.type == SelectionNotify && event.xselection.requestor == window)
                    response = true;*/
            }
        }

        XLockDisplay(display);

        // See how much data is there
        XGetWindowProperty(display, window,
                           selection, 0, 0, False, AnyPropertyType, &type,
                           &format, &len, &bytesLeft, &data);

        // If any 0-length data was returned, free it
        if (data) {
            XFree(data);
            data = nullptr;
        }

        // If there is any data
        if (bytesLeft) {
            // Fetch the data
            result = XGetWindowProperty(display,
                                        window, selection, 0,
                                        bytesLeft, False, AnyPropertyType,
                                        &type, &format, &len, &dummy, &data);

            // If we got some data, duplicate it
            if (result == Success) {
                text.fromUtf8((const Char*)data);
                XFree(data);
            }
        }

        // Delete the property now that we are finished with it
        XDeleteProperty(display, window, selection);
    }

    XUnlockDisplay(display);

    return text;
}

// Set to full screen
void AppWindow::setFullScreen(Bool fullScreen, UInt32 freq)
{
    if (!m_hWnd) {
		O3D_ERROR(E_InvalidOperation("The window must be valid"));
    }

    if ((Display::instance()->getAppWindow() != nullptr) && (Display::instance()->getAppWindow() != this)) {
		O3D_ERROR(E_InvalidOperation("Another window is currently taking the fullscreen"));
	}

    if (fullScreen && (Display::instance()->getAppWindow() == nullptr)) {
        DisplayMode displayMode;
        displayMode.width = m_clientWidth;
        displayMode.height = m_clientHeight;
        displayMode.bpp = 24;//m_bpp;
        displayMode.freq = 0;

        CIT_DisplayModeList cit = Display::instance()->findDisplayMode(displayMode);
        if (cit != Display::instance()->end()) {
            Display::instance()->setDisplayMode(this, cit);
        } else {
            O3D_ERROR(E_InvalidParameter("Invalid video mode"));
        }

        ::Display *display = reinterpret_cast<::Display*>(Application::getDisplay());
		Window window =	static_cast<Window>(m_hWnd);

		// change window style
		XSetWindowAttributes attr;
        attr.override_redirect = False;

		XChangeWindowAttributes(
				display,
				window,
				CWOverrideRedirect,
				&attr);

        // size hints
        XSizeHints *sizeHints = XAllocSizeHints();
        long hints;

        XGetWMNormalHints(display, window, sizeHints, &hints);

        // no min/max
        sizeHints->flags |= PMaxSize | PMinSize;
        sizeHints->min_width = sizeHints->max_width = m_clientWidth;
        sizeHints->min_height = sizeHints->max_height = m_clientHeight;

        // to fix if window is not resizable
        sizeHints->x = 0;
        sizeHints->y = 0;
        sizeHints->width  = m_clientWidth;
        sizeHints->height = m_clientHeight;
        sizeHints->flags |= PPosition | PSize;

        XSetWMNormalHints(display, window, sizeHints);
        XFree(sizeHints);

        /*XEvent e;
        memset(&e, 0, sizeof(e));

        Atom _NET_WM_STATE = XInternAtom(
                display,
                "_NET_WM_STATE",
                False);

        Atom _NET_WM_STATE_ADD = XInternAtom(
                display,
                "_NET_WM_STATE_ADD",
                False);

        Atom _NET_WM_STATE_REMOVE = XInternAtom(
                display,
                "_NET_WM_STATE_REMOVE",
                False);

        Atom _NET_WM_STATE_FULLSCREEN = XInternAtom(
                display,
                "_NET_WM_STATE_FULLSCREEN",
                False);

        e.xany.type = ClientMessage;
        e.xclient.message_type = _NET_WM_STATE;
        e.xclient.format = 32;
        e.xclient.window = window;
        e.xclient.data.l[0] = fullScreen ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
        e.xclient.data.l[1] = _NET_WM_STATE_FULLSCREEN;
        e.xclient.data.l[3] = 0l;

        XSendEvent(display, RootWindow(display, DefaultScreen(display)), 0,
                   SubstructureNotifyMask | SubstructureRedirectMask, &e);*/

        setWindowState(display, window, True);

        // WM Hints
        setWMHints(display, window, True);

        // see if need to resize, move, raise window here to fix some WM
        XResizeWindow(display, window, m_clientWidth, m_clientHeight);
        XMoveWindow(display, window, 0, 0);
        XRaiseWindow(display, window);

        m_posX = m_posY = 0;

        XMapRaised(display, window);

        XFlush(display);
    } else if (!fullScreen && (Display::instance()->getAppWindow() == this)) {
        ::Display *display = reinterpret_cast<::Display*>(Application::getDisplay());
		Window window =	static_cast<Window>(m_hWnd);

        Display::instance()->restoreDisplayMode();

		// change window style
		XSetWindowAttributes attr;
		attr.override_redirect = False;

		XChangeWindowAttributes(
				display,
				window,
				CWOverrideRedirect,
				&attr);

		// Normal window
		setWindowState(display, window, False);
        setWMHints(display, window, False);

		// Center the window
        CIT_DisplayModeList displayMode = Display::instance()->getCurrentDisplayMode();
        m_posX = Int32(displayMode->width - m_width) / 2;
        m_posY = Int32(displayMode->height - m_height) / 2;

        // size hints
        XSizeHints *sizeHints = XAllocSizeHints();
        long hints;

        XGetWMNormalHints(display, window, sizeHints, &hints);

        // reset to normal min/max size
        if (m_resizable) {
            sizeHints->min_width = m_minSize.x(); sizeHints->max_width = m_maxSize.x();
            sizeHints->min_height = m_minSize.y(); sizeHints->max_height = m_maxSize.y();
            sizeHints->flags |= PMaxSize | PMinSize;
        } else {
            // no resize
            sizeHints->min_width = sizeHints->max_width = m_width;
            sizeHints->min_height = sizeHints->max_height = m_height;
            sizeHints->flags |= PMaxSize | PMinSize;
        }

        // to fix if window is not resizable
        sizeHints->x = m_posX;
        sizeHints->y = m_posY;
        sizeHints->width  = m_width;
        sizeHints->height = m_height;
        sizeHints->flags |= PPosition | PSize;

        XSetWMNormalHints(display, window, sizeHints);
        XFree(sizeHints);

        // see if need to resize, move, raise window here to fix some WM
        XResizeWindow(display, window, m_clientWidth, m_clientHeight);
        XMoveWindow(display, window, m_posX, m_posY);
        XRaiseWindow(display, window);

		XFlush(display);
	}
}

// Swap buffers
void AppWindow::swapBuffers()
{
    if (m_hWnd) {
        GL::swapBuffers(Application::getDisplay(), m_hWnd, m_HDC);
    }
}

void AppWindow::setMinSize(const Vector2i &minSize)
{
	m_minSize = minSize;

    if (m_hWnd && m_resizable) {
        ::Display *display = reinterpret_cast<::Display*>(Application::getDisplay());
        Window window =	static_cast<Window>(m_hWnd);

        XSizeHints *sizeHints = XAllocSizeHints();
        long hints;

        XGetWMNormalHints(display, window, sizeHints, &hints);

        if (m_minSize.x() >= 0 && m_minSize.y() >= 0) {
            sizeHints->min_width = m_minSize.x();
            sizeHints->min_height = m_minSize.y();
        } else {
            sizeHints->min_width = 0;
            sizeHints->min_height = 0;
        }

        if (m_maxSize.x() >= 0 && m_maxSize.y() >= 0) {
            sizeHints->max_width = m_maxSize.x();
            sizeHints->max_height = m_maxSize.y();
        } else {
            sizeHints->max_width = 32768;
            sizeHints->max_height = 32768;
        }

        sizeHints->flags |= PMinSize | PMaxSize;

        XSetWMNormalHints(display, window, sizeHints);
        XFree(sizeHints);

        XFlush(display);
	}
}

void AppWindow::setMaxSize(const Vector2i &maxSize)
{
	m_maxSize = maxSize;

    if (m_hWnd && m_resizable) {
        ::Display *display = reinterpret_cast<::Display*>(Application::getDisplay());
        Window window =	static_cast<Window>(m_hWnd);

        XSizeHints *sizeHints = nullptr;
        long hints;

        XGetWMNormalHints(display, window, sizeHints, &hints);

        if (m_minSize.x() >= 0 && m_minSize.y() >= 0) {
            sizeHints->min_width = m_minSize.x();
            sizeHints->min_height = m_minSize.y();
        } else {
            sizeHints->min_width = 0;
            sizeHints->min_height = 0;
		}

        if (m_maxSize.x() >= 0 && m_maxSize.y() >= 0) {
            sizeHints->max_width = m_maxSize.x();
            sizeHints->max_height = m_maxSize.y();
        } else {
            sizeHints->max_width = 32768;
            sizeHints->max_height = 32768;
		}

        sizeHints->flags |= PMinSize | PMaxSize;

        XSetWMNormalHints(display, window, sizeHints);
        XFree(sizeHints);

        XFlush(display);
    }
}

// Process the main loop
void AppWindow::processEvent(EventType eventType, EventData &eventData)
{
    if (!m_running) {
		return;
    }

    switch (eventType) {
		case EVT_CREATE:
			callBackCreate();
			break;
		case EVT_CLOSE:
			callBackClose();
			break;
		case EVT_DESTROY:
			callBackDestroy();
            Application::pushEvent(Application::EVENT_CLOSE_WINDOW, m_hWnd, nullptr);
			break;

		case EVT_UPDATE:
            if (isUpdateNeeded()) {
				callBackUpdate(0);
            }
			break;
		case EVT_PAINT:
            if (isPaintNeeded()) {
				callBackPaint(0);
            }
			break;

		case EVT_MOVE:
			m_posX = eventData.x;
			m_posY = eventData.y;
			callBackMove();
			break;

		case EVT_MAXIMIZE:
			m_width = m_clientWidth = eventData.w;
			m_height = m_clientHeight = eventData.h;
			callBackMaximize();
			break;
		case EVT_MINIMIZE:
			m_width = m_clientWidth = eventData.w;
			m_height = m_clientHeight = eventData.h;
			callBackMinimize();
			break;
		case EVT_RESTORE:
			m_width = m_clientWidth = eventData.w;
			m_height = m_clientHeight = eventData.h;
			callBackRestore();
			break;
		case EVT_RESIZE:
			m_width = m_clientWidth = eventData.w;
			m_height = m_clientHeight = eventData.h;
			callBackResize();
			break;

		case EVT_INPUT_FOCUS_GAIN:
            if (m_inputManager.isInput(Input::INPUT_KEYBOARD)) {
                m_inputManager.getKeyboard()->acquire();
            }
			XSetICFocus((XIC)m_ic);
			callBackFocus();
			break;
		case EVT_INPUT_FOCUS_LOST:
            if (m_inputManager.isInput(Input::INPUT_KEYBOARD)) {
                m_inputManager.getKeyboard()->release();
            }
			XUnsetICFocus((XIC)m_ic);
			callBackLostFocus();
			break;

		case EVT_MOUSE_GAIN:
            if (m_inputManager.isInput(Input::INPUT_MOUSE)) {
                m_inputManager.getMouse()->acquire();
                callBackMouseGain();
            }
			break;
		case EVT_MOUSE_LOST:
            if (m_inputManager.isInput(Input::INPUT_MOUSE)) {
                m_inputManager.getMouse()->release();
                callBackMouseLost();
            }
			break;

		case EVT_MOUSE_BUTTON_DOWN:
            if (m_inputManager.isInput(Input::INPUT_MOUSE)) {
				Bool dblClick;

                switch (eventData.button) {
					// left button
					case Button1:
                        dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::LEFT, True);
						callBackMouseButton(Mouse::LEFT, True, dblClick);
						break;

						// right button
					case Button3:
                        dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::RIGHT, True);
						callBackMouseButton(Mouse::RIGHT, True, dblClick);
						break;

						// middle button
					case Button2:
                        dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::MIDDLE, True);
						callBackMouseButton(Mouse::MIDDLE, True, dblClick);
						break;

						// X1 button
					case 8:
                        dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::X1, True);
						callBackMouseButton(Mouse::X1, True, dblClick);
						break;

						// X2 button
					case 9:
                        dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::X2, True);
						callBackMouseButton(Mouse::X2, True, dblClick);
						break;

						// Wheel up
					case Button4:
						m_inputManager.getMouse()->setMouseWheel(-120);
						callBackMouseWheel();
						break;

					case Button5:
						m_inputManager.getMouse()->setMouseWheel(120);
						callBackMouseWheel();
						break;

					default:
						break;
				}
			}
			break;
		case EVT_MOUSE_BUTTON_UP:
            if (m_inputManager.isInput(Input::INPUT_MOUSE)) {
                switch (eventData.button) {
                    // left button
                    case Button1:
                        m_inputManager.getMouse()->setMouseButton(Mouse::LEFT, False);
                        callBackMouseButton(Mouse::LEFT, False, False);
                        break;

                        // right button
                    case Button3:
                        m_inputManager.getMouse()->setMouseButton(Mouse::RIGHT, False);
                        callBackMouseButton(Mouse::RIGHT, False, False);
                        break;

                        // middle button
                    case Button2:
                        m_inputManager.getMouse()->setMouseButton(Mouse::MIDDLE, False);
                        callBackMouseButton(Mouse::MIDDLE, False, False);
                        break;

                        // X1 button
                    case 8:
                        m_inputManager.getMouse()->setMouseButton(Mouse::X1, False);
                        callBackMouseButton(Mouse::X1, False, False);
                        break;

                        // X2 button
                    case 9:
                        m_inputManager.getMouse()->setMouseButton(Mouse::X2, False);
                        callBackMouseButton(Mouse::X2, False, False);
                        break;

                    default:
                        break;
                }
            }
			break;
		case EVT_MOUSE_MOTION:
            if (m_inputManager.isInput(Input::INPUT_MOUSE)) {
                if (eventData.x || eventData.y) {
                    //m_inputManager.Mouse()->setMouseDelta(eventData.x, eventData.y);
                    m_inputManager.getMouse()->setMousePos(eventData.x, eventData.y);
                    callBackMouseMotion();
                }
            }
            break;
        //case EVT_MOUSE_WHEEL:
        //	m_inputManager.Mouse()->setMouseWheel(eventData.x);
        //	callBackMouseWheel();
        //	break;

		case EVT_KEYDOWN:
            if (m_inputManager.isInput(Input::INPUT_KEYBOARD)) {
                VKey vkey = VKey(eventData.key);
                VKey character = VKey(eventData.character);

                Bool pressed = m_inputManager.getKeyboard()->setKeyState(vkey, True);

				// if not pressed we have a key repeat by the system

                callBackKey(vkey, character, pressed, !pressed);

				// some characters that are not recognized on text input event
                /*if (key == KEY_BACKSPACE) {  // OK for \b
                    callBackCharacter(key, character, static_cast<WChar>('\b'), !pressed);
                } else*/ if ((vkey == KEY_NUMPAD_ENTER) || (vkey == KEY_RETURN)) {
                    callBackCharacter(vkey, character, static_cast<WChar>('\n'), !pressed);
                /*} else if (key == KEY_DELETE) {  // OK for delete
                    callBackCharacter(key, character, static_cast<WChar>(127), !pressed);*/
                } else if (vkey == KEY_TAB) {
                    callBackCharacter(vkey, character, static_cast<WChar>('\t'), !pressed);
                }

				// an unicode character is compound
                if (eventData.unicode != 0) {
					callBackCharacter(
                            vkey,
                            character,
							static_cast<WChar> (eventData.unicode),
							!pressed);
                }
			}
			break;
		case EVT_KEYUP:
            if (m_inputManager.isInput(Input::INPUT_KEYBOARD)) {
                VKey vkey = VKey(eventData.key);
                VKey character = VKey(eventData.character);

                m_inputManager.getKeyboard()->setKeyState(vkey, False);

                callBackKey(vkey, character, False, False);
			}
			break;
		case EVT_CHARDOWN:
            if (m_inputManager.isInput(Input::INPUT_KEYBOARD)) {
                VKey vkey = VKey(eventData.key);
                VKey character = VKey(eventData.character);

                Bool repeat = m_inputManager.getKeyboard()->isKeyDown(vkey);
                callBackCharacter(vkey, character, static_cast<WChar>(eventData.unicode), repeat);
			}
			break;

		default:
			break;
	}
}

void AppWindow::processMotionEvent(EventType eventType, MotionEventData &eventData)
{

}

#endif // O3D_X11
