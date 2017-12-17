/**
 * @file wintools.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"

#include "o3d/core/architecture.h"
#include "o3d/core/wintools.h"

#ifdef O3D_WINDOWS

#include "o3d/core/debug.h"
#include "o3d/core/fileinfo.h"

using namespace o3d;

// Create a normal window. Pop-up window for if fullScreen is true.
_HWND WinTools::createWindow(
        _HINSTANCE hInstance,
        const String &title,
        const String &icon,
        UInt32 width,
        UInt32 height,
        _WNDPROC wndProc,
        Bool fullScreen,
        Bool resizable,
        UInt32 flags)
{
	WNDCLASSEXW wndClass;

	ZeroMemory(&wndClass, sizeof(wndClass));

	wndClass.cbSize = sizeof(WNDCLASSEXW);
	wndClass.lpfnWndProc = wndProc;

    if (hInstance == nullptr) {
        wndClass.hInstance = (HINSTANCE)GetModuleHandleW(nullptr);
    } else {
		wndClass.hInstance = (HINSTANCE)hInstance;
    }

	wndClass.lpszClassName = title.getData();
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    // Takes the first pixel as transparent color by default
    wndClass.hIconSm = createIconFromBitmap(icon, 16, 16);
	wndClass.hIcon = createIconFromBitmap(icon, 32, 32);

    if (RegisterClassExW(&wndClass) == 0) {
		O3D_ERROR(E_InvalidResult(String("Error during windows class registration. Code = ") << UInt32(GetLastError())));
    }

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    DWORD exStyle = WS_EX_APPWINDOW;  // | WS_EX_WINDOWEDGE;

    if (resizable) {
		style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }

    if (fullScreen) {
		style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		exStyle = WS_EX_APPWINDOW;
	}

	// users flags
	style |= flags;

	RECT rect;
	rect.left = 0;
	rect.right = width;
	rect.top = 0;
	rect.bottom = height;

	AdjustWindowRectEx(&rect, style, FALSE, exStyle);
	
	HWND hWnd = CreateWindowExW(
			exStyle,
			title.getData(),
			title.getData(),
			style,
			0,
			0,
			rect.right-rect.left,
			rect.bottom-rect.top,
			NULL,
			NULL,
			(HINSTANCE)hInstance,
			NULL);

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	WinTools::centerWindow((_HWND)hWnd);

	return (_HWND)hWnd;
}

// Destroy a window.
void WinTools::destroyWindow(_HWND hWnd)
{
    if (!hWnd) {
        return;
    }

    // WCHAR className[128];
    // ::GetClassNameW((HWND)hWnd, className, 128);
	::DestroyWindow((HWND)hWnd);
    // ::UnregisterClassW(className, (HINSTANCE)GetModuleHandleW(nullptr));
}

// Unregister Window Class.
void WinTools::unregisterWindowClass(_HINSTANCE hInstance, const String &wnd)
{
    if (hInstance == nullptr) {
        UnregisterClassW(wnd.getData(), (HINSTANCE)GetModuleHandleW(nullptr));
    } else {
		UnregisterClassW(wnd.getData(), (HINSTANCE)hInstance);
    }
}

void WinTools::changeWindowStyle(
	_HWND hWnd,
	UInt32 flags,
	Bool fullScreen,
	Bool resizable,
	Int32 &width,
	Int32 &height)
{
	LONG_PTR exStyle;
	LONG_PTR style;

    if (!fullScreen) {
        style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		exStyle = WS_EX_APPWINDOW;// | WS_EX_WINDOWEDGE;

        if (resizable) {
			style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
        }

		RECT rect;
		rect.left = 0;
		rect.right = width;
		rect.top = 0;
		rect.bottom = height;

		AdjustWindowRectEx(&rect, (DWORD)style, FALSE, (DWORD)exStyle);

		width = rect.right-rect.left;
		height = rect.bottom-rect.top;
    } else {
		style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		exStyle = WS_EX_APPWINDOW;
	}

	style |= flags;

	::SetWindowLongPtr(reinterpret_cast<HWND>(hWnd), GWL_STYLE, style);
	::SetWindowLongPtr(reinterpret_cast<HWND>(hWnd), GWL_EXSTYLE, exStyle);
}

// Send an event
Int32 WinTools::sendMessage(_HWND hWnd, UInt32 msg, UInt32 v1, Int32 v2)
{
	return (Int32)::SendMessage((HWND)hWnd, msg, v1, v2);
}

// Center the window from this parent.
void WinTools::centerWindow(_HWND hWnd)
{
	HWND hwndParent;
	RECT rect, rectP;
	int width, height;
	int screenwidth, screenheight;
	int x, y;

	// make the window relative to its parent
	hwndParent = GetParent((HWND)hWnd);

	GetWindowRect((HWND)hWnd,&rect);

	screenwidth  = GetSystemMetrics(SM_CXSCREEN);
	screenheight = GetSystemMetrics(SM_CYSCREEN);

	if (hwndParent) // parent
	{
		GetWindowRect(hwndParent,&rectP);
	}
	else // no parent
	{
		rectP.left = 0;
		rectP.right = screenwidth;
		rectP.top = 0;
		rectP.bottom = screenheight;
	}

	width  = rect.right - rect.left;
	height = rect.bottom - rect.top;

	x = ((rectP.right-rectP.left) - width) / 2 + rectP.left;
	y = ((rectP.bottom-rectP.top) - height) / 2 + rectP.top;

	SetWindowPos(
		(HWND)hWnd,
		0,
		x,
		y,
		width,
		height,
		/*SWP_SHOWWINDOW | */SWP_NOZORDER | SWP_NOCOPYBITS | SWP_NOSIZE);
}

// Resize a window given its client area size in pixels.
void WinTools::resizeWindow(
	_HWND hWnd,
	Int32 clientWidth,
	Int32 clientHeight)
{
	DWORD exStyle = GetWindowLong((HWND)hWnd, GWL_EXSTYLE);
	DWORD style = GetWindowLong((HWND)hWnd, GWL_STYLE);

	if (!(style & WS_OVERLAPPED))
	{
		RECT rect;
		GetWindowRect((HWND)hWnd, &rect);

		rect.left = 0;
		rect.top = 0;
		rect.right = clientWidth+rect.left;
		rect.bottom = clientHeight+rect.top;

		AdjustWindowRectEx(&rect, style, FALSE, exStyle);

		SetWindowPos(
			(HWND)hWnd,
			HWND_NOTOPMOST,
			rect.left,
			rect.top,
			rect.right-rect.left,
			rect.bottom-rect.top,
			/*SWP_SHOWWINDOW | */SWP_NOZORDER | SWP_NOCOPYBITS | SWP_NOMOVE);
	}
	else
	{
		SetWindowPos(
			(HWND)hWnd,
			HWND_TOPMOST,
			0,
			0,
			clientWidth,
			clientHeight,
			SWP_SHOWWINDOW | SWP_NOCOPYBITS | SWP_NOMOVE);
	}
}

// Return the window client size.
void WinTools::getWindowClientSize(_HWND hWnd, Int32 &width, Int32 &height)
{
	RECT rect;

	GetClientRect((HWND)hWnd, &rect);

	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}

// Create an icon handle from a bmp file
HICON WinTools::createIconFromBitmap(
		const String & filePath,
		Int32 width,
		Int32 height,
		COLORREF colorTransparency)
{
	if (!filePath.isEmpty())
	{
		DiskFileInfo fileInfo(filePath);

		if (fileInfo.getFileExt().compare("bmp", String::CASE_INSENSITIVE) != 0)
		{
			O3D_ERROR(E_InvalidFormat("Icon must be a bmp file."));
		}

        if (!fileInfo.exists())
			return NULL;
	}
	else
		return NULL;

	HBITMAP hBitmap = (HBITMAP)::LoadImageW(
			NULL,
			filePath.getData(),
			IMAGE_BITMAP,
			width,
			height,
			LR_LOADFROMFILE);

	BITMAP bitmapObject;
	if (::GetObject(hBitmap, sizeof(BITMAP), &bitmapObject) != sizeof(BITMAP))
		O3D_ERROR(E_InvalidResult("Unable to retrieve the bitmap object from its handle."));

	// Creations of various DC to manipulate bitmaps
	HDC hDC	= ::GetDC(NULL);
	HDC hBitmapDC = ::CreateCompatibleDC(hDC);
	HDC hAndMaskDC = ::CreateCompatibleDC(hDC);
	HDC hXorMaskDC = ::CreateCompatibleDC(hDC);

	// Creation of two bitmap which will contains the AND mask and the XOR mask
	// The xor mask is in fact simply the colored bitmap, but where all transparent
	// pixel were replaced by 0.
	HBITMAP hAndMask = ::CreateCompatibleBitmap(hDC, bitmapObject.bmWidth, bitmapObject.bmHeight);
	HBITMAP hXorMask = ::CreateCompatibleBitmap(hDC, bitmapObject.bmWidth, bitmapObject.bmHeight);

	// Association of DCs and bitmaps
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hBitmapDC, hBitmap);
	HBITMAP hOldAndMask = (HBITMAP)::SelectObject(hAndMaskDC, hAndMask);
	HBITMAP hOldXorMask = (HBITMAP)::SelectObject(hXorMaskDC, hXorMask);

	if (	(hAndMask == NULL) ||
			(hXorMask == NULL) ||
			(hDC == NULL) ||
			(hBitmapDC == NULL) ||
			(hAndMaskDC == NULL) ||
			(hXorMaskDC == NULL) ||
			(hOldBitmap == NULL) ||
			(hOldAndMask == NULL) ||
			(hOldXorMask == NULL))
		O3D_ERROR(E_InvalidResult("Error during creation of icon masks."));

	// If no transparency color is provided, the first pixel of the bitmap is used
	const COLORREF TransparentPixel =
        (colorTransparency != (COLORREF)(-1) ? colorTransparency : ::GetPixel(hBitmapDC, 0, 0));

	for (Int32 j = 0 ; j < bitmapObject.bmHeight ; ++j)
	{
		for (Int32 i = 0 ; i < bitmapObject.bmWidth ; ++i)
		{
			COLORREF CurrentPixel = ::GetPixel(hBitmapDC, i, j);

			if(CurrentPixel == TransparentPixel)
			{
				::SetPixel(hAndMaskDC,i,j,	RGB(255,255,255));
				::SetPixel(hXorMaskDC,i,j,	RGB(0,0,0));
			}
			else
			{
				::SetPixel(hAndMaskDC,i,j,	RGB(0,0,0));
				::SetPixel(hXorMaskDC,i,j,	CurrentPixel);
			}
		}
	}

	// Here, we just follow msdn's orders:
	// "An application should always replace a new object with the original,
	// default object after it has finished drawing with the new object"
	::SelectObject(hBitmapDC, hOldBitmap);
	::SelectObject(hAndMaskDC, hOldAndMask);
	::SelectObject(hXorMaskDC, hOldXorMask);

	::DeleteDC(hBitmapDC);
	::DeleteDC(hAndMaskDC);
	::DeleteDC(hXorMaskDC);

	::ReleaseDC(NULL,hDC);

	ICONINFO iconInfos;
	iconInfos.fIcon = TRUE;	// TRUE for icon, FALSE for cursor
	iconInfos.xHotspot = 0;	// Ignored in the case of icon
	iconInfos.yHotspot = 0;	// Ignored in the case of icon
	iconInfos.hbmMask = hAndMask;
	iconInfos.hbmColor = hXorMask;

	HICON hIcon = CreateIconIndirect(&iconInfos);

	::DeleteObject(hAndMask);
	::DeleteObject(hXorMask);
	::DeleteObject(hBitmap);

	return hIcon;
}

#endif // O3D_WINDOWS
