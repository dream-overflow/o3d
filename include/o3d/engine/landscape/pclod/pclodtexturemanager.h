/**
 * @file pclodtexturemanager.h
 * @brief PCLOD texture manager.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PCLODTEXTUREMANAGER_H
#define _O3D_PCLODTEXTUREMANAGER_H

#include "o3d/core/thread.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/evt.h"

#include "o3d/engine/scene/sceneentity.h"
#include "o3d/engine/text2d.h"
#include "../landscape.h"

#include <map>
#include <vector>

namespace o3d {

/* Class definition */
class PCLODMaterial;
class PCLODColormap;
class PCLODLightmap;
class PCLODLight;
class PCLODDebugLabel;

class PCLODZoneManager;
class PCLODTopZone;

class PCLODTerrain;

class FrameBuffer;
class Texture2D;

//---------------------------------------------------------------------------------------
//! @class PCLODTextureManager
//-------------------------------------------------------------------------------------
//! PCLOD texture manager.
//---------------------------------------------------------------------------------------
class O3D_API PCLODTextureManager : public SceneEntity
{
private:

    PCLODTextureManager(BaseObject* parent);

	/* Internal types */

	typedef std::map<UInt32, PCLODMaterial*>	T_MaterialMap;
    typedef T_MaterialMap::iterator				IT_MaterialMap;
    typedef T_MaterialMap::const_iterator		CIT_MaterialMap;

	typedef std::map<UInt32, PCLODColormap*>	T_ColormapMap;
    typedef T_ColormapMap::iterator				IT_ColormapMap;
    typedef T_ColormapMap::const_iterator		CIT_ColormapMap;

	typedef std::map<UInt32, PCLODLightmap*>	T_LightmapMap;
    typedef T_LightmapMap::iterator				IT_LightmapMap;
    typedef T_LightmapMap::const_iterator		CIT_LightmapMap;

    typedef std::vector<PCLODLight*>			T_LightArray;
    typedef T_LightArray::iterator				IT_LightArray;
    typedef T_LightArray::const_iterator		CIT_LightArray;

	typedef std::map<UInt32, PCLODDebugLabel*>	T_LabelMap;
    typedef T_LabelMap::iterator				IT_LabelMap;
    typedef T_LabelMap::const_iterator			CIT_LabelMap;

	typedef std::vector<Int8>					T_LodCurve;

	// Members //
	mutable ReadWriteLock m_semaphore;

    T_MaterialMap m_materialMap;		//!< List of materials
    T_ColormapMap m_colormapMap;		//!< List of colormaps
    T_LightmapMap m_lightmapMap;		//!< List of lightmaps
    T_LightArray m_lightArray;			//!< List of lights
	T_LabelMap m_labelMap;

	String m_textureFile;				//!< Texture file of the terrain

	String m_colormapDirectory;
	String m_materialDirectory;

	PCLODZoneManager * m_pZoneManager;

    SmartObject<Text2D> m_text;	      //!< Used to display debug informations on screen with labels
    FrameBuffer * m_pFrameBuffer;	  //!< Used to create and update labels.

	// Configs //
    SmartArrayInt8 m_staticNoise;	  //!< Texture de bruit extraite des PCLODConfigs.

    T_LodCurve m_colormapLodCurve;	  //!< Courbe de variation du niveau de détail extraite des PCLODConfigs.
	T_LodCurve m_lightmapLodCurve;

	// Restricted //
	PCLODTextureManager(const PCLODTextureManager &);
	PCLODTextureManager & operator = (const PCLODTextureManager &);

	/* Load/Unload texture. */
	void rtLoadColormap(PCLODTopZone * _pTopZone);
	void rtRemoveColormap(UInt32 _colormapId);

	void rtLoadLightmap(PCLODTopZone * _pTopZone);
	void rtRemoveLightmap(UInt32 _lightmapId);

	/* Material are never removed, only unload */
	void rtLoadMaterial(PCLODMaterial * _pMaterial);
	void rtUnloadMaterial(UInt32 _materialId);

public:

    O3D_DECLARE_DYNAMIC_CLASS(PCLODTextureManager)

	/* Constructors */
	PCLODTextureManager(PCLODZoneManager *);
	virtual ~PCLODTextureManager();

	/* Release all textures and materials */
	void destroy();

	/* Functions */

	const PCLODTerrain* getTerrain() const;
	PCLODTerrain* getTerrain();

	const PCLODZoneManager * getZoneManager() const;
	PCLODZoneManager * getZoneManager();

	/* Process main thread events (MAIN THREAD) */
	void mtUpdate();

	/* Process thread events (REFRESH THREAD) */
	void rtUpdate();

	/* Load the manager */
	void load(const String & _texFilePath, const String & _materialDir, const String & _colormapDir);

	/* Initialize the manager */
	void init();

	/* Manually add a material
	 * - _filepath must be an absolute path */
	void registerMaterial(UInt32 _matId, const String _filepath, UInt32 _position = 0);

	//! @brief Add a new light
	void mtAddLight(const TerrainBase::LightInfos &);
	//! @brief Remove a light
	void mtRemoveLight(Light *);
	//! @brief Update a light
	void mtUpdateLight(Light *);

	/* Load and release all used materials and colormap.
	 * Functions called in the refresh thread (REFRESH_THREAD) */
	void rtLoadZone(PCLODTopZone *);
	void rtReleaseZone(PCLODTopZone *);

	/* Return a material and colormap*/
	PCLODMaterial * rtGetMaterial(UInt32 _matId);
	PCLODColormap * rtGetColormap(UInt32 _zoneId);
	PCLODLightmap * rtGetLightmap(UInt32 _zoneId);

	/* Events */

	/* Event thrown when the counter of a texture reach 0. Si sa texture était encore
	 * en mémoire video. Un event GLTextureDeletion a été envoyé. */
	void rteOnColormapUnused();
	void rteOnLightmapUnused();
	void rteOnMaterialUnused();

	/* Event thrown when the quadtree changes due to the camera movement */
	void rteOnQuadTreeMoved();

	/* Event thrown when a texture (colormap/materials) was destroyed, and
	 * need to release the GL texture Id */
    void rteOnLightmapDeletion(PCLODLightmap*);
    void rteOnMaterialDeletion(Texture2D*);
    void rteOnColormapDeletion(PCLODColormap*);

    void rteOnZoneLoaded(PCLODDebugLabel*);
    void rteOnZoneRelease(PCLODDebugLabel*);

private:

    Signal<PCLODLightmap*> onLightmapDeletion{this};
    Signal<Texture2D*> onMaterialDeletion{this};
    Signal<PCLODColormap*> onColormapDeletion{this};

    Signal<PCLODDebugLabel*> onZoneLoaded{this};
    Signal<PCLODDebugLabel*> onZoneRelease{this};
};

} // namespace o3d

#endif // _O3D_PCLODTEXTUREMANAGER_H

