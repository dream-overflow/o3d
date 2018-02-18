/**
 * @file tooltip.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TOOLTIP_H
#define _O3D_TOOLTIP_H

#include "layout.h"

namespace o3d {

/**
 * @brief Tool tip for any usage or combines it with another widget. It use of the parent
 * widget to controls its state.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-01
 */
class O3D_API ToolTip : public Widget
{
public:

    O3D_DECLARE_CLASS(ToolTip)

    static const Int32 DELAY_BEFORE_SHOW = 1000;  //!< Delay before show, in ms.

    //! initialization constructor
    ToolTip(
        Widget *parent,
        const Vector2i &pos = DEFAULT_POS,
        const Vector2i &size = DEFAULT_SIZE,
        const String &name = "");

    //! destructor
    virtual ~ToolTip();

    //! get the recommended widget default size
    virtual Vector2i getDefaultSize() override;

    //-----------------------------------------------------------------------------------
    // Layout
    //-----------------------------------------------------------------------------------

    //! set the parent layout. By default a vertical BoxLayout is setup.
    void setLayout(Layout *layout);

    //! get the parent layout. By default a vertical BoxLayout is setup. (const version)
    virtual const Layout* getLayout() const override;
    //! get the parent layout. By default a vertical BoxLayout is setup.
    virtual Layout* getLayout() override;

    //------------------------------------------------------------------------------------
    // Widget
    //-----------------------------------------------------------------------------------

    virtual Vector2i getOrigin() const override;

    //! get the widget client size. the client area is the area which may be drawn on by
    //! the programmer (excluding title bar, border, scrollbars, etc)
    virtual Vector2i getClientSize() const override;

    virtual Bool isTargeted(Int32 x, Int32 y, Widget *&pWidget) override;

    //! Events Management
    virtual void sizeChanged() override;

    // Draw
    virtual void draw() override;
    virtual void updateCache() override;

protected:

    //! default constructor
    ToolTip(BaseObject *parent);

    AutoPtr<Layout> m_pLayout;  //!< Layout used to manage the pane content

    void init();
};

} // namespace o3d

#endif // _O3D_TOOLTIP_H
