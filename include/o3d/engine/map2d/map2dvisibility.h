/**
 * @file map2dvisibility.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MAP2DVISIBILITY_H
#define _O3D_MAP2DVISIBILITY_H

#include "map2dobject.h"

namespace o3d {

/**
 * @brief Visibility controler for a map 2d, using multi-layers and quadtree optimisations
 * The depth is dynamicaly computed from the number of object per cell.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-09-01
 */
class O3D_API Map2dVisibility
{
public:

	/**
	 * @brief Map2dVisibility
	 * If maxObjects constraint is different from 0, then when a cell reaches this limit
	 * and only if the maxDepth is not reach for this branch, it split the cell.
	 * @param parent Scene entity parent
	 * @param position Mostly -cellSize/2 for a zero centered quadtree.
	 * @param cellSize The root cell size. Must be a power of 2.
	 * @param maxDepth at least 1. Minimal cell size is 2.
	 * @param maxObjects Constraint of the number of maximum objects per cell.
	 *        0 means no limits.
	 */
	Map2dVisibility(
		const Vector2i &position,
		Int32 cellSize,
		UInt32 maxDepth,
		UInt32 maxObjects);

	virtual ~Map2dVisibility();

	//! set a constraint of the number of maximums object per cell. 0 means no limits.
	void setMaxObjectPerCell(UInt32 max);

	//! add an object to manage.
	void addObject(Map2dObject *object);

	//! remove an object without deleting it.
	void removeObject(Map2dObject *object);

	//! update an object (when its position change).
	void updateObject(Map2dObject *object);

	//! true if the quadtree state has updated.
	Bool hasUpdated() const { return m_updated; }

	//! clear the updated flag (must be called before any other <add/update/remove>Object).
	void clearUpdated() { m_updated = False; }

	//! get the position.
	const Vector2i& getPos() const { return m_pos; }

	//! get the size of a cell.
	Int32 getCellSize() const { return m_cellSize; }

	/**
	 * @brief check for visible object and add it to visibility manager
	 * @param viewport view area
	 * @return number of rejected object from visible quadtree cells
	 */
	UInt32 checkVisibleObject(const Box2i &viewport);

	//! draw the symbolic and the visible region of the map.
	void draw(const DrawInfo &drawInfo);

	//! get the draw list.
	T_Map2dObjectList& getDrawList() { return m_drawList; }

	//! clear any objects.
	void clear();

	//! get the max depth.
	UInt32 getMaxDepth() const { return m_maxDepth; }

	//! get the max objects number per cell.
	UInt32 getMaxObjects() const { return m_maxObjects; }

	//! Check if another object intersect with the given box, using the quadtree.
	Bool isObjectIntersect(const Box2i &box) const;

	//! Check if another object intersect with the given base (iso base) box, using the quadtree.
	Bool isObjectBaseIntersect(const Map2dObject *from) const;

	//! show the tree in a string.
	String getTreeView() const;

private:

	Vector2i m_pos;
	Int32 m_cellSize;
	UInt32 m_maxDepth;
	UInt32 m_maxObjects;

	Bool m_updated;

	class Map2dQuad *m_root;

	T_Map2dObjectList m_drawList;
};

class O3D_API Map2dQuad
{
public:

	//! Constructor for the initial (root) quad cell.
	Map2dQuad(Map2dVisibility *visibility, const Vector2i &pos, Int32 size);

	//! Constructon for the children quad cells.
	Map2dQuad(const Vector2i &pos, Map2dQuad *parent);

	~Map2dQuad();

	//! Add on level of depth and update objects into them.
	void split();

	//! Reduce the depth from one level and put object into the parent.
	void merge();

	//! Get the centered position.
	const Box2i& getAbsBox() const { return m_absBox; }

	//! Add an object (recursively through the children)
	Bool addObject(Map2dObject *object);

	//! Add an object to this cell (not recursively).
	void addObjectDirect(Map2dObject *object);

	//! clear any objects.
	void clear();

	/**
	 * @brief removeObject Remove an object (recursive remove).
	 * @param object
	 * @param layer
	 * @return true if the element is removed.
	 */
	Bool removeObject(Map2dObject *object);

	//! Update an object (recursive).
	Bool updateObject(Map2dObject *object);

	//! Check if another object intersect with the given box, using the quadtree.
	Bool isObjectIntersect(const Box2i &box) const;

	//! Check if another object intersect with the given base (iso) box, using the quadtree.
	Bool isObjectBaseIntersect(const Map2dObject *from) const;

	//! Is the cell splited.
	Bool isSplitted() const { return m_children[0] != nullptr; }

	//! Find visibles objects and put fill the list with them.
	void findVisiblesObjects(const Box2i &viewport, T_Map2dObjectList &objects, UInt32 &rejected);

	//! Get the visibility controller.
	Map2dVisibility* getVisibility() { return m_visibility; }

	//! Get the visibility controller.
	const Map2dVisibility* getVisibility() const { return m_visibility; }

	//! show the tree in a string.
	void getTreeView(String &out) const;

private:

	Box2i m_absBox;

	Map2dVisibility *m_visibility;

	Map2dQuad *m_parent;
	Map2dQuad *m_children[4];

	UInt32 m_depth;

	typedef std::list<Map2dObject*> T_Map2dObjectList;
	typedef T_Map2dObjectList::iterator IT_Map2dObjectList;
	typedef T_Map2dObjectList::const_iterator CIT_Map2dObjectList;

	T_Map2dObjectList m_objects;

	//! Fill with found object, recursively. Helper for merge().
	void fillWithObjects(T_Map2dObjectList &objects);
};

} // namespace o3d

#endif // _O3D_MAP2DVISIBILITY_H

