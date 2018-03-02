/**
 * @file pclodrenderer.h
 * @brief PCLODTerrain renderer base.
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2006-08-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PCLODRENDERER_H
#define _O3D_PCLODRENDERER_H

#include "o3d/core/base.h"
#include "o3d/core/smartpointer.h"
#include "o3d/core/thread.h"
#include "o3d/engine/vertexbuffer.h"

#include <vector>

namespace o3d {

class PCLODZone;
class PCLODZoneVisibility;
class PCLODRenderManager;
class PCLODMaterial;
class PCLODColormap;
class PCLODLightmap;
class PCLODZoneVisibility;
class PCLODTerrain;
class Scene;
class DrawInfo;
class Shader;

class Vector3;

//---------------------------------------------------------------------------------------
//! @class PCLODRendererBase
//-------------------------------------------------------------------------------------
//! PCLOD terrain renderer base.
//---------------------------------------------------------------------------------------
class O3D_API PCLODRendererBase
{
private:

	/* Members */
	UInt32 m_refCounter;

	mutable FastMutex m_refMutex;

	Bool m_isInit;
	PCLODRenderManager * m_pRenderManager;

	/* Functions */

public:

	/* Constructors */
	explicit PCLODRendererBase();
	virtual ~PCLODRendererBase();

	/* Functions */

	//! @brief Call by the renderer manager before to init it
	virtual Bool isReady() = 0;

	/* Call when the renderer is loaded into the RenderManager */
	virtual void init();

	/* Call each frames */
    virtual void draw(const DrawInfo &drawInfo) = 0;

	/* Called by the RenderManager when the renderer is removed */
	virtual void clean() = 0;

	virtual Bool isVisible();

	/* Used with Front to back rendering */
	virtual Vector3 getPosition() = 0;

	//! Return the terrain instance
	PCLODTerrain* getTerrain();
	const PCLODTerrain* getTerrain() const;

	Scene* getScene();
	const Scene* getScene() const;

	void setRenderManager(PCLODRenderManager * _pManager) { m_pRenderManager = _pManager; }
	PCLODRenderManager * getRenderManager() { return m_pRenderManager; }

	Bool isInit() const { return m_isInit; }

	/* Functions which provide a references counter */
	void useIt();
	void releaseIt();

	Bool noLongerUsed() const;
	UInt32 getReferenceCounter() const;

	inline Bool canRemove() const { return noLongerUsed(); }
	inline Bool isOwned() const { return False; }
	virtual Bool deleteIt()
	{
		delete(this);
		return True;
	}

	/* Renderer counter */
	static UInt32 getRendererCount() { return m_instanceCounter; }

private:

	static UInt32 m_instanceCounter;
};

} // namespace o3d

#endif // _O3D_PCLODRENDERER_H

