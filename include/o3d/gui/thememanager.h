/**
 * @file thememanager.h
 * @brief Window theme manager.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-02-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_THEMEMANAGER_H
#define _O3D_THEMEMANAGER_H

#include "theme.h"
#include "o3d/engine/scene/scenetemplatemanager.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class ThemeManager
//---------------------------------------------------------------------------------------
//! The theme manager implemented in Gui
//! TODO like the SceneObjectManager
//---------------------------------------------------------------------------------------
class O3D_API ThemeManager : public SceneTemplateManager<Theme>
{
public:

	O3D_DECLARE_CLASS(ThemeManager)

	//! Default constructor
	ThemeManager(BaseObject *parent);

	//! Useful updates
	void update();

	//-----------------------------------------------------------------------------------
	// Factory
	//-----------------------------------------------------------------------------------

	//! Delete an existing theme
	//! @return True mean the object was found and successfully deleted
	Bool deleteTheme(Theme *theme);

	//! Create a new theme
	Theme* createTheme();

	//! Add a theme by its definition contained in a file (XML file)
	Theme* addTheme(const String &filename);

	//-----------------------------------------------------------------------------------
	// Default font parameters
	//-----------------------------------------------------------------------------------

	//! return the sub-system default font
	inline void setDefaultFont(ABCFont *defaultFont)
	{
		m_defaultFont = defaultFont;
		O3D_ASSERT(m_defaultFont);
	}

	//! return the sub-system default font.
	inline ABCFont* getDefaultFont() { return m_defaultFont.get(); }
	//! return the sub-system default font (const version).
	inline const ABCFont* getDefaultFont() const { return m_defaultFont.get(); }

protected:

	SmartObject<ABCFont> m_defaultFont;        //!< global default theme font
};

} // namespace o3d

#endif // _O3DTHEMEMANAGER_H

