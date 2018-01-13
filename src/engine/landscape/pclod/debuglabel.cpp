/**
 * @file debuglabel.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/pclod/debuglabel.h"

#include "o3d/engine/landscape/pclod/colormap.h"
#include "o3d/engine/landscape/pclod/lightmap.h"

#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/framebuffer.h"
#include "o3d/engine/text2d.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/shader/shadermanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"

using namespace o3d;

static const Int32 U_MODELVIEWPROJECT_MATRIX = 0;
static const Int32 U_AMBIENT = 1;
static const Int32 U_AMBIENT_MAP = 2;

//! Default constructor
PCLODDebugLabel::PCLODDebugLabel(BaseObject * _pParent):
	WorldLabel(_pParent),
    m_pTexture(nullptr),
	m_titleText(),
    m_lightmapText(""),
    m_colormapText(""),
	m_lightCount(0),
	m_position(),
    m_drawVerticesVbo(getScene()->getContext()),
    m_drawTexCoordsVbo(getScene()->getContext())
{
}

//! A destructor
PCLODDebugLabel::~PCLODDebugLabel()
{
	deletePtr(m_pTexture);
}

void PCLODDebugLabel::initShaders()
{
	Shader *shader = getScene()->getShaderManager()->addShader("ambient");
	
	if (shader)
		shader->buildInstance(m_ambientColor);

    m_ambientColor.assign("default", "default", "MESH");
	m_ambientColor.bindShader();

	m_ambientColor.setUniform(U_MODELVIEWPROJECT_MATRIX, m_ambientColor.getUniformLocation("u_modelViewProjectionMatrix"));
	m_ambientColor.setUniform(U_AMBIENT, m_ambientColor.getUniformLocation("u_ambient"));

	m_ambientColor.unbindShader();
	
	if (shader)
		shader->buildInstance(m_ambientMap);

    m_ambientMap.assign("default", "default", "MESH;AMBIENT_MAP");
	m_ambientMap.bindShader();

	m_ambientMap.setUniform(U_MODELVIEWPROJECT_MATRIX, m_ambientMap.getUniformLocation("u_modelViewProjectionMatrix"));
	m_ambientMap.setUniform(U_AMBIENT, m_ambientMap.getUniformLocation("u_ambient"));
	m_ambientMap.setUniform(U_AMBIENT_MAP, m_ambientMap.getUniformLocation("u_ambientMap"));

	m_ambientMap.unbindShader();	
}

//! Update the content of the label
//! The font and the FBO are shared among all debug label and provided by the texture
//! manager.
void PCLODDebugLabel::mtUpdateContent(
        Text2D * _pText,
        FrameBuffer * _pFbo,
        PCLODColormap * _pColormap,
        PCLODLightmap * _pLightmap)
{
	if (!m_ambientColor.isOperational())
		initShaders();

	String lTitleText("Zone : "), lLightmapText("Lightmap : "), lColormapText("Colormap : ");
	Int32 lLightCount = 0;
	
    if (_pLightmap != nullptr)
	{
		if (_pLightmap->generated())
			lLightmapText << "(" << _pLightmap->getTexture()->getWidth() << ":" << _pLightmap->getTexture()->getHeight() << ")";
		else
			lLightmapText << "NOT LOADED";

        if (_pLightmap->getZone() != nullptr)
			lTitleText << _pLightmap->getZoneId();

		lLightCount = _pLightmap->getLightCount();
	}
	else
        lLightmapText << "nullptr";

    if (_pColormap != nullptr)
	{
		if (_pColormap->loaded())
			lColormapText << "(" << _pColormap->getTexture()->getWidth() << ":" << _pColormap->getTexture()->getHeight() << ")";
		else
			lColormapText << "NOT LOADED";

        if ((_pLightmap == nullptr) || (_pLightmap->getZone() == nullptr))
			lTitleText << _pColormap->getZoneId();
	}
	else
        lColormapText << "nullptr";
	
	if ((lLightmapText != m_lightmapText) ||
		(lColormapText != m_colormapText) ||
		(lTitleText != m_titleText) ||
		(lLightCount != m_lightCount))
	{
		m_lightmapText = lLightmapText;
		m_colormapText = lColormapText;
		m_titleText = lTitleText;
		m_lightCount = lLightCount;

        if (m_pTexture == nullptr)
		{
			m_pTexture = new Texture2D(this);
			m_pTexture->setFiltering(Texture::NO_FILTERING);
			m_pTexture->setPersistant(True);
		}

		if ((Int32(m_pTexture->getWidth()) != _pFbo->getDimension()[X]) ||
			(Int32(m_pTexture->getHeight()) != _pFbo->getDimension()[Y]))
		{
			m_pTexture->create(False, _pFbo->getDimension()[X], _pFbo->getDimension()[Y], PF_RGB_U8);
			setScreenSize(_pFbo->getDimension()[X], _pFbo->getDimension()[Y]);
		}

		Context *glContext = getScene()->getContext();

        Context::AntiAliasingMethod aa = glContext->setAntiAliasing(Context::AA_NONE);

		Int32 lOldViewPort[4];
		glContext->getViewPort(lOldViewPort);
		glContext->setViewPort(0,0,m_pTexture->getWidth(), m_pTexture->getHeight());
		
		_pFbo->bindBuffer();
        _pFbo->attachTexture2D(m_pTexture, FrameBuffer::COLOR_ATTACHMENT0);
		O3D_ASSERT(_pFbo->isCompleteness());
		
		Matrix4 lProjection, lModelView;
		lModelView = glContext->modelView().get();
		lProjection = glContext->projection().get();

		Matrix4 lLocalProjection;
		lLocalProjection.identity();
		lLocalProjection.setData(0, 0, 2.0f / m_pTexture->getWidth());
		lLocalProjection.setData(1, 1, -2.0f / m_pTexture->getHeight());

		glContext->bindTexture(TEXTURE_2D, 0, True);
		glContext->disableDepthTest();
		
		Color lOldBgColor = glContext->getBackgroundColor();
		glContext->setBackgroundColor(1.0f, 1.0f, 1.0f, 1.0f);

		glContext->modelView().identity();
		glContext->projection().set(lLocalProjection);

		_pFbo->clearColor();

		const Float lHalfWidth = 0.5f*(m_pTexture->getWidth()-1);
		const Float lHalfHeight = 0.5f*(m_pTexture->getHeight()-1);
		
		m_ambientColor.bindShader();

		m_ambientColor.setConstColor(
			m_ambientColor.getUniform(U_AMBIENT),
			Color(0.0f, 0.0f, 0.0f, 1.0f));

		m_ambientColor.setConstMatrix4(
			m_ambientColor.getUniform(U_MODELVIEWPROJECT_MATRIX),
			False,
			glContext->modelViewProjection());

		const Float lLineLoopCoords[] = { 
			    -lHalfWidth, +lHalfHeight,
			lHalfWidth-1.5f, +lHalfHeight,
			lHalfWidth-1.5f, -lHalfHeight+0.5f,
			    -lHalfWidth, -lHalfHeight+0.5f };
		
        ArrayBufferf updateLinesVerticesVbo(getScene()->getContext());

		updateLinesVerticesVbo.create(8, VertexBuffer::STATIC, lLineLoopCoords, True);
		updateLinesVerticesVbo.attribute(V_VERTICES_ARRAY, 2, 0, 0);

		getScene()->drawArrays(P_LINE_LOOP, 0, 4);
		
		const Int32 lBorder = 2;
		
		const Float lLinesCoords[] = {
                -lHalfWidth, -lHalfHeight+0.5f + _pText->getTextHeight() + 2*lBorder,
            lHalfWidth-1.5f, -lHalfHeight+0.5f + _pText->getTextHeight() + 2*lBorder };
		
        ArrayBufferf updateLineLoopVerticesVbo(getScene()->getContext());

		updateLineLoopVerticesVbo.create(4, VertexBuffer::STATIC, lLinesCoords, True);
		updateLineLoopVerticesVbo.attribute(V_VERTICES_ARRAY, 2, 0, 0);

		getScene()->drawArrays(P_LINES, 0, 2);

		glContext->disableVertexAttribArray(V_VERTICES_ARRAY);

		m_ambientColor.unbindShader();
		
        const Int32 lFontHeight = _pText->getTextHeight();
		Int32 lCurrentHeight = -_pFbo->getDimension()[Y]/2 + lBorder;

        _pText->setColor(Color(0.0f, 0.0f, 0.0f));

		lCurrentHeight += lFontHeight;
        _pText->write(Vector2i(-_pText->sizeOf(m_titleText)/2, lCurrentHeight), m_titleText);
		lCurrentHeight += lFontHeight + 2*lBorder;
        _pText->write(Vector2i(-_pFbo->getDimension()[X]/2 + lBorder, lCurrentHeight), m_lightmapText);
		lCurrentHeight += lFontHeight;
        _pText->write(Vector2i(-_pFbo->getDimension()[X]/2 + lBorder, lCurrentHeight), m_colormapText);
		lCurrentHeight += lFontHeight;
        _pText->write(Vector2i(-_pFbo->getDimension()[X]/2 + lBorder, lCurrentHeight), String("Light count : ") << m_lightCount);

		_pFbo->detach(FrameBuffer::COLOR_ATTACHMENT0);
		_pFbo->unbindBuffer();
	
		glContext->modelView().set(lModelView);
		glContext->projection().set(lProjection);

		glContext->setBackgroundColor(lOldBgColor);
		glContext->setViewPort(lOldViewPort);
		glContext->setDefaultDepthTest();

        glContext->setAntiAliasing(aa);
	}
}

void PCLODDebugLabel::drawContent()
{
    O3D_ASSERT(m_pTexture != nullptr);

	if (!m_pTexture && !m_pTexture->isValid())
		return;

	if (!m_ambientColor.isOperational())
		initShaders();

	Context *glContext = getScene()->getContext();

	m_ambientMap.bindShader();

	m_ambientMap.setConstColor(
		m_ambientMap.getUniform(U_AMBIENT),
		Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_ambientMap.setConstTexture(
		m_ambientMap.getUniform(U_AMBIENT_MAP),
		m_pTexture,
		0);
	m_ambientMap.setConstMatrix4(
		m_ambientMap.getUniform(U_MODELVIEWPROJECT_MATRIX),
		False,
		getScene()->getContext()->modelViewProjection());

	const Float lVertices[] = { 
			0.0f, 0.0f,
			Float(m_pTexture->getWidth()-1), 0.0f,
			0.0f, Float(m_pTexture->getHeight()-1),
			Float(m_pTexture->getWidth()-1), Float(m_pTexture->getHeight()-1) };

	m_drawVerticesVbo.create(8, VertexBuffer::DYNAMIC, lVertices, True);
	m_drawVerticesVbo.attribute(V_VERTICES_ARRAY, 2, 0, 0);

	const Float lTexCoords[] = { 
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f };

	m_drawTexCoordsVbo.create(8, VertexBuffer::DYNAMIC, lTexCoords, True);
	m_drawTexCoordsVbo.attribute(V_UV_MAP_ARRAY, 2, 0, 0);

	getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);

	m_pTexture->unbind();

	glContext->disableVertexAttribArray(V_UV_MAP_ARRAY);
	glContext->disableVertexAttribArray(V_VERTICES_ARRAY);

	m_ambientMap.unbindShader();
}

