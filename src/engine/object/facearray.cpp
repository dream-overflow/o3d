/**
 * @file facearray.cpp
 * @brief Implementation of FaceArray.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-06-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/facearray.h"
#include "o3d/engine/object/edgearray.h"
#include "o3d/engine/object/facearrayvisitor.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/core/file.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// class FaceArray
//---------------------------------------------------------------------------------------

// Default constructor.
FaceArray::FaceArray(
	PrimitiveFormat format,
	VertexBuffer::Storage storage) :
		m_numElements(0),
		m_offset(0),
		m_minVertex(0),
		m_maxVertex(0),
		m_format(format),
		m_storage(storage),
		m_isDirty(True),
		m_edgeArray(NULL)
{
}

// Virtual destructor.
FaceArray::~FaceArray()
{
	deletePtr(m_edgeArray);
}

// Get the size of a data element (GL_TRIANGLES = 3, GL_LINES = 2...).
UInt32 FaceArray::getElementSize() const
{
	switch (m_format)
	{
		case P_POINTS:
			return 1;
		case P_LINES:
			return 2;
		case P_LINE_LOOP:
			return 2;
		case P_LINE_STRIP:
			return 2;
		case P_TRIANGLES:
			return 3;
		case P_TRIANGLE_STRIP:
			return 3;
		case P_TRIANGLE_FAN:
			return 3;
		default:
			return 0;
	};
}

// Get the number of faces/lines. Count degenerated elements.
UInt32 FaceArray::getNumFaces()
{
	switch (m_format)
	{
		case P_POINTS:
			return m_numElements;
		case P_LINES:
			return m_numElements / 2;
		case P_LINE_LOOP:
			return m_numElements;
		case P_LINE_STRIP:
			return m_numElements - 1;
		case P_TRIANGLES:
			return m_numElements / 3;
		case P_TRIANGLE_STRIP:
			return m_numElements - 2;
		case P_TRIANGLE_FAN:
			return m_numElements - 2;
		default:
			return 0;
	};
}

// Create the edge array.
UInt32 FaceArray::computeEdges()
{
	if (!m_edgeArray || m_isDirty)
	{
		m_edgeArray = new EdgeArray();

		FaceArrayVisitor triangles(this);

		return m_edgeArray->buildEdges(triangles, m_maxVertex);
	}
	else
		return m_edgeArray->getNumEdges();
}

// Set the minimal and maximal vertices indices.
void FaceArray::setMinMaxVertex(UInt32 minIndex, UInt32 maxIndex)
{
	if (minIndex > maxIndex)
		O3D_ERROR(E_InvalidParameter("Minimal vertex index must be lesser than maximal vertex index"));

	m_minVertex = minIndex;
	m_maxVertex = maxIndex;
}

Bool FaceArray::writeToFile(OutStream &os)
{
    os   << m_numElements
		 << m_offset
		 << m_minVertex
		 << m_maxVertex
		 << m_format
		 << m_storage;

	return True;
}

Bool FaceArray::readFromFile(InStream &is)
{
    is   >> m_numElements
		 >> m_offset
		 >> m_minVertex
		 >> m_maxVertex
		 >> m_format
		 >> m_storage;

	m_isDirty = True;
	return True;
}


//---------------------------------------------------------------------------------------
// class FaceArrayUInt16
//---------------------------------------------------------------------------------------

// Default constructor, no parameters.
FaceArrayUInt16::FaceArrayUInt16(
        Context *context,
    PrimitiveFormat format,
    VertexBuffer::Storage storage) :
        FaceArray(format, storage),
        m_vbo(context)
{
}

// Constructor taking a smart array of faces indices.
FaceArrayUInt16::FaceArrayUInt16(
        Context *context,
        PrimitiveFormat format,
        const SmartArrayUInt16 &faces,
    Int32 minIndex,
    Int32 maxIndex,
    VertexBuffer::Storage storage) :
        FaceArray(format, storage),
        m_vbo(context)
{
	setFaces(faces, minIndex, maxIndex);
}

// Set the faces indices.
void FaceArrayUInt16::setFaces(
	const SmartArrayUInt16 &faces,
	Int32 minIndex,
	Int32 maxIndex)
{
	m_faces = faces;
	m_numElements = faces.getNumElt();

	if ((minIndex < 0) || (maxIndex < 0))
		computeMinMax(m_faces.getData(), m_faces.getNumElt());
	else
	{
		m_minVertex = minIndex;
		m_maxVertex = maxIndex;
	}

	m_isDirty = True;
}

// Create the data for GPU
Bool FaceArrayUInt16::create()
{
	if (!m_isDirty)
		return True;

	if (!m_faces.getData())
	{
		m_minVertex = m_maxVertex = 0;
		m_numElements = 0;
		m_faces.releaseCheckAndDelete();
		m_vbo.release();

	//	O3D_ERROR(O3D_E_InvalidParameter("Data array must be defined"));
		return True;
	}

	if (m_minVertex > m_maxVertex)
		O3D_ERROR(E_InvalidParameter("Minimal vertex index must be lesser than maximal vertex index"));

	m_numElements = m_faces.getNumElt();

	//ComputeEdgeInt(m_faces.getData());

	m_vbo.create(m_faces.getNumElt(),m_storage,m_faces.getData());
	m_faces.releaseCheckAndDelete();

	if (m_vbo.isExist())
	{
		m_isDirty = False;
		return True;
	}
	else
		return False;
}

// Create the data for GPU according to the specified number of elements.
Bool FaceArrayUInt16::create(UInt32 numElt)
{
    m_vbo.create(numElt, m_storage, nullptr);
	m_faces.releaseCheckAndDelete(); // a previous set...

	m_numElements = numElt;

	if (m_vbo.isExist())
	{
		m_isDirty = False;
		return True;
	}
	else
		return False;
}

// Directly set and create from a data array.
Bool FaceArrayUInt16::create(
		const UInt16 *faces,
		UInt32 numElt,
		Int32 minIndex,
		Int32 maxIndex)
{
	O3D_ASSERT(numElt > 0);

	if (!faces)
		O3D_ERROR(E_InvalidParameter("Data array must be defined"));
	if (!numElt)
		O3D_ERROR(E_InvalidParameter("Number of elements must be greater than zero"));

	if (minIndex > maxIndex)
		O3D_ERROR(E_InvalidParameter("Minimal vertex index must be lesser than maximal vertex index"));

	m_numElements = numElt;

	// Compute the min/max vertex indices if necessary
	if ((minIndex < 0) || (maxIndex < 0))
		computeMinMax(faces, numElt);
	else
	{
		m_minVertex = minIndex;
		m_maxVertex = maxIndex;
	}

	//ComputeEdgeInt(faces);

	m_vbo.create(numElt, m_storage, faces);
	m_faces.releaseCheckAndDelete(); // a previous set...

	if (m_vbo.isExist())
	{
		m_isDirty = False;
		return True;
	}
	else
		return False;
}

// Directly update a part from a data array.
void FaceArrayUInt16::update(
		const UInt16 *faces,
		UInt32 offset,
		UInt32 numElt)
{
	O3D_ASSERT(numElt > 0);

	if (!m_vbo.isExist())
		O3D_ERROR(E_InvalidPrecondition("Must be previously created"));

	if (!faces)
		O3D_ERROR(E_InvalidParameter("Data array must be defined"));
	if (!numElt)
		O3D_ERROR(E_InvalidParameter("Number of face an indices (elements) must be greater than zero"));

	if (numElt+offset > m_numElements)
		O3D_ERROR(E_InvalidParameter("Number of face to update must be lesser than current number of elements"));

	if (offset >= m_numElements)
		O3D_ERROR(E_InvalidParameter("Offset must be lesser than current number of elements"));

	m_vbo.update(faces, offset, numElt);
	m_isDirty = False;
}

// Lock the data array once it is initialized into VBO
UInt8* FaceArrayUInt16::lockArray(
		UInt32 offset,
		UInt32 size,
		VertexBuffer::LockMode flags)
{
	if (m_faces.isValid())
		return reinterpret_cast<UInt8*>(m_faces.getData() + offset);
	else if (m_vbo.isExist())
		return reinterpret_cast<UInt8*>(m_vbo.lock(offset, size, flags));
	else
        return nullptr;
}

// Unlock the data array
void FaceArrayUInt16::unlockArray()
{
	if (m_vbo.isExist() && m_vbo.isLocked())
		m_vbo.unlock();
}

// Get memory size
UInt32 FaceArrayUInt16::getCapacity() const
{
	if (m_faces.isValid())
		return m_faces.getNumElt() << 1;
	else if (m_vbo.isExist())
		return m_vbo.getCount() << 1;
	else
		return 0;
}

// Bind the data array 
void FaceArrayUInt16::bindArray()
{
	if (m_vbo.isExist())
		return m_vbo.bindBuffer();
}

// Draw using the data array
void FaceArrayUInt16::draw(Scene *scene)
{
	if (m_isDirty)
		return;

	O3D_ASSERT(scene);

	if (m_vbo.isExist())
	{
		UInt32 numElements = m_vbo.getCount() - m_offset;

		m_vbo.bindBuffer();
		scene->drawRangeElementsUInt16(
				m_format,
				m_minVertex,
				m_maxVertex,
				numElements,
				reinterpret_cast<UInt16*>(m_offset * sizeof(UInt16)));
	}
}

// Draw using the data array, and with a specified range.
void FaceArrayUInt16::drawPart(Scene *scene, UInt32 firstIndex, UInt32 lastIndex)
{
	if (m_isDirty)
		return;

	O3D_ASSERT(scene);

	if (m_vbo.isExist())
	{
		UInt32 numElements = lastIndex - firstIndex;

		m_vbo.bindBuffer();
		scene->drawRangeElementsUInt16(
			m_format,
			m_minVertex,
			m_maxVertex,
			numElements,
			reinterpret_cast<UInt16*>(firstIndex * sizeof(UInt16)));
	}
}

Bool FaceArrayUInt16::writeToFile(OutStream &os)
{
    FaceArray::writeToFile(os);

	if (m_faces.isValid())
	{
        os << m_faces;
	}
	else if (m_vbo.isExist())
	{
		UInt16 *data = m_vbo.lock(0, 0, VertexBuffer::READ_ONLY);
		UInt32 count = m_vbo.getCount();

        os << count;
        os.write(data, count);

		m_vbo.unlock();
	}
	else
	{
        os << UInt32(0);
        //O3D_ERROR(E_InvalidPrecondition("The VBO or the data array must be valid"));
	}

	return True;
}

Bool FaceArrayUInt16::readFromFile(InStream &is)
{
    FaceArray::readFromFile(is);

    is >> m_faces;

	return True;
}

void FaceArrayUInt16::computeMinMax(const UInt16 *data, UInt32 numElt)
{
	m_minVertex = o3d::Limits<UInt32>::max();
	m_maxVertex = o3d::Limits<UInt32>::min();

	for (UInt32 i = 0; i < numElt; ++i)
	{
		m_minVertex = o3d::min<UInt32>(m_minVertex, data[i]);
		m_maxVertex = o3d::max<UInt32>(m_maxVertex, data[i]);
	}
}

/*void FaceArrayUInt16::computeEdgeInt(const UInt16 *faces)
{
	UInt32 numFaces = getNumFaces();

	//set the neighbor indices to be -1
	for (UInt32 i = 0; i < m_numElements; ++i)
		m_neighborIndices[i] = -1;

	int numEdges = 0;

	//loop through triangles
	for (UInt32 i = 0; i < numFaces-1; ++i)
	{
		//loop through edges on the first triangle
		for (UInt32 edgeI = 0; edgeI < 3; ++edgeI)
		{
			//continue if this edge already has a neighbor set
			if (m_neighborIndices[i*3+edgeI] != -1)
				continue;

			//loop through triangles with greater indices than this one
			for (UInt32 j = i+1; j < numFaces; ++j)
			{
				//loop through edges on triangle j
				for (UInt32 edgeJ = 0; edgeJ < 3; ++edgeJ)
				{
					//get the vertex indices on each edge
					UInt32 edgeI1 = faces[i*3+edgeI];
					UInt32 edgeI2 = faces[i*3+(edgeI+1)%3];
					UInt32 edgeJ1 = faces[j*3+edgeJ]; // crash here on valgrind
					UInt32 edgeJ2 = faces[j*3+(edgeJ+1)%3];

					//if these are the same (possibly reversed order), these faces are neighbors
					if ((edgeI1==edgeJ1 && edgeI2==edgeJ2) ||
						(edgeI1==edgeJ2 && edgeI2==edgeJ1))
					{
						m_neighborIndices[i*3+edgeI] = j;
						m_neighborIndices[j*3+edgeJ] = i;

						numEdges++;
					}
				}
			}
		}
	}
}*/

//---------------------------------------------------------------------------------------
// class FaceArrayUInt32
//---------------------------------------------------------------------------------------

// Default constructor, no parameters.
FaceArrayUInt32::FaceArrayUInt32(
    Context *context,
	PrimitiveFormat format,
	VertexBuffer::Storage storage) :
		FaceArray(format,storage),
        m_vbo(context)
{
}

// Constructor taking a smart array of faces indices.
FaceArrayUInt32::FaceArrayUInt32(
        Context *context,
        PrimitiveFormat format,
        const SmartArrayUInt32 &faces,
        Int32 minIndex,
        Int32 maxIndex,
    VertexBuffer::Storage storage) :
		FaceArray(format,storage),
        m_vbo(context)
{
	setFaces(faces, minIndex, maxIndex);
}

// Set the vertices indices.
void FaceArrayUInt32::setFaces(
	const SmartArrayUInt32 &faces,
	Int32 minIndex,
	Int32 maxIndex)
{
	m_faces = faces;
	m_numElements = faces.getNumElt();

	if ((minIndex < 0) || (maxIndex < 0))
		computeMinMax(m_faces.getData(), m_faces.getNumElt());
	else
	{
		m_minVertex = minIndex;
		m_maxVertex = maxIndex;
	}

	m_isDirty = True;
}

// Create the data for GPU
Bool FaceArrayUInt32::create()
{
	if (!m_isDirty)
		return True;

	if (!m_faces.getData())
	{
		m_minVertex = m_maxVertex = 0;
		m_numElements = 0;
		m_faces.releaseCheckAndDelete();
		m_vbo.release();

	//	O3D_ERROR(O3D_E_InvalidParameter("Data array must be defined"));
		return True;
	}

	m_numElements = m_faces.getNumElt();

	if (m_minVertex > m_maxVertex)
		O3D_ERROR(E_InvalidParameter("Minimal vertex index must be lesser than maximal vertex index"));

	//ComputeEdgeInt(m_faces.getData());

	m_vbo.create(m_faces.getNumElt(), m_storage, m_faces.getData());
	m_faces.releaseCheckAndDelete();

	if (m_vbo.isExist())
	{
		m_isDirty = False;
		return True;
	}
	else
		return False;
}

// Create the data for GPU according to the specified number of elements.
Bool FaceArrayUInt32::create(UInt32 numElt)
{
	m_vbo.create(numElt, m_storage, NULL);
	m_faces.releaseCheckAndDelete(); // a previous set...

	m_numElements = numElt;

	if (m_vbo.isExist())
	{
		m_isDirty = False;
		return True;
	}
	else
		return False;
}

// Directly set and create from a data array.
Bool FaceArrayUInt32::create(
		const UInt32 *faces,
		UInt32 numElt,
		Int32 minIndex,
		Int32 maxIndex)
{
	O3D_ASSERT(numElt > 0);

	if (!faces)
		O3D_ERROR(E_InvalidParameter("Data array must be defined"));
	if (!numElt)
		O3D_ERROR(E_InvalidParameter("Number of face an indices (elements) must be greater than zero"));

	if (minIndex > maxIndex)
		O3D_ERROR(E_InvalidParameter("Minimal vertex index must be lesser than maximal vertex index"));

	m_numElements = numElt;

	// Compute the min/max vertex indices if necessary
	if ((minIndex < 0) || (maxIndex < 0))
		computeMinMax(faces, numElt);
	else
	{
		m_minVertex = minIndex;
		m_maxVertex = maxIndex;
	}

	//ComputeEdgeInt(faces);

	m_vbo.create(numElt, m_storage, faces);
	m_faces.releaseCheckAndDelete(); // a previous set...

	if (m_vbo.isExist())
	{
		m_isDirty = False;
		return True;
	}
	else
		return False;
}

// Directly update a part from a data array.
void FaceArrayUInt32::update(
		const UInt32 *faces,
		UInt32 offset,
		UInt32 numElt)
{
	O3D_ASSERT(numElt > 0);

	if (!m_vbo.isExist())
		O3D_ERROR(E_InvalidPrecondition("Must be previously created"));

	if (!faces)
		O3D_ERROR(E_InvalidParameter("Data array must be defined"));
	if (!numElt)
		O3D_ERROR(E_InvalidParameter("Number of face an indices (elements) must be greater than zero"));

	if (numElt+offset > m_numElements)
		O3D_ERROR(E_InvalidParameter("Number of face to update must be lesser than current number of elements"));

	if (offset >= m_numElements)
		O3D_ERROR(E_InvalidParameter("Offset must be lesser than current number of elements"));

	m_vbo.update(faces, offset, numElt);
	m_isDirty = False;
}

// Lock the data array once it is initialized into VBO
UInt8* FaceArrayUInt32::lockArray(
		UInt32 offset,
		UInt32 size,
		VertexBuffer::LockMode flags)
{
	if (m_faces.getData())
		return reinterpret_cast<UInt8*>(m_faces.getData() + offset);
	else if (m_vbo.isExist())
		return reinterpret_cast<UInt8*>(m_vbo.lock(offset, size, flags));
	else
        return nullptr;
}

// Unlock the data array
void FaceArrayUInt32::unlockArray()
{
	if (m_vbo.isExist() && m_vbo.isLocked())
		m_vbo.unlock();
}

// Get memory size
UInt32 FaceArrayUInt32::getCapacity() const
{
	if (m_faces.isValid())
		return m_faces.getNumElt() << 2;
	else if (m_vbo.isExist())
		return m_vbo.getCount() << 2;
	else
		return 0;
}

// Bind the data array 
void FaceArrayUInt32::bindArray()
{
	if (m_vbo.isExist())
		return m_vbo.bindBuffer();
}

// Draw using the data array
void FaceArrayUInt32::draw(Scene *scene)
{
	if (m_isDirty)
		return;

	O3D_ASSERT(scene);

	if (m_vbo.isExist())
	{
		UInt32 numElements = m_vbo.getCount() - m_offset;

		m_vbo.bindBuffer();
		scene->drawRangeElementsUInt32(
				m_format,
				m_minVertex,
				m_maxVertex,
				numElements,
				reinterpret_cast<UInt32*>(m_offset * sizeof(UInt32)));
	}
}

// Draw using the data array, and with a specified range.
void FaceArrayUInt32::drawPart(Scene *scene, UInt32 firstIndex, UInt32 lastIndex)
{
	if (m_isDirty)
		return;

	O3D_ASSERT(scene);

	if (m_vbo.isExist())
	{
		UInt32 numElements = lastIndex - firstIndex;

		m_vbo.bindBuffer();
		scene->drawRangeElementsUInt32(
				m_format,
				m_minVertex,
				m_maxVertex,
				numElements,
				reinterpret_cast<UInt32*>(firstIndex * sizeof(UInt32)));
	}
}

Bool FaceArrayUInt32::writeToFile(OutStream &os)
{
    FaceArray::writeToFile(os);

	if (m_faces.isValid())
	{
        os << m_faces;
	}
	else if (m_vbo.isExist())
	{
		UInt32 *data = m_vbo.lock(0, 0, VertexBuffer::READ_ONLY);
		UInt32 count = m_vbo.getCount();

        os << count;
        os.write(data, count);

		m_vbo.unlock();
	}
	else
	{
		O3D_ERROR(E_InvalidPrecondition("The VBO or the data array must be valid"));
	}

	return True;
}

Bool FaceArrayUInt32::readFromFile(InStream &is)
{
    FaceArray::readFromFile(is);

    is >> m_faces;

	return True;
}

void FaceArrayUInt32::computeMinMax(const UInt32 *data, UInt32 numElt)
{
	m_minVertex = o3d::Limits<UInt32>::max();
	m_maxVertex = o3d::Limits<UInt32>::min();

	for (UInt32 i = 0; i < numElt; ++i)
	{
		m_minVertex = o3d::min(m_minVertex, data[i]);
		m_maxVertex = o3d::max(m_maxVertex, data[i]);
	}
}

/*void FaceArrayUInt32::computeEdgeInt(const UInt32 *faces)
{
	UInt32 numFaces = getNumFaces();

	//set the neighbor indices to be -1
	for (UInt32 i = 0; i < m_numElements; ++i)
		m_neighborIndices[i] = -1;

	//loop through triangles
	for (UInt32 i = 0; i < numFaces-1; ++i)
	{
		//loop through edges on the first triangle
		for (UInt32 edgeI = 0; edgeI < 3; ++edgeI)
		{
			//continue if this edge already has a neighbor set
			if (m_neighborIndices[i*3+edgeI] != -1)
				continue;

			//loop through triangles with greater indices than this one
			for (UInt32 j = i+1; j < numFaces; ++j)
			{
				//loop through edges on triangle j
				for (UInt32 edgeJ = 0; edgeJ < 3; ++edgeJ)
				{
					//get the vertex indices on each edge
					UInt32 edgeI1 = faces[i*3+edgeI];
					UInt32 edgeI2 = faces[i*3+(edgeI+1)%3];
					UInt32 edgeJ1 = faces[j*3+edgeJ];
					UInt32 edgeJ2 = faces[j*3+(edgeJ+1)%3];

					//if these are the same (possibly reversed order), these faces are neighbors
					if ((edgeI1==edgeJ1 && edgeI2==edgeJ2) ||
						(edgeI1==edgeJ2 && edgeI2==edgeJ1))
					{
						m_neighborIndices[i*3+edgeI] = j;
						m_neighborIndices[j*3+edgeJ] = i;
					}
				}
			}
		}
	}
}*/

