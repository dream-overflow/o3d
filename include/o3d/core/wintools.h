/**
 * @file wintools.h
 * @brief Win32 miscs wrapper.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-08-31
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WINTOOLS_H
#define _O3D_WINTOOLS_H

#include "string.h"
#include "memorydbg.h"

#ifdef O3D_WINDOWS
#include <windows.h>
//#include <WinDef.h>

namespace o3d {

typedef WNDPROC _WNDPROC;

/**
 * @brief Contain static methods useful to manage winapi.
 */
class O3D_API WinTools
{
public:

	//! Create a simple window. Use pop-up for fullscreen.
    //! @param hInstance Handle instance (mostly null).
	//! @param title String containing the window title.
	//! @param icon String defining the file name of a BMP icon to use, or empty string for none.
	//! @param width Width of the window client area.
	//! @param height Height of the window client area.
	//! @param wndProc Window callback function.
	//! @param fullScreen If TRUE pop-up window.
	//! @param resizable TRUE for resizable (maximize button) style.
	//! @param flags Optional window flags.
	static _HWND createWindow(
			_HINSTANCE hInstance,
			const String &title,
			const String &icon,
            UInt32 width,
            UInt32 height,
			_WNDPROC wndProc,
			Bool fullScreen = False,
			Bool resizable = False,
			UInt32 flags = 0);

	//! Destroy a window.
	static void destroyWindow(_HWND hWnd);

	//! Unregister Window Class.
	static void unregisterWindowClass(_HINSTANCE hInstance, const String &wnd);

	//! Change the style of a window.
	//! @param hWnd Window handle.
	//! @param flags Additional user style.
	//! @param fullScreen TRUE for full screen style.
	//! @param resizable TRUE for resizable (maximize button) style.
	static void changeWindowStyle(
		_HWND hWnd,
		UInt32 flags,
		Bool fullScreen,
		Bool resizable,
		Int32 &width,
		Int32 &height);

	//! Resize a window given its client area size in pixels, and the style (fullScreen or not).
	//! @param hWnd Window handle.
	//! @param clientWidth Window client area width in pixels.
	//! @param clientHeight Window client area height in pixels.
	static void resizeWindow(
		_HWND hWnd,
		Int32 clientWidth,
		Int32 clientHeight);

	//! Post a message to the message queue for a win32 window.
	//! @param hWnd Window handle.
	//! @param msg Message type.
	//! @param v1 Correspond to the lParam.
	//! @param v2 Correspond to the wParam.
	//! @return 0 for no error.
	static Int32 sendMessage(
		_HWND hWnd,
		UInt32 msg,
		UInt32 v1,
		Int32 v2);

	//! Center the window from this parent.
	//! @param hWnd Window handle to center.
	static void centerWindow(_HWND hWnd);

	//! Return the window client size.
	static void getWindowClientSize(_HWND hWnd, Int32 &width, Int32 &height);

	//! Create an icon handle from a bmp file
	//! @param filePath the path of the bitmap file
	//! @param width the desired width of the image
	//! @param height the desired height of the image
	//! @param colorTransparency the color corresponding to transparent pixels (-1 means that
	//!                          this color will be the first pixel of the bitmap)
	//! @return a null handle if the filepath is not valid or if the size are not valid
	static HICON createIconFromBitmap(
			const String &filePath,
			Int32 width,
			Int32 height,
			COLORREF colorTransparency = COLORREF(-1));
};

} // namespace o3d

#endif // O3D_WINDOWS

#endif // _O3D_WINTOOLS_H
