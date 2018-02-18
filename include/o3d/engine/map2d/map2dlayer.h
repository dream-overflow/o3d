/**
 * @file map2dlayer.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MAP2DLAYER_H
#define _O3D_MAP2DLAYER_H

#include "../hierarchy/node.h"
#include "o3d/core/templateprioritymanager.h"
#include "map2dobject.h"

namespace o3d {

class Map2dVisibility;

/**
 * @brief Abstract map 2d layer class.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-08-02
 */
class O3D_API Map2dLayer : public BaseNode
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(Map2dLayer)

	Map2dLayer(BaseObject *parent);

	virtual ~Map2dLayer();

	//
	// Drawable
	//

    virtual UInt32 getDrawType() const override;

	//! Always CLIP_INSIDE
    virtual Geometry::Clipping checkBounding(const AABBox &bbox) const override;
	//! Always CLIP_INSIDE
    virtual Geometry::Clipping checkBounding(const Plane &plane) const override;
	//! Always CLIP_INSIDE
    virtual Geometry::Clipping checkFrustum(const Frustum &frustum) const override;
};

} // namespace o3d

#endif // _O3D_MAP2DLAYER_H
