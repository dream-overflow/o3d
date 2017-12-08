/**
 * @file viewportmanager.h
 * @brief Viewport manager.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-08-01
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VIEWPORTMANAGER_H
#define _O3D_VIEWPORTMANAGER_H

#include "viewport.h"
#include "o3d/core/templateprioritymanager.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class ScreenViewPort;
class FeedbackViewPort;
class Texture2D;

/**
 * @brief The ViewPortManager class
 * View-port displayer with priority ordering.
 */
class O3D_API ViewPortManager : public SceneEntity
{
public:

    O3D_DECLARE_DYNAMIC_CLASS(ViewPortManager)

	//! Default constructor.
	ViewPortManager(BaseObject *parent);

	//! Delete child viewport.
	virtual Bool deleteChild(BaseObject *child);

    //
	// Factory
    //

	//! Delete a viewport.
	//! @return TRUE if the viewport is owned by this manager and if it is
	//!         successfully deleted.
	Bool deleteViewPort(ViewPort *viewPort);

	//! Add a viewport.
	void addViewPort(ViewPort *viewPort, Int32 priority = 0);

	//! Add a screen viewport.
	ScreenViewPort* addScreenViewPort(
			Camera* camera,
			SceneDrawer *drawer,
			Int32 priority = 0);

	//! Add an off-screen viewport.
	FeedbackViewPort* addFeedbackViewPort(Camera* camera,
			SceneDrawer *drawer,
			Texture2D* texture,
			Int32 priority = 0);

	//! Change the priority of a given viewport.
	void changePriority(ViewPort *viewPort, Int32 priority);

	//! Get a viewport according to its id.
	ViewPort* getViewPort(UInt32 id) { return m_manager.find(id); }
	//! Get a viewport according to its id (const).
	const ViewPort* getViewPort(UInt32 id) const { return m_manager.find(id); }

	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

	//! Display all the active view-ports by priority (0,1,...)
	void display();

	//! Reshape the device size of area
	void reshape(UInt32 w, UInt32 h);

	//! Get the viewport manager size (width,height of this)
    UInt32 getReshapeWidth() const { return m_width; }
    UInt32 getReshapeHeight() const { return m_height; }

	//! Select the drawing mode. Draw the scene normally, with Draw method.
	inline void setDrawMode() { m_drawPicking = False; }
	//! Redraw the scene in simplified way, used for picking.
	//! (generally disable landscape rendering, lights, effects...)
	inline void setDrawPickingMode() { m_drawPicking = True; }
	//! Is in redraw mode
    inline Bool isDrawPickingMode() const { return m_drawPicking; }

protected:

	TemplatePriorityManager<ViewPort> m_manager;

	Bool m_drawPicking;         //!< is in redraw mode (else draw mode)
	UInt32 m_width, m_height;   //!< reshape size (device context area)
};

} // namespace o3d

#endif // _O3D_VIEWPORTMANAGER_H
