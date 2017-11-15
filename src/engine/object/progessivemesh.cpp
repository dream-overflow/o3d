/**
 * @file progessivemesh.cpp
 * @brief Progressive mesh data (LOD)
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 24-03-2006
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/progressivemesh.h"

using namespace o3d;

//---------------------------------------------------------------------------------------
// class O3DLODTriangle
//-------------------------------------------------------------------------------------
// Default constructor.
//---------------------------------------------------------------------------------------
LODTriangle::LODTriangle(LODVertex *v0, LODVertex *v1, LODVertex *v2)
{
	vertex[0] = v0;
	vertex[1] = v1;
	vertex[2] = v2;

	computeNormal();

	for (Int32 i = 0; i < 3; i++)
	{
		vertex[i]->face.push(this);

		for (Int32 j = 0; j < 3; j++)
		{
			if (i != j)
			{
				vertex[i]->neighbor.addUnique(vertex[j]);
			}
		}
	}
}

// Destructor.
LODTriangle::~LODTriangle()
{
	for (Int32 i = 0; i < 3; i++)
	{
		if (vertex[i])
			vertex[i]->face.remove(this);
	}

	for (Int32 i = 0; i < 3; i++)
	{
		Int32 i2 = (i + 1) % 3;

		if (!vertex[i] || !vertex[i2])
			continue;

		vertex[i]->removeIfNonNeighbor(vertex[i2]);
		vertex[i2]->removeIfNonNeighbor(vertex[i]);
	}
}

/*---------------------------------------------------------------------------------------
  check for a vertex in this face
---------------------------------------------------------------------------------------*/
Int32 LODTriangle::hasVertex(LODVertex *v)
{
	return ((v == vertex[0]) || (v == vertex[1]) || (v == vertex[2]));
}

/*---------------------------------------------------------------------------------------
  compute the face normal
---------------------------------------------------------------------------------------*/
void LODTriangle::computeNormal()
{
	Vector3 vVector1 = vertex[1]->position - vertex[0]->position;
	Vector3 vVector2 = vertex[2]->position - vertex[1]->position;

	normal = vVector1 ^ vVector2;
	normal = !normal;
}

/*---------------------------------------------------------------------------------------
  replace a vertex of this face
---------------------------------------------------------------------------------------*/
void LODTriangle::replaceVertex(LODVertex *vold, LODVertex *vnew)
{
	O3D_ASSERT(vold && vnew);
	O3D_ASSERT((vold == vertex[0]) || (vold == vertex[1]) || (vold == vertex[2]));
	O3D_ASSERT((vnew != vertex[0]) && (vnew != vertex[1]) && (vnew != vertex[2]));

	if (vold == vertex[0])
	{
		vertex[0] = vnew;
	}
	else if (vold == vertex[1])
	{
		vertex[1] = vnew;
	}
	else
	{
		O3D_ASSERT(vold == vertex[2]);
		vertex[2] = vnew;
	}

	Int32 i;

	vold->face.remove(this);

	O3D_ASSERT(vnew->face.find(this) == -1);

	vnew->face.push(this);

	for (i = 0 ; i < 3 ; i++)
	{
		vold->removeIfNonNeighbor(vertex[i]);
		vertex[i]->removeIfNonNeighbor(vold);
	}

	for (i = 0 ; i < 3 ; i++)
	{
		O3D_ASSERT(vertex[i]->face.find(this) != -1);

		for (Int32 j = 0;  j < 3 ; j++)
		{
			if (i != j)
			{
				vertex[i]->neighbor.addUnique(vertex[j]);
			}
		}
	}

	computeNormal();
}

//---------------------------------------------------------------------------------------
// class LODVertex
//-------------------------------------------------------------------------------------
// constructors
//---------------------------------------------------------------------------------------
LODVertex::LODVertex(const Vector3& v, Int32 VertexID)
{
	position = v;
	id = VertexID;
}

LODVertex::LODVertex(Float *vec, Int32 VertexID)
{
	position.set(vec[0], vec[1], vec[2]);
	id = VertexID;
}

LODVertex::LODVertex(Float x, Float y, Float z, Int32 VertexID)
{
	position.set(x, y, z);
	id = VertexID;
}

// destructor
LODVertex::~LODVertex()
{
	//O3D_ASSERT(face.getSize() == 0);

	while (neighbor.getSize())
	{
		neighbor.get(0)->neighbor.remove(this);
		neighbor.remove(neighbor.get(0));
	}
}

// Return if this vertex is on a border.
Int32 LODVertex::isBorder()
{
	Int32 i, j;

	for (i = 0; i < neighbor.getSize(); i++)
	{
		Int32 count = 0;

		for (j = 0; j < face.getSize(); j++)
		{
			if (face[j]->hasVertex(neighbor[i]))
			{
				count++;
			}
		}

		//O3D_ASSERT(count > 0);

		if (count == 1)
			return 1;
	}

	return 0;
}

// Remove neightbour when there are no more valid.
void LODVertex::removeIfNonNeighbor(LODVertex *n)
{
	// removes n from neighbor list if n isn't a neighbor.
	if (neighbor.find(n) == -1)
		return;

	for (Int32 i = 0 ; i < face.getSize() ; i++)
	{
		if (face.get(i)->hasVertex(n))
			return;
	}

	neighbor.remove(n);
}

//---------------------------------------------------------------------------------------
// class ProgressiveMesh
//-------------------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------------------
ProgressiveMesh::ProgressiveMesh()
{
	m_vertices = NULL;
	m_triangles = NULL;
}

// Destructor
ProgressiveMesh::~ProgressiveMesh()
{
	Int32 i;

	if (m_vertices)
	{
		for (i = 0; i < m_vertices->getSize(); i++)
			deletePtr(m_vertices->get(i));

		deletePtr(m_vertices);
	}

	if (m_triangles)
	{
		for (i = 0; i < m_triangles->getSize(); i++)
			deletePtr(m_triangles->get(i));

		deletePtr(m_triangles);
	}
}

//---------------------------------------------------------------------------------------
// On va génerer des listes de faces. La fonction qui va calculer les différents niveaux
// de LOD va simplifier le même objet en générant une liste de face. De ce fait, seul la
// structure face va etre modifiée. Dans la structure vertex on ne va que modifier les
// valeurs nécessaires au calcul pour le LOD.
//---------------------------------------------------------------------------------------
Float ProgressiveMesh::computeEdgeCollapseCost(LODVertex *u, LODVertex *v)
{
	// if we collapse edge uv by moving u to v then how
	// much different will the model change, i.e. how much "error".
	// Texture, vertex normal, and border vertex code was removed
	// to keep this demo as simple as possible.
	// The method of determining cost was designed in order
	// to exploit small and coplanar regions for
	// effective polygon reduction.
	// Is is possible to add some checks here to see if "folds"
	// would be generated.  i.e. normal of a remaining face gets
	// flipped.  I never seemed to run into this problem and
	// therefore never added code to detect this case.

	Int32 i;
	Vector3 temp;
	temp = v->position - u->position;
	Float edgelength = temp.length();

	if (v == u)
		O3D_ERROR(E_InvalidParameter("Two vertices are similar"));

	Float curvature = 0;

	// find the "sides" triangles that are on the edge uv
	TemplateArray<LODTriangle*> sides(u->face.getSize(), 2);

	for (i = 0; i < u->face.getSize(); i++)
	{
		if (u->face.get(i)->hasVertex(v))
		{
			sides.push(u->face[i]);
		}
	}

	// use the triangle facing most away from the sides
	// to determine our curvature term
	for (i = 0; i < u->face.getSize(); i++)
	{
		Float mincurv = 1; // curve for face i and closer side to it

		for (Int32 j = 0; j < sides.getSize(); j++)
		{
			// use dot product of face normals. '^' defined in vector
			Float dotprod = u->face[i]->normal * sides[j]->normal;

			//Float dotprod = DotProd(u->face[i]->normal, sides[j]->normal);
			mincurv = o3d::min(mincurv, (1.002f - dotprod) / 2.0f);
		}

		curvature = o3d::max(curvature, mincurv);
	}

	// check for border to interior collapses
	if (u->isBorder() && sides.getSize() > 1)
	{
		curvature = 1;
	}

	// the more coplanar the lower the curvature term
	return edgelength * curvature;
}

void ProgressiveMesh::computeEdgeCostAtVertex(LODVertex *v)
{
	// compute the edge collapse cost for all edges that start
	// from vertex v.  Since we are only interested in reducing
	// the object by selecting the min cost edge at each step, we
	// only cache the cost of the least cost edge at this vertex
	// (in member variable collapse) as well as the value of the
	// cost (in member variable objdist).
	if (v->neighbor.getSize() == 0)
	{
		// v doesn't have neighbors so it costs nothing to collapse

		v->collapse = NULL;

		return;
	}

	v->objdist = 1000000;
	v->collapse = NULL;

	// search all neighboring edges for "least cost" edge
	for (Int32 i = 0; i < v->neighbor.getSize(); i++)
	{
		Float dist;

		dist = computeEdgeCollapseCost(v, v->neighbor[i]);

		if (dist < v->objdist)
		{

			v->collapse = v->neighbor[i]; // candidate for edge collapse
			v->objdist = dist; // cost of the collapse
		}
	}
}

void ProgressiveMesh::computeAllEdgeCollapseCosts()
{
	// For all the edges, compute the difference it would make
	// to the model if it was collapsed.  The least of these
	// per vertex is cached in each vertex object.
	for (Int32 i = 0; i < m_vertices->getSize(); i++)
	{
		computeEdgeCostAtVertex(m_vertices->get(i));
	}
}

void ProgressiveMesh::collapse(LODVertex *u, LODVertex *v)
{
	// Collapse the edge uv by moving vertex u onto v
	// Actually remove tris on uv, then update tris that
	// have u to have v, and then remove u.
	if (!v)
	{
		// u is a vertex all by itself so just delete it
		deletePtr(u);
		m_vertices->remove(u);

		return;
	}

	Int32 i;
	TemplateArray<LODVertex*> tmp(u->neighbor.getSize(), 2);

	// make tmp a list of all the neighbors of u
	for (i = 0; i < u->neighbor.getSize(); i++)
	{
		tmp.push(u->neighbor[i]);
	}

	// delete triangles on edge uv:
	for (i = u->face.getSize() - 1; i >= 0; i--)
	{
		if (u->face[i]->hasVertex(v))
		{
			LODTriangle *pFace = u->face[i];
			m_triangles->remove(pFace);

			deletePtr(pFace);
		}
	}

	// update remaining triangles to have v instead of u
	for (i = u->face.getSize() - 1; i >= 0; i--)
	{
		u->face[i]->replaceVertex(u, v);
	}

	deletePtr(u);
	m_vertices->remove(u);

	// recompute the edge collapse costs for neighboring vertices
	for (i = 0; i < tmp.getSize(); i++)
	{
		computeEdgeCostAtVertex(tmp.get(i));
	}
}

LODVertex *ProgressiveMesh::minimumCostEdge()
{
	// Find the edge that when collapsed will affect model the least.
	// This function actually returns a Vertex, the second vertex
	// of the edge (collapse candidate) is stored in the vertex data.
	// Serious optimization opportunity here: this function currently
	// does a sequential search through an unsorted list :-(
	// Our algorithm could be O(n*log(n)) instead of O(n*n)
	LODVertex *mn = m_vertices->get(0);

	for (Int32 i = 0; i < m_vertices->getSize(); i++)
	{
		if (m_vertices->get(i)->objdist < mn->objdist)
		{
			mn = m_vertices->get(i);
		}
	}
	return mn;
}

void ProgressiveMesh::progressiveMesh(
		const Float *vertexList,
		UInt32 numVertices,
		const UInt32 *faceList,
		UInt32 numFaces,
		ArrayUInt32 &collapseMap,
		ArrayUInt32 &permutation)
{
	m_numVertices = numVertices;
	m_numFaces = numFaces;

	m_vertices = new TemplateArray<LODVertex*>(m_numVertices, 2);
	m_triangles = new TemplateArray<LODTriangle*>(m_numFaces, 2);

	UInt32 i;

	collapseMap.setSize(numVertices);
	permutation.setSize(numVertices);

	// fill vertices...
	for (i = 0; i < m_numVertices; i++)
	{
		LODVertex *v = new LODVertex(vertexList, i);
		m_vertices->push(v);
		vertexList += 3;
	}

	// and triangles lists
	for (i = 0; i < m_numFaces; i++)
	{
		LODTriangle *t = new LODTriangle(
				m_vertices->get(faceList[0]),
				m_vertices->get(faceList[1]),
				m_vertices->get(faceList[2]));

		m_triangles->push(t);
		faceList += 3;
	}

	// cache all edge collapse costs
	computeAllEdgeCollapseCosts();

	// reduce the object down to nothing:
	while (m_vertices->getSize() > 0)
	{
		// get the next vertex to collapse
		LODVertex *mn = minimumCostEdge();
		// keep track of this vertex, i.e. the collapse ordering
		permutation[mn->id] = static_cast<UInt16>(m_vertices->getSize() - 1);

		// keep track of vertex to which we collapse to
		collapseMap[m_vertices->getSize() - 1] =
				mn->collapse ? static_cast<UInt16>(mn->collapse->id) : o3d::Limits<UInt16>::max();

		// Collapse this edge
		collapse(mn, mn->collapse);
	}

	// reorder the map list based on the collapse ordering
	for (i = 0; i < m_numVertices; i++)
	{
		collapseMap[i] = (collapseMap[i] == o3d::Limits<UInt16>::max()) ? 0 : permutation[collapseMap[i]];
	}
}

void ProgressiveMesh::progressiveMesh(
		const Float *vertexList,
		UInt32 numVertices,
		const UInt16 *faceList,
		UInt32 numFaces,
		ArrayUInt16 &collapseMap,
		ArrayUInt16 &permutation)
{
	m_numVertices = numVertices;
	m_numFaces = numFaces;

	m_vertices = new TemplateArray<LODVertex*>(m_numVertices, 2);
	m_triangles = new TemplateArray<LODTriangle*>(m_numFaces, 2);

	UInt32 i;

	collapseMap.setSize(numVertices);
	permutation.setSize(numVertices);

	// fill vertices...
	for (i = 0; i < m_numVertices; i++)
	{
		LODVertex *v = new LODVertex(vertexList, i);
		m_vertices->push(v);
		vertexList += 3;
	}

	// and triangles lists
	for (i = 0; i < m_numFaces; i++)
	{
		LODTriangle *t = new LODTriangle(
				m_vertices->get(faceList[0]),
				m_vertices->get(faceList[1]),
				m_vertices->get(faceList[2]));

		m_triangles->push(t);
		faceList += 3;
	}

	// cache all edge collapse costs
	computeAllEdgeCollapseCosts();

	// reduce the object down to nothing:
	while (m_vertices->getSize() > 0)
	{
		// get the next vertex to collapse
		LODVertex *mn = minimumCostEdge();
		// keep track of this vertex, i.e. the collapse ordering
		permutation[mn->id] = static_cast<UInt32>(m_vertices->getSize() - 1);

		// keep track of vertex to which we collapse to
		collapseMap[m_vertices->getSize() - 1] =
				mn->collapse ? static_cast<UInt16>(mn->collapse->id) : o3d::Limits<UInt32>::max();

		// Collapse this edge
		collapse(mn, mn->collapse);
	}

	// reorder the map list based on the collapse ordering
	for (i = 0; i < m_numVertices; i++)
	{
		collapseMap[i] = (collapseMap[i] == o3d::Limits<UInt32>::max()) ? 0 : permutation[collapseMap[i]];
	}
}

