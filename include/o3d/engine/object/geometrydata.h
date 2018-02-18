/**
 * @file geometrydata.h
 * @brief A set of elements defining a geometry object.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-06-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GEOMETRYDATA_H
#define _O3D_GEOMETRYDATA_H

#include "o3d/core/hashmap.h"
#include "o3d/core/templatebitset.h"

#include "o3d/core/smartpointer.h"
#include "vertexelement.h"
#include "facearray.h"
#include "../scene/sceneentity.h"

#include "o3d/geom/aabbox.h"
#include "o3d/geom/obbox.h"
#include "o3d/geom/bsphere.h"

#include "o3d/core/memorydbg.h"

namespace o3d {

//! Define the number max of bones on a vertex
static const Int32 MAX_SKINNING_BONES = 4;

class Primitive;

/**
 * @brief A set of elements defining a geometry object.
 */
class O3D_API GeometryData : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS_NO_COPY(GeometryData)

	//! bounding volume generation method
	enum BoundingMode
	{
		BOUNDING_SPHERE,   //!< Compute the bounding with a simple sphere.
		BOUNDING_BOX,      //!< Compute the bounding with an axis align box.
		BOUNDING_BOXEXT,   //!< Compute the bounding with a sphere optimized axis aligned box.
		BOUNDING_AUTO,     //!< Compute the bounding with the most appropriate method (works nice generally).
		BOUNDING_FAST      //!< Compute the bounding box using a faster method but with a lower precision.
	};

	//! Texture coordinates generation method.
	enum TexCoordsGen
	{
		TEXGEN_VERTEX_NORMALIZE,   //!< Simply normalize each vertex as UV coordinates.
        TEXGEN_VERTEX_PLANAR       //!< Vertex position as UV coordinates.
	};

	//! Default constructor
	GeometryData(BaseObject *parent);

	//! Build from a primitive object.
	GeometryData(BaseObject *parent, const Primitive &primitive);

	//! Virtual destructor
	virtual ~GeometryData();

	//! Create and validate all element and face arrays once all element and face array
	//! are defined.
	//! @param keepLocalDataForSkinning If TRUE keep in RAM any related skinning/rigging
	//! data in RAM. It is useful for CPU skinning to have better performance.
	void create(Bool keepLocalDataForSkinning = False);

	//! Is the geometry data is valid.
	Bool isValid() const;

	//-----------------------------------------------------------------------------------
	// Element
	//-----------------------------------------------------------------------------------

	//! Get the number of element for a specified attribute array.
	UInt32 getElementSize(VertexAttributeArray mode) const;

	//! Get the number of vertices.
	inline UInt32 getNumVertices() const
	{
        if (isVertices()) {
			return getVertices()->getNumElements();
        } else {
            return 0;
        }
	}

	//! Create an element array of a given type.
	//! @param mode Array type.
	//! @param data Data array.
	//! @return The new element or null if existing or error.
	VertexElement* createElement(
			VertexAttributeArray mode,
			const SmartArrayFloat &data);

	//! Get an element array of a given type.
    //! @note Can be null if the element was not previously created.
	inline VertexElement* getElement(VertexAttributeArray mode) { return m_elements[mode]; }

	//! Get an element array of a given type (read only).
    //! @note Can be null if the element was not previously created.
	inline const VertexElement* getElement(VertexAttributeArray mode) const { return m_elements[mode]; }

	//! Delete an element array given a type
	void deleteElement(VertexAttributeArray mode);

	//! Get the vertices element (read only)
	inline const VertexElement* getVertices() const { return m_elements[V_VERTICES_ARRAY]; }

	//! Get the vertices element.
	inline VertexElement* getVertices() { return m_elements[V_VERTICES_ARRAY]; }

	//! Get the normal element (read only)
	inline const VertexElement* getNormals() const { return m_elements[V_NORMALS_ARRAY]; }

	//! Get the normal element.
	inline VertexElement* getNormals() { return m_elements[V_NORMALS_ARRAY]; }

	//! Check for an elements
    inline Bool isElement(VertexAttributeArray mode) const { return (m_elements[mode] != nullptr); }

	//! Check for the vertices element
    inline Bool isVertices() const { return (m_elements[V_VERTICES_ARRAY] != nullptr); }

	//! Check for the normal element
    inline Bool isNormals() const { return (m_elements[V_NORMALS_ARRAY] != nullptr); }

	//! Check for the first unit of texture coordinates element
    inline Bool isTexCoords1() const { return (m_elements[V_UV_MAP_ARRAY] != nullptr); }

	//! Count the number of valid elements.
	UInt32 getNumElements() const;

	//! Set if data are interleaved.
	void setInterleave(Bool interleave);

	//! Is data are interleaved.
	Bool isInterleaved() const;

	//-----------------------------------------------------------------------------------
	// Face arrays
	//-----------------------------------------------------------------------------------

	//! Add a face array given an unique identifier.
	void addFaceArray(UInt32 idArray, FaceArray *faceArray);

	//! Get a face array given its unique identifier.
	//! @note Must not be delete using this accessor.
	inline FaceArray* getFaceArray(UInt32 idArray)
	{
		IT_FaceArrays it = m_faceArrays.find(idArray);
        if (it == m_faceArrays.end()) {
			O3D_ERROR(E_InvalidParameter("The face array id is not available"));
            return nullptr;
        } else {
			return it->second;
        }
	}

	//! Get a face array given its unique identifier (read only).
	//! @note Must not be delete using this accessor.
	inline const FaceArray* getFaceArray(UInt32 idArray) const
	{
		CIT_FaceArrays it = m_faceArrays.find(idArray);
        if (it == m_faceArrays.end()) {
			O3D_ERROR(E_InvalidParameter("The face array key is not available"));
            return nullptr;
        } else {
			return it->second;
        }
	}

	//! Delete a specified face array given its unique identifier
	void deleteFaceArray(UInt32 idArray);

	//! Get the number of face arrays.
	inline UInt32 GetNumFaceArrays() const { return (UInt32)m_faceArrays.size(); }

	//! Get the total number face of any faces arrays.
	UInt32 getNumFaces() const;

	//-----------------------------------------------------------------------------------
	// Process
	//-----------------------------------------------------------------------------------

	//! Create a new vertex blend
	VertexBlend* createVertexBlend();

	//! Get memory usage of all these geometry and faces array.
	UInt32 getCapacity() const;

	//! Bind the vertex buffer object.
	void bindVertexBuffer();

	//! Bind a data array. It enable the vertex attribute array if necessary.
	void attribute(VertexAttributeArray mode, UInt32 location);

	//! Bind an external face array before to draw given the face array unique identifier.
	void bindFaceArray(UInt32 idArray);

	//! Bind a face array before to draw given the face array unique identifier.
	void bindExternalFaceArray(FaceArray *faceArray);

	//! Get the bound face array (read only).
	inline const FaceArray* getBoundFaceArray() const { return m_boundFaceArray; }

	//! Draw the geometry using an external face array, and with a specified range.
	void drawPart(
		FaceArray *faceArray,
		UInt32 firstIndex,
		UInt32 lastIndex);

	//! Draw using the current bound face array.
	void draw();

	//! Draw the local space using an external face array, and with a specified range.
	//! Local space is the representation of the TBN for any vertex.
	void drawLocalSpacePart(
		FaceArray *faceArray,
		UInt32 firstIndex,
		UInt32 lastIndex);

	//! Draw the local space using the current bound face array.
	//! Local space is the representation of the TBN for any vertex.
	void drawLocalSpace();

	//-----------------------------------------------------------------------------------
	// Utilities
	//-----------------------------------------------------------------------------------

	//! Generate UV element using a specified method, into the specified array.
	//! @param dest Must be one of the 2d textures units.
	//! @param method Generation method.
	//! @param scale Scale the input vertex.
	//! @param uFrom u come from X,Y or Z.
	//! @param vFrom v come from X,Y or Z.
	void computeTexCoords(
			VertexAttributeArray dest,
			TexCoordsGen method,
			const Vector3 &scale,
			UInt32 uFrom,
			UInt32 vFrom);

	//-----------------------------------------------------------------------------------
	// Bounding volume
	//-----------------------------------------------------------------------------------

	//! compute the bounding volume of this geometry, using all face arrays.
	//! @param mode Bounding mode computation and usage.
	void computeBounding(BoundingMode mode);

	//! Get the optimal bounding type (BSphere, AABBox, or AABBoxExt).
	inline BoundingMode getBoundingMode() { return m_boundingMode; }

	//! Get the axis aligned bounding box.
	inline const AABBox& getAABBox() const { return *reinterpret_cast<const AABBox*>(&m_AABBoxExt); }

	//! Get the extended axis aligned bounding box.
	inline const AABBoxExt& getAABBoxExt() const { return m_AABBoxExt; }

	//! Get the oriented bounding box.
	inline const OBBox& getOBBox() const { return *reinterpret_cast<const OBBox*>(&m_OBBoxExt); }

	//! Get the oriented extended bounding box.
	inline const OBBoxExt& getOBBoxExt() const { return m_OBBoxExt; }

	//! Get the bounding sphere.
	inline const BSphere& getBSphere() const { return m_BSphere; }

	//-----------------------------------------------------------------------------------
	// Tangent space
	//-----------------------------------------------------------------------------------

	//! Compute the normal.
	//! @param generate If TRUE it will compute the normals according to the vertices
	//! and face array.
	void genNormals(Bool generate = True);

	//! Compute the tangent space (tangents and bi-tangents arrays).
	//! @param generate If TRUE it will compute the tangent space according to the
	//! vertices, normals, and texture coordinates unit 1.
	//! @note It will compute the normals optionally if necessary.
	void genTangentSpace(Bool generate = True);

	//! Is the tangent space (including normals) generated.
	Bool isTangentSpace() const;

	//-----------------------------------------------------------------------------------
	// LOD methods
	//-----------------------------------------------------------------------------------

	//! Compute the progressive mesh for each face arrays.
	void genProgressiveMesh(Bool generate = True);

	//! Define the LOD level in percent for each face arrays.
	void setLodLvl(UInt32 lvl);

	//! Get the LOD level in percent.
	inline UInt32 getLodLvl() const { return m_lodLvl; }

	//! Is progressive mesh is computed for each face arrays.
	Bool isProgressive() const;

	//-----------------------------------------------------------------------------------
	// Vertex blending methods
	//-----------------------------------------------------------------------------------

	//! Get the geometry vertex blender (read only)
	const VertexBlend* getVertexBlender() const { return m_vertexBlender; }

	//! Get the geometry vertex blender
	VertexBlend* getVertexBlender() { return m_vertexBlender; }

	//! Use of a vertex blender to prepare array
	inline void bindVertexBlender(VertexBlend *vertexBlender) { m_vertexBlender = vertexBlender; }

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

protected:

	enum Update
	{
		UPDATE_VERTEX_BUFFER = 0,
		UPDATE_INDEX_BUFFER,
		UPDATE_BOUNDING,
		UPDATE_NORMALS,
		UPDATE_TANGENT_SPACE,
		UPDATE_PROGRESSIVE_MESH,
		GEN_NORMALS,
		GEN_TANGENT_SPACE,
		GEN_PROGRESSIVE_MESH,
		INTERLEAVE_ELEMENTS
	};

	typedef stdext::hash_map<UInt32,FaceArray*> T_FaceArrays;
	typedef T_FaceArrays::iterator IT_FaceArrays;
	typedef T_FaceArrays::const_iterator CIT_FaceArrays;

	ArrayBufferf *m_vbo;      //!< VBO containing the geometry data.
    BitSet32 m_flags;         //!< Flag of update.

	VertexElement *m_elements[NUM_VERTEX_ATTRIBUTES];
	T_FaceArrays m_faceArrays;

	AABBoxExt m_AABBoxExt;         //!< Global axis aligned bounding box.
	OBBoxExt  m_OBBoxExt;          //!< Global oriented bounding box.
	BSphere   m_BSphere;           //!< Global bounding sphere.
	BoundingMode m_boundingMode;   //!< Bounding mode (BSphere, AABBox or AABBoxExt).

    UInt32 m_lodLvl;               //!< LOD level in percent.

	FaceArray *m_boundFaceArray;   //!< Current bound face array.
	VertexBlend *m_vertexBlender;  //!< Vertex blender for software rigging/skinning.

	//! Create/Get/Recreate a VertexElement.
    VertexElement* newVertexElement(VertexAttributeArray mode, const SmartArrayFloat &data);

	//! Build from an O3DPrimitive object
	void buildFromPrimitive(const Primitive & primitive);

	//! Compute normals.
	void computeNormals();

	//! Compute tangents and bi-tangents.
	void computeTangentSpace();

	//! Compute the progressive mesh for each face arrays.
	void computeProgressive();
};

} // namespace o3d

#endif // _O3D_GEOMETRYDATA_H
