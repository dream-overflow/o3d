/**
 * @file map2d.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MAP2D_H
#define _O3D_MAP2D_H

#include "../hierarchy/basenode.h"
#include "map2dlayer.h"

namespace o3d {

class Map2dVisibility;

/**
 * @brief 2d map container.
 * It is a special scene node, that can manager Map2dLayer object and a visibility
 * controller based on a quadtree.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-08-02
 */
class O3D_API Map2d : public BaseNode
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Map2d)

	Map2d(BaseObject *parent);

	virtual ~Map2d();

	virtual Bool deleteChild(BaseObject *child);

	//! Compute recursively the number of element of this branch
	virtual UInt32 getNumElt() const;

	//
	// Drawable
	//

	//! Draw the map using the visibility controller.
	virtual void draw(const DrawInfo &drawInfo);

	virtual UInt32 getDrawType() const;

	//! Always CLIP_INSIDE
	virtual Geometry::Clipping checkBounding(const AABBox &bbox) const;
	//! Always CLIP_INSIDE
	virtual Geometry::Clipping checkBounding(const Plane &plane) const;
	//! Always CLIP_INSIDE
	virtual Geometry::Clipping checkFrustum(const Frustum &frustum) const;

	//
	// Transforms
	//

	//! Get the front transform or null if none (read only)
	virtual const Transform* getTransform() const;

	//! Get the front transform or null if none
	virtual Transform* getTransform();

	//
	// Layers.
	//

	virtual UInt32 getNumSon() const;

	//! Find an object/node given its name
	virtual const SceneObject* findSon(const String &name) const;
	//! Find an object/node given its name
	virtual SceneObject* findSon(const String &name);

	//! Find a scene object and return true if found.
	virtual Bool findSon(SceneObject *object) const;

	/**
	 * @brief addLayer Add a layer a child object.
	 * @param layer Layer object.
	 * @param pos Position in the list. The element is inserted before the current element.
	 */
	void addLayer(Map2dLayer *layer, UInt32 pos);

	//! Remove a layer.
	void removeLayer(Map2dLayer *layer);

	//! Get the position of a layer.
	UInt32 getLayerPos(Map2dLayer *layer) const;

	//! Remove and delete all sons.
	void deleteAllLayers();

	virtual void update();

protected:

	typedef std::list<Map2dLayer*> T_LayerList;
	typedef T_LayerList::iterator IT_LayerList;
	typedef T_LayerList::const_iterator CIT_LayerList;

	std::list<Map2dLayer*> m_layers;
};

} // namespace o3d

#endif // _O3D_MAP2D_H

