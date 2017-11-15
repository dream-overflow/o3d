/**
 * @file sceneobject.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/scene/sceneobject.h"
#include "o3d/engine/object/camera.h"

#include "o3d/engine/scene/sceneobjectmanager.h"
#include "o3d/core/debug.h"
#include "o3d/engine/hierarchy/node.h"
#include "o3d/engine/animation/animationtrack.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/picking.h"
#include "o3d/engine/context.h"
#include "o3d/geom/plane.h"
#include "o3d/geom/frustum.h"
#include "o3d/physic/rigidbody.h"

using namespace o3d;

O3D_IMPLEMENT_CLASS_COMMON(Movable, ENGINE_MOVABLE, nullptr)
O3D_IMPLEMENT_CLASS_COMMON(Pickable, ENGINE_PICKABLE, nullptr)
O3D_IMPLEMENT_CLASS_COMMON(Updatable, ENGINE_UPDATABLE, nullptr)
O3D_IMPLEMENT_CLASS_COMMON(Drawable, ENGINE_DRAWABLE, nullptr)
O3D_IMPLEMENT_CLASS_COMMON(Shadowable, ENGINE_SHADOWABLE, nullptr)

UInt32 SceneObject::getDrawType() const
{
    return (UInt32)Scene::DRAW_UNDEFINED;
}

O3D_IMPLEMENT_ABSTRACT_CLASS8(
	SceneObject,
	ENGINE_SCENE_OBJECT,
	SceneEntity,
	Movable,
	Animatable,
	Pickable,
	Updatable,
	Drawable,
	Shadable,
	Shadowable)

// Constructors
SceneObject::SceneObject(BaseObject *parent) :
	SceneEntity(parent),
	Movable(),
	Animatable(),
	Pickable(),
	Updatable(),
	Drawable(),
	Shadable(),
	Shadowable(),
    m_node(nullptr)
{
	// obtain an unique scene object identifier
	if (getScene() && getScene()->getSceneObjectManager())
		getScene()->getSceneObjectManager()->addElement(this);

	setUpdated();

	m_capacities.setBit(STATE_ACTIVITY, True);
	m_capacities.setBit(STATE_VISIBILITY, True);
	m_capacities.setBit(STATE_PICKING, True);
}

SceneObject::SceneObject(const SceneObject &dup) :
	SceneEntity(dup),
	Movable(dup),
	Animatable(dup),
	Pickable(dup),
	Updatable(dup),
	Drawable(dup),
	Shadable(dup),
	Shadowable(dup),
	m_capacities(dup.m_capacities),
    m_node(nullptr)
{
	// obtain an unique scene object identifier
	if (getScene() && getScene()->getSceneObjectManager())
		getScene()->getSceneObjectManager()->addElement(this);

	setUpdated();
}

// Destructor
SceneObject::~SceneObject()
{
	// release the identifier manually if the object is not a direct child of the manager
	if ((m_id != -1) && getScene() && getScene()->getSceneObjectManager())
		getScene()->getSceneObjectManager()->removeElement(this);
}

// assign
SceneObject& SceneObject::operator=(const SceneObject& dup)
{
	BaseObject::operator =(dup);

	m_capacities = dup.m_capacities;
    m_node = nullptr;

	setUpdated();

	return *this;
}

// Change the parent object
void SceneObject::setParent(BaseObject *parent)
{
	m_parent = parent;

	if (parent)
	{
		if (m_topLevelParent && (m_topLevelParent != parent->getTopLevelParent()))
			O3D_ERROR(E_InvalidParameter("The top-level parent of the new parent is different of the old"));
		else
			m_topLevelParent = parent->getTopLevelParent();
	}
	else
	{
        m_topLevelParent = nullptr;
	}
}

// Hierarchy node accessors
BaseNode* SceneObject::getNode()
{
	return m_node;
}

const BaseNode* SceneObject::getNode() const
{
	return m_node;
}

void SceneObject::setNode(BaseNode *node)
{
	m_node = node;
	setUpdated();
}

// Return the absolute transform matrix (read only)
const Matrix4& SceneObject::getAbsoluteMatrix() const
{
	if (m_node)
		return m_node->getAbsoluteMatrix();
	else
        return Matrix4::getIdentity();
}

void SceneObject::setRigidBody(RigidBody *rigidBody)
{
    // Nothing
}

// Return the absolute matrix of the node or NULL.
const Matrix4& SceneObject::getObjectWorldMatrix() const
{
	if (m_node)
		return m_node->getAbsoluteMatrix();
	else
		return Matrix4::getIdentity();
}

// Setup the modelview matrix to OpenGL
void SceneObject::setUpModelView()
{
    O3D_ASSERT(getScene()->getActiveCamera() != nullptr);

	if (m_node)
		getScene()->getContext()->modelView().set(
				getScene()->getActiveCamera()->getModelviewMatrix() * m_node->getAbsoluteMatrix());
	else
		getScene()->getContext()->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());
}

Bool SceneObject::isMovable() const
{
	return False;
}

Bool SceneObject::getVisibility() const
{
	return m_capacities.getBit(STATE_VISIBILITY);
}

void SceneObject::enableVisibility()
{
	m_capacities.setBit(STATE_VISIBILITY, True);
}

void SceneObject::disableVisibility()
{
	m_capacities.setBit(STATE_VISIBILITY, False);
}

void SceneObject::enable()
{
	m_capacities.setBit(STATE_ACTIVITY, True);
}

void SceneObject::disable()
{
	m_capacities.setBit(STATE_ACTIVITY, False);
}

void SceneObject::enablePicking()
{
	m_capacities.setBit(STATE_PICKING, True);
}

void SceneObject::disablePicking()
{
	m_capacities.setBit(STATE_PICKING, False);
}

Bool SceneObject::isPicking() const
{
	return m_capacities.getBit(STATE_PICKING);
}

// Return O3DShadable::VP_MESH.
Shadable::VertexProgramType SceneObject::getVertexProgramType() const
{
	return VP_MESH;
}

// No external face array is set.
void SceneObject::useExternalFaceArray(
	FaceArray *faceArray,
	UInt32 numFaces,
	UInt32 firstFace,
	UInt32 lastFace)
{
}

// Nothing to process.
void SceneObject::processAllFaces(ProcessingPass pass)
{
}

// Return NULL.
VertexElement* SceneObject::getVertexElement(VertexAttributeArray type) const
{
    return nullptr;
}

// Return NULL.
FaceArray* SceneObject::getFaceArray() const
{
    return nullptr;
}

// Nothing to attribute.
void SceneObject::attribute(VertexAttributeArray mode, UInt32 location)
{
}

// Nothing to do.
void SceneObject::operation(Operations what)
{
}

// Return FALSE.
Bool SceneObject::isOperation(Operations what) const
{
	return False;
}

// Return NULL.
const Float* SceneObject::getMatrixArray() const
{
    return nullptr;
}

// Return FALSE.
Bool SceneObject::isExternalFaceArray() const
{
	return False;
}

// Return NULL.
FaceArray* SceneObject::getFaceArrayToProcess(UInt32 &first, UInt32 &last)
{
	first = last = 0;
    return nullptr;
}

// Returns 0.0f.
Float SceneObject::getDistanceFrom(const Vector3 &point) const
{
    return 0.0f;
}

void SceneObject::update()
{
    clearUpdated();
}

Bool SceneObject::hasUpdated() const
{
    return m_capacities.getBit(STATE_UPDATED);
}

void SceneObject::draw(const DrawInfo &drawInfo)
{
    clearDrawUpdate();
}

Bool SceneObject::isNeedDraw() const
{
    return m_capacities.getBit(STATE_DRAW_UPDATE);
}

// Check only with its parent node position.
Geometry::Clipping SceneObject::checkBounding(const AABBox &bbox) const
{
	if (bbox.include(m_node->getAbsoluteMatrix().getTranslation()))
		return Geometry::CLIP_INSIDE;
	else
		return Geometry::CLIP_OUTSIDE;
}

// Check only with its parent node position.
Geometry::Clipping SceneObject::checkBounding(const Plane &plane) const
{
	Float d = plane * m_node->getAbsoluteMatrix().getTranslation();

	if (d > 0.f)
		return Geometry::CLIP_INSIDE;
	else if (d == 0.f)
		return Geometry::CLIP_INTERSECT;
	else
		return Geometry::CLIP_OUTSIDE;
}

// Always returns inside.
Geometry::Clipping SceneObject::checkFrustum(const Frustum &frustum) const
{
	return Geometry::CLIP_INSIDE;
}

// get pickable color
Color SceneObject::getPickableColor()
{
	return Color(
		((getId() & 0x000000ff)) / 255.f,
		((getId() & 0x0000ff00) >> 8) / 255.f,
		((getId() & 0x00ff0000) >> 16) / 255.f,
		((getId() & 0xff000000) >> 24) / 255.f);
}

// get animation keyframe it map ref
Animatable::AnimatableTrack* SceneObject::getAnimationStatus(const AnimationTrack* track)
{
	// register the track into the animatable
	IT_AnimationKeyFrameItMap it = m_keyFrameMap.find(track);
	if (it == m_keyFrameMap.end())
	{
		AnimatableTrack animatableTrack;

		animatableTrack.Time = 0.f;
		animatableTrack.Current = animatableTrack.First = track->getKeyFrameList().begin();
		animatableTrack.Last = track->getKeyFrameList().end();
		--animatableTrack.Last;

		m_keyFrameMap.insert(std::make_pair(track, animatableTrack));
		return &m_keyFrameMap[track];
	}

    return &it->second;
}

void SceneObject::animate(
        AnimationTrack::TrackType type,
        const void *value,
        UInt32 sizeOfValue,
        AnimationTrack::Target target,
        UInt32 subTarget,
        Animation::BlendMode blendMode,
        Float weight)
{
}

void SceneObject::resetAnim()
{
}

Animatable *SceneObject::getFirstSon()
{
    return nullptr;
}

Animatable *SceneObject::getNextSon()
{
    return nullptr;
}

Bool SceneObject::hasMoreSons()
{
    return False;
}

const Matrix4 &SceneObject::getPrevAnimationMatrix() const
{
     return Matrix4::getIdentity();
}

// get the object ID for player serialization
Int32 SceneObject::getAnimatableId(Animatable::AnimatableManager &type)
{
	type = Animatable::SCENE_OBJECT;
	return getSerializeId();
}

// Enable the cast of shadow.
void SceneObject::enableShadowCast()
{
	m_capacities.setBit(STATE_SHADOW_CASTER, True);
}

// Disable the cast of shadow.
void SceneObject::disableShadowCast()
{
	m_capacities.setBit(STATE_SHADOW_CASTER, False);
}

// Get the shadow cast states.
Bool SceneObject::getShadowCast() const
{
	return m_capacities.getBit(STATE_SHADOW_CASTER);
}

// Project the silhouette according the a specific light.
void SceneObject::projectSilhouette(const DrawInfo &drawInfo)
{
}

// serialization
Bool SceneObject::writeToFile(OutStream &os)
{
    BaseObject::writeToFile(os);

    os << m_capacities;

	return True;
}

Bool SceneObject::readFromFile(InStream &is)
{
    BaseObject::readFromFile(is);

    is >> m_capacities;

    return True;
}

void SceneObject::postImportPass()
{
}

void SceneObject::preExportPass()
{
	setSerializeId((Int32)getScene()->getSceneObjectManager()->getSingleId());
}

