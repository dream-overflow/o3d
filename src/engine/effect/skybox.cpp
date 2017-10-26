/**
 * @file skybox.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/engine/texture/texturemanager.h"
#include "o3d/engine/material/materialprofile.h"
#include "o3d/engine/material/colormaterial.h"
#include "o3d/engine/effect/skybox.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/object/camera.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(SkyBox, ENGINE_EFFECT_SKYBOX, SpecialEffects)

// constructor
SkyBox::SkyBox(BaseObject *parent) :
	SpecialEffects(parent),
	m_currentSide(0),
	m_size(1000.f),
	m_material(this),
	m_halfHeight(False)
{
	for (Int32 i = 0; i < 6; ++i)
	{
		m_textures[i].setUser(this);
        m_vertices[i] = new VertexBufferObjf(getScene()->getContext());
        m_texCoords[i] = new VertexBufferObjf(getScene()->getContext());
	}

	if (!getScene())
		return;

	// material
	m_material.setNumTechniques(1);
	m_material.getTechnique(0).setNumPass(1);

    m_material.getTechnique(0).getPass(0).setMaterial(Material::AMBIENT, new ColorMaterial(this));
    m_material.getTechnique(0).getPass(0).setMaterial(Material::DEFERRED, new ColorMaterial(this));
	m_material.getTechnique(0).getPass(0).setDiffuse(Color(1.f,1.f,1.f,1.f));
    m_material.getTechnique(0).getPass(0).setDiffuseMap(getScene()->getTextureManager()->getDefaultTexture2D());
    m_material.getTechnique(0).getPass(0).setMapFiltering(MaterialPass::DIFFUSE_MAP, Texture::LINEAR_FILTERING);
    m_material.getTechnique(0).getPass(0).setMapWarp(MaterialPass::DIFFUSE_MAP, Texture::CLAMP);
    m_material.getTechnique(0).getPass(0).setMapAnisotropy(MaterialPass::DIFFUSE_MAP, 1.0f);

    m_material.getTechnique(0).getPass(0).disableDepthWrite();
	m_material.prepareAndCompile(*this);
}

SkyBox::~SkyBox()
{
    destroy();

	for (Int32 i = 0; i < 6; ++i)
	{
		deletePtr(m_vertices[i]);
		deletePtr(m_texCoords[i]);
	}
}

// Initialize vertex buffer object for the valid skybox sides.
void SkyBox::initVBO()
{
	Float boxSize = m_size * 0.5f;
	Float yBase = boxSize;

	if (m_halfHeight)
		yBase = 0.f;

	// TODO 1 seul VBO avec offset

	// X+
	if (m_textures[0].isValid())
	{
		const Float vertices[12] = {
				boxSize,   -yBase, -boxSize,
				boxSize,   -yBase,  boxSize,
				boxSize,  boxSize, -boxSize,
				boxSize,  boxSize,  boxSize };

		const Float texCoords[8] = {
				0.f, 1.f,
				1.f, 1.f,
				0.f, 0.f,
				1.f, 0.f };

		m_vertices[0]->create(12, VertexBuffer::STATIC, vertices);
		m_texCoords[0]->create(8, VertexBuffer::STATIC, texCoords);
	}

	// X-
	if (m_textures[1].isValid())
	{
		const Float vertices[12] = {
				-boxSize,   -yBase,  boxSize,
				-boxSize,   -yBase, -boxSize,
				-boxSize,  boxSize,  boxSize,
				-boxSize,  boxSize, -boxSize };

		const Float texCoords[8] = {
				0.f, 1.f,
				1.f, 1.f,
				0.f, 0.f,
				1.f, 0.f };

		m_vertices[1]->create(12, VertexBuffer::STATIC, vertices);
		m_texCoords[1]->create(8, VertexBuffer::STATIC, texCoords);
	}

	// Y+
	if (m_textures[2].isValid())
	{
		const Float vertices[12] = {
				-boxSize, boxSize, -boxSize,
				 boxSize, boxSize, -boxSize,
				-boxSize, boxSize,  boxSize,
				 boxSize, boxSize,  boxSize };

		const Float texCoords[8] = {
				0.f, 1.f,
				1.f, 1.f,
				0.f, 0.f,
				1.f, 0.f };

		m_vertices[2]->create(12, VertexBuffer::STATIC, vertices);
		m_texCoords[2]->create(8, VertexBuffer::STATIC, texCoords);
	}

	// Y-
	if (m_textures[3].isValid())
	{
		const Float vertices[12] = {
				-boxSize, -yBase,  boxSize,
				 boxSize, -yBase,  boxSize,
				-boxSize, -yBase, -boxSize,
				 boxSize, -yBase, -boxSize };

		const Float texCoords[8] = {
				0.f, 1.f,
				1.f, 1.f,
				0.f, 0.f,
				1.f, 0.f };

		m_vertices[3]->create(12, VertexBuffer::STATIC, vertices);
		m_texCoords[3]->create(8, VertexBuffer::STATIC, texCoords);
	}

	// Z+
	if (m_textures[4].isValid())
	{
		const Float vertices[12] = {
			 boxSize,  -yBase, boxSize,
			-boxSize,  -yBase, boxSize,
			 boxSize, boxSize, boxSize,
			-boxSize, boxSize, boxSize };

		const Float texCoords[8] = {
				0.f, 1.f,
				1.f, 1.f,
				0.f, 0.f,
				1.f, 0.f };

		m_vertices[4]->create(12, VertexBuffer::STATIC, vertices);
		m_texCoords[4]->create(8, VertexBuffer::STATIC, texCoords);
	}

	// Z-
	if (m_textures[5].isValid())
	{
		const Float vertices[12] = {
			-boxSize,   -yBase, -boxSize,
			 boxSize,   -yBase, -boxSize,
			-boxSize,  boxSize, -boxSize,
			 boxSize,  boxSize, -boxSize };

		const Float texCoords[8] = {
				0.f, 1.f,
				1.f, 1.f,
				0.f, 0.f,
				1.f, 0.f };

		m_vertices[5]->create(12, VertexBuffer::STATIC, vertices);
		m_texCoords[5]->create(8, VertexBuffer::STATIC, texCoords);
	}
}

void SkyBox::create(
	Float size,
	Texture2D *xp, Texture2D *xn,
	Texture2D *yp, Texture2D *yn,
	Texture2D *zp, Texture2D *zn,
	Bool halfHeight,
	Texture::FilteringMode filtering,
	Float anisotropy)
{
	m_size = size;
	m_halfHeight = halfHeight;

    m_material.getTechnique(0).getPass(0).setMapFiltering(MaterialPass::DIFFUSE_MAP, filtering);
    m_material.getTechnique(0).getPass(0).setMapWarp(MaterialPass::DIFFUSE_MAP, Texture::CLAMP);
    m_material.getTechnique(0).getPass(0).setMapAnisotropy(MaterialPass::DIFFUSE_MAP, anisotropy);

	m_textures[0] = xp; m_textures[1] = xn;
	m_textures[2] = yp; m_textures[3] = yn;
	m_textures[4] = zp; m_textures[5] = zn;

	initVBO();
}

Int32 SkyBox::create(
	Float size,
	const String &xp, const String &xn,
	const String &yp, const String &yn,
	const String &zp, const String &zn,
	Bool halfHeight,
	Texture::FilteringMode filtering,
	Float anisotropy)
{
	m_textures[0] = m_textures[1] = m_textures[2] = m_textures[3] = m_textures[4] = m_textures[5] = NULL;

	m_size = size;
	m_halfHeight = halfHeight;

    m_material.getTechnique(0).getPass(0).setMapFiltering(MaterialPass::DIFFUSE_MAP, filtering);
    m_material.getTechnique(0).getPass(0).setMapWarp(MaterialPass::DIFFUSE_MAP, Texture::CLAMP);
    m_material.getTechnique(0).getPass(0).setMapAnisotropy(MaterialPass::DIFFUSE_MAP, anisotropy);

    if (xp.isValid())
		m_textures[0] = getScene()->getTextureManager()->addTexture2D(xp, filtering > Texture::LINEAR_FILTERING);
    if (xn.isValid())
		m_textures[1] = getScene()->getTextureManager()->addTexture2D(xn, filtering > Texture::LINEAR_FILTERING);
    if (yp.isValid())
		m_textures[2] = getScene()->getTextureManager()->addTexture2D(yp, filtering > Texture::LINEAR_FILTERING);
    if (yn.isValid())
		m_textures[3] = getScene()->getTextureManager()->addTexture2D(yn, filtering > Texture::LINEAR_FILTERING);
    if (zp.isValid())
		m_textures[4] = getScene()->getTextureManager()->addTexture2D(zp, filtering > Texture::LINEAR_FILTERING);
    if (zn.isValid())
		m_textures[5] = getScene()->getTextureManager()->addTexture2D(zn, filtering > Texture::LINEAR_FILTERING);

	Int32 n = 0;

	// check textures
	for (Int32 i = 0 ; i < 6 ; ++i)
		if (m_textures[i])
			n++;

	initVBO();

	return n;
}

// reset settings
void SkyBox::destroy()
{
	m_size = 1000.f;
	m_halfHeight = False;

    m_material.setDiffuse(Color(1.0f,1.0f,1.0f,1.f));

	for (Int32 i = 0; i < 6; ++i)
	{
		m_vertices[i]->release();
		m_texCoords[i]->release();

        m_textures[i] = nullptr;
	}
}

// setup the modelview matrix for OpenGL before render the object
void SkyBox::setUpModelView()
{
    O3D_ASSERT(getScene()->getActiveCamera() != nullptr);

	Matrix4 mat = getScene()->getActiveCamera()->getModelviewMatrix();
	mat.setTranslation(0.f,0.f,0.f);

	getScene()->getContext()->modelView().set(mat);
}

// draw all faces
void SkyBox::processAllFaces(Shadable::ProcessingPass pass)
{
	if (pass == Shadable::PROCESS_GEOMETRY)
	{
		getScene()->drawArrays(P_TRIANGLE_STRIP, 0, 4);
	}
}

void SkyBox::attribute(VertexAttributeArray mode, UInt32 location)
{
	if (mode == V_VERTICES_ARRAY)
		m_vertices[m_currentSide]->attribute(location, 3, 0, 0);
	else if (mode == V_TEXCOORDS_2D_1_ARRAY)
		m_texCoords[m_currentSide]->attribute(location, 2, 0, 0);
}

// draw skybox
void SkyBox::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

    // only drawn in ambient or deferred pass
    if (drawInfo.pass != DrawInfo::AMBIENT_PASS && drawInfo.pass != DrawInfo::DEFERRED_PASS)
        return;

	//Float boxSize = m_size * .5f;
	//Float yBase = boxSize;

	//if (m_halfHeight)
	//	yBase = 0.f;

	setUpModelView();

	// render the six sides
	for (m_currentSide = 0; m_currentSide < 6; ++m_currentSide)
	{
		if (m_textures[m_currentSide].isValid())
		{
            m_material.getTechnique(0).getPass(0).setDiffuseMap(m_textures[m_currentSide].get());
            m_material.processMaterial(*this, nullptr, nullptr, drawInfo);
		}
	}
}

// Serialization
Bool SkyBox::writeToFile(OutStream &os)
{
    if (!SpecialEffects::writeToFile(os))
		return False;

    os   << m_size
	     << m_halfHeight
         << m_material.getTechnique(0).getPass(0).getDiffuse()
         << m_material.getTechnique(0).getPass(0).getMapFiltering(MaterialPass::DIFFUSE_MAP)
         << m_material.getTechnique(0).getPass(0).getMapAnisotropy(MaterialPass::DIFFUSE_MAP);

	for (Int32 i = 0 ; i < 6 ; ++i)
	{
		if (m_textures[i])
            os << m_textures[i]->getResourceName();
		else
            os << String("");
	}

	return True;
}

Bool SkyBox::readFromFile(InStream &is)
{
    if (!SpecialEffects::readFromFile(is))
		return False;

    Color diffuse;
	Texture::FilteringMode filtering;
	Float anisotropy;

    is   >> m_size
	     >> m_halfHeight
         >> diffuse
	     >> filtering
	     >> anisotropy;

    m_material.setDiffuse(diffuse);
    m_material.getTechnique(0).getPass(0).setMapFiltering(MaterialPass::DIFFUSE_MAP, filtering);
    m_material.getTechnique(0).getPass(0).setMapAnisotropy(MaterialPass::DIFFUSE_MAP, anisotropy);

    String textureResourceName;

	for (Int32 i = 0; i < 6; ++i)
	{
        is >> textureResourceName;
        if (textureResourceName.isValid())
			m_textures[i] = getScene()->getTextureManager()->addTexture2D(
                    textureResourceName,
					filtering > Texture::LINEAR_FILTERING);
		else
            m_textures[i] = nullptr;
	}

	return True;
}

