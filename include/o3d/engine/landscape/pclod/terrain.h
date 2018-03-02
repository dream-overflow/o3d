/**
 * @file terrain.h
 * @brief Progressive chunked lod terrain object (progressive ROAM2)
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PCLODTERRAIN_H
#define _O3D_PCLODTERRAIN_H

#include "../landscape.h"
#include "../../sky/skybase.h"
#include "o3d/core/thread.h"
#include "o3d/core/logger.h"

namespace o3d {

#define STRING2(x) #x
#define STRING(x) STRING2(x)

// Macro used to put notes in the source code
// Ex: #pragma COMPIL_MSG("mon_message")
#define COMPIL_MSG(X) message(__FILE__ "[" STRING(__LINE__) "] : " X)

class PCLODZoneManager;
class PCLODConfigs;
class Vector3;

#define PCLOD_MESSAGE(X) getTerrain()->log(PCLODTerrain::PCLOD_LOG_MESSAGE, X)
#define PCLOD_WARNING(X) getTerrain()->log(PCLODTerrain::PCLOD_LOG_WARNING, X)
#define PCLOD_ERROR(X)   getTerrain()->log(PCLODTerrain::PCLOD_LOG_ERROR, X)

//---------------------------------------------------------------------------------------
//! @class PCLODTerrain
//-------------------------------------------------------------------------------------
//! Progressive chunked lod terrain object (progressive ROAM2)
//---------------------------------------------------------------------------------------
class O3D_API PCLODTerrain : public TerrainBase
{
public:

	O3D_DECLARE_CLASS(PCLODTerrain)

	//! PCLOD log message types
	enum PCLODLogType
	{
        PCLOD_LOG_MESSAGE = Logger::MESSAGE,
        PCLOD_LOG_WARNING = Logger::WARNING,
        PCLOD_LOG_ERROR = Logger::ERROR
	};

	//! Default constructor
    PCLODTerrain(BaseObject *pParent, Camera *pCamera = nullptr);

	//! Destructor
	virtual ~PCLODTerrain();

	//! Load a terrain
    void load(
            const String & _headerFile,
            const String & _dataDir,
            const String & _matDir,
            const String & _colormapDir);

	//! Initialize the terrain. Must be call after the terrain is loaded
	//! _position is the position of the origin of the terrain in the world coordinate
	void init(const Vector3 & _position);

	//! Terrain logger
	void log(PCLODLogType, const String &) const;

	PCLODTerrain* getTerrain();
	const PCLODTerrain* getTerrain() const;

	//! Init the sky renderer
	void setSky(SkyBase * _pSky);
	SkyBase * GetSky() { return m_pSky.get(); }
	const SkyBase * GetSky() const { return m_pSky.get(); }

	//! Return the current configs
    PCLODConfigs & getCurrentConfigs() { O3D_ASSERT(m_pCurrentConfigs != nullptr); return *m_pCurrentConfigs; }
    const PCLODConfigs & getCurrentConfigs() const { O3D_ASSERT(m_pCurrentConfigs != nullptr); return *m_pCurrentConfigs; }

	//---------------------------------------------------------------------------------------
	// Processing
	//---------------------------------------------------------------------------------------

    virtual void draw(const DrawInfo &drawInfo) override;
    virtual void update() override;

    virtual void addLight(const LightInfos &) override;
    virtual void removeLight(Light *) override;
    virtual void updateLight(Light * = nullptr) override;

protected:

	String m_description;

	PCLODZoneManager * m_pZoneManager;

	PCLODConfigs * m_pCurrentConfigs;

	SmartObject<SkyBase> m_pSky;

    mutable Logger *m_logger;
	mutable FastMutex m_loggerMutex;
};

} // namaspace o3d

#endif // _O3D_PCLODTERRAIN_H
