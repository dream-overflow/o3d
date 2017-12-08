/**
 * @file node.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_NODE_H
#define _O3D_NODE_H

#include "o3d/core/memorydbg.h"
#include "basenode.h"

#include "../object/atransform.h"

#include <list>

namespace o3d {

class Node;
class RigidBody;

typedef std::list<SceneObject*> T_SonList;
typedef T_SonList::iterator IT_SonList;
typedef T_SonList::const_iterator CIT_SonList;

/**
 * @brief Node element into the world hierarchy.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-02-05
 */
class O3D_API Node : public BaseNode
{
public:

	O3D_DECLARE_CLASS(Node)

	//! default constructor
	//! @param parent Parent object (another node or scene)
	//! @param isStatic True mean unmovable object, optimize visibility determination
    Node(BaseObject *parent = nullptr, Bool isStatic = False);

	//! destructor
    virtual ~Node();

	//! Delete child
    virtual Bool deleteChild(BaseObject *child) override;

	//-----------------------------------------------------------------------------------
	// Child management
	//-----------------------------------------------------------------------------------

	//! Add a son before.
    virtual void addSonFirst(SceneObject *object);

	//! Add a son after
    virtual void addSonLast(SceneObject *object);

	//! Remove a specified son
    virtual void removeSon(SceneObject *object);

    //! Has a direct son scene object.
    virtual Bool hasSon(SceneObject *object) const override;

	//! Find an object/node given its name
    virtual const SceneObject* findSon(const String &name) const override;
	//! Find an object/node given its name
    virtual SceneObject* findSon(const String &name) override;

	//! Find a scene object and return true if found.
    virtual Bool findSon(SceneObject *object) const override;

	//! Get the number of son into this node.
    virtual UInt32 getNumSon() const override;

	//! Remove and delete all sons.
	void deleteAllSons();

	//! Get the son list (read only).
	inline const T_SonList& getSonList() const { return m_objectList; }

	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! Compute recursively the number of element of this branch
    virtual UInt32 getNumElt() const override;

	//! Get the animation matrix
	inline const Matrix4& getAnimationMatrix() const
	{
		// TODO get previous anim matrix
        if (m_animTransform.isValid()) {
			return m_animTransform->getMatrix();
        } else {
			return Matrix4::getIdentity();
        }
	}

	//-----------------------------------------------------------------------------------
	// Transform
	//-----------------------------------------------------------------------------------

	//! Add a new transform
    virtual void addTransform(Transform *transform) override;

	//! Remove and delete a transform
    virtual void deleteTransform(Transform *transform) override;

	//! Remove and delete all transforms
    virtual void deleteAllTransforms() override;

	//! Find a transform
    virtual Bool findTransform(Transform *transform) const override;

	//! Find a transform given its name (read only)
    virtual const Transform* findTransform(const String &name) const override;
	//! Find a transform given its name
    virtual Transform* findTransform(const String &name) override;

	//! Get the transforms list (read only)
    virtual const T_TransformList& getTransforms() const override;
	//! Get the transforms list
    virtual       T_TransformList& getTransforms() override;

	//! Get the front transform or null if none (read only)
    virtual const Transform* getTransform() const override;

	//! Get the front transform or null if none
    virtual Transform* getTransform() override;

	//-----------------------------------------------------------------------------------
	// Movable
	//-----------------------------------------------------------------------------------

	//! Is the object is static (unmovable false) or dynamic (movable true)
    virtual Bool isMovable() const override;

	//-----------------------------------------------------------------------------------
	// Shadable
	//-----------------------------------------------------------------------------------

    virtual void setUpModelView() override;

	//! Return the absolute transform matrix (read only)
    virtual const Matrix4& getAbsoluteMatrix() const override;

	//-----------------------------------------------------------------------------------
	// Animatable
	//-----------------------------------------------------------------------------------

	virtual void animate(
		AnimationTrack::TrackType Type,
		const void* Value,
		UInt32 SizeOfValue,
		AnimationTrack::Target Target,
		UInt32 SubTarget,
		Animation::BlendMode BlendMode,
        Float Weight) override;

    virtual Animatable* getFirstSon() override;
    virtual Animatable* getNextSon() override;
    virtual Bool hasMoreSons() override;
    virtual void resetAnim() override;

    virtual AnimatableTrack* getAnimationStatus(const AnimationTrack* track) override;

    virtual const Matrix4& getPrevAnimationMatrix() const override;

    //-----------------------------------------------------------------------------------
    // Physic
    //-----------------------------------------------------------------------------------

    virtual void setRigidBody(RigidBody *rigidBody) override;

    virtual RigidBody* getRigidBody();

	//-----------------------------------------------------------------------------------
	// Updatable
	//-----------------------------------------------------------------------------------

    virtual void update() override;

	//-----------------------------------------------------------------------------------
	// Drawable
	//-----------------------------------------------------------------------------------
	
    virtual void draw(const DrawInfo &drawInfo) override;

    virtual Geometry::Clipping checkBounding(const AABBox &bbox) const override;

    virtual Geometry::Clipping checkBounding(const Plane &plane) const override;

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

	//! Recursively attribute serialize id to the node and each son
    virtual void preExportPass() override;

protected:

	//! copy constructor
	Node(const Node &dup);

	//! duplicator
	Node& operator=(const Node &dup);

	Bool m_movable;       //!< True mean the object is movable

	T_SonList m_objectList;   //!< List of all scene object and node
    IT_SonList m_curSon;      //!< Iterator for sons accessing methods of Animatable

	T_TransformList m_transformList;       //!< List of all transforms to apply to this node
    AutoPtr<ATransform> m_animTransform;   //!< Animated transform

	AutoPtr<Matrix4> m_prevAnimMatrix;     //!< Previous animation matrix for draw trajectory
	Matrix4 m_worldMatrix;                 //!< World matrix

	T_AnimationKeyFrameItMap m_keyFrameMap;  //!< lookup table for animation keyframe speedup

    RigidBody *m_rigidBody;   //!< Physic rigid body object

	inline void needAnimPart()
	{
        if (!m_animTransform) {
            m_animTransform = new ATransform;
			m_prevAnimMatrix = new Matrix4;
		}
	}
};

} // namespace o3d

#endif // _O3D_NODE_H
