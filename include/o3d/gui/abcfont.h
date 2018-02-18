/**
 * @file abcfont.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_ABCFONT_H
#define _O3D_ABCFONT_H

#include "o3d/core/box.h"
#include "o3d/image/color.h"
#include "o3d/engine/scene/sceneentity.h"
#include "o3d/engine/text2d.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

/**
 * @brief Base class for all font classes of the GUI.
 * Inherits from the Text2D interface to be used by the engine part for font rendering.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-05-08
 */
class O3D_API ABCFont : public SceneEntity, public Text2D
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(ABCFont)

	//! Style of the font (can be a combination)
	enum FontStyle
	{
		STYLE_NORMAL = 1,      //!< Normal characters
		STYLE_ITALIC = 2,      //!< Italic style
		STYLE_BOLD = 4,        //!< Bold style
		STYLE_UNDERLINED = 8,  //!< Underlined style
		STYLE_STRIKEOUT = 16   //!< Strike out style
	};

	//! CharSet definition (char 0 always define the caret)
	enum CharSet
	{
        CHARSET_ASCII,        //!< Ascii charset (0x20 to 0x80)
        CHARSET_LATIN1        //!< Latin1 charset + extensions (0x20 to 0x1FF) + euro + typographic apostrophe
	};

    //! Constructor.
	ABCFont(
		BaseObject *pParent,
        CharSet charSet = CHARSET_LATIN1,
		FontStyle style = STYLE_NORMAL);

    //! Virtual destructor.
    virtual ~ABCFont();

    //! Load a font according to an istream.
	virtual void load(
        class InStream &is,
        const String &path,
        CharSet charSet = CHARSET_LATIN1,
		FontStyle style = STYLE_NORMAL) = 0;

    //! Load a font data file with its texture
	virtual void load(
		const String& fontName,
        CharSet charSet = CHARSET_LATIN1,
		FontStyle style = STYLE_NORMAL) = 0;

    //! Normalize all textures coordinates and compute width
	virtual void initialize() = 0;

    //! Get the font style
    inline FontStyle getStyle() const { return m_style; }

    //! Get the font charset
    inline CharSet getCharSet() const { return m_CharSet; }

    //! set the text font height in pixels.
    virtual void setTextHeight(Int32 y) override;
    //! get the text font height in pixels.
    virtual Int32 getTextHeight() const override;

	//! set the text area size (in number of char per row/column)
	void setNumCharHeight(Int32 y);
	//! get the text area size (in number of char per row/column)
	Int32 getNumCharHeight() const;

    //! set the text color.
    virtual void setColor(const Color& col) override;
    //! get the text color.
    virtual const Color& getColor() const override;

	/**
	 * @brief Set the drawing blend mode
     * @param mode (default is Blending::ONE__ONE_MINUS_SRC_A___SRC_A__ONE).
	 */
    virtual void setBlendFunc(Blending::FuncProfile func) override;
	//! get the drawing blend mode
    virtual Blending::FuncProfile getBlendFunc() const override;

	//! Write a string at the n° row and a cursor at curpos position (-1 mean no cursor)
    virtual Int32 writeAtRow(Int32 row, Int32 x, const String& text, Int32 curspos = -1) override;

	//! how many char in this font
	inline UInt32 getNumChars() const { return m_nChar; }

	//! is this char is supported by the loaded font
	virtual Bool isSupportedChar(UInt32 c) = 0;

protected:

    FontStyle m_style;     //!< Font style (default is normal = 1)
    CharSet m_CharSet;     //!< CharSet definition

	UInt32 m_nChar;        //!< the number of char that contain this font
	Int32 m_charH;         //!< height of a char in pixel size

	Color m_color;             //!< the current color
    Blending::FuncProfile m_blendFunc;  //!< rendering blend function
};

} // namespace o3d

#endif // _O3D_ABCFONT_H
