/**
 * @file gui.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_GUI_H
#define _O3D_GUI_H

#include "o3d/engine/scene/sceneentity.h"
#include "o3d/core/callback.h"
#include "o3d/core/evt.h"
#include "o3d/core/keyboard.h"
#include "o3d/core/mouse.h"
#include "o3d/engine/vertexbuffer.h"
#include "o3d/engine/guimanager.h"
#include "o3d/image/imagetype.h"
#include "guitype.h"

#include <list>

#include "o3d/core/memorydbg.h"

namespace o3d {

class FontManager;
class Renderer;
class ViewPort;
class Camera;
class Texture2D;
class Image;
class WidgetManager;
class ThemeManager;
class Theme;
class ABCFont;
class FrameBuffer;
class I18n;

typedef std::list<FrameBuffer*> T_FBOPooler;
typedef T_FBOPooler::iterator IT_FBOPooler;

/**
 * @brief Main manager for all GUI objects.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-05-10
 * Main manager for all GUI objects. It can load a full customizable theme
 * for widget, manage fonts... The WinManager is mainly owned by an Scene.
 *
 * The textures aren't managed, they are local to their owner (not shared).
 *
 * A special widget Canvas, can be used when you need to insert some 3d content
 * in a window. With this special widget you set him an independent
 * ScreenViewPort whose will renderer your 3d content. So this viewport doesn't be
 * added to the Scene::ViewPortManager.
 */
class O3D_API Gui :
        public SceneEntity,
        public GuiManager,
        NonCopyable<>
{
public:

	O3D_DECLARE_DYNAMIC_CLASS(Gui)

	//-------------------------------------------------------------------------------
	// Setup
	//-------------------------------------------------------------------------------

	//! Default constructor.
	Gui(BaseObject *parent);

	//! Virtual destructor.
	virtual ~Gui();

    virtual void init();

    virtual void release();

    //
    // Events
    //

    /**
     * @brief defaultAttachment Use default connections and attach them to an AppWindow.
     * If you use your own window system you have to connect the scene (and the GUI)
     * to events :
     *  - Gui::postCharacterEvent
     *  - Gui::postMouseMotionEvent
     *  - Gui::postMouseButtonEvent
     *  - Gui::postMouseWheelEvent.
     * @param appWindow
     */
    void defaultAttachment(AppWindow *appWindow);

	//! Call this method to post a key event (the Scene can call it).
    void postKeyEvent(Keyboard* keyboard, KeyEvent event);

	//! Call this method to post a character event (the Scene can call it).
    void postCharacterEvent(Keyboard* keyboard, CharacterEvent event);

	//! Call this method to post a mouse motion event (the Scene can call it).
    void postMouseMotionEvent(Mouse* mouse);

	//! Call this method to post a mouse button event (the Scene can call it).
    void postMouseButtonEvent(Mouse* mouse, ButtonEvent event);

	//! Call this method to post a mouse wheel event (the Scene can call it).
    void postMouseWheelEvent(Mouse* mouse);

	//-------------------------------------------------------------------------------
	// Default drawing method for render a GUI, this is the default method used by
	// viewport of Gui
	//-------------------------------------------------------------------------------

    /**
     * @brief draw This is a default GUI rendering method. By default GUI add a
     * specific viewport to the scene viewport manager, then this method is not used.
     */
    void draw();

    /**
     * @brief update Update of the widget manager, theme manager, and font manager.
     */
    virtual void update();

	//-------------------------------------------------------------------------------
	// Viewport settings
	//-------------------------------------------------------------------------------

    //! Return the gui viewport.
    virtual ViewPort* getViewPort();

    //! Return the gui viewport.
    virtual const ViewPort* getViewPort() const;

	//! Change the winmanager viewport priority (default is O3D_MAX_INT32)
    void changeViewPortPriority(Int32 newPriority = Limits<Int32>::max());

	//! reset the winmanager viewport to its default parameters
	void resetViewPort();

	//! set the screen resolution for the GUI and font
    virtual void reshape(UInt32 width, UInt32 height);

	//! get the screen width resolution for the GUI and font
	Int32 getWidth() const;

	//! get the screen height resolution for the GUI and font
	Int32 getHeight() const;

	//-------------------------------------------------------------------------------
	// Manager accessor
	//-------------------------------------------------------------------------------

	inline I18n* getI18n() { return m_i18n; }
	inline const I18n* getI18n() const { return m_i18n; }

	inline FontManager* getFontManager() { return m_fontManager; }
	inline const FontManager* getFontManager() const { return m_fontManager; }

	inline WidgetManager* getWidgetManager() { return m_widgetManager; }
	inline const WidgetManager* getWidgetManager() const { return m_widgetManager; }

	inline ThemeManager* getThemeManager() { return m_themeManager; }
	inline const ThemeManager* getThemeManager() const { return m_themeManager; }

	//! return the access to the FBO pool
	T_FBOPooler& getFBOPooler();

	//! return an FBO for the given size and pixel format. Create a new one if none existing
	FrameBuffer* findFBO(
		UInt32 width,
		UInt32 height,
		PixelFormat pixelFormat);

	//-------------------------------------------------------------------------------
	// Translation
	//-------------------------------------------------------------------------------

	String tr(const String &key) const;

	String tr(const String &key, const String &_1) const;

	String tr(const String &key, const String &_1, const String &_2) const;

	String tr(const String &key, const String &_1, const String &_2, const String &_3) const;

	//-------------------------------------------------------------------------------
    //! Import/export
	//-------------------------------------------------------------------------------

    //! Import an o3dui GUI file and return imported element.
    //! @param filename The .o3dui GUI file to import.
    Bool importGui(const String &filename);

    //! Import an o3dui GUI file and return imported element.
    //! @param is An opened input stream
    Bool importGui(InStream &is);

private:

	//-------------------------------------------------------------------------------
	// Managers
	//-------------------------------------------------------------------------------

	I18n*          m_i18n;           //!< I18n manager.
	FontManager*   m_fontManager;    //!< Font manager.
	Camera*        m_camera;         //!< Independent single camera.
	ViewPort*      m_viewPort;       //!< The current output viewport of the winmanager.
	WidgetManager* m_widgetManager;  //!< Widgets manager.
	ThemeManager*  m_themeManager;   //!< List of all available themes.
	T_FBOPooler*   m_FBOPooler;      //!< List of all available FBO.

    BitSet8 m_oldMouseButton;
	Vector2i m_oldMousePos;
};

} // namespace o3d

#endif // _O3D_GUI_H
