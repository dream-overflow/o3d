/**
 * @file meshdata.cpp
 * @brief Implementation of MeshData.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-10-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/meshdata.h"

#include "o3d/engine/object/meshdatamanager.h"
#include "o3d/core/debug.h"
#include "o3d/core/filemanager.h"
#include "o3d/engine/glextensionmanager.h"
#include "o3d/core/math.h"
#include "o3d/engine/object/primitive.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine//scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(MeshData, ENGINE_MESH_DATA, SceneResource)

// constructor
MeshData::MeshData(BaseObject *parent) :
	SceneResource(parent),
	m_geometry(this)
{
}

// get capacity (compute the memory taken by the object)
UInt32 MeshData::getCapacity(Bool countOptional) const
{
	UInt32 size = 0;

	if (m_geometry.isValid())
		size += m_geometry->getCapacity();

	return size;
}

// create for OpenGL all non already created sub-mesh
void MeshData::createGeometry()
{
	if (m_geometry.isValid())
		m_geometry->create();
}

// Destroy the contained geometry and clear the filename
void MeshData::destroy()
{
    m_geometry = nullptr;
	m_filename = "";
}

// compute the TBN space
void MeshData::genTangentSpace()
{
	if (m_geometry.isValid())
		m_geometry->genTangentSpace();
}

// compute the vertex normals
void MeshData::genNormals()
{
	if (m_geometry.isValid())
		m_geometry->genNormals();
}

// compute the total number of vertices for this mesh-data
UInt32 MeshData::getNumVertices()const
{
	UInt32 temp = 0;

	if (m_geometry.isValid())
		temp += m_geometry->getNumVertices();

	return temp;
}

// compute the total number of faces for this mesh-data
UInt32 MeshData::getNumFaces()const
{
	UInt32 temp = 0;

	if (m_geometry.isValid())
		temp += m_geometry->getNumFaces();

	return temp;
}

//---------------------------------------------------------------------------------------
// Serialization
//---------------------------------------------------------------------------------------

// Load a geometry file (*.o3dms) using a scene relative path filename or absolute
Bool MeshData::load(const String &filename)
{
	GeometryData *geometry = new GeometryData(this);

	// open the file
    m_filename = FileManager::instance()->getFullFileName(filename);
    InStream* is = FileManager::instance()->openInStream(m_filename);

	// read the mesh data
    if (!geometry->readFromFile(*is))
	{
        deletePtr(is);
		return False;
	}

    deletePtr(is);

	setGeometry(geometry);
	return True;
}

// Save the geometry file (*.o3dms) into the specified path and using the defined filename
Bool MeshData::save()
{
	if (!m_geometry.isValid())
		O3D_ERROR(E_InvalidOperation("A geometry must be defined to export"));

	// TODO do a dirty checking to re-save only if necessary

	// need to define a filename
	if (m_filename.isEmpty())
		O3D_ERROR(E_InvalidPrecondition("The mesh data file name must be previously defined"));

	// open the file
    FileOutStream *os = FileManager::instance()->openOutStream(m_filename, FileOutStream::CREATE);

	// geometry
    if (!m_geometry->writeToFile(*os))
	{
        deletePtr(os);
		return False;
	}

    deletePtr(os);
	return True;
}

Bool MeshData::writeToFile(OutStream &os)
{
    BaseObject::writeToFile(os);

    os << m_resourceName;

	return True;
}

Bool MeshData::readFromFile(InStream &is)
{
    BaseObject::readFromFile(is);

    is >> m_resourceName;

	return True;
}

// Default constructor.
MeshDataTask::MeshDataTask(
		MeshData *meshData,
		const String &filename) :
			m_meshData(meshData),
            m_geometry(nullptr)
{
	if (!meshData)
		O3D_ERROR(E_InvalidParameter("The mesh-data must be valid"));

	m_filename = FileManager::instance()->getFullFileName(filename);
}

Bool MeshDataTask::execute()
{
	O3D_ASSERT(m_filename.isValid());
	if (m_filename.isValid())
	{
        InStream *is = FileManager::instance()->openInStream(m_filename);

		m_geometry = new GeometryData(m_meshData);

        Bool result = m_geometry->readFromFile(*is);

		if (!result)
			deletePtr(m_geometry);

        deletePtr(is);

		return result;
	}
	else
		return False;
}

Bool MeshDataTask::finalize()
{
	if (m_meshData && m_geometry)
	{
		m_meshData->setGeometry(m_geometry);
		m_meshData->createGeometry();

		return True;
	}
	else
		return False;
}

