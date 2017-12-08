/**
 * @file map2dlayer.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/geom/aabbox.h"
#include "o3d/geom/plane.h"
#include "o3d/geom/frustum.h"
#include "o3d/engine/map2d/map2dlayer.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(Map2dLayer, ENGINE_MAP_2D_LAYER, SceneObject)

Map2dLayer::Map2dLayer(BaseObject *parent) :
	BaseNode(parent)
{
	setMovable(True);
	setUpdatable(True);
	setDrawable(True);
	setAnimatable(True);
	setPickable(True);
	setShadable(False);
	setShadowable(False);
}

Map2dLayer::~Map2dLayer()
{
}

UInt32 Map2dLayer::getDrawType() const
{
	return Scene::DRAW_MAP_2D_LAYER;
}

Geometry::Clipping Map2dLayer::checkBounding(const AABBox &bbox) const
{
	return Geometry::CLIP_INSIDE;
}

Geometry::Clipping Map2dLayer::checkBounding(const Plane &plane) const
{
	return Geometry::CLIP_INSIDE;
}

Geometry::Clipping Map2dLayer::checkFrustum(const Frustum &frustum) const
{
	return Geometry::CLIP_INSIDE;
}
