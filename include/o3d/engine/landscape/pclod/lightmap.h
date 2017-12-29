/**
 * @file lightmap.h
 * @brief PCLOD light map.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2007-06-26
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PCLODLIGHTMAP_H
#define _O3D_PCLODLIGHTMAP_H

#include "o3d/core/evt.h"
#include "o3d/core/templatearray2d.h"

#include "o3d/core/vector2.h"

#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/landscape/pclod/lightcontainer.h"

namespace o3d {

class PCLODTerrain;
class PCLODTextureManager;
class PCLODTopZone;
class PCLODLightmap;

class Vector3;

//---------------------------------------------------------------------------------------
//! @class PCLODLightmap
//-------------------------------------------------------------------------------------
//! PCLOD light map.
//---------------------------------------------------------------------------------------
class O3D_API PCLODLightmap : public BaseObject
{
public:

	/* Type */

	//!< Repérage des zones voisines
	enum ZoneDirection {
		DIR_N = 0,
		DIR_E,
		DIR_S,
		DIR_W,

		DIR_SIZE };

	/* Constructors */

	/* A lightmap will be a anchor on a colormap. So it needs the colormap reference.
	 * Par ailleurs, elle a besoin d'incrémenter le compteur de référence sur la heightmap
	 * donc il lui faut un pointeur sur la topZone en question */
	PCLODLightmap(BaseObject *pParent, PCLODTopZone *);
	virtual ~PCLODLightmap();

	/* Functions */

	PCLODTerrain * getTerrain();
	const PCLODTerrain * getTerrain() const;

	PCLODTextureManager * getTextureManager();
	const PCLODTextureManager * getTextureManager() const;

	UInt32 getZoneId() const ;

	Texture2D * getTexture() const { return m_pTexture; }
	PCLODTopZone * getZone() const { return m_pTopZone; }

	//! @brief Return the state of the lightmap
	//! @return True if the lightmap is generated and loaded in video memory.
	Bool generated() const;

	/* IO Functions */

	//! (RT) Update of lights
	void rtUpdate();

	/* Release the texture */
	void mtUnload();

	//! (MT) Reload the texture if there are waiting datas (MAIN THREAD)
	void mtUpdate();

	//! (MT) Add a light
	//! This function adds a light to the lightmap. It is called by the light container
	//! itself but it's the texture manager which asks it. So this function does not
	//! have to warn the light container.
	void mtAddLight(PCLODLight *);

	//! (MT) Remove a light
	//! This function remove a light from the lightmap. It is called by the container.
	//! But the fact that the light was destroyed, was previously detected by the
	//! texture manager. This function just removed the pointer from its list and does
	//! not remove itself from the light container list.
	void mtRemoveLight(PCLODLight *);

	//! Define a neighbors of this zone
	void setNeighbor(ZoneDirection, PCLODLightmap *);

	/* Define/Get the lod */
	void setLod(Int8 _value) { m_offsetLod = _value; }
	Int8 getLod() const { return m_offsetLod; }

	//! @brief Returns a neighbor
	//! @param _direction is the direction you want to get a neighbor:
	//!                   0 => NORTH, 1 => EAST, 2 => SOUTH, 3 => WEST
	//! @param _index is usefull because a lightmap could have more than 1 neighbor on
	//!               a side. 0 => First neighbor, O3D_MAX_INT32 => Last neighbor (clockwise)
	PCLODLightmap * getNeighbor(Int32 _direction, Int32 _index) const;

	//! Exp
	PCLODLightInfo & getLightInfo(PCLODLight* _pLight) { return m_lightMap[_pLight]; }

	Int32 getLightCount() const { return Int32(m_lightMap.size()); }

	/* Functions which provide a references counter */
	void useIt() const;
	void releaseIt() const;

	Bool noLongerUsed() const { return (m_refCounter == 0); }
	UInt32 getReferenceCounter() const { return m_refCounter; }

public:

    Signal<> onTextureUnused{this};

private:

	// Internal types
	typedef std::vector<PCLODLightmap*>		T_LightmapArray;
	typedef T_LightmapArray::iterator		IT_LightmapArray;
	typedef T_LightmapArray::const_iterator	CIT_LightmapArray;

	typedef std::map<PCLODLight*, PCLODLightInfo>	T_LightMap;
	typedef T_LightMap::iterator					IT_LightMap;
	typedef T_LightMap::const_iterator				CIT_LightMap;

	typedef TemplateArray2D<Vector3>	T_NormalMapBuffer;
	typedef TemplateArray2D<Vector3>	T_LightMapBuffer;

	// Members
	PCLODTopZone * m_pTopZone;		//!< On doit garder un pointeur sur la zone puisque la lightmap
												//!< garde une instance sur la heightmap

	T_LightmapArray m_pNeighbors[DIR_SIZE];		//!< Les zones voisines

	T_NormalMapBuffer m_normalMap;				//!< NormalMap: Protected by m_mutex
	T_LightMapBuffer m_staticLightmapBuffer;	//!< Données en attente d'etre envoyé à opengl. Protected by m_mutex
	Bool m_staticLightmapChanged;			//!< True if the static lightmap changed

	T_LightMapBuffer m_dynamicLightmapBuffer;

	T_LightMap m_lightMap;		//!< List of lights that must be applied on the lightmap

	Int8 m_offsetLod;		//!< If enabled, this negativ value defines which level will be used for the heightmap

	Texture2D * m_pTexture;		//!< Contains the lightmap

	mutable UInt32 m_refCounter;
	mutable FastMutex m_counterMutex;

	mutable ReadWriteLock m_semaphore;

	/* Restricted functions */
	void addNeighbor(ZoneDirection, PCLODLightmap *);
	void removeNeighbor(ZoneDirection, PCLODLightmap *);
	void clearNeighbors();

	//! @brief Compute the normal map
	//! This internal function computes the normal map given its neighbors.
	//! It is called by O3DPCLODLightmap::RT_Generate
	void buildNormalMap();
};

} // namespace o3d

#endif // _O3D_PCLODMATERIAL_H
