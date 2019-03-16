/**
 * @file skin.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/skin.h"

#include "o3d/engine/object/humanoid.h"
#include "o3d/engine/hierarchy/node.h"
#include "o3d/core/classfactory.h"
#include "o3d/engine/context.h"
#include "o3d/core/xmldoc.h"
#include "o3d/core/filemanager.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/scene/sceneobjectmanager.h"
#include "o3d/engine/object/cloth.h"
#include "o3d/engine/object/camera.h"

using namespace o3d;

//
// Skin
//

O3D_IMPLEMENT_ABSTRACT_CLASS1(Skin, ENGINE_SKIN, Mesh)

Skin::Skin(BaseObject *pParent) :
    Mesh(pParent),
    m_skinType(UNDEFINED_TYPE),
    m_vertexBlend(nullptr),
    m_recompute(False),
    m_boundingAutoRegen(False),
    m_isSkinning(False),
    m_numBones(0),
    m_bones(nullptr),
    m_skeleton(new Skeleton(this)),
    m_skinMatrices(MAX_SKINNING_MATRIX_ARRAY),
    m_boneImportName(nullptr),
    m_boneImportId(nullptr),
    m_isPrecomputed(False),
    m_useHardware(True)
{
}

Rigging::Rigging(BaseObject *pParent) :
    Skin(pParent)
{
    m_skinType = RIGGING_TYPE;
    m_shadableInfo.vertexProgram = Shadable::VP_RIGGING;
}

Skinning::Skinning(BaseObject *pParent) :
    Skin(pParent)
{
    m_skinType = SKINNING_TYPE;
    m_shadableInfo.vertexProgram = Shadable::VP_SKINNING;
}

Skin::Skin(const Skin& dup) :
    Mesh(dup),
    m_skinType(dup.m_skinType),
    m_vertexBlend(nullptr),
    m_recompute(False),
    m_boundingAutoRegen(dup.m_boundingAutoRegen),
    m_isSkinning(dup.m_isSkinning),
    m_numBones(0),
    m_bones(nullptr),
    m_skeleton(new Skeleton(this)),
    m_skinMatrices(MAX_SKINNING_MATRIX_ARRAY),
    m_boneImportName(nullptr),
    m_boneImportId(nullptr),
    m_isPrecomputed(dup.m_isPrecomputed),
    m_useHardware(dup.m_useHardware)
{
    *m_skeleton.get() = *dup.m_skeleton.get();

    if (dup.m_numBones)
        setNumBones(dup.m_numBones);

    if (dup.m_boneImportName)
        m_boneImportName = new String[dup.m_numBones];

    for (UInt32 i = 0 ; i < m_numBones ; ++i)
    {
        m_refMatrices[i] = dup.m_refMatrices[i];
        m_precomputedRefMatrices[i] = dup.m_precomputedRefMatrices[i];
        m_bones[i] = dup.m_bones[i].get();

        if (dup.m_boneImportName)
            m_boneImportName[i] = dup.m_boneImportName[i];
    }

    m_meshRefMatrix = dup.m_meshRefMatrix;
}

O3D_IMPLEMENT_DYNAMIC_CLASS1(Rigging, ENGINE_RIGGING, Skin)

Rigging::Rigging(const Rigging& dup) :
    Skin(dup)
{
}

O3D_IMPLEMENT_DYNAMIC_CLASS1(Skinning, ENGINE_SKINNING, Skin)

Skinning::Skinning(const Skinning& dup) :
    Skin(dup)
{
}

Skin::Skin(const Mesh& dup) :
    Mesh(dup),
    m_skinType(UNDEFINED_TYPE),
    m_vertexBlend(nullptr),
    m_recompute(False),
    m_boundingAutoRegen(False),
    m_isSkinning(False),
    m_numBones(0),
    m_bones(nullptr),
    m_skeleton(new Skeleton(this)),
    m_skinMatrices(MAX_SKINNING_MATRIX_ARRAY),
    m_boneImportName(nullptr),
    m_boneImportId(nullptr),
    m_isPrecomputed(False),
    m_useHardware(True)
{
}

Rigging::Rigging(const Mesh& dup) :
    Skin(dup)
{
    m_skinType = RIGGING_TYPE;
    m_shadableInfo.vertexProgram = Shadable::VP_RIGGING;
}

Skinning::Skinning(const Mesh& dup) :
    Skin(dup)
{
    m_skinType = SKINNING_TYPE;
    m_shadableInfo.vertexProgram = Shadable::VP_SKINNING;
}

Skin::~Skin()
{
    deletePtr(m_vertexBlend);
    deleteArray(m_bones);
    deleteArray(m_boneImportName);
    deleteArray(m_boneImportId);
}

// duplicate
Skin& Skin::operator=(const Skin& dup)
{
    Mesh::operator=(dup);
    deletePtr(m_vertexBlend);

    m_isPrecomputed = dup.m_isPrecomputed;
    m_isSkinning = dup.m_isSkinning;

    *m_skeleton.get() = *dup.m_skeleton.get();

    setNumBones(dup.m_numBones);

    if (dup.m_boneImportName)
        m_boneImportName = new String[dup.m_numBones];

    for (UInt32 i = 0 ; i < m_numBones ; ++i)
    {
        m_refMatrices[i] = dup.m_refMatrices[i];
        m_precomputedRefMatrices[i] = dup.m_precomputedRefMatrices[i];
        m_bones[i] = dup.m_bones[i].get();

        if (dup.m_boneImportName)
            m_boneImportName[i] = dup.m_boneImportName[i];
    }

    m_meshRefMatrix = dup.m_meshRefMatrix;

    return (*this);
}

Rigging& Rigging::operator=(const Rigging& dup)
{
    Skin::operator=(dup);
    return (*this);
}

Skinning& Skinning::operator=(const Skinning& dup)
{
    Skin::operator=(dup);
    return (*this);
}

// set the meshdata
void Skin::setMeshData(MeshData *meshData)
{
    Mesh::setMeshData(meshData);
    deletePtr(m_vertexBlend);
}

// Check and precompute matrix (automatically called when draw if not called before)
void Skin::initialize()
{
    // check for bones validity
    for (UInt32 i = 0 ; i < m_numBones ; ++i)
    {
        if (!m_bones[i].isValid())
            O3D_ERROR(E_InvalidParameter("Cannot initialize if one or many bones are not valid"));
    }

    preComputeRefMatrices();
}

// search and return a bone by its name
Bones* Skin::searchBone(const String &name)
{
    for (UInt32 i = 0 ; i < m_numBones ; ++i)
    {
        if (m_bones[i] && (m_bones[i]->getName() == name))
            return m_bones[i].get();
    }

    return nullptr;
}

Bool Skin::enableSkinning()
{
    Bool ret = m_isSkinning;
    m_isSkinning = True;
    return ret;
}

Bool Skin::disableSkinning()
{
    Bool ret = m_isSkinning;
    m_isSkinning = False;

    // reset bones matrices
    for (UInt32 i = 0; i < m_numBones; ++i)
    {
        m_skinMatrices[i].identity();
    }

    return ret;
}

// define the number of bones
void Skin::setNumBones(UInt32 numBones)
{
    m_recompute = True;
    m_numBones = numBones;
    m_isPrecomputed = False;

    deleteArray(m_bones);
    deleteArray(m_boneImportName);
    deleteArray(m_boneImportId);

    if (numBones) {
        m_refMatrices.resize(m_numBones);
        m_precomputedRefMatrices.resize(m_numBones);

        m_bones = new SmartObject<Bones>[m_numBones];

        for (UInt32 i = 0; i < m_numBones; ++i) {
            m_bones[i].setUser(this);
        }

        m_isSkinning = True;
    }
}

// Get the matrix array for hardware skinning/rigging.
const Float* Skin::getMatrixArray() const
{
    return m_skinMatrices.getData();
}

//Access to a currently bound vertex element.
VertexElement* Skin::getVertexElement(VertexAttributeArray type) const
{
    if (m_isSkinning && m_vertexBlend) {
        if (type == V_VERTICES_ARRAY)
            return &m_vertexBlend->getVertices();
        else if (type == V_NORMALS_ARRAY)
            return &m_vertexBlend->getNormals();
    }

    if (m_meshData)
        return m_meshData->getGeometry()->getElement(type);
    else
        return nullptr;
}

// Serialization
Bool Skin::writeToFile(OutStream &os)
{
    if (!Mesh::writeToFile(os))
        return False;

    os   << m_numBones
         << m_isSkinning
         << m_useHardware;

    // skeleton
    os << *m_skeleton.get();

    if (getScene()->getClothManager()->isExportRefBoneName())
        os << True;
    else
        os << False;

    for (UInt32 i = 0 ; i < m_numBones ; ++i)
    {
        if (getScene()->getClothManager()->isExportRefBoneName())
        {
            os   << m_bones[i]->getName()
                 << m_refMatrices[i];
        }
        else
        {
            os   << m_bones[i]->getSerializeId()
                 << m_refMatrices[i];
        }
    }

    os << m_meshRefMatrix;

    return True;
}

Bool Rigging::writeToFile(OutStream &os)
{
    if (!Skin::writeToFile(os))
        return False;

    return True;
}

Bool Skinning::writeToFile(OutStream &os)
{
    if (!Skin::writeToFile(os))
        return False;

    return True;
}

Bool Skin::readFromFile(InStream &is)
{
    if (!Mesh::readFromFile(is)) {
        return False;
    }

    deletePtr(m_vertexBlend);

    UInt32 numBones;
    Bool namedBones = False;

    is   >> numBones
         >> m_isSkinning
         >> m_useHardware
         >> namedBones;

    if (m_useHardware) {
        setGPUMode();
    } else {
        setCPUMode();
    }

    // skeleton
    is >> *m_skeleton.get();

    setNumBones(numBones);

    if (namedBones) {
        m_boneImportName = new String[m_numBones];
        for (UInt32 i = 0 ; i < m_numBones ; ++i) {
            is   >> m_boneImportName[i]
                 >> m_refMatrices[i];
        }
    } else {
        m_boneImportId = new Int32[m_numBones];
        for (UInt32 i = 0 ; i < m_numBones ; ++i) {
            is   >> m_boneImportId[i]
                 >> m_refMatrices[i];
        }
    }

    is >> m_meshRefMatrix;

    return True;
}

Bool Rigging::readFromFile(InStream &is)
{
    if (!Skin::readFromFile(is))
        return False;

    return True;
}

Bool Skinning::readFromFile(InStream &is)
{
    if (!Skin::readFromFile(is))
        return False;

    return True;
}

void Skin::postImportPass()
{
    Mesh::postImportPass();

    if (m_boneImportId)
    {
        for (UInt32 i = 0 ; i < m_numBones ; ++i)
        {
            m_bones[i] = reinterpret_cast<Bones*>(
                    getScene()->getSceneObjectManager()->getImportedSceneObject(m_boneImportId[i]));
        }

        deleteArray(m_boneImportId);

        preComputeRefMatrices();
    }
}

// attach Humanoid skeleton to a skin bones list (use nametable for refers it)
void Skin::attachToHumanoidSkeleton(Humanoid* humanoid)
{
    if (!humanoid)
        O3D_ERROR(E_InvalidParameter("null humanoid"));

    if (!m_boneImportName)
        O3D_ERROR(E_InvalidPrecondition("null bones name"));

    for (UInt32 i = 0 ; i < m_numBones ; ++i)
    {
        m_bones[i] = humanoid->searchBone(m_boneImportName[i]);

        if (!m_bones[i])
            O3D_ERROR(E_InvalidResult("Bone not found on the humanoid " + m_boneImportName[i]));
    }
}

// Use software (CPU) mode
void Skin::setCPUMode()
{
     Bool prevUseHardware = m_useHardware;

    m_useHardware = False;
    m_shadableInfo.vertexProgram = Shadable::VP_MESH;

    if (prevUseHardware == m_useHardware) {
        return;
    }

    // rebuild materials if necessary
    for (UInt32 i = 0; i < m_matProfiles.size(); ++i) {
        m_matProfiles[i]->clear();
        m_matProfiles[i]->prepareAndCompile(*this);
    }
}

// Use hardware (GPU) mode
void Skin::setGPUMode()
{
    Bool prevUseHardware = m_useHardware;

    m_useHardware = True;
    m_shadableInfo.vertexProgram =
        getTechnique() == SKINNING_TYPE ?
            Shadable::VP_SKINNING :
            Shadable::VP_RIGGING;

    if (prevUseHardware == m_useHardware) {
        return;
    }

    // rebuild materials if necessary
    for (UInt32 i = 0; i < m_matProfiles.size(); ++i) {
        m_matProfiles[i]->clear();
        m_matProfiles[i]->prepareAndCompile(*this);
    }
}

// Setup the modelview matrix to OpenGL
void Skin::setUpModelView()
{
    O3D_ASSERT(getScene()->getActiveCamera() != nullptr);

    if (!m_node) {
        getScene()->getContext()->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());
    } else {
        getScene()->getContext()->modelView().set(
            getScene()->getActiveCamera()->getModelviewMatrix() * m_node->getAbsoluteMatrix());
    }
}

// update the skin bounding box
void Skin::update()
{
    // update the skeleton
    if (m_skeleton.isValid()) {
        m_skeleton->update();
    }

    // update the skin
    clearUpdated();

    if (m_isPrecomputed) {
        // is a bone has moved
        Bool boneUpdated = False;

        // do we need to update the bounding volume
        for (UInt32 i = 0; i < m_numBones; ++i) {
            boneUpdated |= m_bones[i]->hasUpdated();
            if (boneUpdated) {
                break;
            }
        }

        // recompute the bounding box depending of the bones
        if (boneUpdated && getNode()) {
            m_boundingDirty = True;

            // recompute skinning at drawing (for CPU mode).
            m_recompute = True;

            // set the skin as updated
            setUpdated();
        }

        if (m_boundingDirty) {
            updateBounding();
        }
    }
}

// Update the global bounding volume.
void Skin::updateBounding()
{
    if (!m_boundingDirty) {
        return;
    }

    if (m_isSkinning && m_meshData && m_meshData->getGeometry() && getNode() && m_isPrecomputed && m_boundingAutoRegen) {
        Float length, radius;

        Vector3 vMin;
        Vector3 vMax;
//		Vector3 vMin1;
//		Vector3 vMax1;
        Vector3 vMin2;
        Vector3 vMax2;

        GeometryData::BoundingMode mode = m_meshData->getGeometry()->getBoundingMode();

        // use bounding box
        if ((mode == GeometryData::BOUNDING_BOX) || (mode == GeometryData::BOUNDING_BOXEXT)) {
            m_boundingBox.destroy();

            // and extend it by each bone
            for (UInt32 i = 0; i < m_numBones; ++i) {
                if (m_bones[i]->isEndEffector()) {
                    radius = m_bones[i]->getRadius();

                    vMin2.set(-radius,-radius,-radius);
                    vMin2 += m_bones[i]->getAbsoluteMatrix().getTranslation() - getNode()->getAbsoluteMatrix().getTranslation();

                    vMax2.set(radius,radius,radius);
                    vMax2 += m_bones[i]->getAbsoluteMatrix().getTranslation() - getNode()->getAbsoluteMatrix().getTranslation();

                    m_boundingBox.extend(vMin2);
                    m_boundingBox.extend(vMax2);
                } else {
                    length = m_bones[i]->getLength();
                    radius = m_bones[i]->getRadius();

                    vMin2.set(-radius,-radius,-length/2);
                    vMin2 += m_bones[i]->getAbsoluteMatrix().getTranslation() - getNode()->getAbsoluteMatrix().getTranslation();

                    vMax2.set(radius,radius,length/2);
                    vMax2 += m_bones[i]->getAbsoluteMatrix().getTranslation() - getNode()->getAbsoluteMatrix().getTranslation();

                    m_boundingBox.extend(vMin2);
                    m_boundingBox.extend(vMax2);
                }
            }

            // TODO it is not always efficient I think we need to transform the length of the bones
            // by the Z component of the bones matrix. And to have 0z for min and length for max.
            m_localBoundingBox = m_boundingBox;
            m_boundingBox = m_boundingBox.transformTo(getNode()->getAbsoluteMatrix());

        } else if (mode == GeometryData::BOUNDING_SPHERE) {
            // use bounding sphere
            BSphere bsphere = m_meshData->getGeometry()->getBSphere();

            vMin.set(
                    Limits<Float>::max(),
                    Limits<Float>::max(),
                    Limits<Float>::max());

            vMax.set(
                    Limits<Float>::min(),
                    Limits<Float>::min(),
                    Limits<Float>::min());

            // and extend it by each bone
            for (UInt32 i = 0; i < m_numBones; ++i) {
                if (m_bones[i]->isEndEffector()) {
                    radius = m_bones[i]->getRadius();

                    vMin2.set(-radius,-radius,-radius);
                    vMin2 += m_bones[i]->getAbsoluteMatrix().getTranslation() - getNode()->getAbsoluteMatrix().getTranslation();

                    vMax2.set(radius,radius,radius);
                    vMax2 += m_bones[i]->getAbsoluteMatrix().getTranslation() - getNode()->getAbsoluteMatrix().getTranslation();
                } else {
                    length = m_bones[i]->getLength();
                    radius = m_bones[i]->getRadius();

                    vMin2.set(-radius,-radius,-length/2);
                    vMin2 += m_bones[i]->getAbsoluteMatrix().getTranslation() - getNode()->getAbsoluteMatrix().getTranslation();

                    vMax2.set(radius,radius,length/2);
                    vMax2 += m_bones[i]->getAbsoluteMatrix().getTranslation() - getNode()->getAbsoluteMatrix().getTranslation();
                }

                vMax.maxOf(vMax, vMax2);
                vMin.minOf(vMin, vMin2);
            }

            bsphere.setRadius(
                    o3d::max<Float>(
                            fabs(o3d::minMin<Float>(vMin.x(), vMin.y(), vMin.z())),
                            fabs(o3d::maxMax<Float>(vMax.x(), vMax.y(), vMax.z()))));

            bsphere.applyTransform(
                    m_node->getAbsoluteMatrix(),
                    m_boundingSphere);
        }

        m_boundingDirty = False;
    } else {
        Mesh::updateBounding();
    }
}

// Get the drawing type
UInt32 Skin::getDrawType() const
{
    return Scene::DRAW_SKIN;
}

// Draw the skin
void Skin::draw(const DrawInfo &drawInfo)
{
    if (!getActivity()) {
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

    // if to recompute skinning is necessary
    if (m_recompute) {
        // pre-drawing initialization (made once time)
        if (!m_isPrecomputed) {
            preComputeRefMatrices();
        }

        if (m_isSkinning) {
            // compute news vertices transform matrices
            for (UInt32 i = 0; i < m_numBones; ++i) {
                // Use of Mult and not * because it is faster.
                m_bones[i]->getAbsoluteMatrix().mult(m_precomputedRefMatrices[i], m_skinMatrices[i]);
            }
        }
    }

    // draw symbolics
    if (drawInfo.pass == DrawInfo::AMBIENT_PASS) {
        drawSymbolics(drawInfo);

        // and skeleton
        if (m_skeleton.isValid()) {
            m_skeleton->draw(drawInfo);
        }
    } else if (drawInfo.pass == DrawInfo::PICKING_PASS) {
        // in picking pass draw the skeleton
        if (m_skeleton.isValid()) {
            m_skeleton->draw(drawInfo);
        }
    }

    // visibility is for the skin
    if (!getVisibility()) {
        return;
    }

    // distance of the mesh from the camera
    Float squaredDistance = getDistanceFrom(
            getScene()->getActiveCamera()->getAbsoluteMatrix().getTranslation());

    // bind the geometry data
    m_meshData->getGeometry()->bindVertexBuffer();

    // for each sub-mesh
    MaterialProfile *profile;
    UInt32 numProfiles = static_cast<UInt32>(m_matProfiles.size());

    // setup modelview matrix for this mesh
    setUpModelView();

    for (UInt32 i = 0; i < numProfiles; ++i) {
        profile = m_matProfiles[i];

        if (profile && profile->getActivity()) {
            MaterialTechnique *technique = profile->getBestTechnique(squaredDistance);

            // bind the face array for the current material
            m_meshData->getGeometry()->bindFaceArray(i);
            m_faceArrayId = i;

            // If picking pass, force in internal array mode
            if (drawInfo.pass == DrawInfo::PICKING_PASS) {
                m_shadableInfo.faceArray = nullptr;
            }

            // compute the skinning (CPU pass if non GPU mode) according to the bound face array
            if ((!m_useHardware/* || material.needsSorting()*/) && m_isSkinning) {
                // create the vertex blend the first time if necessary
                if (!m_vertexBlend) {
                    m_vertexBlend = m_meshData->getGeometry()->createVertexBlend();
                }

                // Bind the the vertex blender
                // It must not be unbound while the complete rendering is not finished.
                // Then multiple pass rendering (like alpha-pipeline processing) must
                // let the initial bound.
                m_meshData->getGeometry()->bindVertexBlender(m_vertexBlend);

                // perform the CPU skinning
                prepareDrawing();
            } else {
                // if skinning is disabled or GPU skinning, we use the original vertices data
                m_meshData->getGeometry()->bindVertexBlender(nullptr);
            }

            // process the technique
            if (technique) {
                technique->processMaterial(*this, this, this, drawInfo);
            }

            // draw the symbolic only on AMBIENT_PASS
            if (drawInfo.pass == DrawInfo::AMBIENT_PASS) {
                // processing the rendering of local space immediately
                if (getScene()->getDrawObject(Scene::DRAW_LOCAL_SPACE)) {
                    m_meshData->getGeometry()->drawLocalSpace(drawInfo);
                }
            }
        }
    }

    m_recompute = False;
}

void Skin::processAllFaces(Shadable::ProcessingPass pass)
{
    if (!m_meshData) {
        return;
    }

    if (pass == Shadable::PREPARE_GEOMETRY) {
        // bind the external face array
        if (isExternalFaceArray()) {
            UInt32 first, last;
            FaceArray *faceArray = getFaceArrayToProcess(first, last);

            if (faceArray) {
                // bind an external face array before process the CPU skinning
                m_meshData->getGeometry()->bindExternalFaceArray(faceArray);
            }
        }
    } else if (pass == Shadable::PROCESS_GEOMETRY) {
        // external face array
        if (isExternalFaceArray()) {
            UInt32 first,last;
            FaceArray *faceArray = getFaceArrayToProcess(first,last);

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

// Rigging process software skinning if necessary
void Rigging::prepareDrawing()
{
    // software rigging
    if (m_shadableInfo.activeVertexProgram == Shadable::VP_MESH) {
        if (!m_recompute) {
            return;
        }

        O3D_ASSERT(m_vertexBlend);

        if (m_isSkinning) {
            // we don't process interleaved vertex blender object (only serialized)
            // when dst is interlveaded, in this case the vbo update is not trivial
            // and should be done once per draw with the full or adapted range
            if (m_vertexBlend->isInterleaved()) {
                return;
            }

            // take and lock all needed data from submesh blender
            Float *dstVertices = m_vertexBlend->getVertices().getData().getData();

            const Float *srcRigging = m_meshData->getGeometry()->getElement(V_RIGGING_ARRAY)->lockArray(0, 0);
            UInt32 riggingSrcStride = m_meshData->getGeometry()->getElement(V_RIGGING_ARRAY)->getAdvance();

            Matrix4 matrix;
            Vector3 position;

            UInt32 vertexDstStride = m_vertexBlend->getVertices().getElementSize();
            UInt32 vertexSrcStride = m_meshData->getGeometry()->getVertices()->getAdvance();

            // original data
            const Float *srcVertices = m_meshData->getGeometry()->getVertices()->lockArray(0, 0);

            UInt32 firstIndice = m_meshData->getGeometry()->getBoundFaceArray()->getMinVertex();
            UInt32 lastIndice = m_meshData->getGeometry()->getBoundFaceArray()->getMaxVertex();

            // missing arrays
            if (!dstVertices || !srcRigging || !srcVertices) {
                if (srcRigging) {
                    m_meshData->getGeometry()->getElement(V_RIGGING_ARRAY)->unlockArray();
                }

                if (srcVertices) {
                    m_meshData->getGeometry()->getVertices()->unlockArray();
                }

                return;
            }

            dstVertices += firstIndice * vertexDstStride;
            srcVertices += firstIndice * vertexSrcStride;

            srcRigging += firstIndice * riggingSrcStride;

            // process with normal
            if (m_meshData->getGeometry()->isNormals()) {
                const Float *srcNormals = m_meshData->getGeometry()->getNormals()->lockArray(0, 0);
                Float *dstNormals = m_vertexBlend->getNormals().getData().getData();

                Vector3 normal;

                UInt32 normalDstStride = m_vertexBlend->getNormals().getElementSize();
                UInt32 normalSrcStride =  m_meshData->getGeometry()->getNormals()->getAdvance();

                dstNormals += firstIndice * normalDstStride;
                srcNormals += firstIndice * normalSrcStride;

                // rigging process
                for (UInt32 curVertex = firstIndice; curVertex <= lastIndice; ++curVertex) {
                    Int32 bone = (Int32)srcRigging[0];

                    if (bone != -1) {
                        // vertex
                        position.set(srcVertices);

                        matrix = m_skinMatrices[bone];

                        position = matrix * position;
                        memcpy(dstVertices, position.getData(), 3*sizeof(Float));

                        // normal
                        normal.set(srcNormals);

                        normal = matrix.rotate(normal);
                        memcpy(dstNormals, normal.getData(), 3*sizeof(Float));
                    } else {
                        memcpy(dstVertices, srcVertices, 3*sizeof(Float));
                        memcpy(dstNormals, srcNormals, 3*sizeof(Float));
                    }

                    dstVertices += vertexDstStride;
                    dstNormals += normalDstStride;

                    srcVertices += vertexSrcStride;
                    srcNormals += normalSrcStride;

                    srcRigging += riggingSrcStride;
                }

                m_meshData->getGeometry()->getNormals()->unlockArray();

                // update normals data
                m_vertexBlend->getNormals().update(
                        m_vertexBlend->getNormals().getData().getData() + firstIndice * normalDstStride,
                        firstIndice,
                        lastIndice - firstIndice + 1);
            } else {
                // process only vertices
                // rigging process
                for (UInt32 curVertex = firstIndice; curVertex <= lastIndice; ++curVertex) {
                    Int32 bone = (Int32)srcRigging[0];

                    if (bone != -1) {
                        // vertex
                        position.set(srcVertices);

                        matrix = m_skinMatrices[bone];

                        position = matrix * position;
                        memcpy(dstVertices, position.getData(), 3*sizeof(Float));
                    } else {
                        memcpy(dstVertices, srcVertices, 3*sizeof(Float));
                    }

                    dstVertices += vertexDstStride;
                    srcVertices += vertexSrcStride;

                    srcRigging += riggingSrcStride;
                }
            }

            // update vertices data
            m_vertexBlend->getVertices().update(
                    m_vertexBlend->getVertices().getData().getData() + firstIndice * vertexDstStride,
                    firstIndice,
                    lastIndice - firstIndice + 1);

            m_meshData->getGeometry()->getElement(V_RIGGING_ARRAY)->unlockArray();
            m_meshData->getGeometry()->getVertices()->unlockArray();
        }
    }
}

// Skinning PrepareDrawing
void Skinning::prepareDrawing()
{
    // software skinning
    if (m_shadableInfo.activeVertexProgram == Shadable::VP_MESH) {
        if (!m_recompute) {
            return;
        }

        O3D_ASSERT(m_vertexBlend);

        if (m_isSkinning) {
            // we don't process interleaved vertex blender object (only serialized)
            // when dst is interlveaded, in this case the vbo update is not trivial
            // and should be done once per draw with the full or adapted range
            if (m_vertexBlend->isInterleaved()) {
                return;
            }

            // take and lock all needed data from submesh blender
            Float *dstVertices = m_vertexBlend->getVertices().getData().getData();

            const Float *srcSkinning = m_meshData->getGeometry()->getElement(V_SKINNING_ARRAY)->lockArray(0, 0);
            const Float *srcWeighting = m_meshData->getGeometry()->getElement(V_WEIGHTING_ARRAY)->lockArray(0, 0);

            UInt32 skinningSrcStride = m_meshData->getGeometry()->getElement(V_SKINNING_ARRAY)->getAdvance();
            UInt32 weightingSrcStride = m_meshData->getGeometry()->getElement(V_WEIGHTING_ARRAY)->getAdvance();

            Matrix4 matrix;
            Vector3 position;
            UInt32 boneCount;
            Int32  boneId;
            Float  scale;

            UInt32 vertexDstStride = m_vertexBlend->getVertices().getElementSize();
            UInt32 vertexSrcStride = m_meshData->getGeometry()->getVertices()->getAdvance();

            // original data
            const Float *srcVertices = m_meshData->getGeometry()->getVertices()->lockArray(0, 0);

            UInt32 firstIndice = m_meshData->getGeometry()->getBoundFaceArray()->getMinVertex();
            UInt32 lastIndice = m_meshData->getGeometry()->getBoundFaceArray()->getMaxVertex();

            // missing arrays
            if (!dstVertices || !srcSkinning || !srcWeighting || !srcVertices) {
                if (srcSkinning) {
                    m_meshData->getGeometry()->getElement(V_SKINNING_ARRAY)->unlockArray();
                }
                if (srcWeighting) {
                    m_meshData->getGeometry()->getElement(V_WEIGHTING_ARRAY)->unlockArray();
                }
                if (srcVertices) {
                    m_meshData->getGeometry()->getVertices()->unlockArray();
                }

                return;
            }

            dstVertices += firstIndice * vertexDstStride;
            srcVertices += firstIndice * vertexSrcStride;

            srcSkinning += firstIndice * skinningSrcStride;
            srcWeighting += firstIndice * weightingSrcStride;

            // process with normal
            if (m_meshData->getGeometry()->isNormals()) {
                const Float *srcNormals = m_meshData->getGeometry()->getNormals()->lockArray(0, 0);
                Float *dstNormals = m_vertexBlend->getNormals().getData().getData();

                Vector3 normal;

                UInt32 normalDstStride = m_vertexBlend->getNormals().getElementSize();
                UInt32 normalSrcStride =  m_meshData->getGeometry()->getNormals()->getAdvance();

                dstNormals += firstIndice * normalDstStride;
                srcNormals += firstIndice * normalSrcStride;

                // skinning process
                for (UInt32 curVertex = firstIndice; curVertex <= lastIndice; ++curVertex) {
                    matrix.zero();

                    boneCount = 0;

                    // compute the matrix (matrix blending)
                    while (boneCount < 4) {
                        if ((boneId = (Int32)srcSkinning[boneCount]) != -1) {
                            scale = srcWeighting[boneCount];
                            if (scale > 0.f) {
                                matrix += m_skinMatrices[boneId] * scale;
                            }

                            ++boneCount;
                        } else {
                            break;
                        }
                    }

                    // compute the new vertex and normals only if one or more bone affect it
                    if (boneCount != 0) {
                        // vertex
                        position.set(srcVertices);

                        position = matrix * position;
                        memcpy(dstVertices, position.getData(), 3*sizeof(Float));

                        // normal
                        normal.set(srcNormals);

                        normal = matrix.rotate(normal);
                        memcpy(dstNormals, normal.getData(), 3*sizeof(Float));
                    } else {
                        // otherwise simply copy
                        memcpy(dstVertices, srcVertices, 3*sizeof(Float));
                        memcpy(dstNormals, srcNormals, 3*sizeof(Float));
                    }

                    dstVertices += vertexDstStride;
                    dstNormals += normalDstStride;

                    srcVertices += vertexSrcStride;
                    srcNormals  += normalSrcStride;

                    srcSkinning += skinningSrcStride;
                    srcWeighting += weightingSrcStride;
                }

                m_meshData->getGeometry()->getNormals()->unlockArray();

                // update normals data
                m_vertexBlend->getNormals().update(
                        m_vertexBlend->getNormals().getData().getData() + firstIndice * normalDstStride,
                        firstIndice,
                        lastIndice - firstIndice + 1);
            } else {
                // process only vertices
                // skinning process
                for (UInt32 curVertex = firstIndice; curVertex <= lastIndice; ++curVertex) {
                    matrix.zero();

                    boneCount = 0;

                    // compute the matrix (matrix blending)
                    while (boneCount < 4) {
                        if ((boneId = (Int32)srcSkinning[boneCount]) != -1) {
                            scale = srcWeighting[boneCount];
                            if (scale > 0.f) {
                                matrix += m_skinMatrices[boneId] * scale;
                            }

                            ++boneCount;
                        } else {
                            break;
                        }
                    }

                    // compute the new vertex and normals only if one or more bone affect it
                    if (boneCount != 0) {
                        // vertex
                        position.set(srcVertices);

                        position = matrix * position;
                        memcpy(dstVertices, position.getData(), 3*sizeof(Float));
                    } else {
                        // otherwise simply copy
                        memcpy(dstVertices, srcVertices, 3*sizeof(Float));
                    }

                    dstVertices += vertexDstStride;
                    srcVertices += vertexSrcStride;

                    srcSkinning += skinningSrcStride;
                    srcWeighting += weightingSrcStride;
                }

            }

            // update vertices data
            m_vertexBlend->getVertices().update(
                    m_vertexBlend->getVertices().getData().getData() + firstIndice * vertexDstStride,
                    firstIndice,
                    lastIndice - firstIndice + 1);

            m_meshData->getGeometry()->getElement(V_SKINNING_ARRAY)->unlockArray();
            m_meshData->getGeometry()->getElement(V_WEIGHTING_ARRAY)->unlockArray();
            m_meshData->getGeometry()->getVertices()->unlockArray();
        }
    }
}

//---------------------------------------------------------------------------------------
// ClothManager
//---------------------------------------------------------------------------------------

O3D_IMPLEMENT_DYNAMIC_CLASS1(ClothManager, ENGINE_CLOTH_LIST, SceneTemplateManager<ClothModel>)

// add a cloth to the manager (an XML file that contain def for cloths
Bool ClothManager::importClothModelDef(const String& filename)
{
    XmlDoc xmlDoc("clothmodeldef");
    xmlDoc.read(getScene()->getRootPath() + '/' + filename);

    String tmp;

    // read xml content
    // element TextureFont
    TiXmlElement *pNode = xmlDoc.getDoc()->FirstChildElement("ClothModel");
    if (!pNode) {
        O3D_ERROR(E_InvalidFormat("Invalid O3DClothModel token in " + filename));
    }

    TiXmlNode *pClothNode = nullptr;
    while ((pClothNode = pNode->IterateChildren("Cloth",pClothNode)) != nullptr) {
        ClothModel* clothModel = new ClothModel;

        if (clothModel->readFromXmlFile(pClothNode)) {
            m_ClothModelMap[clothModel->getName()] = clothModel;
        }
    }

    return True;
}

// export the cloths models defintion in an XML file
Bool ClothManager::exportClothModelDef(const String& filename)
{
    XmlDoc xmlDoc("clothmodel");

    const Char *basic =
        "<?xml version=\"1.0\" standalone=\"no\" ?>"
        "<!-- This file contain cloths defintions -->"
        "<ClothModel>"
        "</ClothModel>\n";

    xmlDoc.readData(basic);

    TiXmlElement* pClothModelElt =xmlDoc.getDoc()->FirstChildElement("ClothModel");

    for (IT_ClothModelMap it = m_ClothModelMap.begin() ; it != m_ClothModelMap.end() ; ++it) {
        TiXmlElement cloth("Cloth");

        if (!it->second->writeToXmlFile(cloth)) {
            return False;
        }

        pClothModelElt->InsertEndChild(cloth);
    }

    xmlDoc.write(getScene()->getRootPath() + '/' + filename);

    return True;
}

// clear all cloth defintion
void ClothManager::clearClothModelDef()
{
    for (IT_ClothModelMap it = m_ClothModelMap.begin() ; it != m_ClothModelMap.end() ; ++it) {
        deletePtr(it->second);
    }

    m_ClothModelMap.clear();
}

// serialisation
Bool ClothManager::exportClothModels(const String& path)
{
    //for (IT_TemplateManager it = m_MyMap.begin() ; it != m_MyMap.end() ; ++it)
    //	exportClothModelDef(path);
    // TODO

    return True;
}

// import a file that contain a list of cloth model file (XML format)
//static Int32 importClothModelList(InStream& is)
//{
//    return True;
//}
