/**
 * @file basenode.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_BASENODE_H
#define _O3D_BASENODE_H

#include "o3d/core/memorydbg.h"
#include "o3d/engine/scene/sceneobject.h"

#include "../object/transformable.h"

#include <list>

namespace o3d {

/**
 * @brief Abstract node element into the world hierarchy.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-09-04
 */
class O3D_API BaseNode : public SceneObject, public Transformable
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(BaseNode)

	//! Default constructor
	//! @param parent Parent object (another node or scene)
    BaseNode(BaseObject *parent = nullptr);

	//! destructor
	virtual ~BaseNode();


	//-----------------------------------------------------------------------------------
	// Child management
	//-----------------------------------------------------------------------------------

	//! Get the number of son into this node.
	virtual UInt32 getNumSon() const = 0;

	//! Find an object/node given its name
	virtual const SceneObject* findSon(const String &name) const = 0;
	//! Find an object/node given its name
	virtual SceneObject* findSon(const String &name) = 0;

    //! Has a direct son scene object.
    virtual Bool hasSon(SceneObject *object) const = 0;

    //! Find a scene object recursively from this node and return true if found.
	virtual Bool findSon(SceneObject *object) const = 0;

	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! Compute recursively the number of element of this branch
	virtual UInt32 getNumElt() const = 0;

	//! Is a node object (true mean branch object, false mean leaf object)
    virtual Bool isNodeObject() const override;

	//-----------------------------------------------------------------------------------
	// Transforms default without transformations
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
	// Drawable
	//-----------------------------------------------------------------------------------

    virtual void draw(const DrawInfo &drawInfo) override;

    virtual Geometry::Clipping checkBounding(const AABBox &bbox) const override;

    virtual Geometry::Clipping checkBounding(const Plane &plane) const override;

protected:

	//! Restricted copy constructor.
	BaseNode(const BaseNode &dup);

	//! Restricted copy operator.
	BaseNode& operator= (const BaseNode &dup);
};

} // namespace o3d

#endif // _O3D_BASENODE_H
