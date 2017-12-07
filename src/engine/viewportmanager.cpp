/**
 * @file viewportmanager.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/screenviewport.h"
#include "o3d/engine/feedbackviewport.h"
#include "o3d/engine/viewportmanager.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ViewPortManager, ENGINE_VIEWPORT_LIST, SceneEntity)

// constructor
ViewPortManager::ViewPortManager(BaseObject *parent) :
		SceneEntity(parent),
		m_drawPicking(False),
		m_width(0),
		m_height(0)
{
}

// Delete child viewport.
Bool ViewPortManager::deleteChild(BaseObject *child)
{
	if (child)
	{
		if (child->getParent() != this)
			O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
		else
		{
			ViewPort *viewPort = o3d::dynamicCast<ViewPort*>(child);
			if (viewPort)
				return deleteViewPort(viewPort);
			else
				O3D_ERROR(E_InvalidParameter("Child is not a viewport"));
		}
	}
	return False;
}

//---------------------------------------------------------------------------------------
// Factory
//---------------------------------------------------------------------------------------

// Delete a viewport
Bool ViewPortManager::deleteViewPort(ViewPort *viewPort)
{
	if (viewPort && (viewPort->getId() != -1))
	{
		if (m_manager.find(viewPort->getId()) == viewPort)
			return m_manager.deleteElement(viewPort->getId());
	}
	return False;
}

// Add a viewport.
void ViewPortManager::addViewPort(ViewPort *viewPort, Int32 priority)
{
	if (viewPort)
	{
		if (viewPort->getId() != -1)
			O3D_ERROR(E_InvalidOperation("The viewport is already in a manager"));

		viewPort->setId(m_manager.addElement(viewPort, priority));
		viewPort->setParent(this);
	}
}

ScreenViewPort* ViewPortManager::addScreenViewPort(
		Camera* camera,
		SceneDrawer *drawer,
		Int32 priority)
{
	ScreenViewPort* viewPort = new ScreenViewPort(this, camera, drawer);
	viewPort->setId(m_manager.addElement(viewPort, priority));
	return viewPort;
}

// Add a feedback viewport
FeedbackViewPort* ViewPortManager::addFeedbackViewPort(
		Camera* camera,
		SceneDrawer *drawer,
		Texture2D* texture,
		Int32 priority)
{
	FeedbackViewPort* viewPort = new FeedbackViewPort(this, camera, drawer, texture);
	viewPort->setId(m_manager.addElement(viewPort, priority));
	return viewPort;
}

// Change the priority of a given viewport.
void ViewPortManager::changePriority(ViewPort *viewPort, Int32 priority)
{
	if (viewPort && (viewPort->getId() != -1))
	{
		if (m_manager.find(viewPort->getId()) == viewPort)
		{
			m_manager.select(viewPort->getId());
			m_manager.changePriority(priority);
		}
	}
}

// reshape for all viewports of this manager
void ViewPortManager::reshape(UInt32 width, UInt32 height)
{
	m_width = width;
	m_height = height;
}

// display all active viewports by priority
void ViewPortManager::display()
{
	PriorityManagerElt<ViewPort> *elt = m_manager.getRoot();

    while (elt != nullptr) {
		elt->Element->display(m_width, m_height);
		elt = elt->Son;
	}
}
