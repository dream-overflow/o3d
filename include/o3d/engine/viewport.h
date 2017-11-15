/**
 * @file viewport.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VIEWPORT_H
#define _O3D_VIEWPORT_H

#include "o3d/core/smartpointer.h"
#include "o3d/image/color.h"
#include "o3d/engine/object/camera.h"
#include "o3d/engine/scene/scenedrawer.h"
#include "o3d/core/callback.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Viewport managing.
 * @date 2002-08-01
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * Viewport base class. A viewport is a portion of screen or a feedback (off-screen)
 * rendering zone. Feedback use of FrameBufferObject as a render target.
 */
class O3D_API ViewPort : public SceneEntity
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(ViewPort)

	//! Default constructor
    ViewPort(BaseObject *parent, Camera* camera = nullptr, SceneDrawer* drawer = nullptr);

	//! destructor
	virtual ~ViewPort();

	//! enable activity
	void enable();
	//! disable activity
	void disable();
	//! toggle activity
	Bool toggle();
    //! get activity
    Bool getActivity() const;

	//! Enable percent mode.
	void enablePercent();
	//! Disable percent mode (use pixel size).
	void disablePercent();

	//! Set a rendering camera.
	void setCamera(Camera* camera);
	//! Get the used camera.
	inline Camera* getCamera() { return m_camera.get(); }

	//! Set a scene drawer. The drawer is called by the default draw() method.
	void setSceneDrawer(SceneDrawer* drawer);
	//! Get the used scene drawer.
	inline SceneDrawer* getSceneDrawer() { return m_drawer.get(); }

	//! Set viewport size in pixel or percent (1 is 100%).
	void setSize(Float xpos,Float ypos,Float width,Float height);

	//! Get x position of the viewport in pixels or in percent (1 is 100%).
	inline Float getPosX() const { return m_xpos; }
	//! Get y position of the viewport in pixels or in percent (1 is 100%).
	inline Float getPosY() const { return m_ypos; }
	//! Get width of the viewport in pixels or in percent (1 is 100%).
	inline Float getWidth() const { return m_width; }
	//! Get height of the viewport in pixels or in percent (1 is 100%).
	inline Float getHeight() const { return m_height; }

	//! Display the viewport.
	//! @param w Reshape width in pixels.
	//! @param h Reshape height in pixels.
	virtual void display(UInt32 w, UInt32 h) = 0;

	//! Default virtual drawing Viewport method. You can inherite from a viewport
	//! to change this behavior.
	//! The default drawing method call the SceneDrawer::draw() method.
	virtual void draw();

	//! Default virtual draw viewport method for the picking pass
	//! The default drawing method call the visibility manager draw in PICKING_MODE.
	virtual void drawPicking();

	//! Get the duration of the last display in seconds.
	inline Float getDuration() const { return m_duration; }

protected:

	Float m_xpos, m_ypos;       //!< position
	Float m_width, m_height;    //!< size (can be in percent)

	Bool m_percent;             //!< is using size in percent
	Bool m_isActive;            //!< is active

	UInt32 m_nWidth, m_nHeight; //!< real view-port size

	SmartObject<Camera> m_camera;   //!< rendering camera
	SmartObject<SceneDrawer> m_drawer; //!< scene drawer, default is ShadowVolumeForward.

	Float m_duration;           //!< Last display duration in seconds.
};

} // namespace o3d

#endif // _O3D_VIEWPORT_H

