/**
 * @file object.h
 * @brief PCLOD object.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PCLODOBJECT_H
#define _O3D_PCLODOBJECT_H

#include "o3d/core/vector2.h"
#include "o3d/geom/aabbox.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/evt.h"

#include "zonerenderer.h"
#include <vector>
#include <set>

namespace o3d {

class PCLODZone;
class PCLODTopZone;
class PCLODZoneManager;

typedef SmartPtr<PCLODZone>		    SPCLODZone;
typedef SmartPtr<PCLODTopZone>	    SPCLODTopZone;

typedef std::set<UInt32>			T_MaterialSet;
typedef T_MaterialSet::iterator			IT_MaterialSet;
typedef T_MaterialSet::const_iterator	CIT_MaterialSet;

//---------------------------------------------------------------------------------------
//! @class PCLODMaterialLayer
//-------------------------------------------------------------------------------------
//! PCLOD material layer.
//---------------------------------------------------------------------------------------
class O3D_API PCLODMaterialLayer
{
public:

	/* Constructors */

	PCLODMaterialLayer();
	PCLODMaterialLayer(const SmartArrayUInt32 & _buffer, const Vector2ui & _size, UInt32 _offset = 0, UInt32 _stride = 0);

	/* Reset the object */
	void destroy();

	/* Accessors */
	void setBuffer(const SmartArrayUInt32 & _buffer) { m_buffer = _buffer; }
	const SmartArrayUInt32 & getBuffer() const { return m_buffer; }

	void setSize(const Vector2ui & _size) { m_size = _size; }
	const Vector2ui & getSize() const { return m_size; }

	void setOffset(UInt32 _offset) { m_offset = _offset; }
	UInt32 getOffset() const { return m_offset; }

	void setStride(UInt32 _stride) { m_stride = _stride; }
	UInt32 getStride() const { return m_stride; }

	/* Functions */

	/* Build a buffer containing this reduced layer */
	void buildBuffer(UInt16 * _pVertexIndex, UInt32 _size, Float * & _buffer);

private:

	SmartArrayUInt32 m_buffer;

	Vector2ui m_size;

	UInt32 m_offset;		//!< Position of the first element
	UInt32 m_stride;		//!< Space between the first element of two rows
};

//---------------------------------------------------------------------------------------
//! @class PCLODHeightmapLayer
//-------------------------------------------------------------------------------------
//! PCLOD height map layer.
//---------------------------------------------------------------------------------------
class O3D_API PCLODHeightmapLayer
{
public:

	/* Constructors */
	PCLODHeightmapLayer();
	PCLODHeightmapLayer(const SmartArrayFloat & _buffer, const Vector2ui & _size, UInt32 _offset = 0, UInt32 _stride = 0);

	/* Reset the object */
	void destroy();

	/* Return the value at the specified point */
	Float operator () (UInt32 _x, UInt32 _y);

	/* Accessors */
	void setBuffer(const SmartArrayFloat & _buffer) { m_buffer = _buffer; }
	const SmartArrayFloat & getBuffer() const { return m_buffer; }

	void setSize(const Vector2ui & _size) { m_size = _size; }
	const Vector2ui & getSize() const { return m_size; }

	void setOffset(UInt32 _offset) { m_offset = _offset; }
	UInt32 getOffset() const { return m_offset; }

	void setStride(UInt32 _stride) { m_stride = _stride; }
	UInt32 getStride() const { return m_stride; }

private:

	SmartArrayFloat m_buffer;

	Vector2ui m_size;		//!< Taille de la heightmap

	UInt32 m_offset;		//!< Position of the first element
	UInt32 m_stride;		//!< Space between the first element of two rows
};

//---------------------------------------------------------------------------------------
//! @class PCLODZone
//-------------------------------------------------------------------------------------
//! PCLOD zone.
//---------------------------------------------------------------------------------------
class O3D_API PCLODZone : public EvtHandler
{
	friend class PCLODZoneManager;

public:

	/* Type definition */

	/* This enum is used to specified how the renderer will be built */
	enum PCLODZoneCriteria {
		PCLOD_ZONE_MAX_ERROR,
		PCLOD_ZONE_MED_ERROR,
		PCLOD_ZONE_MIN_ERROR };

protected:

	/* Internal type */
	typedef std::vector<std::pair<UInt16, UInt16> >	T_IndexVertexArray;
	typedef T_IndexVertexArray::iterator				IT_IndexVertexArray;
	typedef T_IndexVertexArray::const_iterator			CIT_IndexVertexArray;

	typedef std::vector<UInt32>				T_FaceIndexArray;
	typedef T_FaceIndexArray::iterator			IT_FaceIndexArray;
	typedef T_FaceIndexArray::const_iterator	CIT_FaceIndexArray;

	struct PCLODRenderingBlock
	{
		/* Members */
		UInt32 indexStart;
		UInt32 indexEnd;

		UInt32 matNb;
		UInt32 matIds[3];
	};

	typedef std::vector<PCLODRenderingBlock>		T_RenderingBlockArray;
	typedef T_RenderingBlockArray::iterator			IT_RenderingBlockArray;
	typedef T_RenderingBlockArray::const_iterator	CIT_RenderingBlockArray;

	typedef std::list<PCLODZoneRenderer *>			T_ZoneRendererList;
	typedef T_ZoneRendererList::iterator			IT_ZoneRendererList;
	typedef T_ZoneRendererList::const_iterator		CIT_ZoneRendererList;

	struct PCLODLodInfos
	{
		/* Members */
		UInt16 index;

		Float maxError;
		Float minError;
		Float medError;
		Float medDelta;

		UInt32 faceNumber;
		UInt32 lodFilePosition;

		T_RenderingBlockArray renderArray;

		/* Datas */
		T_FaceIndexArray faceArray;
	};

	typedef std::vector<PCLODLodInfos> O3D_T_LevelArray;

	/* Members */
	UInt32 m_refCounter;			//!< Used to know how many object own a reference of this zone
	mutable FastMutex m_mutex;

	UInt16 m_zoneLevel;
	UInt32 m_zonePath;			//!< Contains the exact position relative to its topParent

	Vector2ui m_heightmapOrigin;	//!< Position on the base heightmap
	Float m_heightmapMin;		//!< Minimum of the altitude in the subpart of the heightmap corresponding to this zone
	Float m_heightmapMax;

	UInt32 m_vertexArraySize;
	UInt32 m_vertexArrayFilePosition;

	O3D_T_LevelArray m_levelArray;

	PCLODZone * m_pParent;
	PCLODTopZone * m_pTopParent;

	SPCLODZone m_children[4];

	AABBox m_boundingBox;

	/* State */
	Bool m_init;	//!< Means that the zone header was read
	Bool m_loaded;	//!< Means that the datas are loaded

	/* Datas */
	PCLODHeightmapLayer m_heightmap;

	PCLODMaterialLayer m_material;

	T_IndexVertexArray m_vertexArray;	//!< Contains the index of all used vertices

	/* Renderer */
	PCLODZoneRenderer * m_pRenderer;
	UInt8 m_currentLod;					//!< What lod is currently waiting or loaded in the renderer

protected: /* These functions will only we called by the zone top instance */

	/* Constructors */
	PCLODZone(PCLODTopZone * _pTopZone, PCLODZone * _pParent);

	/* Functions */

	/* Build the zone */
    void init(InStream & _headerIs);

	/* Initialize the datas of the zone */
    void initData(InStream &_dataIs, const PCLODHeightmapLayer &, const PCLODMaterialLayer &);

	/* Return the list of materials used by this zone and subzones */
	void buildMaterialSet(T_MaterialSet & _set);

	/* This function build/Destroy the renderer if it doesnt exist */
	void createRenderer();
	void releaseRenderer(Bool _rec = False);

	/* This function destroys the renderer of all children zone.
	 * Called by CreateRenderer */
	void releaseChildrenRenderer();

public:

	/* Destructor is public. Needed for SmartPtr */
	virtual ~PCLODZone();

	/* Totaly destroyed this object, by reseting all values.
	 * Must be called after all threads termination and after a call to
	 * clean. */
	virtual void destroy();

	PCLODTerrain* getTerrain();

	/* Return the state of the zone */
	Bool init() const { return m_init; }
	Bool dataLoaded() const { return m_loaded; }

	/* Return the type of the zone */
	Bool isTopLevelZone() const { return (m_zoneLevel == 0); }

	/* Return the origin and the center of this zone ( Z == 0 for the moment ) */
	Vector2ui getZoneSize() const;
	Vector2f getZoneUnits() const;

	Vector3 getWorldOrigin() const;
	Vector3 getWorldCenter() const;
	Vector2f getWorldSize() const;

	Float getMinAltitude() const { return m_heightmapMin; }
	Float getMaxAltitude() const { return m_heightmapMax; }

	UInt16 getZoneLevel() const { return m_zoneLevel; }
	UInt8 getChildIndex(UInt8) const;
	UInt32 getZonePath() const { return m_zonePath; }

	const AABBox & getBoundingBox() const { return m_boundingBox; }

	/* Return the parent top zone */
	void setTopParent(PCLODTopZone * _pTopZone) { m_pTopParent = _pTopZone; }
	PCLODTopZone * getTopParent() const { return m_pTopParent; }

	void setParent(PCLODZone * _pParent) { m_pParent = _pParent; }
	PCLODZone * getParent() const { return m_pParent; }

	PCLODZone * getChild(UInt8 _index) { O3D_ASSERT(_index < 4); return m_children[0].get(); }

	Bool hasChildren() const { return (m_children[0]); }
	Int32 hasChild(PCLODZone *) const;

	/* UseIt/ReleaseIt */
	void useIt() const;
	void releaseIt() const;

	UInt32 getRefCounter() const;
	Bool noLongerUsed() const;

	inline Bool canRemove() const { return noLongerUsed(); }
	inline Bool isOwned() const { return False; }
	virtual Bool deleteIt()
	{
		delete(this);
		return True;
	}

	/* Renderer */
	PCLODZoneRenderer * getRenderer() const { return m_pRenderer; }

	/* This function update the renderer.
	 * The level 0 of this zone is loaded into the renderer.
	 * - If _lod > max_lod, the lod max_lod will be loaded.
	 * - Need to be public, because the renderer call this function.
	 * - If _lod == m_currentLod, return immédiately */
	void rtUpdateRenderer(UInt8 _lod);

	/* Return the number of lod available with this zone and all subzones if rec == true
	 * On suppose que chaque zone fille à la meme profondeur (je me comprend) */
	UInt32 getLodNumber(Bool _rec = False);

	/* This function return true if the renderer is working.
	 * If rec = true, it returns true if a renderer of  this zone or the children zone is working */
	Bool isRendererActive(Bool _rec = False) const;
};

//---------------------------------------------------------------------------------------
//! @class PCLODTopZone
//-------------------------------------------------------------------------------------
//! PCLOD top zone.
//---------------------------------------------------------------------------------------
class O3D_API PCLODTopZone : public PCLODZone
{
public:

	enum CounterType {
		ZONE_VISIBILITY = 0,
		ZONE_HEIGHTMAP,
		ZONE_MATERIAL,

		COUNTER_NUMBER };

private:

	/* Members */
	UInt32 m_id;

	Vector2i m_origin;	//!< Position of the zone in the world
	Vector2ui m_size;	//!< Size of the zone

	String m_dataFileName;
	UInt32 m_dataFilePosition;

	UInt32 m_heightmapFilePosition;
	UInt32 m_materialFilePosition;

	PCLODZoneManager * m_pZoneManager;

	//!< Array which contains all counters. Ca permet de savoir qui utilisent quoi et de libérer les ressources possibles
	Int32 m_counters[COUNTER_NUMBER];

public:

	/* Constructors */
    PCLODTopZone(InStream & _headerIs, PCLODZoneManager * _zoneManager);
	virtual ~PCLODTopZone();

	virtual void destroy();

	PCLODTerrain* getTerrain();
	const PCLODTerrain* getTerrain() const;

	/* Accessors */
	UInt32 getId() const { return m_id; }
	const Vector2i & getZoneOrigin() const { return m_origin; }
	const Vector2ui & getZoneSize() const { return m_size; }
	const Vector2ui getZoneIndex() const;
	const Vector2ui getZoneExtension() const;

	const String & getDataFileName() const { return m_dataFileName; }

	/* Load the datas of the zone (must be initialized first) */
	void load();

	/* Unload all datas which are not used any more */
	void unload();

	/* Return the list of materials used by this zone */
	void getMaterialSet(T_MaterialSet & _set);

	/* Return the heightmap buffer */
	void getHeightmap(SmartArrayFloat & _heightmap, Vector2ui & _size);

	/* Return the material buffer */
	void getMaterial(SmartArrayUInt32 & _material, Vector2ui & _size);

	/* Return the zone manager used with this zone */
	PCLODZoneManager * getZoneManager() const { return m_pZoneManager; }

	/* All counters */
	void use(CounterType _type);
	void release(CounterType type);

	/* Return the visibility counter */
	UInt32 getCounter(CounterType _type) const { return m_counters[_type]; }

	/* Return true if all counters are 0 */
	Bool unused() const { Int32 buffer[COUNTER_NUMBER] = { 0 }; return (memcmp((const void*)buffer, (const void*)m_counters, COUNTER_NUMBER * sizeof(Int32)) == 0); }

public:

    Signal<SmartPtr<PCLODZone>, SmartPtr<PCLODZoneRenderer> > onRendererCreated{this};
    Signal<SmartPtr<PCLODZone>, SmartPtr<PCLODZoneRenderer> > onRendererRemoved{this};
    Signal<SmartPtr<PCLODZone>, SmartPtr<PCLODZoneRenderer> > onRendererUpdated{this};

    Signal<SmartPtr<PCLODZone> > onZoneVisible{this};
    Signal<SmartPtr<PCLODZone> > onZoneHidden{this};
    Signal<SmartPtr<PCLODZone> > onZoneUnused{this};
    Signal<SmartPtr<PCLODZone> > onZoneHeightmapUnused{this};
};

} // namespace o3d

#endif // _O3D_PCLODOBJECT_H

