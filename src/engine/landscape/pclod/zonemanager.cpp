/**
 * @file zonemanager.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/core/smartarray.h"
#include "o3d/core/vector2.h"
#include "o3d/geom/aabbox.h"
#include "o3d/engine/landscape/pclod/zonemanager.h"

//#include "o3d/engine/landscape/pclod/Terrain.h"
#include "o3d/engine/landscape/pclod/pclodtexturemanager.h"
#include "o3d/engine/landscape/pclod/rendermanager.h"
#include "o3d/engine/landscape/pclod/heightmapio.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/primitive/primitivemanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/scene/scene.h"

#include "o3d/core/objects.h"
#include "o3d/core/filemanager.h"

//#include <memory>

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(PCLODZoneManager, ENGINE_PCLOD_ZONE_MANAGER, BaseObject)

/*==============================================================*\
		class PCLODZoneManager::PCLODZoneManage
\*==============================================================*/

PCLODZoneManager::PCLODZoneManage::PCLODZoneManage(PCLODTopZone * _pZone) :
	m_zone(_pZone)
{
	if (m_zone)
		m_zone->use(PCLODTopZone::ZONE_VISIBILITY);
}

PCLODZoneManager::PCLODZoneManage::PCLODZoneManage(const PCLODZoneManage & _which):
	m_zone(_which.m_zone)
{
	if (m_zone)
		m_zone->use(PCLODTopZone::ZONE_VISIBILITY);
}

PCLODZoneManager::PCLODZoneManage::~PCLODZoneManage()
{
	if (m_zone)
	{
		O3D_ASSERT(m_zone->getRefCounter() > 1);
		m_zone->release(PCLODTopZone::ZONE_VISIBILITY);
	}
}

PCLODZoneManager::PCLODZoneManage & PCLODZoneManager::PCLODZoneManage::operator = (const PCLODZoneManage & _which)
{
	O3D_ASSERT(&_which != this);

	if (m_zone)
	{
		O3D_ASSERT(m_zone->getRefCounter() > 1);
		O3D_ASSERT(m_zone->getCounter(PCLODTopZone::ZONE_VISIBILITY) > 0);

		m_zone->release(PCLODTopZone::ZONE_VISIBILITY);
		m_zone.releaseCheckAndDelete();
	}
	m_zone = _which.m_zone;

	if (m_zone)
		m_zone->use(PCLODTopZone::ZONE_VISIBILITY);

	return *this;
}

/* Functions */
Bool PCLODZoneManager::PCLODZoneManage::isValid() const
{
	return (m_zone);
}

/* Accessors */
void PCLODZoneManager::PCLODZoneManage::setZone(PCLODTopZone * _pZone)
{
	if (m_zone)
	{
		O3D_ASSERT(m_zone->getRefCounter() > 1);
		m_zone->release(PCLODTopZone::ZONE_VISIBILITY);
		m_zone.releaseCheckAndDelete();
	}

	m_zone = SPCLODTopZone(_pZone);

	if (m_zone)
		m_zone->use(PCLODTopZone::ZONE_VISIBILITY);
}

const SPCLODTopZone & PCLODZoneManager::PCLODZoneManage::getZone() const
{
	return m_zone;
}

/*==============================================================*\
					class O3DPCLODZoneManager
\*==============================================================*/

/* Remove a zone.
 * - The zone must not be visible.
 * - The zone must be loaded.
 * - If the zone still belongs to an other object, it will be removed
     from the zone table and a event will be thrown. */
void PCLODZoneManager::rtRemoveZone(UInt32 _zoneId)
{
	// On extrait la structure d'info sur la zone
	IT_ZoneHeaderMap itZone = m_zoneTableMap.find(_zoneId);
	O3D_ASSERT(itZone != m_zoneTableMap.end());

	PCLODZoneInfo & zoneInfo = itZone->second;

    O3D_ASSERT(zoneInfo.pZone.get() != nullptr);
	PCLODTopZone * pZone = zoneInfo.pZone.get();

	O3D_ASSERT(pZone->unused());

	O3D_ASSERT(!pZone->isRendererActive());

	zoneInfo.pZone = SPCLODTopZone();

	PCLOD_MESSAGE(String("ZoneManager : Zone ") << pZone->getId() << " : Remove from the zoneTable");
}

/* Load a zone from its id.
 * - Do nothing if the zone is already loaded */
void PCLODZoneManager::rtLoadZone(UInt32 _zoneId, PCLODZoneManage & _zoneManage)
{
	// On regarde si la zone est deja chargée
	IT_ZoneHeaderMap itZone = m_zoneTableMap.find(_zoneId);
	O3D_ASSERT(itZone != m_zoneTableMap.end());

	PCLODZoneInfo & zoneInfo = itZone->second;
	SPCLODTopZone & zonePtr = zoneInfo.pZone;

    O3D_ASSERT(zonePtr.get() == nullptr);

	// Dans le cas ou la zone est deja chargée, on fait rien.
/*	if (zonePtr && zonePtr->dataLoaded())
	{
		_zoneManage.setZone(zonePtr);
		return;
	}*/

	// Si la zone n'est pas créée, ce qui est probable, on créé l'objet et on charge
	// a partir du disque dur
	if (!zonePtr)
	{
        AutoPtr<InStream> pAutoIs(FileManager::instance()->openInStream(m_headerFilePath));
        InStream * pIs = pAutoIs.get();
        O3D_ASSERT(pIs != nullptr);

        pIs->reset(zoneInfo.zoneFilePosition);
        zonePtr = new PCLODTopZone(*pIs, this);

        zonePtr->onZoneHidden.connect(this, &PCLODZoneManager::rteOnZoneHide, getRefreshThread());
        zonePtr->onZoneVisible.connect(this, &PCLODZoneManager::rteOnZoneVisible, getRefreshThread());
        zonePtr->onZoneUnused.connect(this, &PCLODZoneManager::rteOnZoneUnused, getRefreshThread());

        onMainUpdate.connect(this, &PCLODZoneManager::rteOnMainUpdate, getRefreshThread());

        zonePtr->onRendererRemoved.connect(this, &PCLODZoneManager::rteOnRendererRemoved, getMainThread());
        zonePtr->onRendererUpdated.connect(this, &PCLODZoneManager::rteOnRendererUpdated, getMainThread());
	}

	// Si les données n'ont pas été chargée...
	if (!zonePtr->dataLoaded())
		zonePtr->load();

	_zoneManage.setZone(zonePtr);
}

/* Change the current position of the array 2D */
void PCLODZoneManager::rtMoveCamera(const Vector2i & _direction)
{
	/* This function do not use the object m_zoneTableMap and MUST NOT use it.
	 * Only the function LoadZone is allowed to do it because it's protected by mutex.
	 * Those protections are useful to avoid multiple access to the zone table.
	 * En fait, c'est le seul object sensible puisque les zones ne seront pas supprimées tant
	 * qu'elles sont visibles (cad appartiennent à m_visibleArray). Le risque c'est que l'on demande
	 * le chargement d'une zone au meme moment de sa destruction. On empeche donc les fonctions LoadZone
	 * et RemoveZone de fonctionner en meme temps */
	if ((_direction[X] != 0) || (_direction[Y] != 0))
		m_visibleZoneArray.translate(_direction[X], _direction[Y], PCLODZoneManage());

	// Le doublet suivant est la position sur le tableau 2D global de l'origin du tableau
	// 2D de zone visible. La valeur peut etre négative si le la caméra est sur le bord du terrain.

	// On vérifie que les dimensions du visibleArray sont correctes:
	Vector2i halfSize = Vector2i(Int32(m_visibleZoneArray.width()) / 2, Int32(m_visibleZoneArray.height()) / 2);

	O3D_ASSERT(halfSize[X] * 2 == Int32(m_visibleZoneArray.width() - 1));
	O3D_ASSERT(halfSize[Y] * 2 == Int32(m_visibleZoneArray.height() - 1));

	Vector2i originArray(m_viewArea[X] - _direction[X] - halfSize[X],
							m_viewArea[Y] - _direction[Y] - halfSize[Y]);

	// We must only defined some
	// For the moment, a fast and easy way is implement, but it can be improved
    for (Int32 j = 0 ; j < m_visibleZoneArray.height() ; ++j)
        for (Int32 i = 0 ; i < m_visibleZoneArray.width() ; ++i)
		{
			PCLODZoneManage & pZoneManage = m_visibleZoneArray(i,j);
			Vector2i position(originArray[X] + i, originArray[Y] + j);

			if (!pZoneManage.isValid() &&
                (position[X] >= 0) && (position[X] < m_zoneTableArray.width()) &&
                (position[Y] >= 0) && (position[Y] < m_zoneTableArray.height()))
			{
				UInt32 zoneId = m_zoneTableArray(position[X], position[Y]);

				if (zoneId != 0) // If zoneId = 0, it means there is no zone in this area
				{
					rtLoadZone(zoneId, pZoneManage);
				}
			}
		}
}

void PCLODZoneManager::rtBuildgetRenderer(PCLODZone & _zone)
{
	// Nombre de lod pour la zone actuelle
	UInt8 lodNumber = UInt8(_zone.m_levelArray.size());
	UInt8 lodRequested;
	computeLod(_zone, lodRequested);

	if (lodNumber == 0)
	{
		PCLOD_ERROR(String("ZoneManager: BuildRenderer on a zone which doesnt contain any lod"));
		return;
	}

	UInt8 maxLod = lodNumber - 1;

	// Si on demande un lod trop élevé, on demande aux zones filles.
	if ((lodRequested > maxLod) && _zone.hasChildren())
	{
		// Si ce niveau de zone était deja affiché, il faut aussi libérer le renderer
        if (_zone.getRenderer() != nullptr)
		{
			PCLOD_MESSAGE(String("ZoneManager : Zone ") << _zone.getTopParent()->getId() << " : Zone splitted");

			/* La fonction suivante a pour effet d'envoyer un evement au ZoneManager lui demandant
			 * de supprimer son renderer. Cette fonction va immédiatement couper le lien entre la zone
			 * et son renderer. C'est le RenderManager qui va libérer les ressources allouées. */
			_zone.releaseRenderer();
		}

		for (UInt32 k = 0 ; k < 4 ; ++k)
		{
			O3D_ASSERT(_zone.m_children[k]);
			rtBuildgetRenderer(*_zone.m_children[k]);
		}

		return;
	}
	// Sinon on affiche le mieux que l'on peut. Cad la zone la plus simplifiée.
	else if ((lodRequested > maxLod) && !_zone.hasChildren())
		lodRequested = maxLod;

	// Si le renderer n'existe pas encore, on le créé et on l'ajoute a la liste
    if (_zone.getRenderer() == nullptr)
	{
		// On sait que la zone en question a son renderer inactif, la fonction retourne donc vrai
		// si des zones filles ont leur renderer actif.
		if (_zone.isRendererActive(True))
			PCLOD_MESSAGE(String("ZoneManager : Zone ") << _zone.getTopParent()->getId() << " : Zone merged");

		// Si les zones filles étaient deja affiché, on demande la libération de leur renderer
		// Cette fonction ne va pas supprimer les renderers immédiatement
		_zone.releaseChildrenRenderer();

		// Cette fonction va créer un renderer en envoyant un évenement demandant au zoneManager de
		// l'ajouter au RenderManager
		_zone.createRenderer();
	}

	// On prévient le Renderer qu'il va devoir changer ses données
	_zone.rtUpdateRenderer(lodRequested);
}

/* This function build the list of renderer to add/remove*/
void PCLODZoneManager::computeLod(PCLODZone & _zone, UInt8 & _lod)
{
	// Pour le moment, le niveau est juste calculé suivant la distance de la zone à la camera
	Vector3 zoneCenter = _zone.getWorldCenter();
	//zoneCenter[Y] = m_viewPosition[Y];

	Float distance = (zoneCenter - m_viewPosition).length();

	// C'est le lod le plus élevé qu'on puisse avoir, c'est à dire le plus détaillé.
	// Au niveau du zoneManager, le niveau le plus détaillé sera le niveau 0.
	// C'est plus logique dans cet ordre, mais c'est l'inverse du fichier.
	Int32 maxLod = _zone.getLodNumber(True)-1;

	Int32 currentLod = maxLod;
	currentLod += 1 - Int32(0.05f * distance);

	if (currentLod < 0)
		currentLod = 0;
	else if (currentLod > maxLod)
		currentLod = maxLod;

	_lod = UInt8(currentLod);
}

PCLODZoneManager::PCLODZoneManager(PCLODTerrain * _pTerrain) :
	SceneEntity(_pTerrain),
	m_zoneTableArray(),
	m_zoneTableCenter(),
	m_zoneSize(),
	m_zoneUnits(1.0f, 1.0f),
	m_zoneTableMap(),
	m_worldPosition(0.0f, 0.0f, 0.0f),
	m_visibleZoneArray(),
	m_viewArea(0,0),
	m_viewPosition(0.0f, 0.0f, 0.0f),
	m_viewDistance(0),
	m_viewPixelError(0),
	m_pTerrain(_pTerrain),
    m_pTextureManager(nullptr),
    m_pRenderManager(nullptr),
	m_headerFilePath(),
	m_dataDirectory(),
	m_thread(this),
	m_threadFreq(0),
	m_pauseMutex(),
	m_threadOrder(THREAD_RUN)
{
	m_viewDistance = getTerrain()->getCurrentConfigs().viewDistance();
	m_threadFreq = getTerrain()->getCurrentConfigs().refreshFrequency();
}

PCLODZoneManager::~PCLODZoneManager()
{
	destroy();

	HeightmapIO::destroy();
}

void PCLODZoneManager::destroy()
{
	if (refreshRunning())
		stop();

	// Remove all visible zones
	m_visibleZoneArray.free();

	// On consomme les événements levés par les fonctions précédentes
	// sachant que le thread aura deja été détruit.
	// Certains événements peuvent en générer d'autres. Il faut donc parfois boucler plusieurs fois.
	while (	(EvtManager::instance()->processEvent(this) > 0) ||
			(EvtManager::instance()->processEvent(m_pTextureManager) > 0) ||
			(EvtManager::instance()->processEvent(this, getRefreshThread())) ||
            (EvtManager::instance()->processEvent(m_pTextureManager, getRefreshThread()))) System::waitMs(0);

	deletePtr(m_pTextureManager);
	deletePtr(m_pRenderManager);

	if (EvtManager::instance()->isThreadRegistered(&m_thread))
		EvtManager::instance()->unRegisterThread(&m_thread);

	m_zoneTableMap.clear();

	m_zoneTableArray.free();
	m_zoneTableCenter = Vector2i(0, 0);
	m_zoneSize = Vector2us(0, 0);

	m_worldPosition.set(0.0f, 0.0f, 0.0f);

	m_viewArea = Vector2ui(0, 0);
	m_viewPosition.set(0.0f, 0.0f, 0.0f);

	m_viewDistance = 0;
	m_viewPixelError = 0;

	m_headerFilePath.destroy();
	m_dataDirectory.destroy();

    m_pTerrain = nullptr;
}

void PCLODZoneManager::load(
        InStream &_headerIs,
        const String & _headerFileName,
        const String & _dataDir,
        const String & _matDir,
        const String & _colormapDir)
{
    m_headerFilePath = FileManager::instance()->getFullFileName(_headerFileName);
    String headerFname, headerPath;
    FileManager::getFileNameAndPath(m_headerFilePath, headerFname, headerPath);

	m_dataDirectory = _dataDir;

	// The file must be at the table position
	UInt32 tableSize;
    _headerIs >> tableSize;
    _headerIs >> m_zoneSize[X];
    _headerIs >> m_zoneSize[Y];

	PCLOD_MESSAGE(String("ZoneManager : Readings of the zone table. Size = ") << tableSize);

	UInt32 * pZoneTable = new UInt32[2*tableSize];
    _headerIs.read(pZoneTable, 2*tableSize);

	Vector2us * pZoneExtension = new Vector2us[tableSize];

	// Before we can store all these zone in the 2D array, we must find the origin
	Vector2i tableOrigin;
	Vector2i tableMax;

	for (UInt32 k = 0 ; k < tableSize ; ++k)
	{
		UInt16 * pOrigin = (UInt16*)&pZoneTable[2*k];

		tableOrigin[X] = o3d::min(tableOrigin[X], Int32(pOrigin[0]) - 32767);
		tableOrigin[Y] = o3d::min(tableOrigin[Y], Int32(pOrigin[1]) - 32767);

		// As zones can have different size, we must extract this size
		// In the same time, we check that the header file is correct
		UInt32 zoneInfo[5] = { 0 };
		Char zoneHeader[4] = { 0 };
        _headerIs.reset(pZoneTable[2*k + 1]);
        _headerIs.read(zoneHeader, 4);

		O3D_ASSERT(strncmp(zoneHeader, "ZONE", 4) == 0);
        _headerIs.read(zoneInfo, 5);

		// zoneInfo[3] and [4] are the size of the zone
		pZoneExtension[k] = Vector2us(UInt16(zoneInfo[3]) / (m_zoneSize[X]-1), UInt16(zoneInfo[4]) / (m_zoneSize[Y]-1));

		tableMax[X] = o3d::max(tableMax[X], Int32(pOrigin[0]) - 32767 + pZoneExtension[k][X] - 1);
		tableMax[Y] = o3d::max(tableMax[Y], Int32(pOrigin[1]) - 32767 + pZoneExtension[k][Y] - 1);
	}

	m_zoneTableCenter = - tableOrigin;

	// We can compute the size of the array
	m_zoneTableArray.setSize(tableMax[X] - tableOrigin[X] + 1, tableMax[Y] - tableOrigin[Y] + 1);
	m_zoneTableArray.fill(0);

	for (UInt32 k = 0 ; k < tableSize ; ++k)
	{
		UInt16 * pOrigin = (UInt16*)&pZoneTable[2*k];
		Vector2i position(Int32(pOrigin[0]) - 32767 - m_zoneTableCenter[X], Int32(pOrigin[1]) - 32767 - m_zoneTableCenter[Y]);

		// We store the zone id at the right places
		for (UInt32 j = 0 ; j < pZoneExtension[k][Y] ; ++j)
			for (UInt32 i = 0 ; i < pZoneExtension[k][X] ; ++i)
				m_zoneTableArray(position[X] + i, position[Y] + j) = pZoneTable[2*k];

		// We add the zone in the table
		PCLODZoneInfo newZoneInfos;
        newZoneInfos.pZone				= SPCLODTopZone((PCLODTopZone*)nullptr);
		newZoneInfos.zoneExtension		= pZoneExtension[k];
		newZoneInfos.zoneFilePosition	= pZoneTable[2*k+1];
		newZoneInfos.zoneId				= pZoneTable[2*k];
		newZoneInfos.zonePosition		= position;

		m_zoneTableMap[newZoneInfos.zoneId] = newZoneInfos;

		String logMsg;
		logMsg	<< "Zone : " << k
				<< " Id = " << pZoneTable[2*k]
				<< " File offset = " << pZoneTable[2*k+1]
				<< " Position = " << position[X] << ", " << position[Y]
				<< " Extension = " << pZoneExtension[k][X] << ", " << pZoneExtension[k][Y];

		PCLOD_MESSAGE(logMsg);
	}

	deleteArray(pZoneTable);
	deleteArray(pZoneExtension);

	/* Loading of the texture file */
    String textureFilePath = m_headerFilePath;
	Int32 extPosition;

	if ((extPosition = textureFilePath.find('.')) != -1)
		textureFilePath.truncate(extPosition);

	textureFilePath += String(".tclm");

	m_pTextureManager = new PCLODTextureManager(this);
	m_pTextureManager->load(textureFilePath, _matDir, _colormapDir);

	m_pRenderManager = new PCLODRenderManager(this);

	// La derniere chose est de créer le tableau 2D des zones visibles suivant la distance de vue
	UInt32 sizeX = m_viewDistance / m_zoneSize[X];
	UInt32 sizeY = m_viewDistance / m_zoneSize[Y];

	m_visibleZoneArray.setSize(2*sizeX + 1, 2*sizeY + 1);
}

/* Cette fonction initialise le terrain à partir de la position spécifiée */
void PCLODZoneManager::init(const Vector3 & _position)
{
	PCLOD_MESSAGE("ZoneManager : Initialization");

	m_worldPosition = _position;
	m_pTextureManager->init();

    if (getTerrain()->getCurrentConfigs().asynchRefresh()) {
		EvtManager::instance()->registerThread(&m_thread);
		run();
    } else {
		rtRefreshOnce(True);
    }
}

void PCLODZoneManager::draw(const DrawInfo &drawInfo)
{
    if (!isInit()) {
		return;
    }
	
	m_pTextureManager->mtUpdate();
    m_pRenderManager->draw(drawInfo);

    if (m_pTerrain->getScene()->getDrawObject(Scene::DRAW_QUADTREE)) {
        PrimitiveAccess primitive = m_pTerrain->getScene()->getPrimitiveManager()->access(drawInfo);

		m_pTerrain->getScene()->getContext()->disableDepthTest();
		m_pTerrain->getScene()->getContext()->setCullingMode(CULLING_NONE);
        m_pTerrain->getScene()->getContext()->blending().setFunc(Blending::DISABLED);
		m_pTerrain->getScene()->getContext()->setDrawingMode(Context::DRAWING_WIREFRAME);
        m_pTerrain->getScene()->getContext()->setLineWidth(3.0f);

		// We draw the quadtree attached to the camera
		Vector2ui halfSize = Vector2ui(m_visibleZoneArray.width()/2, m_visibleZoneArray.height()/2);

		Float minPosX = m_worldPosition[X] + Float(Int32(m_viewArea[Y]) - Int32(halfSize[Y])) * Float(m_zoneSize[Y]-1) * m_zoneUnits[Y];
		Float minPosZ = m_worldPosition[Z] + Float(Int32(m_viewArea[X]) - Int32(halfSize[X])) * Float(m_zoneSize[X]-1) * m_zoneUnits[X];

		Float maxPosX = minPosX + Float(m_visibleZoneArray.height() * (m_zoneSize[Y] - 1)) * m_zoneUnits[Y];
		Float maxPosZ = minPosZ + Float(m_visibleZoneArray.width()  * (m_zoneSize[X] - 1)) * m_zoneUnits[X];

		Float posX = minPosX, posZ = minPosZ;

		primitive->setColor(1,0,0);
		primitive->beginDraw(P_LINES);

        for (Int32 k = 0 ; k <= m_visibleZoneArray.height() ; ++k) {
			// Les diagonales
			primitive->addVertex(minPosX, 0.0f, posZ);
			primitive->addVertex(maxPosX, 0.0f, posZ);

			posZ += (m_zoneSize[X]-1) * m_zoneUnits[X];
		}

        for (Int32 k = 0 ; k <= m_visibleZoneArray.width() ; ++k) {
			// Les hauteurs
			primitive->addVertex(posX, 0.0f, minPosZ);
			primitive->addVertex(posX, 0.0f, maxPosZ);

			posX += (m_zoneSize[Y]-1) * m_zoneUnits[Y];
		}

		primitive->endDraw();

		primitive->setColor(1,1,1);
		primitive->beginDraw(P_LINES);

		minPosX = m_worldPosition[X];
		maxPosX = minPosX + Float(m_zoneTableArray.height() * (m_zoneSize[Y]-1)) * m_zoneUnits[Y];
		posX = minPosX;

		minPosZ = m_worldPosition[Z];
		maxPosZ = minPosZ + Float(m_zoneTableArray.width() * (m_zoneSize[X]-1)) * m_zoneUnits[X];
		posZ = minPosZ;

		// On affiche les quadrillages fixes
        for (Int32 k = 0 ; k <= m_zoneTableArray.height() ; ++k) {
			primitive->addVertex(posX, 0.0f, minPosZ);
			primitive->addVertex(posX, 0.0f, maxPosZ);
			posX += Float(m_zoneSize[Y] - 1) * m_zoneUnits[Y];
		}

        for (Int32 k = 0 ; k <= m_zoneTableArray.width() ; ++k) {
			primitive->addVertex(minPosX, 0.0f, posZ);
			primitive->addVertex(maxPosX, 0.0f, posZ);
			posZ += Float(m_zoneSize[X] - 1) * m_zoneUnits[X];
		}

		primitive->endDraw();

		m_pTerrain->getScene()->getContext()->setDefaultDepthTest();
		m_pTerrain->getScene()->getContext()->setDefaultCullingMode();
        m_pTerrain->getScene()->getContext()->setDefaultLineWidth();
        m_pTerrain->getScene()->getContext()->blending().setDefaultFunc();
	}
}

void PCLODZoneManager::mtUpdate()
{
    if (!isInit()) {
		return;
    }

	// Mise à jour de la position de la caméra
    onMainUpdate(getTerrain()->getCamera()->getAbsoluteMatrix().getTranslation());

    if (!getTerrain()->getCurrentConfigs().asynchRefresh()) {
		rtUpdate();
    }

//	m_pTextureManager->MT_Update(); Le manager update seulement avec un draw
    m_pRenderManager->update();
}

/* Update the refresh thread */
void PCLODZoneManager::rtUpdate()
{
    if (!isInit()) {
		return;
    }

	// Pour le moment, la fonction RefreshOnce est dans la thread principal
	rtRefreshOnce();

	m_pTextureManager->rtUpdate();
}

/* Start/Restart the refresh */
void PCLODZoneManager::run()
{
    if (m_thread.isThread()) {
        if (m_threadOrder == THREAD_PAUSE) {
			m_threadOrder = THREAD_RUN;
            m_pauseMutex.unlock();
			return;
		}

		PCLOD_ERROR(String("ZoneManager : Attempt to start the ZoneManager but it's already started"));
		return;
	}

	m_threadOrder = THREAD_RUN;

	m_thread.setPriority(Thread::PRIORITY_NORMAL);
	m_thread.start();
}

/* Stop the refresh thread */
void PCLODZoneManager::stop()
{
    if (!m_thread.isThread()) {
		PCLOD_ERROR(String("ZoneManager : Attempt to stop the refresh but it's not running"));
		return;
	}

	m_threadOrder = THREAD_STOP;

	m_thread.waitFinish();
}

/* Pause the refreshment of the terrain. Available for synchrone and asynchroneous refreshment */
void PCLODZoneManager::pause()
{
    if (!m_pauseMutex.tryLock()) {
		PCLOD_ERROR(String("ZoneManager : Attempt to pause the ZoneManager but it's already paused"));
		return;
    }

	m_threadOrder = THREAD_PAUSE;
}

// Return the id of the zone from its position
Vector2i PCLODZoneManager::getIndexFromWorldPosition(const Vector3 & _worldPosition)
{
	// Position 3D à partir de l'origine du terrain
	Vector3 lRelativePosition(_worldPosition - m_worldPosition);

	lRelativePosition[X] /= m_zoneUnits[Y];
	lRelativePosition[Z] /= m_zoneUnits[X];

	return Vector2i(Int32(lRelativePosition[X] + 0.5f), Int32(lRelativePosition[Y] + 0.5f));
}

// Return the id of the zone from its position
UInt32 PCLODZoneManager::getZoneIdFromPosition(const Vector2i & _worldOrigin)
{
	Vector2i index = _worldOrigin - m_zoneTableCenter;

	if ((index[X] < 0) || (index[Y] < 0))
		return 0;

	if ((index[X] >= Int32(m_zoneTableArray.width())) || (index[Y] >= Int32(m_zoneTableArray.height())))
		return 0;

	return m_zoneTableArray(index[X], index[Y]);
}

/* Events */

/* If we receive this event, it means a zone was just inserted in the visible array */
void PCLODZoneManager::rteOnZoneVisible(SmartPtr<PCLODZone> _zone)
{
    O3D_ASSERT(_zone.get() != nullptr);
    O3D_ASSERT(_zone->getRenderer() == nullptr);
	O3D_ASSERT(_zone->getTopParent()->getCounter(PCLODTopZone::ZONE_VISIBILITY) > 0);

	PCLOD_MESSAGE(String("ZoneManager : Event zoneVisible : Zone ") << _zone->getTopParent()->getId() << " just appeared");

	// Heavy task
	// Load the material texture and colormap (or build it)
	m_pTextureManager->rtLoadZone(_zone->getTopParent());
}

/* Event sent after renderer deletion. When we recieved this event, the zone
 * was already removed from the render manager */
void PCLODZoneManager::rteOnZoneHide(SmartPtr<PCLODZone> _zone)
{
    O3D_ASSERT(_zone.get() != nullptr);
	O3D_ASSERT(_zone->getTopParent()->getCounter(PCLODTopZone::ZONE_VISIBILITY) == 0);
    O3D_ASSERT(_zone->getRenderer() == nullptr);

	PCLOD_MESSAGE(String("ZoneManager : Event zoneHide : Zone ") << _zone->getTopParent()->getId() << " is not more visible");

	// On libère la zone au niveau du TextureManager
	m_pTextureManager->rtReleaseZone(_zone->getTopParent());
}

void PCLODZoneManager::rteOnZoneUnused(SmartPtr<PCLODZone> _zone)
{
    O3D_ASSERT(_zone.get() != nullptr);
	O3D_ASSERT(_zone->getTopParent()->unused());
    O3D_ASSERT(_zone->getRenderer() == nullptr);

	PCLOD_MESSAGE(String("ZoneManager : Event zoneUnused : Zone ") << _zone->getTopParent()->getId() << " is no more used.");

	// On détruit la zone
	rtRemoveZone(_zone->getTopParent()->getId());
}

void PCLODZoneManager::rteOnMainUpdate(Vector3 _newViewPosition)
{
	m_viewPosition = _newViewPosition;
}

/* Event catched in the main thread.
 * On enleve juste le renderer du renderManager */
void PCLODZoneManager::rteOnRendererRemoved(SmartPtr<PCLODZone> _zone, SmartPtr<PCLODZoneRenderer> _renderer)
{
    O3D_ASSERT(_zone.get() != nullptr);
    O3D_ASSERT(_renderer.get() != nullptr);

	if (!m_pRenderManager->findObject(_renderer.get()))
	{
		PCLOD_ERROR("ZoneManager : Event RendererRemoved : Attempt to remove a renderer, but no such renderer is defined in the renderManager");
		return;
	}

	PCLOD_MESSAGE(String("ZoneManager : Event rendererRemoved : Renderer of zone ") << _zone->getTopParent()->getId() << " removed");

	m_pRenderManager->removeObject(_renderer.get());
}

/* Event catched in the main thread.
 * Si c'est le premier update du renderer, ca signifie qu'il n'a jamais été ajouté
 * au RenderManager. On l'ajoute donc. */
void PCLODZoneManager::rteOnRendererUpdated(SmartPtr<PCLODZone> _zone, SmartPtr<PCLODZoneRenderer> _renderer)
{
    O3D_ASSERT(_zone.get() != nullptr);
    O3D_ASSERT(_renderer.get() != nullptr);
    O3D_ASSERT(m_pRenderManager != nullptr);

	// Si le renderer n'a jamais été initialisé, c'est qu'il n'est pas dans le RenderManager.
	// On en est sur puisque qu'on est dans le thread principal
	if (!_renderer->isInit())
	{
		m_pRenderManager->addObject(_renderer.get());

		PCLOD_MESSAGE(String("ZoneManager : Event zoneUpdate : Zone ") << _zone->getTopParent()->getId() << " inserted into the renderManager");
	}
	#ifdef _DEBUG
	else
		O3D_ASSERT(m_pRenderManager->findObject(_renderer.get()));
	#endif

	PCLOD_MESSAGE(String("ZoneManager : Event zoneUpdate : Zone ") << _zone->getTopParent()->getId() << " updated");
}

/* Function used to refresh one time the terrain */
void PCLODZoneManager::rtRefreshOnce(Bool _firstFrame)
{
    O3D_ASSERT(m_pRenderManager != nullptr);

	// On récupère la position actuelle de la caméra
    if (m_pTerrain->getCamera() == nullptr)
	{
		PCLOD_ERROR(String("ZoneManager: Attempt to refresh but the camera is still not defined"));
		return;
	}

	// On calcule sur quel zone du terrain se situe alors la camera
	Vector3 delta = m_viewPosition - m_worldPosition;
	delta[X] /= m_zoneUnits[Y];
	delta[Z] /= m_zoneUnits[X];

	Vector2i newViewArea(Int32(delta[Z] / (m_zoneSize[X]-1)), Int32(delta[X] / (m_zoneSize[Y]-1)));

	// Correction de l'arrondi des nombres négatifs
	if (delta[Z] < 0.0f) newViewArea[X]--;
	if (delta[X] < 0.0f) newViewArea[Y]--;

	// Si la position a changé, on translate le tableau de zones visibles
	// Si des zones sortent de la zone de vue, leur Renderer est ajouté à
	// "m_rendererToDelete".
	// @todo : Ajouter un hysteresis
	if ((newViewArea != m_viewArea) || _firstFrame)
	{
		rtMoveCamera(Vector2i(
			Int32(m_viewArea[X]) - Int32(newViewArea[X]),
			Int32(m_viewArea[Y]) - Int32(newViewArea[Y])));

        onQuadtreeMoved();
	}
	// Note: la translation est opposé au déplacement de la caméra.

	// Si des événements ont été lancé pdt ces étapes et destiné à ce thread, on les traite
	// maintenant. Par exemple, les events VISIBLE doivent initialiser la zone au niveau
	// du texture manager avant que l'on génère des renderers

	// Un peu subtile mais en asynchrone, lors du premier refresh qui a lieu dans le thread principal
	// il faut traiter les événements du thread de refresh
	EvtManager::instance()->processEvent(this);

    for (Int32 k = 0 ; k < m_visibleZoneArray.elt() ; ++k)
	{
		if (m_visibleZoneArray[k].isValid())
			rtBuildgetRenderer(*m_visibleZoneArray[k].getZone().get());
	}

	m_viewArea = newViewArea;
}

/* Static function used with the refresh thread */
Int32 PCLODZoneManager::run(void *)
{
	PCLOD_MESSAGE("ZoneManager : Refresh thread started");
	rtRefreshOnce(True);

	Float timeSpan = (m_threadFreq > 0 ? 1.0f/m_threadFreq : 0.0f);
	UInt32 timeSpanMs = UInt32(1000.0 * timeSpan);

	// On tolère 5% d'écart avant d'afficher un warning
	UInt32 timeSpanMax = UInt32(1050.0 * timeSpan);

	if (timeSpanMs == 0)
		PCLOD_WARNING("ZoneManager : Refresh frequency disabled. Will running at full speed.");

	TimeCounter timeCounter(0.0f);

	while (m_threadOrder != THREAD_STOP)
	{
        if (m_threadOrder == THREAD_PAUSE)
			m_pauseMutex.lock();

		timeCounter.update();

		EvtManager::instance()->processEvent();

		rtUpdate();

		timeCounter.update();

		if (timeSpanMs > 0)
		{
			if (timeCounter.elapsedMs() < timeSpanMs)
                System::waitMs(timeSpanMs - timeCounter.elapsedMs());
			else if (timeCounter.elapsedMs() > timeSpanMax)
				PCLOD_WARNING(String("ZoneManager : Refresh thread can not follow the frequency ") << timeCounter.elapsedMs() << " > " << Int32(1000.0 * timeSpan) << "ms");
		}
	};

	PCLOD_MESSAGE("ZoneManager : Refresh thread ended");

	return 0;
}
