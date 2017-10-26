/**
 * @file truetypefont.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/truetypefont.h"
#include "o3d/gui/fontmaterial.h"

#include "o3d/core/filemanager.h"
#include "o3d/core/architecture.h"
#include "o3d/engine/gl.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/material/material.h"
#include "o3d/engine/texture/texturemanager.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/gui/truetypefont.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"

#if O3D_VC_COMPILER
	#include <freetype2/include/ft2build.h>
    #include <freetype2/include/freetype/freetype.h>
    #include <freetype2/include/freetype/ftglyph.h>
    #include <freetype2/include/freetype/ftoutln.h>
    #include <freetype2/include/freetype/fttrigon.h>
#else
    #include <ft2build.h>
    #include FT_FREETYPE_H
    #include FT_GLYPH_H
#endif

using namespace o3d;

FT_Library m_freeType = nullptr;

O3D_IMPLEMENT_DYNAMIC_CLASS2(TrueTypeFont, GUI_FONT_TRUE_TYPE, ABCFont, Shadable)

Bool TrueTypeFont::initializeFreeType()
{
	// Create and initialize FreeType
	if (!m_freeType)
	{
		if (FT_Init_FreeType(&m_freeType))
		{
			O3D_ERROR(E_InvalidResult("Unable to instantiate FreeType 2 library"));
			return False;
		}
	}
    O3D_ASSERT(m_freeType != nullptr);
	return True;
}

void TrueTypeFont::deleteFreeType()
{
	if (m_freeType)
	{
		FT_Done_FreeType(m_freeType);
        m_freeType = nullptr;
	}
}

/*---------------------------------------------------------------------------------------
  constructor
---------------------------------------------------------------------------------------*/
TrueTypeFont::TrueTypeFont(BaseObject *parent) :
	ABCFont(parent),
	m_material(this),
    m_face(nullptr),
    m_vertices(getScene()->getContext()),
    m_texCoords(getScene()->getContext()),
    m_faces(getScene()->getContext())
{
	if (!getScene())
		return;

	// material
	m_material.setNumTechniques(1);
	m_material.getTechnique(0).setNumPass(1);

    MaterialPass &pass0 = m_material.getTechnique(0).getPass(0);

    pass0.setMaterial(Material::AMBIENT, new FontMaterial(this));
    pass0.setAmbientMap(getScene()->getTextureManager()->getDefaultTexture2D());
    pass0.setMapFiltering(MaterialPass::AMBIENT_MAP, Texture::NO_FILTERING);
    pass0.setMapWarp(MaterialPass::AMBIENT_MAP, Texture::CLAMP);
    pass0.setMapAnisotropy(MaterialPass::AMBIENT_MAP, 1.0f);

    pass0.setBlendingFunc(Blending::SRC_A__ONE_MINUS_SRC_A);
    pass0.setAmbient(m_color);
    pass0.disableDepthTest();
    pass0.enableAlphaTest();
    pass0.setAlphaTestFunc(COMP_GREATER, 0.f);

	m_material.prepareAndCompile(*this);
}

/*---------------------------------------------------------------------------------------
  destructor
---------------------------------------------------------------------------------------*/
TrueTypeFont::~TrueTypeFont()
{
	if (m_face)
		FT_Done_Face((FT_Face)m_face);

	// delete all glyphs textures
	for (IT_GlyphTextureMap it = m_GlyphTextureMap.begin(); it != m_GlyphTextureMap.end(); ++it)
	{
		deletePtr(it->second.texture);
	}
}

/*---------------------------------------------------------------------------------------
  Load a font data file
---------------------------------------------------------------------------------------*/
void TrueTypeFont::load(
    const String& filename,
	CharSet charSet,
	FontStyle style)
{
    if (m_face != nullptr)
		O3D_ERROR(E_InvalidOperation("Font is already loaded"));

    String fname, path;
    String fullfilename = FileManager::instance()->getFullFileName(filename);
    File::getFileNameAndPath(fullfilename, fname, path);

    InStream *is = FileManager::instance()->openInStream(fullfilename);
	try {
        load(*is, path, charSet, style);
	} catch (E_BaseException &e) {
        deletePtr(is);
		throw;
	}

    deletePtr(is);
/*
	m_fontName = fontName;
	m_CharSet = charSet;
	m_style = style;

    O3D_ASSERT(m_freeType != nullptr);

	// Load the font face
	if (FT_New_Face(m_freeType, fontName.toUtf8().getData(), 0, (FT_Face*)&m_face))
		O3D_ERROR(E_InvalidResult("Unable to load the given font"));*/
}

void TrueTypeFont::load(
        InStream &is,
        const String &path,
        CharSet charSet,
        FontStyle style)
{
    if (m_face != nullptr)
		O3D_ERROR(E_InvalidOperation("Font is already loaded"));

	m_CharSet = charSet;
	m_style = style;

    O3D_ASSERT(m_freeType != nullptr);

    m_data.allocate(is.getAvailable());
    is.read(m_data.getData(), m_data.getSize());

	// Load the font face
	if (FT_New_Memory_Face(m_freeType, (FT_Byte*)m_data.getData(), m_data.getSize(), 0, (FT_Face*)&m_face))
		O3D_ERROR(E_InvalidResult("Unable to load the given font"));
}

// get the height of an interline
Int32 TrueTypeFont::getInterline() const
{
	// return 10% of the char height as space between lines
	return (Int32)(m_charH*0.1f);
}

// Write a string/char with/out line return and return the width of the string
Int32 TrueTypeFont::write(
	const Vector2i &pos,
	const String &text,
	Int32 curspos)
{
    if (!m_face || (text.isEmpty() && curspos == -1)) {
		return 0;
    }

    if (text.isEmpty() && curspos == -1) {
        return 0;
    }

	m_material.setBlendingFunc(m_blendFunc);

	IT_GlyphTextureMap glyphTexture = m_GlyphTextureMap.find(m_charH);
    if (glyphTexture == m_GlyphTextureMap.end()) {
        if (!generateTexture(m_charH)) {
			return 0;
        }

		glyphTexture = m_GlyphTextureMap.find(m_charH);
        if (glyphTexture == m_GlyphTextureMap.end()) {
			return 0;
        }
	}

	m_material.setAmbient(m_color);
	m_material.getTechnique(0).getPass(0).setAmbientMap(glyphTexture->second.texture);

	// prepare data before drawn them
	UInt32 len = text.length() + (curspos != -1 ? 1 : 0);

	m_verticesArray.setSize(len*8);
	m_texCoordsArray.setSize(len*8);
	m_facesArray.setSize(len*6);

	Int32 curVert = 0, curUv = 0, curFace = 0, curIndex = 0;
    Float curX = (Float)pos.x(), cursorX = 0.f;
    Float curY = (Float)pos.y();

    // negative left bearing for the first char mean increase initial position
    GlyphChar *glyph = &glyphTexture->second.charMap[0];
    if (glyph->left != 0) {
        curX -= glyph->left;
    }

    for (Int32 i = 0; i < (Int32)text.length(); ++i) {
        if (curspos == i) {
			cursorX = curX;
        }

        glyph = &glyphTexture->second.charMap[(UInt32)text[i]];

		m_facesArray[curFace++] = UInt16(curIndex);
		m_facesArray[curFace++] = UInt16(curIndex+1);
		m_facesArray[curFace++] = UInt16(curIndex+2);
		m_facesArray[curFace++] = UInt16(curIndex);
		m_facesArray[curFace++] = UInt16(curIndex+2);
		m_facesArray[curFace++] = UInt16(curIndex+3);

		curIndex += 4;

        m_verticesArray[curVert++] = curX + glyph->left; m_verticesArray[curVert++] = curY + glyph->rows - glyph->top;
        m_texCoordsArray[curUv++] = glyph->sU;
        m_texCoordsArray[curUv++] = glyph->eV;

        m_verticesArray[curVert++] = curX + glyph->left + glyph->width; m_verticesArray[curVert++] = curY + glyph->rows - glyph->top;
        m_texCoordsArray[curUv++] = glyph->eU;
        m_texCoordsArray[curUv++] = glyph->eV;

        m_verticesArray[curVert++] = curX + glyph->left + glyph->width; m_verticesArray[curVert++] = curY - glyph->top;
        m_texCoordsArray[curUv++] = glyph->eU;
        m_texCoordsArray[curUv++] = glyph->sV;

        m_verticesArray[curVert++] = curX + glyph->left; m_verticesArray[curVert++] = curY - glyph->top;
        m_texCoordsArray[curUv++] = glyph->sU;
        m_texCoordsArray[curUv++] = glyph->sV;

		// translate for next char
        curX += glyph->advance;
	}

	// draw the cursor
    if (curspos != -1) {
		// cursor is at the end of the string
        if (curspos == (Int32)text.length()) {
			cursorX = curX;
        }

		// special value 0 is used for cursor
        glyph = &glyphTexture->second.charMap[0];

		m_facesArray[curFace++] = UInt16(curIndex);
		m_facesArray[curFace++] = UInt16(curIndex+1);
		m_facesArray[curFace++] = UInt16(curIndex+2);
		m_facesArray[curFace++] = UInt16(curIndex);
		m_facesArray[curFace++] = UInt16(curIndex+2);
		m_facesArray[curFace++] = UInt16(curIndex+3);

		curIndex += 4;

        m_verticesArray[curVert++] = cursorX + glyph->left; m_verticesArray[curVert++] = curY + glyph->rows - glyph->top;
        m_texCoordsArray[curUv++] = glyph->sU;
        m_texCoordsArray[curUv++] = glyph->eV;

        m_verticesArray[curVert++] = cursorX + glyph->left + glyph->width; m_verticesArray[curVert++] = curY + glyph->rows - glyph->top;
        m_texCoordsArray[curUv++] = glyph->eU;
        m_texCoordsArray[curUv++] = glyph->eV;

        m_verticesArray[curVert++] = cursorX + glyph->left + glyph->width; m_verticesArray[curVert++] = curY - glyph->top;
        m_texCoordsArray[curUv++] = glyph->eU;
        m_texCoordsArray[curUv++] = glyph->sV;

        m_verticesArray[curVert++] = cursorX + glyph->left; m_verticesArray[curVert++] = curY - glyph->top;
        m_texCoordsArray[curUv++] = glyph->sU;
        m_texCoordsArray[curUv++] = glyph->sV;
	}

    m_material.processMaterial(*this, nullptr, nullptr, DrawInfo());

    return (Int32)curX - pos.x();
}

/*---------------------------------------------------------------------------------------
  Get size of this string
---------------------------------------------------------------------------------------*/
Int32 TrueTypeFont::sizeOf(const String &text)
{
	Int32 size = 0;
	Int32 len = text.length();

    if (len == 0) {
        return 0;
    }

	IT_GlyphTextureMap glyphTexture = m_GlyphTextureMap.find(m_charH);
    if (glyphTexture == m_GlyphTextureMap.end()) {
        if (!generateTexture(m_charH)) {
			return 0;
        }

		glyphTexture = m_GlyphTextureMap.find(m_charH);
        if (glyphTexture == m_GlyphTextureMap.end()) {
			return 0;
        }
	}

    // negative left bearing for the first char mean increase width
    GlyphChar *glyph = &glyphTexture->second.charMap[(UInt32)text[0]];
    if (glyph->left != 0) {
        size -= glyph->left;
    }

    for (Int32 i = 0; i < len; ++i) {
        glyph = &glyphTexture->second.charMap[(UInt32)text[i]];
        size += glyph->advance;
	}

	return size;
}

Int32 TrueTypeFont::sizeOf(UInt32 achar)
{
	IT_GlyphTextureMap glyphTexture = m_GlyphTextureMap.find(m_charH);
    if (glyphTexture == m_GlyphTextureMap.end()) {
        if (!generateTexture(m_charH)) {
			return 0;
        }

		glyphTexture = m_GlyphTextureMap.find(m_charH);
        if (glyphTexture == m_GlyphTextureMap.end()) {
			return 0;
        }
	}

	GlyphChar &glyph = glyphTexture->second.charMap[(UInt32)achar];
    return glyph.advance;
}

Int32 TrueTypeFont::nearestChar(const String &text, Int32 x)
{
    Int32 size = 0;
    Int32 len = text.length();

    if (len == 0) {
        return 0;
    }

    IT_GlyphTextureMap glyphTexture = m_GlyphTextureMap.find(m_charH);
    if (glyphTexture == m_GlyphTextureMap.end()) {
        if (!generateTexture(m_charH)) {
            return 0;
        }

        glyphTexture = m_GlyphTextureMap.find(m_charH);
        if (glyphTexture == m_GlyphTextureMap.end()) {
            return 0;
        }
    }

    // negative left bearing for the first char mean increase width
    GlyphChar *glyph = &glyphTexture->second.charMap[(UInt32)text[0]];
    if (glyph->left != 0) {
        size -= glyph->left;
    }

    for (Int32 i = 0; i < len; ++i) {
        glyph = &glyphTexture->second.charMap[(UInt32)text[i]];

        if (x <= (size + glyph->advance/2)) {
            return i;
        }

        size += glyph->advance;
    }

    return len;
}

/*---------------------------------------------------------------------------------------
  return the bounding box of a given string
---------------------------------------------------------------------------------------*/
Box2i TrueTypeFont::getBoundingBox(const String &text)
{
	Int32 xmin = 0;
	Int32 ymin = o3d::Limits<Int32>::max();
	Int32 xmax = 0;
	Int32 ymax = o3d::Limits<Int32>::min();

	Int32 len = text.length();

    if (len == 0) {
        return 0;   
    }

	IT_GlyphTextureMap glyphTexture = m_GlyphTextureMap.find(m_charH);
    if (glyphTexture == m_GlyphTextureMap.end()) {
        if (!generateTexture(m_charH)) {
			return 0;
        }

		glyphTexture = m_GlyphTextureMap.find(m_charH);
        if (glyphTexture == m_GlyphTextureMap.end()) {
			return 0;
        }
	}

    // negative left bearing for the first char mean increase width
    GlyphChar *glyph = &glyphTexture->second.charMap[(UInt32)text[0]];
    if (glyph->left != 0) {
        xmax -= glyph->left;
    }

    for (Int32 i = 0; i < len; ++i) {
        glyph = &glyphTexture->second.charMap[(UInt32)text[i]];

        xmax += glyph->advance;
        ymin = o3d::min(ymin, -glyph->rows);
        ymax = o3d::max(ymax, glyph->rows-glyph->top);
	}

	return Box2i(xmin, ymin, xmax-xmin, ymax-ymin);
}

/*---------------------------------------------------------------------------------------
  is this char is supported by the loaded font
---------------------------------------------------------------------------------------*/
Bool TrueTypeFont::isSupportedChar(UInt32 c)
{
	IT_GlyphTextureMap glyphTexture = m_GlyphTextureMap.find(m_charH);
    if (glyphTexture == m_GlyphTextureMap.end()) {
        if (!generateTexture(m_charH)) {
			return False;
        }

		glyphTexture = m_GlyphTextureMap.find(m_charH);
        if (glyphTexture == m_GlyphTextureMap.end()) {
			return False;
        }
	}

    if (!c) {
		return False;
    }

    return (glyphTexture->second.charMap.find(c) != glyphTexture->second.charMap.end());
}

Int32 TrueTypeFont::adjust(
        const String &text,
        const Int32 limits[],
        Int32 pos,
        Int32 indexes[],
        Int32 &dstLeft,
        String &dstText)
{
    IT_GlyphTextureMap glyphTexture = m_GlyphTextureMap.find(m_charH);
    if (glyphTexture == m_GlyphTextureMap.end()) {
        if (!generateTexture(m_charH)) {
            return 0;
        }

        glyphTexture = m_GlyphTextureMap.find(m_charH);
        if (glyphTexture == m_GlyphTextureMap.end()) {
            return 0;
        }
    }

    // fill n char from leftPos to width
    Int32 curPos = 0;
    Int32 firstCharIndex = 0;

    dstLeft = limits[0];
    Int32 rightPos = limits[1];
    Int32 width = limits[1] - limits[0] + 1;

    Int32 leftChar = 0;

    // pos is at left outside
    if (pos < dstLeft) {
        // shift the left limit
        dstLeft = o3d::max(pos - (m_charH >> 1), 0);
    }

    // pos is at right outside
    if (pos > rightPos) {
        // shift to contains pos as the last character
        dstLeft = pos - width;
        rightPos = dstLeft + width;
    }

    //
    // now update the content
    //

    Int32 leftBearing = 0;

    Int32 charWidth;
    GlyphChar *glyph;

    // first, search for the first character
    while ((curPos < dstLeft) && (firstCharIndex < (Int32)text.length())) {
        glyph = &glyphTexture->second.charMap[(UInt32)text[firstCharIndex]];
        curPos += glyph->advance;

        if ((firstCharIndex == 0) && (glyph->left != 0)) {
            curPos -= glyph->left;
        }

        ++firstCharIndex;
    }

    dstLeft = curPos;
    leftChar = firstCharIndex;

    // next, fill enough characters to fill the text zone in width
    Int32 lastCharIndex = firstCharIndex;
    Int32 curWidth = 0;

    while ((curPos < rightPos) && (lastCharIndex < (Int32)text.length())) {
        glyph = &glyphTexture->second.charMap[(UInt32)text[lastCharIndex]];
        charWidth = glyph->advance;

        if (lastCharIndex == firstCharIndex && glyph->left != 0) {
            charWidth -= glyph->left;
            leftBearing = glyph->left;
        }

        curPos += charWidth;
        curWidth += charWidth;

        dstText.append(text[lastCharIndex]);

        ++lastCharIndex;
    }

    // last char is partially display, does not count into visible
    if (curPos > rightPos) {
        --lastCharIndex;
    }

    // free space at right and some characters out of screen at left
    // then shift of some char to right
    if ((curWidth < width) && (dstLeft > 0)) {
        while ((curWidth < width) && (firstCharIndex > 0)) {
            --firstCharIndex;

            glyph = &glyphTexture->second.charMap[(UInt32)text[firstCharIndex]];
            charWidth = glyph->advance;

            if (leftBearing != 0) {
                // avoid previous left bearing
                curWidth += leftBearing;

                if (glyph->left != 0) {
                    charWidth -= glyph->left;
                    leftBearing = glyph->left;
                } else {
                    leftBearing = 0;
                }
            }

            if (charWidth + curWidth > width) {
                // restore previous index because we want an exact value for indexes
                ++firstCharIndex;
                break;
            }

            dstText.insert(text[firstCharIndex], 0);

            curWidth += charWidth;
            dstLeft -= charWidth;
            --leftChar;
        }
    }

    indexes[0] = firstCharIndex;
    indexes[1] = lastCharIndex;

    // new position set after defining the new text zone content
    return o3d::max(0, pos - dstLeft + leftBearing);
}

/*---------------------------------------------------------------------------------------
  Generate glyphs's texture for a given font height
---------------------------------------------------------------------------------------*/
Bool TrueTypeFont::generateTexture(UInt32 height)
{
	TextureGlyph textureGlyph;
	UInt32 squareRoot;
	UInt32 maxHeight = height;

    if (m_CharSet == CHARSET_ASCII) {
        squareRoot = 10;

        // compute the size of the texture for 128-32 chars
		UInt32 width = squareRoot*(height+1);
		UInt32 x = 0, y = 0;

		// width must be power of two
		UInt32 tmp = 2;
        while ((width = width >> 1) != 0) {
			tmp <<= 1;
		}
		width = tmp;

        if (width > (UInt32)getScene()->getContext()->getTextureMaxSize()) {
			O3D_ERROR(E_InvalidParameter("Unsupported font height. Desired height is to big for hardware"));
        }

		textureGlyph.texture = new Texture2D(this);
		textureGlyph.texture->setPersistant(True);

		// create the texture with a black background
		ArrayUInt8 black((UInt8)0, width*width, 0);
		textureGlyph.texture->create(False, width, width, PF_RED_U8, black.getData(), PF_RED_U8, True);
		black.destroy();

//		FT_GlyphSlot slot = ((FT_Face)m_pFace)->glyph; not used
		//FT_Set_Char_Size((FT_Face)m_face, 0, height << 6, 72, 72);
		FT_Set_Pixel_Sizes((FT_Face)m_face, 0, height);

		// generate the cursor with special charcode = 0
        generateCursor(textureGlyph.charMap, width, maxHeight+1, x, y);

		// generate char from 32 to 128
        for (UInt32 charcode = 0x20; charcode < 0x80; ++charcode) {
			generateGlyph(textureGlyph.charMap, charcode, width, maxHeight, x, y);
		}

        getScene()->getContext()->bindTexture(TEXTURE_2D, 0, True);

		m_GlyphTextureMap[height] = textureGlyph;
		return True;
    } else if (m_CharSet == CHARSET_LATIN1)	{
        // floor(sqtr(512-32+2 characters))
        squareRoot = 22;

        // compute the size of the texture with 4/5 of height because most of the
        // characters have width lesser than height, and +1 becaue of an empty pixel
        // between each character
        UInt32 width = (squareRoot*(height+1) * 4) / 5;
		UInt32 x = 0, y = 0;

		// width must be power of two
		UInt32 tmp = 2;
        while ((width = width >> 1) != 0) {
			tmp <<= 1;
		}
		width = tmp;

        if (width > (UInt32)getScene()->getContext()->getTextureMaxSize()) {
			O3D_ERROR(E_InvalidParameter("Unsupported font height. Desired height is to big for hardware"));
        }

		textureGlyph.texture = new Texture2D(this);
		textureGlyph.texture->setPersistant(True);

		// create the texture with a black background
        ArrayUInt8 black((UInt8)0, width*width, 0);
		textureGlyph.texture->create(False, width, width, PF_RED_U8, black.getData(), PF_RED_U8, True);
		black.destroy();
#if 0
		// full of black (for debug)
		ArrayUInt8 buffer(width*width);
		glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				0,
				0,
				width,
				width,
				GL_RED,
				GL_UNSIGNED_BYTE,
				buffer.getData());
#endif
		//FT_GlyphSlot slot = ((FT_Face)m_pFace)->glyph; not used
		//FT_Set_Char_Size((FT_Face)m_face, 0, height << 6, 72, 72);
		FT_Set_Pixel_Sizes((FT_Face)m_face, 0, height);

		// generate the cursor with special charcode = 0
        generateCursor(textureGlyph.charMap, width, maxHeight+1, x, y);

        // 32 to 128 : Basic Latin
        for (UInt32 charcode = 0x020; charcode < 0x080; ++charcode) {
			generateGlyph(textureGlyph.charMap, charcode, width, maxHeight, x, y);
		}

        // 128 to 256 : Latin-1 Supplement
        for (UInt32 charcode = 0x080; charcode < 0x100; ++charcode) {
            generateGlyph(textureGlyph.charMap, charcode, width, maxHeight, x, y);
        }

        // 256 to 384 : Latin Extended-A (for example oe is 0x153)
        for (UInt32 charcode = 0x100; charcode < 0x180; ++charcode) {
            generateGlyph(textureGlyph.charMap, charcode, width, maxHeight, x, y);
        }

        // 384 to 512 : Latin Extended-B
        for (UInt32 charcode = 0x180; charcode < 0x200; ++charcode) {
            generateGlyph(textureGlyph.charMap, charcode, width, maxHeight, x, y);
        }

		// EURO symbol
		generateGlyph(textureGlyph.charMap, 8364, width, maxHeight, x, y);
		// typographic apostrophe
		generateGlyph(textureGlyph.charMap, 8217, width, maxHeight, x, y);

#if 0
		Image out; // for debug
		out.allocate(width, width, 3);
		UInt8 *data = out.getDataWrite();
		glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_UNSIGNED_BYTE,data);
		out.save("out.bmp",Image::PNG, 0);
#endif

		getScene()->getContext()->bindTexture(TEXTURE_2D, 0, True);

		m_GlyphTextureMap[height] = textureGlyph;
		return True;
    } else {
		return False;
	}
}

void TrueTypeFont::generateGlyph(
	std::map<UInt32,GlyphChar> &charMap,
	UInt32 charcode,
    UInt32 textureWidth,
    UInt32 fontMaxHeight,
	UInt32 &x,
	UInt32 &y)
{
	FT_GlyphSlot slot = ((FT_Face)m_face)->glyph;
	FT_UInt glyph_index = FT_Get_Char_Index((FT_Face)m_face, charcode);

    if (FT_Load_Glyph((FT_Face)m_face, glyph_index, FT_LOAD_RENDER)) {
		GlyphChar newChar;

		newChar.sU = newChar.sV = newChar.eU = newChar.eV = 0.f;
		newChar.width = newChar.rows = newChar.left = newChar.top = newChar.advance = 0;

		charMap[charcode] = newChar;
    } else {
        if ((x + slot->bitmap.width) > textureWidth) {
			x = 0;
            y += fontMaxHeight + 1;

            if ((y + slot->bitmap.rows) > textureWidth) {
				O3D_ERROR(E_InvalidResult("Glyph generation render out of region"));
            }
		}

		GlyphChar newChar;

        newChar.sU = (Float)x / textureWidth;
        newChar.sV = (Float)y / textureWidth;
        newChar.eU = (Float)(x+slot->bitmap.width) / textureWidth;
        newChar.eV = (Float)(y+slot->bitmap.rows) / textureWidth;

		newChar.width = slot->bitmap.width;
		newChar.rows = slot->bitmap.rows;
		newChar.left = slot->bitmap_left;
		newChar.top = slot->bitmap_top;
		newChar.advance = slot->advance.x >> 6;

		charMap[charcode] = newChar;

        if (slot->bitmap.buffer) {
			glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					x,
					y,
					slot->bitmap.width,
					slot->bitmap.rows,
					GL_RED,
					GL_UNSIGNED_BYTE,
					slot->bitmap.buffer);

			x += slot->bitmap.width + 1;
		}
	}
}

void TrueTypeFont::generateCursor(std::map<UInt32,GlyphChar> &charMap,
    UInt32 textureWidth,
    UInt32 fontMaxHeight,
    UInt32 &x,
    UInt32 &y)
{
	Int32 cursorWidth = 1;

    if ((x + cursorWidth) > textureWidth) {
		x = 0;
        y += fontMaxHeight + 1;

        if (y > textureWidth) {
			O3D_ERROR(E_InvalidResult("Cursor generation render out of region"));
        }
	}

	GlyphChar newChar;

    newChar.sU = (Float)x / textureWidth;
    newChar.sV = (Float)y / textureWidth;
    newChar.eU = (Float)(x+cursorWidth) / textureWidth;
    newChar.eV = (Float)(y+fontMaxHeight) / textureWidth;

	newChar.width = cursorWidth;
    newChar.rows = fontMaxHeight;
    newChar.left = -1;//0;
    newChar.top = (fontMaxHeight*4)/5;
	newChar.advance = 0;

	charMap[0] = newChar;

    ArrayUInt8 buffer(cursorWidth*fontMaxHeight);
    memset(&buffer[0], 255, cursorWidth*fontMaxHeight);

    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, cursorWidth, fontMaxHeight, GL_RED, GL_UNSIGNED_BYTE, buffer.getData());

	x += cursorWidth + 1;
}

Shadable::VertexProgramType TrueTypeFont::getVertexProgramType() const
{
	return VP_MESH;
}

void TrueTypeFont::processAllFaces(Shadable::ProcessingPass pass)
{
	if (pass == Shadable::PREPARE_GEOMETRY)	{
        // reallocate if greater plus a threshold coming from the TemplateArray
        // so its the reason why we use getMaxSize() and not getSize() here.
        if (m_vertices.getCount() < (UInt32)m_verticesArray.getMaxSize()) {
            m_vertices.create(m_verticesArray.getMaxSize(), VertexBuffer::STREAMED, m_verticesArray.getData());
            m_texCoords.create(m_texCoordsArray.getMaxSize(), VertexBuffer::STREAMED, m_texCoordsArray.getData());
            m_faces.create(m_facesArray.getMaxSize(), VertexBuffer::STREAMED, m_facesArray.getData());
        } else {
            // simply update
            m_vertices.update(m_verticesArray.getData(), 0, m_verticesArray.getSize());
            m_texCoords.update(m_texCoordsArray.getData(), 0, m_texCoordsArray.getSize());
        }

        // same for faces
        if (m_faces.getCount() < (UInt32)m_facesArray.getMaxSize()) {
            m_faces.create(m_facesArray.getMaxSize(), VertexBuffer::STREAMED, m_facesArray.getData());
        } else {
            m_faces.update(m_facesArray.getData(), 0, m_facesArray.getSize());
        }
    } else if (pass == Shadable::PROCESS_GEOMETRY) {
		m_faces.bindBuffer();
		getScene()->drawRangeElementsUInt16(
				P_TRIANGLES,
				0,
				(m_verticesArray.getSize() >> 1) - 1,
				m_facesArray.getSize(),
                nullptr);
	}
}

void TrueTypeFont::attribute(VertexAttributeArray mode, UInt32 location)
{
    if (mode == V_VERTICES_ARRAY) {
		m_vertices.attribute(location, 2, 0, 0);
    } else if (mode == V_TEXCOORDS_2D_1_ARRAY) {
		m_texCoords.attribute(location, 2, 0, 0);
    }
}

