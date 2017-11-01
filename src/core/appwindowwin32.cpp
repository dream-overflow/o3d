/**
 * @file appwindowwin32.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/base.h"
#include "o3d/core/wintools.h"

// ONLY IF O3D_WIN32 IS SELECTED
#ifdef O3D_WIN32

#include "o3d/core/application.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/diskfileinfo.h"
#include "o3d/core/debug.h"
#include "o3d/core/architecture.h"
#include "o3d/core/wintools.h"
#include "o3d/core/video.h"

using namespace o3d;

//
// WGL addendums
//

#define WGL_DRAW_TO_WINDOW_ARB         0x2001
#define WGL_ACCELERATION_ARB           0x2003

#define WGL_SUPPORT_OPENGL_ARB         0x2010
#define WGL_DOUBLE_BUFFER_ARB          0x2011
#define WGL_STEREO_ARB                 0x2012
#define WGL_PIXEL_TYPE_ARB             0x2013

#define WGL_COLOR_BITS_ARB             0x2014
#define WGL_RED_BITS_ARB               0x2015
#define WGL_RED_SHIFT_ARB              0x2016
#define WGL_GREEN_BITS_ARB             0x2017
#define WGL_GREEN_SHIFT_ARB            0x2018
#define WGL_BLUE_BITS_ARB              0x2019
#define WGL_BLUE_SHIFT_ARB             0x201A
#define WGL_ALPHA_BITS_ARB             0x201B
#define WGL_ALPHA_SHIFT_ARB            0x201C
#define WGL_TYPE_RGBA_ARB              0x202B

#define WGL_DEPTH_BITS_ARB             0x2022
#define WGL_STENCIL_BITS_ARB           0x2023

#define WGL_NO_ACCELERATION_ARB        0x2025
#define WGL_GENERIC_ACCELERATION_ARB   0x2026
#define WGL_FULL_ACCELERATION_ARB      0x2027

#define WGL_SAMPLE_BUFFERS_ARB         0x2041
#define WGL_SAMPLES_ARB                0x2042

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

// wglGetExtensionsStringARB prototype.
typedef const char * (APIENTRYP PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC hdc);

// wglGetExtensionsStringARB prototype.
typedef BOOL (APIENTRYP PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc,
                                                         const int *piAttribIList,
                                                         const FLOAT *pfAttribFList,
                                                         UINT nMaxFormats,
                                                         int *piFormats,
                                                         UINT *nNumFormats);

static Bool isExtensionSupported(const char *extList, const char *extension)
{
    const char *start;
    const char *where, *terminator;

    // Extension names should not have spaces.
    where = strchr(extension, ' ');
    if (where || *extension == '\0')
        return False;

    // It takes a bit of care to be fool-proof about parsing the
    // OpenGL extensions string. Don't be fooled by sub-strings, etc.
    for (start = extList;;)
    {
        where = strstr(start, extension);

        if (!where)
            break;

        terminator = where + strlen(extension);

        if (where == start || *(where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return True;

        start = terminator;
    }

    return False;
}

//
// WIN32 addendums
//

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

void getWindowPosAndClientSize(
	_HWND hWnd,
	Int32 &posX,
	Int32 &posY,
	Int32 &width,
	Int32 &height,
	Int32 &clientWidth,
	Int32 &clientHeight)
{
	// get window position and client size
	RECT rect;

	GetClientRect((HWND)hWnd, &rect);
	clientWidth = rect.right - rect.left;
	clientHeight = rect.bottom - rect.top;

	GetWindowRect((HWND)hWnd, &rect);
	posX = rect.left;
	posY = rect.top;
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}

//
// AppWindow
//

void AppWindow::setMinSize(const Vector2i &minSize)
{
	m_minSize = minSize;
}

void AppWindow::setMaxSize(const Vector2i &maxSize)
{
	m_maxSize = maxSize;
}

// Apply window settings
void AppWindow::applySettings(Bool fullScreen)
{
	if (!isSet())
		O3D_ERROR(E_InvalidPrecondition("Window not set"));

    m_running = False;

	// create the window
	m_hWnd = WinTools::createWindow(
		NULL,
		m_title,
		"",
		m_clientWidth,
		m_clientHeight,
        (_WNDPROC)AppWindow::wndProc,
		fullScreen,
		m_resizable,
		0);
	
	if (!m_hWnd)
		O3D_ERROR(E_InvalidAllocation("m_hWnd"));

	m_HDC = (_HDC)GetDC((HWND)m_hWnd);

	// Choose and set Pixel Format
	PIXELFORMATDESCRIPTOR pfd;
    Int32 pixelFormat;

    memset(&pfd, 0, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = (BYTE)getBpp();
    pfd.cDepthBits = (BYTE)getDepth();
    pfd.cStencilBits = (BYTE)getStencil();
	pfd.iLayerType = PFD_MAIN_PLANE;    

    OSVERSIONINFO osvi;
    memset(&osvi,0,sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    // active composition for Vista and above
#ifdef PFD_SUPPORT_COMPOSITION
    if (osvi.dwMajorVersion >= 6)
        pfd.dwFlags |= PFD_SUPPORT_COMPOSITION;
#endif

    if ((pixelFormat = ChoosePixelFormat((HDC)m_HDC, &pfd)) == 0)
    {
        WinTools::destroyWindow(m_hWnd);
        WinTools::unregisterWindowClass(NULL, m_title);

        O3D_ERROR(E_InvalidResult("Unable to choose the pixel format"));
    }

    if ((SetPixelFormat((HDC)m_HDC, pixelFormat, &pfd)) == FALSE)
    {
        WinTools::destroyWindow(m_hWnd);
        WinTools::unregisterWindowClass(NULL, m_title);

        O3D_ERROR(E_InvalidResult("Unable to set the pixel format"));
    }

    // MSAA path
    if (m_samples != NO_MSAA)
    {
        int r = 8, g = 8, b = 8, a = 8;

        switch (m_colorFormat)
        {
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

        HGLRC hGLRC;

        // we create a dummy OpenGL context into the dummy window,
        if ((hGLRC = wglCreateContext((HDC)m_HDC)) == NULL)
        {
            WinTools::destroyWindow(m_hWnd);
            WinTools::unregisterWindowClass(NULL, m_title);

            O3D_ERROR(E_InvalidResult("Unable to create the OpenGL context"));
        }

        if (wglMakeCurrent((HDC)m_HDC, hGLRC) == False)
        {
            wglDeleteContext((HGLRC)hGLRC);

            WinTools::destroyWindow(m_hWnd);
            WinTools::unregisterWindowClass(NULL, m_title);

            O3D_ERROR(E_InvalidResult("Unable to set the new OpenGL context as current"));
        }

        // get the wgl extensions
        PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
        wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");

        PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
        wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

        if (!wglChoosePixelFormatARB)
        {
            wglMakeCurrent((HDC)m_HDC, 0);
            wglDeleteContext((HGLRC)hGLRC);

            WinTools::destroyWindow(m_hWnd);
            WinTools::unregisterWindowClass(NULL, m_title);

            O3D_ERROR(E_InvalidResult("Unable to find wglChoosePixelFormatARB"));
        }

        const char *wglExts = nullptr;

        if (wglGetExtensionsStringARB)
            wglExts = wglGetExtensionsStringARB((HDC)m_HDC);

        if (!wglExts || !isExtensionSupported(wglExts, "WGL_ARB_multisample"))
        {
            wglMakeCurrent((HDC)m_HDC, 0);
            wglDeleteContext((HGLRC)hGLRC);

            WinTools::destroyWindow(m_hWnd);
            WinTools::unregisterWindowClass(NULL, m_title);

            O3D_ERROR(E_InvalidResult("Unable to find WGL_ARB_multisample"));
        }

        // attributes list
        int attribsi[] = {
            WGL_DRAW_TO_WINDOW_ARB, True,
            WGL_SUPPORT_OPENGL_ARB, True,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_RED_BITS_ARB, r,
            WGL_GREEN_BITS_ARB, g,
            WGL_BLUE_BITS_ARB, b,
            WGL_ALPHA_BITS_ARB, a,
            WGL_DEPTH_BITS_ARB, (int)getDepth(),
            WGL_STENCIL_BITS_ARB, (int)getStencil(),
            WGL_DOUBLE_BUFFER_ARB, True,
            WGL_SAMPLE_BUFFERS_ARB, m_samples == NO_MSAA ? 0 : 1,
            WGL_SAMPLES_ARB, (int)m_samples,
            0, 0};

        UINT numFormats;
        float attribsf[] = {0, 0};

        // finally get our pixel format
        BOOL valid = wglChoosePixelFormatARB(
                    (HDC)m_HDC,
                    attribsi,
                    attribsf,
                    1,
                    &pixelFormat,
                    &numFormats);

        // delete dummy OpenGL context
        wglMakeCurrent((HDC)m_HDC, 0);
        wglDeleteContext((HGLRC)hGLRC);

        // we found at least one
        if (valid && numFormats > 0)
        {
            // destroy the dummy window
            WinTools::destroyWindow(m_hWnd);

            // and recreate the final window
            m_hWnd = WinTools::createWindow(
                NULL,
                m_title,
                "",
                m_clientWidth,
                m_clientHeight,
                (_WNDPROC)AppWindow::wndProc,
                fullScreen,
                m_resizable,
                0);

            if (!m_hWnd)
                O3D_ERROR(E_InvalidAllocation("m_hWnd"));

            m_HDC = (_HDC)GetDC((HWND)m_hWnd);

            // now we can set our customized pixel format
            if ((SetPixelFormat((HDC)m_HDC, pixelFormat, &pfd)) == FALSE)
            {
                WinTools::destroyWindow(m_hWnd);
                WinTools::unregisterWindowClass(NULL, m_title);

                O3D_ERROR(E_InvalidResult("Unable to set the pixel format"));
            }
        }
        else
            O3D_ERROR(E_InvalidResult("Unable to choose the pixel format"));
    }

    m_PF = pixelFormat;

	Application::addAppWindow(this);

	if (!fullScreen)
		WinTools::centerWindow(m_hWnd);

	// get window position and client size
	getWindowPosAndClientSize(m_hWnd, m_posX, m_posY, m_width, m_height, m_clientWidth, m_clientHeight);

	this->callBackCreate();

	m_running = True;
}

// Destroy the window
void AppWindow::destroy()
{
	// delete inputs
	m_inputManager.destroy();

	m_icon.destroy();

	if (m_hWnd != NULL_HWND)
	{
		if (isFullScreen())
			Video::instance()->restoreDisplayMode();

		WinTools::destroyWindow(m_hWnd);
		WinTools::unregisterWindowClass(NULL, m_title);

		m_hWnd = NULL_HWND;
	}

	m_running = False;
	m_HDC = NULL_HDC;
	m_PF = NULL_PF;
}

// Window settings
void AppWindow::setTitle(const String& title)
{
	m_title = title;

	if (m_hWnd != NULL_HWND)
		SetWindowTextW((HWND)m_hWnd, title.getData());
}

void AppWindow::setIcon(const Image &icon)
{
	if ((icon.getPixelFormat() != PF_RGB_U8) && (icon.getPixelFormat() != PF_RGBA_U8))
		O3D_ERROR(E_InvalidFormat("Icon must be O3D_RGB_U8 or O3D_RGBA_U8"));

	if ((m_hWnd != NULL_HWND) && icon.isValid())
	{
		Image localPicture(icon);

		// get the first pixel as color key
		Rgb colorKey = icon.getRgbPixel(0, 0);
		localPicture.convertToRGBA8(colorKey);

		BYTE *andBits16 = new BYTE[16*16];
		BYTE *andBits32 = new BYTE[32*32];
		BYTE *xorBits16 = new BYTE[16*16*4];
		BYTE *xorBits32 = new BYTE[32*32*4];

		// small icon
		Image icon16(localPicture);
		if ((icon16.getWidth() != 16) ||(icon16.getHeight() != 16))
			icon16.resize(16, 16, FILTER_NEAREST);

		const UInt8 *data16 = icon16.getData();

		for (UInt32 i = 0, j = 0; i < 16*16; ++i, j+=4)
		{
			andBits16[i] = (data16[3] > 0) ? 0 : 0xff;
			xorBits16[j+2] = data16[j];
			xorBits16[j+1] = data16[j+1];
			xorBits16[j]   = data16[j+2];
			xorBits16[j+3] = data16[j+3];
		}

		// large icon
		Image icon32(localPicture);
		if ((icon32.getWidth() != 16) ||(icon32.getHeight() != 16))
			icon32.resize(32, 32, FILTER_NEAREST);

		const UInt8 *data32 = icon32.getData();

		for (UInt32 i = 0, j = 0; i < 32*32; ++i, j+=4)
		{
			andBits32[i] = (data32[3] > 0) ? 0 : 0xff;
			xorBits32[j+2] = data32[j];
			xorBits32[j+1] = data32[j+1];
			xorBits32[j]   = data32[j+2];
			xorBits32[j+3] = data32[j+3];
		}

		// If the window exists send a message, otherwise the ApplySettings will use
		// the value set in m_icon.
		HICON smallIcon = NULL;
		HICON largeIcon = NULL;

		smallIcon = CreateIcon(
			NULL,
			16,
			16,
			1,
			32,
			andBits16,
			xorBits16);

		largeIcon = CreateIcon(
			NULL,
			32,
			32,
			1,
			32,
			andBits32,
			xorBits32);

		// Set the icon for the window
		if (smallIcon)
			::SendMessage((HWND)m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)smallIcon);

		// Set the icon in the task manager
		if (largeIcon)
			::SendMessage((HWND)m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)largeIcon);

		deleteArray(andBits16);
		deleteArray(andBits32);
		deleteArray(xorBits16);
		deleteArray(xorBits32);
	}
	else if (m_hWnd != NULL_HWND)
	{
		// remove icon
		// If the window exists send a message, otherwise the ApplySettings will use
		// the value set in m_icon.
		HICON smallIcon = NULL;
		HICON largeIcon = NULL;

		// Set the icon for the window
		if (smallIcon)
			::SendMessage((HWND)m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)smallIcon);

		// Set the icon in the task manager
		if (largeIcon)
			::SendMessage((HWND)m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)largeIcon);
	}

	m_icon = icon;
}

void AppWindow::setSize(Int32 width,Int32 height)
{
	m_clientWidth = width;
	m_clientHeight = height;

	if (m_hWnd != NULL_HWND)
	{
		RECT rect;

		GetWindowRect((HWND)m_hWnd, &rect);
		m_width = rect.right - rect.left;
		m_height = rect.bottom - rect.top;
	}
}

void AppWindow::resize(Int32 clientWidth, Int32 clientHeight)
{
	if (m_hWnd != NULL_HWND)
	{
		// window is in fullscreen state
		if (Video::instance()->getAppWindow() == this)
		{
			VideoMode videoMode;
			videoMode.width = clientWidth;
			videoMode.height = clientHeight;
            videoMode.bpp = getBpp();
			videoMode.freq = 0;

			CIT_VideoModeList cit = Video::instance()->findDisplayMode(videoMode);
			if (cit == Video::instance()->end())
				O3D_ERROR(E_InvalidParameter("Cannot resize a full screen window with this width/height"));

			Video::instance()->setDisplayMode(this, cit);

			WinTools::resizeWindow(m_hWnd, clientWidth, clientHeight);
			ShowWindow((HWND)m_hWnd, SW_SHOW);

			m_width = m_clientWidth = clientWidth;
			m_height = m_clientHeight = clientHeight;
		}
		// in windowed
		else
		{
			WinTools::resizeWindow(m_hWnd, clientWidth, clientHeight);
			WinTools::centerWindow(m_hWnd);

			// get window position and client size
			getWindowPosAndClientSize(m_hWnd, m_posX, m_posY, m_width, m_height, m_clientWidth, m_clientHeight);
		}

		callBackResize();
	}
	else
	{
		m_width = m_clientWidth = clientWidth;
		m_height = m_clientHeight = clientHeight;
	}
}

Bool AppWindow::pasteToClipboard(const String &text, Bool primary)
{
    if (!m_hWnd)
        O3D_ERROR(E_InvalidOperation("The window must be valid"));

    // nothing to paste
    if (text.isEmpty())
        return False;

    Bool result = False;

    if (OpenClipboard(nullptr))
    {
        EmptyClipboard();

        // Convert unix newlines to windows one's
        String ltext(text);
        ltext.replace("\n", "\r\n");

        const HGLOBAL hglb = GlobalAlloc(GMEM_MOVEABLE, (ltext.length()+1) * sizeof(WChar));
        if(hglb)
        {
            WCHAR *buffer = (WCHAR*)GlobalLock(hglb);
            memcpy(buffer, ltext.getData(), (ltext.length()+1) * sizeof(WChar));

            GlobalUnlock(hglb);
            SetClipboardData(CF_UNICODETEXT/*CF_TEXT*/, hglb);

            result = True;
        }

        CloseClipboard();
    }

    return result;
}

String AppWindow::copyFromClipboard(Bool primary)
{
    if (!m_hWnd)
        O3D_ERROR(E_InvalidOperation("The window must be valid"));

    String text;  // result

    if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
        return text;

    if (!OpenClipboard(nullptr))
    {
        HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT);
        if (hglb)
        {
            WCHAR *buffer = (WCHAR*)(GlobalLock(hglb));
            if (buffer) {
                text = (WChar*)buffer;
                text.replace("\r\n", "\n");

                GlobalUnlock(hglb);
            }
        }
        CloseClipboard();
    }

    return text;
}

// Set to full screen
void AppWindow::setFullScreen(Bool fullScreen, UInt32 freq)
{
	if (m_hWnd == NULL_HWND)
		O3D_ERROR(E_InvalidOperation("The window must be valid"));

	if ((Video::instance()->getAppWindow() != NULL) &&
		(Video::instance()->getAppWindow() != this))
	{
		O3D_ERROR(E_InvalidOperation("Another window is currently taking the fullscreen"));
	}
	
	if (fullScreen && (Video::instance()->getAppWindow() == NULL))
	{
		VideoMode videoMode;
		videoMode.width = m_clientWidth;
		videoMode.height = m_clientHeight;
        videoMode.bpp = getBpp();
		videoMode.freq = 0;

		CIT_VideoModeList cit = Video::instance()->findDisplayMode(videoMode);
		if (cit != Video::instance()->end())
			Video::instance()->setDisplayMode(this, cit);
		else
			O3D_ERROR(E_InvalidParameter("Invalid video mode"));

		Int32 width = m_clientWidth;
		Int32 height = m_clientHeight;

		WinTools::changeWindowStyle(m_hWnd, 0, True, False, width, height);
		
		WinTools::resizeWindow(m_hWnd, m_clientWidth, m_clientHeight);
		SetWindowPos((HWND)m_hWnd, HWND_TOPMOST, 0, 0, width, height, SWP_SHOWWINDOW | SWP_NOCOPYBITS | SWP_NOSIZE);

		m_width = m_clientWidth;
		m_height = m_clientHeight;

		m_posX = m_posY = 0;
	}
	else if (!fullScreen && (Video::instance()->getAppWindow() == this))
	{
		Video::instance()->restoreDisplayMode();

		Int32 width = m_clientWidth;
		Int32 height = m_clientHeight;

		// Normal window
		WinTools::changeWindowStyle(m_hWnd, 0, False, m_resizable, width, height);

		m_width = width;
		m_height = height;

		SetWindowPos((HWND)m_hWnd, HWND_NOTOPMOST, 0, 0, width, height, SWP_SHOWWINDOW | SWP_NOCOPYBITS | SWP_NOMOVE);

		WinTools::centerWindow(m_hWnd);

		// get window position and client size
		getWindowPosAndClientSize(m_hWnd, m_posX, m_posY, m_width, m_height, m_clientWidth, m_clientHeight);
	}

	callBackResize();
}

// Swap buffers
void AppWindow::swapBuffers()
{
	if (m_HDC != NULL_HDC)
		::SwapBuffers((HDC)m_HDC);
}

// Process internals deferred events
void AppWindow::processEvent(EventType eventType, EventData &eventData)
{
	switch (eventType)
	{
		case EVT_UPDATE:
			if (isUpdateNeeded())
				callBackUpdate(0);
			break;
		case EVT_PAINT:
			if (isPaintNeeded())
				callBackPaint(0);
			break;
		default:
			// All is done in the WndProc
			O3D_ERROR(E_InvalidOperation("Not supported"));
			break;
	}
}

inline WPARAM translateWmKey(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_SHIFT)
	{
		return ((lParam >> 16) & 0x7f) == 0x2A ? KEY_LSHIFT : KEY_RSHIFT;
	}

	if (lParam & (1 << 24))
	{
		switch (wParam)
		{
		case VK_RETURN:
			return KEY_NUMPAD_ENTER;
		//case VK_SHIFT: cannot be distinguished here @see KeyboardWIN32.cpp
		//	return KEY_RSHIFT;
		case VK_CONTROL:
			return KEY_RCONTROL;
		case VK_MENU:
			return KEY_RALT;
		default:
			return wParam;
		}
	}
	else
	{
		switch (wParam)
		{
		case VK_RETURN:
			return KEY_RETURN;
		//case VK_SHIFT: cannot be distinguished here @see KeyboardWIN32.cpp
		//	return KEY_LSHIFT;
		case VK_CONTROL:
			return KEY_LCONTROL;
		case VK_MENU:
            return KEY_LALT;
		default:
			return wParam;
		}
	}
}

inline USHORT translateRawInputKey(USHORT VKey, USHORT Flags)
{
	if (Flags & RI_KEY_E0)
	{
		switch (VKey)
		{
		case VK_RETURN:
			return KEY_NUMPAD_ENTER;
//		case VK_SHIFT: cannot be distinguished here @see KeyboardWIN32.cpp
//			return KEY_RSHIFT;
//		case VK_CONTROL: cannot be distinguished here @see KeyboardWIN32.cpp
//			return KEY_RCONTROL;
		case VK_MENU:
			return KEY_RALT;
		default:
			return VKey;
		}
	}
	else
	{
		switch (VKey)
		{
		case VK_RETURN:
			return KEY_RETURN;
//		case VK_SHIFT: cannot be distinguished here @see KeyboardWIN32.cpp
//			return KEY_LSHIFT;
//		case VK_CONTROL: cannot be distinguished here @see KeyboardWIN32.cpp
//			return KEY_LCONTROL;
		case VK_MENU:
			return KEY_LALT;
		default:
			return VKey;
		}
	}
}

// wndProc
LRESULT CALLBACK AppWindow::wndProc(
		HWND hWnd,
		UInt32 msg,
		WPARAM wParam,
		LPARAM lParam)
{
	AppWindow *inst = Application::getAppWindow((_HWND)hWnd);

	EventData eventData;

	static BYTE rawInputBuf[4096];

	if (!inst)
		return DefWindowProcW(hWnd, msg, wParam, lParam);

	switch(msg)
	{
		case WM_CREATE:
			inst->m_hasMouse = False;
			return 0;

		case WM_CLOSE:
			inst->callBackClose();
			return 0;

		// avoid flickering
		case WM_ERASEBKGND:
			return 0;

		// repaint a part or totally of the content
		case WM_PAINT:
			inst->processEvent(EVT_UPDATE, eventData);
			inst->processEvent(EVT_PAINT, eventData);
			return 0;

		case WM_DESTROY:
			inst->callBackDestroy();
			Application::pushEvent(0xfffe, nullptr, inst->getHWND());
			return 0;

		case WM_SETFOCUS:
			inst->callBackFocus();
			break;

		case WM_KILLFOCUS:
			inst->callBackLostFocus();
			break;

		case WM_NCMOUSEMOVE:
		case WM_NCLBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
			if (inst->m_hasMouse)
				inst->callBackMouseLost();
			break;

		case WM_CAPTURECHANGED:
			if (inst->m_hasMouse)
				inst->callBackMouseLost();
			break;
		//case WM_MOUSELEAVE:
		//	inst->getInput().getMouse()->enableCursor();
		///	break;

		case WM_MOUSEACTIVATE:
			if (inst->m_hasMouse)
				inst->callBackMouseGain();
			break;

		case WM_MOVE:
			eventData.x = LOWORD(lParam);
			eventData.y = HIWORD(lParam);

			inst->m_posX = eventData.x;
			inst->m_posY = eventData.y;
			inst->callBackMove();
			break;

		// size hints
		case WM_GETMINMAXINFO:
			{
				LPMINMAXINFO minMaxInfo = nullptr;
				memcpy(minMaxInfo, (void*)lParam, sizeof(MINMAXINFO));
				// min size hint
				{
					minMaxInfo->ptMinTrackSize.x = inst->m_minSize.x();
					minMaxInfo->ptMinTrackSize.y = inst->m_minSize.y();
				}

				// max size hint
				if (inst->m_maxSize.x() >= 0 && inst->m_maxSize.y() >= 0)
				{
					minMaxInfo->ptMaxTrackSize.x = inst->m_maxSize.x();
					minMaxInfo->ptMaxTrackSize.y = inst->m_maxSize.y();
				}
			}
			return 0;

		case WM_SIZE:
			eventData.w = LOWORD(lParam);
			eventData.h = HIWORD(lParam);

			switch(wParam)
			{
				case SIZE_MAXIMIZED:
					inst->setSize(LOWORD(lParam), HIWORD(lParam));
					inst->callBackMaximize();
					inst->callBackResize();
					break;
				case SIZE_RESTORED:
					inst->setSize(LOWORD(lParam), HIWORD(lParam));
					//inst->callBackRestore();
					inst->callBackResize();
					break;
				case SIZE_MINIMIZED:
					inst->setSize(LOWORD(lParam), HIWORD(lParam));
					inst->callBackMinimize();
					break;
				default:
					inst->setSize(LOWORD(lParam), HIWORD(lParam));
					inst->callBackResize();
					break;
			}
			return 0;

		case WM_INPUT:
		{
			UINT bufferSize;
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof (RAWINPUTHEADER));

			if (bufferSize > 4096)
				O3D_ERROR(E_InvalidOperation("Raw input buffer size is too big"));

			GetRawInputData(
				(HRAWINPUT)lParam,
				RID_INPUT,
				(LPVOID)rawInputBuf,
				&bufferSize,
				sizeof (RAWINPUTHEADER));

			RAWINPUT *raw = (RAWINPUT*)rawInputBuf;

			// mouse
			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					if (!inst->m_hasMouse)
						inst->callBackMouseGain();

					LONG mx = raw->data.mouse.lLastX;
					LONG my = raw->data.mouse.lLastY;

					// mouse motion
					if (mx || my)
					{
						inst->getInput().getMouse()->setMouseDelta(mx, my);
						inst->callBackMouseMotion();
					}

					// left
					if (raw->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN)
					{
						Bool dblClick = inst->getInput().getMouse()->setMouseButton(Mouse::LEFT, True);
						inst->callBackMouseButton(Mouse::LEFT, True, dblClick);
					}
					else if (raw->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_UP)
					{
						inst->getInput().getMouse()->setMouseButton(Mouse::LEFT, False);
						inst->callBackMouseButton(Mouse::LEFT, False, False);
					}
					// right
					if (raw->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN)
					{
						Bool dblClick = inst->getInput().getMouse()->setMouseButton(Mouse::RIGHT, True);
						inst->callBackMouseButton(Mouse::RIGHT, True, dblClick);
					}
					else if (raw->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_UP)
					{
						inst->getInput().getMouse()->setMouseButton(Mouse::RIGHT, False);
						inst->callBackMouseButton(Mouse::RIGHT, False, False);
					}
					// middle
					if (raw->data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN)
					{
						Bool dblClick = inst->getInput().getMouse()->setMouseButton(Mouse::MIDDLE, True);
						inst->callBackMouseButton(Mouse::MIDDLE, True, dblClick);
					}
					else if (raw->data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_UP)
					{
						inst->getInput().getMouse()->setMouseButton(Mouse::MIDDLE, False);
						inst->callBackMouseButton(Mouse::MIDDLE, False, False);
					}
					// 4
					if (raw->data.mouse.ulButtons & RI_MOUSE_BUTTON_4_DOWN)
					{
						Bool dblClick = inst->getInput().getMouse()->setMouseButton(Mouse::X1, True);
						inst->callBackMouseButton(Mouse::X1, True, dblClick);
					}
					else if (raw->data.mouse.ulButtons & RI_MOUSE_BUTTON_4_UP)
					{
						inst->getInput().getMouse()->setMouseButton(Mouse::X1, False);
						inst->callBackMouseButton(Mouse::X1, False, False);
					}
					// 5
					if (raw->data.mouse.ulButtons & RI_MOUSE_BUTTON_5_DOWN)
					{
						Bool dblClick = inst->getInput().getMouse()->setMouseButton(Mouse::X2, True);
						inst->callBackMouseButton(Mouse::X2, True, dblClick);
					}
					else if (raw->data.mouse.ulButtons & RI_MOUSE_BUTTON_5_UP)
					{
						inst->getInput().getMouse()->setMouseButton(Mouse::X2, False);
						inst->callBackMouseButton(Mouse::X2, False, False);
					}

					// wheel
					if (raw->data.mouse.ulButtons & RI_MOUSE_WHEEL)
					{
						SHORT wheel = (SHORT)raw->data.mouse.usButtonData;
						inst->getInput().getMouse()->setMouseWheel(-wheel);
						inst->callBackMouseWheel();
					}
				}
			}
			// keyboard
			else if ((raw->header.dwType == RIM_TYPEKEYBOARD))
			{
				Bool state = !(raw->data.keyboard.Flags & RI_KEY_BREAK);

				VKey key = VKey(translateRawInputKey(raw->data.keyboard.VKey, raw->data.keyboard.Flags));
                Bool pressed = inst->getInput().getKeyboard()->setKeyState(key, state);

				if (state)
                    inst->callBackKey(key, key, pressed, !pressed);
				else
                    inst->callBackKey(key, key, False, False);
			}
		}
		return 0;

		case WM_MOUSEMOVE:
			if (!inst->m_hasMouse)
				inst->callBackMouseGain();

			if (!inst->getInput().getMouse()->isGrabbed())
			{
				inst->getInput().getMouse()->setMousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				inst->callBackMouseMotion();
			}
			return 0;

		case WM_LBUTTONDOWN:
			if (!inst->getInput().getMouse()->isGrabbed())
			{
				Bool dblClick = inst->getInput().getMouse()->setMouseButton(Mouse::LEFT, True);
				inst->callBackMouseButton(Mouse::LEFT, True, dblClick);

				// mouse capture move outside the window
				if (inst->m_captureMouse == 0)
					::SetCapture((HWND)inst->m_hWnd);
				++inst->m_captureMouse;
			}
			return 0;
		case WM_LBUTTONUP:
			if (!inst->getInput().getMouse()->isGrabbed())
			{
				inst->getInput().getMouse()->setMouseButton(Mouse::LEFT, False);
				inst->callBackMouseButton(Mouse::LEFT, False, False);

				// mouse move outside window capture release
				--inst->m_captureMouse;
				if (inst->m_captureMouse == 0)
					::ReleaseCapture();
			}
			return 0;

		case WM_RBUTTONDOWN:
			if (!inst->getInput().getMouse()->isGrabbed())
			{
				Bool dblClick = inst->getInput().getMouse()->setMouseButton(Mouse::RIGHT, True);
				inst->callBackMouseButton(Mouse::RIGHT, True, dblClick);

				// mouse capture move outside the window
				if (inst->m_captureMouse == 0)
					::SetCapture((HWND)inst->m_hWnd);
				++inst->m_captureMouse;
			}
			return 0;
		case WM_RBUTTONUP:
			if (!inst->getInput().getMouse()->isGrabbed())
			{
				inst->getInput().getMouse()->setMouseButton(Mouse::RIGHT, False);
				inst->callBackMouseButton(Mouse::RIGHT, False, False);

				--inst->m_captureMouse;
				if (inst->m_captureMouse == 0)
					::ReleaseCapture();
			}
			return 0;

		case WM_MBUTTONDOWN:
			if (!inst->getInput().getMouse()->isGrabbed())
			{
				Bool dblClick = inst->getInput().getMouse()->setMouseButton(Mouse::MIDDLE, True);
				inst->callBackMouseButton(Mouse::MIDDLE, True, dblClick);

				// mouse capture move outside the window
				if (inst->m_captureMouse == 0)
					::SetCapture((HWND)inst->m_hWnd);
				++inst->m_captureMouse;
			}
			return 0;
		case WM_MBUTTONUP:
			if (!inst->getInput().getMouse()->isGrabbed())
			{
				inst->getInput().getMouse()->setMouseButton(Mouse::MIDDLE, False);
				inst->callBackMouseButton(Mouse::MIDDLE, False, False);

				--inst->m_captureMouse;
				if (inst->m_captureMouse == 0)
					::ReleaseCapture();
			}
			return 0;

		case WM_XBUTTONDOWN:
			if (!inst->getInput().getMouse()->isGrabbed())
			{
				if (HIWORD(wParam) == XBUTTON1)
				{
					Bool dblClick = inst->getInput().getMouse()->setMouseButton(Mouse::X1, True);
					inst->callBackMouseButton(Mouse::X1, True, dblClick);
				}
				else if (HIWORD(wParam) == XBUTTON2)
				{
					Bool dblClick = inst->getInput().getMouse()->setMouseButton(Mouse::X2, True);
					inst->callBackMouseButton(Mouse::X2, True, dblClick);
				}

				// mouse capture move outside the window
				if (inst->m_captureMouse == 0)
					::SetCapture((HWND)inst->m_hWnd);
				++inst->m_captureMouse;
			}
			return 0;
		case WM_XBUTTONUP:
			if (!inst->getInput().getMouse()->isGrabbed())
			{
				if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
				{
					inst->getInput().getMouse()->setMouseButton(Mouse::X1, False);
					inst->callBackMouseButton(Mouse::X1, False, False);
				}
				else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
				{
					inst->getInput().getMouse()->setMouseButton(Mouse::X2, False);
					inst->callBackMouseButton(Mouse::X1, False, False);
				}

				--inst->m_captureMouse;
				if (inst->m_captureMouse == 0)
					::ReleaseCapture();
			}
			return 0;

		case WM_MOUSEWHEEL:
			if (!inst->getInput().getMouse()->isGrabbed())
			{
				inst->getInput().getMouse()->setMouseWheel(-GET_WHEEL_DELTA_WPARAM(wParam));
				inst->callBackMouseWheel();
			}
			return 0;

		case WM_KEYDOWN:
			if (!inst->getInput().getKeyboard()->isGrabbed())// && !(lParam & (1 << 30)))
			{
				VKey key = VKey(translateWmKey(wParam, lParam));
                Bool pressed = inst->m_inputManager.getKeyboard()->setKeyState(key, True);

				// if not pressed we have a key repeat by the system
				// to have this state we have to comment !(lParam & (1 << 30))

                inst->callBackKey(key, key, pressed, !pressed);

				// need delete and new line caracters
				if (key == KEY_DELETE)
                    inst->callBackCharacter(key, key, static_cast<WChar>(127), !pressed);
				else if (key == KEY_RETURN || key == KEY_NUMPAD_ENTER)
                    inst->callBackCharacter(key, key, L'\n', !pressed);
			}
			return 0;
		
		case WM_KEYUP:
			if (!inst->getInput().getKeyboard()->isGrabbed())
			{
				VKey key = VKey(translateWmKey(wParam, lParam));
                inst->getInput().getKeyboard()->setKeyState(key, False);

                inst->callBackKey(key, key, False, False);
			}
			return 0;

//		case WM_UNICHAR:
		case WM_CHAR:
			switch (wParam)
			{
				case 0x00:
					break;
				case 0x0A:
					// Process a linefeed.
					break;
				case 0x1B:
					// Process an escape.
					break;
				default:
					{
						// !(lParam & (1 << 30))) == TRUE mean repeat

						VKey key = VKey((lParam << 16) & 8);
						Bool repeat = inst->getInput().getKeyboard()->isKeyDown(key);// !(lParam & (1 << 30)))
						//Bool pressed = inst->m_inputManager.getKeyboard()->setKeyState(key, !(lParam & 1 << 31));
						
                        inst->callBackCharacter(key, key, static_cast<WChar>(wParam), repeat);// !pressed);
					}
					break;
			}
			return 0;

		/*case WM_ENTERMENULOOP:
			inst->processEvent(EVT_INPUT_FOCUS_LOST, eventData);
			break;

		case WM_EXITMENULOOP:
			inst->processEvent(EVT_INPUT_FOCUS_GAIN, eventData);
			break;*/

		/*case WM_ACTIVATE:
			if (LOWORD(wParam) != WA_INACTIVE)
				inst->processEvent(EVT_INPUT_FOCUS_GAIN, eventData);
			return 0;*/
	}

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

#endif // O3D_WIN32

