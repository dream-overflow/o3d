/**
 * @file statictext.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_STATICTEXT_H
#define _O3D_STATICTEXT_H

#include "widget.h"
#include "../abcfont.h"
#include "../textzone.h"

namespace o3d {

/**
 * @brief A widget representing a simple static text
 * @author RinceWind, Frederic SCHERMA
 * @date 2006-10-03
 * A widget representing a simple static text
 */
class O3D_API StaticText : public Widget
{
public:

	O3D_DECLARE_CLASS(StaticText)

	//! initialization constructor
	StaticText(
		Widget *parent,
		const String &label,
        const Vector2i &pos = DEFAULT_POS,
		const Vector2i &size = DEFAULT_SIZE,
		Bool multiLine = False,
		const String &name = "");

	//! destructor
	virtual ~StaticText();

    virtual void enable(Bool active = True) override;

	//! Set the font
	inline void setFont(ABCFont* font)
	{
		m_textZone.setFont(font);
		setDirty();
	}
	//! Get the font (const version)
	inline const ABCFont* getFont() const { return m_textZone.font(); }
	//! Get the font
	inline ABCFont* getFont() { return m_textZone.font(); }

	//! Set the font height
	inline void setFontHeight(UInt32 _fontHeight)
	{
		m_textZone.setFontHeight(_fontHeight);
		setDirty();
	}
	//! Get the font height
	inline UInt32 getFontHeight() const { return m_textZone.fontHeight(); }

	//! Set text
	inline void setText(const String &text)
	{
		m_textZone.setText(text);
		setDirty();
	}
	//! Get text
	inline const String& getText() const {return m_textZone.text();}

	//! Set multiline text
	void setMultiline(Bool multiLine = True);
	//! Is multiline text
	Bool isMultiline() const { return m_textZone.isMultiLine(); }

	//! Set the text color.
	void setTextColor(const Color &col)
	{
		m_textZone.setColor(col);
		setDirty();
	}
	//! Get the text color.
	const Color& getTextColor() const { return m_textZone.color(); }

	//! Set the horizontal alignment (default is left)
	inline void setTextHorizontalAlignment(TextZone::HorizontalAlignmentMode align)
	{
		m_textZone.setHorizontalAlignment(align);
		setDirty();
	}
	//! Get the horizontal alignment (default is left)
	inline TextZone::HorizontalAlignmentMode getTextHorizontalAlignment() const
	{
		return m_textZone.horizontalAlignment();
	}

	//! Set the vertical alignment (default is middle)
	inline void setTextVerticalAlignment(TextZone::VerticalAlignmentMode align)
	{
		m_textZone.setVerticalAlignment(align);
		setDirty();
	}
	//! Get the vertical alignment (default is middle)
	inline TextZone::VerticalAlignmentMode getTextVerticalAlignment() const
	{
		return m_textZone.verticalAlignment();
	}

    //-----------------------------------------------------------------------------------
    // Properties
    //-----------------------------------------------------------------------------------

    //! fit to the size of the content
    virtual Vector2i fit() override;

	//! get the recommended widget default size
    virtual Vector2i getDefaultSize() override;

	//! Events Management
    virtual void positionChanged() override;
    virtual void sizeChanged() override;

	//! Is widget targeted ?
    virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget) override;

	// Draw
    virtual void draw() override;
    virtual void updateCache() override;

    virtual void layout() override;

protected:

	//! default constructor
	StaticText(BaseObject *parent);

	TextZone m_textZone;

	void init();
    void adjustBestSize();
};

} // namespace o3d

#endif // _O3D_STATICTEXT_H
