/**
 * @file thememanager.cpp
 * @brief Implementation of ThemeManager.h
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2010-02-11
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/thememanager.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(ThemeManager, GUI_THEME_MANAGER, SceneTemplateManager<Theme>)

// Default constructor
ThemeManager::ThemeManager(BaseObject *parent) :
	SceneTemplateManager<Theme>(parent),
	m_defaultFont(this)
{
}

// Process the remove of deleted theme
void ThemeManager::update()
{
	processDeferred();
}

//---------------------------------------------------------------------------------------
// Factory
//---------------------------------------------------------------------------------------

// Delete an existing theme
Bool ThemeManager::deleteTheme(Theme *theme)
{
	if (theme && theme->getParent() == this)
		return deleteElementPtr(theme);
	else
		return False;
}

// Create a new theme
Theme* ThemeManager::createTheme()
{
	Theme *theme = new Theme(this);
	addElement(theme);
	return theme;
}

// Add a theme by its definition contained in a file (XML file)
Theme* ThemeManager::addTheme(const String &filename)
{
	Theme *theme = new Theme(this, filename);
	addElement(theme);
	return theme;
}

