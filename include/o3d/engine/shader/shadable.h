/**
 * @file shadable.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

/**
 * @file shadable.h
 * @brief Shadable interface
 * @date 2007-12-09
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @copyright Copyright (C) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_SHADABLE_H
#define _O3D_SHADABLE_H

#include "o3d/core/smartpointer.h"
#include "o3d/core/matrix4.h"
#include "o3d/engine/object/facearray.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class AABBoxExt;
class Matrix4;
class VertexElement;
class FaceArray;

/**
 * @brief Shadable object interface
 * @date 2007-12-09
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * Shablable interface for material rendered objects. It require the use of an
 * MaterialProfile object. It offers 3 vertex program methods :
 * - mesh for static mesh
 * - rigging for rigged by bones mesh
 * - skinning for skinned by bones mesh
 */
class O3D_API Shadable
{
public:

	O3D_DECLARE_INTERFACE(Shadable)

	//! Vertex program type.
	enum VertexProgramType
	{
		VP_MESH = 0,   //!< No additional transformation.
		VP_RIGGING,    //!< Transforms each vertex by zero or one bone.
		VP_SKINNING,   //!< Transforms each vertex by zero to four bones and weights.
		VP_BILLBOARD   //!< Billboard, always toward the camera.
	};

	//! Number of vertex program type.
	static const Int32 NUM_VERTEX_PROGRAM_TYPES = 4;

	//! Geometry process step.
	enum ProcessingPass
	{
		PREPARE_GEOMETRY,       //!< Prepare the geometry data.
		PROCESS_GEOMETRY,       //!< Process the geometry data (draw faces).
		PREPARE_SHADOW_VOLUME,  //!< Prepare the geometry for shadow volume rendering.
		PROCESS_SHADOW_VOLUME   //!< Process the rendering of shadow volume.
	};

	//! Number of processing passes.
	static const Int32 NUM_PROCESSING_PASS = 4;

	//! Additional data to query to the object.
	enum Operations
	{
		NORMALS,             //!< Normals element array.
		TANGENT_SPACE,       //!< Tangent space, include normals.
		CREATE               //!< Validate data query.
	};

	//! Get the shadable vertex program type.
	virtual VertexProgramType getVertexProgramType() const = 0;


	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! Set an external face array to process currently.
	//! @param faceArray External face array.
	//! @param numFaces Number of face to process from.
	//! @param firstFace Index of the first face to process.
	//! @param lastFast Index of the last face to process.
	virtual void useExternalFaceArray(
		FaceArray *faceArray,
		UInt32 numFaces,
		UInt32 firstFace,
		UInt32 lastFace) = 0;

	//! Use the internal face array.
	inline void useInternalFaceArray() { useExternalFaceArray(NULL, 0, 0, 0); }

	//! Return true if there is a specific list of faces.
	virtual Bool isExternalFaceArray() const = 0;

	//! Get the specific faces list.
	//! @return The external face array to process or NULL if none.
	//! @note first and last contains the first and last faces index to process.
	virtual FaceArray* getFaceArrayToProcess(UInt32 &first, UInt32 &last) = 0;

	//! Setup the modelview matrix for OpenGL before render the object
	virtual void setUpModelView() = 0;

	//! Process the rendering of any faces of the current face array.
	virtual void processAllFaces(ProcessingPass pass) = 0;


    //-----------------------------------------------------------------------------------
	// Access methods for current object state.
	//-----------------------------------------------------------------------------------

	//! Access to a currently processed vertex element.
	//! @param type The array type to retrieve.
	//! @return The vertex element or NULL.
	virtual VertexElement* getVertexElement(VertexAttributeArray type) const = 0;

	//! Access to the currently processed face array.
	//! @return The face array or NULL.
	virtual FaceArray* getFaceArray() const = 0;

	//! Bind an array in the specified attribute array location.
	virtual void attribute(VertexAttributeArray mode, UInt32 location) = 0;

	//! Make an operation.
	virtual void operation(Operations what) = 0;

	//! Is an operation is already done. For example ask if the tangent space is defined,
	//! or is the geometry data is create.
	virtual Bool isOperation(Operations what) const = 0;

	//! Obtain the absolute matrix of the object.
	virtual const Matrix4& getObjectWorldMatrix() const = 0;

	//! Get the matrix array for hardware skinning/rigging.
	virtual const Float* getMatrixArray() const = 0;

	//! Get the minimal square distance of the object bounding volume from the given point.
	virtual Float getDistanceFrom(const Vector3 &point) const = 0;
};

} // namespace o3d

//! Shadable no data access methods
#define O3D_SHADABLE_NO_DATA_ACCESS \
    virtual VertexElement* getVertexElement(VertexAttributeArray) const { return nullptr; }\
    virtual FaceArray* getFaceArray() const { return nullptr; }\
    virtual void operation(Operations what) {}\
	virtual Bool isOperation(Operations what) const { return False; }

//! Shadable always null material/shader
#define O3D_SHADABLE_VP_MESH \
	virtual ShaderProgram getVertexProgramType() const { return VP_MESH; }

//! Shadable no external face array support
#define O3D_SHADABLE_NO_EXT_ARRAY \
    virtual void useExternalFaceArray(FaceArray*, UInt32, UInt32, UInt32) {}\
    virtual Bool isExternalFaceArray() const { return False; }\
    virtual FaceArray* getFaceArrayToProcess(UInt32&, UInt32&) { return nullptr; }

//! Shadable no matrix array, no boundingBox, no modelview
#define O3D_SHADABLE_NO_MISC1 \
    virtual void setUpModelView() {}\
    virtual const Matrix4& getObjectWorldMatrix() const { return Matrix4::getIdentity(); }\
    virtual const Float* getMatrixArray() const { return nullptr; }\
	virtual Float getDistanceFrom(const Vector3 &point) const { return 0.0f; }

//! Shadable no matrix array, no boundingBox
#define O3D_SHADABLE_NO_MISC2 \
    virtual const Float* getMatrixArray() const { return nullptr; }

#endif // _O3D_SHADABLE_H

