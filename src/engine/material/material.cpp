/**
 * @file material.cpp
 * @brief Implementation of Material.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-08-30
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/material/material.h"
#include "o3d/engine/shader/shadermanager.h"

#include "o3d/engine/context.h"
#include "o3d/engine/object/vertexelement.h"
#include "o3d/engine/object/facearrayvisitor.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/core/debug.h"
#include "o3d/core/classfactory.h"
#include "o3d/core/filemanager.h"
#include "o3d/engine/shadow/shadowable.h"

using namespace o3d;

O3D_IMPLEMENT_CLASS_COMMON(Shadable, ENGINE_SHADABLE, nullptr)
O3D_IMPLEMENT_ABSTRACT_CLASS1(Material, ENGINE_MATERIAL, SceneResource)

// Default constructor.
Material::Material(BaseObject *parent) :
    SceneResource(parent),
	m_valid(False),
    m_vertexArray(getScene()->getContext()),
	m_initMode(AMBIENT)
{
}

// Virtual destructor.
Material::~Material()
{
}

// Build/rebuild the vertex attribute array according to the actual parameters
// of the shadable object.
void Material::buildVertexArray(Shadable &shadable)
{
	m_vertexArray.create(m_arrays, shadable);
}

// Serialization
Bool Material::writeToFile(OutStream &os)
{
    BaseObject::writeToFile(os);

    os << m_shaderName;

	return True;
}

Bool Material::readFromFile(InStream &is)
{
    BaseObject::readFromFile(is);

    is >> m_shaderName;

	return True;
}

// Save the material according to its class type and file name.
Bool Material::save()
{
	// need to define a filename
	if (m_filename.isEmpty())
		O3D_ERROR(E_InvalidPrecondition("The material file name must be previously defined"));

	// open the file
    FileOutStream *os = FileManager::instance()->openOutStream(m_filename, FileOutStream::CREATE);

	// write the material according to its object type
    if (!ClassFactory::writeToFile(*os, *this))
	{
        deletePtr(os);
		return False;
	}

    deletePtr(os);
	return True;
}
/*
// animate the material
void Material::animate(
	AnimationTrack::TrackType type,
	const void* value,
	UInt32 sizeOfValue,
	AnimationTrack::Target target,
	UInt32 subTarget,
	Animation::BlendMode blendMode,
	Float weight)
{
	Vector3 Vector;

	switch (type)
	{
		case AnimationTrack::TRACK_TYPE_FLOAT:
			if (sizeOfValue != sizeof(Float))
			{
				// the value size is invalid
				O3D_ERROR(E_InvalidParameter("SizeOfValue"));
				break;
			}

			switch (target)
			{
				case AnimationTrack::TARGET_MATERIAL_AMBIENT_R:
					if (blendMode == Animation::BLEND_REPLACE)  m_ambient[R] = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_ambient[R] += *((Float*)value);
				break;
				case AnimationTrack::TARGET_MATERIAL_AMBIENT_G:
					if (blendMode == Animation::BLEND_REPLACE)  m_ambient[G] = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_ambient[G] += *((Float*)value);
				break;
				case AnimationTrack::TARGET_MATERIAL_AMBIENT_B:
					if (blendMode == Animation::BLEND_REPLACE)  m_ambient[B] = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_ambient[B] += *((Float*)value);
				break;
				case AnimationTrack::TARGET_MATERIAL_AMBIENT_A:
					if (blendMode == Animation::BLEND_REPLACE)  m_ambient[A] = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_ambient[A] += *((Float*)value);
				break;
				case AnimationTrack::TARGET_MATERIAL_DIFFUSE_R:
					if (blendMode == Animation::BLEND_REPLACE)  m_diffuse[R] = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_diffuse[R] += *((Float*)value);
				break;
				case AnimationTrack::TARGET_MATERIAL_DIFFUSE_G:
					if (blendMode == Animation::BLEND_REPLACE)  m_diffuse[G] = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_diffuse[G] += *((Float*)value);
				break;
				case AnimationTrack::TARGET_MATERIAL_DIFFUSE_B:
					if (blendMode == Animation::BLEND_REPLACE)  m_diffuse[B] = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_diffuse[B] += *((Float*)value);
				break;
				case AnimationTrack::TARGET_MATERIAL_DIFFUSE_A:
					if (blendMode == Animation::BLEND_REPLACE)  m_diffuse[A] = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_diffuse[A] += *((Float*)value);
				break;
				case AnimationTrack::TARGET_MATERIAL_SPECULAR_R:
					if (blendMode == Animation::BLEND_REPLACE)  m_specular[R] = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_specular[R] += *((Float*)value);
				break;
				case AnimationTrack::TARGET_MATERIAL_SPECULAR_G:
					if (blendMode == Animation::BLEND_REPLACE)  m_specular[G] = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_specular[G] += *((Float*)value);
				break;
				case AnimationTrack::TARGET_MATERIAL_SPECULAR_B:
					if (blendMode == Animation::BLEND_REPLACE)  m_specular[B] = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_specular[B] += *((Float*)value);
				break;
				case AnimationTrack::TARGET_MATERIAL_SPECULAR_A:
					if (blendMode == Animation::BLEND_REPLACE)  m_specular[A] = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_specular[A] += *((Float*)value);
				break;
				case AnimationTrack::TARGET_MATERIAL_SHINE:
					if (blendMode == Animation::BLEND_REPLACE)  m_shine = *((Float*)value);
					else if (blendMode == Animation::BLEND_ADD)	m_shine += *((Float*)value);
				break;
				default:
				break;
			}
		break;

		case AnimationTrack::TRACK_TYPE_VECTOR:
			if (sizeOfValue != sizeof(Vector3))
			{
				// the value size is invalid
				O3D_ERROR(E_InvalidParameter("SizeOfValue"));
				break;
			}

			Vector = *((Vector3*)value);

			switch (target)
			{
				case AnimationTrack::TARGET_MATERIAL_AMBIENT:
					if (blendMode == Animation::BLEND_REPLACE)  m_ambient = Color(Vector[0],Vector[1],Vector[2],1.f);
					else if (blendMode == Animation::BLEND_ADD)	m_ambient += Color(Vector[0],Vector[1],Vector[2],1.f);
				break;
				case AnimationTrack::TARGET_MATERIAL_DIFFUSE:
					if (blendMode == Animation::BLEND_REPLACE)  m_diffuse = Color(Vector[0],Vector[1],Vector[2],1.f);
					else if (blendMode == Animation::BLEND_ADD)	m_diffuse += Color(Vector[0],Vector[1],Vector[2],1.f);
				break;
				case AnimationTrack::TARGET_MATERIAL_SPECULAR:
					if (blendMode == Animation::BLEND_REPLACE)  m_specular = Color(Vector[0],Vector[1],Vector[2],1.f);
					else if (blendMode == Animation::BLEND_ADD)	m_specular += Color(Vector[0],Vector[1],Vector[2],1.f);
				break;
				default:
				break;
			}
		break;

		default:
		break;
	}
}

// get the object ID for player serialization
Int32 Material::getAnimatableId(Animatable::AnimatableManager &type)
{
	type = Animatable::Material;
	return getId();
}
*/
/*
// Object rendering for shadow pass.
void MaterialDep::processShadow(Shadable &shadable, Shadowable &object, const DrawInfo &drawInfo)
{
	shadable.ProcessAllFaces(Shadable::PREPARE_GEOMETRY);

	if (drawInfo.Light.ShadowType == Shadowable::SHADOW_VOLUME)
	{
		object.projectSilhouette(drawInfo);
	}
	else if (drawInfo.Light.ShadowType == Shadowable::SHADOW_MAP_HARD)
	{
		// TODO have location in drawInfo
		Int32 a_vertex = drawInfo.ShaderInstance->getAttributeLocation("a_vertex");
		Int32 u_modelViewProjectionMatrix = drawInfo.ShaderInstance->getUniformLocation("u_modelViewProjectionMatrix");

		// TODO temporarily here have the light world matrix ^ - 1
        getScene()->getContext()->modelView().set(drawInfo.Light.matrixShadowMapProjection);

		drawInfo.shaderInstance->setConstMatrix4(
				u_modelViewProjectionMatrix,
				False,
				getScene()->getContext()->modelViewProjection());

		shadable.bindArray(V_VERTICES_ARRAY, a_vertex);

		shadable.processAllFaces(Shadable::PROCESS_GEOMETRY);
	}
}
*/
