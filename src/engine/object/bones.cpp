/**
 * @file bones.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/skeleton.h"
#include "o3d/engine/object/bones.h"

#include "o3d/engine/object/camera.h"
#include "o3d/engine/primitive/primitivemanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/matrix.h"
#include "o3d/engine/context.h"
#include "o3d/engine/hierarchy/hierarchytree.h"
#include "o3d/physic/rigidbody.h"

using namespace o3d;

static const Float BONES_WIDTH = 0.5f;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Bones, ENGINE_BONES, Node)

// Get the drawing type
UInt32 Bones::getDrawType() const { return Scene::DRAW_BONES; }

// default constructor
Bones::Bones(BaseObject *parent, Bool endeffector) :
    Node(parent, False),
    m_skeleton(nullptr),
	m_length(0.f),
	m_radius(1.f),
	m_isEndEffector(endeffector)
{
	setDrawable(True);
}

Bones::Bones(Skeleton *skeleton, Bool endeffector) :
    Node(skeleton, False),
    m_skeleton(skeleton),
    m_length(0.f),
    m_radius(1.f),
    m_isEndEffector(endeffector)
{
    setDrawable(True);
}

// copy constructor
Bones::Bones(const Bones &dup) :
	Node(dup),
    m_skeleton(dup.m_skeleton),
    m_length(dup.m_length),
	m_radius(dup.m_radius),
	m_isEndEffector(dup.m_isEndEffector)
{
	m_isEndEffector = dup.m_isEndEffector;
}

// duplicate
Bones& Bones::operator=(const Bones &dup)
{
	Node::operator =(dup);

    m_length = dup.m_length;
	m_radius = dup.m_radius;

	m_isEndEffector = dup.m_isEndEffector;

    return *this;
}

Bool Bones::deleteChild(BaseObject *child)
{
    if (child)
    {
        if (child->getParent() != this)
            O3D_ERROR(E_InvalidParameter("The parent child differ from this"));
        else
        {
            // object should be type of SceneObject
            SceneObject *object = dynamicCast<SceneObject*>(child);
            if (object)
            {
                IT_SonList it = m_objectList.begin();
                for (; it != m_objectList.end(); ++it)
                {
                    if ((*it) == object)
                        break;
                }

                // remove the object of the son list
                if (it != m_objectList.end())
                {
                    m_objectList.erase(it);
                    object->setNode(nullptr);
                }

                deletePtr(object);
            }
            else
            {
                // otherwise simply delete it
                deletePtr(child);
            }

            return True;
        }
    }
    return False;
}

void Bones::addSonFirst(SceneObject *object)
{
    O3D_ASSERT(object);

    if (object)
    {
        object->setParent(this);
        object->setNode(this);
        object->setPersistant(True);

        m_objectList.push_front(object);
    }
}

void Bones::addSonLast(SceneObject *object)
{
    O3D_ASSERT(object);

    if (object)
    {
        object->setParent(this);
        object->setNode(this);
        object->setPersistant(True);

        m_objectList.push_back(object);
    }
}

void Bones::removeSon(SceneObject *object)
{
    IT_SonList it = m_objectList.begin();
    for (; it != m_objectList.end(); ++it)
    {
        if ((*it) == object)
            break;
    }

    if (it == m_objectList.end())
    {
        O3D_ERROR(E_InvalidParameter("Object not found"));
    }
    else
    {
        // remove the object of the son list
        m_objectList.erase(it);

        // no node
        object->setParent(getScene());
        object->setNode(nullptr);
        object->setPersistant(False);
    }
}

// draw the bones symbolic
void Bones::draw(const DrawInfo &drawInfo)
{
    if (!getActivity() || !getVisibility()) {
		return;
    }

    // Symbolic pass
    if ((drawInfo.pass == DrawInfo::AMBIENT_PASS/*SymbolicPass*/) && getScene()->getDrawObject(Scene::DRAW_BONES)) {
        PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access(drawInfo);

        // the bone
        BaseNode *parentBone = getNode();
        if (!m_isEndEffector && parentBone) {
            Vector3 dir = (m_worldMatrix.getTranslation() -
                parentBone->getAbsoluteMatrix().getTranslation());

            m_length = dir.length();

            Matrix4 parentToChild(parentBone->getAbsoluteMatrix());
            parentToChild.reComputeBasisGivenZ(dir);

            // set current modelview
            if (m_skeleton) {
                if (getScene()->getActiveCamera()) {
                    primitive->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix() * m_skeleton->getAbsoluteMatrix() * parentToChild);
                } else {
                    primitive->modelView().set(m_skeleton->getAbsoluteMatrix() * parentToChild);
                }
            } else if (getScene()->getActiveCamera()) {
                primitive->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix() * parentToChild);
            } else {
                primitive->modelView().set(parentToChild);
            }

            if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS)) {
                primitive->drawLocalAxis();
            }

            primitive->setColor(1,1,0);
            primitive->beginDraw(P_LINES);

                // on Z+
                primitive->addVertex(0,0,-BONES_WIDTH);
                primitive->addVertex(-BONES_WIDTH,-BONES_WIDTH,0);
                primitive->addVertex(0,0,-BONES_WIDTH);
                primitive->addVertex(BONES_WIDTH,-BONES_WIDTH,0);
                primitive->addVertex(0,0,-BONES_WIDTH);
                primitive->addVertex(-BONES_WIDTH,BONES_WIDTH,0);
                primitive->addVertex(0,0,-BONES_WIDTH);
                primitive->addVertex(BONES_WIDTH,BONES_WIDTH,0);

                primitive->addVertex(0,0,m_length);
                primitive->addVertex(-BONES_WIDTH,-BONES_WIDTH,0);
                primitive->addVertex(0,0,m_length);
                primitive->addVertex(BONES_WIDTH,-BONES_WIDTH,0);
                primitive->addVertex(0,0,m_length);
                primitive->addVertex(-BONES_WIDTH,BONES_WIDTH,0);
                primitive->addVertex(0,0,m_length);
                primitive->addVertex(BONES_WIDTH,BONES_WIDTH,0);

                primitive->addVertex(-BONES_WIDTH,-BONES_WIDTH,0);
                primitive->addVertex(BONES_WIDTH,-BONES_WIDTH,0);
                primitive->addVertex(BONES_WIDTH,-BONES_WIDTH,0);
                primitive->addVertex(BONES_WIDTH,BONES_WIDTH,0);
                primitive->addVertex(BONES_WIDTH,BONES_WIDTH,0);
                primitive->addVertex(-BONES_WIDTH,BONES_WIDTH,0);
                primitive->addVertex(-BONES_WIDTH,BONES_WIDTH,0);
                primitive->addVertex(-BONES_WIDTH,-BONES_WIDTH,0);

            primitive->endDraw();

            if (getScene()->getDrawObject(Scene::DRAW_BOUNDING_VOLUME)) {
                primitive->setColor(0,1,0);
                primitive->modelView().rotateX(o3d::toRadian(90.f));
                primitive->draw(PrimitiveManager::WIRE_CYLINDER1, Vector3(m_radius,m_length,m_radius));
            }
        } else {
            // set current modelview
            if (m_skeleton) {
                if (getScene()->getActiveCamera()) {
                    primitive->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix() * m_skeleton->getAbsoluteMatrix());
                } else {
                    primitive->modelView().set(m_skeleton->getAbsoluteMatrix());
                }
            } else if (getScene()->getActiveCamera()) {
                primitive->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());
            } else {
                primitive->modelView().set(Matrix4::getIdentity());
            }

            if (getScene()->getDrawObject(Scene::DRAW_LOCAL_AXIS)) {
                primitive->drawLocalAxis();
            }

            primitive->setColor(1,1,0);
            primitive->draw(PrimitiveManager::WIRE_SPHERE1, Vector3(BONES_WIDTH,BONES_WIDTH,BONES_WIDTH));

            if (getScene()->getDrawObject(Scene::DRAW_BOUNDING_VOLUME)) {
                primitive->setColor(0,1,0);
                primitive->draw(PrimitiveManager::WIRE_SPHERE1, Vector3(m_radius,m_radius,m_radius));
            }
        }
    } else if ((drawInfo.pass == DrawInfo::PICKING_PASS) && getScene()->getDrawObject(Scene::DRAW_BONES)) {
        // Picking pass
        PrimitiveAccess primitive = getScene()->getPrimitiveManager()->access(drawInfo);

        // the bone
        primitive->setPickableId(getPickableId());

        BaseNode *parentBone = getNode();
        if (!m_isEndEffector && parentBone) {
            Vector3 dir = (m_worldMatrix.getTranslation() -
                parentBone->getAbsoluteMatrix().getTranslation());

            m_length = dir.length();

            Matrix4 parentToChild(parentBone->getAbsoluteMatrix());
            parentToChild.reComputeBasisGivenZ(dir);

            // set current modelview
            if (m_skeleton) {
                if (getScene()->getActiveCamera()) {
                    primitive->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix() * m_skeleton->getAbsoluteMatrix() * parentToChild);
                } else {
                    primitive->modelView().set(m_skeleton->getAbsoluteMatrix() * parentToChild);
                }
            } else if (getScene()->getActiveCamera()) {
                primitive->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix() * parentToChild);
            } else {
                primitive->modelView().set(parentToChild);
            }

            primitive->modelView().rotateX(o3d::toRadian(90.f));
            primitive->draw(PrimitiveManager::SOLID_CYLINDER1, Vector3(BONES_WIDTH, m_length, BONES_WIDTH));
        } else {
            // set current modelview
            if (m_skeleton) {
                if (getScene()->getActiveCamera()) {
                    primitive->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix() * m_skeleton->getAbsoluteMatrix());
                } else {
                    primitive->modelView().set(m_skeleton->getAbsoluteMatrix());
                }
            } else if (getScene()->getActiveCamera()) {
                primitive->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix());
            } else {
                primitive->modelView().set(Matrix4::getIdentity());
            }

            primitive->draw(PrimitiveManager::SOLID_SPHERE1, Vector3(BONES_WIDTH, BONES_WIDTH, BONES_WIDTH));
        }
    }

    // draw recursively
    for (IT_SonList it = m_objectList.begin(); it != m_objectList.end(); ++it) {
        //getScene()->getContext()->modelView().push();
        (*it)->draw(drawInfo);
        //getScene()->getContext()->modelView().pop();
    }
}

void Bones::update()
{
    if (!getActivity())
        return;

    clearUpdated();
    Bool dirty = False;

    if (getNode() && getNode()->hasUpdated())
    {
        // the parent has change so the child need to be updated
        dirty = True;
    }
    else
    {
        // check if a transform has changed since last update
        for (IT_TransformList it = m_transformList.begin(); it != m_transformList.end(); ++it)
        {
            dirty |= (*it)->isDirty() | (*it)->hasUpdated();
            if (dirty)
                break;
        }
    }

    // is animation transform needed
    if (m_animTransform.isValid() && (m_animTransform->update() || m_animTransform->hasUpdated()))
    {
        m_animTransform->clearUpdated();
        dirty = True;

        // only here for draw animation trajectory TODO not the good place
        *m_prevAnimMatrix = m_animTransform->getMatrix();
    }

    if (dirty)
    {
        if (getNode())
            m_worldMatrix = getNode()->getAbsoluteMatrix();
        else
            m_worldMatrix.identity();

        // local transforms
        for (IT_TransformList it = m_transformList.begin(); it != m_transformList.end(); ++it)
        {
            (*it)->update();
            m_worldMatrix *= (*it)->getMatrix();
            (*it)->clearUpdated();
        }

        if (m_animTransform.isValid())
            m_worldMatrix *= m_animTransform->getMatrix();

        // compute the absolute matrix
        //if (m_rigidBody)
        //{
        //	m_worldMatrix = m_rigidBody->getWorldToBody();
        //}
        //else
        //{
        //	m_worldMatrix = getNode()->getFather()->getObject();
        //}

        setUpdated();
    }

    // update each son (recursively if necessary)
    for (IT_SonList it = m_objectList.begin(); it != m_objectList.end(); ++it)
    {
        SceneObject *object = (*it);

        if (object->getActivity())
        {
            // compute object absolute matrix
            object->update();
        }
    }
}

void Bones::setUpModelView()
{
    if (m_skeleton)
    {
        if (getScene()->getActiveCamera())
            getScene()->getContext()->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix() * m_skeleton->getAbsoluteMatrix() * getAbsoluteMatrix());
        else
            getScene()->getContext()->modelView().set(m_skeleton->getAbsoluteMatrix() * getAbsoluteMatrix());
    }
    else if (getScene()->getActiveCamera())
    {
        getScene()->getContext()->modelView().set(getScene()->getActiveCamera()->getModelviewMatrix() * getAbsoluteMatrix());
    }
    else
        getScene()->getContext()->modelView().set(getAbsoluteMatrix());
}

//---------------------------------------------------------------------------------------
// Serialization
//---------------------------------------------------------------------------------------

Bool Bones::writeToFile(OutStream &os)
{
    Node::writeToFile(os);

    os   << m_radius
		 << m_isEndEffector;

	return True;
}

Bool Bones::readFromFile(InStream &is)
{
    Node::readFromFile(is);

    is   >> m_radius
		 >> m_isEndEffector;

	return True;
}

void Bones::postImportPass()
{
	Node::postImportPass();
}

#undef BONES_WIDTH
