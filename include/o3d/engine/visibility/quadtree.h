/**
 * @file quadtree.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_QUADTREE_H
#define _O3D_QUADTREE_H

#include "o3d/core/hashmap.h"
#include "o3d/core/templatearray2d.h"
#include "o3d/engine/scene/sceneobject.h"
#include "o3d/engine/visibility/visibilityabc.h"

#include <vector>

#include "o3d/core/memorydbg.h"

namespace o3d {

class QuadObject;
class QuadZone;
class Quadtree;
class Scene;

// Conversion from a 3D space ZX to 2D space XY
#define QUAD_Z X
#define QUAD_X Y

typedef std::vector<QuadObject*> T_ZoneObjectList;
typedef T_ZoneObjectList::iterator IT_ZoneObjectList;
typedef T_ZoneObjectList::const_iterator CIT_ZoneObjectList;

typedef std::vector<QuadZone*> T_QuadZoneList;
typedef T_QuadZoneList::iterator IT_QuadZoneList;
typedef T_QuadZoneList::const_iterator CIT_QuadZoneList;

/**
 * @brief A QuadTree element.
 * @date 2006-12-08
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 */
class QuadObject : public EvtHandler
{
public:

	QuadObject(Quadtree * _pQuadTree, SceneObject * _object);
	virtual ~QuadObject();

	//! Return the SceneObject stored.
	inline SceneObject * getSceneObject() { return m_pObject; }
	//! Return the SceneObject stored (read only).
	inline const SceneObject * getSceneObject() const { return m_pObject; }

	//! Return the number of zone which contains the object m_pObject.
	inline UInt32 getZoneNumber() const { return UInt32(m_zoneList.size()); }

	//! Return the list of zone.
	inline T_QuadZoneList & getZoneList() { return m_zoneList; }
	//! Return the list of zone (read only).
	inline const T_QuadZoneList & getZoneList() const { return m_zoneList; }

	//! Add a container of this object
	void addZoneContainer(QuadZone * _zone);
	//! Remove a container of this object
	void removeZoneContainer(QuadZone * _zone);

public:

    Signal<> onDestroyed{this};
    Signal<> onUnused{this};

private:

	Bool m_drawn;

	SceneObject * m_pObject;
	Quadtree * m_pQuadTree;

	T_QuadZoneList m_zoneList;
};

typedef std::map<SceneObject*, QuadObject*> T_ObjectMap;
typedef T_ObjectMap::iterator IT_ObjectMap;
typedef T_ObjectMap::const_iterator CIT_ObjectMap;


/**
 * @brief Used by a QuadTree to represent a zone
 * @date 2006-12-08
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 */
class QuadZone : public EvtHandler
{
	typedef std::vector<UInt8> T_ZonePosition;
	typedef T_ZonePosition::iterator IT_ZonePosition;
	typedef T_ZonePosition::const_iterator CIT_ZonePosition;

public:

	//! Constructor.
	QuadZone(Quadtree * _quad, Vector2i _position, Float _size);

	//! Virtual destructor.
	virtual ~QuadZone();

	//! Draw the zone
	void draw(Scene *scene);

	//! Set the relative position of the zone.
	void setPosition(const Vector2i & _vec) { m_position = _vec; }
	//! Return the relative position of the zone.
	Vector2i getPosition() const { return m_position; }

	//! Set the size of the zone.
	void setSize(Float _size) { m_size = _size; }
	//! Return the size of the zone.
	Float getSize() const { return m_size; }

	//! Return the absolute position of the zone/
	//! Doesn't work for subzone yet.
	Vector3 getAbsolutePosition() const;

	//! Return the center of this zone.
	Vector3 getAbsoluteCenter() const;

	//! Return True, if a zone is a children of this zone.
	Bool hasChildren(QuadZone * _zone) const;

	//! Return True, if a zone is a children of this zone, directly or not.
	Bool findZone(QuadZone * _zone) const;

	//! Add an object to the zone.
	void addObject(QuadObject * _object);
	//! Remove an object from the zone.
	void removeObject(QuadObject * _object);
	//! Remove any objects from the zone.
	void removeAllObjects();

	//! Find an object into the zone.
	//! @return The QuadObject that contains the object.
	QuadObject * findObject(SceneObject * _object);
	//! Find object into the zone (read only).
	//! @return The QuadObject that contains the object.
	const QuadObject * findObject(SceneObject * _object) const;

	//! Return the list of object in this zone
	inline T_ZoneObjectList & getObjectList() { return m_objectList; }
	//! Return the list of object in this zone
	inline const T_ZoneObjectList & getObjectList() const { return m_objectList; }

	// Accessors
    inline Bool hasParent() const { return (m_pParent != nullptr); }
	void setParent(QuadZone * _parent);
	inline QuadZone * getParent() { return m_pParent; }
	inline const QuadZone * getParent() const { return m_pParent; }

	inline Bool hasChildren() const { return ((m_pChildren[0]) || (m_pChildren[1]) || (m_pChildren[2]) || (m_pChildren[3])); }
	inline QuadZone * getChild(UInt8 _index) { O3D_ASSERT(_index < 4); return m_pChildren[_index]; }
	inline const QuadZone * getChild(UInt8 _index) const { O3D_ASSERT(_index < 4); return m_pChildren[_index]; }

private:

	Quadtree * m_pQuadTree;

	QuadZone * m_pParent;
	QuadZone * m_pChildren[4];

	Vector2i m_position;		//!< Position of the zone relative to the center of the quadtree
	Float m_size;			//!< The size of this zone

	T_ZonePosition m_subPosition;	//!< Define the position of the subzone. Topzone if m_subPosition.size() == 0

	T_ZoneObjectList m_objectList;	//!< Objects contained by the zone

	// Slot
	void onObjectDeletion();
};


/**
 * @brief Dynamic quadtree.
 * @date 2006-12-08
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * Dynamic QuadTree following the move of the camera by translating its zones.
 * Useful as a global visibility controller for large plane area.
 * @todo success lifetime to preserve useless CPU computations
 * @todo lifetime counter which depends on screen movements
 */
class O3D_API Quadtree : public VisibilityABC
{
public:

	O3D_DECLARE_CLASS(Quadtree)

	enum QuadDirection {
		NORTH = 0,
		SOUTH,
		EAST,
		WEST };

	//! Default constructor (centered to 0,0,0 by default).
	//! @param halfSize Half size of the quad-tree in number of zones.
	//! @param zoneSize Size of a zone of the quad-tree.
	Quadtree(
		BaseObject *parent,
		UInt32 halfSize = 0,
		Float zoneSize = 0.0f);

	//! Virtual destructor.
	virtual ~Quadtree();

	//! Clear all objects contained in the quadTree.
	void clear();

	virtual UInt32 getNumObjects() const;

	//! Return the center of the quad-tree.
	const Vector3& getQuadCenter() const { return m_center; }

	//! Return the origin of the first zone.
	const Vector3 getQuadOrigin() const
	{
		Float offset = (m_topZone.width()/2 + 0.5f)*m_zoneSize;
		return Vector3(m_center[X] - offset, 0.0f, m_center[Z] - offset);
	}

	//! Return the origin of the centered zone.
	const Vector3 getQuadCenterOrigin() const { return Vector3(m_center[X] - 0.5f*m_zoneSize, 0.0f, m_center[Z] - 0.5f*m_zoneSize); }

	//! Return the neighbor of a zone.
	//! Doesn't support multi level quad-tree yet.
	QuadZone * getNeighbor(const QuadZone & _zone, QuadDirection _direction);

	//! Add an object (we suppose that it doesn't exist).
	//! @note This method is called by the user or the engine, but not by the quadTree.
	virtual void addObject(SceneObject *object);

	//! Remove an object from the quad-tree (throw an event).
	//! @note This function is called by the user or the engine, but not by the quadTree.
	virtual Bool removeObject(SceneObject *object);

	//! Update an object (not used yet).
	//! @note This method is called by the user or the engine, but not by the quadTree.
	virtual void updateObject(SceneObject *object);

	//! Check for visible object and add it to visibility manager.
	virtual void checkVisibleObject(const VisibilityInfos &);

	//! Draw the quad-tree.
	virtual void draw();

protected:

	TemplateArray2D<QuadZone*> m_topZone;
	T_ObjectMap m_objectMap;

	Float m_zoneSize;
	Vector3 m_center;			//!< Center of the quad-tree. That's not the origin of a zone.
	Float m_hysteresis;

	Vector3 m_currentPosition;	//!< Current position of the user

    //! Translate the array of zones
	void translate(const Vector2i &);

	//! Signals: When an object is not used anymore
	void onObjectUnused();
};

} // namespace o3d

#endif // _O3D_QUADTREE_H
