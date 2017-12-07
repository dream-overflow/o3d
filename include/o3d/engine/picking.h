/**
 * @file picking.h
 * @brief 3d picking handler
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-10-22
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PICKING_H
#define _O3D_PICKING_H

#include "o3d/core/vector2.h"
#include "o3d/image/color.h"
#include "framebuffer.h"
#include "pickable.h"
#include "object/camera.h"
#include "o3d/core/evt.h"
#include "o3d/core/memorydbg.h"

#include <set>

namespace o3d {

/**
 * @brief The Picking manager
 * @details This class manage all pickable objects. A pickable object work in 2 pass.
 * And when a pickable is updated it is add to this manager for be checked if it is on
 * a zone or under a pointer (like a mouse pointer).
 */
class O3D_API Picking : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Picking)

	//! Picking technique
	enum Mode
	{
		DISABLE,       //!< picking is disable (default).
		COLOR,         //!< picking use of color mode. exact picking but no depth (the best way to process a picking).
		HIGHLIGHTING   //!< optimized method for highlighting picking (hybrid model, not implemented).
	};

	//! Definition of an hit
	struct Hit
	{
		Pickable *pickable;
		Vector3 position;
		Int32 depth;

		//! Default constructor.
		Hit() :
            pickable(nullptr),
			depth(0)
		{
		}

		//! Initialization constructor.
		Hit(Pickable *_pickable, Int32 _depth, const Vector3 &_position)
		{
			pickable = _pickable;
			depth = _depth;
			position = _position;
		}

		//! Assign.
		inline const Hit& operator= (const Hit &cmp)
		{
			pickable = cmp.pickable;
			depth = cmp.depth;
			position = cmp.position;

			return *this;
		}

		inline Bool operator== (const Hit &cmp) const { return (depth == cmp.depth); }
		inline Bool operator< (const Hit &cmp) const { return (depth < cmp.depth); }
	};

	typedef std::multiset<Hit> T_Hits;
	typedef T_Hits::iterator IT_Hits;

	//! Default ctor
	Picking(BaseObject *parent);

	//! Destructor
	virtual ~Picking();

	//! Set picking mode (default is color picking)
	inline void setMode(Mode mode) { m_mode = mode; }
	//! Get picking mode (default is color picking)
	inline Mode getMode() const { return m_mode; }

	//! Set the picking for all object (useful for viewer like O3D Studio, bypass the picking state of Pickable object)
	inline void setByPass(Bool value) { m_bypass = value; }
	//! Get the picking for all object (useful for viewer like O3D Studio, bypass the picking state of Pickable object)
	inline Bool isByPass() const { return m_bypass; }

	//! Is a hit have been processed
	inline Bool isHits() const { return m_hit; }

	//! Get hited object
	inline Pickable* getSingleHit()
	{
        if (m_hit && m_outObjectList.size()) {
			return m_outObjectList.begin()->pickable;
        }
        return nullptr;
	}

	//! Get multiset of hits
	inline const T_Hits& getHits() const { return m_outObjectList; }

	//! Clear the output list of picked objects
	inline void clearPickedList() { m_outObjectList.clear(); }

	//! Post a picking event to perform
	void postPickingEvent(Int32 x,Int32 y);

	//! Set the picking window area (half-size is used [-X..X,-Y..Y] around the cursor position
	inline void setPickingWindow(const Vector2f& size) { m_size = size; }

	//! Get the picking window corner position
	inline Vector2f getWindowPos() const  { return m_position; }

	//! Get the picking window size
	inline Vector2f getWindowSize() const { return m_size; }

	//! Get the picking position
	inline Vector2i getPickingPos() const  { return Vector2i(m_x,m_y); }

	//! Is there a picking pass to process
	inline Bool isPickingToProcess() const { return m_isPicking; }

	//! Is the picking is in processing
	inline Bool isProcessinggetPicking() const { return m_redraw; }

	// Redraw scene for make the picking

	//! Pre-redraw the scene if picking
	void preReDraw();

	//! Post redraw the scene if picking, hits can be checked after this call
	void postReDraw();

	//! Set the camera used for compute the hit pixel position and pointer position.
	void setCamera(class Camera *camera);

	//! Get the camera used for compute the hit pixel position and pointer position (read only).
	inline const Camera* getCamera() const { return m_camera.get(); }

	//! Get the camera used for compute the hit pixel position and pointer position.
	inline Camera* getCamera() { return m_camera.get(); }

	//! Get the 3d position of the last hit.
	//! @return Compute 3d hit position using the camera defined by SetCamera.
	const Vector3& getHitPos() const { return m_hitPos; }

	//! Get from a 2d position a 3d coordinate.
	//! @param x X viewport coordinate.
	//! @param y Y viewport coordinate.
	//! @return Compute 3d position using the camera defined by SetCamera.
	Vector3 getPointerPos(Int32 x,Int32 y);

public:

	//! Called when a hit is found. The first parameter is the pickable object, and
	//! the second is the hit position.
    Signal<Pickable*, Vector3> onHit{this};

private:

	Mode m_mode;                    //!< current picking state
	Bool m_isPicking;           //!< is a picking pass is asked

	Bool m_redraw;              //!< is the picking is in redrawing mode

	T_Hits m_outObjectList;         //!< list of picked objects (depend of the inner list)

	Vector2f m_position;            //!< top-left position of the picking window
	Vector2f m_size;                //!< width & height of the picking window

	Bool m_bypass;              //!< true mean the we redraw and check all object for picking

	Bool m_pointerMode;         //!< returns only a single object under a pointer (default mode)

	Bool m_hit;	                //!< is there one or more hits found

	Vector3 m_hitPos;               //!< 3d hit position

	Int32 m_x,m_y;              //!< position of the pointer

	UInt32 m_stacksize;         //!< buffer hits stack size (default 16)

	Color m_oldBackground;

	SmartObject<Camera> m_camera;   //!< camera used for pixel hit computation

	FrameBuffer m_fbo;              //!< FBO for color picking
};

} // namespace o3d

#endif // _O3D_PICKING_H
