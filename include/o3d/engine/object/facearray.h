/**
 * @file facearray.h
 * @brief A face set element.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-06-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FACEARRAY_H
#define _O3D_FACEARRAY_H

#include "o3d/core/smartarray.h"
#include "o3d/engine/vertexbuffer.h"
#include "o3d/core/templatearray.h"
#include "o3d/core/memorydbg.h"

#include "../enginetype.h"

namespace o3d {

class GeometryData;
class Scene;
class EdgeArray;

//---------------------------------------------------------------------------------------
//! @class FaceArray
//-------------------------------------------------------------------------------------
//! A face set element.
//---------------------------------------------------------------------------------------
class O3D_API FaceArray : NonCopyable<>
{
public:

	//! Default constructor
	FaceArray(
		PrimitiveFormat format = P_TRIANGLES,
		VertexBuffer::Storage storage = VertexBuffer::STATIC);

	//! Virtual destructor
	virtual ~FaceArray();

	//! Set the data format (triangle, lines...).
	inline void setFormat(PrimitiveFormat format) { m_format = format; }

	//! Get the data format.
	inline PrimitiveFormat getFormat() const { return m_format; }

	//! Set the data storage type.
	inline void setStorageType(VertexBuffer::Storage storage)
	{
		if (storage != m_storage)
		{
			m_storage = storage;
			m_isDirty = True;
		}
	}

	//! Get the data storage type.
	inline VertexBuffer::Storage getStorageType() const { return m_storage; }

	//! Get the size of a data element (triangle = 3, line = 2...).
	//! @note Return 0 for polygon format
	UInt32 getElementSize() const;

	//! Create the data for GPU.
	//! @return TRUE if the data are successfully assigned.
	virtual Bool create() = 0;

	//! Create the data for GPU according to the specified number of elements.
	//! @return TRUE if the data are successfully created.
	virtual Bool create(UInt32 numElt) = 0;

    //! Is the data are created (Create called, and system RAM = null if VBO used).
	inline Bool isCreated()const { return !m_isDirty; }

	//! Get the face element type size in bytes.
	virtual UInt32 getTypeSize() const = 0;

	//! Get the number of element.
	inline UInt32 getNumElements() const { return m_numElements; }

	//! Get the number of faces/lines. Count degenerated elements.
	UInt32 getNumFaces();

	//! Create the edge array.
	//! @return The number of computed edges.
	UInt32 computeEdges();

	//! Check if the edges are computed.
    inline Bool isEdgeArray() const { return m_edgeArray != nullptr; }

	//! Return the edge array.
	inline const EdgeArray& getEdgeArray() const { return *m_edgeArray; }

	//! Get he neighbor indices.
	//inline const ArrayInt32& getNeighborIndices() const { return m_neighborIndices; }


	//-----------------------------------------------------------------------------------
	// Process
	//-----------------------------------------------------------------------------------

	//! Set the first face index to draw.
	//! @note Invalid behavior for non static type like polygon, fans, strip
	//! Prefer the second method GetFirstIndice(...) in this case.
	inline void setFirstFace(UInt32 faceIndex) { m_offset = faceIndex * getElementSize(); }

	//! Get the first face index to draw.
	//! @note Invalid behavior for non static type like polygon, fans, strip
	inline UInt32 getFirstFace() const { return m_offset / getElementSize(); }

	//! Set the first element offset to draw.
	//! @note Invalid behavior for static type like triangle, line...
	//! Prefer the first method SetFirstFace(...) in this case.
	inline void setFirstIndice(UInt32 offset) { m_offset = offset; }

	//! Get the first element to draw.
	inline UInt32 getFirstIndice() const { return m_offset; }

	//! Lock the data array once it is initialized into VBO.
	virtual UInt8* lockArray(
		UInt32 offset = 0,
		UInt32 size = 0,
		VertexBuffer::LockMode flags = VertexBuffer::READ_WRITE) = 0;

	//! Unlock the data array.
	virtual void unlockArray() = 0;

	//! Get memory size
	virtual UInt32 getCapacity() const = 0;

	//! Bind the data array.
	virtual void bindArray() = 0;

	//! Draw using the data array.
	//! @param scene Valid scene to draw from.
	virtual void draw(Scene *scene) = 0;

	//! Draw using the data array, and with a specified range.
	//! @param scene Valid scene to draw from.
	virtual void drawPart(Scene *scene, UInt32 firstIndex, UInt32 lastIndex) = 0;

	//! Get the minimal vertex index.
	inline UInt32 getMinVertex() const { return m_minVertex; }

	//! Get the maximal vertex index.
	inline UInt32 getMaxVertex() const { return m_maxVertex; }

	//! Set the minimal and maximal vertices indices.
	//! @param minIndex Minimal vertex index.
	//! @param maxIndex Maximal vertex index.
	void setMinMaxVertex(UInt32 minIndex, UInt32 maxIndex);

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	UInt32 m_numElements;           //!< Number of data elements.
	UInt32 m_offset;                //!< Offset of the starting face to draw.

	UInt32 m_minVertex;             //!< Min vertex id.
	UInt32 m_maxVertex;             //!< Max vertex id.

	PrimitiveFormat m_format;           //!< P_TRIANGLES...
	VertexBuffer::Storage m_storage;    //!< Memory storage of the faces.

	Bool m_isDirty;                 //!< TRUE mean a call to create is necessary.

	EdgeArray *m_edgeArray;             //!< If non null, contain the computed edge.

	//ArrayInt32 m_neighborIndices;
};

//---------------------------------------------------------------------------------------
//! @class FaceArrayUInt16
//-------------------------------------------------------------------------------------
//! A face set element, using unsigned short indices. This is the most typically used
//! object because we rarely found geometry with more than 655535 vertices.
//---------------------------------------------------------------------------------------
class O3D_API FaceArrayUInt16 : public FaceArray
{
public:

	//! Constructor.
    //! @param context Context.
	//! @param format Primitive type (line, point, triangle...)
	FaceArrayUInt16(
        Context *context,
		PrimitiveFormat format = P_TRIANGLES,
		VertexBuffer::Storage storage = VertexBuffer::STATIC);

	//! Constructor taking a smart array of faces indices.
    //! @param context Context.
	//! @param format Primitive type (line, point, triangle...)
	//! @param faces Smart array containing faces.
	//! @param minIndex Minimal vertex index (if -1, it searched for).
	//! @param maxIndex Maximal vertex index (if -1, it searched for).
	//! @note To finalize you must call Create().
	FaceArrayUInt16(
        Context *context,
		PrimitiveFormat format,
		const SmartArrayUInt16 &faces,
		Int32 minIndex = -1,
		Int32 maxIndex = -1,
		VertexBuffer::Storage storage = VertexBuffer::STATIC);

	//! Virtual destructor.
	virtual ~FaceArrayUInt16() {}

	//! Set the faces indices.
	//! @param faces Smart array containing faces.
	//! @param minIndex Minimal vertex index (if -1, it searched for).
	//! @param maxIndex Maximal vertex index (if -1, it searched for).
	//! @note To finalize you must call Create().
	void setFaces(
		const SmartArrayUInt16 &faces,
		Int32 minIndex = -1,
		Int32 maxIndex = -1);

	//! Get the vertices indices (taking original array).
	//! @note It is not valid after Create().
	inline const SmartArrayUInt16& getFaces() const { return m_faces; }

	//! Get the vertices indices (taking original array). Only for convenience.
	//! @note It is not valid after Create().
	inline SmartArrayUInt16& getFaces() { return m_faces; }

	//! Get the face element type size in bytes.
	virtual UInt32 getTypeSize() const { return 2; }

	//! Get the VBO.
	//! @note It is not valid before Create().
	inline const ElementBufferObjus& getVbo() const { return m_vbo; }


	//-----------------------------------------------------------------------------------
	// Process
	//-----------------------------------------------------------------------------------

	//! Create the data for GPU.
	//! @return TRUE if the data are successfully assigned.
	virtual Bool create();

	//! Create the data for GPU according to the specified number of elements.
	//! @return TRUE if the data are successfully created.
	virtual Bool create(UInt32 numElt);

	//! Directly set and create from a data array.
	//! The VBO is created, data are not kept in memory (only VBO).
	//! @param faces External array containing faces.
	//! @param minIndex Minimal vertex index (if -1, it searched for).
	//! @param maxIndex Maximal vertex index (if -1, it searched for).
	//! @return TRUE if the data are successfully assigned.
	Bool create(
			const UInt16 *faces,
			UInt32 numElt,
			Int32 minIndex = -1,
			Int32 maxIndex = -1);

	//! Directly update a part from a data array.
	//! The VBO is update, data are not kept in memory (only VBO).
	//! The new data size must be lesser or equal than the current.
	//! @param numFaces Number of faces.
	//! @param offset Offset of the first element to update.
	//! @param numElt Number of element to update.
	void update(
			const UInt16 *faces,
			UInt32 offset,
			UInt32 numElt);

	//! Lock the data array once it is initialized into VBO.
	virtual UInt8* lockArray(
		UInt32 offset = 0,
		UInt32 size = 0,
		VertexBuffer::LockMode flags = VertexBuffer::READ_WRITE);

	//! Unlock the data array.
	virtual void unlockArray();

	//! Get memory size.
	virtual UInt32 getCapacity() const;

	//! Bind the data array.
	virtual void bindArray();

	//! Draw using the data array.
	//! @param scene Valid scene to draw from.
	virtual void draw(Scene *scene);

	//! Draw using the data array, and with a specified range.
	//! @param scene Valid scene to draw from.
	virtual void drawPart(Scene *scene, UInt32 firstIndex, UInt32 lastIndex);

	inline ArrayUInt16& getPermutation() { return m_permutation; }
	inline ArrayUInt16& getCollapseMap() { return m_collapseMap; }

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	SmartArrayUInt16 m_faces;       //!< memory array
	ElementBufferObjus m_vbo;       //!< VBO data (used only if created)

	ArrayUInt16 m_permutation;
	ArrayUInt16 m_collapseMap;

	//void computeEdgeInt(const UInt16 *faces);
	void computeMinMax(const UInt16 *data, UInt32 numElt);
};

//---------------------------------------------------------------------------------------
//! @class FaceArrayUInt32
//-------------------------------------------------------------------------------------
//! A face set element, using unsigned integer indices.
//---------------------------------------------------------------------------------------
class O3D_API FaceArrayUInt32 : public FaceArray
{
public:

	//! Constructor.
    //! @param context Context.
	//! @param format Primitive type (line, point, triangle...)
	FaceArrayUInt32(
        Context *context,
		PrimitiveFormat format = P_TRIANGLES,
		VertexBuffer::Storage storage = VertexBuffer::STATIC);

	//! Constructor taking a smart array of faces indices.
    //! @param context Context.
	//! @param format Primitive type (line, point, triangle...)
	//! @param faces Smart array containing faces.
	//! @param minIndex Minimal vertex index (if -1, it searched for).
	//! @param maxIndex Maximal vertex index (if -1, it searched for).
	//! @note To finalize you must call Create().
	FaceArrayUInt32(
        Context *context,
		PrimitiveFormat format,
		const SmartArrayUInt32 &faces,
		Int32 minIndex = -1,
		Int32 maxIndex = -1,
		VertexBuffer::Storage storage = VertexBuffer::STATIC);

	//! Virtual destructor.
	virtual ~FaceArrayUInt32() {}

	//! Set the vertices indices.
	//! @param faces Smart array containing faces.
	//! @param minIndex Minimal vertex index (if -1, it searched for).
	//! @param maxIndex Maximal vertex index (if -1, it searched for).
	//! @note To finalize you must call Create().
	void setFaces(
		const SmartArrayUInt32 &faces,
		Int32 minIndex = -1,
		Int32 maxIndex = -1);

	//! Get the vertices indices (taking original array).
	//! @note It is not valid after Create().
	inline const SmartArrayUInt32& getFaces() const { return m_faces; }

	//! Get the vertices indices (taking original array). Only for convenience.
	//! @note It is not valid after Create().
	inline SmartArrayUInt32& getFaces() { return m_faces; }

	//! Get the face element type size in bytes.
	virtual UInt32 getTypeSize() const { return 4; }

	//! Get the VBO.
	//! @note It is not valid before Create().
	inline const ElementBufferObjui& getVbo() const { return m_vbo; }


	//-----------------------------------------------------------------------------------
	// Process
	//-----------------------------------------------------------------------------------

	//! Create the data for GPU.
	//! @return TRUE if the data are successfully assigned.
	virtual Bool create();

	//! Create the data for GPU according to the specified number of elements.
	//! @return TRUE if the data are successfully created.
	virtual Bool create(UInt32 numElt);

	//! Directly set and create from a data array.
	//! The VBO is created, data are not kept in memory (only VBO).
	//! Set the vertices indices
	//! @param faces External array containing faces.
	//! @param minIndex Minimal vertex index (if -1, it searched for).
	//! @param maxIndex Maximal vertex index (if -1, it searched for).
	//! @return TRUE if the data are successfully assigned.
	Bool create(
			const UInt32 *faces,
			UInt32 numElt,
			Int32 minIndex = -1,
			Int32 maxIndex = -1);

	//! Directly update a part from a data array.
	//! The VBO is update, data are not kept in memory (only VBO).
	//! The new data size must be lesser or equal than the current.
	//! @param numFaces Number of faces.
	//! @param offset Offset of the first element to update.
	//! @param numElt Number of element to update.
	void update(
			const UInt32 *faces,
			UInt32 offset,
			UInt32 numElt);

	//! Lock the data array once it is initialized into VBO.
	virtual UInt8* lockArray(
		UInt32 offset = 0,
		UInt32 size = 0,
		VertexBuffer::LockMode flags = VertexBuffer::READ_WRITE);

	//! Unlock the data array.
	virtual void unlockArray();

	//! Get memory size.
	virtual UInt32 getCapacity() const;

	//! Bind the data array.
	virtual void bindArray();

	//! Draw using the data array.
	//! @param scene Valid scene to draw from.
	virtual void draw(Scene *scene);

	//! Draw using the data array, and with a specified range.
	//! @param scene Valid scene to draw from.
	virtual void drawPart(Scene *scene, UInt32 firstIndex, UInt32 lastIndex);

	inline ArrayUInt32& getPermutation() { return m_permutation; }
	inline ArrayUInt32& getCollapseMap() { return m_collapseMap; }

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	SmartArrayUInt32 m_faces;       //!< memory array
	ElementBufferObjui m_vbo;       //!< VBO data (used only if created)

	ArrayUInt32 m_permutation;
	ArrayUInt32 m_collapseMap;

	//void computeEdgeInt(const UInt32 *faces);
	void computeMinMax(const UInt32 *data, UInt32 numElt);
};

} // namespace o3d

#endif // _O3D_FACEARRAY_H

