/**
 * @file alphapipeline.cpp
 * @brief Implementation of AlphaPipeline.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-03-14
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/alphapipeline.h"

#include "o3d/engine/object/primitive.h"
#include "o3d/engine/object/vertexelement.h"
#include "o3d/engine/scene/sceneobject.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/material/materialtechnique.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/geom/aabbox.h"

#include <list>

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(AlphaPipeline, ENGINE_ALPHA_PIPELINE, SceneEntity)

// Default constructor.
AlphaPipeline::AlphaPipeline(BaseObject *pParent) :
	SceneEntity(pParent),
	m_numFaces(0),
	m_inputSize(0),
    m_inputList(nullptr),
    m_inputZ(nullptr),
	m_outputFacesUInt16(10*1024*3, 10*1024*3),
	m_outputFacesUInt32(10*1024*3, 10*1024*3),
    m_faceArrayUInt16(getScene()->getContext(), P_TRIANGLES, VertexBuffer::DYNAMIC),
    m_faceArrayUInt32(getScene()->getContext(), P_TRIANGLES, VertexBuffer::DYNAMIC)
{
}

// Virtual destructor.
AlphaPipeline::~AlphaPipeline()
{
	deleteArray(m_inputList);
	deleteArray(m_inputZ);
}

// clear input and output lists
void AlphaPipeline::reset()
{
	m_numFaces = 0;
	m_inputSize = 0;

	m_outputFacesUInt32.destroy();
	m_outputFacesUInt16.destroy();

	deleteArray(m_inputList);
	deleteArray(m_inputZ);
}

// resize input lists
void AlphaPipeline::setInputListSize(UInt32 size)
{
	if (size > m_inputSize)
	{
		UInt32 newsize;

		// new size
		if (size < m_inputSize + 100)
			newsize = m_inputSize + 100;
		else
			newsize = size;

		// realloc
		AlphaPipelineToken* InputListTmp = new AlphaPipelineToken[newsize];
		Float*			   InputZTmp    = new Float[newsize];

		// copy
		if (m_inputList != NULL)
		{
			memcpy(InputListTmp,m_inputList,m_inputSize*sizeof(AlphaPipelineToken));
			deleteArray(m_inputList);
		}

		if (m_inputZ != NULL)
		{
			memcpy(InputZTmp,m_inputZ,m_inputSize*sizeof(Float));
			deleteArray(m_inputList);
		}

		m_inputList = InputListTmp;
		m_inputZ = InputZTmp;
		m_inputSize = newsize;
	}
}

// sort z average
void AlphaPipeline::sort()
{
	m_radixSort.sort(m_inputZ, m_numFaces);

	// clear the draw list
	m_processList.clear();

	if (m_numFaces == 0)
		return;

	UInt32 index = 0;			// vertex index
	UInt32 packedCount = 0;		// number of faces for the object

	Shadable *shadable = NULL;	// object to compact
	MaterialTechnique *material = NULL;   // material technique to compact

	UInt32 startIndex = 0;      // object start position
	UInt32 endIndex = 0;        // object start position

	// sorted faces list
	UInt32* indices = m_radixSort.getIndices();
	UInt32 i3;

	UInt32 minVertex, maxVertex;
	UInt32 numVertices;

	// resize the output face array if necessary
	if (m_faceArrayUInt32.getNumElements() < m_numFaces*3)
		m_faceArrayUInt32.create(m_numFaces*3);

	if (m_faceArrayUInt16.getNumElements() < m_numFaces*3)
		m_faceArrayUInt16.create(m_numFaces*3);

	while (index < m_numFaces)
	{
		// first element as reference
		shadable = m_inputList[indices[index]].getShadable();
		material = m_inputList[indices[index]].getMaterial();
		startIndex = index;
		packedCount = 1;

		// count the number of contiguous similar references
		while ((index < m_numFaces-1) &&
			   (m_inputList[indices[index+1]].getShadable() == shadable) &&
			   (m_inputList[indices[index+1]].getMaterial() == material))
		{
			// and one more packed face
			++packedCount;
			++index;
		}

		// compute the end face index
		endIndex = startIndex + packedCount*3;

		// now, write the face list into the output list of face,
		// according to the number of vertices, if there is not vertices array,
		// we assume there is lesser than 65536 vertices.
		VertexElement *verticesArray = shadable->getVertexElement(V_VERTICES_ARRAY);
		if (verticesArray)
			numVertices = verticesArray->getNumElements();
		else
			numVertices = 0;

		if (numVertices > 65535)
		{
			// resize the output array if necessary
			m_outputFacesUInt32.setSize(packedCount*3);

			UInt32 *output = m_outputFacesUInt32.getData();
			minVertex = o3d::Limits<UInt32>::max();
			maxVertex = 0;

			// add the faces by group of three vertices
			for (UInt32 i = 0; i < packedCount; ++i)
			{
				i3 = i*3;

				m_inputList[indices[startIndex+i]].getVertices(&output[i3]);

				maxVertex = o3d::max<UInt32>(maxVertex, output[i3]);
				maxVertex = o3d::max<UInt32>(maxVertex, output[i3+1]);
				maxVertex = o3d::max<UInt32>(maxVertex, output[i3+2]);

				minVertex = o3d::min<UInt32>(minVertex, maxVertex);
			}

			m_faceArrayUInt32.update(output, startIndex, packedCount*3);
		}
		else
		{
			// resize the output array if necessary
			m_outputFacesUInt16.setSize(packedCount*3);

			UInt16 *output = m_outputFacesUInt16.getData();
			minVertex = o3d::Limits<UInt16>::max();
			maxVertex = 0;

			// add the faces by group of three vertices
			for (UInt32 i = 0; i < packedCount; ++i)
			{
				i3 = i*3;

				m_inputList[indices[startIndex+i]].getVertices(&output[i3]);

				maxVertex = o3d::max<UInt32>(maxVertex, output[i3]);
				maxVertex = o3d::max<UInt32>(maxVertex, output[i3+1]);
				maxVertex = o3d::max<UInt32>(maxVertex, output[i3+2]);

				minVertex = o3d::min<UInt32>(minVertex, maxVertex);
			}

			m_faceArrayUInt16.update(output, startIndex, packedCount*3);
		}

		// one more draw set to process
		if (material && shadable)
		{
			DrawSet drawSet;
			drawSet.countIndex = packedCount;
			drawSet.firstIndex = startIndex;
			drawSet.lastIndex = endIndex;
			drawSet.material = material;
			drawSet.maxVertex = maxVertex;
			drawSet.minVertex = minVertex;
			drawSet.shadable = shadable;
			drawSet.numVertices = numVertices;

			m_processList.push_back(drawSet);
		}

		// face of the next object/material
		++index;
	}

	// clear input
	m_numFaces = 0;
}

// Process the draw list
void AlphaPipeline::draw(const DrawInfo &drawInfo)
{
	for (std::list<DrawSet>::iterator it = m_processList.begin(); it != m_processList.end(); ++it)
	{
		DrawSet &drawSet = *it;

		if (drawSet.numVertices > 65535)
		{
			m_faceArrayUInt32.setMinMaxVertex(drawSet.minVertex, drawSet.maxVertex);
			drawSet.shadable->useExternalFaceArray(
					&m_faceArrayUInt32,
					drawSet.countIndex,
					drawSet.firstIndex,
					drawSet.lastIndex);
		}
		else
		{
			m_faceArrayUInt16.setMinMaxVertex(drawSet.minVertex, drawSet.maxVertex);
			drawSet.shadable->useExternalFaceArray(
					&m_faceArrayUInt16,
					drawSet.countIndex,
					drawSet.firstIndex,
					drawSet.lastIndex);
		}

		drawSet.shadable->setUpModelView();

		// use rendering pipeline of sorted materials pass only
		drawSet.material->processAlphaPipeline(
				*drawSet.shadable,
				drawInfo);

		drawSet.shadable->useInternalFaceArray();
	}
}

