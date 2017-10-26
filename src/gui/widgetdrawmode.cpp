/**
 * @file widgetdrawmode.cpp
 * @brief Base class for drawing widgets.
 * @author RinceWind
 * @author  Frederic SCHERMA
 * @date 2006-10-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/widgetdrawmode.h"
#include "o3d/gui/widgetdrawmaterial.h"

#include "o3d/gui/gui.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/texture/texturemanager.h"
#include "o3d/engine/context.h"

using namespace o3d;

// default constructor
WidgetDrawMode::WidgetDrawMode(Context *context, BaseObject *parent) :
    m_vertices(context),
    m_texCoords(context),
    m_faces(context),
	m_verticesOffs(0),
	m_facesOffs(0),
	m_material(parent)
{
	O3D_ASSERT(parent);

	// material
	m_material.setNumTechniques(1);
	m_material.getTechnique(0).setNumPass(1);

	m_material.getTechnique(0).getPass(0).setMaterial(Material::AMBIENT, new WidgetDrawMaterial(parent));
	m_material.getTechnique(0).getPass(0).setAmbientMap(m_material.getScene()->getTextureManager()->getDefaultTexture2D());
	m_material.getTechnique(0).getPass(0).setMapFiltering(MaterialPass::AMBIENT_MAP, Texture::NO_FILTERING);
	m_material.getTechnique(0).getPass(0).setMapWarp(MaterialPass::AMBIENT_MAP, Texture::REPEAT);
	m_material.getTechnique(0).getPass(0).setMapAnisotropy(MaterialPass::AMBIENT_MAP, 1.0f);

    m_material.getTechnique(0).getPass(0).setBlendingFunc(Blending::SRC_A__ONE_MINUS_SRC_A);
	m_material.getTechnique(0).getPass(0).setAmbient(Color(1.f,1.f,1.f,1.f));
	m_material.getTechnique(0).getPass(0).disableDepthTest();
	m_material.getTechnique(0).getPass(0).enableAlphaTest();
	m_material.getTechnique(0).getPass(0).setAlphaTestFunc(COMP_GREATER, 0.f);

	m_material.prepareAndCompile(*this);
}

WidgetDrawMode::WidgetDrawMode(WidgetDrawMode &dup) :
    m_vertices(dup.m_vertices.getContext()),
    m_texCoords(dup.m_texCoords.getContext()),
    m_faces(dup.m_faces.getContext()),
	m_verticesOffs(0),
	m_facesOffs(0),
	m_material(dup.m_material.getParent()),
	m_defaultSize(dup.m_defaultSize),
	m_defaultTextColor(dup.m_defaultTextColor),
	m_topLeftMargin(dup.m_topLeftMargin),
	m_bottomRightMargin(dup.m_bottomRightMargin)
{
    BaseObject *parent = dup.m_material.getParent();

	// material
	m_material.setNumTechniques(1);
	m_material.getTechnique(0).setNumPass(1);

	m_material.getTechnique(0).getPass(0).setMaterial(Material::AMBIENT, new WidgetDrawMaterial(parent));
	m_material.getTechnique(0).getPass(0).setAmbientMap(m_material.getScene()->getTextureManager()->getDefaultTexture2D());
	m_material.getTechnique(0).getPass(0).setMapFiltering(MaterialPass::AMBIENT_MAP, Texture::NO_FILTERING);
	m_material.getTechnique(0).getPass(0).setMapWarp(MaterialPass::AMBIENT_MAP, Texture::REPEAT);
	m_material.getTechnique(0).getPass(0).setMapAnisotropy(MaterialPass::AMBIENT_MAP, 1.0f);

    m_material.getTechnique(0).getPass(0).setBlendingFunc(Blending::SRC_A__ONE_MINUS_SRC_A);
	m_material.getTechnique(0).getPass(0).setAmbient(Color(1.f,1.f,1.f,1.f));
	m_material.getTechnique(0).getPass(0).disableDepthTest();
	m_material.getTechnique(0).getPass(0).enableAlphaTest();
	m_material.getTechnique(0).getPass(0).setAlphaTestFunc(COMP_GREATER, 0.f);

	m_material.prepareAndCompile(*this);
}

// destructor
WidgetDrawMode::~WidgetDrawMode()
{
}

// Set the texture.
void WidgetDrawMode::setTexture(Texture2D* texture)
{
	m_material.getTechnique(0).getPass(0).setAmbientMap(texture);
}

// Get the texture.
Texture2D* WidgetDrawMode::getTexture()
{
	return m_material.getTechnique(0).getPass(0).getAmbientMap();
}

void WidgetDrawMode::setMargin(const Vector2i &topLeft, const Vector2i &bottomRight)
{
	m_topLeftMargin = topLeft;
	m_bottomRightMargin = bottomRight;
}

Shadable::VertexProgramType WidgetDrawMode::getVertexProgramType() const
{
	return VP_MESH;
}

// Set the current shader of this object.
void WidgetDrawMode::setMaterial(Material* material)
{
	m_material.getTechnique(0).getPass(0).setMaterial(Material::AMBIENT, material);
}

void WidgetDrawMode::processAllFaces(Shadable::ProcessingPass pass)
{
	if (pass == Shadable::PREPARE_GEOMETRY)
	{
		m_faces.create(m_facesOffs, VertexBuffer::STREAMED, m_facesArray.getData(), True);
	}
	else if (pass == Shadable::PROCESS_GEOMETRY)
	{
		m_material.getScene()->drawRangeElementsUInt32(
				P_TRIANGLES,
				0,
				(m_verticesOffs >> 1) - 1,
				m_facesOffs,
				NULL);

		// and reset the arrays's offset
		m_verticesOffs = m_facesOffs = 0;
	}
}

void WidgetDrawMode::attribute(VertexAttributeArray mode, UInt32 location)
{
	// because this material is processed only on change of vertex data,
	// we update (or create) VBOs in this method

	if (mode == V_VERTICES_ARRAY)
	{
		m_vertices.create(m_verticesOffs, VertexBuffer::STREAMED, m_verticesArray.getData(), True);
		m_vertices.getContext()->vertexAttribArray(location, 2, 0, 0);
	}
	else if (mode == V_TEXCOORDS_2D_1_ARRAY)
	{
		m_texCoords.create(m_verticesOffs, VertexBuffer::STREAMED, m_texCoordsArray.getData(), True);
		m_vertices.getContext()->vertexAttribArray(location, 2, 0, 0);
	}
}

/*---------------------------------------------------------------------------------------
  Drawing methods for widget elements
---------------------------------------------------------------------------------------*/
void WidgetDrawMode::drawWidgetElement(
	WidgetDataSet widget,
	Int32 x,
	Int32 y,
	Int32 width,
	Int32 height)
{
	if (!widget.width || !widget.height)
		return;

	Float fW = 1.f / (Float)m_material.getTechnique(0).getPass(0).getAmbientMap()->getWidth();
	Float fH = 1.f / (Float)m_material.getTechnique(0).getPass(0).getAmbientMap()->getHeight();

	UInt32 sU = widget.x;
	UInt32 w;

	if (width < 0)
		w = widget.width;
	else
		w = width;

	UInt32 sV = widget.y;
	UInt32 h;

	if (height < 0)
		h = widget.height;
	else
		h = height;

	// null size
	if (!h || !w)
		return;

	UInt32 vertexId = m_verticesOffs >> 1;

	m_verticesArray[m_verticesOffs] = (Float)x;
	m_texCoordsArray[m_verticesOffs++] = (Float)sU * fW;
	m_verticesArray[m_verticesOffs] = (Float)y;
	m_texCoordsArray[m_verticesOffs++] = (Float)sV * fH;

	m_verticesArray[m_verticesOffs] = (Float)x;
	m_texCoordsArray[m_verticesOffs++] = (Float)sU * fW;
	m_verticesArray[m_verticesOffs] = (Float)y + h;
	m_texCoordsArray[m_verticesOffs++] = (Float)(sV+h) * fH;

	m_verticesArray[m_verticesOffs] = (Float)x + w;
	m_texCoordsArray[m_verticesOffs++] = (Float)(sU+w) * fW;
	m_verticesArray[m_verticesOffs] = (Float)y + h;
	m_texCoordsArray[m_verticesOffs++] = (Float)(sV+h) * fH;

	m_verticesArray[m_verticesOffs] = (Float)x + w;
	m_texCoordsArray[m_verticesOffs++] = (Float)(sU+w) * fW;
	m_verticesArray[m_verticesOffs] = (Float)y;
	m_texCoordsArray[m_verticesOffs++] = (Float)sV * fH;

	m_facesArray[m_facesOffs++] = vertexId;
	m_facesArray[m_facesOffs++] = vertexId + 1;
	m_facesArray[m_facesOffs++] = vertexId + 2;
	m_facesArray[m_facesOffs++] = vertexId;
	m_facesArray[m_facesOffs++] = vertexId + 2;
	m_facesArray[m_facesOffs++] = vertexId + 3;
}

void WidgetDrawMode::repeatWidgetElement(
	WidgetDataSet widget,
	Int32 x,
	Int32 y,
	Int32 width,
	Int32 height)
{
	if (!widget.width || !widget.height)
		return;

	Float fW = 1.f / (Float)m_material.getTechnique(0).getPass(0).getAmbientMap()->getWidth();
	Float fH = 1.f / (Float)m_material.getTechnique(0).getPass(0).getAmbientMap()->getHeight();

	UInt32 sU = widget.x;
	UInt32 w, wU = widget.width;

	if (width < 0)
		w = widget.width;
	else
		w = width;

	UInt32 sV = widget.y;
	UInt32 h, hV = widget.height;

	if (height < 0)
		h = widget.height;
	else
		h = height;

	// null size
	if (!h || !w)
		return;

	UInt32 vertexId = m_verticesOffs >> 1;

	m_verticesArray[m_verticesOffs] = (Float)x;
	m_texCoordsArray[m_verticesOffs++] = (Float)sU * fW;
	m_verticesArray[m_verticesOffs] = (Float)y;
	m_texCoordsArray[m_verticesOffs++] = (Float)sV * fH;

	m_verticesArray[m_verticesOffs] = (Float)x;
	m_texCoordsArray[m_verticesOffs++] = (Float)sU * fW;
	m_verticesArray[m_verticesOffs] = (Float)y + h;
	m_texCoordsArray[m_verticesOffs++] = (Float)(sV+hV) * fH;

	m_verticesArray[m_verticesOffs] = (Float)x + w;
	m_texCoordsArray[m_verticesOffs++] = (Float)(sU+wU) * fW;
	m_verticesArray[m_verticesOffs] = (Float)y + h;
	m_texCoordsArray[m_verticesOffs++] = (Float)(sV+hV) * fH;

	m_verticesArray[m_verticesOffs] = (Float)x + w;
	m_texCoordsArray[m_verticesOffs++] = (Float)(sU+wU) * fW;
	m_verticesArray[m_verticesOffs] = (Float)y;
	m_texCoordsArray[m_verticesOffs++] = (Float)sV * fH;

	m_facesArray[m_facesOffs++] = vertexId;
	m_facesArray[m_facesOffs++] = vertexId + 1;
	m_facesArray[m_facesOffs++] = vertexId + 2;
	m_facesArray[m_facesOffs++] = vertexId;
	m_facesArray[m_facesOffs++] = vertexId + 2;
	m_facesArray[m_facesOffs++] = vertexId + 3;
}

