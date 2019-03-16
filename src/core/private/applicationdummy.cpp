/**
 * @file applicationdummy.cpp
 * @brief
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2019-03-16
 * @copyright Copyright (c) 2001-2019 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/application.h"

#ifdef O3D_DUMMY

using namespace o3d;

void Application::apiInitPrivate()
{

}

void Application::apiQuitPrivate()
{

}

// Run the application main loop.
void Application::runPrivate(oBool runOnce)
{

}

// Push a user application event.
void Application::pushEventPrivate(EventType type, _HWND hWnd, void *data)
{

}

#endif // O3D_DUMMY
