/**
 * @file materialpass.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/core/classfactory.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/texture/cubemaptexture.h"
#include "o3d/engine/material/materialprofile.h"
#include "o3d/engine/material/materialtechnique.h"
#include "o3d/engine/material/materialpass.h"
#include "o3d/engine/texture/texturemanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"

using namespace o3d;

// Default constructor.
MaterialPass::MaterialPass(MaterialTechnique *technique) :
	m_technique(technique),
	m_activity(True),
    m_glContext(nullptr),
	m_cullingMode(CULLING_BACK_FACE),
	m_isSorted(False),
    m_blendingFunc(Blending::DISABLED),
	m_doubleSide(False),
	m_depthTest(True),
	m_depthWrite(True),
	m_infiniteDepthRange(False),
	m_alphaTest(False),
	m_alphaFunc(COMP_ALWAYS),
	m_alphaRef(0.f)
{
	O3D_ASSERT(m_technique);

	for (Int32 i = 0; i < Material::NUM_INIT_MODES; ++i)
		m_materials[i].setUser(technique->getProfile());

	m_glContext = technique->getProfile()->getScene()->getContext();

	m_floatParams[SHINE] = 1.f;
	m_floatParams[REFLECTIVITY] = 1.f;
	m_floatParams[TRANSPARENCY] = 1.f;

	m_vec2fParams[BUMP_OFFSET].set(0.f, 0.f);

	for (Int32 i = 0; i < NUM_MAP_TYPES; ++i)
		m_mapTypes[i].texture.setUser(technique->getProfile());
}

// Destructor.
MaterialPass::~MaterialPass()
{

}

// Copy operator.
MaterialPass& MaterialPass::operator= (const MaterialPass &dup)
{
	// materials
	m_activity = dup.m_activity;

	for (Int32 i = 0; i < Material::NUM_INIT_MODES; ++i)
	{
		if (dup.m_materials[i].isValid())
			m_materials[i] = static_cast<Material*>(
					dup.m_materials[i]->getClassInfo()->createInstance(m_technique->getProfile()));
		else
            m_materials[i] = nullptr;
	}

	// setting
	for (Int32 i = 0; i < NUM_FLOAT_PARAMS; ++i)
		m_floatParams[i] = dup.m_floatParams[i];

	for (Int32 i = 0; i < NUM_VECTOR2F_PARAM; ++i)
		m_vec2fParams[i] = dup.m_vec2fParams[i];

	for (Int32 i = 0; i < NUM_COLOR_MODELS; ++i)
		m_colors[i] = dup.m_colors[i];

	for (Int32 i = 0; i < NUM_MAP_TYPES; ++i)
	{
		m_mapTypes[i].texture = dup.m_mapTypes[i].texture;
		m_mapTypes[i].filtering = dup.m_mapTypes[i].filtering;
		m_mapTypes[i].warp = dup.m_mapTypes[i].warp;
		m_mapTypes[i].anisotropy = dup.m_mapTypes[i].anisotropy;
		m_mapTypes[i].resourceName = dup.m_mapTypes[i].resourceName;
	}

	// context
	m_cullingMode = dup.m_cullingMode;

	m_isSorted = dup.m_isSorted;
	m_blendingFunc = dup.m_blendingFunc;

	m_doubleSide = dup.m_doubleSide;
	m_depthTest = dup.m_depthTest;
	m_depthWrite = dup.m_depthWrite;
	m_infiniteDepthRange = dup.m_infiniteDepthRange;

	m_alphaTest = dup.m_alphaTest;
	m_alphaFunc = dup.m_alphaFunc;
	m_alphaRef = dup.m_alphaRef;

	return *this;
}

// Is the pass supported by the hardware.
Bool MaterialPass::isPassSupported() const
{
	for (Int32 i = 0; i < Material::NUM_INIT_MODES; ++i)
	{
		if (m_materials[i].isValid() && !m_materials[i]->isMaterialSupported())
			return False;
	}
	return True;
}

// Set the material.
void MaterialPass::setMaterial(Material::InitMode mode, Material *material)
{
	m_materials[mode] = material;
}

// Build the material pass (load texture, material...).
void MaterialPass::prepareAndCompile(Shadable &shadable)
{
	// load textures maps
	loadMaps();

	for (Int32 i = 0; i < Material::NUM_INIT_MODES; ++i)
	{
		if (m_materials[i].isValid() && !m_materials[i]->isValid())
			m_materials[i]->initialize(Material::InitMode(i), *this, shadable);
	}
}

// Clear any loaded pass data (inverse of PrepareAndCompile).
void MaterialPass::clear()
{
	for (Int32 i = 0; i < NUM_MAP_TYPES; ++i)
	{
		if (m_mapTypes[i].texture.isValid())
            m_mapTypes[i].texture = nullptr;
	}

	for (Int32 i = 0; i < Material::NUM_INIT_MODES; ++i)
	{
		if (m_materials[i].isValid())
			m_materials[i]->release();
	}
}

// Set a map type texture.
void MaterialPass::setMap(MapType type, Texture *map)
{
	m_mapTypes[type].texture = map;
	m_mapTypes[type].resourceName = map->getResourceName();
}

// Set a map resource name. Need a call to PrepareAndCompile before to be effective.
void MaterialPass::setMapResourceName(MapType type, const String &name)
{
    m_mapTypes[type].texture = nullptr;
	m_mapTypes[type].resourceName = name;
}

// Process the material pass to a shadable object.
void MaterialPass::processMaterial(
		Shadable &shadable,
		Shadowable *shadowable,
		Pickable *pickable,
		const DrawInfo &drawInfo)
{
	if (m_activity)
	{
		// establish the draw context
		if (m_depthTest)
			m_glContext->enableDepthTest();
		else
			m_glContext->disableDepthTest();

		if (m_depthWrite)
			m_glContext->enableDepthWrite();
		else
			m_glContext->disableDepthWrite();

		if (m_doubleSide || m_glContext->isOverrideDoubleSided())
			m_glContext->enableDoubleSide();
		else
			m_glContext->disableDoubleSide();

		if (m_infiniteDepthRange)
			m_glContext->setInfiniteDepthRange();
		else
			m_glContext->setDefaultDepthRange();

        m_glContext->blending().setFunc(m_blendingFunc);
		m_glContext->setCullingMode(m_cullingMode);

		// and process the material
		if ((drawInfo.pass == DrawInfo::AMBIENT_PASS) && m_materials[Material::AMBIENT].isValid())
			m_materials[Material::AMBIENT]->processAmbient(shadable, drawInfo, *this);
		else if ((drawInfo.pass == DrawInfo::PICKING_PASS) && m_materials[Material::PICKING].isValid())
			m_materials[Material::PICKING]->processPicking(shadable, *pickable, drawInfo, *this);
		else if ((drawInfo.pass == DrawInfo::LIGHTING_PASS) && m_materials[Material::LIGHTING].isValid())
			m_materials[Material::LIGHTING]->processLighting(shadable, *shadowable, drawInfo, *this);
		else if ((drawInfo.pass == DrawInfo::DEFERRED_PASS) && m_materials[Material::DEFERRED].isValid())
			m_materials[Material::DEFERRED]->processDeferred(shadable, drawInfo, *this);
	}
}

// Establish the filtering mode, warp mode, and anisotropy filtering to a specific map.
void MaterialPass::assignMapSetting(MapType type) const
{
	const Sampler &sampler = m_mapTypes[type];
	if (sampler.texture.isValid())
	{
		Texture *texture = sampler.texture.get();

		texture->setFiltering(sampler.filtering);
		texture->setWrap(sampler.warp);
		texture->setAnisotropy(sampler.anisotropy);
	}
}

Bool MaterialPass::writeToFile(OutStream &os) const
{
    os << MATERIAL_PASS_VERSION;

	for (Int32 i = 0; i < NUM_FLOAT_PARAMS; ++i)
        os << m_floatParams[i];

	for (Int32 i = 0; i < NUM_VECTOR2F_PARAM; ++i)
        os << m_vec2fParams[i];

	for (Int32 i = 0; i < NUM_COLOR_MODELS; ++i)
        os << m_colors[i];

	for (Int32 i = 0; i < NUM_MAP_TYPES; ++i)
	{
        os   << m_mapTypes[i].resourceName
			 << m_mapTypes[i].filtering
			 << m_mapTypes[i].warp
			 << m_mapTypes[i].anisotropy;
	}

	// context
    os   << m_cullingMode
	     << m_isSorted
	     << m_blendingFunc
	     << m_doubleSide
	     << m_depthTest
	     << m_depthWrite
	     << m_infiniteDepthRange
	     << m_alphaTest
	     << m_alphaFunc
	     << m_alphaRef;

	// material
	for (Int32 i = 0; i < Material::NUM_INIT_MODES; ++i)
	{
		if (m_materials[i].isValid())
		{
            os << True;
            ClassFactory::writeToFile(os, *m_materials[i]);
		}
		else
            os << False;
	}

	return True;
}

Bool MaterialPass::readFromFile(InStream &is)
{
	Int32 version;
    is >> version;

	if (version != MATERIAL_PASS_VERSION)
        O3D_ERROR(E_InvalidFormat("Material pass version"));

	// setting
	for (Int32 i = 0; i < NUM_FLOAT_PARAMS; ++i)
        is >> m_floatParams[i];

	for (Int32 i = 0; i < NUM_VECTOR2F_PARAM; ++i)
        is >> m_vec2fParams[i];

	for (Int32 i = 0; i < NUM_COLOR_MODELS; ++i)
        is >> m_colors[i];

	for (Int32 i = 0; i < NUM_MAP_TYPES; ++i)
	{
        is   >> m_mapTypes[i].resourceName
			 >> m_mapTypes[i].filtering
			 >> m_mapTypes[i].warp
			 >> m_mapTypes[i].anisotropy;
	}

	// context
    is   >> m_cullingMode
	     >> m_isSorted
	     >> m_blendingFunc
	     >> m_doubleSide
	     >> m_depthTest
	     >> m_depthWrite
	     >> m_infiniteDepthRange
	     >> m_alphaTest
	     >> m_alphaFunc
	     >> m_alphaRef;

	Bool isMaterial;

	// material
	for (Int32 i = 0; i < Material::NUM_INIT_MODES; ++i)
	{
        is >> isMaterial;

		if (isMaterial)
            m_materials[i] = ClassFactory::readFromFile(is, m_technique->getProfile());
		else
            m_materials[i] = nullptr;
	}

	return True;
}

// Load a 2D map to a specific type.
void MaterialPass::load2DMap(MapType type)
{
	Sampler &sampler = m_mapTypes[type];

	if (sampler.resourceName.isValid() && !sampler.texture.isValid())
		sampler.texture = m_technique->getProfile()->getScene()->getTextureManager()->addTexture2D(
				sampler.resourceName,
				sampler.filtering > Texture::LINEAR_FILTERING);
	else if (sampler.resourceName.isEmpty())
        sampler.texture = nullptr;
}

// Load a cube map to a specific type.
void MaterialPass::loadCubeMap(MapType type)
{
	Sampler &sampler = m_mapTypes[type];

	if (sampler.resourceName.isValid() && !sampler.texture.isValid())
		sampler.texture = m_technique->getProfile()->getScene()->getTextureManager()->addCubeMapTexture(
				sampler.resourceName,
				sampler.filtering > Texture::LINEAR_FILTERING);
	else if (sampler.resourceName.isEmpty())
        sampler.texture = nullptr;
}

// Load valid textures maps.
void MaterialPass::loadMaps()
{
	load2DMap(AMBIENT_MAP);
	load2DMap(DIFFUSE_MAP);
	load2DMap(SPECULAR_MAP);
	load2DMap(EMISSION_MAP);
	load2DMap(NORMAL_MAP);
	load2DMap(HEIGHT_MAP);
	load2DMap(OPACITY_MAP);
	load2DMap(REFLECTION_MAP);
	loadCubeMap(CUBICAL_ENV_MAP);
}

