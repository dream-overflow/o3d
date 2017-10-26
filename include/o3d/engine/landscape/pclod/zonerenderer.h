/**
 * @file zonerenderer.h
 * @brief Renderer of terrain zones
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2008-04-10
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PCLODZONERENDERER_H
#define _O3D_PCLODZONERENDERER_H

#include "pclodrenderer.h"
#include "o3d/core/vector2.h"
#include "../../shader/shader.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class PCLODZoneRenderer
//-------------------------------------------------------------------------------------
//! Definition
//---------------------------------------------------------------------------------------
class O3D_API PCLODZoneRenderer : public PCLODRendererBase
{
public:

	/* Structure only used to send the data to this renderer */
	struct PCLODRenderingBlock
	{
		UInt32 indexStart;
		UInt32 indexEnd;

		UInt32 matNb;
		PCLODMaterial * matPtr[3];
	};

	typedef std::vector<PCLODRenderingBlock> T_RenderingBlockInfoArray;
	typedef T_RenderingBlockInfoArray::iterator IT_RenderingBlockInfoArray;
	typedef T_RenderingBlockInfoArray::const_iterator CIT_RenderingBlockInfoArray;

private:

	/* Type */

	/* Contains some informations extracted from the zone */
	struct PCLODZoneInfo
	{
		Vector2ui	zoneSize;
		Vector2f	zoneUnits;
		Vector3		zoneOrigin;
		Vector3		zoneCenter;

		Vector2ui	topZoneSize;
		Vector3		topZoneOrigin;

		AABBox	    bounding;
	};

	/* Members */

	/* This function is used to avoid multiple definition of m_dataVertexBuffer.
	 * It could happen if the refresh thread is too fast. This mutex is locked in
	 * the function Init and released when it returns */
	FastMutex m_mutex;

	/* Infos about the associated zone */
	PCLODZoneInfo m_infos;

	/* Pre-load */

	/* These buffers are created by a thread and sent to openGL with
	 * the function init() */
	SmartArrayFloat m_dataVertexBuffer;
	SmartArrayUInt32 m_dataIndexBuffer;
	SmartArrayFloat m_dataAttribBuffer;
	SmartArrayFloat m_dataCoordBuffer;

	T_RenderingBlockInfoArray m_dataRenderingParts;

	PCLODColormap * m_pDataColormap;

	PCLODLightmap * m_pDataLightmap;

	/* OpenGL members */
	VertexBufferObjf *m_glVertexBuffer;
	ElementBufferObjui *m_glIndexBuffer;

	VertexBufferObjf *m_glAttribBuffer;
	VertexBufferObjf *m_glTexCoordBuffer;

	T_RenderingBlockInfoArray m_glRenderingParts;

	PCLODColormap * m_pGlColormap;

	PCLODLightmap * m_pGlLightmap;

	ShaderInstance m_ambientColor;

	// Functions

	//! Function which used shader to render this zone, need material buffer to be defined
	void drawWithShaders();

	//! Function which used wire-frame mode to render the zone
	void drawWireFrame();

	//! Draw the bounding symbolic
	void drawBounding();

	//! This function clear the temporary data
	void destroyData();

	//! Unsafe Update
	void unsafeUpdate();

	//! Unsafe : Free the GL render list
	void unsafeFreeGLRenderingParts();

	//! Unsafe : Free the RAM render list
	void unsafeFreeRamRenderingParts();

public:

	//! Default cConstructor.
	PCLODZoneRenderer(PCLODZone *);

	//! Virtual destructor.
	virtual ~PCLODZoneRenderer();

	virtual Bool isVisible();

	//! @brief Function called before initialization.
	//! It allows the render manager to know if all resources are available.
	virtual Bool isReady();

	// Function called before the first call to draw() in the main thread.
	// Used for GL initialization
	virtual void init();

	virtual void draw();

	// Used to release the zone instance and avoid circular SmartPtr reference
	virtual void clean();

	virtual Vector3 getPosition();

	/* Functions */

	// This function allow us to set the data which will be send to openGL
	void setData(
			const SmartArrayFloat & _vertexBuffer,
			const SmartArrayUInt32 & _indexBuffer,
			const SmartArrayFloat & _attribBuffer,
			const SmartArrayFloat & _coordBuffer,
			const T_RenderingBlockInfoArray & _renderingBlock,
			PCLODColormap * _colormap,
			PCLODLightmap * _lightmap);

	// Update the GL data if there are waiting data
	void update();

	// Return true if there are waiting data
	Bool needUpdate() const;
};

} // namespace o3d

#endif // _O3D_PCLODZONERENDERER_H

