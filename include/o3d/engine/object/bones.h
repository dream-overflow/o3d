/**
 * @file bones.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BONES_H
#define _O3D_BONES_H

#include "o3d/engine/hierarchy/node.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Skeleton;

/**
 * @brief A skeleton bone is a node.
 * @date 2003-08-12
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * A bone is used generally by a Skeleton in a Skin object.
 */
class O3D_API Bones : public Node
{
public:

	O3D_DECLARE_CLASS(Bones)

    //! Default constructor.
    Bones(BaseObject *parent, Bool endeffector = False);

    //! Construct from an owner Skeleton.
    Bones(Skeleton *skeleton, Bool endeffector = False);

	//! copy constructor
	Bones(const Bones& dup);

    //! duplicator. does not change the owner skeleton.
	Bones& operator=(const Bones& dup);

    //! Delete child
    virtual Bool deleteChild(BaseObject *child) override;

	//! Get the drawing type
    virtual UInt32 getDrawType() const override;

    //! Add a son before (only a Bones).
    virtual void addSonFirst(SceneObject *object) override;

    //! Add a son after (only a Bones).
    virtual void addSonLast(SceneObject *object) override;

    //! Remove a specified son (only a Bones).
    virtual void removeSon(SceneObject *object) override;

    //! Set the owner Skeleton (can be null).
    void setSkeleton(Skeleton *skeleton) { m_skeleton = skeleton; }

    //! Get the owner Skeleton (can be null).
    Skeleton* getSkeleton() { return m_skeleton; }

    //! Get the owner Skeleton (can be null) (const version).
    const Skeleton* getSkeleton() const { return m_skeleton; }

	//-----------------------------------------------------------------------------------
	// Parameters
	//-----------------------------------------------------------------------------------

	//! set/get is the bone is an ending one
	inline void setEndEffector(Bool b) { m_isEndEffector = b; }
	inline Bool isEndEffector() const { return m_isEndEffector; }

	//! Define the bounding cylinder radius
	inline void setRadius(Float r) { m_radius = r; }
	//! Get the radius of the bone
	inline Float getRadius() const { return m_radius; }

	//! Get the last computed length
	inline Float getLength() const { return m_length; }

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

    //! This update does not add the Bones to the visibility manager.
    virtual void update() override;

    //! Setup modelview according to the skeleton.
    virtual void setUpModelView() override;

    //! Draw the bones symbolic recursively.
    virtual void draw(const DrawInfo &drawInfo) override;

	// Serialization
    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

	//! post import pass, called after all objects are imported
    virtual void postImportPass() override;

protected:

    Skeleton *m_skeleton;   //!< Owner skeleton.

	Float m_length;         //!< The length of the bone (used for visibility and symbolic, can be used for collider too)
	Float m_radius;         //!< Radius of the cylinder for visibility computation (can be used for collider too)

	Bool  m_isEndEffector;  //!< is the bone a end effector
};

} // namespace o3d

#endif // _O3D_BONES_H
