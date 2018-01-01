/**
 * @file mesh.cpp
 * @brief Implementation of Mesh.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-05-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/engine/object/mesh.h"

#include "o3d/engine/object/meshdatamanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/object/primitivemanager.h"
#include "o3d/engine/object/edgearray.h"
#include "o3d/engine/alphapipeline.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/hierarchy/node.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/geom/frustum.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Mesh, ENGINE_MESH, ShadableObject)

// Default constructor.
Mesh::Mesh(BaseObject *parent) :
	ShadableObject(parent),
	m_meshData(this),
	m_faceArrayId(0),
	m_boundingDirty(True)
{

	setUpdatable(True);
	setDrawable(True);
	setPickable(True);
	setAnimatable(True);
	setShadable(True);
}

// Copy constructor.
Mesh::Mesh(const Mesh &dup):
	ShadableObject(dup),
	m_meshData(this,dup.m_meshData),
	m_faceArrayId(0),
	m_boundingDirty(True)
{
	// duplicate any profiles
	UInt32 numProfiles = static_cast<UInt32>(dup.m_matProfiles.size());
	setNumMaterialProfiles(numProfiles);

    for (UInt32 i = 0; i < numProfiles; ++i) {
		*m_matProfiles[i] = *dup.m_matProfiles[i];
    }
}

// Virtual destructor.
Mesh::~Mesh()
{
	// delete any profiles
	UInt32 numProfiles = static_cast<UInt32>(m_matProfiles.size());

    for (UInt32 i = 0; i < numProfiles; ++i) {
		deletePtr(m_matProfiles[i]);
    }
}

// Copy operator.
Mesh& Mesh::operator=(const Mesh &dup)
{
	ShadableObject::operator=(dup);

	m_meshData = dup.m_meshData;
	m_faceArrayId = 0;

	m_boundingBox.destroy();
	m_boundingDirty = True;

	// duplicate any profiles
	UInt32 numProfiles = static_cast<UInt32>(dup.m_matProfiles.size());
	setNumMaterialProfiles(numProfiles);

    for (UInt32 i = 0; i < numProfiles; ++i) {
		*m_matProfiles[i] = *dup.m_matProfiles[i];
    }

	return *this;
}

// Set the meshdata.
void Mesh::setMeshData(MeshData *meshData)
{
	m_meshData = meshData;
	m_faceArrayId = 0;

	m_boundingDirty = True;
}

// Set the number of material profile (and number of sub-mesh to profile).
void Mesh::setNumMaterialProfiles(UInt32 numProfiles)
{
	// delete more techniques
    if (m_matProfiles.size() > static_cast<size_t>(numProfiles)) {
        for (size_t i = numProfiles; i < m_matProfiles.size(); ++i) {
			deletePtr(m_matProfiles[i]);
        }
	}

	size_t oldSize = m_matProfiles.size();
	m_matProfiles.resize(numProfiles);

	// add more techniques
    if (oldSize < static_cast<size_t>(numProfiles)) {
        for (size_t i = oldSize; i < numProfiles; ++i) {
			m_matProfiles[i] = new MaterialProfile(this);
        }
	}
}

// Get the profile at a specified index (correspond to a sub-mesh index) (read only).
const MaterialProfile& Mesh::getMaterialProfile(UInt32 index) const
{
    if (static_cast<size_t>(index) >= m_matProfiles.size()) {
		O3D_ERROR(E_IndexOutOfRange("Profile index"));
    }

	return *m_matProfiles[index];
}

// Get the profile at a specified index (correspond to a sub-mesh index).
MaterialProfile& Mesh::getMaterialProfile(UInt32 index)
{
    if (static_cast<size_t>(index) >= m_matProfiles.size()) {
		O3D_ERROR(E_IndexOutOfRange("Profile index"));
    }
	
	return *m_matProfiles[index];	
}

// Initialize any material profiles.
void Mesh::initMaterialProfiles()
{
	UInt32 numProfiles = static_cast<UInt32>(m_matProfiles.size());

    for (UInt32 i = 0; i < numProfiles; ++i) {
		m_matProfiles[i]->prepareAndCompile(*this);
    }
}

// Check the global bounding box with an AABBox
Geometry::Clipping Mesh::checkBounding(const AABBox &bbox) const
{
    if (m_meshData && m_meshData->getGeometry()) {
		GeometryData::BoundingMode mode = m_meshData->getGeometry()->getBoundingMode();

        if (mode == GeometryData::BOUNDING_BOXEXT) {
			// use the world space bounding box, that was previously
			// transformed on an update
            if (bbox.include(m_boundingBox)) {
				return Geometry::CLIP_INSIDE;
            } else if (bbox.intersect(m_boundingBox)) {
				return Geometry::CLIP_INTERSECT;
            } else {
				return Geometry::CLIP_OUTSIDE;
            }
        } else if (mode == GeometryData::BOUNDING_BOX) {
			// use the world space bounding box, that was previously
			// transformed on an update
            if (bbox.include(*reinterpret_cast<const AABBox*>(&m_boundingBox))) {
				return Geometry::CLIP_INSIDE;
            } else if (bbox.intersect(*reinterpret_cast<const AABBox*>(&m_boundingBox))) {
				return Geometry::CLIP_INTERSECT;
            } else {
				return Geometry::CLIP_OUTSIDE;
            }
        } else if (mode == GeometryData::BOUNDING_SPHERE) {
			// use the world space bounding sphere, that was previously
			// transformed on an update
            if (bbox.include(m_boundingSphere)) {
				return Geometry::CLIP_INSIDE;
            } else if (bbox.intersect(m_boundingSphere)) {
				return Geometry::CLIP_INTERSECT;
            } else {
				return Geometry::CLIP_OUTSIDE;
            }
		}
	}

	return Geometry::CLIP_OUTSIDE;
}

// Check the global bounding box with an infinite plane
Geometry::Clipping Mesh::checkBounding(const Plane &plane) const
{
    if (m_meshData && m_meshData->getGeometry()) {
		GeometryData::BoundingMode mode = m_meshData->getGeometry()->getBoundingMode();

        if (mode == GeometryData::BOUNDING_BOXEXT) {
			// use the world space bounding box, that was previously
			// transformed on an update
			Bool front = m_boundingBox.clipFront(plane);
			Bool back = m_boundingBox.clipBack(plane);

            if (!front && back) {
				return Geometry::CLIP_INSIDE;
            } else if (front && back) {
				return Geometry::CLIP_INTERSECT;
            } else {
				return Geometry::CLIP_OUTSIDE;
            }
        } else if (mode == GeometryData::BOUNDING_BOX) {
			// use the world space bounding box, that was previously
			// transformed on an update
			Bool front = reinterpret_cast<const AABBox*>(&m_boundingBox)->clipFront(plane);
			Bool back = reinterpret_cast<const AABBox*>(&m_boundingBox)->clipBack(plane);

            if (!front && back) {
				return Geometry::CLIP_INSIDE;
            } else if (front && back) {
				return Geometry::CLIP_INTERSECT;
            } else {
				return Geometry::CLIP_OUTSIDE;
            }
        } else if (mode == GeometryData::BOUNDING_SPHERE) {
            // use the world space bounding sphere, that was previously
			// transformed on an update
			Bool front = m_boundingSphere.clipFront(plane);
			Bool back = m_boundingSphere.clipBack(plane);

            if (!front && back) {
				return Geometry::CLIP_INSIDE;
            } else if (front && back) {
				return Geometry::CLIP_INTERSECT;
            } else {
				return Geometry::CLIP_OUTSIDE;
            }
		}
	}

	return Geometry::CLIP_OUTSIDE;
}

// Check the global bounding volume with the frustum.
Geometry::Clipping Mesh::checkFrustum(const Frustum &frustum) const
{
    if (m_meshData && m_meshData->getGeometry()) {
		GeometryData::BoundingMode mode = m_meshData->getGeometry()->getBoundingMode();

        if (mode == GeometryData::BOUNDING_BOXEXT) {
			// use the world space bounding box, that was previously
			// transformed on an update
			return frustum.boxInFrustum(m_boundingBox);
        } else if (mode == GeometryData::BOUNDING_BOX) {
			// use the world space bounding box, that was previously
			// transformed on an update
			return frustum.boxInFrustum(*reinterpret_cast<const AABBox*>(&m_boundingBox));
        } else if (mode == GeometryData::BOUNDING_SPHERE) {
			// use the world space bounding sphere, that was previously
			// transformed on an update
			return frustum.sphereInFrustum(
					m_boundingSphere.getCenter(),
					m_boundingSphere.getRadius());
		}
	}

	return Geometry::CLIP_INSIDE;
}

// Get the drawing type
UInt32 Mesh::getDrawType() const
{
	return Scene::DRAW_MESH;
}

// Update pass
void Mesh::update()
{
	clearUpdated();

    if (m_node && (m_node->hasUpdated())) {
		m_boundingDirty = True;
		setUpdated();
	}

    if (m_boundingDirty) {
		updateBounding();
    }
}

// Update the global bounding volume.
void Mesh::updateBounding()
{
    if (!m_boundingDirty) {
		return;
    }

    if (m_meshData && m_meshData->getGeometry()) {
        if (m_node) {
            if (m_meshData->getGeometry()->getBoundingMode() == GeometryData::BOUNDING_SPHERE) {
				m_meshData->getGeometry()->getBSphere().applyTransform(
						m_node->getAbsoluteMatrix(),
						m_boundingSphere);
            } else {
				m_localBoundingBox = m_meshData->getGeometry()->getAABBoxExt();
				m_boundingBox = m_localBoundingBox.transformTo(m_node->getAbsoluteMatrix());
			}
        } else {
            if (m_meshData->getGeometry()->getBoundingMode() == GeometryData::BOUNDING_SPHERE) {
				m_boundingSphere = m_meshData->getGeometry()->getBSphere();
            } else {
				m_boundingBox = m_localBoundingBox = m_meshData->getGeometry()->getAABBoxExt();
            }
		}
    } else {
		m_localBoundingBox.destroy();
		m_boundingBox.destroy();
	}

	m_boundingDirty = False;
}

// Draw the symbolics objects.
void Mesh::drawSymbolics()
{
	// Draw primitives
	if (getScene()->getDrawObject(Scene::DRAW_BOUNDING_VOLUME) ||
        getScene()->getDrawObject(Scene::DRAW_MESH_LOCAL_AXIS)) {

		PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access();

        // camera space @todo with current modelview, push pop...
		getScene()->getContext()->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());

		// draw its global bounding volume
        if (getScene()->getDrawObject(Scene::DRAW_BOUNDING_VOLUME)) {
			GeometryData::BoundingMode mode = m_meshData->getGeometry()->getBoundingMode();

            if (mode == GeometryData::BOUNDING_BOXEXT) {
				// use the world space bounding box, that was previously
				// transformed on an update
				primitive->boundingBox(m_boundingBox);
            } else if (mode == GeometryData::BOUNDING_BOX) {
				// use the world space bounding box, that was previously
				// transformed on an update
				primitive->boundingBox(*reinterpret_cast<AABBox*>(&m_boundingBox));
            } else if (mode == GeometryData::BOUNDING_SPHERE) {
				// use the world space bounding sphere, that was previously
				// transformed on an update
				primitive->boundingSphere(m_boundingSphere);
			}
		}

		// draw local axis
        if (getScene()->getDrawObject(Scene::DRAW_MESH_LOCAL_AXIS)) {
			// object space
			getScene()->getContext()->modelView().mult(m_node->getAbsoluteMatrix());

			primitive->drawLocalAxis();
		}
	}
}

// Draw pass
void Mesh::draw(const DrawInfo &drawInfo)
{
    if (!getActivity() || !getVisibility()) {
		return;
    }

	// mesh data ?
    if (!m_meshData || !m_meshData->getGeometry()) {
		return;
    }

	// draw mesh ?
    if (!getScene()->getDrawObject((Scene::DrawObjectType)getDrawType())) {
		return;
    }

	// draw symbolics
    if (drawInfo.pass == DrawInfo::AMBIENT_PASS) {
		drawSymbolics();
    }

	// distance of the mesh from the camera
	Float squaredDistance = getDistanceFrom(
			getScene()->getActiveCamera()->getAbsoluteMatrix().getTranslation());

	// bind the geometry data
	m_meshData->getGeometry()->bindVertexBuffer();

	// for each sub-mesh
	MaterialProfile *profile;
	UInt32 numProfiles = static_cast<UInt32>(m_matProfiles.size());

    // setup modelview matrix
    setUpModelView();

    for (UInt32 i = 0; i < numProfiles; ++i) {
		profile = m_matProfiles[i];

        if (profile && profile->getActivity()) {
			MaterialTechnique *technique = profile->getBestTechnique(squaredDistance);

			// bind the face array for the current sub-mesh
			m_meshData->getGeometry()->bindFaceArray(i);
			m_faceArrayId = i;

			// If picking pass, force in internal array mode
            if (drawInfo.pass == DrawInfo::PICKING_PASS) {
                m_shadableInfo.faceArray = nullptr;
            }

			// process the technique
            if (technique) {
				technique->processMaterial(*this, this, this, drawInfo);
            }

			// draw the symbolic only on AMBIENT_PASS
            if (drawInfo.pass == DrawInfo::AMBIENT_PASS) {
				// processing the rendering of local space immediately
                if (getScene()->getDrawObject(Scene::DRAW_LOCAL_SPACE)) {
					m_meshData->getGeometry()->drawLocalSpace();
                }
			}
		}
	}
}

// Process all faces
void Mesh::processAllFaces(Shadable::ProcessingPass pass)
{
    if (!m_meshData) {
		return;
    }

    if (pass == Shadable::PREPARE_GEOMETRY) {
		// bind the external face array
        if (isExternalFaceArray()) {
			UInt32 first,last;
			FaceArray *faceArray = getFaceArrayToProcess(first, last);

            if (faceArray) {
				// bind an external face array before process
				m_meshData->getGeometry()->bindExternalFaceArray(faceArray);
			}
		}

        // @todo VAOs and same for skin/rigg
    } else if (pass == Shadable::PROCESS_GEOMETRY) {
		// external face array
        if (isExternalFaceArray()) {
			UInt32 first, last;
			FaceArray *faceArray = getFaceArrayToProcess(first, last);

            if (faceArray) {
				// draw the external face array
				m_meshData->getGeometry()->drawPart(faceArray, first, last);
			}

			// and reset for the next process
			useInternalFaceArray();
        } else {
            // internal face array
			// draw the bound face array
			m_meshData->getGeometry()->draw();
		}
    }
}

VertexElement* Mesh::getVertexElement(VertexAttributeArray type) const
{
    if (m_meshData) {
		return m_meshData->getGeometry()->getElement(type);
    } else {
        return nullptr;
    }
}

FaceArray* Mesh::getFaceArray() const
{
    if (m_shadableInfo.faceArray) {
		return m_shadableInfo.faceArray;
    } else if (m_meshData) {
		return m_meshData->getGeometry()->getFaceArray(m_faceArrayId);
    } else {
        return nullptr;
    }
}

// Make an operation.
void Mesh::operation(Operations what)
{
	O3D_ASSERT(m_meshData.isValid());

    if ((what == TANGENT_SPACE) && m_meshData.isValid()) {
		m_meshData->genTangentSpace();
    } else if ((what == NORMALS) && m_meshData.isValid()) {
		m_meshData->genNormals();
    } else if ((what == CREATE) && m_meshData.isValid()) {
		m_meshData->createGeometry();
    }
}

// Is an operation is already done. For example ask if the tangent space is defined,
Bool Mesh::isOperation(Operations what) const
{
	O3D_ASSERT(m_meshData.isValid());

    if ((what == TANGENT_SPACE) && m_meshData.isValid() && m_meshData->getGeometry()) {
		return m_meshData->getGeometry()->isTangentSpace();
    } else if ((what == NORMALS) && m_meshData.isValid() && m_meshData->getGeometry()) {
		return m_meshData->getGeometry()->isNormals();
    } else if ((what == CREATE) && m_meshData.isValid() && m_meshData->getGeometry()) {
		return m_meshData->getGeometry()->isValid();
    } else {
		return False;
    }
}

// Get the matrix array for hardware skinning/rigging
const Float* Mesh::getMatrixArray() const
{
    return nullptr;
}

// Get the minimal square distance of the object bounding volume from the given point.
Float Mesh::getDistanceFrom(const Vector3 &point) const
{
    if (m_meshData && m_meshData->getGeometry()) {
        if (m_meshData->getGeometry()->getBoundingMode() == GeometryData::BOUNDING_SPHERE) {
			return (point - m_boundingSphere.getCenter()).squareLength() - o3d::sqr(m_boundingSphere.getRadius());
        } else {
			return (point - m_boundingBox.clamp(point)).squareLength() - o3d::sqr(m_boundingBox.getRadius());
        }
    } else {
		return 0.0f;
    }
}

// bind a specified array
void Mesh::attribute(VertexAttributeArray mode, UInt32 location)
{
    if (m_meshData) {
		m_meshData->getGeometry()->attribute(mode, location);
    }
}

#include "o3d/engine/object/light.h"
#include "o3d/engine/object/facearrayvisitor.h"
#include "o3d/engine/shader/shadermanager.h"

// Project the silhouette according the a specific light.
void Mesh::projectSilhouette(const DrawInfo &drawInfo)
{
    if (!getActivity() || !getVisibility()) {
		return;
    }

    if (m_meshData && m_capacities.getBit(STATE_SHADOW_CASTER)) {
		FaceArray *faceArray = getFaceArray();
        if (!faceArray) {
			return;
        }

        if (!faceArray->isEdgeArray()) {
			faceArray->computeEdges();
        }

		// Get the edges for the current bound face array.
		const EdgeArray &edgeArray = faceArray->getEdgeArray();
		const EdgeArray::Edge *edges = edgeArray.getEdges();
		const UInt32 numEdges = edgeArray.getNumEdges();

		// Get the light vector.
		Vector4 lightVec;
        if (drawInfo.light.type == Light::POINT_LIGHT) {
			lightVec = m_node->getAbsoluteMatrix().invertStd() * drawInfo.light.worldPos;
        } else if (drawInfo.light.type == Light::SPOT_LIGHT) {
			lightVec = m_node->getAbsoluteMatrix().invertStd() * drawInfo.light.worldPos;
        } else if (drawInfo.light.type == Light::DIRECTIONAL_LIGHT) {
			lightVec = m_node->getAbsoluteMatrix().invertStd() * drawInfo.light.worldPos;
		}

		// Vertex elements.
		VertexElement *verticesElements = getVertexElement(V_VERTICES_ARRAY);
		const Float *vertices = verticesElements->lockArray(0, 0);

        if (!vertices) {
			return;
        }

		// Project the silhouette according to the light.
		Vector3 v1, v2, v3;
		Vector3 normal;

		Float NdotL;

		FaceArrayVisitor triangles(faceArray);
		const UInt32 numTriangles = triangles.getNumTriangles();
		ArrayUInt8 facingLight(numTriangles);

        // @todo only recompute the light has moved, the mesh has moved or its vertices (skin or geometry) was update.
        for (FaceArrayIterator it = triangles.begin(); it != triangles.end(); ++it) {
			v1.set(&vertices[it.a*3]);
			v2.set(&vertices[it.b*3]);
			v3.set(&vertices[it.c*3]);

			// Compute the normal of the current face
			normal = (v2 - v1) ^ (v3 - v1);
			normal.normalize();

			Vector4 F(normal, -(normal * v1));
			NdotL = F * lightVec;

			// light is facing the triangle
            if (NdotL > 0.f) {
				facingLight[it.id] = 1;
            }
		}

		// 4 vertices by edge is the max...
		ArrayFloat shadowVertices(numEdges*4*4);
		Vector4 v11, v22, v33, v44;

		// compute the list of triangles
        for (UInt32 i = 0; i < numEdges; ++i) {
			const EdgeArray::Edge &edge = edges[i];
			// TODO 2 triangles and not a quad

            if (edge.triangleIndex[0] != edge.triangleIndex[1]) {
                if (facingLight[edge.triangleIndex[0]] != facingLight[edge.triangleIndex[1]]) {
					//printf("%i %i\n", edge.triangleIndex[0], edge.triangleIndex[1]);

                    if (facingLight[edge.triangleIndex[0]] == 1) {
						// add vertices
						v11 = Vector4(Vector3(&vertices[edge.vertexIndex[1]*3]), 1.f);
						v22 = Vector4(Vector3(&vertices[edge.vertexIndex[0]*3]), 1.f);

						shadowVertices.pushArray(v11.getData(), 4);
						shadowVertices.pushArray(v22.getData(), 4);

						// project to infinite in light direction
						v33 = v22;
						v33[W] = 0.f;
						v44 = v11;
						v44[W] = 0.f;

						shadowVertices.pushArray(v33.getData(), 4);
						shadowVertices.pushArray(v44.getData(), 4);
                    } else {
						// add vertices
						v11 = Vector4(Vector3(&vertices[edge.vertexIndex[0]*3]), 1.f);
						v22 = Vector4(Vector3(&vertices[edge.vertexIndex[1]*3]), 1.f);

						shadowVertices.pushArray(v11.getData(), 4);
						shadowVertices.pushArray(v22.getData(), 4);

						// project to infinite in light direction
						v33 = v22;
						v33[W] = 0.f;
						v44 = v11;
						v44[W] = 0.f;

						shadowVertices.pushArray(v33.getData(), 4);
						shadowVertices.pushArray(v44.getData(), 4);
					}
				}
            } else {
                if (facingLight[edge.triangleIndex[0]] == 1) {
					/*// add vertices
					// add vertices
					v11 = O3DVector4(O3DVector3(&vertices[edge.vertexIndex[1]*3]), 1.f);
					v22 = O3DVector4(O3DVector3(&vertices[edge.vertexIndex[0]*3]), 1.f);

					shadowVertices.pushArray(v11.getData(), 4);
					shadowVertices.pushArray(v22.getData(), 4);

					// project to infinite in light direction
					v33 = v22;
					v33[W] = 0.f;
					v44 = v11;
					v44[W] = 0.f;

					shadowVertices.pushArray(v33.getData(), 4);
					shadowVertices.pushArray(v44.getData(), 4);*/
                } else {
					/*// add vertices
					v11 = O3DVector4(O3DVector3(&vertices[edge.vertexIndex[0]*3]), 1.f);
					v22 = O3DVector4(O3DVector3(&vertices[edge.vertexIndex[1]*3]), 1.f);

					shadowVertices.pushArray(v11.getData(), 4);
					shadowVertices.pushArray(v22.getData(), 4);

					// project to infinite in light direction
					v33 = v22;
					v33[W] = 0.f;
					v44 = v11;
					v44[W] = 0.f;

					shadowVertices.pushArray(v33.getData(), 4);
					shadowVertices.pushArray(v44.getData(), 4);*/
				}
			}
		}

		// TODO Add caps, and light position in the shadow volume detection

		verticesElements->unlockArray();

		getScene()->getContext()->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());
		getScene()->getContext()->modelView().mult(m_node->getAbsoluteMatrix());

		// TODO bind the shader into the scene once
		Shader *shader = getScene()->getShaderManager()->addShader("shadowVolume");
		ShaderInstance shaderInstance;
		shaderInstance.attach(shader);
        shaderInstance.assign(0, 0);

		shaderInstance.bindShader();

		Int32 a_vertex = shaderInstance.getAttributeLocation("a_vertex");
		Int32 u_lightPos = shaderInstance.getUniformLocation("u_lightPos");
		Int32 u_modelViewProjectionMatrix = shaderInstance.getUniformLocation("u_modelViewProjectionMatrix");

		shaderInstance.setConstMatrix4(u_modelViewProjectionMatrix, False, getScene()->getContext()->modelViewProjection());
		shaderInstance.setConstVector4(u_lightPos, lightVec);

		getScene()->getContext()->enableVertexAttribArray(a_vertex);
		getScene()->getContext()->bindVertexBuffer(0);
		glVertexAttribPointer(
				a_vertex,
				4,
				GL_FLOAT,
				GL_FALSE,
				0,
				shadowVertices.getData());

		// no caps
        if (1) {
			getScene()->getContext()->setCullingMode(CULLING_BACK_FACE);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

			// TODO 2 triangles
			//getScene()->drawArrays(GL_QUADS, 0, shadowVertices.getSize()/4);

			getScene()->getContext()->setCullingMode(CULLING_FRONT_FACE);
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);

			// TODO 2 triangles
			//getScene()->drawArrays(GL_QUADS, 0, shadowVertices.getSize()/4);
/*
			glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
			getScene()->getContext()->setCullingMode(O3D_CULLING_NONE);

			glActiveStencilFaceEXT(GL_BACK);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP);

			glActiveStencilFaceEXT(GL_FRONT);
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR_WRAP);

			// TODO 2 triangles
			//getScene()->drawArrays(GL_QUADS, 0, shadowVertices.getSize()/4);

			glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
*/		} else {
            // caps
			getScene()->getContext()->setCullingMode(CULLING_FRONT_FACE);
			glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);

			// TODO 2 triangles
			//getScene()->drawArrays(GL_QUADS, 0, shadowVertices.getSize()/4);

			getScene()->getContext()->setCullingMode(CULLING_BACK_FACE);
			glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

			// TODO 2 triangles
			//getScene()->drawArrays(GL_QUADS, 0, shadowVertices.getSize()/4);
/*
			glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
			getScene()->getContext()->setCullingMode(O3D_CULLING_NONE);

			glActiveStencilFaceEXT(GL_BACK); // GL_FRONT
			glStencilOp(GL_KEEP, GL_INCR_WRAP, GL_KEEP);

			glActiveStencilFaceEXT(GL_FRONT); // GL_BACK
			glStencilOp(GL_KEEP, GL_DECR_WRAP, GL_KEEP);

			// TODO 2 triangles
			//getScene()->drawArrays(GL_QUADS, 0, shadowVertices.getSize()/4);

			glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
*/		}

		getScene()->getContext()->disableVertexAttribArray(a_vertex);
		shaderInstance.unbindShader();

		getScene()->getContext()->setCullingMode(CULLING_BACK_FACE);
	}
}

// serialization
Bool Mesh::writeToFile(OutStream &os)
{
    if (!m_meshData) {
        O3D_ERROR(E_InvalidPrecondition("Undefined meshdata"));
    }

    if (!ShadableObject::writeToFile(os)) {
		return False;
    }

	// write used meshdata info
    os << *m_meshData.get();

	// write any material profiles
	UInt32 numProfiles = static_cast<UInt32>(m_matProfiles.size());
    os << numProfiles;

    for (UInt32 i = 0; i < numProfiles; ++i) {
        os << *m_matProfiles[i];
    }

	return True;
}

Bool Mesh::readFromFile(InStream &is)
{
    if (!ShadableObject::readFromFile(is)) {
		return False;
    }

	// load the meshdata
    m_meshData = getScene()->getMeshDataManager()->readMeshData(is);

	// not found ? then error
    if (!m_meshData) {
        O3D_ERROR(E_InvalidParameter("Unable to load the MeshData"));
    }

	// read any material profiles
	UInt32 numProfiles;
    is >> numProfiles;

	setNumMaterialProfiles(numProfiles);

    for (UInt32 i = 0; i < numProfiles; ++i) {
        is >> *m_matProfiles[i];
    }

	m_faceArrayId = 0;

	return True;
}
