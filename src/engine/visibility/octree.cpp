/**
 * @file octree.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-12-08
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/visibility/octree.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/context.h"
#include "o3d/geom/frustum.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Octree, ENGINE_VISIBILITY_OCTREE, VisibilityABC)

// default constructor
Octree::Octree(
	BaseObject *parent,
	const Vector3 &position,
	const Vector3 &size) :
		VisibilityABC(parent,position,size)
{
}

// destructor
Octree::~Octree()
{
}

// get the number of object in entry
UInt32 Octree::getNumObjects()const
{
	return 0;
}

// add an object (we suppose that it doesn't exist)
void Octree::addObject(SceneObject *object) {}

// remove an object
Bool Octree::removeObject(SceneObject *object)
{
	return True;
}

// update an object
void Octree::updateObject(SceneObject *object) {}

// check for visible object and add it to visibility manager
void Octree::checkVisibleObject(const VisibilityInfos &) {}

// draw the octree
void Octree::draw()
{
	// setup modelview
	getScene()->getContext()->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());
}
