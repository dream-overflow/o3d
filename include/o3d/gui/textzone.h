/**
 * @file textzone.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3DTEXTZONE_H
#define _O3DTEXTZONE_H

#include "abcfont.h"
#include <vector>

namespace o3d {

/**
 * @brief Class to draw text zone (MultiLine, Alignment, selection...)
 * @author Rincewind, Frederic Scherma
 * @date 2006-10-21
 */
class O3D_API TextZone
{
public:

	enum HorizontalAlignmentMode
	{
		LEFT_ALIGN,
		CENTER_ALIGN,
		RIGHT_ALIGN,
		//PLAIN_ALIGN
	};

	enum VerticalAlignmentMode
	{
		TOP_ALIGN,
		MIDDLE_ALIGN,
		BOTTOM_ALIGN
	};

	//! constructor
	TextZone();

	//! destructor
	virtual ~TextZone();

	//! set the font
	inline void setFont(ABCFont* _font) { m_font = _font; setDirty(); }
	//! get the font (const version)
    inline const ABCFont* font() const { return m_font; }
	//! get the font
    inline ABCFont* font() { return m_font; }

	//! set/get the fontHeight (-1 mean use the default font size)
	inline void setFontHeight(Int32 _fontHeight) { m_fontHeight = _fontHeight; setDirty(); }
	inline Int32 fontHeight() const { return m_fontHeight; }

    //! Set the foreground color of the text
	inline void setColor(Color color) { m_color = color; }
    //! Set the foreground color of the text
	inline void setColor(Float r, Float g, Float b) { m_color.set(r,g,b); }
    //! Get the foreground color of the text
    inline const Color& color() const { return m_color; }

    //! Set the highlight (as background) color
    inline void setHighlightColor(Color color) { m_highlightColor = color; }
    //! Set the highlight (as background) color
    inline void setHighlightColor(Float r, Float g, Float b) { m_highlightColor.set(r,g,b); }
    //! Get the highlight (as background) color
    inline const Color& highlightColor() const { return m_highlightColor; }

    //! Set/get the horizontal alignment
	inline void setHorizontalAlignment(HorizontalAlignmentMode _align) { m_hTextAlign =  _align; }
    //! Get the horizontal alignment
    inline HorizontalAlignmentMode horizontalAlignment() const { return m_hTextAlign; }

    //! Set the vertical alignment
	inline void setVerticalAlignment(VerticalAlignmentMode _align) { m_vTextAlign = _align; }
    //! Get the vertical alignment
    inline VerticalAlignmentMode verticalAlignment() const { return m_vTextAlign; }

    //! Set the box of the text area
	inline void setBox(const Box2i &box)
	{
		m_box = box;
		setDirty();
	}
    //! Get the box of the text area
    inline const Box2i& box() const { return m_box; }

    //! Set the box top left position
	inline void setPos(const Vector2i &pos) { m_box.setPos(pos); }
    //! Set the box top left position
	inline void setPos(Int32 x, Int32 y) { m_box.x() = x; m_box.y() = y; }

    //! Set the box top left X coordinate
	inline void setPosX(Int32 x) { m_box.x() = x; }
    //! Set the box top left Y coordinate
	inline void setPosY(Int32 y) { m_box.y() = y; }

    //! Get the box top left position
    inline const Vector2i& pos() const { return m_box.pos(); }

    //! Set the box size of the text area
    inline void setSize(const Vector2i &size)
	{
		m_box.setSize(size);
		setDirty();
	}
    //! Set the box size of the text area
    inline void setSize(Int32 width, Int32 height)
	{
		m_box.width() = width;
		m_box.height() = height;
		setDirty();
	}

    //! Set the box width of the text area
    inline void setWidth(Int32 width)
	{
		m_box.width() = width;
		setDirty();
	}
    //! Set the box height of the text area
    inline void setHeight(Int32 height)
	{
		m_box.height() = height;
		setDirty();
	}

    //! Get the box size of the text area
    inline const Vector2i& size() const { return m_box.size(); }

    //! Set the multi-line mode.
	inline void setMultiLine(Bool mode) { m_multiLine = mode; setDirty(); }
    //! Is multi-line mode.
	inline Bool isMultiLine() const { return m_multiLine; }

    //! Set the text content.
	inline void setText(const String &_text)
	{
		m_text = _text;
		setDirty();
	}
    //! Get the text content (read-only)
    inline const String& text() const { return m_text; }

    //! Return the text at a given line
	const String& getTextLine(UInt32 row) const;

    //! Return the number of multi-line
    inline UInt32 getNumLines() const { return (m_multiLine ? (UInt32)m_textMultiLined.size() : 1); }

	//! Write the text
	void write(ABCFont* defaultFont, Int32 defaultFontSize);

	//! Compute the size of the text
	Vector2i sizeOf(ABCFont* defaultFont, Int32 defaultFontSize);

    //! Defines the highlight begin and end characters.
    //! @note begin <= end.
    void setHighlight(Int32 begin, Int32 end);

    //! Get the highlight begin and end characters as Int32[2] array.
    const Int32* getHighlight() const { return m_highlight; }

    //! Reset the highlight selection (set at 0).
    void resetHighlight();

protected:

    ABCFont* m_font;          //!< Font to use for rendering the text
    Int32 m_fontHeight;       //!< Specific font height
    Color m_color;            //!< Color of the text (foreground)
    Color m_highlightColor;   //!< Color of the selected regions of text (in background)

	HorizontalAlignmentMode m_hTextAlign;
	VerticalAlignmentMode m_vTextAlign;

    Box2i m_box;              //!< Location of the rect of text
    Bool m_multiLine;         //!< True if the text is compound of many lines

    Int32 m_highlight[2];     //!< In characters, first and last selected characters

    String m_text;            //!< Text to display (can contains new lines \n)
	std::vector<String> m_textMultiLined;

	//! Create the MultiLine List
	void createMultiLineList(ABCFont* pFont);

	Bool m_isDirty;           //!< true mean the textzone need an update

	inline Bool isDirty() const { return m_isDirty; }
	inline void setDirty() { m_isDirty = True; }
	inline void setClean() { m_isDirty = False; }
};

} // namespace o3d

#endif // _O3DTEXTZONE_H

