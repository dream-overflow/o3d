/**
 * @file primitive.cpp
 * @brief Implementation of Primitive.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @author  Julien IBARZ
 * @date 2003-02-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/primitive.h"

#include "o3d/core/vector3.h"
#include "o3d/core/debug.h"
#include "o3d/engine/glextensionmanager.h"

#include <math.h>

using namespace o3d;

//---------------------------------------------------------------------------------------
// Primitive
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_ABSTRACT_CLASS1(Primitive, ENGINE_PRIMITIVE, BaseObject)

Primitive::Primitive(UInt32 flags) :
    BaseObject(nullptr),
		m_capacities(flags),
		m_verticesCount(0),
        m_pVertices(nullptr),
		m_indicesCount(0),
        m_pIndices(nullptr),
        m_pTexCoords(nullptr)
{
}

Primitive::Primitive(
	UInt32 _verticesCount,
	UInt32 _indicesCount,
	UInt32 _flags) :
        BaseObject(nullptr),
			m_capacities(_flags),
			m_verticesCount(_verticesCount),
            m_pVertices(nullptr),
			m_indicesCount(_indicesCount),
            m_pIndices(nullptr),
            m_pTexCoords(nullptr)
{
	m_pVertices = new Float[_verticesCount*3];
	O3D_ASSERT(m_pVertices);

	m_pIndices = new UInt32[_indicesCount];
	O3D_ASSERT(m_pIndices);

	if (m_capacities & GEN_TEX_COORDS)
	{
		m_pTexCoords = new Float[_verticesCount*2];
		O3D_ASSERT(m_pTexCoords);
	}
}

Primitive::Primitive(const Primitive &dup) :
	BaseObject(dup),
		m_capacities(dup.m_capacities),
		m_verticesCount(dup.m_verticesCount),
        m_pVertices(nullptr),
		m_indicesCount(dup.m_indicesCount),
        m_pIndices(nullptr),
        m_pTexCoords(nullptr)
{
}

Primitive::~Primitive()
{
	deleteArray(m_pVertices);
	deleteArray(m_pIndices);
	deleteArray(m_pTexCoords);
}

//---------------------------------------------------------------------------------------
// O3DCube
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_ABSTRACT_CLASS1(Cube, ENGINE_CUBE, Primitive)

// The edges
const UInt32 Cube::EDGES[12][2] = {
	{ 0, 1 },
	{ 1, 2 },
	{ 2, 3 },
	{ 3, 0 },
	{ 0, 4 },
	{ 1, 5 },
	{ 2, 6 },
	{ 3, 7 },
	{ 4, 5 },
	{ 5, 6 },
	{ 6, 7 },
	{ 7, 4 }
};

//Faces
const UInt32 Cube::FACES[6][4] = {
	{ 0, 5, 8 , 4  },
	{ 1, 6, 9 , 5  },
	{ 2, 7, 10, 6  },
	{ 3, 4, 11, 7  },
	{ 0, 3, 2 , 1  },
	{ 8, 9, 10, 11 }
};

// true means as the same direction than the definition edges
const Bool Cube::FACE_EDGE_DIR[6][4] = {
	{ True , True , True, True },
	{ True , True , True, True },
	{ True , True , True, True },
	{ True , True , True, True },
	{ False, False, True, True },
	{ True , True , False , False  }
};


Cube::Cube(
	Float size,
	UInt32 division,
	UInt32 flags) :
		Primitive(flags),
		m_size(size),
		m_division(division)
{
	O3D_ASSERT(size > 0.); 

	m_pEdgeOffSet = new UInt32[12];
	m_pFaceOffSet = new UInt32[6];
	
	if (isWired())
	{
		m_verticesCount = (8 + 12 * division);
		m_indicesCount = (12 + m_division * 2 * 6) * 2;
	}
	else
	{
		m_verticesCount = (8 + 12 * division + division * division * 6);
		m_indicesCount = (division + 1) * (division + 1) * 6 * 2 * 3;
	}
	m_pVertices = new Float[m_verticesCount*3]; 
	m_pIndices = new UInt32[m_indicesCount];

	Float halfSize = size * 0.5f;

	// Les sommets du cube
	Vector3 sommets[8];
	sommets[0] = Vector3(-halfSize , -halfSize , -halfSize );
	sommets[1] = Vector3(halfSize, -halfSize , -halfSize );
	sommets[2] = Vector3(halfSize, halfSize, -halfSize );
	sommets[3] = Vector3(-halfSize , halfSize, -halfSize );
	sommets[4] = Vector3(-halfSize , -halfSize , halfSize);
	sommets[5] = Vector3(halfSize, -halfSize , halfSize);
	sommets[6] = Vector3(halfSize, halfSize, halfSize);
	sommets[7] = Vector3(-halfSize , halfSize, halfSize);

	// Copy sommets to the vertices buffer
	for(UInt32 i = 0 ; i < 8 ; ++i)
		sommets[i].copyTo(&m_pVertices[i * 3]);

	// Generate the edges
	UInt32 offSet = 8 * 3;

	if (division != 0)
		for(UInt32 edge = 0 ; edge < 12 ; ++edge)
		{
			m_pEdgeOffSet[edge] = offSet / 3;

			Vector3 step;
			for(int i = 0 ; i < 3 ; ++i)
				step[i] = (sommets[ EDGES[edge][1] ][i] - sommets[ EDGES[edge][0] ][i]) / Float(m_division + 1);
			Vector3 position(sommets[ EDGES[edge][0] ]);

			position += step;
			for(UInt32 i = 0 ; i < division ; ++i)
			{
				position.copyTo(&m_pVertices[offSet]);
				position += step;
				offSet += 3;
			}
		}

	if (isWired())
		constructWired(size);
	else
		constructFilled(size,offSet);

	deleteArray(m_pEdgeOffSet);
	deleteArray(m_pFaceOffSet);
}

void Cube::constructFilled(Float size, UInt32 offSet)
{
	// Construct the vertices face
	if (m_division != 0)
		for (UInt32 face = 0 ; face < 6 ; ++face)
		{
			m_pFaceOffSet[face] = offSet / 3;
			const Vector3 begin(&(m_pVertices[ getBeginVertice(FACES[face][0], FACE_EDGE_DIR[face][0])*3 ]));

			Vector3 iStep;
			for (UInt32 c = 0 ; c < 3 ; ++c)
				iStep[c] = (
						m_pVertices[getEndVertice(FACES[face][0],FACE_EDGE_DIR[face][0])*3 + c] -
						m_pVertices[ getBeginVertice(FACES[face][0], FACE_EDGE_DIR[face][0])*3 + c]) /
					Float(m_division + 1);
			
			Vector3 jStep;
			for (UInt32 c = 0 ; c < 3 ; ++c)
				jStep[c] = (
						m_pVertices[getEndVertice(FACES[face][3],FACE_EDGE_DIR[face][3])*3 + c] -
						m_pVertices[ getBeginVertice(FACES[face][3],FACE_EDGE_DIR[face][3])*3 + c]) /
					Float(m_division + 1);
		
			for (UInt32 i = 0 ; i < m_division ; ++i)
			{
				for (UInt32 j = 0 ; j < m_division ; ++j) 
				{
					const Vector3 pos(begin + iStep * Float(i + 1) + jStep * Float(j + 1));

					pos.copyTo(&(m_pVertices[offSet]));
					offSet += 3;
				}
			}
		}

	// Construct the faces
	UInt32 offSetFace = 0;
	for(UInt32 face = 0 ; face < 6 ; ++face)
	{
		for(UInt32 j = 0 ; j < m_division + 1 ; ++j)
		{
			for(UInt32 i = 0 ; i < m_division + 1 ; ++i)
			{
				// First triangle
				m_pIndices[offSetFace] = getVertice(face,i,j);
				++offSetFace;
				m_pIndices[offSetFace] = getVertice(face,i+1,j);
				++offSetFace;
				m_pIndices[offSetFace] = getVertice(face,i,j+1);
				++offSetFace;
				// Second triangle
				m_pIndices[offSetFace] = getVertice(face,i+1,j);
				++offSetFace;
				m_pIndices[offSetFace] = getVertice(face,i+1,j+1);
				++offSetFace;
				m_pIndices[offSetFace] = getVertice(face,i,j+1);
				++offSetFace;
			}
		}
	}
}

void Cube::constructWired(Float size)
{
	// Generate lines
	UInt32 offSetLine = 0;

	// Generate edges lines
	for(UInt32 i = 0 ; i < 12 ; ++i)
	{
		m_pIndices[offSetLine] = EDGES[i][0];
		++offSetLine;
		m_pIndices[offSetLine] =  EDGES[i][1];
		++offSetLine;
	}

	for (UInt32 face = 0 ; face < 6 ; ++face)
	{
		// Generate intermediary lines
		for (UInt32 i = 0 ; i < m_division ; ++i)
		{
				// First line
				m_pIndices[offSetLine] = this->getVertice(FACES[face][0], i, FACE_EDGE_DIR[face][0]);
				++offSetLine;
				m_pIndices[offSetLine] = this->getVertice(FACES[face][2], i, FACE_EDGE_DIR[face][2]);
				++offSetLine;
				// Second line
				m_pIndices[offSetLine] = this->getVertice(FACES[face][1], i, FACE_EDGE_DIR[face][1]);
				++offSetLine;
				m_pIndices[offSetLine] = this->getVertice(FACES[face][3], i, FACE_EDGE_DIR[face][3]);
				++offSetLine;
		}
	}
}


//---------------------------------------------------------------------------------------
// O3DCylinder
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_ABSTRACT_CLASS1(Cylinder, ENGINE_CYLINDER, Primitive)

Cylinder::Cylinder(
	Float base1,
	Float base2,
	Float height,
	UInt32 slices,
	UInt32 stacks,
	UInt32 flags) :
		Primitive(flags),
			m_base1(base1),
			m_base2(base2),
			m_height(height),
			m_slices(slices),
			m_stacks(stacks)
{
	O3D_ASSERT(base1 > 0.f); 
	O3D_ASSERT(base2 >= 0.f);
	// We can't build a circle with less than three slices (a triangle...)
	O3D_ASSERT(slices > 3);
	// We do not support height < 0.
	O3D_ASSERT(height > 0.f);

	if (isWired())
	{
		m_verticesCount = slices * (stacks + 2);
		m_indicesCount = slices * 2 + slices * 2 * 2 * (stacks + 1);
	}
	else
	{
		m_verticesCount = (slices + 1) * (stacks + 2);
		m_indicesCount = slices * 3 * 2 + slices * 3 * 2 * (stacks + 1);
	}

	if (base2 == 0.f)
	{
		if (isWired())
		{
			m_indicesCount -= slices * 2;
			m_verticesCount -= slices - 1;
		}
		else
		{
			m_indicesCount -= slices * 3 * 2;
			m_verticesCount -= slices;
		}
	}

	m_pVertices = new Float[m_verticesCount*3];
	m_pIndices = new UInt32[m_indicesCount];

	if (isWired())
		constructWired(base1,base2,height,slices,stacks);
	else
		constructFilled(base1,base2,height,slices,stacks);
}

void Cylinder::constructWired(
	Float base1,
	Float base2,
	Float height,
	UInt32 slices,
	UInt32 stacks)
{
	// Construct the base begin
	// The begin circle
	Float angle = 0.;
	Float step = 2 * o3d::PI / slices;

	//  The vertice
	for (UInt32 i = 0 ; i < slices ; ++i)
	{
		const UInt32 verticeIndice = i * 3;
		m_pVertices[verticeIndice] = cos(angle) * base1;
		m_pVertices[verticeIndice + 1] = 0.;
		m_pVertices[verticeIndice + 2] = sin(angle) * base1;

		angle += step;
	}
	// The lines
	for (UInt32 i = 0 ; i < slices ; ++i)
	{
		const UInt32 triangleIndice = i * 2;
		m_pIndices[triangleIndice] = i;
		m_pIndices[triangleIndice + 1] = i + 1;
	}
	// Loop to the begin
	m_pIndices[(slices - 1) * 2 + 1] = 0;

	// Construct the body
	UInt32 lastOffSet = 0;
	UInt32 offSet = 0;
	const Float yStep = height / Float(stacks + 1);
	Float y = yStep;
	const UInt32 division = (base2 == 0.f) ? stacks : stacks + 1;
	for (UInt32 i = 0 ; i < division ; ++i)
	{
		lastOffSet = offSet;
		offSet += slices;

		// The vertice
		angle = 0.;
		const Float p = (base2 - base1) / (height - 0.f);
		for (UInt32 j = 0 ; j < slices ; ++j)
		{
			const UInt32 verticeIndice = (offSet + j) * 3;
			// Linear interpolation
			const Float radius = p * (y - 0.f) + base1;

			m_pVertices[verticeIndice] = cos(angle) * radius;
			m_pVertices[verticeIndice + 1] = y;
			m_pVertices[verticeIndice + 2] = sin(angle) * radius;

			angle += step;
		}
		y += yStep;

		// The lines
		for (UInt32 j = 0 ; j < slices ; ++j)
		{
			const UInt32 triangleIndice = slices * 2 + (i * slices + j) * 2 * 2;
			// Line of the circle
			m_pIndices[triangleIndice] = offSet + j;
			m_pIndices[triangleIndice + 1] = offSet + j + 1;
			// Line between the circles
			m_pIndices[triangleIndice + 2] = lastOffSet + j;
			m_pIndices[triangleIndice + 3] = offSet + j;

		}
		// Loop to the begin
		const UInt32 loopIndice = slices * 2 + (i * slices + slices - 1) * 2 * 2;
		m_pIndices[loopIndice + 1] = offSet;
	}
	
	if(base2 == 0.f)
	{
		lastOffSet = offSet;
		offSet += slices;
		// The end point
		m_pVertices[offSet * 3] = 0.f;
		m_pVertices[offSet * 3 + 1] = height;
		m_pVertices[offSet * 3 + 2] = 0.f;

		// The lines
		for (UInt32 j = 0 ; j < slices ; ++j)
		{
			const UInt32 triangleIndice = slices * 2 + (division * slices) * 2 * 2 + j * 2;
			// Line between the circles
			m_pIndices[triangleIndice] = lastOffSet + j;
			m_pIndices[triangleIndice + 1] = offSet;

		}
	}
}

void Cylinder::constructFilled(
	Float base1,
	Float base2,
	Float height,
	UInt32 slices,
	UInt32 stacks)
{
	// Construct the base begin
	// The center
	m_pVertices[0] = 0.;
	m_pVertices[1] = 0.;
	m_pVertices[2] = 0.;

	// The begin circle
	Float angle = 0.;
	Float step = 2 * o3d::PI / slices;

	//  The vertices
	for (UInt32 i = 0 ; i < slices ; ++i)
	{
		const UInt32 verticeIndice = 3 + i * 3;
		m_pVertices[verticeIndice] = sin(angle) * base1;
		m_pVertices[verticeIndice + 1] = 0.;
		m_pVertices[verticeIndice + 2] = cos(angle) * base1;

		angle += step;
	}

	// The triangles
	for (UInt32 i = 0 ; i < slices ; ++i)
	{
		const UInt32 triangleIndice = i * 3;
		m_pIndices[triangleIndice] = 0;
		m_pIndices[triangleIndice + 1] = i + 2;
		m_pIndices[triangleIndice + 2] = i + 1;
	}

	// Loop to the begin
	m_pIndices[(slices - 1) * 3 + 1] = 1;

	// Construct the body
	UInt32 lastOffSet = 0;
	UInt32 offSet = 0;
	const Float yStep = height / Float(stacks + 1);
	Float y = yStep;
	const UInt32 division = (base2 == 0.f) ? stacks : stacks + 1;
	for (UInt32 i = 0 ; i < division ; ++i)
	{
		lastOffSet = offSet;
		offSet += slices + 1;

		// The vertices
		angle = -step;
		const Float p = (base2 - base1) / (height - 0.f);
		for (UInt32 j = 0 ; j < slices + 1 ; ++j)
		{
			const UInt32 verticeIndice = (offSet + j) * 3;
			// Linear interpolation
			const Float radius = p * (y - 0.f) + base1;

			if (j == 0)
			{
				m_pVertices[verticeIndice] = 0.f;
				m_pVertices[verticeIndice + 2] = 0.f;
			}
			else
			{
				m_pVertices[verticeIndice] = sin(angle) * radius;
				m_pVertices[verticeIndice + 2] = cos(angle) * radius;
			}
			m_pVertices[verticeIndice + 1] = y;

			angle += step;
		}
		y += yStep;

		// The triangles
		for (UInt32 j = 0 ; j < slices ; ++j)
		{
			// First triangle
			const UInt32 triangleIndice = slices * 3 + (i * slices + j) * 3 * 2;
			m_pIndices[triangleIndice] = lastOffSet + j + 1;
			m_pIndices[triangleIndice + 1] = lastOffSet + j + 2;
			m_pIndices[triangleIndice + 2] = offSet + j + 1;
			// Second triangle
			m_pIndices[triangleIndice + 3] = lastOffSet + j + 2;
			m_pIndices[triangleIndice + 4] = offSet + j + 2;
			m_pIndices[triangleIndice + 5] = offSet + j + 1;
		}
		// Loop to the begin
		const UInt32 loopIndice = slices * 3 + (i * slices + slices - 1) * 3 * 2;
		m_pIndices[loopIndice + 1] = lastOffSet + 1;
		m_pIndices[loopIndice + 3] = lastOffSet + 1;
		m_pIndices[loopIndice + 4] = offSet + 1;
	}

	// for cone
	if (base2 == 0.f)
	{
		lastOffSet = offSet;
		offSet += slices + 1;

		// End point
		m_pVertices[offSet * 3] = 0.f;
		m_pVertices[offSet * 3 + 1] = height;
		m_pVertices[offSet * 3 + 2] = 0.f;

		// The triangles
		for (UInt32 j = 0 ; j < slices ; ++j)
		{
			// First triangle
			const UInt32 triangleIndice = slices * 3 + (division * slices) * 3 * 2 + j * 3;
			m_pIndices[triangleIndice] = lastOffSet + j + 1;
			m_pIndices[triangleIndice + 1] = lastOffSet + j + 2;
			m_pIndices[triangleIndice + 2] = offSet;
		}
		// Loop to the begin
		const UInt32 loopIndice = slices * 3 + (division * slices) * 3 * 2 + (slices - 1) * 3;
		m_pIndices[loopIndice + 1] = lastOffSet + 1;
	}
	// non null base2
	else
	{
		UInt32 triangleIndice = slices * 3 + (stacks * slices + slices) * 3 * 2; // or m_indicesCount - slices * 3;
		lastOffSet += slices + 1;

		// The triangles
		for (UInt32 i = 0 ; i < slices ; ++i)
		{
			m_pIndices[triangleIndice] = offSet;
			m_pIndices[triangleIndice + 1] = lastOffSet + i + 1;
			m_pIndices[triangleIndice + 2] = lastOffSet + i + 2;

			triangleIndice += 3;
		}

		// Loop to the begin
		m_pIndices[triangleIndice - 1] = lastOffSet + 1;
	}
}


//---------------------------------------------------------------------------------------
// Sphere
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_ABSTRACT_CLASS1(Sphere, ENGINE_SPHERE, Primitive)

Sphere::Sphere(
	Float radius,
	UInt32 slices,
	UInt32 stacks,
	UInt32 flags) :
		Primitive(flags),
			m_radius(radius),
			m_slices(slices),
			m_stacks(stacks)
{
	O3D_ASSERT(radius > 0.); 
	O3D_ASSERT(slices >= 3); 
	O3D_ASSERT(stacks >= 1);

	if (isWired() && isTexCoords())
	{
		O3D_ERROR(E_InvalidParameter("Texture coordinate can not be generated in wireframe mode"));
		return;
	}

	constructVertices(radius, slices, stacks);

	if (isWired())
		constructWired(radius, slices, stacks);
	else
		constructFilled(radius, slices, stacks);
}

Sphere::TexCoordCorrectionPolicy Sphere::getTexCoordCorrectionPolicy() const
{
	if ((m_capacities & Sphere::FAST_CORRECTION) != 0)
		return Sphere::FAST_CORRECTION;
	else
		return Sphere::NO_CORRECTION;
}

void Sphere::constructVertices(Float radius, UInt32 slices, UInt32 stacks)
{
	// If texture is enabled, one vertex must be added on each stack.
	UInt32 lVerticesCount = 2 + stacks * slices + (isTexCoords() ? stacks : 0);

	// Reallocate a new buffer only if needed
	if ((m_pVertices != NULL) && (m_verticesCount != lVerticesCount))
	{
		deleteArray(m_pVertices);
		deleteArray(m_pTexCoords);
	}
		
	m_verticesCount = lVerticesCount;

	if (m_pVertices == NULL)
		m_pVertices = new Float[3*m_verticesCount];

	if ((m_pTexCoords == NULL) && isTexCoords())
		m_pTexCoords = new Float[2*m_verticesCount];

	UInt32 offSet = 0;
	// Construct the vertices
	m_pVertices[offSet++] = 0.f;
	m_pVertices[offSet++] = - radius;
	m_pVertices[offSet++] = 0.f;
	m_pVertices[offSet++] = 0.f;
	m_pVertices[offSet++] = radius;
	m_pVertices[offSet++] = 0.f;
	
	Float lStackAngle = 0.0f;
	Float lStackAngleStep = 0.0f;
	const Float lSliceAngleStep = 2.f * o3d::PI / slices;

	if (getTexCoordCorrectionPolicy() == Sphere::FAST_CORRECTION)
	{
		// DO NOT forget to modify the threshold in the tex coordinates generation
		const Float lStackCorrectionThreshold = 0.01f;

		Float lStackCorrection = atan(lStackCorrectionThreshold);
		lStackAngle = - 0.5f * o3d::PI + lStackCorrection;
		lStackAngleStep = (o3d::PI - 2.f * lStackCorrection) / (stacks - 1);
	}
	else
	{
		lStackAngleStep = o3d::PI / (stacks + 1);
		lStackAngle = - 0.5f * o3d::PI + lStackAngleStep;
	}

	for (UInt32 i = 0 ; i < stacks ; ++i, lStackAngle += lStackAngleStep)
	{
		Float lSliceAngle = 0.f;
		const Float lStackRadius = radius * cos(lStackAngle);
		const Float lStackPos = radius * sin(lStackAngle);

		for (UInt32 j = 0 ; j < slices ; ++j, lSliceAngle += lSliceAngleStep)
		{
			m_pVertices[offSet++] = cos(lSliceAngle) * lStackRadius;
			m_pVertices[offSet++] = lStackPos;
			m_pVertices[offSet++] = sin(lSliceAngle) * lStackRadius;
		}

		if (isTexCoords())
		{
			m_pVertices[offSet++] = lStackRadius;
			m_pVertices[offSet++] = lStackPos;
			m_pVertices[offSet++] = 0.0f;
		}
	}

	if (isTexCoords())
	{
		// Computation of texture coordinates
		offSet = 0;

		m_pTexCoords[offSet++] = 0.5f;
		m_pTexCoords[offSet++] = 1.0f;

		m_pTexCoords[offSet++] = 0.5f;
		m_pTexCoords[offSet++] = 0.0f;

		Float lIStacks = 0.0f;
		Float lISlices = 0.0f;
		Float lV = 0.0f;

		if (getTexCoordCorrectionPolicy() == Sphere::FAST_CORRECTION)
		{
			const Float lStackCorrectionThreshold = 0.01f;
			Float lStackCorrection = 0.0f;

			lStackCorrection = o3d::IVI_PI * atan(lStackCorrectionThreshold);
			lIStacks = (1.0f - 2.0f * lStackCorrection) / (stacks - 1);
			lISlices = 1.0f / slices;

			lV = 1.0f - lStackCorrection;
		}
		else
		{
			lIStacks = 1.0f / (stacks + 1);
			lISlices = 1.0f / slices;

			lV = 1.0f - lIStacks;
		}

		for (UInt32 i = 0 ; i < stacks ; ++i, lV -= lIStacks)
		{
			Float lU = 1.0f;

			for (UInt32 j = 0 ; j < slices ; ++j, lU -= lISlices)
			{
				m_pTexCoords[offSet++] = lU;;
				m_pTexCoords[offSet++] = lV;
			}

			m_pTexCoords[offSet++] = 0.0f;
			m_pTexCoords[offSet++] = lV;
		}
	}
}

void Sphere::constructFilled(Float radius, UInt32 slices, UInt32 stacks)
{
	const UInt32 lRealSlices = slices + (isTexCoords() ? 1 : 0);
	const UInt32 lNewIndicesCount = (2 * lRealSlices * stacks)* 3;

	// Reallocate a new buffer only if needed
	if ((m_pIndices != NULL) && (m_indicesCount != lNewIndicesCount))
		deleteArray(m_pIndices);
	
	m_indicesCount = lNewIndicesCount;

	if (m_pIndices == NULL)
		m_pIndices = new UInt32[m_indicesCount]; 

	UInt32 offSet = 0;

	// Loop for the first and the end
	for (UInt32 i = 0 ; i < lRealSlices-1 ; ++i)
	{
		// Begin triangle
		m_pIndices[offSet++] = 0;
		m_pIndices[offSet++] = 2 + i;
		m_pIndices[offSet++] = 2 + i + 1;
		// End triangle
		const UInt32 offSetVertice = m_verticesCount - i - 1;
		m_pIndices[offSet++] = 1;
		m_pIndices[offSet++] = offSetVertice;
		m_pIndices[offSet++] = offSetVertice - 1;
	}

		// Last iteration
		m_pIndices[offSet++] = 0;
		m_pIndices[offSet++] = 2 + lRealSlices-1;
		m_pIndices[offSet++] = 2;
		const UInt32 offSetVertice = m_verticesCount - lRealSlices;
		m_pIndices[offSet++] = 1;
		m_pIndices[offSet++] = offSetVertice;
		m_pIndices[offSet++] = m_verticesCount - 1;

	// Loop
	m_pIndices[offSet - 1] = m_verticesCount - 1;
	m_pIndices[offSet - 4] = 2;

	// Loop between the stacks
	UInt32 verticeOffSet = 2;
	for (UInt32 i = 0 ; i < stacks - 1 ; ++i)
	{
		const UInt32 verticeBegin = verticeOffSet;
		verticeOffSet += lRealSlices;

		for (UInt32 j = 0 ; j < lRealSlices-1 ; ++j)
		{
			// First triangle
			m_pIndices[offSet++] = verticeBegin + j;
			m_pIndices[offSet++] = verticeOffSet + j + 1;
			m_pIndices[offSet++] = verticeBegin + j + 1;
			// Second triangle
			m_pIndices[offSet++] = verticeOffSet + j + 1;
			m_pIndices[offSet++] = verticeBegin + j;
			m_pIndices[offSet++] = verticeOffSet + j;
		}

		// Last iteration
			// First triangle
			m_pIndices[offSet++] = verticeBegin + lRealSlices-1;
			m_pIndices[offSet++] = verticeOffSet;
			m_pIndices[offSet++] = verticeBegin;
			// Second triangle
			m_pIndices[offSet++] = verticeOffSet;
			m_pIndices[offSet++] = verticeBegin + lRealSlices-1;
			m_pIndices[offSet++] = verticeOffSet + lRealSlices-1;
	}
}

void Sphere::constructWired(Float radius, UInt32 slices, UInt32 stacks)
{
	const UInt32 lNewIndicesCount = (stacks * slices + slices * (stacks + 1)) * 2;

	// Reallocate a new buffer only if needed
	if ((m_pIndices != NULL) && (m_indicesCount != lNewIndicesCount))
		deleteArray(m_pIndices);
	
	m_indicesCount = lNewIndicesCount;

	if (m_pIndices == NULL)
		m_pIndices = new UInt32[m_indicesCount]; 

	UInt32 offSet = 0;
	UInt32 offSetVertice = 2;

	// Loop for the first and the end
	for (UInt32 i = 0 ; i < slices ; ++i)
	{
		// Begin Line
		m_pIndices[offSet++] = 0;
		m_pIndices[offSet++] = 2 + i;
	}

	for (UInt32 i = 0 ; i < slices ; ++i)
	{
		// End Line
		m_pIndices[offSet++] = 1;
		m_pIndices[offSet++] = m_verticesCount - 1 - i;
	}

	// The stacks circles
	for(UInt32 i = 0 ; i < stacks ; ++i)
	{
		const UInt32 verticeBegin = offSetVertice;

		for(UInt32 j = 0 ; j < slices ; ++j)
		{
			m_pIndices[offSet++] = offSetVertice;
			m_pIndices[offSet++] = ++offSetVertice;
		}
		// Loop to begin
		m_pIndices[offSet - 1] = verticeBegin;
	}

	// Loop between the stacks
	UInt32 verticeOffSet = 2;
	for (UInt32 i = 0 ; i < stacks - 1 ; ++i)
	{
		const UInt32 verticeBegin = verticeOffSet;
		verticeOffSet += slices;

		for (UInt32 j = 0 ; j < slices ; ++j)
		{
			// First triangle
			m_pIndices[offSet++] = verticeBegin + j;
			m_pIndices[offSet++] = verticeOffSet + j;
		}
	}
}


//---------------------------------------------------------------------------------------
// O3DSurface
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_ABSTRACT_CLASS1(Surface, ENGINE_SURFACE, Primitive)

Surface::Surface(
	Float width,
	Float height,
	UInt32 widthDiv,
	UInt32 heightDiv,
	UInt32 flags) :
		Primitive(flags),
			m_width(width),
			m_height(height),
			m_widthDiv(widthDiv),
			m_heightDiv(heightDiv)
{
	O3D_ASSERT(width > 0.f);
	O3D_ASSERT(height > 0.f);

	if (isSimpleGrid())
	{
		// vertices
		m_verticesCount = ((widthDiv+2) * (heightDiv+2)) - (widthDiv * heightDiv);
		m_pVertices = new Float[m_verticesCount*3];

		// texcoords
		if (isTexCoords())
			O3D_ERROR(E_InvalidParameter("Simple grid surface cannot have texture coordinates"));

		// indices
		m_indicesCount = ((widthDiv+2) + (heightDiv+2)) * 2;
		m_pIndices = new UInt32[m_indicesCount];

		// step
		Float xDelta = width / (Float)(widthDiv+1);	
		Float yDelta = height / (Float)(heightDiv+1);

		UInt32 offSet = 0;
		UInt32 verticeIndex;

		// start at half height
		Float yScale = height * 0.5f;

		// horizontal lines
		for (UInt32 y = 0; y < heightDiv+2; ++y)
		{
			verticeIndex = offSet*3;

			m_pVertices[verticeIndex  ] = -width * 0.5f;
			m_pVertices[verticeIndex+1] = 0.f;
			m_pVertices[verticeIndex+2] = yScale;

			m_pIndices[offSet] = offSet;
			++offSet;

			verticeIndex = offSet*3;

			m_pVertices[verticeIndex  ] = width * 0.5f;
			m_pVertices[verticeIndex+1] = 0.f;
			m_pVertices[verticeIndex+2] = yScale;

			m_pIndices[offSet] = offSet;
			++offSet;

			yScale -= yDelta;
		}

		// left and right vert lines
		m_pIndices[offSet] = 0;
		++offSet;
		m_pIndices[offSet] = (heightDiv+1)*2;
		++offSet;

		m_pIndices[offSet] = 1;
		++offSet;
		m_pIndices[offSet] = (heightDiv+1)*2+1;
		++offSet;

		// start at -half width + 1*xDelta
		Float xScale = -width * 0.5f + xDelta;

		// vertical lines
		for (UInt32 x = 1; x < widthDiv+1; ++x)
		{
			verticeIndex = (offSet-4)*3;

			m_pVertices[verticeIndex  ] = xScale;
			m_pVertices[verticeIndex+1] = 0.f;
			m_pVertices[verticeIndex+2] = -height * 0.5f;

			m_pIndices[offSet] = offSet-4;
			++offSet;

			verticeIndex = (offSet-4)*3;

			m_pVertices[verticeIndex  ] = xScale;
			m_pVertices[verticeIndex+1] = 0.f;
			m_pVertices[verticeIndex+2] = height * 0.5f;

			m_pIndices[offSet] = offSet-4;
			++offSet;

			xScale += xDelta;
		}
	}
	else
	{
		if (isWired() && isDoubleSided())
			O3D_ERROR(E_InvalidParameter("Wired mode is incompatible with double sided mode"));

		// vertices
		m_verticesCount = ((widthDiv+2) * (heightDiv+2)) * (isDoubleSided() ? 2 : 1);

		m_pVertices = new Float[m_verticesCount*3];

		UInt32 n = 0; 
		Float x,y;

		// step
		Float deltaX = width / (Float)(widthDiv+1);	
		Float deltaY = height / (Float)(heightDiv+1);

		// start at half height
		y = height * 0.5f;

		if (isTexCoords())
		{
			// generating vertices with textures coordinates

			m_pTexCoords = new Float[m_verticesCount*2];

			UInt32 o = 0;
			Float u,v;

			// step
			Float deltaU = 1.0f / (Float)(widthDiv+1);	
			Float deltaV = 1.0f / (Float)(heightDiv+1);

			// start at 1.0
			v = 1.0f;

			for (UInt32 i = 0 ; i < heightDiv+2 ; ++i)
			{
				x = -width * 0.5f;
				u = 0.0f;

				for (UInt32 j = 0 ; j < widthDiv+2 ; ++j)
				{
					m_pVertices[n] = x;
					m_pVertices[n+1] = 0.f; // y+ oriented
					m_pVertices[n+2] = y;

					m_pTexCoords[o] = u;
					m_pTexCoords[o+1] = v;

					n += 3;
					o += 2;

					x += deltaX;
					u += deltaU;
				}

				y -= deltaY;
				v -= deltaV;
			}

			// duplicate texcoords for the other side
			if (isDoubleSided())
			{
				memcpy(&m_pTexCoords[o],m_pTexCoords,o*sizeof(Float));
			}
		}
		else
		{
			// generating vertices with textures coordinates

			for (UInt32 i = 0 ; i < heightDiv+2 ; ++i)
			{
				x = -width * 0.5f;

				for (UInt32 j = 0 ; j < widthDiv+2 ; ++j)
				{
					m_pVertices[n] = x;
					m_pVertices[n+1] = 0.f; // y+ oriented
					m_pVertices[n+2] = y;

					n += 3;
					x += deltaX;
				}

				y -= deltaY;
			}

			
		}

		// duplicate vertices for the other side
		if (isDoubleSided())
		{
			memcpy(&m_pVertices[n],m_pVertices,n*sizeof(Float));
		}

		// faces
		if (isWired())
		{
			m_indicesCount = (((2 * (widthDiv+1)) + 1) * (heightDiv+1) + widthDiv+1) * 2;
			m_pIndices = new UInt32[m_indicesCount]; 
			constructWired(width, height, widthDiv, heightDiv);
		}
		else
		{
			m_indicesCount = (2 * (widthDiv+1) * (heightDiv+1)) * 3 * (isDoubleSided() ? 2 : 1);
			m_pIndices = new UInt32[m_indicesCount];
			constructFilled(width, height, widthDiv, heightDiv, isDoubleSided());
		}
	}
}

void Surface::constructFilled(
	Float width,
	Float height,
	UInt32 widthDiv,
	UInt32 heightDiv,
	Bool doubleSided)
{
	UInt32 p1,p2,p3,p4;

 	UInt32 offSet = 0;
	UInt32 verticeOffset = widthDiv+2;

	for (UInt32 y = 0 ; y < heightDiv+1 ; ++y)
	{
		for (UInt32 x = 0 ; x < widthDiv+1 ; ++x)
		{
			p1 = x + y * verticeOffset;
			p2 = p1 + verticeOffset;
			p3 = p2 + 1;
			p4 = p1 + 1;

			// face 1
			m_pIndices[offSet] = p1;
			++offSet;
			m_pIndices[offSet] = p3;
			++offSet;
			m_pIndices[offSet] = p2;
			++offSet;

			// face 2
			m_pIndices[offSet] = p1;
			++offSet;
			m_pIndices[offSet] = p4;
			++offSet;
			m_pIndices[offSet] = p3;
			++offSet;
		}
	}

	if (!doubleSided)
		return;

	// second part of the vertices
	UInt32 nextSideOffset = ((widthDiv+2) * (heightDiv+2));

	for (UInt32 y = 0 ; y < heightDiv+1 ; ++y)
	{
		for (UInt32 x = 0 ; x < widthDiv+1 ; ++x)
		{
			p1 = x + y * verticeOffset + nextSideOffset;
			p2 = p1 + verticeOffset;
			p3 = p2 + 1;
			p4 = p1 + 1;

			// face 1
			m_pIndices[offSet] = p1;
			++offSet;
			m_pIndices[offSet] = p2;
			++offSet;
			m_pIndices[offSet] = p3;
			++offSet;

			// face 2
			m_pIndices[offSet] = p1;
			++offSet;
			m_pIndices[offSet] = p3;
			++offSet;
			m_pIndices[offSet] = p4;
			++offSet;
		}
	}
}

void Surface::constructWired(
	Float width,
	Float height,
	UInt32 widthDiv,
	UInt32 heightDiv)
{
	UInt32 p1,p2,p3;

 	UInt32 offSet = 0;
	UInt32 verticeOffset = widthDiv+2;

	for (UInt32 y = 0 ; y < heightDiv+1 ; ++y)
	{
		for (UInt32 x = 0 ; x < widthDiv+1 ; ++x)
		{
			p1 = x + y * verticeOffset;
			p2 = p1 + 1;
			p3 = p1 + verticeOffset;

			// hor. line
			m_pIndices[offSet] = p1;
			++offSet;
			m_pIndices[offSet] = p2;
			++offSet;

			// vert. line 2
			m_pIndices[offSet] = p1;
			++offSet;
			m_pIndices[offSet] = p3;
			++offSet;
		}

		// last vert. line
		m_pIndices[offSet] = m_pIndices[offSet-3];
		++offSet;
		m_pIndices[offSet] = m_pIndices[offSet-1] + verticeOffset;
		++offSet;
	}

	// last hor. lines
	verticeOffset = (widthDiv+2)*(heightDiv+1);
	for (UInt32 x = 0 ; x < widthDiv+1 ; ++x)
	{
		m_pIndices[offSet] = verticeOffset++;
		++offSet;
		m_pIndices[offSet] = verticeOffset;
		++offSet;
	}
}
