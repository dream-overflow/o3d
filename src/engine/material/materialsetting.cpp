/**
 * @file materialsetting.cpp
 * @brief Implementation of MaterialSetting.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-01-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"

#include "o3d/core/file.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/texture/cubemaptexture.h"
#include "o3d/engine/texture/texturemanager.h"
#include "o3d/engine/material/materialsetting.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

// Default constructor.
MaterialSetting::MaterialSetting(BaseObject *parent) :
	m_activity(True)
{
	m_floatParams[SHINE] = 1.f;
	m_floatParams[REFLECTIVITY] = 1.f;
	m_floatParams[TRANSPARENCY] = 1.f;

	m_vec2fParams[BUMP_OFFSET].set(0.f, 0.f);

	for (Int32 i = 0; i < NUM_MAP_TYPES; ++i)
		m_mapTypes[i].setUser(parent);
}

// Destructor.
MaterialSetting::~MaterialSetting()
{

}

// Copy operator.
MaterialSetting& MaterialSetting::operator= (const MaterialSetting &dup)
{
	for (Int32 i = 0; i < NUM_FLOAT_PARAMS; ++i)
		m_floatParams[i] = dup.m_floatParams[i];

	for (Int32 i = 0; i < NUM_VECTOR2F_PARAM; ++i)
		m_vec2fParams[i] = dup.m_vec2fParams[i];

	for (Int32 i = 0; i < NUM_MAP_TYPES; ++i)
		m_mapTypes[i] = dup.m_mapTypes[i];

	return *this;
}

// Write content to file.
Bool MaterialSetting::writeToFile(OutStream &os) const
{
    os << MATERIAL_SETTING_VERSION;

	for (Int32 i = 0; i < NUM_FLOAT_PARAMS; ++i)
        os << m_floatParams[i];

	for (Int32 i = 0; i < NUM_VECTOR2F_PARAM; ++i)
        os << m_vec2fParams[i];

	for (Int32 i = 0; i < NUM_MAP_TYPES; ++i)
	{
		/*if (m_mapTypes[i].isValid())
			file << m_mapTypes[i]->getResourceName();
		else
			file << O3DString("<undefined>");*/
		if (m_mapTypes[i].isValid())
		{
            os   << True
				 << *m_mapTypes[i].get();
		}
		else
            os << False;
	}

	return True;
}

// Read content from file.
Bool MaterialSetting::readFromFile(Scene *scene, InStream &is)
{
	Int32 version;
    is >> version;

	if (version != MATERIAL_SETTING_VERSION)
        O3D_ERROR(E_InvalidFormat("Material setting version"));

	for (Int32 i = 0; i < NUM_FLOAT_PARAMS; ++i)
        is >> m_floatParams[i];

	for (Int32 i = 0; i < NUM_VECTOR2F_PARAM; ++i)
        is >> m_vec2fParams[i];

	Bool isMap = False;

	// AmbientMap
    is >> isMap;
	if (isMap)
        m_mapTypes[AMBIENT_MAP] = scene->getTextureManager()->readTexture2D(is);

	// DiffuseMap
    is >> isMap;
	if (isMap)
        m_mapTypes[DIFFUSE_MAP] = scene->getTextureManager()->readTexture2D(is);

	// SpecularMap
    is >> isMap;
	if (isMap)
        m_mapTypes[SPECULAR_MAP] = scene->getTextureManager()->readTexture2D(is);

	// EmissionMap
    is >> isMap;
	if (isMap)
        m_mapTypes[EMISSION_MAP] = scene->getTextureManager()->readTexture2D(is);

	// NormalMap
    is >> isMap;
	if (isMap)
        m_mapTypes[NORMAL_MAP] = scene->getTextureManager()->readTexture2D(is);

	// HeightMap
    is >> isMap;
	if (isMap)
        m_mapTypes[HEIGHT_MAP] = scene->getTextureManager()->readTexture2D(is);

	// OpacityMap
    is >> isMap;
	if (isMap)
        m_mapTypes[OPACITY_MAP] = scene->getTextureManager()->readTexture2D(is);

	// ReflectionMap
    is >> isMap;
	if (isMap)
        m_mapTypes[REFLECTION_MAP] = scene->getTextureManager()->readTexture2D(is);

	// CubicalEnvMap
    is >> isMap;
	if (isMap)
        m_mapTypes[CUBICAL_ENV_MAP] = scene->getTextureManager()->readTextureCubeMap(is);

	return True;
}

