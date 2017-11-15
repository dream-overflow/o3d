/**
 * @file skeleton.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/object/skeleton.h"
#include "o3d/engine/scene/scene.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Skeleton, ENGINE_SKELETON, SceneEntity)

Skeleton::Skeleton(BaseObject *parent, Bones *root) :
    SceneEntity(parent),
    m_root(this, root)
{
    if (!typeOf<SceneObject>(parent))
        O3D_ERROR(E_InvalidParameter("Parent of a Skeleton must be a SceneObject"));
}

void Skeleton::dupSubTree(const Bones *srcParent, Bones *dstParent)
{
    for (CIT_SonList cit = srcParent->getSonList().begin(); cit != srcParent->getSonList().end(); ++cit)
    {
        const Bones *srcBone = (const Bones*)*cit;
        Bones *dstBone = new Bones(dstParent);
        *dstBone = *srcBone;

        dstParent->addSonLast(dstBone);

        // sub tree
        if (!srcBone->getSonList().empty())
            dupSubTree(srcBone, dstBone);
    }
}

Skeleton &Skeleton::operator=(const Skeleton &dup)
{
    if (dup.m_root.isValid())
    {
        // duplicate the root first
        m_root = new Bones(this);
        *m_root.get() = *dup.m_root.get();

        // recursively duplicate the children
        dupSubTree(dup.m_root.get(), m_root.get());
    }
    else
    {
        // empty skeleton
        m_root = nullptr;
    }

    return *this;
}

void Skeleton::setParent(BaseObject *parent)
{
    if (!typeOf<SceneObject>(parent))
        O3D_ERROR(E_InvalidParameter("Parent of a Skeleton must be a SceneObject"));

    BaseObject::setParent(parent);
}

const Matrix4 &Skeleton::getAbsoluteMatrix()
{
    return static_cast<SceneObject*>(getParent())->getAbsoluteMatrix();
}

Bones *Skeleton::createBones(Bool endEffector)
{
    m_root = new Bones(this, endEffector);
    return m_root.get();
}

Bones *Skeleton::createBones(Bones *parent, Bool endEffector)
{
    Bones *bones = new Bones(parent, endEffector);
    bones->setSkeleton(this);

    parent->addSonLast(bones);

    return bones;
}

void Skeleton::draw(const DrawInfo &drawInfo)
{
    // draw only if allowed
    if (m_root.isValid() && getScene()->getDrawObject(Scene::DRAW_BONES))
        m_root->draw(drawInfo);
}

void Skeleton::update()
{
     if (m_root.isValid())
         m_root->update();
}

Bool Skeleton::writeToFile(OutStream &os)
{
    if (m_root.isValid())
    {
        os << True;
        m_root->writeToFile(os);
    }
    else
    {
        os << False;
    }

    return True;
}

Bool Skeleton::readFromFile(InStream &is)
{
    Bool root = False;
    is >> root;

    if (root)
    {
        m_root = new Bones(this);
        m_root->readFromFile(is);
    }
    else
    {
        m_root = nullptr;
    }

    return True;
}

void Skeleton::postImportPass()
{
    // nothing
}

