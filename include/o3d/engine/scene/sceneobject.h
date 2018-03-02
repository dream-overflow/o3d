/**
 * @file sceneobject.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCENEOBJECT_H
#define _O3D_SCENEOBJECT_H

#include "o3d/core/templatebitset.h"
#include "o3d/geom/aabbox.h"

#include "sceneentity.h"

#include "../animation/animatable.h"
#include "../shader/shadable.h"
#include "../shadow/shadowable.h"

#include "../movable.h"
#include "../updatable.h"
#include "../drawable.h"
#include "../pickable.h"

#include "o3d/core/memorydbg.h"

namespace o3d {

class BaseNode;
class RigidBody;

/**
 * @brief Base class for all scene object.
 * @date 2007-12-09
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API SceneObject :
	public SceneEntity,
	public Movable,
	public Animatable,
	public Pickable,
	public Updatable,
	public Drawable,
	public Shadable,
	public Shadowable
{
protected:

	//! Capacities and states bitset index
	enum Capacities
	{
        CAPS_MOVABLE = 0,         //!< Movable object (not static, need dynamic update).
        CAPS_UPDATABLE = 1,       //!< Updatable object (update, node, visibility...).
		CAPS_DRAWABLE = 2,        //!< Drawable object (draw, visibility...).
		CAPS_PICKABLE = 3,        //!< Pickable object (picking).
        CAPS_ANIMATABLE = 4,      //!< Animatable object (animation).
        CAPS_SHADABLE = 5,        //!< Shadable object (mesh using materials).
        CAPS_SHADOWABLE = 6,      //!< Shadowable object (mesh that can receive shadow).
        STATE_SHADOW_CASTER = 16, //!< True if the object cast shadows.
        STATE_UPDATED,            //!< True if the object update occurred.
        STATE_ACTIVITY,           //!< True if the object is active.
        STATE_VISIBILITY,         //!< True if the object is visible.
        STATE_PICKING,            //!< True if the object picking is active.
        STATE_DRAW_UPDATE,        //!< True if the next draw need an update.
		CAPS_SCENE_OBJECT_NEXT    //!< Next capacity bit free.
	};

public:

	O3D_DECLARE_ABSTRACT_CLASS(SceneObject)

	//! Default constructor.
	//! When the scene object is created it is automatically added to the
	//! SceneObjectManager of the scene.
	SceneObject(BaseObject *parent);

	//! Copy constructor.
	//! When the scene object is created it is automatically added to the
	//! SceneObjectManager of the scene.
	SceneObject(const SceneObject &dup);

	//! Virtual destructor.
	//! When the scene object is destroyed it is automatically removed from the
	//! SceneObjectManager of the scene.
	virtual ~SceneObject();

	//! Duplicate.
	SceneObject& operator=(const SceneObject &dup);

	//! Set parent.
    virtual void setParent(BaseObject *parent) override;

	//-----------------------------------------------------------------------------------
	// Scene object capacities
	//-----------------------------------------------------------------------------------

	//! Is it a movable object. Thats mean it is hierarchically movable.
	inline Bool hasMovable() const { return m_capacities.getBit(CAPS_MOVABLE); }
	//! Is it an updatable object. Thats mean it is hierarchically updated at each frame.
	inline Bool hasUpdatable() const { return m_capacities.getBit(CAPS_UPDATABLE); }
	//! Is it a drawable object. Thats mean it is drawn by the visibility manager.
	inline Bool hasDrawable() const { return m_capacities.getBit(CAPS_DRAWABLE); }
	//! Is it a pickable object. Thats mean if the object support for picking or not (not define its picking state).
	inline Bool hasPickable() const { return m_capacities.getBit(CAPS_PICKABLE); }
	//! Is it a drawable animatable. Thats mean it use of the animation engine, and so of calls of Animate method.
	inline Bool hasAnimatable() const { return m_capacities.getBit(CAPS_ANIMATABLE); }
	//! Is it a shadable object. Shadable mean using an Material for the rendering (draw).
	inline Bool hasShadable() const { return m_capacities.getBit(CAPS_SHADABLE); }
	//! Is it a shadowable object. Shadowable mean using stencil pass.
	inline Bool hasShadowable() const { return m_capacities.getBit(CAPS_SHADOWABLE); }

	//-----------------------------------------------------------------------------------
	// Scene object hierarchy
	//-----------------------------------------------------------------------------------

	//! Return the hierarchy node object.
    virtual BaseNode* getNode();
	//! Return the hierarchy node object (read only).
	virtual const BaseNode* getNode() const;

	//! Define the hierarchy node object.
	virtual void setNode(BaseNode *node);

	//! Return the absolute transform matrix (read only).
	virtual const Matrix4& getAbsoluteMatrix() const;

	//! Is a node object (true mean node object, false mean leaf object).
	virtual Bool isNodeObject() const { return False; }

    //-----------------------------------------------------------------------------------
    // Physic
    //-----------------------------------------------------------------------------------

    //! Defines a rigid body object from the physic entity manager.
    virtual void setRigidBody(RigidBody *rigidBody);

	//-----------------------------------------------------------------------------------
	// Scene object draw specific
	//-----------------------------------------------------------------------------------

	//! Is the object visible.
    virtual Bool getVisibility() const override;

	//! Enable the visibility state.
    virtual void enableVisibility() override;

	//! Disable the visibility state.
    virtual void disableVisibility() override;

	//! Toggle the visibility state.
	inline Bool toggleVisibility()
	{
        if (getVisibility()) {
			disableVisibility();
			return False;
        } else {
			enableVisibility();
			return True;
		}
	}

	//! Set the object visibility.
	inline void setVisible(Bool state)
	{
        if (state) {
			enableVisibility();
        } else {
			disableVisibility();
        }
	}

    //! Is a light based objet. It is usefull for many light computation, especially
    //! duing visibility processing.
    virtual Bool isLight() const;

	//-----------------------------------------------------------------------------------
	// Movable
	//-----------------------------------------------------------------------------------

	//! Is the object is static (unmovable false) or dynamic (movable true).
    virtual Bool isMovable() const override;

    //-----------------------------------------------------------------------------------
	// Drawable
	//-----------------------------------------------------------------------------------

	//! Nothing to draw.
    virtual void draw(const DrawInfo &drawInfo) override;

	//! Check only with its parent node position.
    virtual Geometry::Clipping checkBounding(const AABBox &bbox) const override;

	//! Check only with its parent node position.
    virtual Geometry::Clipping checkBounding(const Plane &plane) const override;

	//! Always returns inside.
    virtual Geometry::Clipping checkFrustum(const Frustum &frustum) const override;

	//! Return O3D_UNDEFINED draw type.
    virtual UInt32 getDrawType() const override;

    //! Is need a draw.
    virtual Bool isNeedDraw() const override;

	//-----------------------------------------------------------------------------------
	// Scene object activity
	//-----------------------------------------------------------------------------------

	//! Enable the object update.
	virtual void enable();

	//! Disable the object.
	virtual void disable();

	//! Is the object active.
	inline Bool getActivity() const
	{
		return m_capacities.getBit(STATE_ACTIVITY);
	}

	//! Set the object activity.
	inline void setActivity(Bool state)
	{
        if (state) {
			enable();
        } else {
			disable();
        }
	}

	//! Toggle the object activity.
	inline Bool toggleActivity()
	{
        if (getActivity()) {
			disable();
			return False;
        } else {
			enable();
			return True;
		}
	}

	//-----------------------------------------------------------------------------------
	// Scene object picking specific
	//-----------------------------------------------------------------------------------

	//! Enable the picking processing of the object.
    virtual void enablePicking() override;

	//! Disable the picking processing of the object.
    virtual void disablePicking() override;

	//! Is the picking processing is enabled.
    virtual Bool isPicking() const override;

	//! Define the picking processing state of the object.
	inline void setPicking(Bool state)
	{
        if (state) {
			enablePicking();
        } else {
			disablePicking();
        }
	}

	//! Toggle the picking processing state of the object.
	inline Bool togglePicking()
	{
        if (isPicking()) {
			disablePicking();
			return False;
        } else {
			enablePicking();
			return True;
		}
	}

    //! Get the pickable id.
    virtual UInt32 getPickableId() override;

	//-----------------------------------------------------------------------------------
	// Animatable specific
	//-----------------------------------------------------------------------------------

	//! Get the object ID for player serialization.
    virtual Int32 getAnimatableId(AnimatableManager &type) override;

	//! Get animation key-frame it map ref.
    virtual AnimatableTrack* getAnimationStatus(const AnimationTrack* track) override;

	//! Nothing to animate.
	virtual void animate(
		AnimationTrack::TrackType type,
		const void* value,
		UInt32 sizeOfValue,
		AnimationTrack::Target target,
		UInt32 subTarget,
		Animation::BlendMode blendMode,
        Float weight) override;

	//! Nothing to reset.
    virtual void resetAnim() override;

    //! Return null.
    virtual Animatable* getFirstSon() override;

    //! Return null.
    virtual Animatable* getNextSon() override;

	//! Return FALSE.
    virtual Bool hasMoreSons() override;

	//! Return identity matrix.
    virtual const Matrix4& getPrevAnimationMatrix() const override;

	//-----------------------------------------------------------------------------------
	// Shadable specific
	//-----------------------------------------------------------------------------------

	//! Return the absolute matrix of the node or identity.
    virtual const Matrix4& getObjectWorldMatrix() const override;

	//! Set the modelview matrix according to its parent node and the current active camera.
    virtual void setUpModelView() override;

	//! Return Shadable::VP_MESH.
    virtual VertexProgramType getVertexProgramType() const override;

	//! No external face array is set.
	virtual void useExternalFaceArray(
		FaceArray *faceArray,
		UInt32 numFaces,
		UInt32 firstFace,
        UInt32 lastFace) override;

	//! Nothing to process.
    virtual void processAllFaces(ProcessingPass pass) override;

    //! Return null.
    virtual VertexElement* getVertexElement(VertexAttributeArray type) const override;

    //! Return null.
    virtual FaceArray* getFaceArray() const override;

	//! Nothing to attribute.
    virtual void attribute(VertexAttributeArray mode, UInt32 location) override;

	//! Nothing to process.
    virtual void operation(Operations what) override;

	//! Always return FALSE.
    virtual Bool isOperation(Operations what) const override;

    //! Return null.
    virtual const Float* getMatrixArray() const override;

	//! Return FALSE.
    virtual Bool isExternalFaceArray() const override;

    //! Return null.
    virtual FaceArray* getFaceArrayToProcess(UInt32 &first, UInt32 &last) override;

	//! Returns 0.0f.
    virtual Float getDistanceFrom(const Vector3 &point) const override;

	//-----------------------------------------------------------------------------------
	// Updatable specific
	//-----------------------------------------------------------------------------------

    //! Nothing to update. Only clear the updated flag.
    virtual void update() override;

    //! Check if it has been modified at its last update. Returns the updated flag.
    virtual Bool hasUpdated() const override;

	//! Force to process an update the next time.
	inline void queryUpdate() { setUpdated(); }

	//-----------------------------------------------------------------------------------
	// Shadowable specific
	//-----------------------------------------------------------------------------------

	//! Enable the cast of shadow.
    virtual void enableShadowCast() override;

	//! Disable the cast of shadow.
    virtual void disableShadowCast() override;

	//! Get the shadow cast states.
    virtual Bool getShadowCast() const override;

	//! Project the silhouette according to a specified light.
	//! Do nothing by default.
    virtual void projectSilhouette(const DrawInfo &drawInfo) override;

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

    virtual Bool writeToFile(OutStream &os) override;
    virtual Bool readFromFile(InStream &is) override;

	//! Post importation call. All object can implement a post importation processing,
	//! when all object are imported.
    virtual void postImportPass();

	//! Pre export call. All object can implement a pre importation processing.
	//! By default it define a single export ID taken from Scene.
	virtual void preExportPass();

protected:

	BitSet32 m_capacities;  //!< Object capacities

	BaseNode *m_node;       //!< Father node. Null mean no father.

	T_AnimationKeyFrameItMap m_keyFrameMap;

	//! Set the object update processed flag.
    inline void setUpdated() { return m_capacities.setBit(STATE_UPDATED, True); }
	//! Clear the object update processed flag.
    inline void clearUpdated() { return m_capacities.setBit(STATE_UPDATED, False); }
    //! Get the object update processed flag.
	inline Bool isNeedUpdate() { return !m_capacities.getBit(STATE_UPDATED); }

    //! Set the object draw update processed flag.
    inline void setDrawUpdate() { return m_capacities.setBit(STATE_DRAW_UPDATE, True); }
    //! Clear the object draw update processed flag.
    inline void clearDrawUpdate() { return m_capacities.setBit(STATE_DRAW_UPDATE, False); }
    //! Get the object draw update processed flag.
    inline Bool isNeedDrawUpdate() { return !m_capacities.getBit(STATE_DRAW_UPDATE); }

	//! Set the object as movable.
    inline void setMovable(Bool state) { m_capacities.setBit(CAPS_MOVABLE, state); }
	//! Set the object as updatable.
    inline void setUpdatable(Bool state) { m_capacities.setBit(CAPS_UPDATABLE, state); }
	//! Set the object as drawable.
    inline void setDrawable(Bool state) { m_capacities.setBit(CAPS_DRAWABLE, state); }
	//! Set the object as pickable.
    inline void setPickable(Bool state) { m_capacities.setBit(CAPS_PICKABLE, state); }
	//! Set the object as animatable.
    inline void setAnimatable(Bool state) { m_capacities.setBit(CAPS_ANIMATABLE, state); }
	//! Set the object as shadable.
    inline void setShadable(Bool state) { m_capacities.setBit(CAPS_SHADABLE, state); }
	//! Set the object as shadowable.
    inline void setShadowable(Bool state) { m_capacities.setBit(CAPS_SHADOWABLE, state); }
};

} // namespace o3d

#endif // _O3D_SCENEOBJECT_H
