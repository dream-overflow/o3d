/**
 * @file lightcontainer.h
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-04-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PCLODLIGHTCONTAINER_H
#define _O3D_PCLODLIGHTCONTAINER_H

#include "o3d/core/baseobject.h"
#include "o3d/core/templatearray2d.h"

#include "o3d/engine/landscape/landscape.h"

#include "o3d/core/vector2.h"
#include "o3d/core/vector3.h"

namespace o3d {

class PCLODTopZone;
class PCLODLightmap;
class PCLODLightInfo;
class PCLODTextureManager;
class PCLODTerrain;

//---------------------------------------------------------------------------------------
//! @class PCLODLight
//-------------------------------------------------------------------------------------
//! Simple implementation of light container for the terrain engine. Additionnal datas
//! are used to manage the update of lights.
//!
//! Simplifications:
//! For the moment, self shadowing is only computed for directional lights.
//---------------------------------------------------------------------------------------
class O3D_API PCLODLight : public BaseObject
{
public:

	//! @brief A constructor
	PCLODLight(BaseObject *, const TerrainBase::LightInfos &);
	//! @brief The destructor
	virtual ~PCLODLight();

	//! @brief Delete event
	//! Called when the object is not used anymore and must be destroyed
	virtual Bool deleteIt();

	//! @brief (MT) Update the light
	//! This function is called each frames by the texture manager.
	void mtUpdate();

	//! @brief (RT) Update the light
	//! This function is called every refreshment by the thread.
	void rtUpdate();

	//! @brief (RT) Generate the lightmap associated to the light
	//! This function can be called in the refresh thread. It is used to generate the
	//! lightmap used by O3DPCLODLightmap. A call to this function must be done for
	//! each light of the zone during each updates.
	//! Results of the computed are added to the existing lightmap.
	//! Note that this function does not need semaphores since we only use 2 threads
	//! and the a light is only used in one thread.
	//! @param _info is an private object
	//! @param _pTopZone a pointer to the top zone object associated to the lightmap
	//! @param _normalMap a reference to the normal map array
	//! @param _pLightmap the target buffer for the lightmap
	//! @param _computedPosition and _computedSize define the square that were changed
	//!        by this function.
	//! @param _level defines a simplification level (not used yet)
	//! @return True if the lightmap changed.
	//! @exception ASSERT if _pTopZone is null, _normapMap not defined, _lightmap or
	//!            or normal map has not the correct size.
	Bool generateLightmap(		PCLODLightInfo & _info,
									PCLODTopZone * _pTopZone,
									const TemplateArray2D<Vector3> & _normalMap,
									TemplateArray2D<Vector3> & _lightmap,
									Vector2i & _computedPosition,
									Vector2i & _computedSize,
									UInt32 _level);

	//! @brief Wether or not a lightmap must be refreshed
	//! This function is called by the main and the refresh thread. In fact it's not
	//! necessary to protect it againt multiple access since the semaphore in the
	//! lightmap object will not allowed any modifications on the light map.
	Bool needUpdate(const PCLODLightInfo &) const;

	//! @brief Invalidate the light
	//! This function is used to warn the container that the light has changed and must
	//! be totally refreshed. During the next update, O3DPCLODLight::NeedUpdate will
	//! return True whatever happens.
	void invalidate();

	//! @brief (MT) Check if a lightmap must be inserted
	//! This function is called by the texture manager when the light was just inserted
	//! in the terrain or if a zone is inserted.
	Bool mtAffectLightmap(const PCLODLightmap *) const;
	//! @brief (MT) Add a lightmap
	//! This function is called by the texture manager if it detects that a lightmap
	//! could be alter by a newly inserted light.
	void mtAddLightmap(PCLODLightmap *);
	//! @brief (MT) Remove a lightmap
	//! This function is called by the texture manager when it received an event which
	//! warns it that a lightmap was destroyed.
	void mtRemoveLightmap(PCLODLightmap *);

	//! @brief Return the state of the light
	//! @return True if the light is static, it means that the light will
	//! be refresh asynchroneously
	Bool isStaticLight() const;
	//! @brief Return the state of the light
	//! @return True if the light is static, it means that the light will
	//! be refresh in the main thread
	Bool isDynamicLight() const;

	//! @brief Return the instance of the light
	const Light * getLight() const { return m_pLight.get(); }

	//! @brief Return the terrain instance
	const PCLODTerrain * getTerrain() const;

	//! @brief Return the texture manager instance
	const PCLODTextureManager * getTextureManager() const;

	//! @brief Return the state of the light
	//! This function is used by the texture manager to check that the light
	//! was not externally destroyed. In this case, this light container will
	//! be removed by the manager.
	//! @return True if the light instance is valid, False otherwise
	Bool isValid() const { return m_pLight.isValid(); }

protected:

	//! @brief Internal function
	Bool needUpdate();

	//! @brief Internal function
	//! @return the depth of a cone if the light is a spot light
	Float getSpotDepth() const;

private:

	// Internal type
	typedef std::vector<PCLODLightmap *>	T_LightArray;
	typedef T_LightArray::iterator			IT_LightArray;
	typedef T_LightArray::const_iterator	CIT_LightArray;

	// Members

	Bool m_forceRefresh;		//!< True if Invalidate was called

	Int32 m_frameIndex;

	Vector3 m_lightPosition;		//!< Position of the light saved during the last update
	Vector3 m_lightColor;			//!< Same but for the color
	Vector3 m_lightAmbiant;
	Vector3 m_lightDirection;
	Float m_spotCutoff;
	Float m_spotExponent;
	Vector3 m_spotCoefficients;

	SmartObject<Light> m_pLight;

	TerrainBase::LightInfos m_lightInfos;

	T_LightArray m_lightmaps;		//!< List of lightmaps using this light
};

//---------------------------------------------------------------------------------------
//! @class PCLODLightInfo
//-------------------------------------------------------------------------------------
//! Private type
//---------------------------------------------------------------------------------------
class O3D_API PCLODLightInfo
{
	friend class PCLODLight;

//private:
public:

	Int32 frameIndex;


	Int32 lightmapLevel;

	Array2DFloat shadowMap;
	Bool shadowUpToDate;		//!< Used during computation

public:

	PCLODLightInfo();
	~PCLODLightInfo();
};

} // namespace o3d

#endif // _O3D_PCLODLIGHTCONTAINER_H
