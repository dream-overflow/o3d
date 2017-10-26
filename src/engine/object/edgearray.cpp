/**
 * @file edgearray.cpp
 * @brief Implementation of EdgeArray.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-11-19
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/edgearray.h"

#include "o3d/engine/object/vertexelement.h"
#include "o3d/engine/object/facearrayvisitor.h"

using namespace o3d;

// Default constructor.
EdgeArray::EdgeArray(VertexBuffer::Storage storage) :
	m_storage(storage),
	m_edges(NULL),
	m_isDirty(True)
{
}

// Destructor
EdgeArray::~EdgeArray()
{
	deletePtr(m_edges);
}

// Create the GPU data.
Bool EdgeArray::create()
{
	return False;
}

UInt32 EdgeArray::buildEdges(
		FaceArrayVisitor &triangles,
		UInt32 numVertices)
{
	if (triangles.getNumTriangles() == 0)
		return 0;

	UInt32 edgeCount = 0;

	Int32 numTriangles = static_cast<Int32> (triangles.getNumTriangles());

	deletePtr(m_edges);
	m_edges = new Edge[numTriangles * 3];
/*
	UInt32 maxEdgeCount = numTriangles * 3;
	UInt32 *firstEdge = new UInt32[numVertices + maxEdgeCount];
	UInt32 *nextEdge = firstEdge + numVertices;

	memset(firstEdge, 0xff, sizeof(UInt32) * numVertices);

	// First pass over all triangles. This finds all the edges satisfying the
	// condition that the first vertex index is less than the second vertex index
	// when the direction from the first vertex to the second vertex represents
	// a counterclockwise winding around the triangle to which the edge belongs.
	// For each edge found, the edge index is stored in a linked list of edges
	// belonging to the lower-numbered vertex index i. This allows us to quickly
	// find an edge in the second pass whose higher-numbered vertex index is i.

	for (O3DFaceArrayIterator it = triangles.begin(); it != triangles.end(); ++it)
	{
		UInt32 i1 = it.c;
		for (UInt32 b = 0; b < 3; b++)
		{
			UInt32 i2 = it.v[b];
			if (i1 < i2)
			{
				Edge *edge = &m_edges[edgeCount];

				edge->vertexIndex[0] = i1;
				edge->vertexIndex[1] = i2;
				edge->triangleIndex[0] = it.a;
				edge->triangleIndex[1] = it.a;

				UInt32 edgeIndex = firstEdge[i1];
				if (edgeIndex == 0xffffffff)
				{
					firstEdge[i1] = edgeCount;
				}
				else
				{
					for (;;)
					{
						UInt32 index = nextEdge[edgeIndex];
						if (index == 0xffffffff)
						{
							nextEdge[edgeIndex] = edgeCount;
							break;
						}

						edgeIndex = index;
					}
				}

				nextEdge[edgeCount] = 0xffffffff;
				edgeCount++;
			}

			i1 = i2;
		}
	}

	// Second pass over all triangles. This finds all the edges satisfying the
	// condition that the first vertex index is greater than the second vertex index
	// when the direction from the first vertex to the second vertex represents
	// a counterclockwise winding around the triangle to which the edge belongs.
	// For each of these edges, the same edge should have already been found in
	// the first pass for a different triangle. So we search the list of edges
	// for the higher-numbered vertex index for the matching edge and fill in the
	// second triangle index. The maximum number of comparisons in this search for
	// any vertex is the number of edges having that vertex as an endpoint.

	for (O3DFaceArrayIterator it = triangles.begin(); it != triangles.end(); ++it)
	{
		UInt32 i1 = it.c;
		for (UInt32 b = 0; b < 3; b++)
		{
			UInt32 i2 = it.v[b];
			if (i1 > i2)
			{
				for (UInt32 edgeIndex = firstEdge[i2];
						edgeIndex != 0xffffffff;
						edgeIndex = nextEdge[edgeIndex])
				{
					Edge *edge = &m_edges[edgeIndex];

					if ((edge->vertexIndex[1] == i1) && (edge->triangleIndex[0] == edge->triangleIndex[1]))
					{
						edge->triangleIndex[1] = it.id;
						break;
					}
				}
			}

			i1 = i2;
		}
	}

	deleteArray(firstEdge);
*/
	Edge *edge = m_edges;
	// first pass: find edges
	for (FaceArrayIterator it = triangles.begin(); it != triangles.end(); ++it)
	{
		if (it.a < it.b)
		{
			edge->vertexIndex[0] = it.a;
			edge->vertexIndex[1] = it.b;
			edge->triangleIndex[0] = it.id;
			edge->triangleIndex[1] = it.id;
			++edgeCount;
			++edge;
		}

		if (it.b < it.c)
		{
			edge->vertexIndex[0] = it.b;
			edge->vertexIndex[1] = it.c;
			edge->triangleIndex[0] = it.id;
			edge->triangleIndex[1] = it.id;
			++edgeCount;
			++edge;
		}

		if (it.c < it.a)
		{
			edge->vertexIndex[0] = it.c;
			edge->vertexIndex[1] = it.a;
			edge->triangleIndex[0] = it.id;
			edge->triangleIndex[1] = it.id;
			++edgeCount;
			++edge;
		}
	}

	// second pass: match triangles to edges
	for (FaceArrayIterator it = triangles.begin(); it != triangles.end(); ++it)
	{
		if (it.a > it.b)
		{
			edge = m_edges;
			for (UInt32 i = 0; i < edgeCount; ++i)
			{
				if ((edge->vertexIndex[0] == it.b) && (edge->vertexIndex[1] == it.a)
						&& (edge->triangleIndex[1] == edge->triangleIndex[0]))
				{
					edge->triangleIndex[1] = it.id;
					break;
				}

				++edge;
			}
		}

		if (it.b > it.c)
		{
			edge = m_edges;
			for (UInt32 i = 0; i < edgeCount; ++i)
			{
				if ((edge->vertexIndex[0] == it.c) && (edge->vertexIndex[1] == it.b)
						&& (edge->triangleIndex[1] == edge->triangleIndex[0]))
				{
					edge->triangleIndex[1] = it.id;
					break;
				}

				++edge;
			}
		}

		if (it.c > it.a)
		{
			edge = m_edges;
			for (UInt32 i = 0; i < edgeCount; ++i)
			{
				if ((edge->vertexIndex[0] == it.a) && (edge->vertexIndex[1] == it.c)
						&& (edge->triangleIndex[1] == edge->triangleIndex[0]))
				{
					edge->triangleIndex[1] = it.id;
					break;
				}

				++edge;
			}
		}
	}

	m_numEdges = edgeCount;
	return edgeCount;
}

