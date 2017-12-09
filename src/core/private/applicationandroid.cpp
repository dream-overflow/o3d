/**
 * @file applicationandroid.cpp
 * @brief
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/application.h"
#include "o3d/core/filemanager.h"

// IF O3D_ANDROID IS SELECTED
#ifdef O3D_ANDROID

#include "o3d/core/debug.h"
#include "o3d/core/appwindow.h"

// Specific dependencies includes
// #include <android/>

using namespace o3d;

void Application::apiInitPrivate()
{

}

void Application::apiQuitPrivate()
{

}

// Run the application main loop.
void Application::runPrivate(Bool runOnce)
{

}

// Push a user application event.
void Application::pushEventPrivate(EventType type, _HWND hWnd, void *data)
{

}

void Application::getBaseNamePrivate(Int32 argc, Char **argv)
{

}

#endif // O3D_ANDROID
