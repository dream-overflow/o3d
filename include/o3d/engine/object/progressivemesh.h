/**
 * @file progressivemesh.h
 * @brief Progressive mesh data computation
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2006-03-24
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PROGRESSIVEMESH_H
#define _O3D_PROGRESSIVEMESH_H

#include "o3d/core/memorydbg.h"
#include "o3d/core/templatearray.h"
#include "o3d/core/math.h"
#include "o3d/core/vector3.h"

namespace o3d {

class LODTriangle;
class LODVertex;

//---------------------------------------------------------------------------------------
//! @class LODTriangle
//-------------------------------------------------------------------------------------
//! Progressive mesh triangle container.
//---------------------------------------------------------------------------------------
class LODTriangle
{
public:

	LODVertex *vertex[3];  //!< 3 vertices defines the face.
	Vector3 normal;        //!< Normals of the face.

	//! Default constructor.
	LODTriangle(LODVertex *v0, LODVertex *v1, LODVertex *v2);

	//! Destructor.
	~LODTriangle();

	//! Compute the face normal.
	void computeNormal();

	//! Replace a vertex of this face.
	void replaceVertex(LODVertex *vold,LODVertex *vnew);

	//! Check for a vertex in this face.
	Int32 hasVertex(LODVertex *v);
};


//---------------------------------------------------------------------------------------
//! @class LODVertex
//-------------------------------------------------------------------------------------
//! Progressive mesh vertex container.
//---------------------------------------------------------------------------------------
class LODVertex
{
public:

	Vector3 position;   //!< position of this vertex (space object)
	Int32 id;          //!< indice of vertex in original list

	TemplateArray<LODVertex*>   neighbor;  //!< neighbor vertices
	TemplateArray<LODTriangle*> face;      //!< neighbor faces

	Float objdist;       //!< Cost if we collapse this vertex

	LODVertex *collapse;  //!< Vertex candidate for collapse it

	//! Constructor.
	LODVertex(const Vector3& v, Int32 vertexID);

	//! Constructor.
	LODVertex(Float *v, Int32 vertexID);

	//! Constructor.
	LODVertex(Float x, Float y, Float z, Int32 vertexID);

	//! Destructor.
	~LODVertex();

	//! Return if this vertex is on a border.
	Int32 isBorder();

	//! Remove neighbor when there are no more valid.
	void removeIfNonNeighbor(LODVertex *n);
};


//---------------------------------------------------------------------------------------
//! @class ProgressiveMesh
//-------------------------------------------------------------------------------------
//! Progressive mesh data for mesh level of detail (LOD).
//---------------------------------------------------------------------------------------
class O3D_API ProgressiveMesh
{
public:

	//! Default constructor.
	ProgressiveMesh();
	//! Destructor.
	~ProgressiveMesh();

	//! Collapse map computation for UINT32 face list.
	void progressiveMesh(
			const Float *vertexList,
			UInt32 numVertices,
			const UInt32 *faceList,
			UInt32 numFaces,
			ArrayUInt32 &map,
			ArrayUInt32 &permutation);

	//! Collapse map computation for UINT16 face list.
	void progressiveMesh(
			const Float *vertexList,
			UInt32 numVertices,
			const UInt16 *faceList,
			UInt32 numFaces,
			ArrayUInt16 &map,
			ArrayUInt16 &permutation);

protected:

	TemplateArray<LODVertex*>   *m_vertices;
	TemplateArray<LODTriangle*> *m_triangles;

	UInt32 m_numFaces;
	UInt32 m_numVertices;

	//! LOD mesh computation
	Float computeEdgeCollapseCost(LODVertex *u, LODVertex *v);

	void computeEdgeCostAtVertex(LODVertex *v);
	void computeAllEdgeCollapseCosts();
	void collapse(LODVertex *u,LODVertex *v);

	LODVertex *minimumCostEdge();
};

} // namespace o3d

#endif // _O3D_PROGRESSIVEMESH_H

