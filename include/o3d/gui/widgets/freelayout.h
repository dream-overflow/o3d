/**
 * @file freelayout.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FREELAYOUT_H
#define _O3D_FREELAYOUT_H

#include "layout.h"

namespace o3d {

/**
 * @brief A free layout.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-11-15
 */
class O3D_API FreeLayout : public Layout
{
public:

    O3D_DECLARE_CLASS(FreeLayout)

    //! Construct from a parent widget.
    FreeLayout(Widget *widget);

    //! Construct from a parent window.
    FreeLayout(Window *window);

    //! Construct from a parent layout.
    FreeLayout(Layout *layout);

    //-----------------------------------------------------------------------------------
    // Layout processing
    //-----------------------------------------------------------------------------------

    //! (Re)Compute the minimal size of the layout
    virtual Vector2i calcMin();
    //! (Re)Compute the size of the layout
    virtual void computeSizes();

protected:

    //! Default constructor.
    FreeLayout(BaseObject *parent);

    Int32 m_minWidth;
    Int32 m_minHeight;
    Int32 m_fixedWidth;
    Int32 m_fixedHeight;
};

} // namespace o3d

#endif // _O3D_FREELAYOUT_H

