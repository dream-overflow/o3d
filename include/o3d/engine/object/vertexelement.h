/**
 * @file vertexelement.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VERTEXELEMENT_H
#define _O3D_VERTEXELEMENT_H

#include "o3d/engine/vertexbuffer.h"
#include "o3d/engine/enginetype.h"
#include "o3d/core/smartarray.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class VertexElement;

/**
 * @brief VertexBufferBuilder
 * @date 2008-06-06
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * An elements of a geometry object stored in 32 bits float array. Excepted faces array.
 */
class O3D_API VertexBufferBuilder : NonCopyable<>
{
public:

	//! Default constructor.
	//! @param interleave If TRUE interleave data using stride.
	VertexBufferBuilder(Bool interleaved);

	//! Add a vertex data to the VBO.
	//! @param data Smart array.
	//! @param eltSize Size of one element in float (> 0).
	//! @return The offset in bytes into the VBO.
	UInt32 addData(const SmartArrayFloat &data, UInt32 eltSize);

	//! Add a vertex element to the VBO.
	//! @param data Smart array.
	//! @return The offset in bytes into the VBO.
	UInt32 addData(VertexElement *vertexElement, Bool keepLocalData = False);

	//! Create the VBO and validate any vertex elements.
	//! @param vbo Reference to an existing object.
	//! @note If the given vbo contains data it will be release before.
	void create(ArrayBufferf &vbo);

private:

	struct Element
	{
		SmartArrayFloat data;          //!< Smart array.
        UInt32 offset;                 //!< Offset in float entity
        VertexElement *vertexElement;  //!< Can be null.
        Bool keepLocalData;            //!< Keep local data if TRUE.
		const Float *lockedData;
		UInt32 numElt;
		UInt32 eltSize;
	};

	UInt32 m_interleave;

	std::vector<Element> m_datas;  //!< List of data to push into the VBO.
    UInt32 m_stride;               //!< Current stride.
    UInt32 m_size;                 //!< Total number of float elements.
};

/**
 * @brief The VertexElement class
 * @date 2008-06-06
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * An elements of a geometry object stored in 32 bits float array. Excepted faces array.
 */
class O3D_API VertexElement : NonCopyable<>
{
public:

	//! Lock mode
	enum LockMode
	{
		WRITE_ONLY = 0,  //!< Only for write data.
		READ_WRITE       //!< Read and write data.
	};

	//! Default constructor.
	//! @param type The type of data supported by this array.
	//! @param numElt Number of elements of element size.
	//! @param eltSize Size of an element. ie (x,y,z)=3 (u,v)=2.
	//! @note numElt*eltSize = length of the array.
	VertexElement(
			VertexAttributeArray type,
			UInt32 numElt = 0,
			UInt32 eltSize = 0);

	//! Construct with a data array.
	//! @param type The type of data supported by this array.
	//! @param data Data array.
	//! @param eltSize Size of an element. ie (x,y,z)=3 (u,v)=2.
	VertexElement(
			VertexAttributeArray type,
			const SmartArrayFloat &data,
			UInt32 eltSize = 0);

	//! Destructor.
	virtual ~VertexElement();

	//! Set the element size > 0. ie (x,y,z)=3 (u,v)=2.
	//! @note Element size must be defined before call Create.
	void setElementSize(UInt32 eltSize);

	//! Get the element size (ie 3 for a normal mean x,y,z value)
	inline UInt32 getElementSize() const { return m_eltSize; }

	//! Replace the data array. If a previous data array exists, and the
	//! the vertex element is valid, it must have the same size.
	void setData(const SmartArrayFloat &data, UInt32 eltSize);

	//! Get the data array (read only).
	inline const SmartArrayFloat& getData() const { return m_data; }

	//! Get the data array. This method can create inconsistency with the content of
	//! the VBO. If you are sure about what you do you can call setConsistentData(),
	//! to avoid the inconsistency status.
	inline SmartArrayFloat& getData() { return m_data; }

	//! Informs the object that local data and VBO data are consistent.
	inline void setConsistent(Bool consistency = True) { m_isDirty = !consistency; }

	//! Is the local data and VBO data are consistent.
	inline Bool isConsistent() { return !m_isDirty; }

	//! Get the element type
	inline VertexAttributeArray getType() const { return m_type; }

	//! Get the number of elements.
	//! @note The n-uple (x,y,z) mean one element for example.
	inline UInt32 getNumElements() const { return m_numElt; }

	//! Create and validate the vertex element.
	//! @param vbo Associated VBO where data are resident.
	//! @param offset Offset in data element where the data are stored into the associated VBO.
	//! @param stride Stride to the next element data into the associated VBO.
	//! @param keepLocalData If TRUE local data are kept.
	//! @note Stride and offsets are expressed in float entity.
	void create(
			ArrayBufferf &vbo,
			UInt32 offset,
			UInt32 stride,
			Bool keepLocalData = False);

	//! Is the vertex element is ready to use.
	inline Bool isValid() const { return m_isValid; }

	//! Get the associated VBO (read only).
	inline const ArrayBufferf& getVbo() const { return *m_vbo; }

	//! Update partially or entirely the content of the VBO.
	//! @param data Data to set.
	//! @param offset Offset in number of element (this value is multiplied by element size).
	//! @param numElt Size in number of element (this value is multiplied by element size).
	//! @note Take care of the stride.
	// TODO remove, replace, move ?
	void update(const Float *data, UInt32 offset, UInt32 numElt);

	//-----------------------------------------------------------------------------------
	// Process
	//-----------------------------------------------------------------------------------

	//! Attribute to a specific vertex array by its index.
	//! @note It bind the VBO if necessary, and enable the vertex attribute array
	//! if necessary.
	void attribute(UInt32 index);

	//! Access to the data in read only.
	//! @param offset Offset in number of elements.
	//! @param numElt Number of elements.
	const Float* lockArray(UInt32 offset = 0, UInt32 numElt = 0);

	//! Access to the data according to a specified mode.
	//! @param mode Lock mode.
	//! @param offset Offset in number of elements.
	//! @param numElt Number of elements.
	Float* lockArray(LockMode mode, UInt32 offset = 0,	UInt32 numElt = 0);

	//! Unlock the data array
	void unlockArray();

	//! Get the stride in float unit.
	inline UInt32 getStride() const { return m_stride / sizeof(Float); }

	//! Get the offset in float unit.
	inline UInt32 getOffset() const	{ return m_offset / sizeof(Float); }

	//! Get memory capacity in bytes.
	inline UInt32 getCapacity() const { return m_numElt * m_eltSize * sizeof(Float); }

    //! Get the data type. DATA_FLOAT.
	inline DataType getDataType() const { return DATA_FLOAT; }

    //! Get the advance per vertex. It returns the element size or the stride depending
    //! if stride is 0 or not.
    inline UInt32 getAdvance() const
    {
        return m_stride > 0 ? m_stride / sizeof(Float) : m_eltSize;
    }

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

protected:

	enum Lock
	{
		LOCK_NONE,
		LOCK_VBO,
		LOCK_DATA
	};

	VertexAttributeArray m_type;

	UInt32 m_numElt;    //!< Number of elements. m_numElt*m_eltSize = number of value.
	UInt32 m_eltSize;   //!< Size of an elements (3 for x,y,z, 2 for u,v...).

	ArrayBufferf *m_vbo;
	UInt32 m_offset;    //!< Offset in bytes into the associated VBO.
	UInt32 m_stride;    //!< Stride in bytes into the associated VBO.

	SmartArrayFloat m_data;

	Bool m_isValid;     //!< TRUE mean ready for use (linked to a valid VBO).
	Bool m_isDirty;     //!< TRUE mean local data differs from VBO data.

	Lock m_locked;          //!< Current lock status.
};

/**
 * @brief Vertex blender
 * @date 2008-06-06
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * Software vertex blending controller. Because these data can be modified more than
 * one time by frame, we need to works using local data. That the data of the vertex
 * element must be modified using the Update method.
 * For best performance use keptData when create for vertices/normals/rigging/skinning/
 * weighting vertex elements.
 */
class O3D_API VertexBlend : NonCopyable<>
{
public:

	//! Default constructor
	VertexBlend(
            Context *context,
			VertexElement *vertices,
			VertexElement *normals);

	//! Destructor
	~VertexBlend() {}

	//! Get the blended vertices element
	inline VertexElement& getVertices() { return *m_vertices; }

	//! Get the blended normals element
	inline VertexElement& getNormals() { return *m_normals; }

    //! Is interleaved.
    inline Bool isInterleaved() const { return m_vertices->getStride() > 0; }

    //! Is normal supported.
    inline Bool isNormal() const { return m_normals != nullptr; }

protected:

    ArrayBufferf m_vbo;       //!< Internal VBO.

	VertexElement *m_vertices;    //!< Final vertices element.
	VertexElement *m_normals;     //!< Final normal element.
};

} // namespace o3d

#endif // _O3D_VERTEXELEMENT_H

