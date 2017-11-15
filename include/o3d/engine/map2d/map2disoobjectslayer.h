/**
 * @file map2disoobjectslayer.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MAP2DISOOBJECTSLAYER_H
#define _O3D_MAP2DISOOBJECTSLAYER_H

#include "map2dlayer.h"
#include "map2dtileset.h"

namespace o3d {

/**
 * @brief A layer of a 2d map, specialized for the rendering of multiple objects
 * in 2d isometrics ccordinates. Object rendering is done from up to down, and
 * right to left.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-08-13
 */
class O3D_API Map2dIsoObjectsLayer : public Map2dLayer
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Map2dIsoObjectsLayer)

	/**
	 * @brief Map2dIsoObjectsLayer
	 * @param parent Parent map 2d
	 * @param area Position and size of the layer.
	 * @param maxDepth Max depth of the quatree.
	 * @param maxObjectsPerCell Max objects number per cell of the quadtree @see Map2dVisibility
	 */
	Map2dIsoObjectsLayer(
			BaseObject *parent,
			const Box2i &area = Box2i(),
			UInt32 maxDepth = 1,
			UInt32 maxObjectsPerCell = 0);

	virtual ~Map2dIsoObjectsLayer();

	//! Delete child. This node need to redone the sort for rendering.
	virtual Bool deleteChild(BaseObject *child);

	//! Compute recursively the number of element of this branch
	virtual UInt32 getNumElt() const;

	virtual void update();

	virtual void draw(const DrawInfo &drawInfo);

	//
	// Transforms
	//

	//! Get the front transform or null if none (read only)
	virtual const Transform* getTransform() const;

	//! Get the front transform or null if none
	virtual Transform* getTransform();

	//
	// Objects
	//

	virtual UInt32 getNumSon() const;

	//! Find an object/node given its name
	virtual const SceneObject* findSon(const String &name) const;
	//! Find an object/node given its name
	virtual SceneObject* findSon(const String &name);

	//! Find a scene object and return true if found.
	virtual Bool findSon(SceneObject *object) const;

	//! Get the objects list.
	const T_Map2dObjectList& getObjects();

	//! Get the visibles objects.
	const T_Map2dObjectList& getVisiblesObjects();

	//! Check in another object intersect at this box.
	Bool isObjectIntersect(const Box2i &box) const;

	/**
	 * Check in an object intersect with another. It avoid self collisions.
	 * The collision is done by the base box of the objects.
	 */
	Bool isObjectBaseIntersect(const Map2dObject *from) const;

	/**
	 * @brief addObject Add a new map2d object as child of this node layer.
	 * @param name Name only for information
	 * @param pos Position relative to the layer
	 * @param tileSet Related tileSet where the object texture come from
	 * @param tileIt Tile identifier into the related tileSet
	 * @return The new object.
	 */
	Map2dObject* addObject(
			const String &name,
			const Vector2i &pos,
            const Rect2i &baseRect,
			Map2dTileSet *tileSet,
			UInt32 tileId);

	//! Remove a specified object.
	void removeObject(Map2dObject *object);

	//! Update a specified object.
	void updateObject(Map2dObject *object);

	//! Move a specified object.
	//void moveObject(Map2dObject *object, const Vector2i &pos);

	//! Remove and delete all objects.
	virtual void deleteAllObjects();

protected:

	Bool m_sort;      //!< Need to sort at next draw
	Box2i m_box;          //!< Visible area of this layer

	class Map2dVisibility *m_visibility;

	T_Map2dObjectList m_objects;

    std::vector<Map2dObject*> m_drawList;
};

} // namespace o3d

#endif // _O3D_MAP2DISOOBJECTSLAYER_H

