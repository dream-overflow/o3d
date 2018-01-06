/**
 * @file zonerenderer.cpp
 * @brief Implementation of ZoneRenderer.h
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2004-12-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/core/smartarray.h"
#include "o3d/core/vector2.h"
#include "o3d/geom/aabbox.h"
#include "o3d/engine/landscape/pclod/zonerenderer.h"
#include "o3d/engine/landscape/pclod/rendermanager.h"
#include "o3d/engine/landscape/pclod/terrain.h"

#include "o3d/core/vector3.h"
#include "o3d/geom/frustum.h"
#include "o3d/core/memorydbg.h"

#include "o3d/engine/landscape/pclod/pclodmaterial.h"
#include "o3d/engine/landscape/pclod/colormap.h"
#include "o3d/engine/landscape/pclod/lightmap.h"

#include "o3d/engine/landscape/pclod/configs.h"
#include "o3d/engine/landscape/pclod/object.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/primitive/primitivemanager.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"

using namespace o3d;

static const Int32 A_VERTEX = 0;
static const Int32 U_MODELVIEWPROJECT_MATRIX = 0;
static const Int32 U_AMBIENT = 1;

/*================================================================
					class PCLODZoneRenderer
================================================================*/
PCLODZoneRenderer::PCLODZoneRenderer(PCLODZone * _pZone):
	PCLODRendererBase(),
	m_mutex(),
	m_infos(),
	m_dataVertexBuffer(),
	m_dataIndexBuffer(),
	m_dataAttribBuffer(),
	m_dataCoordBuffer(),
	m_dataRenderingParts(),
    m_pDataColormap(nullptr),
    m_pDataLightmap(nullptr),

    m_glVertexBuffer(nullptr),
    m_glIndexBuffer(nullptr),
    m_glAttribBuffer(nullptr),
    m_glTexCoordBuffer(nullptr),
	m_glRenderingParts(),
    m_pGlColormap(nullptr),
    m_pGlLightmap(nullptr)
{
    O3D_ASSERT(_pZone != nullptr);

	/* Initialisation de la structure d'infos */
	m_infos.zoneSize = _pZone->getZoneSize();
	m_infos.zoneUnits = _pZone->getZoneUnits();
	m_infos.zoneOrigin = _pZone->getWorldOrigin();
	m_infos.zoneCenter = _pZone->getWorldCenter();

	m_infos.topZoneSize = _pZone->getTopParent()->getZoneSize();
	m_infos.topZoneOrigin = _pZone->getTopParent()->getWorldOrigin();

	m_infos.bounding = _pZone->getBoundingBox();
}

PCLODZoneRenderer::~PCLODZoneRenderer()
{
	clean();
}

Bool PCLODZoneRenderer::isVisible()
{
	return True;
}

Bool PCLODZoneRenderer::isReady()
{
	return (isInit() || (m_pDataLightmap->generated() && m_pDataColormap->loaded()));
}

/* Function called before the first call to draw() in the main thread.
 * Used for opengl initialisation */
void PCLODZoneRenderer::init()
{
    FastMutexLocker locker(m_mutex);

	// On effectue certain test sur la validite des donnees

	UInt32 vertexNumber = UInt32(m_dataVertexBuffer.getNumElt()/3);
	O3D_ASSERT(vertexNumber * 3 == m_dataVertexBuffer.getNumElt());

	Bool ok = True;

	if (m_dataVertexBuffer.getNumElt() == 0)
	{
		PCLOD_ERROR("Renderer : The vertex buffer is empty");
		ok = False;
	}

	if (m_dataIndexBuffer.getNumElt() == 0)
	{
		PCLOD_ERROR("Renderer : The index buffer is empty");
		ok = False;
	}

	if (m_dataAttribBuffer.getNumElt() < vertexNumber)
	{
		PCLOD_ERROR(String("Renderer : The size of the attrib buffer (") << m_dataAttribBuffer.getNumElt() << ") is not compatible with "
							"the size of the vertex buffer " << m_dataVertexBuffer.getNumElt());
		ok = False;
	}

	if (m_dataRenderingParts.size() == 0)
	{
		PCLOD_ERROR(String("Renderer : There are no rendering parts defined in this renderer"));
		ok = False;
	}

	/*
	if (m_dataCoordBuffer.getNbElt() < 2*vertexNumber)
	{
		PCLOD_ERROR(O3DString("Renderer : The size of the tex coord buffer (") << m_dataCoordBuffer.getNbElt() << ") is not compatible " <<
							"with the size of the vertex buffer " << m_dataVertexBuffer.getNbElt());

		ok = False;
	}*/

	if (!ok)
	{
		destroyData();

		return;
	}

	// On charge les donnees de la ram vers la memoire video
	unsafeUpdate();

	PCLODRendererBase::init();
}

void PCLODZoneRenderer::draw()
{
	update();

	if (getTerrain()->getCurrentConfigs().frustumCulling() &&
		(getScene()->getFrustum()->boxInFrustum(m_infos.bounding) != Geometry::CLIP_OUTSIDE))
	{
		if (!getTerrain()->getCurrentConfigs().wireFrame())
			drawWithShaders();
		else
			drawWireFrame();

		if (getScene()->getDrawObject(Scene::DRAW_BOUNDING_VOLUME))
			drawBounding();
	}
}

/* Used to release the zone instance and avoid circular SmartPtr reference */
void PCLODZoneRenderer::clean()
{
    FastMutexLocker locker(m_mutex);

	// On detruit les donnees en RAM
	destroyData();

	// On detruit les donnees en memoire video
	deletePtr(m_glVertexBuffer);
	deletePtr(m_glIndexBuffer);
	deletePtr(m_glAttribBuffer);
	deletePtr(m_glTexCoordBuffer);

	// On decremente le compteur de ref de chaque materiau
	unsafeFreeGLRenderingParts();

	m_glRenderingParts.clear();

	if (m_pGlColormap)
	{
		m_pGlColormap->releaseIt();
        m_pGlColormap = nullptr;
	}

	if (m_pGlLightmap)
	{
		m_pGlLightmap->releaseIt();
        m_pGlLightmap = nullptr;
	}
}

Vector3 PCLODZoneRenderer::getPosition()
{
	return m_infos.zoneCenter;
}

/* This function allow us to set the data which will be send to openGL */
void PCLODZoneRenderer::setData(	const SmartArrayFloat & _vertexBuffer,
									const SmartArrayUInt32 & _indexBuffer,
									const SmartArrayFloat & _attribBuffer,
									const SmartArrayFloat & _coordBuffer,
									const T_RenderingBlockInfoArray & _renderingBlock,
									PCLODColormap * _colormap,
									PCLODLightmap * _lightmap)
{
	// Pour chaque buffer, si il est non nul, on le copie
	// Par ailleurs, la taille de chaque buffer doit respecter certaines conditions.

    FastMutexLocker locker(m_mutex);

    if (_vertexBuffer.getData() != nullptr)
	{
		O3D_ASSERT((_vertexBuffer.getNumElt() / 3) * 3 == _vertexBuffer.getNumElt());

		m_dataVertexBuffer = _vertexBuffer;
	}

    if (_indexBuffer.getData() != nullptr)
		m_dataIndexBuffer = _indexBuffer;

    if (_attribBuffer.getData() != nullptr)
		m_dataAttribBuffer = _attribBuffer;

    if (_coordBuffer.getData() != nullptr)
		m_dataCoordBuffer = _coordBuffer;

	if (_renderingBlock.size() > 0)
	{
		// Si la zone possedait deja des listes de rendus, on libere les materiaux
		unsafeFreeRamRenderingParts();

		m_dataRenderingParts.resize(_renderingBlock.size());
		O3D_ASSERT(m_dataRenderingParts.size() == _renderingBlock.size());

		IT_RenderingBlockInfoArray itTarget = m_dataRenderingParts.begin();

		for(CIT_RenderingBlockInfoArray	it = _renderingBlock.begin() ;
											it != _renderingBlock.end() ;
											it++, itTarget++)
		{
			memcpy((void*)&(*itTarget)/*itTarget.operator ->()*/, (const void*)&(*it)/*it.operator ->()*/, sizeof(PCLODRenderingBlock));

			for (UInt32 k = 0 ; k < it->matNb ; ++k)
			{
                O3D_ASSERT(it->matPtr[k] != nullptr);
				it->matPtr[k]->useIt();
			}

			// J'ai un bug etrange parfois: m_dataRenderingParts.size() != _renderingBlock.size()
			// et ce malgre le resize(...).
			O3D_ASSERT(m_dataRenderingParts.size() == _renderingBlock.size());
		}
	}

	if (_colormap)
	{
		if (m_pDataColormap)
			m_pDataColormap->releaseIt();

		m_pDataColormap = _colormap;
		m_pDataColormap->useIt();
	}

	if (_lightmap)
	{
		if (m_pDataLightmap)
			m_pDataLightmap->releaseIt();

		m_pDataLightmap = _lightmap;
		m_pDataLightmap->useIt();
	}
}

/* Update the GL datas if there are waiting datas */
void PCLODZoneRenderer::update()
{
    FastMutexLocker locker(m_mutex);

	if (!needUpdate())
		return;

	unsafeUpdate();
}

/* Return true if there are waiting datas */
Bool PCLODZoneRenderer::needUpdate() const
{
	// Pour le moment, on se contente de ce test
	return (m_dataVertexBuffer.getNumElt() > 0);
}

/* This function clear the temporary data */
void PCLODZoneRenderer::destroyData()
{
	// On nettoye tout et on quitte
	m_dataVertexBuffer.releaseCheckAndDelete();
	m_dataIndexBuffer.releaseCheckAndDelete();
	m_dataAttribBuffer.releaseCheckAndDelete();
	m_dataCoordBuffer.releaseCheckAndDelete();

	unsafeFreeRamRenderingParts();

    if (m_pDataColormap != nullptr)
	{
		O3D_ASSERT(m_pDataColormap->getReferenceCounter() > 1);
		m_pDataColormap->releaseIt();
        m_pDataColormap = nullptr;
	}

    if (m_pDataLightmap != nullptr)
	{
		O3D_ASSERT(m_pDataLightmap->getReferenceCounter() > 1);
		m_pDataLightmap->releaseIt();
        m_pDataLightmap = nullptr;
	}
}

/* Unsafe Update */
void PCLODZoneRenderer::unsafeUpdate()
{
	if (m_dataVertexBuffer.getNumElt() > 0)
	{
		if (!m_glVertexBuffer)
            m_glVertexBuffer = new ArrayBufferf(getScene()->getContext());

		m_glVertexBuffer->create(m_dataVertexBuffer.getNumElt(), VertexBuffer::STATIC, m_dataVertexBuffer.getData());
	}

	if (m_dataIndexBuffer.getNumElt() > 0)
	{
		if (!m_glIndexBuffer)
            m_glIndexBuffer = new ElementArrayBufferui(getScene()->getContext());

		m_glIndexBuffer->create(m_dataIndexBuffer.getNumElt(), VertexBuffer::STATIC, m_dataIndexBuffer.getData());
	}

	if (m_dataAttribBuffer.getNumElt() > 0)
	{
		if (!m_glAttribBuffer)
            m_glAttribBuffer = new ArrayBufferf(getScene()->getContext());

		m_glAttribBuffer->create(m_dataAttribBuffer.getNumElt(), VertexBuffer::STATIC, m_dataAttribBuffer.getData());
	}

	/*
	if (m_dataCoordBuffer.getNbElt() > 0)
	{
		if (!m_glTexCoordBuffer)
			m_glTexCoordBuffer = new O3DVertexBufferObjf(m_pRenderManager);

		m_glTexCoordBuffer.Create(m_dataCoordBuffer.GetNbElt(), O3DVertexBufferType::Static, m_dataCoordBuffer.getData());
	}
	*/

	if (m_dataRenderingParts.size() > 0)
	{
		// On supprime proprement les matériaux actuellement possédées
		unsafeFreeGLRenderingParts();

		m_glRenderingParts = m_dataRenderingParts;

		// On prend les instances de chaque nouveau matériau
		for(IT_RenderingBlockInfoArray	it = m_glRenderingParts.begin() ;
											it != m_glRenderingParts.end() ;
											it++)
		{
			for (UInt32 k = 0 ; k < it->matNb ; ++k)
			{
				it->matPtr[k]->useIt();
				it->matPtr[k]->loadToGL();
			}
		}
	}

    if (m_pDataColormap != nullptr)
	{
		if (m_pGlColormap)
			m_pGlColormap->releaseIt();

		m_pGlColormap = m_pDataColormap;
		m_pGlColormap->useIt();
		m_pGlColormap->loadToGL();
	}

    if (m_pDataLightmap != nullptr)
	{
		if (m_pGlLightmap)
			m_pGlLightmap->releaseIt();

		m_pGlLightmap = m_pDataLightmap;
		m_pGlLightmap->useIt();
	}

	destroyData();
}

/* Unsafe : Free the GL render list */
void PCLODZoneRenderer::unsafeFreeGLRenderingParts()
{
	if (m_glRenderingParts.size() == 0)
		return;

	// On supprime proprement les matériaux actuellement possédées
	for(IT_RenderingBlockInfoArray	it = m_glRenderingParts.begin() ;
										it != m_glRenderingParts.end() ;
										it++)
	{
		for (UInt32 k = 0 ; k < it->matNb ; ++k)
		{
			it->matPtr[k]->releaseIt();
            it->matPtr[k] = nullptr;
		}
	}

	m_glRenderingParts.clear();
}

/* Unsafe : Free the RAM render list */
void PCLODZoneRenderer::unsafeFreeRamRenderingParts()
{
	if (m_dataRenderingParts.size() == 0)
		return;

	for(IT_RenderingBlockInfoArray	it = m_dataRenderingParts.begin() ;
										it != m_dataRenderingParts.end() ;
										it++)
	{
		for (UInt32 k = 0 ; k < it->matNb ; ++k)
		{
			it->matPtr[k]->releaseIt();
            it->matPtr[k] = nullptr;
		}
	}

	m_dataRenderingParts.clear();
}

// Function which used shader to render this zone, need material buffer to be defined
void PCLODZoneRenderer::drawWithShaders()
{
	PCLODConfigs & currentConfig = getTerrain()->getCurrentConfigs();

	if (!currentConfig.shadersInit())
		return;

	Context *glContext = getScene()->getContext();

    glContext->blending().setFunc(Blending::DISABLED);
	glContext->enableDepthTest();
	glContext->setCullingMode(CULLING_FRONT_FACE);

	m_glIndexBuffer->bindBuffer();
	/* Fin */

	Vector4 zoneConfigs(	(m_infos.zoneSize[X]-1) * m_infos.zoneUnits[X],
							(m_infos.zoneSize[Y]-1) * m_infos.zoneUnits[Y],
							m_infos.zoneUnits[X],
							m_infos.zoneUnits[Y]);
	zoneConfigs[0] = 1.0f/zoneConfigs[0];
	zoneConfigs[1] = 1.0f/zoneConfigs[1];
	zoneConfigs[2] = 1.0f/zoneConfigs[2];
	zoneConfigs[3] = 1.0f/zoneConfigs[3];

	Vector4 topZonePosition(	(m_infos.topZoneSize[X]-1) * m_infos.zoneUnits[X],
								(m_infos.topZoneSize[Y]-1) * m_infos.zoneUnits[Y],
								m_infos.topZoneOrigin[Z],
								m_infos.topZoneOrigin[X]);
	topZonePosition[X] = 1.0f/topZonePosition[X];
	topZonePosition[Y] = 1.0f/topZonePosition[Y];

	Vector4 distanceConfigs(currentConfig.getDistanceOnlyMaterial(), currentConfig.getDistanceOnlyColormap(), 0.0f, 0.0f);
	Vector3 viewPosition = getTerrain()->getCamera()->getAbsoluteMatrix().getTranslation();

	ShaderInstance lShaderInstance1;
	ShaderInstance lShaderInstance2;
	currentConfig.getShader(0, lShaderInstance1);
	currentConfig.getShader(1, lShaderInstance2);

	Int32 materialAttribLocation = 0;
	Int32 vertexAttribLocation = 0;

	for (IT_RenderingBlockInfoArray itPart = m_glRenderingParts.begin() ; itPart != m_glRenderingParts.end() ; itPart++)
	{
		PCLODRenderingBlock & currentBlock = *itPart;

		// Initialization
		switch (currentBlock.matNb)
		{
		case 1:
			{
				lShaderInstance1.bindShader();

				// transform matrix
				lShaderInstance1.setConstMatrix4(
						"u_modelViewProjectionMatrix",
						False,
						glContext->modelViewProjection());

				// vertices attribute
				vertexAttribLocation = lShaderInstance1.getAttributeLocation("a_vertex");
				m_glVertexBuffer->attribute(vertexAttribLocation, 3, 0, 0);

				lShaderInstance1.setConstTexture("O3D_TexColormap",	m_pGlColormap->getTexture(), 0);
				lShaderInstance1.setConstTexture("O3D_Material", currentBlock.matPtr[0]->getTexture(), 1);
				lShaderInstance1.setConstTexture("O3D_Lightmap", m_pGlLightmap->getTexture(), 2);

				lShaderInstance1.setConstVector4("O3D_DistanceConfigs", distanceConfigs);
				lShaderInstance1.setConstVector4("O3D_ZoneConfigs", zoneConfigs);
				lShaderInstance1.setConstVector4("O3D_TopZonePosition", topZonePosition);
				lShaderInstance1.setConstVector3("O3D_ViewPosition", viewPosition);
			}
			break;
		case 2:
			{
				lShaderInstance2.bindShader();

				// transform matrix
				lShaderInstance2.setConstMatrix4(
						"u_modelViewProjectionMatrix",
						False,
						glContext->modelViewProjection());

				// vertices attribute
				vertexAttribLocation = lShaderInstance2.getAttributeLocation("a_vertex");
				m_glVertexBuffer->attribute(vertexAttribLocation, 3, 0, 0);

				// material attribute
				materialAttribLocation = lShaderInstance2.getAttributeLocation("O3D_MaterialId");
				m_glAttribBuffer->attribute(materialAttribLocation, 1, 0, 0);

				lShaderInstance2.setConstTexture("O3D_TexColormap",	m_pGlColormap->getTexture(), 0);
				lShaderInstance2.setConstTexture("O3D_Material0", currentBlock.matPtr[0]->getTexture(), 1);
				lShaderInstance2.setConstTexture("O3D_Material1", currentBlock.matPtr[1]->getTexture(), 2);
				lShaderInstance2.setConstTexture("O3D_Lightmap", m_pGlLightmap->getTexture(), 3);
	
				lShaderInstance2.setConstVector2(
						"O3D_MaterialTable",
						Vector2f(Float(currentBlock.matPtr[0]->getMatId()), Float(currentBlock.matPtr[1]->getMatId())));

				lShaderInstance2.setConstVector4("O3D_DistanceConfigs", distanceConfigs);
				lShaderInstance2.setConstVector4("O3D_ZoneConfigs", zoneConfigs);
				lShaderInstance2.setConstVector4("O3D_TopZonePosition", topZonePosition);
				lShaderInstance2.setConstVector3("O3D_ViewPosition", viewPosition);
			}
			break;
		default:
			continue;
		}

		// Rendu opengl
		getTerrain()->getScene()->drawElementsUInt32(
				P_TRIANGLES,
				currentBlock.indexEnd - currentBlock.indexStart,
				reinterpret_cast<UInt32*>(currentBlock.indexStart*sizeof(UInt32)));

		// Post clean
		switch(currentBlock.matNb)
		{
		case 1:
			glContext->disableVertexAttribArray(vertexAttribLocation);
			lShaderInstance1.unbindShader();
			break;
		case 2:
			glContext->disableVertexAttribArray(vertexAttribLocation);
			glContext->disableVertexAttribArray(materialAttribLocation);
			lShaderInstance2.unbindShader();
			break;
		}
	}

	glContext->setActiveTextureUnit(2);
	glContext->bindTexture(TEXTURE_2D, 0);

	glContext->setActiveTextureUnit(1);
	glContext->bindTexture(TEXTURE_2D, 0);

	glContext->setActiveTextureUnit(0);
	glContext->bindTexture(TEXTURE_2D, 0);
}

/* Function which used wireframe mode to render the zone */
void PCLODZoneRenderer::drawWireFrame()
{
	PCLODConfigs & currentConfig = getTerrain()->getCurrentConfigs();

	if (!currentConfig.shadersInit())
		return;

	Context *glContext = getScene()->getContext();

	ShaderInstance lShaderInstance;
	currentConfig.getSimpleShader(lShaderInstance);

/*	m_ambientColor.setUniform(U_MODELVIEWPROJECT_MATRIX, m_ambientColor.getUniformLocation("u_modelviewProjectionMatrix"));
	m_ambientColor.setUniform(U_AMBIENT, m_ambientColor.getUniformLocation("u_ambient"));

	m_ambientColor.setAttribute(A_VERTEX, m_ambientColor.getAttributeLocation("a_vertex"));
	*/
	glContext->setCullingMode(CULLING_FRONT_FACE);
    glContext->blending().setFunc(Blending::DISABLED);
	glContext->setDrawingMode(Context::DRAWING_WIREFRAME);

	lShaderInstance.bindShader();

	lShaderInstance.setConstColor("u_ambient", Color(0.0f, 0.0f, 0.0f, 1.0f));
	lShaderInstance.setConstMatrix4("u_modelViewProjectionMatrix", False, glContext->modelViewProjection());

	m_glVertexBuffer->attribute(V_VERTICES_ARRAY, 3, 0, 0);

	m_glIndexBuffer->bindBuffer();
    getScene()->drawElementsUInt32(P_TRIANGLES, m_glIndexBuffer->getCount(), nullptr);

	glContext->disableVertexAttribArray(V_VERTICES_ARRAY);

	lShaderInstance.unbindShader();

	glContext->setDefaultCullingMode();
	glContext->setDrawingMode(Context::DRAWING_FILLED);
}

void PCLODZoneRenderer::drawBounding()
{
    getScene()->getContext()->setLineWidth(2.0f);

	getScene()->getPrimitiveManager()->bind();
	getScene()->getPrimitiveManager()->boundingBox(
			m_infos.bounding,
			Color(0.0f, 0.0f, 1.0f));
	getScene()->getPrimitiveManager()->unbind();

    getScene()->getContext()->setDefaultLineWidth();
}
