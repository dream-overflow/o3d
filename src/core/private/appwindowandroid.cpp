/**
 * @file appwindowandroid.cpp
 * @brief
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/appwindow.h"

#include "o3d/core/timer.h"

// ONLY IF O3D_ANDROID IS SELECTED
#ifdef O3D_ANDROID

// #include <android/

#include "o3d/core/application.h"
#include "o3d/core/debug.h"
#include "o3d/core/display.h"

using namespace o3d;

// Window settings
void AppWindow::setTitle(const String &title)
{
    // if window created
    m_title = title;

}

void AppWindow::setIcon(const Image &icon)
{

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

}

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
    if(!isSet()) {
        O3D_ERROR(E_InvalidPrecondition("Window not set"));
    }

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

    if (fullScreen) {

    }

    if (m_resizable && !fullScreen) {

    }

    if (!window) {
        O3D_ERROR(E_InvalidResult("Unable to create the window"));
    }

    // min size hint
    if (m_minSize.x() >= 0 && m_minSize.y() >= 0) {

    }

    // max size hint
    if (m_maxSize.x() >= 0 && m_maxSize.y() >= 0) {

    }

    // get the current window pos

    // window size
    m_width = m_clientWidth;
    m_height = m_clientHeight;

//    m_PF = (_PF)SDL_GetWindowPixelFormat(window);
//    m_HDC = (_HDC)window;
//    m_hWnd = (_HWND)SDL_GetWindowID(window);

    setIcon(m_icon);

    Application::addAppWindow(this);

    // create event
    callBackCreate();

    m_running = True;
}

// Destroy the window.
void AppWindow::destroy()
{
    // delete inputs
    m_inputManager.destroy();

    m_icon.destroy();

    if (m_HDC != NULL_HDC) {
        if (isFullScreen()) {
            Display::instance()->restoreDisplayMode();
        }

        EventData event;
        processEvent(EVT_DESTROY, event);

        // @todo

        m_hWnd = NULL_HWND;
        m_HDC = NULL_HDC;
        m_PF = NULL_PF;
    }

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

    CString utf8 = text.toUtf8();
    int result = 0;

    return result == 0;
}

String AppWindow::copyFromClipboard(Bool primary)
{
    if (!m_hWnd) {
        O3D_ERROR(E_InvalidOperation("The window must be valid"));
    }

    String text;  // result


    return text;
}

// Set to full screen
void AppWindow::setFullScreen(Bool fullScreen, UInt32 freq)
{
    if (m_hWnd != NULL_HWND) {
        O3D_ERROR(E_InvalidOperation("The window must be valid"));
    }

}

// Swap buffers
void AppWindow::swapBuffers()
{
    if (m_running) {
        /// GL::swapBuffers(reinterpret_cast<xxx*>(m_HDC));
    }
}

// Process internals deferred events
void AppWindow::processEvent(EventType eventType, EventData &eventData)
{
    if (!m_running) {
        return;
    }

}

#endif // O3D_ANDROID
