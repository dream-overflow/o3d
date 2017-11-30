/**
 * @file geometrydata.cpp
 * @brief Implementation of Geometry.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-06-06
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/geometrydata.h"

#include "o3d/engine/object/facearrayvisitor.h"
#include "o3d/engine/object/primitivemanager.h"
#include "o3d/engine/object/progressivemesh.h"
#include "o3d/engine/context.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/core/math.h"
#include "o3d/core/vector2.h"
#include "o3d/geom/boundinggen.h"
#include "o3d/core/file.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(GeometryData, ENGINE_GEOMETRY_DATA, SceneEntity)

// Default constructor
GeometryData::GeometryData(BaseObject *parent) :
	SceneEntity(parent),
	m_vbo(NULL),
	m_flags(),
	m_boundingMode(BOUNDING_AUTO),
	m_lodLvl(100),
	m_boundFaceArray(NULL),
	m_vertexBlender(NULL)
{
	//m_flags.fillTrue();

    m_flags.enable(UPDATE_VERTEX_BUFFER);
    m_flags.disable(UPDATE_PROGRESSIVE_MESH);

	for (Int32 i = 0; i < NUM_VERTEX_ATTRIBUTES; ++i)
	{
		m_elements[i] = NULL;
	}
}

// Build from a primitive object.
GeometryData::GeometryData(BaseObject *parent, const Primitive &primitive) :
	SceneEntity(parent),
	m_vbo(NULL),
	m_flags(),
	m_boundingMode(BOUNDING_AUTO),
	m_lodLvl(100),
	m_boundFaceArray(NULL),
	m_vertexBlender(NULL)
{
	//m_flags.fillTrue();

    m_flags.enable(UPDATE_VERTEX_BUFFER);
    m_flags.disable(UPDATE_PROGRESSIVE_MESH);

	for (Int32 i = 0; i < NUM_VERTEX_ATTRIBUTES; ++i)
	{
		m_elements[i] = NULL;
	}

	buildFromPrimitive(primitive);
}

// Virtual destructor
GeometryData::~GeometryData()
{
	for (Int32 i = 0; i < NUM_VERTEX_ATTRIBUTES; ++i)
	{
		deletePtr(m_elements[i]);
	}

	for (IT_FaceArrays it = m_faceArrays.begin(); it != m_faceArrays.end(); ++it)
	{
		deletePtr(it->second);
	}

	deletePtr(m_vbo);
}

// Create and validate all elements and face arrays.
void GeometryData::create(Bool keepLocalDataForSkinning)
{
	if (!isVertices())
		O3D_ERROR(E_InvalidPrecondition("Vertices element must be defined"));

	// compute normals if necessary
	if (m_flags.getBit(UPDATE_NORMALS) &&
		(m_flags.getBit(GEN_NORMALS) || m_elements[V_NORMALS_ARRAY]))
		computeNormals();

	// auto tangent space computation
	if (m_flags.getBit(UPDATE_TANGENT_SPACE) &&
		(m_flags.getBit(GEN_TANGENT_SPACE) || m_elements[V_TANGENT_ARRAY] || m_elements[V_BITANGENT_ARRAY]))
		computeTangentSpace();

	// compute progressive mesh
	if (m_flags.getBit(UPDATE_PROGRESSIVE_MESH) && m_flags.getBit(GEN_PROGRESSIVE_MESH))
		computeProgressive();

	// vertex buffer object
	if (m_flags.getBit(UPDATE_VERTEX_BUFFER))
	{
		// elements
		Bool keepData;
		UInt32 size = 0;

        VertexBufferObjf *newVbo = new VertexBufferObjf(getScene()->getContext());

		// interleave data
        if (m_flags.getBit(INTERLEAVE_ELEMENTS))
		{
			UInt32 offset[NUM_VERTEX_ATTRIBUTES];
			UInt32 stride = 0;

			UInt32 numElt = getVertices()->getNumElements();
			UInt32 dstOfs = 0;

			const Float *data[NUM_VERTEX_ATTRIBUTES];

			for (Int32 i = 0; i < NUM_VERTEX_ATTRIBUTES; ++i)
			{
				if (m_elements[i])
				{
					size += m_elements[i]->getNumElements()*m_elements[i]->getElementSize();

					offset[i] = stride;
					stride += m_elements[i]->getElementSize();

					data[i] = m_elements[i]->lockArray(0, 0);
				}
			}

            newVbo->create(size, VertexBuffer::STATIC, nullptr, True);
			Float *vboData = newVbo->lock(0, 0, VertexBuffer::WRITE_ONLY);

			// interleave data into the dst VBO
			for (UInt32 s = 0; s < numElt; ++s)
			{
				for (Int32 i = 0; i < NUM_VERTEX_ATTRIBUTES; ++i)
				{
					if (m_elements[i])
					{
						memcpy(
								vboData + dstOfs,
								data[i] + m_elements[i]->getElementSize() * s,
								m_elements[i]->getElementSize() * sizeof(Float));

						dstOfs += m_elements[i]->getElementSize();
					}
				}
			}

			for (Int32 i = 0; i < NUM_VERTEX_ATTRIBUTES; ++i)
			{
				if (m_elements[i])
				{
					if (keepLocalDataForSkinning)
					{
						switch (i)
						{
							case V_VERTICES_ARRAY:
							case V_NORMALS_ARRAY:
							case V_RIGGING_ARRAY:
							case V_SKINNING_ARRAY:
							case V_WEIGHTING_ARRAY:
								keepData = True;
								break;

							default:
								keepData = False;
								break;
						}
					}
					else
						keepData = False;

					m_elements[i]->unlockArray();

					m_elements[i]->create(
							*newVbo,
							offset[i],
							stride,
							keepData);
				}
			}

			newVbo->unlock();
		}
		// serialize data
		else
		{
			UInt32 offset[NUM_VERTEX_ATTRIBUTES];

			for (Int32 i = 0; i < NUM_VERTEX_ATTRIBUTES; ++i)
			{
				if (m_elements[i])
				{
					offset[i] = size;
					size += m_elements[i]->getNumElements()*m_elements[i]->getElementSize();
				}
			}

            newVbo->create(size, VertexBuffer::STATIC, nullptr, True);

			for (Int32 i = 0; i < NUM_VERTEX_ATTRIBUTES; ++i)
			{
				if (m_elements[i])
				{
					if (keepLocalDataForSkinning)
					{
						switch (i)
						{
							case V_VERTICES_ARRAY:
							case V_NORMALS_ARRAY:
							case V_RIGGING_ARRAY:
							case V_SKINNING_ARRAY:
							case V_WEIGHTING_ARRAY:
								keepData = True;
								break;

							default:
								keepData = False;
								break;
						}
					}
					else
						keepData = False;

					const Float *data = m_elements[i]->lockArray(0, 0);

					newVbo->update(
							data,
							offset[i],
							m_elements[i]->getNumElements()*m_elements[i]->getElementSize());

					m_elements[i]->unlockArray();

					m_elements[i]->create(*newVbo, offset[i], 0, keepData);
				}
			}
		}

		newVbo->unbindBuffer();

		// delete for a previous VBO
		deletePtr(m_vbo);

		// and define the new one
		m_vbo = newVbo;

		m_flags.setBit(UPDATE_VERTEX_BUFFER, False);
	}

	// index buffer object
	if (m_flags.getBit(UPDATE_INDEX_BUFFER))
	{
		// faces array
		for (IT_FaceArrays it = m_faceArrays.begin(); it != m_faceArrays.end(); ++it)
		{
			it->second->create();
		}

		m_flags.setBit(UPDATE_INDEX_BUFFER, False);
	}
}

// Is the geometry data is valid.
Bool GeometryData::isValid() const
{
	return !m_flags.getBit(UPDATE_VERTEX_BUFFER) && !m_flags.getBit(UPDATE_INDEX_BUFFER);
}

// Create an element array of a given type.
VertexElement* GeometryData::createElement(
		VertexAttributeArray mode,
		const SmartArrayFloat &data)
{
	if (m_elements[mode])
		O3D_ERROR(E_ValueRedefinition("This array has a previous definition"));

	VertexElement *element = newVertexElement(mode, data);

	switch (mode)
	{
		case V_VERTICES_ARRAY:
			m_flags.setBit(UPDATE_BOUNDING, True);
			m_flags.setBit(UPDATE_PROGRESSIVE_MESH, True);

			if (!m_elements[V_NORMALS_ARRAY])
				m_flags.setBit(UPDATE_NORMALS, True);

			if (!m_elements[V_TANGENT_ARRAY] || !m_elements[V_BITANGENT_ARRAY])
				m_flags.setBit(UPDATE_TANGENT_SPACE, True);
			break;

		// validate normals and invalidate tangent space
		case V_NORMALS_ARRAY:
			m_flags.setBit(UPDATE_NORMALS, False);

			if (!m_elements[V_TANGENT_ARRAY] || !m_elements[V_BITANGENT_ARRAY])
				m_flags.setBit(UPDATE_TANGENT_SPACE, True);
			break;

		// validate tangent space
		case V_TANGENT_ARRAY:
		case V_BITANGENT_ARRAY:
			if (m_elements[V_TANGENT_ARRAY] && m_elements[V_BITANGENT_ARRAY])
				m_flags.setBit(UPDATE_TANGENT_SPACE, False);
			break;

		default:
			break;
	}

	m_flags.setBit(UPDATE_VERTEX_BUFFER, True);

	return element;
}

// Delete an element array given a type
void GeometryData::deleteElement(VertexAttributeArray mode)
{
	if (m_elements[mode])
	{
		deletePtr(m_elements[mode]);

		switch (mode)
		{
			case V_VERTICES_ARRAY:
				m_flags.setBit(UPDATE_BOUNDING, True);
				m_flags.setBit(UPDATE_NORMALS, True);
				m_flags.setBit(UPDATE_PROGRESSIVE_MESH, True);
				break;

			case V_NORMALS_ARRAY:
				m_flags.setBit(UPDATE_NORMALS, True);
				m_flags.setBit(UPDATE_TANGENT_SPACE, True);
				break;

			case V_TANGENT_ARRAY:
			case V_BITANGENT_ARRAY:
				m_flags.setBit(UPDATE_TANGENT_SPACE, True);
				break;

			default:
				break;
		}

		m_flags.setBit(UPDATE_VERTEX_BUFFER, True);
	}
}

// Count the number of valid elements.
UInt32 GeometryData::getNumElements() const
{
	UInt32 count = 0;

	for (Int32 i = 0; i < NUM_VERTEX_ATTRIBUTES; ++i)
	{
		if (m_elements[i])
			++count;
	}

	return count;
}

// Set if data are interleaved.
void GeometryData::setInterleave(Bool interleave)
{
	if (m_flags.getBit(INTERLEAVE_ELEMENTS) != interleave)
	{
		m_flags.setBit(INTERLEAVE_ELEMENTS, interleave);
		m_flags.setBit(UPDATE_VERTEX_BUFFER, True);
	}
}

// Is data are interleaved.
Bool GeometryData::isInterleaved() const
{
	return m_flags.getBit(INTERLEAVE_ELEMENTS);
}

// Add a face array given an unique identifier.
void GeometryData::addFaceArray(UInt32 idArray, FaceArray *faceArray)
{
	O3D_ASSERT(faceArray);

	if (m_faceArrays.find(idArray) == m_faceArrays.end())
		m_faceArrays[idArray] = faceArray;
	else
	{
		O3D_ERROR(E_InvalidParameter("The face array id is already in usage"));
	}

	m_flags.setBit(UPDATE_INDEX_BUFFER, True);
	m_flags.setBit(UPDATE_PROGRESSIVE_MESH, True);
}

// Delete a specified face array given its unique identifier
void GeometryData::deleteFaceArray(UInt32 idArray)
{
	IT_FaceArrays it = m_faceArrays.find(idArray);
	if (it == m_faceArrays.end())
	{
		O3D_ERROR(E_InvalidParameter("The face array id is not available"));
	}
	else
	{
		deletePtr(it->second);
		m_faceArrays.erase(it);
	}

	m_flags.setBit(UPDATE_INDEX_BUFFER, True);
	m_flags.setBit(UPDATE_PROGRESSIVE_MESH, True);
}

// Get the total number face of each face array.
UInt32 GeometryData::getNumFaces() const
{
	UInt32 count = 0;

	for (CIT_FaceArrays it = m_faceArrays.begin(); it != m_faceArrays.end(); ++it)
	{
		count += it->second->getNumFaces();
	}

	return 0;
}

// Create a new vertex blend
VertexBlend* GeometryData::createVertexBlend()
{
    VertexBlend *vertexBlend = new VertexBlend(getScene()->getContext(), getVertices(), getNormals());
	return vertexBlend;
}

// Get memory usage of all the geometry and faces array.
UInt32 GeometryData::getCapacity() const
{
	UInt32 count = 0;

	for (Int32 i = 0; i < NUM_VERTEX_ATTRIBUTES; ++i)
	{
		if (m_elements[i])
			count += m_elements[i]->getCapacity();
	}

	for (CIT_FaceArrays it = m_faceArrays.begin(); it != m_faceArrays.end(); ++it)
	{
		count += it->second->getCapacity();
	}

	return count;
}

// Bind the vertex buffer object.
void GeometryData::bindVertexBuffer()
{
	m_vbo->bindBuffer();
}

// Bind a data array.
void GeometryData::attribute(VertexAttributeArray mode, UInt32 location)
{
	O3D_ASSERT(m_elements[mode]);

	if (m_vertexBlender)
	{
		if (mode == V_VERTICES_ARRAY)
		{
			m_vertexBlender->getVertices().attribute(location);
			return;
		}
		else if (mode == V_NORMALS_ARRAY)
		{
			m_vertexBlender->getNormals().attribute(location);
			return;
		}
	}

	if (m_elements[mode])
	{
			m_elements[mode]->attribute(location);
	}
}

// Bind a face array before to draw given the face array unique identifier.
void GeometryData::bindFaceArray(UInt32 idArray)
{
	IT_FaceArrays it = m_faceArrays.find(idArray);
	if (it != m_faceArrays.end())
		m_boundFaceArray = it->second;
	else
        m_boundFaceArray = nullptr;
}

// Bind a face array before to draw given the face array unique identifier.
void GeometryData::bindExternalFaceArray(FaceArray *faceArray)
{
	m_boundFaceArray = faceArray;
}

// Draw using the current bound face array.
void GeometryData::draw()
{
	if (!m_flags.getBit(UPDATE_VERTEX_BUFFER) &&
		!m_flags.getBit(UPDATE_INDEX_BUFFER) &&
		m_boundFaceArray)
	{
		m_boundFaceArray->draw(getScene());
	}
}

// Draw the geometry using an external face array, and with a specified range.
void GeometryData::drawPart(
	FaceArray *faceArray,
	UInt32 firstIndex,
	UInt32 lastIndex)
{
	if (!m_flags.getBit(UPDATE_VERTEX_BUFFER) && faceArray)
		faceArray->drawPart(getScene(), firstIndex, lastIndex);
}

// Draw the local space using an external face array, and with a specified range.
void GeometryData::drawLocalSpacePart(
	FaceArray *faceArray,
	UInt32 firstIndex,
	UInt32 lastIndex)
{
	if (getScene()->getDrawObject(Scene::DRAW_LOCAL_SPACE) &&
		!m_flags.getBit(UPDATE_VERTEX_BUFFER) &&
		!m_flags.getBit(UPDATE_INDEX_BUFFER) &&
		!m_flags.getBit(UPDATE_NORMALS) &&
		!m_flags.getBit(UPDATE_TANGENT_SPACE) &&
		faceArray)
	{
		if (faceArray->getNumElements() == 0)
			return;

		// need vertices, normals, tangents and bi-tangents
		if (!m_elements[V_VERTICES_ARRAY] || !m_elements[V_NORMALS_ARRAY] ||
			!m_elements[V_TANGENT_ARRAY] || !m_elements[V_BITANGENT_ARRAY])
		{
			return;
		}

		// lock the elements
		const Float *vertices = m_elements[V_VERTICES_ARRAY]->lockArray(0, 0);
		const Float *normals = m_elements[V_NORMALS_ARRAY]->lockArray(0, 0);
		const Float *tangents = m_elements[V_TANGENT_ARRAY]->lockArray(0, 0);
		const Float *bitangents = m_elements[V_BITANGENT_ARRAY]->lockArray(0, 0);

		// error...
		if (!vertices || !normals || !tangents || !bitangents)
		{
			if (vertices) m_elements[V_VERTICES_ARRAY]->unlockArray();
			if (normals) m_elements[V_NORMALS_ARRAY]->unlockArray();
			if (tangents) m_elements[V_TANGENT_ARRAY]->unlockArray();
			if (bitangents) m_elements[V_BITANGENT_ARRAY]->unlockArray();
			return;
		}

		FaceArrayVisitor triangles(faceArray, firstIndex, lastIndex-firstIndex);

		// access to triangles
		Float vector[3];
		UInt32 indice;

		// simple drawing mode
		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

		getScene()->getContext()->setCullingMode(CULLING_NONE);

		primitive->setColor(1.f,1.f,1.f);
		primitive->beginDraw(P_LINES);

		/*UInt32 verticesOfs, normalOfs, tangentOfs, bitangentOfs;

		if (isInterleaved()) TODO
		{
			verticesOfs = m_elements[V_VERTICES_ARRAY]->getElementSize() * m_elements[V_VERTICES_ARRAY]->getStride();
			normalOfs = m_elements[V_NORMALS_ARRAY]->getElementSize() * m_elements[V_NORMALS_ARRAY]->getStride();
			tangentOfs = m_elements[V_TANGENT_ARRAY]->getElementSize() * m_elements[V_TANGENT_ARRAY]->getStride();
			bitangentOfs = m_elements[V_BITANGENT_ARRAY]->getElementSize() * m_elements[V_BITANGENT_ARRAY]->getStride();
		}
		else
		{
			verticesOfs = m_elements[V_VERTICES_ARRAY]->getElementSize();
			normalOfs = m_elements[V_NORMALS_ARRAY]->getElementSize();
			tangentOfs = m_elements[V_TANGENT_ARRAY]->getElementSize();
			bitangentOfs = m_elements[V_BITANGENT_ARRAY]->getElementSize();
		}*/

		for (FaceArrayIterator it = triangles.begin(); it != triangles.end(); ++it)
		{
			// tangent in red
			indice = it.a*3;
			vector[0] = vertices[indice]   + tangents[indice];
			vector[1] = vertices[indice+1] + tangents[indice+1];
			vector[2] = vertices[indice+2] + tangents[indice+2];

			primitive->addVertex(vector, Color(1.0f,0.0f,0.0f));
			primitive->addVertex(&vertices[indice], Color(1.0f,0.0f,0.0f));

			// binormal in green
			indice = it.b*3;
			vector[0] = vertices[indice]   + bitangents[indice];
			vector[1] = vertices[indice+1] + bitangents[indice+1];
			vector[2] = vertices[indice+2] + bitangents[indice+2];

			primitive->addVertex(vector, Color(0.0f,1.0f,0.0f));
			primitive->addVertex(&vertices[indice], Color(0.0f,1.0f,0.0f));

			// normal in blue
			indice = it.c*3;
			vector[0] = vertices[indice]   + normals[indice];
			vector[1] = vertices[indice+1] + normals[indice+1];
			vector[2] = vertices[indice+2] + normals[indice+2];

			primitive->addVertex(vector, Color(0.0f,0.0f,1.0f));
			primitive->addVertex(&vertices[indice], Color(0.0f,0.0f,1.0f));
		}

		primitive->endDraw();

		m_elements[V_VERTICES_ARRAY]->unlockArray();
		m_elements[V_NORMALS_ARRAY]->unlockArray();
		m_elements[V_TANGENT_ARRAY]->unlockArray();
		m_elements[V_BITANGENT_ARRAY]->unlockArray();
	}
}

// Draw the local space using the current bound face array.
void GeometryData::drawLocalSpace()
{
	if (m_boundFaceArray)
		drawLocalSpacePart(m_boundFaceArray, 0, 0);
}

// compute the bounding volume of this geometry, using all face arrays.
void GeometryData::computeBounding(BoundingMode mode)
{
	if (!isVertices())
		O3D_ERROR(E_InvalidPrecondition("Vertices must must defined"));

	const Float *vertices = getVertices()->lockArray(0, 0);
	UInt32 numVertices = getNumVertices();

	if (!vertices)
		O3D_ERROR(E_InvalidPrecondition("Unable to access to the vertices array"));

	// fast bounding box computation
	if (mode == BOUNDING_FAST)
	{
		// build the bounding box by using the minimum and maximum of the
		// vertices present in the vertex array.
		Vector3 curVertex;

		Vector3 minCoords(
				Limits<Float>::max(),
				Limits<Float>::max(),
				Limits<Float>::max());

		Vector3 maxCoords(
				Limits<Float>::min(),
				Limits<Float>::min(),
				Limits<Float>::min());

		UInt32 numIndices = numVertices * 3;

		for (UInt32 vert = 0; vert < numIndices; vert += 3)
		{
			curVertex.set(vertices + vert);

			minCoords.minOf(minCoords, curVertex);
			maxCoords.maxOf(maxCoords, curVertex);
		}

		// get the bounding box
		m_AABBoxExt.setMinMax(minCoords, maxCoords);

		// and the bounding sphere
		m_BSphere.setCenter(m_AABBoxExt.getCenter());
		m_BSphere.setRadius(m_AABBoxExt.getRadius());

		m_boundingMode = GeometryData::BOUNDING_BOXEXT;
	}
	// precise bounding box computation
	else
	{
		BoundingGen bounding;
		UInt32 numIndices = numVertices * 3;

		// first pass bounding volume computation
		for (UInt32 vert = 0 ; vert < numIndices ; vert += 3)
			bounding.firstPass(vertices[vert], vertices[vert+1], vertices[vert+2]);

		bounding.flushFirstPass(numVertices);

		// second pass bounding volume computation
		for (UInt32 vert = 0 ; vert < numIndices ; vert += 3)
			bounding.secondPass(vertices[vert], vertices[vert+1], vertices[vert+2]);

		bounding.flushSecondPass(numVertices);

		// third pass bounding volume computation
		for (UInt32 vert = 0 ; vert < numIndices ; vert += 3)
			bounding.thirdPass(vertices[vert], vertices[vert+1], vertices[vert+2]);

		bounding.flushThirdPass();

		// fourth pass bounding volume computation
		for (UInt32 vert = 0 ; vert < numIndices ; vert += 3)
			bounding.fourthPass(vertices[vert], vertices[vert+1], vertices[vert+2]);

		// get the bounding sphere
		bounding.getBoundingSphere(m_BSphere);

		// the axis aligned bounding box (with extended sphere)
		bounding.getAxisAlignedBoundingBoxExt(m_AABBoxExt);

		// and the oriented bounding box
		//bounding.getOrientedBoundingBox(m_bboxExt);
	}

	// unlock data array
	getVertices()->unlockArray();

	// if we use the bounding sphere
	if (mode == BOUNDING_SPHERE)
	{
		m_boundingMode = GeometryData::BOUNDING_SPHERE;
	}
	// if we use the (extended) bounding box
	else if (mode == BOUNDING_BOX)
	{
		m_boundingMode = BOUNDING_BOX;
	}
	else if (mode == BOUNDING_BOXEXT)
	{
		m_boundingMode = BOUNDING_BOXEXT;
	}
	// or if we let the automatic mode choose for us
	else if (mode == BOUNDING_AUTO)
	{
		// compute the standard deviation of the bounding box
		Vector3 vec = m_AABBoxExt.getHalfSize();
		Float average = (vec[X] + vec[Y] + vec[Z]) / 3.f;
		Float ecart = 0.f;

		for (Int32 i = 0 ; i < 3 ; ++i)
		{
			ecart += (vec[i] - average) * (vec[i] - average);
		}

		// standard deviation is null so bounding sphere
		if (ecart == 0.f)
		{
			m_boundingMode = BOUNDING_SPHERE;
		}
		else
		{
            ecart = Math::sqrt(ecart / 3.f);

			// check with the largest component
			Float largest = o3d::maxMax((vec[X]),(vec[Y]),(vec[Z]));
			ecart /= largest;

			// standard deviation is lesser than 0.2 so bounding sphere,
			// if in 0.2+ and and 0.5 use bounding box ext,
			// otherwise use bounding box.
			if (ecart <= 0.2f)
				m_boundingMode = BOUNDING_SPHERE;
			else if (ecart <= 0.5f)
				m_boundingMode = BOUNDING_BOXEXT;
			else
				m_boundingMode = BOUNDING_BOX;
		}
	}

	// bounding generated
	m_flags.setBit(UPDATE_BOUNDING, False);
}

// Compute the normal.
void GeometryData::genNormals(Bool generate)
{
	if (m_flags.getBit(GEN_NORMALS) != generate)
	{
		m_flags.setBit(GEN_NORMALS, generate);
		m_flags.setBit(UPDATE_NORMALS, generate);
	}
}

// Compute the tangent space (tangents and bi-tangents arrays).
void GeometryData::genTangentSpace(Bool generate)
{
	if (m_flags.getBit(GEN_TANGENT_SPACE) != generate)
	{
		m_flags.setBit(GEN_TANGENT_SPACE, generate);
		m_flags.setBit(UPDATE_TANGENT_SPACE, generate);
	}
}

// Is the tangent space (including normals) generated.
Bool GeometryData::isTangentSpace() const
{
	return !m_flags.getBit(UPDATE_TANGENT_SPACE) && !m_flags.getBit(UPDATE_NORMALS);
}

// Compute the progressive mesh for each face arrays.
void GeometryData::genProgressiveMesh(Bool generate)
{
	if (m_flags.getBit(GEN_PROGRESSIVE_MESH) != generate)
	{
		m_flags.setBit(GEN_PROGRESSIVE_MESH, generate);
		m_flags.setBit(UPDATE_PROGRESSIVE_MESH, generate);
	}
}

// Define the LOD level in percent for each face arrays.
void GeometryData::setLodLvl(UInt32 lvl)
{
	// TODO
}

// Is progressive mesh is computed for each face arrays.
Bool GeometryData::isProgressive() const
{
	return !m_flags.getBit(UPDATE_PROGRESSIVE_MESH);
}

// write to file
Bool GeometryData::writeToFile(OutStream &os)
{
    BaseObject::writeToFile(os);

	// global bounding
    os   << m_boundingMode
		 << m_AABBoxExt
		 << m_BSphere;

	// common
    os   << m_flags.getBit(GEN_TANGENT_SPACE)
		 << m_flags.getBit(GEN_PROGRESSIVE_MESH)
		 << m_flags.getBit(INTERLEAVE_ELEMENTS);

	// write elements
    os << getNumElements();
	for (Int32 i = 0; i < NUM_VERTEX_ATTRIBUTES; ++i)
	{
		if (m_elements[i])
		{
            os << m_elements[i]->getType();

            m_elements[i]->writeToFile(os);
		}
	}

	// write face array
    os << (UInt32)m_faceArrays.size();
	for (IT_FaceArrays it = m_faceArrays.begin(); it != m_faceArrays.end(); ++it)
	{
		if (it->second)
		{
            os   << it->first
				 << it->second->getTypeSize();

            it->second->writeToFile(os);
		}
	}

	return True;
}

// read from file
Bool GeometryData::readFromFile(InStream &is)
{
    BaseObject::readFromFile(is);

	m_flags.fillTrue();
	m_flags.setBit(UPDATE_PROGRESSIVE_MESH, False);

	// destroy if necessary
	for (Int32 i = 0; i < NUM_VERTEX_ATTRIBUTES; ++i)
	{
		deletePtr(m_elements[i]);
	}

	for (IT_FaceArrays it = m_faceArrays.begin(); it != m_faceArrays.end(); ++it)
	{
		deletePtr(it->second);
	}

	// global bounding
    is   >> m_boundingMode
		 >> m_AABBoxExt
		 >> m_BSphere;

	// common
	Bool boolean;

    is >> boolean;
	m_flags.setBit(GEN_TANGENT_SPACE, boolean);

    is >> boolean;
	m_flags.setBit(GEN_PROGRESSIVE_MESH, boolean);

    is >> boolean;
	m_flags.setBit(INTERLEAVE_ELEMENTS, boolean);

	// read elements
	UInt32 numElements;
    is >> numElements;
	for (UInt32 i = 0; i < numElements; ++i)
	{
		VertexAttributeArray type;
		SmartArrayFloat data;

        is >> type;

		m_elements[type] = new VertexElement(type, 0, 0);
        m_elements[type]->readFromFile(is);
	}

	// read face array
	UInt32 numFaceArray, faceArrayId, typeSize;

    is >> numFaceArray;
	for (UInt32 i = 0; i < numFaceArray; ++i)
	{
        is   >> faceArrayId
			 >> typeSize;

		if (typeSize == 2)
		{
            FaceArrayUInt16 *faceArray = new FaceArrayUInt16(getScene()->getContext());
            faceArray->readFromFile(is);

			addFaceArray(faceArrayId, faceArray);
		}
		else if (typeSize == 4)
		{
            FaceArrayUInt32 *faceArray = new FaceArrayUInt32(getScene()->getContext());
            faceArray->readFromFile(is);

			addFaceArray(faceArrayId, faceArray);
		}
	}

	return True;
}

// Generate UV element using a specified method, into the specified array
void GeometryData::computeTexCoords(
		VertexAttributeArray dest,
		TexCoordsGen method,
		const Vector3 &scale,
		UInt32 uFrom,
		UInt32 vFrom)
{
	// dest must be one of the 2d textures units
	switch (dest)
	{
		case V_TEXCOORDS_2D_1_ARRAY:
		case V_TEXCOORDS_2D_2_ARRAY:
		case V_TEXCOORDS_2D_3_ARRAY:
		case V_TEXCOORDS_2D_4_ARRAY:
		case V_TEXCOORDS_2D_5_ARRAY:
		case V_TEXCOORDS_2D_6_ARRAY:
		case V_TEXCOORDS_2D_7_ARRAY:
		case V_TEXCOORDS_2D_8_ARRAY:
		case V_TEXCOORDS_2D_9_ARRAY:
		case V_TEXCOORDS_2D_10_ARRAY:
		case V_TEXCOORDS_2D_11_ARRAY:
		case V_TEXCOORDS_2D_12_ARRAY:
			break;

		default:
			O3D_ERROR(E_InvalidParameter("Only 2d texture coordinate destination are allowed"));
			break;
	}

	if (!m_elements[V_VERTICES_ARRAY])
		O3D_ERROR(E_InvalidPrecondition("Vertices are required"));

	if (m_elements[dest])
		deletePtr(m_elements[dest]);

	const Float *vertices = m_elements[V_VERTICES_ARRAY]->lockArray(0, 0);

	if (!vertices)
		O3D_ERROR(E_InvalidPrecondition("Unable to lock vertices data array"));

	UInt32 numVertices = getNumVertices();

	// normalize method
	if (method == TEXGEN_VERTEX_NORMALIZE)
	{
		SmartArrayFloat texCoordsArray(numVertices*2);

		Vector3 vert;

		// for each vertex
		for (UInt32 i = 0; i < numVertices; ++i)
		{
			vert.set(&vertices[i*3]);
			vert[X] *= scale[X];
			vert[Y] *= scale[Y];
			vert[Z] *= scale[Z];
			vert.normalize();

			texCoordsArray[i*2] = vert[uFrom];
			texCoordsArray[i*2+1] = vert[vFrom];
		}

		m_elements[V_VERTICES_ARRAY]->unlockArray();

		// finally create the texCoord element if necessary
		newVertexElement(dest, texCoordsArray);
	}
	else if (method == TEXGEN_VERTEX_PLANAR)
	{
		SmartArrayFloat texCoordsArray(numVertices*2);

		Vector3 vert;

		// for each vertex
		for (UInt32 i = 0; i < numVertices; ++i)
		{
			vert.set(&vertices[i*3]);
			vert[X] *= scale[X];
			vert[Y] *= scale[Y];
			vert[Z] *= scale[Z];

			texCoordsArray[i*2] = vert[uFrom];
			texCoordsArray[i*2+1] = vert[vFrom];
		}

		m_elements[V_VERTICES_ARRAY]->unlockArray();

		// finally create the texCoord element if necessary
		newVertexElement(dest, texCoordsArray);
	}
	else
	{
		m_elements[V_VERTICES_ARRAY]->unlockArray();
		O3D_ERROR(E_InvalidParameter("Invalid generation method"));
	}

	m_flags.setBit(UPDATE_TANGENT_SPACE, True);
	m_flags.setBit(UPDATE_VERTEX_BUFFER, True);
}

UInt32 GeometryData::getElementSize(VertexAttributeArray mode) const
{
	// check the element size
	switch (mode)
	{
		case V_RIGGING_ARRAY:
			return 1;
	
		case V_TEXCOORDS_2D_1_ARRAY:
		case V_TEXCOORDS_2D_2_ARRAY:
		case V_TEXCOORDS_2D_3_ARRAY:
		case V_TEXCOORDS_2D_4_ARRAY:
		case V_TEXCOORDS_2D_5_ARRAY:
		case V_TEXCOORDS_2D_6_ARRAY:
		case V_TEXCOORDS_2D_7_ARRAY:
		case V_TEXCOORDS_2D_8_ARRAY:
		case V_TEXCOORDS_2D_9_ARRAY:
		case V_TEXCOORDS_2D_10_ARRAY:
		case V_TEXCOORDS_2D_11_ARRAY:
		case V_TEXCOORDS_2D_12_ARRAY:
			return 2;

		case V_VERTICES_ARRAY:
		case V_NORMALS_ARRAY:
		case V_TANGENT_ARRAY:
		case V_BITANGENT_ARRAY:
		case V_TEXCOORDS_3D_1_ARRAY:
		case V_TEXCOORDS_3D_2_ARRAY:
		case V_TEXCOORDS_3D_3_ARRAY:
		case V_TEXCOORDS_3D_4_ARRAY:
		case V_TEXCOORDS_3D_5_ARRAY:
		case V_TEXCOORDS_3D_6_ARRAY:
		case V_TEXCOORDS_3D_7_ARRAY:
		case V_TEXCOORDS_3D_8_ARRAY:
		case V_TEXCOORDS_3D_9_ARRAY:
		case V_TEXCOORDS_3D_10_ARRAY:
		case V_TEXCOORDS_3D_11_ARRAY:
		case V_TEXCOORDS_3D_12_ARRAY:
			return 3;

		case V_SKINNING_ARRAY:
		case V_WEIGHTING_ARRAY:
		case V_COLOR_ARRAY:
			return 4;

		default:
			return 0;
	}
}

// Create/Get/Recreate a VertexElement.
VertexElement* GeometryData::newVertexElement(
		VertexAttributeArray mode,
		const SmartArrayFloat &data)
{
	UInt32 eltSize = getElementSize(mode);

	if (m_elements[mode])
	{
		if ((m_elements[mode]->getNumElements() != data.getNumElt() / eltSize) ||
			(m_elements[mode]->getElementSize() != eltSize))
		{
			deletePtr(m_elements[mode]);
		}
		else
			return m_elements[mode];
	}

	m_elements[mode] = new VertexElement(mode, data, eltSize);
	return m_elements[mode];
}

// Build from an Primitive object
void GeometryData::buildFromPrimitive(const Primitive &primitive)
{
	// set elements
	newVertexElement(
			V_VERTICES_ARRAY,
			SmartArrayFloat(primitive.getVertices(), primitive.getNumVertices() * 3));

	// set texture coordinates
	if (primitive.isTexCoords())
	{
		newVertexElement(
				V_TEXCOORDS_2D_1_ARRAY,
				SmartArrayFloat(primitive.getTexCoords(), primitive.getNumVertices() * 2));
	}

	// set face array
	if (primitive.getNumVertices() < 65536)
	{
		SmartArrayUInt16 faceData(primitive.getNumIndices());
		const UInt32 *primitiveFace = primitive.getFacesIndices();

		for (UInt32 i = 0; i < primitive.getNumIndices(); ++i)
			faceData[i] = (UInt16) primitiveFace[i];

        FaceArrayUInt16 *facesArray = new FaceArrayUInt16(getScene()->getContext(), primitive.getFaceType());
		facesArray->setFaces(faceData);

		addFaceArray(0, facesArray);
	}
	else
	{
		SmartArrayUInt32 faceData(primitive.getFacesIndices(), primitive.getNumIndices());

        FaceArrayUInt32 *facesArray = new FaceArrayUInt32(getScene()->getContext(), primitive.getFaceType());
		facesArray->setFaces(faceData);

		addFaceArray(0, facesArray);
	}
}

// Compute normals
void GeometryData::computeNormals()
{
	if (!m_elements[V_VERTICES_ARRAY])
		O3D_ERROR(E_InvalidPrecondition("Vertices are required"));

	if (m_elements[V_NORMALS_ARRAY])
		deletePtr(m_elements[V_NORMALS_ARRAY]);

	const Float *vertices = m_elements[V_VERTICES_ARRAY]->lockArray(0, 0);

	if (!vertices)
		O3D_ERROR(E_InvalidPrecondition("Unable to lock vertices data array"));

	UInt32 numVertices = getNumVertices();

	UInt32 i3;

	Vector3 *tmpNormals = new Vector3[numVertices];
	Vector3 v1,v2,r;

	// for each face array
	for (IT_FaceArrays it = m_faceArrays.begin(); it != m_faceArrays.end(); ++it)
	{
		if (it->second->getNumElements() == 0)
			continue;

		FaceArrayVisitor triangles(it->second);

		for (FaceArrayIterator it = triangles.begin(); it != triangles.end(); ++it)
		{
			Vector3 p1(&vertices[it.a*3]);
			Vector3 p2(&vertices[it.b*3]);
			Vector3 p3(&vertices[it.c*3]);

			v1 = p3 - p2; // C-B or v1 = p2 - p1; // B-A
			v2 = p1 - p2; // A-B or v2 = p3 - p1; // C-A

			r = v1 ^ v2;
			r.normalize();

			tmpNormals[it.a] += r;
			tmpNormals[it.b] += r;
			tmpNormals[it.c] += r;
		}
	}

	m_elements[V_VERTICES_ARRAY]->unlockArray();

	SmartArrayFloat normals(numVertices*3);

	// Normalize computed normals
	for (UInt32 i = 0; i < numVertices; ++i)
    {
		i3 = i*3;

		tmpNormals[i].normalize();
		normals[i3]   = tmpNormals[i][X];
		normals[i3+1] = tmpNormals[i][Y];
		normals[i3+2] = tmpNormals[i][Z];
	}

	deleteArray(tmpNormals);

	// finally create the normals element if necessary
	newVertexElement(V_NORMALS_ARRAY, normals);

	m_flags.setBit(UPDATE_NORMALS, False);
	m_flags.setBit(UPDATE_VERTEX_BUFFER, True);
}

// Compute tangent vector (used by ComputeTangentSpace)
static void computeTangentVector(
	UInt32 a,
	UInt32 b,
	UInt32 c,
	Vector3 &sTangent,
	Vector3 &tTangent,
	const Float* pVertices,
	const Float* pTexCoords,
	const Float* pNormals)
{
	Vector3 vA(&pVertices[a*3]);
	Vector3 vB(&pVertices[b*3]);
	Vector3 vC(&pVertices[c*3]);

	Vector2f tA(&pTexCoords[a*2]);
	Vector2f tB(&pTexCoords[b*2]);
	Vector2f tC(&pTexCoords[c*2]);
/*
	Vector3 Side0 = vA - vB;
	Vector3 Side1 = vC - vB;

	// Normal
	Vector3 Normal = Side1 ^ Side0;
	Normal.normalize();

	// s tangent
	Float deltaT0 = tA[Y] - tB[Y];
	Float deltaT1 = tC[Y] - tB[Y];

	sTangent = Side0 * deltaT1 - Side1 * deltaT0;
	sTangent.normalize();

	// t tangent
	Float deltaS0 = tA[X] - tB[X];
	Float deltaS1 = tC[X] - tB[X];

	tTangent = Side0 * deltaS1 - Side1 * deltaS0;
	tTangent.normalize();

	// is tex coord inversion needed ?
	Vector3 CrossTangent = sTangent ^ tTangent;

	if (CrossTangent * Normal < 0.f)
	{
		sTangent = -sTangent;
		tTangent = -tTangent;
	}
*/
	Vector3 nA(&pNormals[a*3]);

	Vector3 vAB = vB - vA;
	Vector3 vAC = vC - vA;

	Vector3 vProjAB = vAB - (nA * (nA * vAB));
	Vector3 vProjAC = vAC - (nA * (nA * vAC));

	// s tangent
	Float deltaT0 = tB[Y] - tA[Y];
	Float deltaT1 = tC[Y] - tA[Y];

	// t tangent
	Float deltaS0 = tB[X] - tA[X];
	Float deltaS1 = tC[X] - tA[X];

	Float scale = 0.f;
	Float delta = ((deltaS0 * deltaT1) - (deltaS1 * deltaT0));
	if (delta != 0.f) scale = 1.f / delta;

	sTangent = (vProjAB * deltaT1 - vProjAC * deltaT0) * scale;
	sTangent.normalize();

	tTangent = (vProjAC * deltaS0 - vProjAB * deltaS1) * scale;
	tTangent.normalize();
}

// Compute tangents and bi-tangents
void GeometryData::computeTangentSpace()
{
	if (!m_elements[V_VERTICES_ARRAY])
		O3D_ERROR(E_InvalidPrecondition("Vertices are required"));

	if (!m_elements[V_NORMALS_ARRAY])
		computeNormals();
		//O3D_ERROR(O3D_E_InvalidPrecondition("Normals are required"));

	if (!m_elements[V_TEXCOORDS_2D_1_ARRAY])
		O3D_ERROR(E_InvalidPrecondition("Primary texture coordinates unit are required"));

	if (m_elements[V_TANGENT_ARRAY])
		deletePtr(m_elements[V_TANGENT_ARRAY]);

	if (m_elements[V_BITANGENT_ARRAY])
		deletePtr(m_elements[V_BITANGENT_ARRAY]);

	const Float *vertices  = m_elements[V_VERTICES_ARRAY]->lockArray(0, 0);
	const Float *normals   = m_elements[V_NORMALS_ARRAY]->lockArray(0, 0);
	const Float *texCoords = m_elements[V_TEXCOORDS_2D_1_ARRAY]->lockArray(0, 0);

	if (!vertices || !normals || !texCoords)
	{
		O3D_ERROR(E_InvalidPrecondition("Unable to lock data array"));

		if (vertices)
			m_elements[V_VERTICES_ARRAY]->unlockArray();
		if (normals)
			m_elements[V_NORMALS_ARRAY]->unlockArray();
		if (texCoords)
			m_elements[V_TEXCOORDS_2D_1_ARRAY]->unlockArray();
	}

	UInt32 numVertices = getNumVertices();

	Vector3 tangent,bitangent;

	UInt32 i3;

	Vector3 *tan1 = new Vector3[numVertices*2];
	Vector3 *tan2 = tan1 + numVertices;

	// for each face array
	for (IT_FaceArrays it = m_faceArrays.begin(); it != m_faceArrays.end(); ++it)
	{
		if (it->second->getNumElements() == 0)
			continue;

		FaceArrayVisitor triangles(it->second);

		for (FaceArrayIterator it = triangles.begin(); it != triangles.end(); ++it)
		{
			computeTangentVector(it.a, it.b, it.c, tangent, bitangent, vertices, texCoords, normals);

			tan1[it.a] += tangent;
			tan1[it.b] += tangent;
			tan1[it.c] += tangent;

			tan2[it.a] += bitangent;
			tan2[it.b] += bitangent;
			tan2[it.c] += bitangent;

			// We use += because we want to average the tangent vectors with
			// neighboring triangles that share vertices.
			//ComputeTangentVector(a,b,c,tangent,bitangent,vertices,texCoords,normals);
			//tan1[a] += tangent;
			//tan2[a] += bitangent;

			//ComputeTangentVector(b,c,a,tangent,bitangent,vertices,texCoords,normals);
			//tan1[b] += tangent;
			//tan2[b] += bitangent;

			//ComputeTangentVector(c,a,b,tangent,bitangent,vertices,texCoords,normals);
			//tan1[c] += tangent;
			//tan2[c] += bitangent;
		}
	}

	m_elements[V_VERTICES_ARRAY]->unlockArray();
	m_elements[V_TEXCOORDS_2D_1_ARRAY]->unlockArray();
	m_elements[V_NORMALS_ARRAY]->unlockArray();

	SmartArrayFloat tangents(numVertices*3);
	SmartArrayFloat bitangents(numVertices*3);

	// Normalize et set tangents and binormals
	for (UInt32 i = 0; i < numVertices; ++i)
	{
		i3 = i*3;

		tan1[i].normalize();
		tangents[i3]   = tan1[i][X];
		tangents[i3+1] = tan1[i][Y];
		tangents[i3+2] = tan1[i][Z];

		tan2[i].normalize();
		bitangents[i3]   = tan2[i][X];
		bitangents[i3+1] = tan2[i][Y];
		bitangents[i3+2] = tan2[i][Z];
	}

	deleteArray(tan1);

	// finally create the tangent and bitangent element if necessary
	newVertexElement(V_TANGENT_ARRAY, tangents);
	newVertexElement(V_BITANGENT_ARRAY, bitangents);

	m_flags.setBit(UPDATE_TANGENT_SPACE, False);
	m_flags.setBit(UPDATE_VERTEX_BUFFER, True);
}

// Compute the progressive mesh for each face arrays.
void GeometryData::computeProgressive()
{
	if (!m_elements[V_VERTICES_ARRAY])
		O3D_ERROR(E_InvalidPrecondition("Vertices are required"));

	const Float *vertices = m_elements[V_VERTICES_ARRAY]->lockArray(0, 0);

	if (!vertices)
		O3D_ERROR(E_InvalidPrecondition("Unable to lock vertices data array"));

	UInt32 numVertices = getNumVertices();

	// for each face array
	for (IT_FaceArrays it = m_faceArrays.begin(); it != m_faceArrays.end(); ++it)
	{
		FaceArray *faceArray = it->second;

		// No faces to process...
		if (faceArray->getNumElements() == 0)
			continue;

		// Process progressive mesh only on TRIANGLES mesh
		if (faceArray->getFormat() != P_TRIANGLES)
			continue;

		ProgressiveMesh progMesh;

		UInt8 *triangles = faceArray->lockArray(0, 0, VertexBuffer::READ_ONLY);

		if (!triangles)
			O3D_ERROR(E_InvalidPrecondition("Unable to lock face data array"));

		UInt32 numTriangles = faceArray->getNumFaces();

		// compute the collapse map
		if (faceArray->getTypeSize() == sizeof(UInt32))
		{
			FaceArrayUInt32 *faceArray32 = reinterpret_cast<FaceArrayUInt32*>(faceArray);

			progMesh.progressiveMesh(
					vertices,
					numVertices,
					reinterpret_cast<UInt32*>(triangles),
					numTriangles,
					faceArray32->getCollapseMap(),
					faceArray32->getPermutation());
		}
		else if (faceArray->getTypeSize() == sizeof(UInt16))
		{
			FaceArrayUInt16 *faceArray16 = reinterpret_cast<FaceArrayUInt16*>(faceArray);

			progMesh.progressiveMesh(
					vertices,
					numVertices,
					reinterpret_cast<UInt16*>(triangles),
					numTriangles,
					faceArray16->getCollapseMap(),
					faceArray16->getPermutation());
		}

		faceArray->unlockArray();
	}

	m_elements[V_VERTICES_ARRAY]->unlockArray();

	m_flags.setBit(UPDATE_PROGRESSIVE_MESH, False);
	//m_flags.setBit(UPDATE_VERTEX_BUFFER, True);
	//m_flags.setBit(UPDATE_INDEX_BUFFER, True);
}

