/**
 * @file displayandroid.cpp
 * @brief
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/base.h"

/* ONLY IF O3D_ANDROID IS SELECTED */
#ifdef O3D_ANDROID

#include "o3d/core/appwindow.h"
#include "o3d/core/display.h"
#include "o3d/core/debug.h"

// #include <android/

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
        m_appWindow = nullptr;
    }
}

// list all display mode for selected device
void Display::listDisplayModes()
{
    m_desktop = m_currentMode = m_modes.end();
}

#endif // O3D_ANDROID
