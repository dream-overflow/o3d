/**
 * @file visibilitymanager.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/visibility/visibilitymanager.h"

#include "o3d/engine/visibility/octree.h"
#include "o3d/engine/visibility/quadtree.h"
#include "o3d/engine/visibility/visibilitybasic.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(VisibilityManager, ENGINE_VISIBILITY_MANAGER, SceneEntity)

// constructor
VisibilityManager::VisibilityManager(BaseObject *parent) :
	SceneEntity(parent),
    m_global(QUADTREE),
    m_globalController(nullptr),
    m_maxDistance(10000.0f),
	m_useMaxDistance(True),
	m_useMaxZFar(True),
	m_drawList(4096, 4096)
{
	setGlobal(m_global, 8, 128.f);
}

// destructor
VisibilityManager::~VisibilityManager()
{
	deletePtr(m_globalController);
    // @todo delete specifics controllers
}

// set global visibility controller
void VisibilityManager::setGlobal(
		VisibilityManager::VisibilityType type,
		UInt32 halfSize,
		Float zoneSize)
{
	deletePtr(m_globalController);

    switch (type) {
		case DISTANCE:
			m_globalController = new VisibilityBasic(this);
			break;

		case QUADTREE:
			m_globalController = new Quadtree(this, halfSize, zoneSize);
			break;

		case OCTREE:
            m_globalController = new Octree(this); //, halfSize, zoneSize);
			break;

		default:
			break;
	}

	m_global = type;
}

// add an object to the current visibility mode
void VisibilityManager::addObject(SceneObject *object)
{
	m_globalController->addObject(object);
    // @todo search in specifics controllers
}

// add/update an object to the current visibility mode
void VisibilityManager::updateObject(SceneObject *object)
{
	m_globalController->updateObject(object);
    // @todo search in specifics controllers
}

// remove an object of a its specified structure
void VisibilityManager::removeObject(SceneObject *object)
{
	m_globalController->removeObject(object);
    // @todo search in specifics controllers
}

// Process the visibility determination.
void VisibilityManager::processVisibility()
{
    // clear the draw list and effective light list
	m_drawList.forceSize(0);
    m_effectiveLightList.forceSize(0);

	VisibilityInfos info = {
			getScene()->getActiveCamera()->getAbsoluteMatrix().getTranslation(),
			getScene()->getActiveCamera()->getAbsoluteMatrix().getZ(),
			getMaxDistance(),
			m_useMaxDistance };

	// check for visible objects
	m_globalController->checkVisibleObject(info);
}

void VisibilityManager::draw(const DrawInfo &drawInfo)
{
	// draw the global controller
    if (drawInfo.pass == DrawInfo::AMBIENT_PASS/*SYMBOLIC_PASS*/) {
		m_globalController->draw();
	}

	SceneObject * sceneObject;

    // according to the draw list check if each objet lies with the frustum and draw it
	Int32 numObject = m_drawList.getSize();
    for (Int32 i = 0; i < numObject; ++i) {
		sceneObject = m_drawList[i];

        if (sceneObject->checkFrustum(*getScene()->getFrustum()) != Geometry::CLIP_OUTSIDE) {
			sceneObject->draw(drawInfo);
        } else {
            // System::Print(String("Visibility manager culling ") + sceneObject->getName(), "");
        }
    }
}

const TemplateArray<Light *> VisibilityManager::getEffectiveLights() const
{
    return m_effectiveLightList;
}

void VisibilityManager::setMaxDistance(Float max)
{
    m_maxDistance = max;
    m_useMaxDistance = True;
    m_useMaxZFar = False;
}

Float VisibilityManager::getMaxDistance() const
{
    if (m_useMaxZFar && getScene()->getActiveCamera()) {
		return getScene()->getActiveCamera()->getZfar();
    } else {
        return m_maxDistance;
    }
}

void VisibilityManager::useZFarMaxDistance()
{
    m_useMaxZFar = m_useMaxDistance = True;
}

void VisibilityManager::useDefinedMaxDistance()
{
    m_useMaxDistance = True;
    m_useMaxZFar = False;
}
