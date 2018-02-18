/**
 * @file rootwindow.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ROOTWINDOW_H
#define _O3D_ROOTWINDOW_H

#include "widget.h"

namespace o3d {

class WidgetManager;

/**
 * @brief Widget manager root window. Special window defining the background of the gui.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-07
 */
class O3D_API RootWindow : public Widget
{
public:

    O3D_DECLARE_CLASS(RootWindow)

    //! Creation constructor as a top-level window
    RootWindow(
        WidgetManager *parent,
        const Vector2i &pos = DEFAULT_POS,
        const Vector2i &size = DEFAULT_SIZE,
        const String &name = "");

    //! Virtual destructor.
    virtual ~RootWindow();


    //-----------------------------------------------------------------------------------
    // Layout
    //-----------------------------------------------------------------------------------

    //! set the parent layout. By default an vertical layout is setup.
    void setLayout(Layout *layout);

    //! get the parent layout. By default a vertical layout is setup. (const version)
    virtual const Layout* getLayout() const override;
    //! get the parent layout. By default a vertical layout is setup.
    virtual Layout* getLayout() override;

    //! get the recommended widget default size
    virtual Vector2i getDefaultSize() override;

    //! get the widget client size. the client area is the area which may be drawn on by
    //! the programmer (excluding title bar, border, scrollbars, etc)
    virtual Vector2i getClientSize() const override;


    //-----------------------------------------------------------------------------------
    // Properties
    //-----------------------------------------------------------------------------------

    //! Get the current cursor type name for this widget if targeted.
    virtual String getCursorTypeName() const override;

    virtual void show(Bool show) override;

    //-----------------------------------------------------------------------------------
    // Widget
    //-----------------------------------------------------------------------------------

    virtual Vector2i getOrigin() const override;

    //! get the border size of the window depending of its defined theme
    //! the border size is the theme borders size and the title bar height
    virtual Vector2i getWindowBorderSize() const override;


    //-----------------------------------------------------------------------------------
    // Events
    //-----------------------------------------------------------------------------------

    //! Is widget targeted ?
    virtual Bool isTargeted(Int32 x,Int32 y,Widget *&widget) override;

    //! for move or resize
    virtual Bool mouseLeftPressed(Int32 x,Int32 y) override;
    virtual Bool mouseLeftReleased(Int32 x,Int32 y) override;
    virtual Bool mouseMove(Int32 x,Int32 y) override;

    virtual Bool keyboardToggled(Keyboard *keyboard, KeyEvent event) override;

    //! Others Event
    virtual void focused() override;
    virtual void lostFocus() override;
    virtual void sizeChanged() override;
    virtual void positionChanged() override;

    // Draw
    virtual void draw() override;
    virtual void updateCache() override;

private:

    RootWindow(BaseObject *parent);

    AutoPtr<Layout> m_pLayout;  //!< Layout used to manage the window content

    void init();
};

} // namespace o3d

#endif // _O3D_ROOTWINDOW_H
