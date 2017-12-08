/**
 * @file basenode.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/hierarchy/basenode.h"

#include "o3d/core/classfactory.h"

#include "o3d/engine/object/camera.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/scene/sceneobjectmanager.h"

#include "o3d/geom/plane.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(BaseNode, ENGINE_BASE_NODE, SceneObject)

// Constructors
BaseNode::BaseNode(BaseObject *parent) :
	SceneObject(parent)
{
	setMovable(True);
	setUpdatable(True);
	setDrawable(False);
	setPickable(False);
	setAnimatable(True);
	setShadable(False);
	setShadowable(False);
}

BaseNode::BaseNode(const BaseNode &dup) :
	SceneObject(dup)
{
    m_node = nullptr;
}

// Destructor
BaseNode::~BaseNode()
{
}

// Duplicate
BaseNode& BaseNode::operator=(const BaseNode &dup)
{
	SceneObject::operator= (dup);
	return *this;
}

Bool BaseNode::isNodeObject() const
{
	return True;
}

void BaseNode::addTransform(o3d::Transform *transform)
{
	O3D_ERROR(E_InvalidOperation("Not supported"));
}

void BaseNode::deleteTransform(Transform *transform)
{
	O3D_ERROR(E_InvalidOperation("Not supported"));
}

void BaseNode::deleteAllTransforms()
{
}

Bool BaseNode::findTransform(Transform *transform) const
{
	return False;
}

const Transform *BaseNode::findTransform(const String &name) const
{
	return nullptr;
}

Transform *BaseNode::findTransform(const String &name)
{
	return nullptr;
}

const T_TransformList& BaseNode::getTransforms() const
{
	O3D_ERROR(E_InvalidOperation("Not supported"));
}

T_TransformList& BaseNode::getTransforms()
{
	O3D_ERROR(E_InvalidOperation("Not supported"));
}

const Transform *BaseNode::getTransform() const
{
	return nullptr;
}

Transform *BaseNode::getTransform()
{
	return nullptr;
}

// Draw the branch
void BaseNode::draw(const DrawInfo &drawInfo)
{
    if (!getActivity() || !getVisibility()) {
		return;
    }
}

// Check the global bounding box with an AABBox
Geometry::Clipping BaseNode::checkBounding(const AABBox &bbox) const
{
    if (bbox.include(getAbsoluteMatrix().getTranslation())) {
		return Geometry::CLIP_INSIDE;
    } else {
		return Geometry::CLIP_OUTSIDE;
    }
}

// Check the global bounding box with an infinite plane
Geometry::Clipping BaseNode::checkBounding(const Plane &plane) const
{
	Float d = plane * getAbsoluteMatrix().getTranslation();

    if (d > 0.f) {
		return Geometry::CLIP_INSIDE;
    } else if (d == 0.f) {
		return Geometry::CLIP_INTERSECT;
    } else {
		return Geometry::CLIP_OUTSIDE;
    }
}
