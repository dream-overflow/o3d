/**
 * @file transformable.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TRANSFORMABLE_H
#define _O3D_TRANSFORMABLE_H

#include "transform.h"

namespace o3d {

typedef std::list<Transform*> T_TransformList;
typedef T_TransformList::iterator IT_TransformList;
typedef T_TransformList::const_iterator CIT_TransformList;

/**
 * @brief Transformable is able to apply a stack of transformations objects.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-09-04
 */
class O3D_API Transformable
{
public:

	//! Add a new transform
	virtual void addTransform(Transform *transform) = 0;

	//! Remove and delete a transform
	virtual void deleteTransform(Transform *transform) = 0;

	//! Remove and delete all transforms
	virtual void deleteAllTransforms() = 0;

	//! Find a transform
	virtual Bool findTransform(Transform *transform) const = 0;

	//! Find a transform given its name (read only)
	virtual const Transform* findTransform(const String &name) const = 0;
	//! Find a transform given its name
	virtual Transform* findTransform(const String &name) = 0;

	//! Get the transforms list (read only)
	virtual const T_TransformList& getTransforms() const = 0;
	//! Get the transforms list
	virtual       T_TransformList& getTransforms() = 0;

	//! Get the front transform or null if none (read only)
	virtual const Transform* getTransform() const = 0;

	//! Get the front transform or null if none
	virtual Transform* getTransform() = 0;
};

} // namespace o3d

#endif // _O3D_TRANSFORMABLE_H

