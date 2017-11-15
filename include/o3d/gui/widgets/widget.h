/**
 * @file widget.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WIDGET_H
#define _O3D_WIDGET_H

#include "o3d/core/file.h"
#include "o3d/core/keyboard.h"
#include "o3d/engine/blending.h"
#include "widgetproperties.h"
#include "../theme.h"

namespace o3d {

class WidgetManager;
class Layout;
class LayoutItem;
class Gui;

/**
 * @brief A widget base class for all others GUI element
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-05-17
 * A widget base class for all others GUI element such as, static text, button, window
 * and more. Event are generated by signals (see 'public signal:' above). Each widget
 * can have more specifics signals.
 */
class O3D_API Widget : public WidgetProperties
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(Widget)

	static const Vector2i DEFAULT_POS;   //!< default position (-1,-1)
	static const Vector2i DEFAULT_SIZE;  //!< default size (-1,-1)

	//! Size/resize/position mode.
	enum SizeMode
	{
		SIZE_USE_EXISTING = 0,    //!< Use the existing value when -1 if found
		SIZE_FORCE = 1,           //!< Force to size even if the value are the same
		SIZE_ALLOW_MINUS_ONE = 2, //!< Allow the use of -1 value
		SIZE_NO_ADJUSTEMENT = 4,  //!< No adjustment of the position
		SIZE_AUTO_WIDTH = 8,      //!< Auto size on width
		SIZE_AUTO_HEIGHT = 16     //!< Auto size on height
	};

	//! Default initialization constructor
	Widget(
		Widget *parent,
        const Vector2i &pos = DEFAULT_POS,
        const Vector2i &size = DEFAULT_SIZE,
		const String &name = "");

	//! Default initialization constructor
	Widget(
		WidgetManager *parent,
        const Vector2i &pos = DEFAULT_POS,
        const Vector2i &size = DEFAULT_SIZE,
		const String &name = "");

	//! Virtual destructor.
	virtual ~Widget();

    //! Get the Gui.
    Gui* getGui();
    //! Get the Gui (const version.
    const Gui* getGui() const;

	//-----------------------------------------------------------------------------------
	// Hierarchy management
	//-----------------------------------------------------------------------------------

	//! return the top level parent window (ie the window/dialog/frame that contain him, or himself) (const version)
	const Widget* getTopLevelWidget() const;

	//! return the top level parent widget (ie the window/dialog/frame that contain him, or himself)
	Widget* getTopLevelWidget();

	//! return true if this widget is a top level widget
	inline Bool isTopLevelWidget() const { return getTopLevelWidget() == this; }

	//! check if the widget has a parent
    Bool hasParent() const { return (m_parent != nullptr); }

	//! retrieve the parent widget or WidgetManager has parent for TopLevelWidget
	Widget* getParentWidget();

	//! retrieve the parent widget or WidgetManager has parent for TopLevelWidget (const version)
	const Widget* getParentWidget() const;

	//! set the focus to the widget
	void setFocus();


	//-----------------------------------------------------------------------------------
	// Layout
	//-----------------------------------------------------------------------------------

	//! get is the widget is a layout type or not
	inline Bool isLayoutObject() const { return m_capacities.getBit(CAPS_LAYOUT_TYPE); }

	//! get the parent layout. By default an HorizontalLayout is setup. (const version)
	virtual const Layout* getLayout() const;
	//! get the parent layout. By default an HorizonsalLayout is setup.
	virtual Layout* getLayout();

	//! process the layout calculation
	virtual void layout();

	//! Fit the window to the size of the content of the layout
	virtual Vector2i fit();

	//! Similar to Fit, but sizes the interior (virtual) size of a window
	virtual void fitInside();

	//! Set size hints
	virtual void setSizeHints(
		Int32 minW,
		Int32 minH,
		Int32 maxW = -1,
		Int32 maxH = -1,
		Int32 incW = -1,
		Int32 incH = -1);

	//! Set size hints
	void setSizeHints(const Vector2i &min, const Vector2i &max, const Vector2i &inc)
	{
        setSizeHints(min.x(), min.y(), max.x(), max.y(), inc.x(), inc.y());
	}

	//! Set virtual size hints
	virtual void setVirtualSizeHints(
		Int32 minW,
		Int32 minH,
		Int32 maxW = -1,
		Int32 maxH = -1);

	//! Set virtual size hints
	inline void setVirtualSizeHints(const Vector2i &min, const Vector2i &max)
	{
        setVirtualSizeHints(min.x(), min.y(), max.x(), max.y());
	}

	//! Compute the best size for the window depending of its content.
	//! The size which give the best look.
	Vector2i getBestSize();

	//! Compute the best virtual size
	Vector2i getBestVirtualSize();

	//! Called from GetBestSize() to convert best virtual size (returned by the window
	//! sizer) to the best size for the window itself. It is overridden at
	//! O3DScrollPane level to convert scroll size to real size.
	virtual Vector2i getWindowSizeForVirtualSize(const Vector2i &size) const
	{
		return size;
	}

	//! Merge the widget's best size into the widget's minimum size, giving priority
	//! to the min size components, and returns the results.
	Vector2i getEffectiveMinSize();


	//-----------------------------------------------------------------------------------
	// Position property
	//-----------------------------------------------------------------------------------

	//! set the relative position (relative to parent origin)
    void setPos(const Vector2i& pos);
    //! set the relative position (relative to parent origin)
    void setPos(Int32 x, Int32 y);

	//! get the relative position (relative to parent origin)
    inline const Vector2i& pos() const { return m_pos; }

	//! translate the relative position (relative to parent origin)
    void translate(const Vector2i &pos);
    void translate(Int32 x, Int32 y);

	//! return the absolute position (depend of its parent)
	Vector2i getAbsPos() const;

	//! return the top-left relative origin (client area) into the widget (for its children)
	virtual Vector2i getOrigin() const;

	//! return the top-left absolute position of the widget, containing the local origin (client area)
	virtual Vector2i getAbsPosOrigin() const;


	//-----------------------------------------------------------------------------------
	// Size property
	//-----------------------------------------------------------------------------------

	//! set the widget size
    inline void setSize(const Vector2i& size) { setSize(size.x(), size.y()); }

	//! set the widget size
    void setSize(Int32 width, Int32 height);

	//! set the widget size width
    inline void setWidth(Int32 width) { setSize(width, -1); }

	//! set the widget size height
    inline void setHeight(Int32 height) { setSize(-1, height); }

	//! get the widget size
    inline const Vector2i& size() const { return m_size; }

	//! get the widget client size. the client area is the area which may be drawn on by
	//! the programmer (excluding title bar and borders)
	virtual Vector2i getClientSize() const;

	//! get the recommended widget default size
	virtual Vector2i getDefaultSize() = 0;

	//! set the position and size
    void setSize(Int32 x, Int32 y, Int32 width, Int32 height, Int32 mode = 0);

	//! get the border size of the window depending of its defined theme
    virtual Vector2i getWindowBorderSize() const { return Vector2i(0, 0); }


	//-----------------------------------------------------------------------------------
	// Minimal Size property
	//-----------------------------------------------------------------------------------

	//! set the minimal widget size
    inline void setMinSize(const Vector2i& size)
	{
		m_minSize = size;
		sizeChanged();
	}

	//! set the minimal widget size
    inline void setMinSize(Int32 width, Int32 height)
	{
		m_minSize.set(width,height);
		sizeChanged();
	}

	//! set the minimal widget size width
    inline void setMinWidth(Int32 width)
	{
        m_minSize.x() = width;
		sizeChanged();
	}

	//! set the minimal widget size height
    inline void sMinHeight(Int32 height)
	{
        m_minSize.y() = height;
		sizeChanged();
	}

	//! get the minimal widget size
    virtual Vector2i getMinSize() { return m_minSize; }


	//-----------------------------------------------------------------------------------
	// Virtual size property
	//-----------------------------------------------------------------------------------

	//! set the virtual widget size
    inline void setVirtualSize(const Vector2i& size) { virtualSizeChanged(size.x(), size.y()); }

	//! set the virtual widget size
    inline void setVirtualSize(Int32 width, Int32 height) { virtualSizeChanged(width, height); }

	//! get the virtual widget size
	virtual Vector2i getVirtualSize() const;


	//-----------------------------------------------------------------------------------
	// Maximal Size property
	//-----------------------------------------------------------------------------------

	//! set the maximal widget size
    inline void setMaxSize(const Vector2i& size)
	{
		m_maxSize = size;
		sizeChanged();
	}

	//! set the maximal widget size
    inline void setMaxSize(Int32 width, Int32 height)
	{
		m_maxSize.set(width,height);
		sizeChanged();
	}

	//! set the maximal widget size width
    inline void setMaxWidth(Int32 width)
	{
        m_maxSize.x() = width;
		sizeChanged();
	}

	//! set the maximal widget size height
    inline void setMaxHeight(Int32 height)
	{
        m_maxSize.y() = height;
		sizeChanged();
	}

	//! get the maximal widget size
    inline const Vector2i& maxSize() const { return m_maxSize; }

	//! get the maximal widget width
    inline Int32 maxWidth() const { return m_maxSize.x(); }

	//! get the maximal widget height
    inline Int32 maxHeight() const { return m_maxSize.y(); }


	//-----------------------------------------------------------------------------------
	// Theme properties
	//-----------------------------------------------------------------------------------

	//! set the theme
	inline void setTheme(Theme* pTheme)
	{
		m_theme = pTheme;
		setDirty();
	}

	//! get the theme (const version)
	inline const Theme* getTheme() const { return m_theme; }

	//! get the theme
	inline Theme* getTheme() { return m_theme; }

	//! get the theme (const version). return the theme of the widget or the valid theme of one of its parent.
	const Theme* getUsedTheme() const;

	//! get the used theme. return the theme of the widget or the valid theme of one of its parent.
	Theme* getUsedTheme();


	//-----------------------------------------------------------------------------------
	// View properties
	//-----------------------------------------------------------------------------------

	//! Set the widget always on top. Does nothing.
	virtual void setAlwaysOnTop(Bool onTop = True);

	//! Is the widget always on top.
	//! @return FALSE.
	virtual Bool isAlwaysOnTop() const;

	//! Set the widget as modal. Does nothing.
	virtual void setModal(Bool modal = True);

	//! Is the widget is modal.
	//! @return FALSE.
	virtual Bool isModal() const;

	//! Get the current cursor type name for this widget if targeted.
	//! @return "Default".
	virtual String getCursorTypeName() const;

	//! Get the text translation for a specific key.
	String tr(const String &key) const;

	//! Get the text translation for a specific key with one parameters.
	String tr(const String &key, const String &_1) const;

    //! Get the text translation for a specific key with one parameters.
    String tr(const String &key, Int32 _1) const;

	//! Get the text translation for a specific key with 2 parameters.
	String tr(const String &key, const String &_1, const String &_2) const;

	//! Get the text translation for a specific key with 3 parameters.
	String tr(const String &key, const String &_1, const String &_2, const String &_3) const;


	//-----------------------------------------------------------------------------------
	// Focusable
	//-----------------------------------------------------------------------------------

    //! Return true if the widget have the focus.
	inline Bool isFocused() const { return m_capacities.getBit(STATE_FOCUSED); }

	//! Returns true if the widget is focusable (default true).
	virtual Bool isFocusable() const;

    //! Returns the widget tab index.
    inline Int32 getTabIndex() const { return m_tabIndex; }
    //! Set the widget tab index (relative to its top level parent widget).
    void setTabIndex(Int32 index);

    //! Is the widget can contains children widgets
    inline Bool isContainer() const { return m_capacities[CAPS_CONTAINER]; }

    //! Get the widget corresponding to next tab index or nullptr if none.
    //! @param widget Start searching from this widget. Can be null.
    //! @param direction -1 or 1 meaning the way to search the next widget.
    //!     0 returns itself.
    //! @note If widget is null depending of direction, it return the first or the last
    //! widget.
    virtual Widget* findNextTabIndex(Widget *widget, Int32 direction);

    //! Get the previously focused widget.
    //! @note Only for top-level widgets.
    virtual Widget* getPreviouslyFocusedWidget();
    //! Set the previously focused widget.
    //! @note Only for top-level widgets.
    virtual void setPreviouslyFocusedWidget(Widget *widget);


	//-----------------------------------------------------------------------------------
	// Scrollable
	//-----------------------------------------------------------------------------------

    //! True if the widged have the scrollable capacity (virtual size, scroll bar...)
	inline Bool isScrollable() const { return m_capacities.getBit(CAPS_SCROLLABLE); }

	//! Get the offset of the scrolling.
	virtual Vector2i getScrollPos() const;


    //-----------------------------------------------------------------------------------
    // ToolTip
    //-----------------------------------------------------------------------------------

    //! Define a tool tip. Default does nothing.
    virtual void setToolTip(Widget *toolTip);

    //! Get the widget tool tip or nullptr if none. (default returns nullptr).
    virtual Widget* getToolTip();

     /**
      * @brief setToolTip Helper that create a tooltip with a simple static text in
      * a single line. It use of the virtual setToolTip method.
      * @param label
      */
    virtual void setToolTip(const String &label);


    //-----------------------------------------------------------------------------------
    // Clipboard and Drag'n'drop
    //-----------------------------------------------------------------------------------

    //! True if the widget have the copy capacity (can send content to the clipboard).
    inline Bool isCopyable() const { return m_capacities.getBit(CAPS_COPYABLE); }
    //! True if the widget have the paste capacity (can accept content from clipboard).
    inline Bool isPastable() const { return m_capacities.getBit(CAPS_PASTABLE); }

    //! Cut the current selection and send it to the system clipboard.
    //! @param primary To the primary clipboard if True and available.
    //! @return True if the cut success (has content, content sent).
    virtual Bool cut(Bool primary=False);
    //! Copy the current selection and send it to the system clipboard.
    //! @param primary To the primary clipboard if True and available.
    //! @return True if the paste success (has content, content sent).
    virtual Bool copy(Bool primary=False);
    //! Replace the current selection by the content of the system clipboard.
    //! @param primary Use the primary clipboard if True and available.
    //! @return True if the copy success (has content, compatible content type).
    virtual Bool paste(Bool primary=False);

    //! True if the widget have the drag capacity (can drag its content).
    inline Bool isDragable() const { return m_capacities.getBit(CAPS_DRAGABLE); }
    //! True if the widget have the drop capacity (can accept content from current drag).
    inline Bool isDropable() const { return m_capacities.getBit(CAPS_DROPABLE); }

	//-----------------------------------------------------------------------------------
	// Events and updates
	//-----------------------------------------------------------------------------------

	//! check if the widget is targeted and return true and non null widget if a target is found
	virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget);

	// Mouse events. coordinates are relative to parent origin
	virtual Bool mouseLeftPressed(Int32 x, Int32 y);
	virtual Bool mouseLeftReleased(Int32 x, Int32 y);
    virtual Bool mouseMiddlePressed(Int32 x, Int32 y);
    virtual Bool mouseMiddleReleased(Int32 x, Int32 y);
	virtual Bool mouseRightPressed(Int32 x, Int32 y);
	virtual Bool mouseRightReleased(Int32 x, Int32 y);
    virtual Bool mouseX1Pressed(Int32 x, Int32 y);
    virtual Bool mouseX1Released(Int32 x, Int32 y);
    virtual Bool mouseX2Pressed(Int32 x, Int32 y);
    virtual Bool mouseX2Released(Int32 x, Int32 y);

	virtual Bool mouseWheel(Int32 x, Int32 y, Int32 z);

	virtual Bool mouseMove(Int32 x, Int32 y);
	virtual void mouseMoveIn();
	virtual void mouseMoveOut();

    virtual Bool mouseDrag(Int32 x, Int32 y, Widget *&draged);
    virtual Bool mouseDrop(Int32 x, Int32 y, Widget *droped);

	/**
	 * @brief keyboardToggled Key event
	 * @param keyboard keyboard controller
	 * @param event keyboard event
	 * @return true if the widget consume the key, false to give it to its parent.
	 */
	virtual Bool keyboardToggled(Keyboard *keyboard, KeyEvent event);
	//! Character event.
	virtual Bool character(Keyboard *keyboard, CharacterEvent event);

    // Others Event

	virtual void focused();
	virtual void lostFocus();
	virtual void positionChanged();
	virtual void sizeChanged();
	virtual void valueChanged();
	virtual void virtualSizeChanged(Int32 x, Int32 y);

    //! Each time the theme change and according to the type of the widget this method
    //! is called. The widget has the charge to update its computed state as necessary.
    virtual void themeChanged();


	//-----------------------------------------------------------------------------------
	// Processing
	//-----------------------------------------------------------------------------------

    //! Set the alpha blending function used to draw the widget.
    void setBlendFunc(Blending::FuncProfile func);
    //! Get the alpha blending function used to draw the widget.
    Blending::FuncProfile getBlendFunc() const { return m_shader.blendFunc; }

	//! Unused update
	virtual void update() {}

	//! Draw the widget and all of its sons
	virtual void draw() = 0;

	//! Draw the widget into the cache. Must be specified for each widget
	virtual void updateCache() = 0;

	//-----------------------------------------------------------------------------------
	// Serialization
	//-----------------------------------------------------------------------------------

	virtual Bool writeToFile(OutStream &os);
	virtual Bool readFromFile(InStream &is);

	virtual void postImportPass() {}

public:

	//-----------------------------------------------------------------------------------
	// User events. This event can be used directly by the user program
	//-----------------------------------------------------------------------------------

	//! The mouse enter into the widget
    Signal<> onEnterWidget{this};

	//! The mouse leave out of the widget
    Signal<> onLeaveWidget{this};

	//! The widget obtain the focus
    Signal<> onFocus{this};

	//! The widget lost the focus
    Signal<> onLostFocus{this};

protected:

	enum CapacitiesWidget
	{
        CAPS_LAYOUT_TYPE = CAPS_WIDGET_PROPERTIES_LAST + 1,  //!< Specialized for layout
        CAPS_CONTAINER,     //!< Not a layout but can contains children widgets
        CAPS_FOCUSABLE,     //!< Input focus
        CAPS_DRAGABLE,      //!< Drag with pointer
        CAPS_DROPABLE,      //!< Drop with pointer
        CAPS_SCROLLABLE,    //!< Scrollable content with virtual size
        CAPS_COPYABLE,      //!< Clipboard copy
        CAPS_PASTABLE,      //!< Clipboard past
        STATE_FOCUSED,      //!< Widget have the input focus
        STATE_HOVER,        //!< Widget is hover by pointer
		CAPS_WIDGET_LAST = STATE_HOVER
	};

	//! Default constructor.
	Widget(BaseObject *parent);

	//! Common init part (called by SetParent)
	void init();

    //! Update the tab index for top-level widgets
    virtual void updateTabIndex(Widget *widget);


	//-----------------------------------------------------------------------------------
	// Physical size
	//-----------------------------------------------------------------------------------

	Vector2i m_pos;            //!< widget position (without borders)
	Vector2i m_size;           //!< widget size (without borders)

	Vector2i m_minSize;        //!< minimal size (and set by the initial size)
	Vector2i m_maxSize;        //!< maximal size


	//-----------------------------------------------------------------------------------
	// Virtual size (scrolling)
	//-----------------------------------------------------------------------------------

	Vector2i m_virtualSize;    //!< virtual size (ie the scrolled area)
	Vector2i m_minVirtualSize; //!< minimal virtual size
	Vector2i m_maxVirtualSize; //!< maximal virtual size


	//-----------------------------------------------------------------------------------
	// Others
	//-----------------------------------------------------------------------------------

	Theme *m_theme;            //!< theme used to display the widget
	LayoutItem *m_layoutItem;  //!< the layout item information

    Int32 m_tabIndex;          //!< tab index (default -1 not used in tab navigation)

	//-----------------------------------------------------------------------------------
	// Drawing members
	//-----------------------------------------------------------------------------------

	Texture2D m_texture;
	Vector2f m_textureTopRightCoord;

	//! Simply draw the cache (a simple textured quad by default)
	virtual void drawCache();

	//! Cache update if necessary, and followed by a draw of him. (call UpdateCache and DrawCache methods)
	void processCache(Bool clear = True);

	struct WigdetShader
	{
        WigdetShader(Context *context);

		ShaderInstance shaderInstance;

		Int32 a_vertex;
		Int32 a_texCoords;
		Int32 u_modelViewProjectionMatrix;
		Int32 u_alpha;
		Int32 u_texture;

        VertexBufferObjf vertices;
        VertexBufferObjf texCoords;

        Blending::FuncProfile blendFunc;
	};

	WigdetShader m_shader;

public:

	//! INTERNAL : define the layout item
	void defineLayoutItem();

	//! INTERNAL : get the layout item
	LayoutItem* getLayoutItem();
	//! INTERNAL : get the layout item (const version)
	const LayoutItem* getLayoutItem() const;
};

} // namespace o3d

#endif // _O3D_WIDGET_H

