/**
 * @file terrain.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/pclod/terrain.h"

#include "o3d/engine/landscape/pclod/zonemanager.h"
#include "o3d/engine/landscape/pclod/pclodtexturemanager.h"
#include "o3d/engine/landscape/pclod/configs.h"

#include "o3d/core/filemanager.h"
#include "o3d/core/smartpointer.h"
#include <memory>

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(PCLODTerrain, ENGINE_PCLOD_TERRAIN, TerrainBase)

/* Terrain logger */
void PCLODTerrain::log(PCLODLogType _type, const String & _msg) const
{
	FastMutexLocker locker(m_loggerMutex);

    if (m_pCurrentConfigs != nullptr)
	{
		if (!getCurrentConfigs().logEnabled())
			return;

		switch(_type)
		{
		case PCLOD_LOG_MESSAGE:
			if (!getCurrentConfigs().messageShown())
				return;
			break;
		case PCLOD_LOG_WARNING:
			if (!getCurrentConfigs().warningShown())
				return;
			break;
		case PCLOD_LOG_ERROR:
			if (!getCurrentConfigs().errorShown())
				return;
			break;
		default:
			return;
		}
	}

    String head;
    if (_type == PCLOD_LOG_MESSAGE)
        head = "<MSG> " ;
    else if (_type == PCLOD_LOG_WARNING)
        head = "<WRN> ";
    else if (_type == PCLOD_LOG_WARNING)
        head = "<ERR> ";

    m_logger.log((Logger::LogLevel)_type, head + _msg);
}

PCLODTerrain* PCLODTerrain::getTerrain()
{
	return this;
}

const PCLODTerrain* PCLODTerrain::getTerrain() const
{
	return this;
}

void PCLODTerrain::setSky(SkyBase * _pSky)
{
	m_pSky = _pSky;
}

PCLODTerrain::PCLODTerrain(BaseObject *pParent, Camera *pCamera) :
	TerrainBase(pParent, pCamera),
	m_description(),
    m_pZoneManager(nullptr),
    m_pCurrentConfigs(nullptr),
	m_pSky(this),
	m_logger("PCLODTerrain.log"),
	m_loggerMutex()
{
	m_logger.clearLog();
	m_logger.writeHeaderLog();

	m_pCurrentConfigs = new PCLODConfigs((BaseObject*)this);

	m_pCurrentConfigs->setShadersPath("pclodTerrain/lightmaps");
}

PCLODTerrain::~PCLODTerrain()
{
	deletePtr(m_pZoneManager);
	deletePtr(m_pCurrentConfigs);

	m_logger.writeFooterLog();
}

void PCLODTerrain::draw()
{
    if (m_pZoneManager == nullptr)
		PCLOD_WARNING(String("Terrain : Attempt to draw but the terrain is not initialized"));
	else
		m_pZoneManager->draw();

	DrawInfo drawInfo(DrawInfo::AMBIENT_PASS);

	if (m_pSky)
		m_pSky->draw(drawInfo);
}

void PCLODTerrain::update()
{
    if (m_pZoneManager == nullptr) {
		PCLOD_WARNING(String("Terrain : Attempt to update the terrain, but you must initialize it before"));
    } else {
		m_pZoneManager->mtUpdate();
    }

    if (m_pSky) {
        m_pSky->update();
    }
}

void PCLODTerrain::addLight(const LightInfos & _infos)
{
    if (m_pZoneManager == nullptr) {
		PCLOD_WARNING(String("Terrain : Attempt do add a light but the terrain is not initialized"));
    } else {
		m_pZoneManager->getTextureManager()->mtAddLight(_infos);
    }
}

void PCLODTerrain::removeLight(Light * _pLight)
{
    if (m_pZoneManager == nullptr)
		PCLOD_WARNING(String("Terrain : Attempt do remove a light but the terrain is not initialized"));
	else
		m_pZoneManager->getTextureManager()->mtRemoveLight(_pLight);
}

void PCLODTerrain::updateLight(Light * _pLight)
{
    if (m_pZoneManager == nullptr)
		PCLOD_WARNING(String("Terrain : Attempt do update a light but the terrain is not initialized"));
	else
		m_pZoneManager->getTextureManager()->mtUpdateLight(_pLight);
}

/* Load a terrain */
void PCLODTerrain::load(
        const String & _headerFile,
        const String & _dataDir,
        const String & _matDir,
        const String & _colormapDir)
{
    String fullHeaderFile = FileManager::instance()->getFullFileName(_headerFile);
    String headerFname, headerPath;
    File::getFileNameAndPath(fullHeaderFile, headerFname, headerPath);

    O3D_ASSERT(m_pZoneManager == nullptr);

    AutoPtr<InStream> pHeaderIs(FileManager::instance()->openInStream(fullHeaderFile));
    if (pHeaderIs.get() == nullptr)
	{
        PCLOD_ERROR(String("Terrain : Terrain header file <") << _headerFile << String("> not found."));
		return;
	}
	else
        PCLOD_MESSAGE(String("Terrain : Terrain header file <") << _headerFile << "> opened.");

	// We check that this file is a terrain header file
	Char dataFileHeader[9] = { 0 };
    UInt32 byteRead = pHeaderIs->read(dataFileHeader, 8);

	if ((byteRead != 8 * sizeof(Char)) || (strncmp((const Char*)dataFileHeader, (const Char*)"O3DHCLM ", 8) != 0))
	{
        PCLOD_ERROR(String("Terrain : Terrain header file <") << _headerFile << "> invalid.");
		return;
	}

	UInt16 v0, v1;
    *pHeaderIs >> v0;
    *pHeaderIs >> v1;

	PCLOD_MESSAGE(String("Terrain : Terrain header file version : ") << v0 << "." << v1);

	UInt32 headerPosition;
	UInt32 tablePosition;

    *pHeaderIs >> headerPosition;
    *pHeaderIs >> tablePosition;

	/* Header reading */
    pHeaderIs->reset(headerPosition);
    *pHeaderIs >> m_name;
    *pHeaderIs >> m_description;

	PCLOD_MESSAGE(String("Terrain : Terrain name : ") << m_name << ", " << m_description);

    pHeaderIs->reset(tablePosition);

	/* Creation of the zone manager */
	m_pZoneManager = new PCLODZoneManager(this);
    m_pZoneManager->load(*pHeaderIs, fullHeaderFile, _dataDir, _matDir, _colormapDir);
}

/* Initialize the terrain. Must be call after the terrain is loaded
 * _position is the position of the origin of the terrain in the world coordinate */
void PCLODTerrain::init(const Vector3 & _position)
{
    if (m_pZoneManager == nullptr)
		PCLOD_ERROR(String("Terrain : You must load the terrain before the initialization"));
	else
	{
		m_pCurrentConfigs->init();
		m_pZoneManager->init(_position);
	}
}

