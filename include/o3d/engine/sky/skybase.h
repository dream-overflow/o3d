/**
 * @file skybase.h
 * @brief Base class for all sky renderers
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-05-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SKYBASE_H
#define _O3D_SKYBASE_H

#include "o3d/engine/scene/sceneobject.h"

namespace o3d {

class SkyObjectBase;

/**
 * @brief Definition of the base class used for all sky renderer
 */
class O3D_API SkyBase : public SceneObject
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(SkyBase)

	//! Default constructor
	SkyBase(BaseObject * _pParent);
	//! The destructor
	virtual ~SkyBase();

	//! Return Scene::DrawSky
    virtual UInt32 getDrawType() const override;

	//! Called when the sky must be drawn
    virtual void draw(const DrawInfo &drawInfo) override = 0;

	//! Defines the current time of the virtual world
	virtual void setTime(Double);
	//! Returns the current time of the virtual world
	virtual Double getTime();
};

} // namespace o3d

#endif // _O3D_SKY_BASE
