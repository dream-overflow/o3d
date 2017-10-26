/**
 * @file map2disoobjectslayer.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/map2d/map2disoobjectslayer.h"
#include "o3d/engine/map2d/map2dvisibility.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/context.h"

#include <algorithm>

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Map2dIsoObjectsLayer, ENGINE_MAP_2D_OBJECT_LAYER, Map2dLayer)

Map2dIsoObjectsLayer::Map2dIsoObjectsLayer(
		BaseObject *parent,
		const Box2i &area,
		UInt32 maxDepth,
		UInt32 maxObjectsPerCell) :
	Map2dLayer(parent),
	m_sort(True),
	m_box(area),
	m_visibility(nullptr)
{
	m_visibility = new Map2dVisibility(
				area.pos(),
				nextPow2(max(area.width(), area.height())),
				max((UInt32)1, maxDepth),
				maxObjectsPerCell);
}

Map2dIsoObjectsLayer::~Map2dIsoObjectsLayer()
{
	m_visibility->clear();

	IT_Map2dObjectList it = m_objects.begin();

	while (it != m_objects.end())
	{
		deletePtr(*it);
		++it;
	}

	deletePtr(m_visibility);
}

Bool Map2dIsoObjectsLayer::deleteChild(BaseObject *child)
{
	if (child)
	{
		if (child->getParent() != this)
			O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
		else
		{
			// object should be type of Map2dObject
			Map2dObject *object = dynamicCast<Map2dObject*>(child);
			if (object)
			{
				IT_Map2dObjectList it = m_objects.begin();
				for (; it != m_objects.end(); ++it)
				{
					if ((*it) == object)
						break;
				}

				// remove the object of the son list
				if (it != m_objects.end())
				{
					// remove it from the quadtree
					if (m_visibility != nullptr)
						m_visibility->removeObject(object);

					m_sort = True;

					m_objects.erase(it);
					object->setNode(NULL);
				}

				deletePtr(object);
			}
			else
			{
				// otherwise simply delete it
				deletePtr(child);
			}

			return True;
		}
	}
	return False;
}

UInt32 Map2dIsoObjectsLayer::getNumElt() const
{
	return m_objects.size();
}

const Transform *Map2dIsoObjectsLayer::getTransform() const
{
	return nullptr;
}

Transform *Map2dIsoObjectsLayer::getTransform()
{
	return nullptr;
}

void Map2dIsoObjectsLayer::update()
{
	if (!getActivity())
		return;

	clearUpdated();
	Bool dirty = False;

	if (getNode() && getNode()->hasUpdated())
	{
		// the parent has change so the child need to be updated
		dirty = True;
	}

	if (dirty)
	{
		setUpdated();
	}

	// check if a sort is necessary at the next draw
	m_sort |= m_visibility->hasUpdated();
	m_visibility->clearUpdated();
/*
	// update each son (recursively if necessary) TODO optimize that
	for (IT_Map2dObjectList it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		Map2dObject *object = (*it);

		if (object->getActivity())
		{
			// compute object absolute matrix
			object->update();

			if (object->hasUpdated())
			{
				// only for drawable and dynamic objects
				if (object->hasDrawable())
				{
					m_visibility->updateObject(object);
					m_sort = True;
				}
			}
		}
	}*/
}

void Map2dIsoObjectsLayer::draw(const DrawInfo &drawInfo)
{
	if (!m_capacities.getBit(STATE_ACTIVITY) || !m_capacities.getBit(STATE_VISIBILITY))
		return;

	setUpModelView();

	if (getScene()->getDrawObject(Scene::DRAW_MAP_2D_LAYER))
	{
		// TODO Symbolics a quad in red
	}

	Camera *camera = getScene()->getActiveCamera();

	// process to a sort on the visible area if necessary
    if (m_sort || camera->isCameraChanged())
	{
		Vector3 camPos = camera->getAbsoluteMatrix().getTranslation();

		Box2i viewport(
					camPos.x() - (-camera->getLeft() + camera->getRight()) / 2,
					camPos.y() - (camera->getBottom() - camera->getTop()) / 2,
					-camera->getLeft() + camera->getRight(),
					camera->getBottom() - camera->getTop());

		m_drawList.clear();

		/*UInt32 rejected = */m_visibility->checkVisibleObject(viewport);

        T_Map2dObjectList &drawList = m_visibility->getDrawList();
        m_drawList.reserve(drawList.size());

        // inject for sort
        for (Map2dObject *object : drawList)
        {
            m_drawList.push_back(object);
        }

        std::sort(m_drawList.begin(), m_drawList.end(), &Map2dObject::compare);

        m_sort = False;

        //System::print(String::print("rejected object=%u", rejected), "");
        //System::print(m_visibility->getTreeView(), "");
	}

    for (Map2dObject *object : m_drawList)
    {
        object->draw(drawInfo);
    }
}

UInt32 Map2dIsoObjectsLayer::getNumSon() const
{
	return m_objects.size();
}

SceneObject* Map2dIsoObjectsLayer::findSon(const String &name)
{
	if (getName() == name)
		return this;

	for (IT_Map2dObjectList it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		SceneObject *object = (*it);
		if (object->isNodeObject())
		{
			SceneObject *result = ((BaseNode*)object)->findSon(name);
			if (result)
				return result;
		}
		else if (object->getName() == name)
			return object;
	}

	return NULL;
}

const SceneObject* Map2dIsoObjectsLayer::findSon(const String &name) const
{
	if (getName() == name)
		return this;

	for (CIT_Map2dObjectList it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		const SceneObject *object = (*it);
		if (object->isNodeObject())
		{
			const SceneObject *result = ((BaseNode*)object)->findSon(name);
			if (result)
				return result;
		}
		else if (object->getName() == name)
			return object;
	}

	return NULL;
}

Bool Map2dIsoObjectsLayer::findSon(SceneObject *object) const
{
	if (this == object)
		return True;

	for (CIT_Map2dObjectList it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		const SceneObject *search = (*it);
		if (search->isNodeObject())
		{
			Bool result = ((BaseNode*)search)->findSon(object);
			if (result)
				return True;
		}
		else if (search == object)
			return True;
	}

	return False;
}

const T_Map2dObjectList &Map2dIsoObjectsLayer::getObjects()
{
	return m_objects;
}

const T_Map2dObjectList& Map2dIsoObjectsLayer::getVisiblesObjects()
{
	return m_visibility->getDrawList();
}

Bool Map2dIsoObjectsLayer::isObjectIntersect(const Box2i &box) const
{
	return m_visibility->isObjectIntersect(box);
}

Bool Map2dIsoObjectsLayer::isObjectBaseIntersect(const Map2dObject *from) const
{
	return m_visibility->isObjectBaseIntersect(from);
}

Map2dObject* Map2dIsoObjectsLayer::addObject(
        const String &name,
        const Vector2i &pos,
        const Rect2i &baseRect,
        Map2dTileSet *tileSet,
        UInt32 tileId)
{
	Map2dObject *object = new Map2dObject(this);
	object->setName(name);
	object->setNode(this);
	object->setTile(tileSet, tileId);
    object->setBaseRect(baseRect);
	object->setPos(pos);

	m_objects.push_back(object);

	m_sort = True;

	m_visibility->addObject(object);

	return object;
}

// remove a specified son
void Map2dIsoObjectsLayer::removeObject(Map2dObject *object)
{
	IT_Map2dObjectList it = m_objects.begin();
	for (; it != m_objects.end(); ++it)
	{
		if ((*it) == object)
			break;
	}

	if (it == m_objects.end())
	{
		O3D_ERROR(E_InvalidParameter("Object not found"));
	}
	else
	{
		m_visibility->removeObject(object);

		// remove the object of the son list
		m_objects.erase(it);

		// no node
		object->setParent(getScene());
		object->setNode(NULL);
		object->setPersistant(False);

		m_sort = True;
	}
}

void Map2dIsoObjectsLayer::updateObject(Map2dObject *object)
{

}

//void Map2dIsoObjectsLayer::moveObject(Map2dObject *object, const Vector2i &pos)
//{
//	if (object != nullptr)
//	{
//		object->setPos(pos);
//		m_visibility->updateObject(object);
//	}
//}

// Remove all sons (delete objects if no longer used)
void Map2dIsoObjectsLayer::deleteAllObjects()
{
	IT_Map2dObjectList it = m_objects.begin();

	while (it != m_objects.end())
	{
		deletePtr(*it);

		++it;
	}

	m_objects.clear();
	m_visibility->clear();

	m_sort = True;
}

