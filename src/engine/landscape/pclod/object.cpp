/**
 * @file object.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/core/smartarray.h"
#include "o3d/geom/aabbox.h"
#include "o3d/engine/landscape/pclod/object.h"
#include "o3d/engine/landscape/pclod/zonemanager.h"
#include "o3d/engine/landscape/pclod/heightmapio.h"
#include "o3d/engine/landscape/pclod/pclodtexturemanager.h"
#include "o3d/engine/landscape/pclod/terrain.h"
#include "o3d/core/filemanager.h"

using namespace o3d;

/*---------------------------------------------------------------------------------------
  Class PCLODMaterialLayer
---------------------------------------------------------------------------------------*/
PCLODMaterialLayer::PCLODMaterialLayer():
	m_buffer(),
	m_size(),
	m_offset(0),
	m_stride(0)
{
}

PCLODMaterialLayer::PCLODMaterialLayer(const SmartArrayUInt32 & _buffer, const Vector2ui & _size, UInt32 _offset, UInt32 _stride):
	m_buffer(_buffer),
	m_size(_size),
	m_offset(_offset),
	m_stride(_stride)
{
}

/* Reset the object */
void PCLODMaterialLayer::destroy()
{
	m_buffer.releaseCheckAndDelete();
	m_size = Vector2ui(0, 0);
	m_stride = 0;
	m_offset = 0;
}

/* Build a buffer containing this reduced layer */
void PCLODMaterialLayer::buildBuffer(UInt16 * _pVertexIndex, UInt32 _size, Float * & _buffer)
{
	O3D_ASSERT(m_buffer.getNumElt() > 0);
    O3D_ASSERT(_buffer == nullptr);

	_buffer = new Float[_size];

	Float * pTarget = _buffer;
	UInt16 * pIndex = _pVertexIndex;
	UInt16 * pEnd = &_pVertexIndex[2*_size];

	while (pIndex  != pEnd)
	{
		*pTarget = Float(m_buffer[m_offset + (*pIndex) + (*(pIndex+1))*m_stride]);
		++pTarget;
		pIndex += 2;
	}
}

/*---------------------------------------------------------------------------------------
  Class O3DPCLODHeightmapLayer
---------------------------------------------------------------------------------------*/
PCLODHeightmapLayer::PCLODHeightmapLayer():
	m_buffer(),
	m_size(),
	m_offset(0),
	m_stride(0)
{
}

PCLODHeightmapLayer::PCLODHeightmapLayer(const SmartArrayFloat & _buffer, const Vector2ui & _size, UInt32 _offset, UInt32 _stride):
	m_buffer(_buffer),
	m_size(_size),
	m_offset(_offset),
	m_stride(_stride)
{
}

/* Return the value at the specified point */
Float PCLODHeightmapLayer::operator () (UInt32 _x, UInt32 _y)
{
	O3D_ASSERT(_x < m_size[X]);
	O3D_ASSERT(_y < m_size[Y]);

	return m_buffer[m_offset + _x + _y * m_stride];
}

/* Reset the object */
void PCLODHeightmapLayer::destroy()
{
	m_buffer.releaseCheckAndDelete();
	m_size = Vector2ui(0, 0);
	m_stride = 0;
	m_offset = 0;
}

/*---------------------------------------------------------------------------------------
  Class PCLODZone
---------------------------------------------------------------------------------------*/
PCLODZone::PCLODZone(PCLODTopZone * _pTopZone, PCLODZone * _pParent):
	m_refCounter(0),
	m_mutex(),
	m_zoneLevel(0),
	m_zonePath(0),
	m_heightmapOrigin(),
	m_heightmapMin(0.0f),
	m_heightmapMax(0.0f),
	m_vertexArraySize(),
	m_vertexArrayFilePosition(),
	m_levelArray(),
	m_pParent(_pParent),
	m_pTopParent(_pTopZone),
	m_boundingBox(),
	m_init(False),
	m_loaded(False),
	m_heightmap(),
	m_material(),
	m_vertexArray(),
    m_pRenderer(nullptr),
	m_currentLod(0xFF)
{
}

PCLODZone::~PCLODZone()
{
	releaseRenderer(False);

    if (m_pParent != nullptr) // Evite que deux messages soient affichés pour la destruction d'une topZone
		PCLOD_MESSAGE("Zone : Destroyed");

	destroy();
}

PCLODTerrain* PCLODZone::getTerrain()
{
	return m_pTopParent->getTerrain();
}

/* Totaly destroyed this object, by reseting all values.
 * Must be called after all threads termination and after a call to
 * clean. */
void PCLODZone::destroy()
{
	O3D_ASSERT(m_refCounter == 0);

	// Le renderer a deja été libéré par la fonction clean();
    O3D_ASSERT(m_pRenderer == nullptr);

	m_zoneLevel = 0;
	m_zonePath = 0;
	m_heightmapOrigin = Vector2ui(0, 0);
	m_heightmapMin = 0.0f;
	m_heightmapMax = 0.0f;

	m_vertexArraySize = 0;
	m_vertexArrayFilePosition  = 0;

	m_levelArray.clear();

    m_pParent = nullptr;
    m_pTopParent = nullptr;

	m_boundingBox.destroy();

	m_init = False;
	m_loaded = False;

	m_heightmap.destroy();
	m_material.destroy();

	m_vertexArray.clear();

	m_children[0].releaseCheckAndDelete();
	m_children[1].releaseCheckAndDelete();
	m_children[2].releaseCheckAndDelete();
	m_children[3].releaseCheckAndDelete();
}

void PCLODZone::init(InStream &_headerIs)
{
	O3D_ASSERT(!init());
	O3D_ASSERT(!dataLoaded());

	Char header[3] = { 0 };
    _headerIs.read(header, 2);
	O3D_ASSERT(strncmp(header, "LV", 2) == 0);

    _headerIs >> m_zoneLevel;
	UInt32 zoneDataFilePosition;
    _headerIs >> zoneDataFilePosition;
    _headerIs >> m_heightmapOrigin[X];
    _headerIs >> m_heightmapOrigin[Y];
    _headerIs >> m_heightmapMin;
    _headerIs >> m_heightmapMax;

	UInt32 childrenZonePosition[4];
    _headerIs.read(childrenZonePosition, 4);

    _headerIs >> m_vertexArraySize;
    _headerIs >> m_vertexArrayFilePosition;

	UInt32 lodNumber;
    _headerIs >> lodNumber;

	m_levelArray.resize(lodNumber);

	for (UInt32 k = 0 ; k < lodNumber; ++k)
	{
		PCLODLodInfos & currentLod = m_levelArray[k];

		Char lodHeader[3] = { 0 };
        _headerIs.read(lodHeader, 2);
		O3D_ASSERT(strncmp(lodHeader, "LD", 2) == 0);

        _headerIs >> currentLod.index;

        _headerIs >> currentLod.maxError;
        _headerIs >> currentLod.minError;
        _headerIs >> currentLod.medError;
        _headerIs >> currentLod.medDelta;

        _headerIs >> currentLod.faceNumber;
        _headerIs >> currentLod.lodFilePosition;

		UInt32 renderingBlockNumber;
        _headerIs >> renderingBlockNumber;

		currentLod.renderArray.resize(renderingBlockNumber);
        _headerIs.reader(&currentLod.renderArray[0], sizeof(PCLODRenderingBlock), renderingBlockNumber);
	}

	if (childrenZonePosition[0] > 0)
	{
		for (UInt32 k = 0 ; k < 4 ; ++k)
		{
            _headerIs.reset(childrenZonePosition[k]);

			m_children[k] = new PCLODZone(m_pTopParent, this);
            m_children[k]->init(_headerIs);
		}
	}

	// On connait maintenant le niveau de la zone.
	// On sait que son parent est chargé, on peut donc déterminer m_zonePath;
	if (m_zoneLevel > 0)
	{
        O3D_ASSERT(m_pParent != nullptr);
		O3D_ASSERT(m_pParent->hasChild(this) != -1);

		Int32 index = m_pParent->hasChild(this);
		index <<= 2*(m_zoneLevel-1);

		m_zonePath = m_pParent->m_zonePath | index;
	}

	// Initialisation de la bounding box
	Vector3 lBoundingHalfSize;
	lBoundingHalfSize[X] = getWorldSize()[X];
	lBoundingHalfSize[Y] = (getMaxAltitude() - getMinAltitude());
	lBoundingHalfSize[Z] = getWorldSize()[Y];

	m_boundingBox.setCenter(getWorldCenter());
	m_boundingBox.setSize(lBoundingHalfSize);

	m_init = True;
}

/* Initialize the datas of the zone */
void PCLODZone::initData(InStream & _dataIs, const PCLODHeightmapLayer & _heightmap, const PCLODMaterialLayer & _material)
{
	m_heightmap = _heightmap;
	m_material = _material;

	/* Vertex array */
	m_vertexArray.resize(m_vertexArraySize);

    _dataIs.reset(m_vertexArrayFilePosition);
    _dataIs.read(&m_vertexArray[0].first, 2*m_vertexArraySize);

	/* We load each levels */
	for (UInt32 k = 0 ; k < UInt32(m_levelArray.size()) ; ++k)
	{
		PCLODLodInfos & current = m_levelArray[k];

		current.faceArray.resize(current.faceNumber);

		// Here is the structure of face datas in the data file
		// - 2 byte header: "LD" <= pointed by current.lodFilePosition
		// - 1 ushort : the index of this lod
		// - array of uint

		Char lodHeader[3] = { 0 };
		UInt16 lodIndex;

        _dataIs.reset(current.lodFilePosition);
        _dataIs.read(lodHeader, 2);
        _dataIs >> lodIndex;

		O3D_ASSERT(strncmp((const Char*)lodHeader, "LD", 2) == 0);
		O3D_ASSERT(lodIndex == k);

        _dataIs.read(&current.faceArray[0], current.faceNumber);
	}

	/* Eachs children */
	if (m_children[0])
	{
		for (UInt32 k = 0 ; k < 4 ; ++k)
		{
			PCLODHeightmapLayer heightmap;
			heightmap.setBuffer(_heightmap.getBuffer());
			heightmap.setSize(Vector2ui((_heightmap.getSize()[X] >> 1) + 1, (_heightmap.getSize()[Y] >> 1) + 1));
			heightmap.setStride(_heightmap.getSize()[X]);
			heightmap.setOffset((k % 2) * (_heightmap.getSize()[X] >> 1) + (k >> 1) * (_heightmap.getSize()[X] * (_heightmap.getSize()[Y]/2)));

			PCLODMaterialLayer material;
			material.setBuffer(_material.getBuffer());
			material.setSize(Vector2ui((_material.getSize()[X] >> 1) + 1, (_material.getSize()[Y] >> 1) + 1));
			material.setStride(_material.getSize()[X]);
			material.setOffset((k % 2) * (_material.getSize()[X] >> 1) + (k >> 1) * (_material.getSize()[X] * (_material.getSize()[Y]/2)));

            m_children[k]->initData(_dataIs, heightmap, material);
		}
	}

	m_loaded = True;
}

/* Return the list of materials used by this zone and subzones */
void PCLODZone::buildMaterialSet(T_MaterialSet & _set)
{
	O3D_ASSERT(init());

	// If this zone has children, we directly ask them to return the material set
	// Since a material used by these children, will necessarily be used by this zone
	if (hasChildren())
	{
		for (UInt32 k = 0 ; k < 4 ; ++k)
			m_children[k]->buildMaterialSet(_set);
	}
	else
	{
		// We only have to look at the materials used at the best levels
		// since materials can not appeared

		T_RenderingBlockArray & renderList = m_levelArray.back().renderArray;

		for (IT_RenderingBlockArray it = renderList.begin() ; it != renderList.end() ; it++)
		{
			for (UInt32 k = 0 ; k < it->matNb ; ++k)
				_set.insert(it->matIds[k]);
		}
	}
}

Vector2ui PCLODZone::getZoneSize() const
{
	UInt32 sizeX = m_pTopParent->getZoneSize()[X] >> m_zoneLevel;
	UInt32 sizeY = m_pTopParent->getZoneSize()[Y] >> m_zoneLevel;

	if (m_zoneLevel > 0)
	{
		++sizeX;
		++sizeY;
	}

	return Vector2ui(sizeX, sizeY);
}

Vector2f PCLODZone::getZoneUnits() const
{
	Vector2f units;
	m_pTopParent->getZoneManager()->getZoneUnits(units);

	return units;
}

/* Return the origin and the center of this zone ( Z == 0 for the moment ) */
Vector3 PCLODZone::getWorldOrigin() const
{
	Float dx, dy;
	m_pTopParent->getZoneManager()->getZoneUnits(dx, dy);

	return Vector3(	dx * Float(m_pTopParent->getZoneOrigin()[Y] + m_heightmapOrigin[Y]),
						0.5f * (m_heightmapMin + m_heightmapMax),
						dy * Float(m_pTopParent->getZoneOrigin()[X] + m_heightmapOrigin[X]));
}

Vector3 PCLODZone::getWorldCenter() const
{
	Float dx, dy;
	m_pTopParent->getZoneManager()->getZoneUnits(dx, dy);

	Vector2ui size = getZoneSize();
	size[X] >>= 1;
	size[Y] >>= 1;

	return Vector3(	dy * Float(m_pTopParent->getZoneOrigin()[Y] + m_heightmapOrigin[Y] + size[X]),
						0.5f * (m_heightmapMin + m_heightmapMax),
						dx * Float(m_pTopParent->getZoneOrigin()[X] + m_heightmapOrigin[X] + size[Y]));
}

Vector2f PCLODZone::getWorldSize() const
{
	Float dx, dy;
	m_pTopParent->getZoneManager()->getZoneUnits(dx, dy);

	Vector2ui size = getZoneSize();

	return Vector2f(dx * Float(size[X]-1), dy * Float(size[Y]-1));
}

UInt8 PCLODZone::getChildIndex(UInt8 _level) const
{
	if (_level == 0)
		return 0;

	UInt32 path = m_zonePath >> 2*(_level - 1);
	return UInt8(path & 0x00000003);
}

/* This function build/Destroy the renderer if it doesnt exist */
void PCLODZone::createRenderer()
{
    if (m_pRenderer == nullptr)
	{
		m_pRenderer = new PCLODZoneRenderer(this);
		m_pRenderer->useIt();

        m_pTopParent->onRendererCreated(SmartPtr<PCLODZone>(this), SmartPtr<PCLODZoneRenderer>(m_pRenderer));
	}
	else
		PCLOD_WARNING(String("Zone: Attempt to create a renderer but it already exists"));
}

void PCLODZone::releaseRenderer(Bool _rec)
{
    if (m_pRenderer != nullptr)
	{
		PCLODZoneRenderer * lRenderer = m_pRenderer;

		m_pRenderer->releaseIt();
        m_pRenderer = nullptr;
		m_currentLod = 0xFF;

        m_pTopParent->onRendererRemoved(SmartPtr<PCLODZone>(this), SmartPtr<PCLODZoneRenderer>(lRenderer));
	}

	if ((_rec) && hasChildren())
	{
		m_children[0]->releaseRenderer(True);
		m_children[1]->releaseRenderer(True);
		m_children[2]->releaseRenderer(True);
		m_children[3]->releaseRenderer(True);
	}
}

/* This function destroys the renderer of all children zone.
 * Called by CreateRenderer */
void PCLODZone::releaseChildrenRenderer()
{
	if (hasChildren())
	{
		m_children[0]->releaseRenderer(True);
		m_children[1]->releaseRenderer(True);
		m_children[2]->releaseRenderer(True);
		m_children[3]->releaseRenderer(True);
	}
}

Int32 PCLODZone::hasChild(PCLODZone * pZone) const
{
	for (Int32 k = 0 ; k < 4 ; ++k)
	{
		if (m_children[k].get() == pZone)
			return k;
	}

	return -1;
}

/* UseIt/ReleaseIt */
void PCLODZone::useIt() const
{
	FastMutexLocker locker(m_mutex);
	const_cast<PCLODZone*>(this)->m_refCounter++;
}

void PCLODZone::releaseIt() const
{
	FastMutexLocker locker(m_mutex);
	O3D_ASSERT(m_refCounter > 0);

	const_cast<PCLODZone*>(this)->m_refCounter--;
}

UInt32 PCLODZone::getRefCounter() const
{
	FastMutexLocker locker(m_mutex);
	return m_refCounter;
}

Bool PCLODZone::noLongerUsed() const
{
	FastMutexLocker locker(m_mutex);
	return (m_refCounter == 0);
}

/* This function update the renderer.
 * The level 0 of this zone is loaded into the renderer. */
void PCLODZone::rtUpdateRenderer(UInt8 _lod)
{
	O3D_ASSERT(dataLoaded());
    O3D_ASSERT(m_pTopParent != nullptr);
    O3D_ASSERT(m_pRenderer != nullptr);

	// La plupart du temps (99,99%) cette condition est vérifiée
	if (_lod == m_currentLod)
		return;

	if (m_levelArray.size() == 0)
	{
		PCLOD_ERROR(String("Zone ") << m_pTopParent->getId() << " : Attempt to update a renderer, but the zone (" << m_pTopParent->getId() << ") is empty");
		return;
	}

	if (_lod >= UInt8(m_levelArray.size()))
		_lod = UInt8(m_levelArray.size()) - 1;

	PCLODLodInfos & currentLod = m_levelArray[_lod];

	// Vertex Buffer and coord buffer
	Float * pVertexBuffer = new Float[3*m_vertexArray.size()];
	Float * pCoordBuffer = new Float[2*m_vertexArray.size()];

	Vector2ui zoneSize = m_heightmap.getSize();

	Float fx, fz;
	Float ifwidth = 1.0f/Float(zoneSize[X]-1), ifheight = 1.0f/Float(zoneSize[Y]-1);

	Float coordX = Float(m_heightmapOrigin[X]) * ifwidth;
	Float coordZ = Float(m_heightmapOrigin[Y]) * ifheight;

	Vector3 zoneWorldOrigin = getWorldOrigin();

	for (UInt32 k = 0 ; k < UInt32(m_vertexArray.size()) ; ++k)
	{
		fz = Float(m_vertexArray[k].first);
		fx = Float(m_vertexArray[k].second);

		pVertexBuffer[3*k + 0] = zoneWorldOrigin[X] + fx;
		pVertexBuffer[3*k + 1] = m_heightmap(m_vertexArray[k].first, m_vertexArray[k].second);
		pVertexBuffer[3*k + 2] = zoneWorldOrigin[Z] + fz;

		pCoordBuffer[2*k + 0] = coordX + fx * ifwidth;
		pCoordBuffer[2*k + 1] = coordZ + fz * ifheight;
	}

	// Index Buffer
	UInt32 * pIndexBuffer = new UInt32[UInt32(currentLod.faceArray.size())];
	memcpy((void*)pIndexBuffer, (const void*)&currentLod.faceArray[0], UInt32(currentLod.faceArray.size()) * sizeof(UInt32));

	// Attrib buffer
    Float * pAttribBuffer = nullptr;

	m_material.buildBuffer(&m_vertexArray[0].first, UInt32(m_vertexArray.size()), pAttribBuffer);

	// Rendering blocks
	PCLODZoneRenderer::T_RenderingBlockInfoArray renderingParts;
	renderingParts.reserve(currentLod.renderArray.size());

	PCLODZoneRenderer::PCLODRenderingBlock renderPart;

	PCLODTextureManager * pTexManager = m_pTopParent->getZoneManager()->getTextureManager();

	for (IT_RenderingBlockArray it = currentLod.renderArray.begin(); it != currentLod.renderArray.end() ; it++)
	{
		memcpy((void*)&renderPart, (const void*)&(*it)/*it.operator->()*/, 3*sizeof(UInt32));
		memset((void*)&renderPart.matPtr, 0, 3*sizeof(PCLODMaterial*));

		for (UInt32 k = 0 ; k < it->matNb ; ++k)
		{
			renderPart.matPtr[k] = pTexManager->rtGetMaterial(it->matIds[k]);
		}

		renderingParts.push_back(renderPart);
	}

	// Envoi des données
	SmartArrayFloat dataVertexBuffer(pVertexBuffer, UInt32(3*m_vertexArray.size()), True);
	SmartArrayUInt32 dataIndexBuffer(pIndexBuffer, UInt32(currentLod.faceArray.size()), True);
	SmartArrayFloat dataAttribBuffer(pAttribBuffer, UInt32(m_vertexArray.size()), True);

	deleteArray(pCoordBuffer);
	SmartArrayFloat dataCoordBuffer;

    PCLODColormap * pColormap = nullptr;
    PCLODLightmap * pLightmap = nullptr;

	if (m_currentLod == 0xFF)
	{
		// On évite d'appeler le texture manager lorsque la texture est deja chargée. Ceci
		// pour éviter les conflits de thread. De plus, aucune utilité de dépenser du
		// cpu pour les recherches dans les std::map
		pColormap = pTexManager->rtGetColormap(m_pTopParent->getId());
		pLightmap = pTexManager->rtGetLightmap(m_pTopParent->getId());
	}

	m_pRenderer->setData(	dataVertexBuffer,
							dataIndexBuffer,
							dataAttribBuffer,
							dataCoordBuffer,
							renderingParts,
							pColormap,
							pLightmap);

	m_currentLod = _lod;

    m_pTopParent->onRendererUpdated(SmartPtr<PCLODZone>(this), SmartPtr<PCLODZoneRenderer>(m_pRenderer));
}

/* Return the number of lod available with this zone and all subzones if rec == true*/
UInt32 PCLODZone::getLodNumber(Bool _rec)
{
	if (_rec && hasChildren())
		return (UInt32(m_levelArray.size()) + m_children[0]->getLodNumber(True));
	else
		return (UInt32(m_levelArray.size()));
}

/* This function return true if the renderer is working.
 * If rec = true, it returns true if a renderer of  this zone or the children zone is working */
Bool PCLODZone::isRendererActive(Bool _rec) const
{
	if (_rec && hasChildren())
        return ((m_pRenderer != nullptr) || m_children[0]->isRendererActive(True));
    else return (m_pRenderer != nullptr);
}

/*---------------------------------------------------------------------------------------
  Class PCLODTopZone
---------------------------------------------------------------------------------------*/
PCLODTopZone::PCLODTopZone(InStream &_headerIs, PCLODZoneManager * _zoneManager):
    PCLODZone(nullptr, nullptr),
	m_id(0),
	m_origin(),
	m_size(),
	m_dataFileName(),
	m_dataFilePosition(0),
	m_heightmapFilePosition(0),
	m_materialFilePosition(0),
    m_pZoneManager(nullptr)
{
	memset((void*)m_counters, 0, COUNTER_NUMBER * sizeof(Int32));

	setTopParent(this);

    O3D_ASSERT(_zoneManager != nullptr);

	m_pZoneManager = _zoneManager;

	// On vérifie que l'entete de zone est correcte.
	Char header[5] = {0};
    _headerIs.read(header, 4);
	O3D_ASSERT(strncmp(header, "ZONE", 4) == 0);

    _headerIs >> m_id;
    _headerIs >> m_origin[X];
    _headerIs >> m_origin[Y];
    _headerIs >> m_size[X];
    _headerIs >> m_size[Y];
    _headerIs >> m_dataFileName;
    _headerIs >> m_dataFilePosition;
    _headerIs >> m_heightmapFilePosition;
    _headerIs >> m_materialFilePosition;

    init(_headerIs);
}

PCLODTopZone::~PCLODTopZone()
{
    O3D_ASSERT(m_pZoneManager != nullptr);
	O3D_ASSERT(unused());

	PCLOD_MESSAGE(String("TopZone ") << m_id << " : Destroyed");
}

void PCLODTopZone::destroy()
{
	O3D_ASSERT(unused());

	PCLODZone::destroy();

	m_id = 0;

	m_origin = Vector2i(0,0);
	m_size = Vector2ui(0,0);

	m_dataFileName.destroy();
	m_dataFilePosition = 0;

	m_heightmapFilePosition = 0;
	m_materialFilePosition = 0;

    m_pZoneManager = nullptr;
}

PCLODTerrain* PCLODTopZone::getTerrain()
{
	return m_pZoneManager->getTerrain();
}

const PCLODTerrain* PCLODTopZone::getTerrain() const
{
	return m_pZoneManager->getTerrain();
}

const Vector2ui PCLODTopZone::getZoneIndex() const
{
    O3D_ASSERT(m_pZoneManager != nullptr);

	Vector2ui baseSize;
	m_pZoneManager->getBaseZoneSize(baseSize);

	return Vector2ui((m_origin[X])/(baseSize[X] - 1), (m_origin[Y])/(baseSize[Y]-1));
}

const Vector2ui PCLODTopZone::getZoneExtension() const
{
    O3D_ASSERT(m_pZoneManager != nullptr);

	Vector2ui baseSize;
	m_pZoneManager->getBaseZoneSize(baseSize);

	return Vector2ui((m_size[X]-1)/(baseSize[X] - 1), (m_size[Y]-1)/(baseSize[Y]-1));
}

/* Load the datas of the zone (must be initialized first) */
void PCLODTopZone::load()
{
	O3D_ASSERT(init());
	O3D_ASSERT(!dataLoaded());

	String fullPath;
	fullPath << m_pZoneManager->getDataFilePath() << String("/") << m_dataFileName;

    AutoPtr<InStream> pdataIs(FileManager::instance()->openInStream(fullPath));

    if (pdataIs.get() == nullptr)
	{
		PCLOD_ERROR(String("Impossible to load the zone. Invalid file : ") << fullPath);
		return;
	}

    InStream &dataIs = *pdataIs;

	/* Heightmap */
    Float * _buffer = nullptr;
    dataIs.reset(m_heightmapFilePosition);
    if (!HeightmapIO::instance()->readFromFile(dataIs, _buffer, m_size.x(), m_size.y()))
	{
		PCLOD_ERROR(String("Heightmap invalid or format not supported"));
		return;
	}

	SmartArrayFloat heightmapData(_buffer, m_size[X] * m_size[Y], True);
	PCLODHeightmapLayer heightmap(heightmapData, m_size, 0, m_size[X]);

	/* Materials */
	PCLODMaterialLayer materialLayer;

	if (m_materialFilePosition > 0)
	{
        SmartArrayUInt32 materialData(m_size.x() * m_size.y());
        dataIs.reset(m_materialFilePosition);
        dataIs.read(materialData.getData(), m_size.x() * m_size.y());

		materialLayer.setBuffer(materialData);
        materialLayer.setSize(m_size);
        materialLayer.setStride(m_size.x());
		materialLayer.setOffset(0);
	}

    initData(dataIs, heightmap, materialLayer);

	PCLOD_MESSAGE(String("Zone : " ) << getId() << " : Data successfuly loaded");
}

/* Unload all datas which are not used any more */
void PCLODTopZone::unload()
{
	O3D_ASSERT(init());
	O3D_ASSERT(dataLoaded());

	PCLOD_ERROR(String("Zone : " ) << getId() << " : Data released");
}

/* Return the list of materials used by this zone */
void PCLODTopZone::getMaterialSet(T_MaterialSet & _set)
{
	buildMaterialSet(_set);
}

/* Return the heightmap buffer */
void PCLODTopZone::getHeightmap(SmartArrayFloat & _heightmap, Vector2ui & _size)
{
	_heightmap = m_heightmap.getBuffer();
	_size = m_heightmap.getSize();
}

/* Return the material buffer */
void PCLODTopZone::getMaterial(SmartArrayUInt32 & _material, Vector2ui & _size)
{
	_material = m_material.getBuffer();
	_size = m_material.getSize();
}

/* All counters */
void PCLODTopZone::use(CounterType _type)
{
	switch(_type)
	{
		// The ZoneManage du ZoneManager utilise ce compteur. Ca permet de connaitre le nombre de fois
		// que l'object VisibleZoneArray fait référence a cette zone. Dans le cas ou l'extension de celle-ci
		// est différente de (1,1)
	case ZONE_VISIBILITY:
		{
			m_counters[ZONE_VISIBILITY]++;

			if (m_counters[ZONE_VISIBILITY] == 1)
			{
                if (m_pRenderer != nullptr)
					PCLOD_WARNING(String("Zone ") << m_pTopParent->getId() << " : The zone juste appeared but its renderer is already defined");

                onZoneVisible(SmartPtr<PCLODZone>(this));
			}
		}
		break;
		// Le compteur de heightmap est utilisé par les lightmaps.
	case ZONE_HEIGHTMAP:
		{
			m_counters[ZONE_HEIGHTMAP]++;

			// On suppose pour le moment, que ce compteur est utilisé apres le chargement de la zone
			O3D_ASSERT(m_heightmap.getBuffer().getNumElt() > 0);
		}
		break;
	// Le compteur de material est utilisé par les colormaps.
	case ZONE_MATERIAL:
		{
			m_counters[ZONE_MATERIAL]++;

			O3D_ASSERT(m_material.getBuffer().getNumElt() > 0);
		}
		break;
	default:
		O3D_ASSERT(0);
		break;
	}
}

void PCLODTopZone::release(CounterType _type)
{
	O3D_ASSERT(m_counters[_type] > 0);

	switch(_type)
	{
	case ZONE_VISIBILITY:
		{
			m_counters[ZONE_VISIBILITY]--;

			if (m_counters[ZONE_VISIBILITY] == 0)
			{
				// Cette assertion permet d'arrêter l'éxécution sur ce point pour pour analyser qui
				// a libérer une référence.
#ifdef _MSC_VER
#pragma COMPIL_MSG("Test de heightmap enlever pour le moment")
#endif
				//O3D_ASSERT(m_counters[ZONE_HEIGHTMAP] > 0);

				releaseRenderer(True);

                onZoneHidden(SmartPtr<PCLODZone>(this));

				PCLOD_MESSAGE(String("Zone ") << getId() << " : Disappeared");
			}

		}
		break;
	case ZONE_HEIGHTMAP:
		{
			m_counters[ZONE_HEIGHTMAP]--;

			if (m_counters[ZONE_HEIGHTMAP] == 0)
			{
				// Pour le moment, je vois pas comment il serait possible que la heightmap ne soit plus utilisée
				// alors que la zone est tjs affichée. Les seuls possesseur éventuels de la heightmap, sont la
				// lightmap et la shadow map. Dès que la zone est visible, ces textures sont utilisées.
				O3D_ASSERT(m_counters[ZONE_VISIBILITY] == 0);

                onZoneHeightmapUnused(SmartPtr<PCLODZone>(this));

				PCLOD_MESSAGE(String("Zone ") << getId() << " : Heightmap not used anymore");
			}
		}
		break;
	// Le compteur de material est utilisé par les colormaps.
	case ZONE_MATERIAL:
		{
			m_counters[ZONE_MATERIAL]--;

			PCLOD_MESSAGE(String("Zone ") << getId() << " : Materials not used anymore");
		}
		break;
	default:
		O3D_ASSERT(0);
		break;
	}

	if (unused())
        onZoneUnused(SmartPtr<PCLODZone>(this));
}

