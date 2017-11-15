/**
 * @file map2d.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/geom/aabbox.h"
#include "o3d/geom/plane.h"
#include "o3d/geom/frustum.h"
#include "o3d/engine/map2d/map2dvisibility.h"
#include "o3d/engine/map2d/map2d.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Map2d, ENGINE_MAP_2D, BaseNode)

Map2d::Map2d(BaseObject *parent) :
	BaseNode(parent)
{
}

Map2d::~Map2d()
{
	deleteAllLayers();
}

// Delete child
Bool Map2d::deleteChild(BaseObject *child)
{
	if (child)
	{
		if (child->getParent() != this)
			O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
		else
		{
			// object should be type of Map2dLayer
			Map2dLayer *object = dynamicCast<Map2dLayer*>(child);
			if (object)
			{
				IT_LayerList it = m_layers.begin();
				for (; it != m_layers.end(); ++it)
				{
					if ((*it) == object)
						break;
				}

				// remove the object of the son list
				if (it != m_layers.end())
				{
					m_layers.erase(it);
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

UInt32 Map2d::getNumElt() const
{
	UInt32 size = m_layers.size();

	for (Map2dLayer *layer : m_layers)
	{
		size += layer->getNumElt();
	}

	return size;
}

void Map2d::draw(const DrawInfo &drawInfo)
{
	if (getActivity() && getVisibility())
	{
		// Draw each layer in the order
		for (Map2dLayer *layer : m_layers)
		{
			layer->draw(drawInfo);
		}
	}
}

UInt32 Map2d::getDrawType() const
{
	return Scene::DRAW_MAP_2D;
}

Geometry::Clipping Map2d::checkBounding(const AABBox &bbox) const
{
	return Geometry::CLIP_INSIDE;
}

Geometry::Clipping Map2d::checkBounding(const Plane &plane) const
{
	return Geometry::CLIP_INSIDE;
}

Geometry::Clipping Map2d::checkFrustum(const Frustum &frustum) const
{
	return Geometry::CLIP_INSIDE;
}

const Transform *Map2d::getTransform() const
{
	return nullptr;
}

Transform *Map2d::getTransform()
{
	return nullptr;
}

UInt32 Map2d::getNumSon() const
{
	return m_layers.size();
}

void Map2d::addLayer(Map2dLayer *layer, UInt32 pos)
{
	if (layer == nullptr)
		return;

	// begin
	if (pos == 0)
	{
		layer->setParent(this);
		layer->setNode(this);
		layer->setPersistant(True);

		m_layers.push_front(layer);

		return;
	}

	// end
	if (pos >= m_layers.size())
	{
		layer->setParent(this);
		layer->setNode(this);
		layer->setPersistant(True);

		m_layers.push_back(layer);

		return;
	}

	// others position
	UInt32 c = 0;
	auto it = m_layers.begin();
	while (c < pos)
		++it;

	layer->setParent(this);
	layer->setNode(this);
	layer->setPersistant(True);

	m_layers.insert(it, layer);
}

SceneObject* Map2d::findSon(const String &name)
{
	if (getName() == name)
		return this;

	for (IT_LayerList it = m_layers.begin(); it != m_layers.end(); ++it)
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

const SceneObject* Map2d::findSon(const String &name) const
{
	if (getName() == name)
		return this;

	for (CIT_LayerList it = m_layers.begin(); it != m_layers.end(); ++it)
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

Bool Map2d::findSon(SceneObject *object) const
{
	if (this == object)
		return True;

	for (CIT_LayerList it = m_layers.begin(); it != m_layers.end(); ++it)
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

void Map2d::update()
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

	// update each son (recursively if necessary)
	for (IT_LayerList it = m_layers.begin(); it != m_layers.end(); ++it)
	{
		Map2dLayer *layer = (*it);

		if (layer->getActivity())
		{
			// compute object absolute matrix
			layer->update();
		}
	}
}

void Map2d::removeLayer(Map2dLayer *layer)
{
	IT_LayerList it = m_layers.begin();
	for (; it != m_layers.end(); ++it)
	{
		if ((*it) == layer)
			break;
	}

	if (it == m_layers.end())
	{
		O3D_ERROR(E_InvalidParameter("Layer not found"));
	}
	else
	{
		// remove the layer of the son list
		m_layers.erase(it);

		// no node
		layer->setParent(getScene());
		layer->setNode(NULL);
		layer->setPersistant(False);

		//m_visibility->removeLayer(layer);
	}
}

void Map2d::deleteAllLayers()
{
	IT_LayerList it = m_layers.begin();

	while (it != m_layers.end())
	{
		deletePtr(*it);

		++it;
	}

	m_layers.clear();
}

