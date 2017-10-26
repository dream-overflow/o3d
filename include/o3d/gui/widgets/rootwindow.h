/**
 * @file rootwindow.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
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
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
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
    virtual const Layout* getLayout() const;
    //! get the parent layout. By default a vertical layout is setup.
    virtual Layout* getLayout();

    //! get the recommended widget default size
    virtual Vector2i getDefaultSize();

    //! get the widget client size. the client area is the area which may be drawn on by
    //! the programmer (excluding title bar, border, scrollbars, etc)
    virtual Vector2i getClientSize() const;


    //-----------------------------------------------------------------------------------
    // Properties
    //-----------------------------------------------------------------------------------

    //! Get the current cursor type name for this widget if targeted.
    virtual String getCursorTypeName() const;

    virtual void show(Bool show);

    //-----------------------------------------------------------------------------------
    // Widget
    //-----------------------------------------------------------------------------------

    virtual Vector2i getOrigin() const;

    //! get the border size of the window depending of its defined theme
    //! the border size is the theme borders size and the title bar height
    virtual Vector2i getWindowBorderSize() const;


    //-----------------------------------------------------------------------------------
    // Events
    //-----------------------------------------------------------------------------------

    //! Is widget targeted ?
    virtual Bool isTargeted(Int32 x,Int32 y,Widget *&widget);

    //! for move or resize
    virtual Bool mouseLeftPressed(Int32 x,Int32 y);
    virtual Bool mouseLeftReleased(Int32 x,Int32 y);
    virtual Bool mouseMove(Int32 x,Int32 y);

    virtual Bool keyboardToggled(Keyboard *keyboard, KeyEvent event);

    //! Others Event
    virtual void focused();
    virtual void lostFocus();
    virtual void sizeChanged();
    virtual void positionChanged();

    // Draw
    virtual void draw();
    virtual void updateCache();

private:

    RootWindow(BaseObject *parent);

    AutoPtr<Layout> m_pLayout;  //!< Layout used to manage the window content

    void init();
};

} // namespace o3d

#endif // _O3D_ROOTWINDOW_H

