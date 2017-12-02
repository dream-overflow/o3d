/**
 * @file fontmanager.h
 * @brief Font manager.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-02-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FONTMANAGER_H
#define _O3D_FONTMANAGER_H

#include "abcfont.h"
#include "o3d/engine/scene/scenetemplatemanager.h"

namespace o3d {

class TextureFont;
class TrueTypeFont;

/**
 * @brief Font manager instancied by Gui.
 */
class O3D_API FontManager : public SceneTemplateManager<ABCFont>
{
public:

	O3D_DECLARE_CLASS(FontManager)

	//! Default constructor.
	FontManager(BaseObject *parent);

	//! Virtual destructor.
	virtual ~FontManager();

	//! Useful updates.
	void update();

	//-----------------------------------------------------------------------------------
	// Factory
	//-----------------------------------------------------------------------------------

	//! Delete an existing font.
	//! @return True mean the object was found and successfully deleted.
	Bool deleteFont(ABCFont *font);

	//! Create a texture based font.
	TextureFont* createTextureFont();

	//! Add a texture based font from a definition file.
	TextureFont* addTextureFont(
		const String& fontName,
		ABCFont::CharSet charSet = ABCFont::CHARSET_LATIN1,
		ABCFont::FontStyle style = ABCFont::STYLE_NORMAL);

	//! Create a true type based font.
	TrueTypeFont* createTrueTypeFont();

	//! Add a true type based font from parameters.
	TrueTypeFont* addTrueTypeFont(
		const String& fontName,
		ABCFont::CharSet charSet = ABCFont::CHARSET_LATIN1,
		ABCFont::FontStyle style = ABCFont::STYLE_NORMAL);
};

} // namespace o3d

#endif // _O3DFONTMANAGER_H
