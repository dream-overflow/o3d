/**
 * @file visibilityabc.h
 * @brief VisibilityABC is the base class for all object visibility methods computation
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2006-12-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VISIBILITYABC_H
#define _O3D_VISIBILITYABC_H

#include "o3d/core/memorydbg.h"
#include "o3d/engine/hierarchy/node.h"
#include "o3d/core/vector3.h"
#include "o3d/geom/aabbox.h"
#include "o3d/core/evt.h"

#include <list>

namespace o3d {

typedef std::list<SmartObject<SceneObject> > T_ObjectList;
typedef T_ObjectList::iterator IT_ObjectList;
typedef T_ObjectList::const_iterator CIT_ObjectList;

//! Structure which contains useful info sent to the visibility manager
struct VisibilityInfos
{
	Vector3 cameraPosition;
	Vector3 cameraOrientation;

	Float	viewMaxDistance;
	Bool	viewUseMaxDistance;
};

/**
 * @brief VisibilityABC is the base class for all object visibility methods computation.
 * @details A visibility object inherit from a node because it can be put anywhere in the
 * hierarchy tree (scene graph).
 */
class O3D_API VisibilityABC : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(VisibilityABC)

	//! Default constructor.
	//! @param parent Parent object.
	//! @param position Center of the visibility controller.
	//! @param halfSize Half size of the bounding volume of the controller.
	VisibilityABC(
		BaseObject *parent,
		const Vector3 &position,
		const Vector3 &halfSize);

	virtual ~VisibilityABC();

	//! get the number of object in entry
	virtual UInt32 getNumObjects() const = 0;

	//! add an object (we suppose that it doesn't exist)
	virtual void addObject(SceneObject *object) = 0;

	//! remove an object without deleting it
	//! @return True if the object was found and successfully removed
	virtual Bool removeObject(SceneObject *object) = 0;

	//! update an object
	virtual void updateObject(SceneObject *object) = 0;

	//! check for visible object and add it to visibility manager
	virtual void checkVisibleObject(const VisibilityInfos &) = 0;

	//! draw the symbolic
    virtual void draw(const DrawInfo &drawInfo) = 0;

	//! Get the centered world position.
	inline const Vector3& getPosition() const { return m_bbox.getCenter(); }

	//! Get the bounding size of the controller.
	inline const Vector3& getHalfSize() const { return m_bbox.getHalfSize(); }

	//! Get the bounding box of the controller.
	inline const AABBox getBoundingBox() const { return m_bbox; }

protected:

	AABBox m_bbox;        //!< Bounding box.
};

} // namespace o3d

#endif // _O3D_VISIBILITYABC_H
