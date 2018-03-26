/**
 * @file visibilitybasic.h
 * @brief Quadtree visibility.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-05-29
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VISIBILITYBASIC_H
#define _O3D_VISIBILITYBASIC_H

#include "visibilityabc.h"

#include <list>

namespace o3d {

/**
 * @brief Basic visibility manager based on a max distance (radius).
 */
class O3D_API VisibilityBasic : public VisibilityABC
{
public:

	O3D_DECLARE_CLASS(VisibilityBasic)

	//! default constructor
	VisibilityBasic(
		BaseObject *parent,
		const Vector3 &position = Vector3(),
		const Vector3 &size = Vector3());

	//! destructor
	virtual ~VisibilityBasic();

	//! get the number of object in entry
    virtual UInt32 getNumObjects() const override;

	//! add an object (we suppose that it doesn't exist)
    virtual void addObject(SceneObject *object) override;

	//! remove an object
    virtual Bool removeObject(SceneObject *object) override;

	//! update an object
    virtual void updateObject(SceneObject *object) override;

	//! check for visible object and add it to visibility manager
    virtual void checkVisibleObject(const VisibilityInfos &) override;

	//! draw the symbolic
    virtual void draw(const DrawInfo &drawInfo) override;

private:

	T_ObjectList m_objectList;
};

} // namespace o3d

#endif // _O3D_VISIBILITYBASIC_H
