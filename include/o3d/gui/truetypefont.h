/**
 * @file truetypefont.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TTFFONT_H
#define _O3D_TTFFONT_H

#include "abcfont.h"
#include "o3d/engine/shader/shadable.h"
#include "o3d/engine/material/materialprofile.h"
#include "o3d/engine/vertexbuffer.h"
#include "o3d/core/memorydbg.h"

namespace o3d {

class Texture2D;

/**
 * @brief TrueTypeFont loading and rendering.
 * @todo Dynamically manage unloaded characters as necessary. Need a dynamic atlas.
 * @date 2005-05-09
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API TrueTypeFont : public ABCFont, public Shadable
{
public:

	O3D_DECLARE_CLASS(TrueTypeFont)

	static Bool initializeFreeType();
	static void deleteFreeType();

	//! default constructor
	TrueTypeFont(BaseObject *parent);

	//! Virtual destructor.
	virtual ~TrueTypeFont();

	//! Load a font data file
	virtual void load(
            InStream &is,
            const String &path,
            CharSet charSet = CHARSET_LATIN1,
            FontStyle style = STYLE_NORMAL);

	//! Load a font data file
    virtual void load(
            const String& filename,
            CharSet charSet = CHARSET_LATIN1,
            FontStyle style = STYLE_NORMAL);

	//! return the bounding box of a given string
	virtual Box2i getBoundingBox(const String &text);

	//! get the height of an interline
	virtual Int32 getInterline() const;

	//! Write a string without line return and return the width of the string
	virtual Int32 write(const Vector2i &pos, const String& text, Int32 curspos = -1);

	//! Get the width of this string/char
	virtual Int32 sizeOf(const String& text);

	//! Get the width of a single char
	virtual Int32 sizeOf(UInt32 achar);

    //! Get the nearest (round) character index for a given string at a given pixel position
    virtual Int32 nearestChar(const String &text, Int32 x);

	//! is this char is supported by the loaded font
	virtual Bool isSupportedChar(UInt32 c);

    //! According to a given text, a left and right limits in pixel, and a pixel position
    //! to be contained exactly in this limits, set the characters indexes (left and right),
    //! and return the adjusted position in pixels into this relative range of indexes.
    virtual Int32 adjust(
            const String &text,
            const Int32 limits[2],
            Int32 pos,
            Int32 indexes[],
            Int32 &dstLeft,
            String &dstText);

	//! nothing to process
	virtual void initialize() {}


	//-----------------------------------------------------------------------------------
	// Shadable override
	//-----------------------------------------------------------------------------------

	O3D_SHADABLE_NO_DATA_ACCESS
	O3D_SHADABLE_NO_MISC1
	O3D_SHADABLE_NO_EXT_ARRAY

	virtual VertexProgramType getVertexProgramType() const;

	virtual void processAllFaces(Shadable::ProcessingPass pass);
	virtual void attribute(VertexAttributeArray mode, UInt32 location);

protected:

	MaterialProfile m_material;  //! Material profile.
	String m_fontName;           //!< Name of the font used.
	void *m_face;

	//! Information about a glyph character
	struct GlyphChar
	{
		Float sU;  //!< normalized start U
		Float sV;  //!< normalized start V
		Float eU;  //!< normalized end U
		Float eV;  //!< normalized end V

		Int32 width;
		Int32 rows;
		Int32 left;
		Int32 top;
		Int32 advance;

		GlyphChar() : sU(0.f), sV(0.f), eU(0.f), eV(0.f),
			width(0), rows(0), left(0), top(0), advance(0) {}
	};

	//! Texture glyph element
	struct TextureGlyph
	{
		Texture2D *texture;                   //!< texture of supported chars
		std::map<UInt32,GlyphChar> charMap;  //!< map of supported chars

		//! Default constructor
        TextureGlyph() : texture(nullptr) {}
	};

	typedef std::map<UInt32,TextureGlyph> T_GlyphTextureMap;
	typedef T_GlyphTextureMap::iterator IT_GlyphTextureMap;

	T_GlyphTextureMap m_GlyphTextureMap;

	//! Generate glyphs's texture for a given font height
	Bool generateTexture(UInt32 height);

	void generateGlyph(
		std::map<UInt32,GlyphChar> &charMap,
		UInt32 charcode,
        UInt32 textureWidth,
        UInt32 fontMaxHeight,
		UInt32 &x,
		UInt32 &y);

	void generateCursor(
		std::map<UInt32,GlyphChar> &charMap,
        UInt32 textureWidth,
        UInt32 fontMaxHeight,
		UInt32 &x,
		UInt32 &y);

	ArrayFloat m_verticesArray;
	ArrayFloat m_texCoordsArray;
	ArrayUInt16 m_facesArray;

	VertexBufferObjf m_vertices;
	VertexBufferObjf m_texCoords;
	ElementBufferObjus m_faces;

	SmartArrayUInt8 m_data;
};

} // namespace o3d

#endif // _O3DTTFFONT_H

