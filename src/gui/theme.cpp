/**
 * @file theme.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/gui/precompiled.h"
#include "o3d/gui/theme.h"

#include "o3d/engine/scene/scene.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/core/xmldoc.h"
#include "o3d/core/filemanager.h"
#include "o3d/gui/gui.h"
#include "o3d/engine/matrix.h"
#include "o3d/gui/widgetdrawsimple.h"
#include "o3d/gui/widgetdrawcompass.h"
#include "o3d/gui/texturefont.h"
#include "o3d/gui/fontmanager.h"
#include "o3d/gui/truetypefont.h"
#include "o3d/gui/thememanager.h"
#include "o3d/gui/guitype.h"

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(Theme, GUI_THEME, SceneEntity)

//---------------------------------------------------------------------------------------
// Theme
//---------------------------------------------------------------------------------------

// default constructor.
Theme::Theme(BaseObject *parent) :
	SceneEntity(parent),
    m_texture(nullptr),
	m_defaultFont(this),
	m_defaultFontSize(11),
    m_defaultFontColor(),
    m_defaultHighlightFontColor(0.25f, 0.35f, 1.0f),
	m_iconSet(this),
	m_cursorManager(this)
{
	for (Int32 i = 0; i < NUM_WIDGET_DRAW; i++)
        m_widgetDraw[i] = nullptr;

	createStringToWidgetTypeMap();

    m_defaultFont = ((Gui*)getScene()->getGui())->getThemeManager()->getDefaultFont();
}

// construct and load a theme file
Theme::Theme(BaseObject *parent, const String &filename) :
	SceneEntity(parent),
    m_texture(nullptr),
	m_defaultFont(this),
	m_defaultFontSize(11),
    m_defaultFontColor(),
    m_defaultHighlightFontColor(0.25f, 0.35f, 1.0f),
	m_iconSet(this),
	m_cursorManager(this)
{
	for (Int32 i = 0; i < NUM_WIDGET_DRAW; i++)
        m_widgetDraw[i] = nullptr;

	createStringToWidgetTypeMap();

    m_defaultFont = ((Gui*)getScene()->getGui())->getThemeManager()->getDefaultFont();

	load(filename);
}

// Virtual destructor.
Theme::~Theme()
{
	for (Int32 i = 0; i < NUM_WIDGET_DRAW; i++)
		deletePtr(m_widgetDraw[i]);

	deletePtr(m_texture);
}

Theme::WidgetDraw Theme::string2WidgetDraw(const String &str) const
{
	std::map<String, WidgetDraw>::const_iterator cit = m_widgetNameToType.find(str);
	if (cit == m_widgetNameToType.end())
		O3D_ERROR(E_InvalidParameter("Unsupported widget type " + str));

	return cit->second;
}

// Create the map of string widget name to integer widget type.
void Theme::createStringToWidgetTypeMap()
{
	m_widgetNameToType.insert(std::make_pair(String("TitleBar_False"), TITLE_BAR_FALSE));
	m_widgetNameToType.insert(std::make_pair(String("TitleBar_Normal"), TITLE_BAR_NORMAL));
	m_widgetNameToType.insert(std::make_pair(String("Window_Normal"), WINDOW_NORMAL));
	m_widgetNameToType.insert(std::make_pair(String("ReduceButton_False"), REDUCE_BUTTON_FALSE));
	m_widgetNameToType.insert(std::make_pair(String("ReduceButton_True"), REDUCE_BUTTON_TRUE));
    m_widgetNameToType.insert(std::make_pair(String("ReduceButton_Hover"), REDUCE_BUTTON_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("MaximizeButton_False"), MAXIMIZE_BUTTON_FALSE));
	m_widgetNameToType.insert(std::make_pair(String("MaximizeButton_True"), MAXIMIZE_BUTTON_TRUE));
    m_widgetNameToType.insert(std::make_pair(String("MaximizeButton_Hover"), MAXIMIZE_BUTTON_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("CloseButton_False"), CLOSE_BUTTON_FALSE));
	m_widgetNameToType.insert(std::make_pair(String("CloseButton_True"), CLOSE_BUTTON_TRUE));
    m_widgetNameToType.insert(std::make_pair(String("CloseButton_Hover"), CLOSE_BUTTON_HOVER));

	m_widgetNameToType.insert(std::make_pair(String("Frame_Normal"), FRAME_NORMAL));
    m_widgetNameToType.insert(std::make_pair(String("Frame_Hover"), FRAME_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("Frame_Disabled"), FRAME_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("Pane_Label"), PANE_LABEL));

    m_widgetNameToType.insert(std::make_pair(String("StaticText_False"), STATIC_TEXT_FALSE));
    m_widgetNameToType.insert(std::make_pair(String("StaticText_Hover"), STATIC_TEXT_HOVER));
    m_widgetNameToType.insert(std::make_pair(String("StaticText_Disabled"), STATIC_TEXT_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("Button_True"), BUTTON_TRUE));
	m_widgetNameToType.insert(std::make_pair(String("Button_False"), BUTTON_FALSE));
    m_widgetNameToType.insert(std::make_pair(String("Button_Hover"), BUTTON_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("Button_Disabled"), BUTTON_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("ToolButton_True"), TOOL_BUTTON_TRUE));
	m_widgetNameToType.insert(std::make_pair(String("ToolButton_False"), TOOL_BUTTON_FALSE));
    m_widgetNameToType.insert(std::make_pair(String("ToolButton_Hover"), TOOL_BUTTON_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("ToolButton_Disabled"), TOOL_BUTTON_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("RadioButton_True"), RADIO_BUTTON_TRUE));
	m_widgetNameToType.insert(std::make_pair(String("RadioButton_Normal"), RADIO_BUTTON_NORMAL));
    m_widgetNameToType.insert(std::make_pair(String("RadioButton_Hover"), RADIO_BUTTON_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("RadioButton_Disabled"), RADIO_BUTTON_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("CheckBox_True"), CHECK_BOX_TRUE));
	m_widgetNameToType.insert(std::make_pair(String("CheckBox_Third"), CHECK_BOX_THIRD));
	m_widgetNameToType.insert(std::make_pair(String("CheckBox_Normal"), CHECK_BOX_NORMAL));
    m_widgetNameToType.insert(std::make_pair(String("CheckBox_Hover"), CHECK_BOX_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("CheckBox_Disabled"), CHECK_BOX_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("ListBox_Normal"), LISTBOX_NORMAL));
    m_widgetNameToType.insert(std::make_pair(String("ListBox_Hover"), LISTBOX_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("ListBox_Disabled"), LISTBOX_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("EditBox_Normal"), EDITBOX_NORMAL));
    m_widgetNameToType.insert(std::make_pair(String("EditBox_Hover"), EDITBOX_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("EditBox_Disabled"), EDITBOX_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("ToolBar"), TOOL_BAR));

	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_Border"), SCROLL_BAR_BORDER));

	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonUp_False"), SCROLL_BAR_BUTTON_UP_FALSE));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonUp_True"), SCROLL_BAR_BUTTON_UP_TRUE));
    m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonUp_Hover"), SCROLL_BAR_BUTTON_UP_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonUp_Disabled"), SCROLL_BAR_BUTTON_UP_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonDown_False"), SCROLL_BAR_BUTTON_DOWN_FALSE));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonDown_True"), SCROLL_BAR_BUTTON_DOWN_TRUE));
    m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonDown_Hover"), SCROLL_BAR_BUTTON_DOWN_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonDown_Disabled"), SCROLL_BAR_BUTTON_DOWN_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonLeft_False"), SCROLL_BAR_BUTTON_LEFT_FALSE));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonLeft_True"), SCROLL_BAR_BUTTON_LEFT_TRUE));
    m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonLeft_Hover"), SCROLL_BAR_BUTTON_LEFT_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonLeft_Disabled"), SCROLL_BAR_BUTTON_LEFT_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonRight_False"), SCROLL_BAR_BUTTON_RIGHT_FALSE));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonRight_True"), SCROLL_BAR_BUTTON_RIGHT_TRUE));
    m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonRight_Hover"), SCROLL_BAR_BUTTON_RIGHT_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_ButtonRight_Disabled"), SCROLL_BAR_BUTTON_RIGHT_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_Slider_False"), SCROLL_BAR_SLIDER_FALSE));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_Slider_True"), SCROLL_BAR_SLIDER_TRUE));
    m_widgetNameToType.insert(std::make_pair(String("ScrollBar_Slider_Hover"), SCROLL_BAR_SLIDER_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_Slider_Disabled"), SCROLL_BAR_SLIDER_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_HorizontalEmpty_False"), SCROLL_BAR_HORIZONTAL_EMPTY_FALSE));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_HorizontalEmpty_True"), SCROLL_BAR_HORIZONTAL_EMPTY_TRUE));
    m_widgetNameToType.insert(std::make_pair(String("ScrollBar_HorizontalEmpty_Hover"), SCROLL_BAR_HORIZONTAL_EMPTY_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_HorizontalEmpty_Disabled"), SCROLL_BAR_HORIZONTAL_EMPTY_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_VerticalEmpty_False"), SCROLL_BAR_VERTICAL_EMPTY_FALSE));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_VerticalEmpty_True"), SCROLL_BAR_VERTICAL_EMPTY_TRUE));
    m_widgetNameToType.insert(std::make_pair(String("ScrollBar_VerticalEmpty_Hover"), SCROLL_BAR_VERTICAL_EMPTY_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("ScrollBar_VerticalEmpty_Disabled"), SCROLL_BAR_VERTICAL_EMPTY_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("Tab_Active"), TAB_ACTIVE));
	m_widgetNameToType.insert(std::make_pair(String("Tab_Normal"), TAB_NORMAL));
    m_widgetNameToType.insert(std::make_pair(String("Tab_Hover"), TAB_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("Tab_Disabled"), TAB_DISABLED));
	m_widgetNameToType.insert(std::make_pair(String("Tab_ButtonList_False"), TAB_BUTTON_LIST_FALSE));
	m_widgetNameToType.insert(std::make_pair(String("Tab_ButtonList_True"), TAB_BUTTON_LIST_TRUE));
    m_widgetNameToType.insert(std::make_pair(String("Tab_ButtonList_Hover"), TAB_BUTTON_LIST_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("Tab_ButtonList_Disabled"), TAB_BUTTON_LIST_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("MenuWidget"), MENU_WIDGET));
	//m_widgetNameToType.insert(std::make_pair(String("Menu_Bar"), MENU_BAR));
	m_widgetNameToType.insert(std::make_pair(String("Menu_LabelItem_Normal"), MENU_LABEL_ITEM_NORMAL));
    m_widgetNameToType.insert(std::make_pair(String("Menu_LabelItem_Hover"), MENU_LABEL_ITEM_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("Menu_LabelItem_Disabled"), MENU_LABEL_ITEM_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("DropDown_Normal"), DROPDOWN_NORMAL));
    m_widgetNameToType.insert(std::make_pair(String("DropDown_Hover"), DROPDOWN_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("DropDown_Disabled"), DROPDOWN_DISABLED));

	m_widgetNameToType.insert(std::make_pair(String("ComboBox_Button_Normal"), COMBOBOX_BUTTON_NORMAL));
    m_widgetNameToType.insert(std::make_pair(String("ComboBox_Button_Hover"), COMBOBOX_BUTTON_HOVER));
	m_widgetNameToType.insert(std::make_pair(String("ComboBox_Button_Disabled"), COMBOBOX_BUTTON_DISABLED));

    m_widgetNameToType.insert(std::make_pair(String("ToolTip_Normal"), TOOL_TYPE_NORMAL));
}

ABCFont* Theme::getDefaultFont()
{
	if (m_defaultFont)
		return m_defaultFont.get();
    return ((Gui*)getScene()->getGui())->getThemeManager()->getDefaultFont();
}

const ABCFont* Theme::getDefaultFont() const
{
	if (m_defaultFont)
		return m_defaultFont.get();
    return ((Gui*)getScene()->getGui())->getThemeManager()->getDefaultFont();
}

// Load a theme file (XML format with reference to a texture file)
Bool Theme::load(const String &filename)
{
    if (m_texture && m_texture->isValid())
		O3D_ERROR(E_InvalidOperation("Theme already loaded"));

    String fullfilename = FileManager::instance()->getFullFileName(filename);

    String path, fname;
    FileManager::getFileNameAndPath(fullfilename, fname, path);

    XmlDoc xmlDoc("import_theme");
    xmlDoc.read(fullfilename);

	// Read XML Content
	// Element TextureTheme
	TiXmlElement *node = xmlDoc.getDoc()->FirstChildElement("TextureTheme");
	if (!node)
		O3D_ERROR(E_InvalidFormat("Missing TextureTheme token in " + filename));

	// Element Global
	node = xmlDoc.getDoc()->FirstChildElement()->FirstChildElement("Global");
	if (!node)
		O3D_ERROR(E_InvalidFormat("Missing Global token in " + filename));

	String texturefile;
	xmlDoc.getEltAttStr(node,"texture",texturefile);

	// Load the texture
    if (!loadTexture(path + '/' + texturefile))
		return False;

	String iconSetPath;
	xmlDoc.getEltAttStr(node,"iconSet", iconSetPath);

	// Load the icon set
    if (iconSetPath.isValid() && !m_iconSet.load(path + '/' + iconSetPath))
		return False;

	String cursorSetPath;
	xmlDoc.getEltAttStr(node,"cursorSet", cursorSetPath);

	// Load the cursor set
    if (cursorSetPath.isValid() && !m_cursorManager.load(path + '/' + cursorSetPath))
		return False;

	Int32 tmpInt;

    //
	// Element DefaultFont
    //

	node = xmlDoc.getDoc()->FirstChildElement()->FirstChildElement("DefaultFont");
	if (!node)
		O3D_ERROR(E_InvalidFormat("Missing DefaultFont token in " + filename));

	// type of the default font
	String fontType(node->ToElement()->Attribute("type"));

	// default font name and load it
	String fontName(node->ToElement()->Attribute("name"));
	if (!fontType.isEmpty() && !fontName.isEmpty())
	{
		if (fontType == "TrueType")
            m_defaultFont = ((Gui*)getScene()->getGui())->getFontManager()->addTrueTypeFont(
                path + '/' + fontName);
		else if (fontType == "TextureType")
            m_defaultFont = ((Gui*)getScene()->getGui())->getFontManager()->addTextureFont(
                path + '/' + fontName);
		else
			O3D_ERROR(E_InvalidFormat("Invalid default font type in " + filename));
	}

	String fontColor(node->ToElement()->Attribute("color"));
	if (fontColor.isValid())
	{
		m_defaultFontColor = Color::fromHex(fontColor);
	}

    String highlightFontColor(node->ToElement()->Attribute("highlight"));
    if (highlightFontColor.isValid())
    {
        m_defaultHighlightFontColor = Color::fromHex(highlightFontColor);
    }

	// default font size
	node->ToElement()->Attribute("height",&tmpInt);
	m_defaultFontSize = (UInt32)tmpInt;

    //
	// Element Data
    //

	node = xmlDoc.getDoc()->FirstChildElement()->FirstChildElement("Data");
	if (!node)
		O3D_ERROR(E_InvalidFormat("Missing Data token in " + filename));

	// Iterate through widgets elements
    TiXmlNode *themeNode = nullptr;

	//WidgetSetting wdgSetting;
	Int32 x;
	Int32 y;
	Int32 width;
	Int32 height;
	String textColor;

	WidgetDraw widgDraw;
	String type;
	String inherit;

    while ((themeNode = node->IterateChildren("Widget", themeNode)) != nullptr)
	{
		widgDraw = string2WidgetDraw(themeNode->ToElement()->Attribute("name"));
		type = themeNode->ToElement()->Attribute("type");
		inherit = themeNode->ToElement()->Attribute("inherit");

		// inherited
		if (inherit.isValid())
		{
			WidgetDraw inheritWidgDraw = string2WidgetDraw(inherit);
			WidgetDrawMode *source = m_widgetDraw[inheritWidgDraw];

			if (source == nullptr)
				O3D_ERROR(E_InvalidFormat("Cannot resolve an inherit param in " + filename));

			if (source->getType() == WidgetDrawMode::SIMPLE_MODE)
			{
				WidgetDrawSimple *currDrawMode = new WidgetDrawSimple(*(WidgetDrawSimple*)source);

				// Texture
                currDrawMode->setTexture(m_texture);

				// WidgetDataSet
				TiXmlNode* pElementNode = themeNode->FirstChildElement("Element");
				if (pElementNode != nullptr)
				{
					// override
					if (!pElementNode->ToElement()->Attribute("x",&x))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'x'' in " + filename));

					if (!pElementNode->ToElement()->Attribute("y",&y))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'y' in " + filename));

					if (!pElementNode->ToElement()->Attribute("width",&width))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'width' in " + filename));

					if (!pElementNode->ToElement()->Attribute("height",&height))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'height' in " + filename));
				}

				// default color text
				textColor = themeNode->ToElement()->Attribute("textColor");
				if (textColor.isValid())
					currDrawMode->setDefaultTextColor(Color::fromHex(textColor));

				// Margins
				TiXmlNode *marginNode = themeNode->FirstChildElement("Margin");
				if (marginNode != nullptr)
				{
					if (!marginNode->ToElement()->Attribute("left",&x))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'left' in " + filename));

					if (!marginNode->ToElement()->Attribute("top",&y))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'top' in " + filename));

					if (!marginNode->ToElement()->Attribute("right",&width))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'right' in " + filename));

					if (!marginNode->ToElement()->Attribute("bottom",&height))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'bottom' in " + filename));

					currDrawMode->setMargin(Vector2i(x, y), Vector2i(width, height));
				}

				m_widgetDraw[widgDraw] = currDrawMode;
			}
			else if (source->getType() == WidgetDrawMode::COMPASS_MODE)
			{
				WidgetDrawCompass *currDrawMode = new WidgetDrawCompass(*(WidgetDrawCompass*)source);

				// Texture
				currDrawMode->setTexture(m_texture);

				// default widget size
				if (themeNode->ToElement()->Attribute("defaultWidth",&width))
					currDrawMode->setDefaultSize(width, currDrawMode->getDefaultSize().y());
				if (themeNode->ToElement()->Attribute("defaultHeight",&height))
					currDrawMode->setDefaultSize(currDrawMode->getDefaultSize().x(), height);

				// default color text
				textColor = themeNode->ToElement()->Attribute("textColor");
				if (textColor.isValid())
					currDrawMode->setDefaultTextColor(Color::fromHex(textColor));

				// WidgetDataSets
				TiXmlNode *pElementNode = NULL;
				WidgetDrawCompass::CompassElement elementName;

				while ((pElementNode = themeNode->IterateChildren("Element",pElementNode)) != NULL)
				{
					if (!pElementNode->ToElement()->Attribute("x",&x))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'x'' in " + filename));

					if (!pElementNode->ToElement()->Attribute("y",&y))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'y' in " + filename));

					if (!pElementNode->ToElement()->Attribute("width",&width))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'width' in " + filename));

					if (!pElementNode->ToElement()->Attribute("height",&height))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'height' in " + filename));

					if (!pElementNode->ToElement()->Attribute("name"))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'name' in " + filename));

					elementName = WidgetDrawCompass::string2CompassElement(pElementNode->ToElement()->Attribute("name"));

					currDrawMode->setData(elementName, x, y, width, height);
				}

				// Margins
				TiXmlNode *marginNode = themeNode->FirstChildElement("Margin");
				if (marginNode != nullptr)
				{
					if (!marginNode->ToElement()->Attribute("left",&x))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'left' in " + filename));

					if (!marginNode->ToElement()->Attribute("top",&y))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'top' in " + filename));

					if (!marginNode->ToElement()->Attribute("right",&width))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'right' in " + filename));

					if (!marginNode->ToElement()->Attribute("bottom",&height))
						O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'bottom' in " + filename));

					currDrawMode->setMargin(Vector2i(x, y), Vector2i(width, height));
				}

				m_widgetDraw[widgDraw] = currDrawMode;
			}
		}
		else if (type == "Simple")
		{
            WidgetDrawSimple* currDrawMode = new WidgetDrawSimple(getScene()->getContext(), this);

			// Texture
            currDrawMode->setTexture(m_texture);

			// WidgetDataSet
			TiXmlNode* pElementNode = themeNode->FirstChildElement("Element");
			if (pElementNode != nullptr)
			{
				if (!pElementNode->ToElement()->Attribute("x",&x))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'x'' in " + filename));

				if (!pElementNode->ToElement()->Attribute("y",&y))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'y' in " + filename));

				if (!pElementNode->ToElement()->Attribute("width",&width))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'width' in " + filename));

				if (!pElementNode->ToElement()->Attribute("height",&height))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'height' in " + filename));
			}
			else
				O3D_ERROR(E_InvalidFormat("Missing <Element> in " + filename));

			currDrawMode->setData(x, y, width, height);
			currDrawMode->setDefaultSize(width, height);

			// default color text
			textColor = themeNode->ToElement()->Attribute("textColor");
			if (textColor.isValid())
				currDrawMode->setDefaultTextColor(Color::fromHex(textColor));
			else
				currDrawMode->setDefaultTextColor(m_defaultFontColor);

			// Margins
			TiXmlNode *marginNode = themeNode->FirstChildElement("Margin");
			if (marginNode != nullptr)
			{
				if (!marginNode->ToElement()->Attribute("left",&x))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'left' in " + filename));

				if (!marginNode->ToElement()->Attribute("top",&y))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'top' in " + filename));

				if (!marginNode->ToElement()->Attribute("right",&width))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'right' in " + filename));

				if (!marginNode->ToElement()->Attribute("bottom",&height))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'bottom' in " + filename));

				currDrawMode->setMargin(Vector2i(x, y), Vector2i(width, height));
			}
			else
			{
				// default are 0
				currDrawMode->setMargin(Vector2i(0, 0), Vector2i(0, 0));
			}

			m_widgetDraw[widgDraw] = currDrawMode;
		}
		else if (type == "Compass")
		{
            WidgetDrawCompass* currDrawMode = new WidgetDrawCompass(getScene()->getContext(), this);

			// default widget size
			themeNode->ToElement()->Attribute("defaultWidth",&width);
			themeNode->ToElement()->Attribute("defaultHeight",&height);
			currDrawMode->setDefaultSize(width, height);

			// Texture
			currDrawMode->setTexture(m_texture);

			// default color text
			textColor = themeNode->ToElement()->Attribute("textColor");
			if (textColor.isValid())
				currDrawMode->setDefaultTextColor(Color::fromHex(textColor));
			else
				currDrawMode->setDefaultTextColor(m_defaultFontColor);

			// WidgetDataSets
            TiXmlNode *pElementNode = nullptr;
			WidgetDrawCompass::CompassElement elementName;

            while ((pElementNode = themeNode->IterateChildren("Element",pElementNode)) != nullptr)
			{
				if (!pElementNode->ToElement()->Attribute("x",&x))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'x'' in " + filename));

				if (!pElementNode->ToElement()->Attribute("y",&y))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'y' in " + filename));

				if (!pElementNode->ToElement()->Attribute("width",&width))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'width' in " + filename));

				if (!pElementNode->ToElement()->Attribute("height",&height))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'height' in " + filename));

				if (!pElementNode->ToElement()->Attribute("name"))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'name' in " + filename));

				elementName = WidgetDrawCompass::string2CompassElement(pElementNode->ToElement()->Attribute("name"));

				currDrawMode->setData(elementName, x, y, width, height);
			}

			// Margins
			TiXmlNode *marginNode = themeNode->FirstChildElement("Margin");
			if (marginNode != nullptr)
			{
				if (!marginNode->ToElement()->Attribute("left",&x))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'left' in " + filename));

				if (!marginNode->ToElement()->Attribute("top",&y))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'top' in " + filename));

				if (!marginNode->ToElement()->Attribute("right",&width))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'right' in " + filename));

				if (!marginNode->ToElement()->Attribute("bottom",&height))
					O3D_ERROR(E_InvalidFormat("Missing <Element> attribute 'bottom' in " + filename));

				currDrawMode->setMargin(Vector2i(x, y), Vector2i(width, height));
			}
			else
			{
				// defaults are NE and SW elements size
				currDrawMode->setMargin(
							Vector2i(currDrawMode->getEltWidth(WidgetDrawCompass::NORTH_WEST),
									 currDrawMode->getEltHeight(WidgetDrawCompass::NORTH_WEST)),
							Vector2i(currDrawMode->getEltWidth(WidgetDrawCompass::NORTH_WEST),
									 currDrawMode->getEltHeight(WidgetDrawCompass::NORTH_WEST)));
			}

			m_widgetDraw[widgDraw] = currDrawMode;
		}
		else
		{
			O3D_ERROR(E_InvalidFormat("Unknown widget type in " + filename));
		}
	}

	return True;
}

// Set the texture
Bool Theme::loadTexture(const String &filename)
{
	Image pic;

	if (!pic.load(filename))
		return False;

	m_texture = new Texture2D(this, pic);
	m_texture->setPersistant(True);

	if (!m_texture->create(False))
	{
        m_texture = nullptr;
		return False;
	}

	return True;
}

// Drawing method
void Theme::drawWidget(WidgetDraw widget, Int32 x, Int32 y, Int32 width, Int32 height)
{
	O3D_ASSERT(widget < NUM_WIDGET_DRAW);

	if ((widget < NUM_WIDGET_DRAW) && m_widgetDraw[widget])
		m_widgetDraw[widget]->draw(x, y, width, height);
}

// Return the widget draw mode
const WidgetDrawMode* Theme::getWidgetDrawMode(WidgetDraw widget) const
{
	O3D_ASSERT(widget < NUM_WIDGET_DRAW);

	if (widget < NUM_WIDGET_DRAW)
		return m_widgetDraw[widget];
	else
		return NULL;
}

//Return the default widget size (defined by the theme)
Vector2i Theme::getDefaultWidgetSize(WidgetDraw widget) const
{
	O3D_ASSERT(widget < NUM_WIDGET_DRAW);

	if (widget < NUM_WIDGET_DRAW)
		return m_widgetDraw[widget]->getDefaultSize();
	else
		return Vector2i(0,0);
}

