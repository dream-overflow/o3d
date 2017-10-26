/**
 * @file map2dvisibility.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/map2d/map2dvisibility.h"

using namespace o3d;

Map2dVisibility::Map2dVisibility(const Vector2i &position,
		Int32 cellSize,
		UInt32 maxDepth,
		UInt32 maxObjects) :
	m_pos(position),
	m_cellSize(cellSize),
	m_maxDepth(maxDepth),
	m_maxObjects(maxObjects),
	m_updated(True),
	m_root(nullptr)
{
	if (!isPow2(m_cellSize))
		O3D_ERROR(E_InvalidParameter("Cell size must be power of 2"));

	m_root = new Map2dQuad(this, position, cellSize);
}

Map2dVisibility::~Map2dVisibility()
{
	deletePtr(m_root);
}

void Map2dVisibility::addObject(Map2dObject *object)
{
	m_updated |= m_root->addObject(object);
}

void Map2dVisibility::removeObject(Map2dObject *object)
{
	m_updated |= m_root->removeObject(object);
}

void Map2dVisibility::updateObject(Map2dObject *object)
{
	m_updated |= m_root->updateObject(object);
}

UInt32 Map2dVisibility::checkVisibleObject(const Box2i &viewport)
{
	m_drawList.clear();
	UInt32 rejected = 0;
	m_root->findVisiblesObjects(viewport, m_drawList, rejected);

	return rejected;
}

void Map2dVisibility::draw(const DrawInfo &drawInfo)
{
	for (IT_Map2dObjectList it2 = m_drawList.begin(); it2 != m_drawList.end(); ++it2)
	{
		(*it2)->draw(drawInfo);
	}
}

void Map2dVisibility::clear()
{
	m_drawList.clear();
	m_root->clear();
}

Bool Map2dVisibility::isObjectIntersect(const Box2i &box) const
{
	return m_root->isObjectIntersect(box);
}

Bool Map2dVisibility::isObjectBaseIntersect(const Map2dObject *from) const
{
	return m_root->isObjectBaseIntersect(from);
}

String Map2dVisibility::getTreeView() const
{
	String out;
	m_root->getTreeView(out);
	return out;
}

Map2dQuad::Map2dQuad(o3d::Map2dVisibility *visibility, const Vector2i &pos, Int32 size) :
	m_visibility(visibility),
	m_parent(nullptr),
	m_depth(0)
{
	m_absBox.set(pos, Vector2i(size, size));
	m_children[0] = m_children[1] = m_children[2] = m_children[3] = nullptr;
}

Map2dQuad::Map2dQuad(const Vector2i &pos, Map2dQuad *parent) :
	m_visibility(parent->m_visibility),
	m_parent(parent),
	m_depth(parent->m_depth+1)
{
	O3D_ASSERT(m_parent != nullptr);
	O3D_ASSERT(parent->m_visibility != nullptr);

	m_absBox.set(pos, Vector2i(parent->getAbsBox().width() >> 1, parent->getAbsBox().height() >> 1));

	m_children[0] = m_children[1] = m_children[2] = m_children[3] = nullptr;

}

Map2dQuad::~Map2dQuad()
{
	for (Map2dObject *object : m_objects)
	{
		object->setQuad(nullptr);
	}

	if (isSplitted())
	{
		for (Int32 i = 0; i < 4; ++i)
		{
			deletePtr(m_children[i]);
		}
	}
}

void Map2dQuad::split()
{
	// no more split
	if (m_depth >= m_visibility->getMaxDepth())
		return;

	Int32 halfSize = m_absBox.width() >> 1;

	// top-left
	m_children[0] = new Map2dQuad(
				Vector2i(m_absBox.x(), m_absBox.y()),
				this);
	// top right
	m_children[1] = new Map2dQuad(
				Vector2i(m_absBox.x() + halfSize, m_absBox.y()),
				this);
	// bottom left
	m_children[2] = new Map2dQuad(
				Vector2i(m_absBox.x(), m_absBox.y() + halfSize),
				this);
	// bottom right
	m_children[3] = new Map2dQuad(
				Vector2i(m_absBox.x() + halfSize, m_absBox.y() + halfSize),
				this);

	// optimize the objects distribution
	Map2dObject *object;
	IT_Map2dObjectList it = m_objects.begin(), rmit;
	while (it != m_objects.end())
	{
		object = *it;

		for (Int32 i = 0; i < 4; ++i)
		{
			// inside
            if (m_children[i]->m_absBox.isInside(object->getIsoAbsBox()))
			{
				m_children[i]->m_objects.push_back(object);
				object->setQuad(m_children[i]);

                //System::print(String::print("optimize %i to ", object->getId()) + m_children[i]->m_absBox + String(" from ") + object->getIsoAbsBox(), "");

				rmit = it;
				++it;
				m_objects.erase(rmit);

				object = nullptr;

				break;
			}
		}

		if (object != nullptr)
			++it;
	}
}

void Map2dQuad::merge()
{
	if (isSplitted())
	{
		for (Int32 i = 0; i < 4; ++i)
		{
			m_children[i]->fillWithObjects(m_objects);
		}
	}
}

void Map2dQuad::fillWithObjects(T_Map2dObjectList &objects)
{
	for (Map2dObject *object : m_objects)
	{
		objects.push_back(object);
	}

	// recursively
	if (isSplitted())
	{
		for (Int32 i = 0; i < 4; ++i)
		{
			m_children[i]->fillWithObjects(objects);
		}
	}
}

Bool Map2dQuad::addObject(Map2dObject *object)
{
	// recurse on children if necessary
	if (isSplitted())
	{
		for (Int32 i = 0; i < 4; ++i)
		{
			// inside
			if (m_children[i]->addObject(object))
				return True;
		}
	}
	// split on constraint
	else if ((m_visibility->getMaxObjects() > 0) &&
			 (m_objects.size() >= m_visibility->getMaxObjects()) &&
			 (m_depth < m_visibility->getMaxDepth()-1))
	{
		split();

		// insert if possible
		for (Int32 i = 0; i < 4; ++i)
		{
			// inside
			if (m_children[i]->addObject(object))
				return True;
		}
	}

	// inside
    if (m_absBox.isInside(object->getIsoAbsBox()))
	{
		m_objects.push_back(object);
		object->setQuad(this);
		return True;
	}

	// outside or intersect
	return False;
}

void Map2dQuad::addObjectDirect(Map2dObject *object)
{
	m_objects.push_back(object);
	object->setQuad(this);
}

void Map2dQuad::clear()
{
	for (Map2dObject *object : m_objects)
	{
		object->setQuad(nullptr);
	}

	m_objects.clear();

	// recurse on children
	if (isSplitted())
	{
		for (Int32 i = 0; i < 4; ++i)
		{
			m_children[i]->clear();
		}
	}
}

Bool Map2dQuad::removeObject(Map2dObject *object)
{
	for (IT_Map2dObjectList it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		if ((*it) == object)
		{
			m_objects.erase(it);
			object->setQuad(nullptr);
			return True;
		}
	}

	// recurse on children
	if (isSplitted())
	{
		for (Int32 i = 0; i < 4; ++i)
		{
			if (m_children[i]->removeObject(object))
				return True;
		}
	}

	return False;
}

Bool Map2dQuad::updateObject(Map2dObject *object)
{
	// recurse on children if necessary
	if (isSplitted())
	{
		for (Int32 i = 0; i < 4; ++i)
		{
			// inside
			if (m_children[i]->updateObject(object))
				return True;
		}
	}
	// split on constraint
	else if ((m_visibility->getMaxObjects() > 0) &&
			 (m_objects.size() >= m_visibility->getMaxObjects()) &&
			 (m_depth < m_visibility->getMaxDepth()-1))
	{
		split();

		// insert if possible
		for (Int32 i = 0; i < 4; ++i)
		{
			// inside
			if (m_children[i]->updateObject(object))
				return True;
		}
	}

	// inside
    if (m_absBox.isInside(object->getIsoAbsBox()))
	{
		if (object->getQuad() != this)
		{
			if (object->getQuad() != nullptr)
			{
				object->getQuad()->m_objects.remove(object);
			}

			m_objects.push_back(object);
			object->setQuad(this);
		}

		return True;
	}

	// outside or intersect
	return False;
}

Bool Map2dQuad::isObjectIntersect(const Box2i &box) const
{
	for (Map2dObject *object : m_objects)
	{
        if (box.isIntersect(object->getIsoAbsBox()))
			return True;
	}

	// recurse on children if necessary
	if (isSplitted())
	{
		for (Int32 i = 0; i < 4; ++i)
		{
			// inside
			if (m_children[i]->isObjectIntersect(box))
				return True;
		}
	}

	// outside
	return False;
}

Bool Map2dQuad::isObjectBaseIntersect(const Map2dObject *from) const
{
	// out of the cell
    if (!from->getIsoAbsBox().isIntersect(m_absBox) && !m_absBox.isInside(from->getIsoAbsBox()))
        return False;

    for (Map2dObject *object : m_objects)
	{
        // TODO iso

        // intersection, a inside b, b inside a
        if (from != object && (
                    from->getIsoAbsBaseRect().intersect2(object->getIsoAbsBaseRect()) ||
                    from->getIsoAbsBaseRect().inside(object->getIsoAbsBaseRect()) ||
                    object->getIsoAbsBaseRect().inside(from->getIsoAbsBaseRect())))
			return True;
	}

	// recurse on children if necessary
	if (isSplitted())
	{
		for (Int32 i = 0; i < 4; ++i)
		{
			// inside
			if (m_children[i]->isObjectBaseIntersect(from))
				return True;
		}
	}

	// outside
	return False;
}

void Map2dQuad::findVisiblesObjects(const Box2i &viewport, T_Map2dObjectList &objects, UInt32 &rejected)
{
    if (!viewport.isIntersect(m_absBox))
        return;

    //System::print(String("acceptCell:depth=") << m_depth << String(", box=") << m_absBox << String("numObjects=") << (Int32)m_objects.size(),"");

	for (Map2dObject *object : m_objects)
	{
		// visible object
        if (viewport.isIntersect(object->getIsoAbsBox()))
		{
			objects.push_back(object);
		}
        //else
        {
            // rejected
            ++rejected;
        }
	}

	// recurse on children
	if (isSplitted())
	{
		for (Int32 i = 0; i < 4; ++i)
		{
			m_children[i]->findVisiblesObjects(viewport, objects, rejected);
		}
	}
}

void Map2dQuad::getTreeView(String &out) const
{
	String ofs;
	for (UInt32 i = 0; i < m_depth; ++i)
	{
		ofs += ' ';
	}

	out += ofs + String("cell: depth=") << m_depth << String(", numObjects=") << (Int32)m_objects.size() << "\n";

	for (Map2dObject *object : m_objects)
	{
        out += ofs + String("--> object=") << object->getId() << ", box=" << object->getIsoAbsBox() << "\n";
	}

	if (isSplitted())
	{
		for (Int32 i = 0; i < 4; ++i)
		{
			m_children[i]->getTreeView(out);
		}
	}
}

