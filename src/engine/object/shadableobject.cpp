/**
 * @file shadableobject.cpp
 * @brief Implementation of ShadableObject.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2007-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/shadableobject.h"

using namespace o3d;

O3D_IMPLEMENT_ABSTRACT_CLASS1(ShadableObject, ENGINE_SHADABLE_OBJECT, SceneObject)

// Default constructor
ShadableObject::ShadableObject(BaseObject *parent) :
	SceneObject(parent)
{
    m_shadableInfo.faceArray = nullptr;
	m_shadableInfo.numFaces = 0;
	m_shadableInfo.firstFace = 0;
	m_shadableInfo.lastFace = 0;

	m_shadableInfo.vertexProgram = Shadable::VP_MESH;
	m_shadableInfo.activeVertexProgram = Shadable::VP_MESH;
}

// Copy constructor
ShadableObject::ShadableObject(const ShadableObject &dup) :
	SceneObject(dup)
{
    m_shadableInfo.faceArray = nullptr;
	m_shadableInfo.numFaces = 0;
	m_shadableInfo.firstFace = 0;
	m_shadableInfo.lastFace = 0;

	m_shadableInfo.vertexProgram = dup.m_shadableInfo.vertexProgram;
	m_shadableInfo.activeVertexProgram = Shadable::VP_MESH;
}

// Duplicate
ShadableObject& ShadableObject::operator= (const ShadableObject &dup)
{
	SceneObject::operator =(dup);

    m_shadableInfo.faceArray = nullptr;
	m_shadableInfo.numFaces = 0;
	m_shadableInfo.firstFace = 0;
	m_shadableInfo.lastFace = 0;

	m_shadableInfo.vertexProgram = dup.m_shadableInfo.vertexProgram;
	m_shadableInfo.activeVertexProgram = Shadable::VP_MESH;

	return *this;
}

// Get the shadable vertex program to use.
Shadable::VertexProgramType ShadableObject::getVertexProgramType() const
{
	return m_shadableInfo.vertexProgram;
}

// Set an external face array to process. It is needed for the O3DAlphaPipeline.
void ShadableObject::useExternalFaceArray(
	FaceArray *faceArray,
	UInt32 numFaces,
	UInt32 firstFace,
	UInt32 lastFace)
{
	m_shadableInfo.faceArray = faceArray;
	m_shadableInfo.numFaces = numFaces;
	m_shadableInfo.firstFace = firstFace;
	m_shadableInfo.lastFace = lastFace;
}

//! Return TRUE if there is an external face array to process.
Bool ShadableObject::isExternalFaceArray() const
{
	return m_shadableInfo.faceArray;
}

//! get the specified faces list
FaceArray* ShadableObject::getFaceArrayToProcess(UInt32 &first, UInt32 &last)
{
	first = m_shadableInfo.firstFace;
	last = m_shadableInfo.lastFace;

	return m_shadableInfo.faceArray;
}
