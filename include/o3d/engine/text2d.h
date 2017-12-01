/**
 * @file text2d.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXT2D_H
#define _O3D_TEXT2D_H

#include "o3d/core/baseobject.h"
#include "o3d/core/box.h"
#include "o3d/image/color.h"
#include "o3d/engine/blending.h"
#include "o3d/engine/enginetype.h"

namespace o3d {

/**
 * @brief Interface to render a 2D text.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2014-02-24
 */
class O3D_API Text2D : public virtual Sharable
{
public:

    O3D_DECLARE_INTERFACE(Text2D)

    //! Virtual destructor
    virtual ~Text2D() = 0;

    //! set the text font height in pixel size.
    virtual void setTextHeight(Int32 y) = 0;
    //! get the text font height in pixel size.
    virtual Int32 getTextHeight() const = 0;

    //! get the height of an interline in pixels.
    virtual Int32 getInterline() const = 0;

    //! return the bounding box of a given string.
    virtual Box2i getBoundingBox(const String &text) = 0;

    //! set the text color
    virtual void setColor(const Color& col) = 0;
    //! get the text color
    virtual const Color& getColor() const = 0;

    /**
     * @brief Set the drawing blend mode
     * @param mode (default is Blending::ONE__ONE_MINUS_SRC_A___SRC_A__ONE).
     */
    virtual void setBlendFunc(Blending::FuncProfile func) = 0;
    //! get the drawing blend mode
    virtual Blending::FuncProfile getBlendFunc() const = 0;

    //! Write a string and a cursor at curpos position (-1 mean no cursor)
    virtual Int32 write(const Vector2i &pos, const String& text, Int32 curspos = -1) = 0;

    //! Write a string at the n° row and a cursor at curpos position (-1 mean no cursor)
    virtual Int32 writeAtRow(Int32 row, Int32 x, const String& text, Int32 curspos = -1) = 0;

    //! Get the width of a string
    virtual Int32 sizeOf(const String& text) = 0;

    //! Get the width of a single char
    virtual Int32 sizeOf(UInt32 achar) = 0;

    //! Get the nearest (round) character index for a given string at a given pixel position
    //! @param text The string to measure
    //! @param x The pixel position on x coordinate and relative to the begining of the string
    //! @return The nearest (round) character index of the given string.
    virtual Int32 nearestChar(const String &text, Int32 x) = 0;

    //! According to a given text, a left and right limits in pixel, and a pixel position
    //! to be contained exactly in this limits, set the characters indexes (left and right),
    //! and return the adjusted position in pixels into this relative range of indexes.
    virtual Int32 adjust(
            const String &text,
            const Int32 limits[2],
            Int32 pos,
            Int32 indexes[2],
            Int32 &dstLeft,
            String &dstText) = 0;
};

} // namespace o3d

#endif // _O3D_TEXT2D_H
