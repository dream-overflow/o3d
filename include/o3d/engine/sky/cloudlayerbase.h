/**
 * @file cloudlayerbase.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2009-30-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CLOUDLAYERBASE_H
#define _O3D_CLOUDLAYERBASE_H

#include "o3d/engine/scene/sceneobject.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class SkyBase
//-------------------------------------------------------------------------------------
//! Definition of the base class used for all sky renderer
//---------------------------------------------------------------------------------------
class O3D_API CloudLayerBase : public SceneObject
{
public:

	//! Default constructor
	CloudLayerBase(BaseObject * _pParent);
	//! The destructor
	virtual ~CloudLayerBase();

	//! Return Scene::DrawCloudLayer
	virtual UInt32 getDrawType() const;

	//! Called when the sky must be drawn
	virtual void draw(const DrawInfo &drawInfo) = 0;

	//! Called when the time changes
	virtual void setTime(Double);

	//! Transfer function
	virtual bool project(const Vector3 & _dir, SmartArrayFloat & _target);
};

} // namespace o3d

#endif // _O3D_CLOUDLAYERBASE_H

