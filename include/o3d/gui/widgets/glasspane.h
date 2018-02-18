/**
 * @file glasspane.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GLASSPANE_H
#define _O3D_GLASSPANE_H

#include "window.h"

namespace o3d {

/**
 * @brief GlassPane
 * An invisible pane, that receive all events like another widget.
 * It is a top-level widget. It can own a layout liki a window, and manage children.
 * For example, a menu is a good case of a child in a glass pane.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-10-13
 */
class O3D_API GlassPane : public Window
{
public:

	O3D_DECLARE_CLASS(GlassPane)

	GlassPane(
			Widget *parent,
			const Vector2i &pos = DEFAULT_POS,
			const Vector2i &size = DEFAULT_SIZE,
			const String &name = "");

	//! Creation constructor as a top-level window
	GlassPane(
		WidgetManager *parent,
		const Vector2i &pos = DEFAULT_POS,
		const Vector2i &size = DEFAULT_SIZE,
		const String &name = "");

	virtual ~GlassPane();

	//------------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

	// Mouse events. coordinates are relative to parent origin
    virtual Bool mouseLeftPressed(Int32 x, Int32 y) override;
    virtual Bool mouseLeftReleased(Int32 x, Int32 y) override;
    virtual Bool mouseRightPressed(Int32 x, Int32 y) override;
    virtual Bool mouseRightReleased(Int32 x, Int32 y) override;
    virtual Bool mouseWheel(Int32 x, Int32 y, Int32 z) override;
    virtual Bool mouseMove(Int32 x, Int32 y) override;
    //virtual void mouseMoveIn() override;
    //virtual void mouseMoveOut() override;
    //virtual Bool mouseDrag(Float x, Float y, Widget *&draged) override;
    //virtual Bool mouseDrop(Float x, Float y, Widget *droped) override;

	/**
	 * @brief keyboardToggled Key event
	 * @param keyboard keyboard controller
	 * @param event keyboard event
	 * @return true if the widget consume the key, false to give it to its parent.
	 */
    virtual Bool keyboardToggled(Keyboard *keyboard, KeyEvent event) override;
	//! Character event.
    virtual Bool character(Keyboard *keyboard, CharacterEvent event) override;

	//! Is widget targeted ?
    virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget) override;

	// Draw
    virtual void draw() override;
    virtual void updateCache() override;

	//! Ask to delete the pane asynchronously
    Signal<> onDeleteGlassPane{this};

protected:

	GlassPane(BaseObject *parent);

	//! Slot delete glass pane
	void deleteGlassPane();
};


} // namespace o3d

#endif // _O3D_GLASSPANE_H
