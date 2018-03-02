/**
 * @file visibilitybasic.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/visibility/visibilitybasic.h"

#include "o3d/engine/visibility/visibilitymanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/primitive/primitivemanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"

#include <algorithm>

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(VisibilityBasic, ENGINE_VISIBILITY_BASIC, VisibilityABC)

//---------------------------------------------------------------------------------------
// class VisibilityBasic
//---------------------------------------------------------------------------------------
VisibilityBasic::VisibilityBasic(
	BaseObject *parent,
	const Vector3 &position,
	const Vector3 &size) :
		VisibilityABC(parent,position,size),
		m_objectList(),
		m_useMaxDistance(False)
{
}

// destructor
VisibilityBasic::~VisibilityBasic()
{
}

// add an object (we suppose that it doesn't exist)
void VisibilityBasic::addObject(SceneObject *object)
{
    O3D_ASSERT(object != nullptr);

	if (std::find(m_objectList.begin(), m_objectList.end(), object) != m_objectList.end())
	{
		// The object is already in the set
		O3D_ERROR(E_ValueRedefinition("Attempt to add an object but it is already present"));
		return;
	}

	m_objectList.push_front(SmartObject<SceneObject>((BaseObject*)this, object));
}

// remove an object
Bool VisibilityBasic::removeObject(SceneObject *object)
{
	IT_ObjectList it = std::find(m_objectList.begin(), m_objectList.end(), object);
	//O3D_ASSERT(it != m_objectList.end());

	if (it != m_objectList.end())
	{
		m_objectList.erase(it);
		return True;
	}
	return False;
}

// update an object
void VisibilityBasic::updateObject(SceneObject *object)
{

}

// check for visible object and add it to visibility manager
void VisibilityBasic::checkVisibleObject(const VisibilityInfos & _infos)
{
	if (m_objectList.size() > 1)
	{
		Vector3 camPosition = _infos.cameraPosition;

		Float nextDistance = (camPosition - m_objectList.front()->getAbsoluteMatrix().getTranslation()).normInf();
		IT_ObjectList nextObject = m_objectList.begin();
		nextObject++;

		Float curDistance = 0.0f;
		IT_ObjectList it = m_objectList.begin();

		while (nextObject != m_objectList.end())
		{
			curDistance = nextDistance;
			nextDistance = (camPosition - (*nextObject)->getAbsoluteMatrix().getTranslation()).normInf();

			if ((_infos.viewUseMaxDistance) && (curDistance > _infos.viewMaxDistance))
			{
				// @TODO why not an event ?
				it = m_objectList.erase(it);
				nextObject++;
			}
			else
			{
				if (curDistance > nextDistance)
					std::iter_swap(it, nextObject);

				it++;
				nextObject++;
			}
		}
	}
	else if (m_objectList.size() == 1)
	{
		Float distance = (_infos.cameraPosition - (m_objectList.front())->getAbsoluteMatrix().getTranslation()).normInf();

		if ((_infos.viewUseMaxDistance) && (distance > _infos.viewMaxDistance))
			removeObject(m_objectList.front().get());
	}

	// visible objects will drawn
	for (IT_ObjectList it = m_objectList.begin() ; it != m_objectList.end() ; it++)
	{
		getScene()->getVisibilityManager()->addObjectToDraw(it->get());
	}
}

// draw the symbolic
void VisibilityBasic::draw(const DrawInfo &drawInfo)
{
    if (getScene()->getDrawObject(Scene::DRAW_BOUNDING_VOLUME)) {
		Float radius = getScene()->getVisibilityManager()->getMaxDistance();

        PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access(drawInfo);

		// setup modelview
		primitive->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());

		primitive->setColor(1.f, 1.f, 1.f);
		primitive->draw(PrimitiveManager::WIRE_SPHERE1, Vector3(radius, radius, radius));
	}
}
