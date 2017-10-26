/**
 * @file skeleton.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SKELETON_H
#define _O3D_SKELETON_H

#include "o3d/engine/object/bones.h"

namespace o3d {

/**
 * @brief Skeleton container, is like a HierarchyTree object but dedicated to Bones.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-02-26
 */
class O3D_API Skeleton : public SceneEntity
{
public:

    O3D_DECLARE_DYNAMIC_CLASS(Skeleton)

    /**
     * @brief Skeleton constructon.
     * @param parent Must be a SceneObject that defines its getAbsoluteMatrix() method.
     * @param root Default root, can be null.
     */
    Skeleton(BaseObject *parent, Bones *root = nullptr);

    //! Duplicate a skeleton. Bones hierarchy is duplicate.
    Skeleton& operator=(const Skeleton &dup);

    virtual void setParent(BaseObject *parent);

    //! Get the absolute matrix of the skeleton, related to its parent.
    virtual const Matrix4& getAbsoluteMatrix();

    //! Get the root bone.
    Bones* getRoot() { return m_root.get(); }

    //! Get the root bone (const version).
    const Bones* getRoot() const { return m_root.get(); }

    //! Set the root bone, previous is deleted if no longer used.
    void setRoot(Bones *root) { m_root = root; }

    //! Create a new root bone.
    Bones* createBones(Bool endEffector = False);

    //! Create a new child bone.
    Bones* createBones(Bones *parent, Bool endEffector = False);

    virtual void draw(const DrawInfo &drawInfo);

    virtual void update();

    // Serialization
    virtual Bool writeToFile(OutStream &os);
    virtual Bool readFromFile(InStream &is);

    //! post import pass, called after all objects are imported
    virtual void postImportPass();

protected:

    SmartObject<Bones> m_root;   //!< Skeleton root bone (node).

    void dupSubTree(const Bones *srcParent, Bones *dstParent);
};

} // namespace o3d

#endif // _O3D_SKELETON_H

