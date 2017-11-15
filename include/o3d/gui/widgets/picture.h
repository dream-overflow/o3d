/**
 * @file picture.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PICTURE_H
#define _O3D_PICTURE_H

#include "widget.h"
#include "o3d/image/image.h"

namespace o3d {

/**
 * @brief A static image widget.
 */
class O3D_API Picture : public Widget
{
public:

    O3D_DECLARE_DYNAMIC_CLASS(Picture)

    //! Default initialization constructor from a parent.
    //! @param parent Parent widget.
    //! @param iconSet Icon set.
    //! @param name Icon name into the icon set.
    //! @param iconSize Icon size into the icon set.
    Picture(
            Widget *parent,
            Image *image,
            const Vector2i &pos = DEFAULT_POS,
            const Vector2i &size = DEFAULT_SIZE,
            const String &name = "");

    //! Virtual destructor.
    virtual ~Picture();

    // draw
    virtual void draw();
    virtual void updateCache();

    virtual Vector2i getDefaultSize();

protected:

    //! Default initialization constructor from a parent base object.
    Picture(BaseObject *parent);
};

} // namespace o3d

#endif // _O3D_PICTURE_H

