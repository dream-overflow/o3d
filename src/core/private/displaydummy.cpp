/**
 * @file displaydummy.cpp
 * @brief
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2019-03-16
 * @copyright Copyright (c) 2001-2019 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/base.h"

/* ONLY IF O3D_DUMMY IS SELECTED */
#ifdef O3D_DUMMY

#include "o3d/core/application.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/display.h"
#include "o3d/core/debug.h"

using namespace o3d;

// change display mode
void o3d::Display::setDisplayMode(AppWindow *appWindow, CIT_DisplayModeList mode)
{

}

// restore to desktop display mode
void o3d::Display::restoreDisplayMode()
{

}

// list all display mode for selected device
void o3d::Display::listDisplayModes()
{

}

#endif // O3D_DUMMY
