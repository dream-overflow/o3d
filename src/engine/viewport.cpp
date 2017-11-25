/**
 * @file viewport.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/viewport.h"

#include "o3d/core/classfactory.h"
#include "o3d/core/architecture.h"
#include "o3d/core/objects.h"
#include "o3d/engine/gl.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/shadow/shadowvolumeforward.h"
#include "o3d/engine/visibility/visibilitymanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/picking.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// ViewPort
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_ABSTRACT_CLASS1(ViewPort, ENGINE_VIEWPORT, SceneEntity)

// constructor
ViewPort::ViewPort(BaseObject *parent, Camera* camera, SceneDrawer *drawer) :
		SceneEntity(parent),
		m_xpos(0.f),
		m_ypos(0.f),
		m_width(1.f),
		m_height(1.f),
		m_percent(True),
		m_isActive(True),
		m_camera(this, camera),
		m_drawer(this, drawer),
		m_duration(0.0f)
{
	if (!m_drawer.isValid())
		m_drawer.set(this, new ShadowVolumeForward(this));
}

// destructor
ViewPort::~ViewPort()
{
}

// draw
void ViewPort::draw()
{
	if (m_camera && m_drawer)
		m_drawer->draw();
}

// draw picking
void ViewPort::drawPicking()
{
	if (m_camera && m_drawer)
		m_drawer->drawPicking();
}

// define the viewing camera
void ViewPort::setCamera(Camera* camera)
{
	m_camera = camera;
}

// Set a scene drawer.
void ViewPort::setSceneDrawer(SceneDrawer* drawer)
{
	m_drawer = drawer;
}

// enable the viewport activity
void ViewPort::enable()
{
	m_isActive = True;
}

// disable the viewport activity
void ViewPort::disable()
{
	m_isActive = False;
}

// toggle the viewport activity
Bool ViewPort::toggle()
{
    return (m_isActive = !m_isActive);
}

Bool ViewPort::getActivity() const
{
    return m_isActive;
}

// enable the percent mode for size
void ViewPort::enablePercent()
{
	m_percent = True;
}

// disable the percent mode for size
void ViewPort::disablePercent()
{
	m_percent = False;
}

// define the viewport position and size
void ViewPort::setSize(
	Float _xpos,
	Float _ypos,
	Float _width,
	Float _height)
{
	m_xpos = _xpos;
	m_ypos = _ypos;
	m_width = _width;
	m_height = _height;
}
