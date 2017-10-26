/**
 * @file caret.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_CARET_H
#define _O3D_CARET_H

#include "o3d/core/objects.h"
#include "textzone.h"

namespace o3d {

/**
 * @brief Class that manage and draw a caret for widgets
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2008-02-16
 */
class O3D_API Caret
{
public:

    //! default constructor. take a valid TextZone object.
    Caret(TextZone *textZone);

	//! set the blind/visibility time (in ms, default is 500)
    inline void setVisibilityBlindTime(UInt32 ms) { m_blindTime.setCheckTime(ms); }
	//! get the blind/visibility time in ms
    inline UInt32 getVisibilityBlindTime() const { return m_blindTime.getCheckTime(); }

	//! reset the visibility state to visible
	void resetVisibilityState();

    //! draw the caret using the current defined font
	void draw(const Vector2i &pos);

	//! set the position of the caret in pixel position
	void setPixelPosition(const Vector2i &pos);

	//! get the position of the caret in pixel position
    inline const Vector2i& getPixelPosition() { return m_pixelPos; }

	//! check if the caret is visible at this time
    inline Bool isVisible() const { return m_isVisible; }

protected:

    TextZone *m_textZone;      //!< managed text zone

    Bool m_isVisible;          //!< true when the caret is visible
    TimeCounterMs m_blindTime; //!< delay of the caret visiblity/blind

    Vector2i m_pixelPos;       //!< position of the caret in pixel
};

} // namespace o3d

#endif // _O3D_CARET_H

