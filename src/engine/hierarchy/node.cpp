/**
 * @file node.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/hierarchy/node.h"

#include "o3d/core/classfactory.h"

#include "o3d/engine/object/camera.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/scene/sceneobjectmanager.h"
#include "o3d/engine/visibility/visibilitymanager.h"

#include "o3d/engine/context.h"

#include "o3d/engine/animation/animationtrack.h"
#include "o3d/geom/plane.h"

#include "o3d/physic/rigidbody.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Node, ENGINE_NODE, BaseNode)

// Constructors
Node::Node(BaseObject *parent, Bool isStatic) :
	BaseNode(parent),
    m_movable(!isStatic),
    m_rigidBody(nullptr)
{
	setMovable(True);
	setUpdatable(True);
	setDrawable(False);
	setPickable(False);
	setAnimatable(True);
	setShadable(False);
	setShadowable(False);
}

Node::Node(const Node &dup) :
    BaseNode(dup),
    m_rigidBody(nullptr)
{
    m_node = nullptr;
}

// Destructor
Node::~Node()
{
	// remove all sons
    deleteAllSons();

	// remove all transforms
	deleteAllTransforms();
}

// Duplicate
Node& Node::operator=(const Node &dup)
{
	BaseNode::operator= (dup);
	m_movable = dup.m_movable;

	return *this;
}

// Delete child
Bool Node::deleteChild(BaseObject *child)
{
    if (child) {
        if (child->getParent() != this) {
			O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
        } else {
			// object should be type of SceneObject
			SceneObject *object = dynamicCast<SceneObject*>(child);
            if (object) {
				IT_SonList it = m_objectList.begin();
                for (; it != m_objectList.end(); ++it) {
					if ((*it) == object)
						break;
				}

				// remove the object of the son list
                if (it != m_objectList.end()) {
					m_objectList.erase(it);

                    if (object->hasDrawable())
                        getScene()->getVisibilityManager()->removeObject(object);

                    object->setNode(nullptr);
				}

				deletePtr(object);
            } else {
				// otherwise simply delete it
				deletePtr(child);
			}

			return True;
		}
	}
	return False;
}

Bool Node::isMovable() const
{
	return m_movable;
}

// Compute recursively the number of element of this branch
UInt32 Node::getNumElt() const
{
	UInt32 n = getNumSon();

	// count recursively
    for (CIT_SonList it = m_objectList.begin(); it != m_objectList.end(); ++it) {
        if ((*it)->isNodeObject()) {
			n += ((Node*)(*it))->getNumElt();
        }
	}

	return n;
}

// Update the branch
void Node::update()
{
    if (!getActivity()) {
		return;
    }

	clearUpdated();
	Bool dirty = False;

    if (getNode() && getNode()->hasUpdated()) {
		// the parent has change so the child need to be updated
		dirty = True;
    } else {
		// check if a transform has changed since last update
        for (IT_TransformList it = m_transformList.begin(); it != m_transformList.end(); ++it) {
            dirty |= (*it)->isDirty() | (*it)->hasUpdated();
            if (dirty) {
				break;
            }
		}
	}

	// is animation transform needed
    if (m_animTransform.isValid() && (m_animTransform->update() || m_animTransform->hasUpdated())) {
		m_animTransform->clearUpdated();
		dirty = True;

		// only here for draw animation trajectory TODO not the good place
		*m_prevAnimMatrix = m_animTransform->getMatrix();
	}

    if (dirty) {
        if (getNode()) {
			m_worldMatrix = getNode()->getAbsoluteMatrix();
        } else {
			m_worldMatrix.identity();
        }

		// local transforms
        for (IT_TransformList it = m_transformList.begin(); it != m_transformList.end(); ++it) {
			(*it)->update();
			m_worldMatrix *= (*it)->getMatrix();
			(*it)->clearUpdated();
		}

        if (m_animTransform.isValid()) {
			m_worldMatrix *= m_animTransform->getMatrix();
        }

		setUpdated();
	}

    // compute the absolute matrix
    if (m_rigidBody)
    {
        m_worldMatrix = m_rigidBody->getWorldToBody();
    }

	// update each son (recursively if necessary)
	for (IT_SonList it = m_objectList.begin(); it != m_objectList.end(); ++it)
	{
		SceneObject *object = (*it);

		if (object->getActivity())
		{
			// compute object absolute matrix
			object->update();

            if (object->hasUpdated())
            {
                // only for drawable and dynamic objects
                if (object->hasDrawable() &&
                    object->getVisibility() &&
                    getScene()->getDrawObject((Scene::DrawObjectType)object->getDrawType()) )
                        // && object->isMovable() )
                {
                    getScene()->getVisibilityManager()->updateObject(object);
                }
            }
        }
	}
}

// Draw the branch
void Node::draw(const DrawInfo &drawInfo)
{
	if (!getActivity() || !getVisibility())
		return;

	// draw recursively
	// not here because it is the scene visibility manager
    for (IT_SonList it = m_objectList.begin(); it != m_objectList.end(); ++it)
    {
        (*it)->draw(drawInfo);
    }
}

// Check the global bounding box with an AABBox
Geometry::Clipping Node::checkBounding(const AABBox &bbox) const
{
	if (bbox.include(m_worldMatrix.getTranslation()))
		return Geometry::CLIP_INSIDE;
	else
		return Geometry::CLIP_OUTSIDE;
}

// Check the global bounding box with an infinite plane
Geometry::Clipping Node::checkBounding(const Plane &plane) const
{
	Float d = plane * m_worldMatrix.getTranslation();

	if (d > 0.f)
		return Geometry::CLIP_INSIDE;
	else if (d == 0.f)
		return Geometry::CLIP_INTERSECT;
	else
		return Geometry::CLIP_OUTSIDE;
}

// Animatable
void Node::animate(
	AnimationTrack::TrackType type,
	const void* value,
	UInt32 sizeOfValue,
	AnimationTrack::Target target,
	UInt32 subTarget,
	Animation::BlendMode blendMode,
	Float weight)
{
	// cannot animate a static node
	O3D_ASSERT(m_movable);
    if (!m_movable) {
		return;
    }

	needAnimPart();

    switch (type) {
		case AnimationTrack::TRACK_TYPE_BOOL:
			if (sizeOfValue != sizeof(Bool))
				O3D_ERROR(E_InvalidParameter("Invalid SizeOfValue"));

            switch (target)
            {
            case AnimationTrack::TARGET_OBJECT_DISPLAY:
                setVisible(*((Bool*)value));
                break;
            default:
                break;
            }
        break;

		case AnimationTrack::TRACK_TYPE_FLOAT:
			if (sizeOfValue != sizeof(Float))
				O3D_ERROR(E_InvalidParameter("Invalid SizeOfValue"));

			switch (target)
			{
            case AnimationTrack::TARGET_OBJECT_POS_X:
                m_animTransform->setDirty();
                if (blendMode == Animation::BLEND_REPLACE)
                    m_animTransform->m_position[X] = *((Float*)value) * weight;
                else if (blendMode == Animation::BLEND_ADD)
                    m_animTransform->m_position[X] += *((Float*)value) * weight;
                break;
            case AnimationTrack::TARGET_OBJECT_POS_Y:
                m_animTransform->setDirty();
                if (blendMode == Animation::BLEND_REPLACE)
                    m_animTransform->m_position[Y] = *((Float*)value) * weight;
                else if (blendMode == Animation::BLEND_ADD)
                    m_animTransform->m_position[Y] += *((Float*)value) * weight;
                break;
            case AnimationTrack::TARGET_OBJECT_POS_Z:
                m_animTransform->setDirty();
                if (blendMode == Animation::BLEND_REPLACE)
                    m_animTransform->m_position[Z] = *((Float*)value) * weight;
                else if (blendMode == Animation::BLEND_ADD)
                    m_animTransform->m_position[Z] += *((Float*)value) * weight;
                break;

            case AnimationTrack::TARGET_OBJECT_ROT_X:
                m_animTransform->setDirty();
                if (blendMode == Animation::BLEND_REPLACE)
                    m_animTransform->m_rot[X] = *((Float*)value) * weight;
                else if (blendMode == Animation::BLEND_ADD)
                    m_animTransform->m_rot[X] += *((Float*)value) * weight;
            break;
            case AnimationTrack::TARGET_OBJECT_ROT_Y:
                m_animTransform->setDirty();
                if (blendMode == Animation::BLEND_REPLACE)
                    m_animTransform->m_rot[Y] = *((Float*)value) * weight;
                else if (blendMode == Animation::BLEND_ADD)
                    m_animTransform->m_rot[Y] += *((Float*)value) * weight;
                break;
            case AnimationTrack::TARGET_OBJECT_ROT_Z:
                m_animTransform->setDirty();
                if (blendMode == Animation::BLEND_REPLACE)
                    m_animTransform->m_rot[Z] = *((Float*)value) * weight;
                else if (blendMode == Animation::BLEND_ADD)
                    m_animTransform->m_rot[Z] += *((Float*)value) * weight;
                break;

            case AnimationTrack::TARGET_OBJECT_SCALE_X:
                m_animTransform->setDirty();
                if (blendMode == Animation::BLEND_REPLACE)
                    m_animTransform->m_scale[X] = *((Float*)value) * weight;
                else if (blendMode == Animation::BLEND_ADD)
                    m_animTransform->m_scale[X] += *((Float*)value) * weight;
                break;
            case AnimationTrack::TARGET_OBJECT_SCALE_Y:
                m_animTransform->setDirty();
                if (blendMode == Animation::BLEND_REPLACE)
                    m_animTransform->m_scale[Y] = *((Float*)value) * weight;
                else if (blendMode == Animation::BLEND_ADD)
                    m_animTransform->m_scale[Y] += *((Float*)value) * weight;
                break;
            case AnimationTrack::TARGET_OBJECT_SCALE_Z:
                m_animTransform->setDirty();
                if (blendMode == Animation::BLEND_REPLACE)
                    m_animTransform->m_scale[Z] = *((Float*)value) * weight;
                else if (blendMode == Animation::BLEND_ADD)
                    m_animTransform->m_scale[Z] += *((Float*)value) * weight;
                break;
            case AnimationTrack::TARGET_OBJECT_DISPLAY:
                setVisible(*((Float*)value) > 0);
                break;
            default:
                break;
			}
		break;

		case AnimationTrack::TRACK_TYPE_VECTOR:
			if (sizeOfValue != sizeof(Vector3))
				O3D_ERROR(E_InvalidParameter("Invalid SizeOfValue"));

			switch (target)
            {
            case AnimationTrack::TARGET_OBJECT_POS:
                m_animTransform->setDirty();
                if (blendMode == Animation::BLEND_REPLACE)
                    m_animTransform->m_position = *((Vector3*)value) * weight;
                else if (blendMode == Animation::BLEND_ADD)
                    m_animTransform->m_position += *((Vector3*)value) * weight;
                break;
            case AnimationTrack::TARGET_OBJECT_SCALE:
                m_animTransform->setDirty();
                if (blendMode == Animation::BLEND_REPLACE)
                    m_animTransform->m_scale = *((Vector3*)value) * weight;
                else if (blendMode == Animation::BLEND_ADD)
                    m_animTransform->m_scale += *((Vector3*)value) * weight;
                break;
            default:
                break;
			}
		break;

		case AnimationTrack::TRACK_TYPE_QUATERNION:
			if (sizeOfValue != sizeof(Quaternion))
				O3D_ERROR(E_InvalidParameter("Invalid SizeOfValue"));

            switch (target)
            {
            case AnimationTrack::TARGET_OBJECT_ROT:
                m_animTransform->setDirty();
                if (blendMode == Animation::BLEND_REPLACE)
                    m_animTransform->m_rotation = *((Quaternion*)value) * weight;
                else if (blendMode == Animation::BLEND_ADD)
                    m_animTransform->m_rotation += *((Quaternion*)value) * weight;
                break;
            default:
                break;
            }
        break;

		default:
		break;
	}
}

Animatable* Node::getFirstSon()
{
	m_curSon = m_objectList.begin();

	if (m_objectList.empty())
        return nullptr;

	return ((*m_curSon));
}

Animatable* Node::getNextSon()
{
	++m_curSon;

	if (m_curSon == m_objectList.end())
        return nullptr;

	return ((*m_curSon));
}

Bool Node::hasMoreSons()
{
	return (m_curSon != m_objectList.end());
}

Animatable::AnimatableTrack* Node::getAnimationStatus(const AnimationTrack *track)
{
	// register the track into the animatable
	IT_AnimationKeyFrameItMap it = m_keyFrameMap.find(track);
	if (it == m_keyFrameMap.end())
	{
		AnimatableTrack animatableTrack;

		animatableTrack.Time = 0.f;
		animatableTrack.Current = animatableTrack.First = track->getKeyFrameList().begin();
		animatableTrack.Last = track->getKeyFrameList().end();

		if (track->getKeyFrameList().empty())
			O3D_ERROR(E_InvalidParameter("The track does not contains keys"));

		--animatableTrack.Last;

		m_keyFrameMap.insert(std::make_pair(track, animatableTrack));
		return &m_keyFrameMap[track];
	}

	return &it->second;
}

const Matrix4& Node::getPrevAnimationMatrix() const
{
    return *m_prevAnimMatrix.get();
}

void Node::setRigidBody(RigidBody *rigidBody)
{
    m_rigidBody = rigidBody;
}

RigidBody *Node::getRigidBody()
{
    return m_rigidBody;
}

void Node::resetAnim()
{
	needAnimPart();
	m_animTransform->identity();
}

// Setup the modelview matrix to OpenGL
void Node::setUpModelView()
{
    O3D_ASSERT(getScene()->getActiveCamera() != nullptr);
	getScene()->getContext()->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix() * m_worldMatrix);
}

const Matrix4& Node::getAbsoluteMatrix() const
{
	return m_worldMatrix;
}

// add a son before
void Node::addSonFirst(SceneObject *object)
{
	O3D_ASSERT(object);

	if (object)
	{
		object->setParent(this);
		object->setNode(this);
		object->setPersistant(True);

		m_objectList.push_front(object);

        if (object->hasDrawable())
            getScene()->getVisibilityManager()->addObject(object);
	}
}

// add a son after
void Node::addSonLast(SceneObject *object)
{
	O3D_ASSERT(object);

    if (object) {
		object->setParent(this);
		object->setNode(this);
		object->setPersistant(True);

		m_objectList.push_back(object);

        if (object->hasDrawable()) {
            getScene()->getVisibilityManager()->addObject(object);
        }
	}
}

// remove a specified son
void Node::removeSon(SceneObject *object)
{
	IT_SonList it = m_objectList.begin();
    for (; it != m_objectList.end(); ++it) {
		if ((*it) == object)
			break;
	}

    if (it == m_objectList.end()) {
		O3D_ERROR(E_InvalidParameter("Object not found"));
    } else {
		// remove the object of the son list
		m_objectList.erase(it);

        if (object->hasDrawable()) {
            getScene()->getVisibilityManager()->removeObject(object);
        }

		// no node
		object->setParent(getScene());
        object->setNode(nullptr);
		object->setPersistant(False);
	}
}

// Find an object/node given its name
SceneObject* Node::findSon(const String &name)
{
    if (getName() == name) {
		return this;
    }

    for (IT_SonList it = m_objectList.begin(); it != m_objectList.end(); ++it) {
		SceneObject *object = (*it);
        if (object->isNodeObject()) {
			SceneObject *result = ((BaseNode*)object)->findSon(name);
            if (result) {
				return result;
            }
        } else if (object->getName() == name) {
			return object;
        }
	}

    return nullptr;
}

// Find an object/node given its name
const SceneObject* Node::findSon(const String &name) const
{
    if (getName() == name) {
		return this;
    }

	for (CIT_SonList it = m_objectList.begin(); it != m_objectList.end(); ++it)	{
		const SceneObject *object = (*it);
        if (object->isNodeObject()) {
			const SceneObject *result = ((BaseNode*)object)->findSon(name);
            if (result) {
				return result;
            }
        } else if (object->getName() == name) {
			return object;
        }
	}

    return nullptr;
}

// Find a scene object and return true if found
Bool Node::findSon(SceneObject *object) const
{
    if (this == object) {
		return True;
    }

    for (CIT_SonList it = m_objectList.begin(); it != m_objectList.end(); ++it) {
		const SceneObject *search = (*it);
        if (search->isNodeObject()) {
			Bool result = ((BaseNode*)search)->findSon(object);
            if (result) {
				return True;
            }
		}
        else if (search == object) {
			return True;
        }
	}

	return False;
}

// Remove all sons (delete objects if no longer used)
void Node::deleteAllSons()
{
	IT_SonList it = m_objectList.begin();

    while (it != m_objectList.end()) {
        if ((*it)->hasDrawable()) {
            getScene()->getVisibilityManager()->removeObject(*it);
        }

		deletePtr(*it);

		++it;
	}

	m_objectList.clear();
}

UInt32 Node::getNumSon() const
{
	return (UInt32)m_objectList.size();
}

// Add a new transform
void Node::addTransform(Transform *transform)
{
	m_transformList.push_back(transform);
	transform->setParent(this);
}

// Remove and delete a transform
void Node::deleteTransform(Transform *transform)
{
	for (IT_TransformList it = m_transformList.begin(); it != m_transformList.end(); ++it)
	{
		if ((*it) == transform)
		{
			deletePtr(*it);
			m_transformList.erase(it);
		}
	}

	O3D_ERROR(E_InvalidParameter("Transform not found"));
}

// Remove and delete all transforms
void Node::deleteAllTransforms()
{
	for (IT_TransformList it = m_transformList.begin(); it != m_transformList.end(); ++it)
		deletePtr(*it);

	m_transformList.clear();
}

// Find a transform
Bool Node::findTransform(Transform *transform) const
{
	for (CIT_TransformList it = m_transformList.begin(); it != m_transformList.end(); ++it)
	{
		if ((*it) == transform)
			return True;
	}
	return False;
}

// Find a transform given its name (read only)
const Transform* Node::findTransform(const String &name) const
{
	for (CIT_TransformList it = m_transformList.begin(); it != m_transformList.end(); ++it)
	{
		if ((*it)->getName() == name)
			return (*it);
	}
    return nullptr;
}

// Find a transform given its name
Transform* Node::findTransform(const String &name)
{
	for (IT_TransformList it = m_transformList.begin(); it != m_transformList.end(); ++it)
	{
		if ((*it)->getName() == name)
			return (*it);
	}
    return nullptr;
}

const T_TransformList& Node::getTransforms() const
{
	return m_transformList;
}

T_TransformList& Node::getTransforms()
{
	return m_transformList;
}

const Transform* Node::getTransform() const
{
	if (m_transformList.size())
		return m_transformList.front();
    else
        return nullptr;
}

//! Get the front transform or null if none
Transform* Node::getTransform()
{
	if (m_transformList.size())
		return m_transformList.front();
    else
        return nullptr;
}

//---------------------------------------------------------------------------------------
// serialization
//---------------------------------------------------------------------------------------
Bool Node::writeToFile(OutStream &os)
{
    if (!BaseNode::writeToFile(os))
		return False;

	// number of sons
    os << getNumSon();

	// write each son (recursively if other nodes)
    for (IT_SonList it = m_objectList.begin() ; it != m_objectList.end() ; ++it) {
        SceneObject *object = (*it);

        if (getScene()->getCurSceneIO().isIO(object)) {
            ClassFactory::writeToFile(os,*object);
        }
	}

	// write each transforms
    os << (UInt32)m_transformList.size();

    for (IT_TransformList it = m_transformList.begin() ; it != m_transformList.end() ; ++it) {
        if (!ClassFactory::writeToFile(os,*(*it))) {
			return False;
        }
	}

	return True;
}

Bool Node::readFromFile(InStream &is)
{
    if (!BaseNode::readFromFile(is)) {
		return False;
    }

	// set imported object
	getScene()->getSceneObjectManager()->setImportedSceneObject(getSerializeId(), this);

	UInt32 num;
    is >> num;

	// read each son (recursively if other nodes)
    for (UInt32 i = 0; i < num; ++i) {
		// import class name
		String className;
        is >> className;

		// clone a class instance
        BaseObject *object = ClassFactory::getInstanceOfClassInfo(className)->createInstance(this);

        if (typeOf<SceneObject>(object)) {
			addSonLast(dynamicCast<SceneObject*>(object));

			// and read the object
            is >> *object;

			// set imported object
			getScene()->getSceneObjectManager()->setImportedSceneObject(
					object->getSerializeId(),
					reinterpret_cast<SceneObject*>(object));
        } else {
            O3D_ERROR(E_InvalidFormat("Invalid object type, must be a scene object"));
			return False;
		}
	}

	// read each transforms
    is >> num;

    for (UInt32 i = 0; i < num; ++i) {
        BaseObject *object = ClassFactory::readFromFile(is, this);
		addTransform(reinterpret_cast<Transform*>(object));
	}

	return True;
}

void Node::preExportPass()
{
	setSerializeId((Int32)getScene()->getSceneObjectManager()->getSingleId());

    for (IT_SonList it = m_objectList.begin() ; it != m_objectList.end() ; ++it) {
		(*it)->preExportPass();
	}
}
