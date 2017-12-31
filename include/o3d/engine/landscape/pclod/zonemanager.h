/**
 * @file zonemanager.h
 * @brief Progressive chunked lod zone manager.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ZONEMANAGER_H
#define _O3D_ZONEMANAGER_H

#include "o3d/core/templatearray2d.h"
#include "o3d/core/vector2.h"
#include "o3d/core/debug.h"

#include "o3d/engine/landscape/pclod/object.h"
#include "o3d/engine/landscape/pclod/pclodrenderer.h"
#include "o3d/engine/landscape/pclod/terrain.h"
#include "o3d/engine/landscape/pclod/configs.h"

#include <map>

namespace o3d {

class PCLODTerrain;
class PCLODZoneRenderer;
class PCLODRendererBase;

class PCLODTextureManager;
class PCLODRenderManager;

#define O3DPCLOD_ZONE_MANAGER_DEFAULT_REFRESH_FREQUENCY 10

//---------------------------------------------------------------------------------------
//! @class PCLODZoneManager
//-------------------------------------------------------------------------------------
//! Progressive chunked lod zone manager.
//---------------------------------------------------------------------------------------
class O3D_API PCLODZoneManager : public SceneEntity, public Runnable
{
private:

	/* Internal type */
	enum ThreadOrder {
		THREAD_RUN = 0,
		THREAD_PAUSE,
		THREAD_STOP	};

	struct PCLODZoneInfo
	{
		UInt32 zoneId;

		UInt32 zoneFilePosition;

		Vector2ui zonePosition;			//!< Coordinate in the array
		Vector2ui zoneExtension;

		SPCLODTopZone pZone;
	};

	/* Class in charge of updating the visible counter of a topzone */
	class PCLODZoneManage
	{
	public:

		/* Constructors */
        PCLODZoneManage(PCLODTopZone * _pZone = nullptr);
		PCLODZoneManage(const PCLODZoneManage & _which);
		~PCLODZoneManage();

		PCLODZoneManage & operator = (const PCLODZoneManage & _which);

		/* Functions */
		Bool isValid() const;

		/* Accessors */
		void setZone(PCLODTopZone * _pZone);
		const SPCLODTopZone & getZone() const;

	private:

		/* Members */
		SPCLODTopZone m_zone;
	};

	typedef std::map<UInt32, PCLODZoneInfo> T_ZoneHeaderMap;
	typedef T_ZoneHeaderMap::iterator		 IT_ZoneHeaderMap;
	typedef T_ZoneHeaderMap::const_iterator  CIT_ZoneHeaderMap;

	typedef std::vector<PCLODRendererBase*>	T_RendererArray;
	typedef T_RendererArray::iterator		IT_RendererArray;
	typedef T_RendererArray::const_iterator	CIT_RendererArray;

	typedef TemplateArray2D<PCLODZoneManage> T_VisibleZoneArray;

	typedef std::map<UInt32, FastMutex> T_ZoneLockedMap;
	typedef T_ZoneLockedMap::iterator	 IT_ZoneLockedMap;

	/* Members */
	Array2DUInt32 m_zoneTableArray;		//!< Store the zone id for each part of the world
	Vector2i m_zoneTableCenter;		//!< Coordinate in the array 2D of the centered zone
	Vector2us m_zoneSize;			//!< Contains the base size of zones
	Vector2f m_zoneUnits;			//!< Base units of the heightmaps

	T_ZoneHeaderMap m_zoneTableMap;	//!< For each zone id, this map contains the zone header

	Vector3 m_worldPosition;			//!< Specify the position in the world of the origin of the terrain quadtree.

	T_VisibleZoneArray m_visibleZoneArray;

	Vector2ui m_viewArea;	//!< Indicate in which zone we are in the m_zoneTableArray
	Vector3 m_viewPosition;	//!< Specify the position of the camera

	UInt32 m_viewDistance;	//!< In heightmap units
	UInt32 m_viewPixelError;

	PCLODTerrain * m_pTerrain;
	PCLODTextureManager * m_pTextureManager;
	PCLODRenderManager * m_pRenderManager;

	String m_headerFilePath;
	String m_dataDirectory;			//!< Directory in which are stored data files

	/* Refreshment */
	Thread m_thread;		    //!< Used for asynchroneous refreshment
	UInt32 m_threadFreq;	    //!< Refresh frequency
	FastMutex m_pauseMutex;	//!< Mutex used to pause the refresh thread
	ThreadOrder m_threadOrder;

	/* Restricted functions */

	//! Load a zone from its id. (REFRESH THREAD)
	//! - Do nothing if the zone is already loaded */
	void rtLoadZone(UInt32 _zoneId, PCLODZoneManage & _zoneInfo);

	//! Remove a zone. (REFRESH THREAD)
	//! - The zone must not be visible.
	//! - If the zone still belongs to an other object, it will be removed
	//! from the zone table and a event will be thrown.
	void rtRemoveZone(UInt32 _zoneId);

	//! Change the current position of the array 2D. (REFRESH THREAD)
	//! Cette fonction est la seule à accéder aux autres membres de la classe
	//! dans le thread de refresh.
	void rtMoveCamera(const Vector2i & _direction);

	//! Create or update the renderer of the specified zone (REFRESH THREAD)
	//! - It can merge and split zone.
	void rtBuildgetRenderer(PCLODZone & _zone);

public:

	O3D_DECLARE_ABSTRACT_CLASS(PCLODZoneManager)

	//! Default constructor
	PCLODZoneManager(PCLODTerrain * _pTerrain);

	//! Destructor
	~PCLODZoneManager();

	void destroy();

	//! Get the terrain parent
	inline       PCLODTerrain* getTerrain()       { return m_pTerrain; }
	//! Get the terrain parent (const version)
	inline const PCLODTerrain* getTerrain() const { return m_pTerrain; }

	//! Charge le terrain à partir du fichier d'entetes
	//! Cette fonction n'initialise pas le terrain. Apres cette fonction, le terrain ne pourra
	//! pas encore être affiché.
    void load(
            InStream & _headerIs,
            const String &_headerFileName,
            const String & _dataDir,
            const String & _texDir,
            const String & _colormapDir);

	//! Cette fonction initialise le terrain à partir de la position spécifiée.
	void init(const Vector3 & _position);
    Bool isInit() const { return (m_pRenderManager != nullptr) && (m_pTextureManager != nullptr); }

	void draw();

	//! Update the gui thread.
	void mtUpdate();

	//! Update the refresh thread
	void rtUpdate();

	//! Remove all visible zones
//	void clear();

	const String & getHeaderFilePath() const { return m_headerFilePath; }
	const String & getDataFilePath() const { return m_dataDirectory; }

	PCLODTextureManager * getTextureManager() const { return m_pTextureManager; }
	PCLODRenderManager * getRenderManager() const { return m_pRenderManager; }

	//! Start/Restart the refresh
	void run();
	Bool refreshRunning() const { return (m_thread.isThread()); }

	//! Stop the refresh thread
	void stop();

	//! Pause the refreshment of the terrain. Available for synchrone and asynchroneous refreshment.
	void pause();
	Bool isPaused() const { return m_pauseMutex.isLocked(); }

	//! Accessors
	void getZoneUnits(Float & _dx, Float & _dy) const { _dx = m_zoneUnits[X]; _dy = m_zoneUnits[Y]; }
	void getZoneUnits(Vector2f & _units) const { _units = m_zoneUnits; }

	void getBaseZoneSize(Vector2ui & _size) const { _size = m_zoneSize; }

	void getCurrentPosition(Vector2ui & _position) const { _position = m_viewArea; }

	Vector2i getIndexFromWorldPosition(const Vector3 & _worldPosition);
	UInt32 getZoneIdFromPosition(const Vector2i & _worldOrigin);

    Thread * getRefreshThread() { return (getTerrain()->getCurrentConfigs().asynchRefresh() ? &m_thread : nullptr); }
    Thread * getMainThread() { return nullptr; }

	/* Events */
    void rteOnZoneVisible(SmartPtr<PCLODZone>);
    void rteOnZoneHide(SmartPtr<PCLODZone>);
    void rteOnZoneUnused(SmartPtr<PCLODZone>);

    void rteOnMainUpdate(Vector3);

    void rteOnRendererRemoved(SmartPtr<PCLODZone>, SmartPtr<PCLODZoneRenderer>);
    void rteOnRendererUpdated(SmartPtr<PCLODZone>, SmartPtr<PCLODZoneRenderer>);

	//! Retourne le niveau de lod pour la zone spécifiée (REFRESH THREAD)
	//! @note FONCTION TEMPORAIRE
	void computeLod(PCLODZone & _zone, UInt8 & _lod);

private:

	//! Function used to refresh one time the terrain. (REFRESH THREAD)
	void rtRefreshOnce(Bool _firstFrame = False);

	//! Static function used with the refresh thread.
	Int32 run(void * _arg);

public:

    Signal<> onQuadtreeMoved{this};
    Signal<Vector3> onMainUpdate{this};  //! Envoi de la nouvelle position de la caméra à thread refresh
};

} // namespace o3d

#endif // _O3D_ZONEMANAGER_H

