/**
 * @file appwindowdummy.cpp
 * @brief
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2019-03-16
 * @copyright Copyright (c) 2001-2019 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/appwindow.h"

// ONLY IF O3D_DUMMY IS SELECTED
#ifdef O3D_DUMMY

#include "o3d/core/debug.h"
#include "o3d/core/application.h"
#include "o3d/core/display.h"

using namespace o3d;

// Window settings
void AppWindow::setTitle(const String &title)
{
    // if window created
    m_title = title;
}

//void AppWindow::setIcon(const Image &icon)
//{
//    if ((icon.getPixelFormat() != PF_RGB_8) && (icon.getPixelFormat() != PF_RGBA_8)) {
//        O3D_ERROR(E_InvalidFormat("Icon must be RGB888 or RGBA8888"));
//    }

//    m_icon = icon;
//}

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
    m_clientWidth = clientWidth;
    m_clientHeight = clientHeight;
}

// Apply window settings
void AppWindow::applySettings(Bool fullScreen)
{
    if (!isSet()) {
        O3D_ERROR(E_InvalidPrecondition("Window not set"));
    }

    // window size
    m_width = m_clientWidth;
    m_height = m_clientHeight;

    // set title and icon
    setTitle(m_title);
    // setIcon(m_icon);

    Application::addAppWindow(this);

    m_running = True;
}

// destroy the window.
void AppWindow::destroy()
{
    // m_icon.destroy();
    m_running = False;
}

Bool AppWindow::pasteToClipboard(const String &text, Bool primary)
{
    return False;
}

String AppWindow::copyFromClipboard(Bool primary)
{
    return "";
}

// Set to full screen
void AppWindow::setFullScreen(Bool fullScreen, UInt32 /*freq*/)
{

}

// Swap buffers
void AppWindow::swapBuffers()
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

// Process the main loop
void AppWindow::processEvent(EventType eventType, EventData &eventData)
{

}

void AppWindow::processMotionEvent(EventType eventType, MotionEventData &eventData)
{

}

#endif // O3D_DUMMY
