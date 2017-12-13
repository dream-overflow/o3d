/**
 * @file touchscreenandroid.h
 * @brief Touch screen input controller specialization for Android
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"

#include <math.h>
#include "o3d/core/touchscreen.h"
#include "o3d/core/debug.h"

#ifdef O3D_ANDROID

using namespace o3d;

TouchScreen::TouchScreen(AppWindow *appWindow, Int32 xlimit, Int32 ylimit) :
    Input(),
    m_appWindow(appWindow)
{

}

TouchScreen::~TouchScreen()
{

}

void TouchScreen::update()
{

}

void TouchScreen::acquire()
{

}

void TouchScreen::release()
{

}

#endif
