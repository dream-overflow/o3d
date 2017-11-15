/**
 * @file edgearray.h
 * @brief Edge array mostly contained by a face array.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_EDGEARRAY_H
#define _O3D_EDGEARRAY_H

#include "o3d/core/smartarray.h"
#include "o3d/engine/vertexbuffer.h"
#include "o3d/core/memorydbg.h"

#include <vector>

namespace o3d {

class VertexElement;
class VertexBlend;
class FaceArrayVisitor;

//---------------------------------------------------------------------------------------
//! @class EdgeArray
//-------------------------------------------------------------------------------------
//! Edge array mostly contained by a face array. Edge are compute given a faces array, 
//! and vertices.
//---------------------------------------------------------------------------------------
class O3D_API EdgeArray : NonCopyable<>
{
public:

	//! Define an edge record.
	struct Edge
	{
		UInt32 vertexIndex[2];    //!< The two vertices indices that define the edge.
		UInt32 triangleIndex[2];  //!< The two triangles indices that share the edge.
		//Int32 triangleIndex[2];   //!< The two triangles indices that share the edge.
	};

	//! Default constructor.
	EdgeArray(VertexBuffer::Storage storage = VertexBuffer::STATIC);

	//! Destructor.
	~EdgeArray();

	//! Set the data storage type.
	inline void SetStorageType(VertexBuffer::Storage storage)
	{
		if (storage != m_storage)
		{
			m_storage = storage;
			m_isDirty = True;
		}
	}

	//! Get the data storage type.
	inline VertexBuffer::Storage getStorageType() const { return m_storage; }

	//! Create the GUP data.
	Bool create();

	//! Is the data are created (Create called, and system RAM = NULL if VBO used)
	inline Bool isCreated()const { return !m_isDirty; }

	//! Build the edge from an O3DFaceArray object.
	//! @param faceArray A valid face array consisting of triangles/fans/strip.
	//! @param numVertices Number of vertices.
	//! @return The number of computed edges.
	UInt32 buildEdges(FaceArrayVisitor &faceTriangulate, UInt32 numVertices);

	//! Get the number of edges.
	inline UInt32 getNumEdges() const { return m_numEdges; }

	//! Get the edge list.
	inline const Edge* getEdges() const { return m_edges; }

protected:

	VertexBuffer::Storage m_storage; //!< Memory storage of the faces.

	Edge *m_edges;                   //!< Contain the compute edges list.
	UInt32 m_numEdges;           //!< Number of edges.

	Bool m_isDirty;              //!< TRUE mean a call to create is necessary.
};

} // namespace o3d

#endif // _O3D_EDGEARRAY_H

