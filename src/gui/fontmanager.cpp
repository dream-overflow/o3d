/**
 * @file fontmanager.cpp
 * @brief Implementation of FontManager.h
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-02-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/fontmanager.h"
#include "o3d/gui/texturefont.h"
#include "o3d/gui/truetypefont.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(FontManager, GUI_FONT_MANAGER, SceneTemplateManager<ABCFont>)

// Default constructor.
FontManager::FontManager(BaseObject *parent) :
	SceneTemplateManager<ABCFont>(parent)
{
	TrueTypeFont::initializeFreeType();
}

// Virtual destructor.
FontManager::~FontManager()
{
	TemplateManager<ABCFont>::destroy();
	TrueTypeFont::deleteFreeType();
}

// Useful updates.
void FontManager::update()
{
	processDeferred();
}

//---------------------------------------------------------------------------------------
// Factory
//---------------------------------------------------------------------------------------

// Delete an existing font
Bool FontManager::deleteFont(ABCFont *font)
{
	if (font && font->getParent() == this)
		return deleteElementPtr(font);
	else
		return False;
}

// Create a texture based font
TextureFont* FontManager::createTextureFont()
{
	TextureFont* pFont = new TextureFont(this);

	addElement(pFont);
	return pFont;
}

// Add a texture based font
TextureFont* FontManager::addTextureFont(
	const String& fontName,
	ABCFont::CharSet charSet,
	ABCFont::FontStyle style)
{
	TextureFont* font = new TextureFont(this);
	font->load(fontName, charSet, style);

	addElement(font);
	return font;
}

// Create a true type based font
TrueTypeFont* FontManager::createTrueTypeFont()
{
	TrueTypeFont* font = new TrueTypeFont(this);

	addElement(font);
	return font;
}

// Add a true type based font
TrueTypeFont* FontManager::addTrueTypeFont(
	const String& fontName,
	ABCFont::CharSet charSet,
	ABCFont::FontStyle style)
{
	TrueTypeFont* font = new TrueTypeFont(this);
	font->load(fontName, charSet, style);

	addElement(font);
	return font;
}

