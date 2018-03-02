/**
 * @file texturefont.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/engine/glextdefines.h"
#include "o3d/gui/texturefont.h"
#include "o3d/gui/fontmaterial.h"

#include "o3d/core/architecture.h"
#include "o3d/core/xmldoc.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/context.h"
#include "o3d/engine/matrix.h"
#include "o3d/core/filemanager.h"
#include "o3d/gui/gui.h"
#include "o3d/gui/guitype.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/texture/texturemanager.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS2(TextureFont, GUI_FONT_TEXTURE, ABCFont, Shadable)

/*---------------------------------------------------------------------------------------
  constructor
---------------------------------------------------------------------------------------*/
TextureFont::TextureFont(BaseObject *parent) :
	ABCFont(parent),
	m_material(this),
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

// destructor
TextureFont::~TextureFont()
{
}

void TextureFont::load(const String& filename, CharSet charSet, FontStyle style)
{
    String fname, path;
    String fullfilename = FileManager::instance()->getFullFileName(filename);
    FileManager::getFileNameAndPath(fullfilename, fname, path);

    InStream *is = FileManager::instance()->openInStream(fullfilename);

    try {
        load(*is, path, charSet, style);
    } catch (E_BaseException &e) {
        deletePtr(is);
		throw;
	}
}

// Load a font data file
void TextureFont::load(InStream &is, const String &path, CharSet charSet, FontStyle style)
{
	m_CharSet = charSet;
	m_style = style;

	XmlDoc xmlDoc("texturefont");

    xmlDoc.read(is);

    String filePath = FileManager::instance()->getFullFileName(path);
	String tmp;

	// read xml content
	// element TextureFont
	TiXmlElement *pNode = xmlDoc.getDoc()->FirstChildElement("TextureFont");
	if (!pNode)
        O3D_ERROR(E_InvalidFormat("Missing TextureFont token in "));

	// element Global
	pNode = xmlDoc.getDoc()->FirstChildElement()->FirstChildElement("Global");
	if (!pNode)
        O3D_ERROR(E_InvalidFormat("Missing Global token in "));

	Int32 tmp_int;
	String textureFiles[5], texturefile;

	// texture filename
	xmlDoc.getEltAttStr(pNode,"texture",texturefile);

	// style
	xmlDoc.getEltAttStr(pNode,"normal", textureFiles[0]);
	xmlDoc.getEltAttStr(pNode,"italic", textureFiles[1]);
	xmlDoc.getEltAttStr(pNode,"bold", textureFiles[2]);
	xmlDoc.getEltAttStr(pNode,"underlined", textureFiles[3]);
	xmlDoc.getEltAttStr(pNode,"strikeout", textureFiles[4]);

	texturefile = textureFiles[o3d::log2(style)];
	if (texturefile.isEmpty())
        O3D_ERROR(E_InvalidFormat("Unavailable font style "));

	String styleName;

	if (style == STYLE_NORMAL)
		styleName = "Normal";
	else if (style == STYLE_ITALIC)
		styleName = "Italic";
	else if (style == STYLE_BOLD)
		styleName = "Bold";
	else if (style == STYLE_UNDERLINED)
		styleName = "Underlined";
	else if (style == STYLE_STRIKEOUT)
		styleName = "StrikeOut";

	if (styleName.isEmpty())
        O3D_ERROR(E_InvalidFormat("Missing font style to the Load method "));

	// blending
	xmlDoc.getEltAttStr(pNode,"blending", tmp);
	if (tmp == "alphablend")
         m_blendFunc = Blending::SRC_A__ONE;
	else if (tmp == "alphamask")
        m_blendFunc = Blending::ONE__ONE;

	// charset
	xmlDoc.getEltAttStr(pNode,"charset", tmp);
	if (tmp == "ascii")
		m_CharSet = CHARSET_ASCII;
	else if (tmp == "occidental")
		m_CharSet = CHARSET_LATIN1;

	if (charSet != m_CharSet)
        O3D_ERROR(E_InvalidFormat("Font style from the file differ from desired one"));

	// element Data
	pNode = xmlDoc.getDoc()->FirstChildElement()->FirstChildElement("Data");
	if (!pNode)
        O3D_ERROR(E_InvalidFormat("Missing Data token in "));

    TiXmlNode *pFontNode = nullptr;
    TiXmlNode *pSubFontNode = nullptr;

	Int32 charcode = 0;
	const Char *bufchar;

	// cursor definition
	pFontNode = pNode->FirstChild("Cursor");
	if (pFontNode)
	{
		GlyphChar cursorChar;

		pFontNode->ToElement()->Attribute("sU",&tmp_int); cursorChar.sU = UInt16(tmp_int);
		pFontNode->ToElement()->Attribute("eU",&tmp_int); cursorChar.eU = UInt16(tmp_int);
		pFontNode->ToElement()->Attribute("sV",&tmp_int); cursorChar.sV = UInt16(tmp_int);
		pFontNode->ToElement()->Attribute("eV",&tmp_int); cursorChar.eV = UInt16(tmp_int);

		m_textureGlyph.charMap[0] = cursorChar;
	}

    pFontNode = nullptr;

	// the different subfont
    while ((pFontNode = pNode->IterateChildren("Char",pFontNode)) != nullptr)
	{
		// ASCII char code
		bufchar = pFontNode->ToElement()->Attribute("ascii");
		if (bufchar)
			charcode = (Char)bufchar[0];
		else
		// utf8 charcode, to convert to widechar
		{
			bufchar = pFontNode->ToElement()->Attribute("utf8");
			if (bufchar)
			{
				String myChar;
				myChar.fromUtf8(bufchar);
				charcode = myChar[0];
			}
		}

		GlyphChar cursorChar;

		pSubFontNode = pFontNode->FirstChildElement(styleName.toUtf8().getData());
		if (pSubFontNode)
		{
			pSubFontNode->ToElement()->Attribute("sU",&tmp_int); cursorChar.sU = UInt16(tmp_int);
			pSubFontNode->ToElement()->Attribute("eU",&tmp_int); cursorChar.eU = UInt16(tmp_int);
			pSubFontNode->ToElement()->Attribute("sV",&tmp_int); cursorChar.sV = UInt16(tmp_int);
			pSubFontNode->ToElement()->Attribute("eV",&tmp_int); cursorChar.eV = UInt16(tmp_int);
		}

		m_textureGlyph.charMap[charcode] = cursorChar;
	}

	// load the texture
	if (loadTexture(filePath + '/' + texturefile))
		initialize();
}

// load a font texture file : path relative to Gui
Bool TextureFont::loadTexture(const String &filename)
{
	Image pic;

	if (!pic.load(filename))
		return False;

	// take luminance or rgb format
	if ((pic.getPixelFormat() != PF_RED_8) || !pic.isRgb())
		return False;

	m_textureGlyph.texture = new Texture2D(this, pic);
	m_textureGlyph.texture->setPersistant(True);

	if (!m_textureGlyph.texture->create(False))
	{
        m_textureGlyph.texture = nullptr;
		return False;
	}

	m_material.getTechnique(0).getPass(0).setAmbientMap(m_textureGlyph.texture);

	return True;
}

Shadable::VertexProgramType TextureFont::getVertexProgramType() const
{
	return VP_MESH;
}

void TextureFont::processAllFaces(Shadable::ProcessingPass pass)
{
	if (pass == Shadable::PREPARE_GEOMETRY)
	{
        // reallocate if greater plus a threshold coming from the TemplateArray
        // so its the reason why we use getMaxSize() and not getSize() here.
        if (m_vertices.getCount() < (UInt32)m_verticesArray.getMaxSize())
        {
            m_vertices.create(m_verticesArray.getMaxSize(), VertexBuffer::STREAMED, m_verticesArray.getData());
            m_texCoords.create(m_texCoordsArray.getMaxSize(), VertexBuffer::STREAMED, m_texCoordsArray.getData());
            m_faces.create(m_facesArray.getMaxSize(), VertexBuffer::STREAMED, m_facesArray.getData());
        }
        else
        {
            // simply update
            m_vertices.update(m_verticesArray.getData(), 0, m_verticesArray.getSize());
            m_texCoords.update(m_texCoordsArray.getData(), 0, m_texCoordsArray.getSize());
        }

        // same for faces
        if (m_faces.getCount() < (UInt32)m_facesArray.getMaxSize())
        {
            m_faces.create(m_facesArray.getMaxSize(), VertexBuffer::STREAMED, m_facesArray.getData());
        }
        else
        {
            m_faces.update(m_facesArray.getData(), 0, m_facesArray.getSize());
        }
	}
	else if (pass == Shadable::PROCESS_GEOMETRY)
	{
		m_faces.bindBuffer();
		getScene()->drawRangeElementsUInt16(
				P_TRIANGLES,
				0,
				(m_verticesArray.getSize() >> 1) - 1,
				m_facesArray.getSize(),
                nullptr);
	}
}

void TextureFont::attribute(VertexAttributeArray mode, UInt32 location)
{
	if (mode == V_VERTICES_ARRAY)
	{
		m_vertices.bindBuffer();
		getScene()->getContext()->vertexAttribArray(location, 2, 0, 0);
	}
	else if (mode == V_UV_MAP_ARRAY)
	{
		m_texCoords.bindBuffer();
		getScene()->getContext()->vertexAttribArray(location, 2, 0, 0);
	}
}

/*---------------------------------------------------------------------------------------
  get the height of an interline
---------------------------------------------------------------------------------------*/
Int32 TextureFont::getInterline() const
{
	return (Int32)(m_charH*0.1f); // at this time return 10% of the char height as interline
}

/*---------------------------------------------------------------------------------------
  Write a string/char with/out line return and return the width of the string
---------------------------------------------------------------------------------------*/
Int32 TextureFont::write(
	const Vector2i &pos,
	const String& text,
	Int32 curspos)
{
	if (!m_textureGlyph.texture || !m_textureGlyph.texture->isValid() || (text.isEmpty() && curspos == -1))
		return 0;

    m_material.setBlendingFunc(m_blendFunc);
	m_material.setAmbient(m_color);

	// prepare data before draw them
	UInt32 len = text.length() + (curspos != -1 ? 1 : 0);

	m_verticesArray.setSize(len*8);
	m_texCoordsArray.setSize(len*8);
	m_facesArray.setSize(len*6);

	Int32 curVert = 0, curUv = 0, curFace = 0, curIndex = 0;
    Float curX = (Float)pos.x(), cursorX = 0.f;
    Float curY = (Float)pos.y()- m_charH; // on baseline

	for (Int32 i = 0; i < (Int32)text.length(); ++i)
	{
		if (curspos == i)
			cursorX = curX;

		GlyphChar &glyph = m_textureGlyph.charMap[(UInt32)text[i]];

		m_facesArray[curFace++] = UInt16(curIndex);
		m_facesArray[curFace++] = UInt16(curIndex+1);
		m_facesArray[curFace++] = UInt16(curIndex+2);
		m_facesArray[curFace++] = UInt16(curIndex);
		m_facesArray[curFace++] = UInt16(curIndex+2);
		m_facesArray[curFace++] = UInt16(curIndex+3);

		curIndex += 4;

		m_verticesArray[curVert++] = curX; m_verticesArray[curVert++] = curY + m_charH;
		m_texCoordsArray[curUv++] = glyph.n_sU;
		m_texCoordsArray[curUv++] = glyph.n_eV;

		m_verticesArray[curVert++] = curX +(glyph.width*m_charH); m_verticesArray[curVert++] = curY + m_charH;
		m_texCoordsArray[curUv++] = glyph.n_eU;
		m_texCoordsArray[curUv++] = glyph.n_eV;

		m_verticesArray[curVert++] = curX + (glyph.width*m_charH); m_verticesArray[curVert++] = curY;
		m_texCoordsArray[curUv++] = glyph.n_eU;
		m_texCoordsArray[curUv++] = glyph.n_sV;

		m_verticesArray[curVert++] = curX; m_verticesArray[curVert++] = curY;
		m_texCoordsArray[curUv++] = glyph.n_sU;
		m_texCoordsArray[curUv++] = glyph.n_sV;

		// translate for next char
		curX += glyph.width * m_charH;
	}

	// draw the cursor
	if (curspos != -1)
	{
		// cursor is at the end of the string
		if (curspos == (Int32)text.length())
			cursorX = curX;

		GlyphChar &glyph = m_textureGlyph.charMap[0];

		m_facesArray[curFace++] = UInt16(curIndex);
		m_facesArray[curFace++] = UInt16(curIndex+1);
		m_facesArray[curFace++] = UInt16(curIndex+2);
		m_facesArray[curFace++] = UInt16(curIndex);
		m_facesArray[curFace++] = UInt16(curIndex+2);
		m_facesArray[curFace++] = UInt16(curIndex+3);

		curIndex += 4;

		m_verticesArray[curVert++] = cursorX; m_verticesArray[curVert++] = curY + m_charH;
		m_texCoordsArray[curUv++] = glyph.n_sU;
		m_texCoordsArray[curUv++] = glyph.n_eV;

		m_verticesArray[curVert++] = cursorX + (glyph.width*m_charH); m_verticesArray[curVert++] = curY + m_charH;
		m_texCoordsArray[curUv++] = glyph.n_eU;
		m_texCoordsArray[curUv++] = glyph.n_eV;

		m_verticesArray[curVert++] = cursorX + (glyph.width*m_charH); m_verticesArray[curVert++] = curY;
		m_texCoordsArray[curUv++] = glyph.n_eU;
		m_texCoordsArray[curUv++] = glyph.n_sV;

		m_verticesArray[curVert++] = cursorX; m_verticesArray[curVert++] = curY;
		m_texCoordsArray[curUv++] = glyph.n_sU;
		m_texCoordsArray[curUv++] = glyph.n_sV;
	}

    m_material.processMaterial(*this, nullptr, nullptr, DrawInfo());

    return (Int32)curX - pos.x();
}

/*---------------------------------------------------------------------------------------
  Get size of this string
---------------------------------------------------------------------------------------*/
Int32 TextureFont::sizeOf(const String &text)
{
	Int32 size = 0;
	Int32 len = text.length();

	for (Int32 i = 0; i < len; ++i)
	{
		GlyphChar &glyph = m_textureGlyph.charMap[(UInt32)text[i]];
		size += (Int32)(glyph.width * m_charH);
	}

	return size;
}

Int32 TextureFont::sizeOf(UInt32 achar)
{
	GlyphChar &glyph = m_textureGlyph.charMap[(UInt32)achar];
    return (Int32)(glyph.width * m_charH);
}

Int32 TextureFont::nearestChar(const String &text, Int32 x)
{
    Int32 size = 0;
    Int32 len = text.length();
    Int32 advance;

    for (Int32 i = 0; i < len; ++i)
    {
        GlyphChar &glyph = m_textureGlyph.charMap[(UInt32)text[i]];
        advance = (Int32)(glyph.width * m_charH);

        if (x <= (size + advance/2))
            return i;

        size += advance;
    }

    return len;
}

/*---------------------------------------------------------------------------------------
  return the bounding box of a given string
---------------------------------------------------------------------------------------*/
Box2i TextureFont::getBoundingBox(const String &text)
{
	Int32 xmin = 0;
	Int32 ymin = o3d::Limits<Int32>::max();
	Int32 xmax = 0;
	Int32 ymax = o3d::Limits<Int32>::min();

	Int32 len = text.length();

	for (Int32 i = 0; i < len; ++i)
	{
		GlyphChar &glyph = m_textureGlyph.charMap[(UInt32)text[i]];

		xmax += (Int32)(glyph.width * m_charH);
        ymin = o3d::min(ymin, 0);//-glyph.rows);
        ymax = o3d::max(ymax, m_charH);//glyph.rows-glyph.top);
	}

    return Box2i(xmin, ymin, xmax-xmin, ymax-ymin);
}

Int32 TextureFont::adjust(
        const String &text,
        const Int32 limits[],
        Int32 pos,
        Int32 indexes[],
        Int32 &dstLeft,
        String &dstText)
{
    // fill n char from leftPos to width
    Int32 curPos = 0;
    UInt32 firstCharIndex = 0;

    dstLeft = limits[0];
    Int32 rightPos = limits[1];
    Int32 width = limits[1] - limits[0] + 1;

    Int32 leftChar = 0;

    // pos is at left outside
    if (pos < dstLeft)
    {
        // shift the left limit
        dstLeft = o3d::max(pos - (m_charH >> 1), 0);
    }

    // pos is at right outside
    if (pos > rightPos)
    {
        // shift to contains pos as the last character
        dstLeft = pos - width;
        rightPos = dstLeft + width;
    }

    //
    // now update the content
    //

    Int32 charWidth;

    // first, search for the first character
    while ((curPos < dstLeft) && (firstCharIndex < text.length()))
    {
        curPos += m_textureGlyph.charMap[(UInt32)text[firstCharIndex]].width;
        ++firstCharIndex;
    }

    dstLeft = curPos;
    leftChar = firstCharIndex;

    // next, fill enough characters to fill the text zone in width
    Int32 lastCharIndex = firstCharIndex;
    Int32 curWidth = 0;

    while ((curPos < rightPos) && (lastCharIndex < (Int32)text.length()))
    {
        charWidth = m_textureGlyph.charMap[(UInt32)text[lastCharIndex]].width;

        curPos += charWidth;
        curWidth += charWidth;

        dstText.append(text[lastCharIndex]);

        ++lastCharIndex;
    }

    // last char is partially display, does not count into visible
    if (curPos > rightPos)
    {
        --lastCharIndex;
    }

    // free space at right and some characters out of screen at left
    // then shift of some char to right
    if ((curWidth < width) && (dstLeft > 0))
    {
        while ((curWidth < width) && (firstCharIndex > 0))
        {
            --firstCharIndex;

            charWidth = m_textureGlyph.charMap[(UInt32)text[firstCharIndex]].width;

            if (charWidth + curWidth > width)
            {
                // restore previous index because we want an exact value for m_visible
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
    return pos - dstLeft;
}

/*---------------------------------------------------------------------------------------
  is this char is supported by the loaded font
---------------------------------------------------------------------------------------*/
Bool TextureFont::isSupportedChar(UInt32 c)
{
	if (!c)
		return False;

	return (m_textureGlyph.charMap.find(c) != m_textureGlyph.charMap.end());
}

/*---------------------------------------------------------------------------------------
  Normalize all textures coordinates and compute width
---------------------------------------------------------------------------------------*/
void TextureFont::initialize()
{
	if (!m_textureGlyph.texture)
			O3D_ERROR(E_InvalidPrecondition("Texture must be defined"));

	Float fW = 1.f / (Float)m_textureGlyph.texture->getWidth();
	Float fH = 1.f / (Float)m_textureGlyph.texture->getHeight();

	// check for the largest char width
	Float largest = 0.f;
	//Int32 largestChar = -1;

	std::map<UInt32, GlyphChar>::iterator it;

	for (it = m_textureGlyph.charMap.begin(); it != m_textureGlyph.charMap.end(); ++it)
	{
		GlyphChar &glyph = it->second;

		if ((Float)(glyph.eU - glyph.sU) > largest)
		{
			largest = (Float)(glyph.eU - glyph.sU);
			//largestChar = (Int32)largest;
		}
	}

	for (it = m_textureGlyph.charMap.begin(); it != m_textureGlyph.charMap.end(); ++it)
	{
		GlyphChar &glyph = it->second;

		glyph.width = ((Float)(glyph.eU - glyph.sU) / largest);

		glyph.n_sU = fW * glyph.sU;
		glyph.n_eU = fW * glyph.eU;
		glyph.n_sV = fH * glyph.sV;
		glyph.n_eV = fH * glyph.eV;
	}

	// create the cursor
	GlyphChar &cursor = m_textureGlyph.charMap[0];

	cursor.width = 1.f / largest;

	cursor.n_sU = fW * cursor.sU;
	cursor.n_eU = fW * cursor.eU;
	cursor.n_sV = fH * cursor.sV;
	cursor.n_eV = fH * cursor.eV;
}

