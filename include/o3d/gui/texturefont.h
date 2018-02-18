/**
 * @file texturefont.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEXTUREFONT_H
#define _O3D_TEXTUREFONT_H

#include "abcfont.h"
#include "o3d/engine/shader/shadable.h"
#include "o3d/engine/material/materialprofile.h"
#include "o3d/engine/vertexbuffer.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Texture2D;

/**
 * @brief Manage a font from a texture and a data file that contain characters step.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-05-08
 * TextureFont is independent of Scene::TextureManager.
 * Font file is relative to current working directory, and the texture file is
 * relative to its font file.
 */
class O3D_API TextureFont : public ABCFont, public Shadable
{
public:

	O3D_DECLARE_CLASS(TextureFont)

	//! default constructor
	TextureFont(BaseObject *parent);

	//! Virtual destructor.
	virtual ~TextureFont();

	//! Load a font data file with its texture
	//! The file is relative to Gui (XML format) path, and texture have path relative to the font data file
	virtual void load(
        InStream &is,
        const String &path,
		CharSet charSet = CHARSET_LATIN1,
        FontStyle style = STYLE_NORMAL) override;

	//! Load a font data file with its texture
	//! The file is relative to Gui (XML format) path, and texture have path relative to the font data file
	virtual void load(
		const String& font_file,
		CharSet charSet = CHARSET_LATIN1,
        FontStyle style = STYLE_NORMAL) override;

	//! Normalize all textures coordinates and compute width
	//! @note Must be called after GenerateHomogenousMap or SetCursor usage
    virtual void initialize() override;

	//! get the height of an interline
    virtual Int32 getInterline() const override;

	//! write a string without line return and return the width of the string
    virtual Int32 write(const Vector2i &pos, const String &text, Int32 curspos = -1) override;

	//! get the width of this string/char
    virtual Int32 sizeOf(const String& text) override;

	//! Get the width of a single char
    virtual Int32 sizeOf(UInt32 achar) override;

    //! Get the nearest (round) character index for a given string at a given pixel position
    virtual Int32 nearestChar(const String &text, Int32 x) override;

	//! is this char is supported by the loaded font
    virtual Bool isSupportedChar(UInt32 c) override;

	//! return the bounding box of a given string
    virtual Box2i getBoundingBox(const String &text) override;

    //! According to a given text, a left and right limits in pixel, and a pixel position
    //! to be contained exactly in this limits, set the characters indexes (left and right),
    //! and return the adjusted position in pixels into this relative range of indexes.
    virtual Int32 adjust(
            const String &text,
            const Int32 limits[2],
            Int32 pos,
            Int32 indexes[2],
            Int32 &dstLeft,
            String &dstText) override;


	//-----------------------------------------------------------------------------------
	// Shadable override
	//-----------------------------------------------------------------------------------

	O3D_SHADABLE_NO_DATA_ACCESS
	O3D_SHADABLE_NO_MISC1
	O3D_SHADABLE_NO_EXT_ARRAY

    virtual VertexProgramType getVertexProgramType() const override;

    virtual void processAllFaces(Shadable::ProcessingPass pass) override;
    virtual void attribute(VertexAttributeArray mode, UInt32 location) override;

protected:

	MaterialProfile m_material;  //! Material profile.

	//! This struct contain 2 couples of value for a character position
	struct GlyphChar
	{
		Float n_sU;  //!< normalized start U
		Float n_sV;  //!< normalized start V
		Float n_eU;  //!< normalized end U
		Float n_eV;  //!< normalized end V

		UInt16 sU;  //!< start U
		UInt16 sV;  //!< start V
		UInt16 eU;  //!< end U
		UInt16 eV;  //!< end V

		Float width; //!< the width for cursor and length computation

		GlyphChar() : n_sU(0.f), n_sV(0.f), n_eU(0.f), n_eV(0.f),
			sU(0), sV(0), eU(0), eV(0), width(0) {}
	};

	//! Texture glyph element
	struct TextureGlyph
	{
		AutoPtr<Texture2D> texture;        //!< texture of supported chars
		std::map<UInt32,GlyphChar> charMap;  //!< map of supported chars
	};

	TextureGlyph m_textureGlyph;

	String m_fontdef;        //!< XML file that contain definition of the font

	ArrayFloat m_verticesArray;
	ArrayFloat m_texCoordsArray;
	ArrayUInt16 m_facesArray;

	ArrayBufferf m_vertices;
	ArrayBufferf m_texCoords;
	ElementArrayBufferus m_faces;

	Bool loadTexture(const String& filename);
};

} // namespace o3d

#endif // _O3D_TEXTUREFONT_H
