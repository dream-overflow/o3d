/**
 * @file octree.h
 * @brief Octree visibility.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-12-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_OCTREE_H
#define _O3D_OCTREE_H

#include "o3d/core/memorydbg.h"
#include "visibilityabc.h"

namespace o3d {

/**
 * @brief Octree based visibility controller.
 * @todo Implement it.
 */
class O3D_API Octree : public VisibilityABC
{
public:

	O3D_DECLARE_CLASS(Octree)

	//! default constructor
	Octree(
		BaseObject *parent,
		const Vector3 &position = Vector3(),
		const Vector3 &size = Vector3());

	//! destructor
	virtual ~Octree();

	//! get the number of object in entry
	virtual UInt32 getNumObjects() const;

	//! add an object (we suppose that it doesn't exist)
	virtual void addObject(SceneObject *object);

	//! remove an object
	virtual Bool removeObject(SceneObject *object);

	//! update an object
	virtual void updateObject(SceneObject *object);

	//! check for visible object and add it to visibility manager
	virtual void checkVisibleObject(const VisibilityInfos &);

	//! draw the quadtree
    virtual void draw(const DrawInfo &drawInfo);
};

} // namespace o3d

#endif // _O3D_OCTREE_H
