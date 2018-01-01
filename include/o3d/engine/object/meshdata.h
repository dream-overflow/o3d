/**
 * @file meshdata.h
 * @brief MeshData object that contain any kind of geometry.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-10-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MESHDATA_H
#define _O3D_MESHDATA_H

#include <vector>

#include "geometrydata.h"
#include "../scene/sceneentity.h"
#include "o3d/core/task.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class MeshDataManager;

/**
 * @brief A mesh data object contain any geometry data, and skinning weighting/bones id
 * informations.
 * @details It contain a GeometryData object and its geometry object contain one or many
 * FaceArray objects.
 * @note Important consideration: For save memory and increase OpenGL performance,
 * use face arrays in 16bits (UInt16) for meshdata for vertices lesser
 * than 65536. Otherwise use a 32bits (UInt32) face array. So take care of using the
 * optimal format. You can process an optimize method to process many optimization for
 * you, but this cost some CPU time, and only recommended at authoring state.
 */
class O3D_API MeshData : public SceneResource
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(MeshData)

	//! Default constructor
	MeshData(BaseObject *parent);

	//! Compute the size of the geometry and all faces array.
	//! @param countOptional count the size of additional arrays of MeshData
	UInt32 getCapacity(Bool countOptional) const;

	//-----------------------------------------------------------------------------------
	// Global data
	//-----------------------------------------------------------------------------------

	//! Get the number of vertices contained
	UInt32 getNumVertices() const;

	//! Compute the total number of faces
	UInt32 getNumFaces() const;

	//-----------------------------------------------------------------------------------
	// Generation methods
	//-----------------------------------------------------------------------------------

	//! Compute tangent and bi-tangent
	void genTangentSpace();

	//! Compute normals
	void genNormals();

	//! Compute the bounding volume given the mode
    void computeBounding(GeometryData::BoundingMode mode);

	//-----------------------------------------------------------------------------------
	// General settings
	//-----------------------------------------------------------------------------------

	//! Set the .o3dms mesh data file name.
	inline void setFileName(const String &filename) { m_filename = filename; }
	//! Get the .o3dms mesh data file name.
	inline const String& getFileName() const { return m_filename; }

	//-----------------------------------------------------------------------------------
	// Geometry data methods
	//-----------------------------------------------------------------------------------

	//! Define the geometry object. The previous is deleted.
    inline void setGeometry(GeometryData *geometry) { m_geometry = geometry; }

	//! Get the geometry object, or null if none.
	inline GeometryData* getGeometry() const { return m_geometry.get(); }

	//! Get the geometry object in way to process a specified face array
    //! @param arrayId Identifier of the FaceArray into the geometry to bind
	//! @note 0 always mean the first face array.
	inline void bindGeometry(UInt32 arrayId)
	{
        if (m_geometry.isValid()) {
			m_geometry->bindFaceArray(arrayId);
        }
	}

	//! Create/validate geometry data. Create VBO if necessary or validate for VertexArray.
    //! @param setPersistant @see Geometry::create
	void createGeometry();

	//! Destroy the contained geometry and clear the filename
	void destroy();

	//-----------------------------------------------------------------------------------
	// LOD management
	//-----------------------------------------------------------------------------------

	//! Set the current LOD percent
	inline void setCurrentLod(UInt32 lvl)
	{
        if (m_geometry.isValid()) {
			return m_geometry->setLodLvl(lvl);
        }
	}

	//! Get the current LOD percent (100 if not supported)
	inline UInt32 getCurrentLod()const
	{
        if (m_geometry.isValid()) {
			return m_geometry->getLodLvl();
        } else {
			return 100;
        }
	}

	//! Is progressive mesh supported
	inline Bool isProgressive() const
	{
        if (m_geometry.isValid()) {
			return m_geometry->isProgressive();
        } else {
			return False;
        }
	}

	//! Compute the the progressive mesh data
	inline void genProgressiveMesh()
	{
        if (m_geometry.isValid()) {
			return m_geometry->genProgressiveMesh();
        }
	}

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	//! Load a geometry file (*.o3dms) using a scene relative path filename or absolute.
	Bool load(const String &filename);

	//! Save the geometry file (*.o3dms) into the defined filename.
	Bool save();

    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

	String m_filename;                    //!< filename for retrieve the meshdata
	SmartObject<GeometryData> m_geometry; //!< Geometry data
};

/**
 * @brief Task responsible of the loading of a mesh-data object.
 */
class O3D_API MeshDataTask : public Task
{
public:

	//! Default constructor.
	//! @param meshData Mesh-data target.
	//! @param filename Filename of the mesh-data to load.
    MeshDataTask(MeshData *meshData, const String &filename);

    virtual Bool execute() override;
    virtual Bool finalize() override;

private:

	String m_filename;
	MeshData *m_meshData;
	GeometryData *m_geometry;
};

} // namespace o3d

#endif // _O3D_MESHDATA_H
