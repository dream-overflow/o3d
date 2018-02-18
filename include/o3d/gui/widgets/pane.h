/**
 * @file pane.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_PANE_H
#define _O3D_PANE_H

#include "widget.h"
#include "../textzone.h"

namespace o3d {

/**
 * @brief Pane
 * Sized pane widget, that can have a border, and a title label
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2013-10-10
 */
class O3D_API Pane : public Widget
{
public:

	O3D_DECLARE_CLASS(Pane)

	enum PaneStyle
	{
		EMPTY_STYLE = 0,
		BORDER_STYLE = 1,
		LABEL_STYLE = 2,
		DEFAULT_STYLE = BORDER_STYLE | LABEL_STYLE
	};

	Pane(
			Widget *parent,
			const String &label = "",
			const Vector2i &pos = DEFAULT_POS,
			const Vector2i &size = DEFAULT_SIZE,
			PaneStyle style = DEFAULT_STYLE,
			const String &name = "");

	virtual ~Pane();

	//-----------------------------------------------------------------------------------
	// Layout
	//-----------------------------------------------------------------------------------

	//! set the parent layout. By default a vertical BoxLayout is setup.
	void setLayout(Layout *layout);

	//! get the parent layout. By default a vertical BoxLayout is setup. (const version)
    virtual const Layout* getLayout() const override;
	//! get the parent layout. By default a vertical BoxLayout is setup.
    virtual Layout* getLayout() override;

	//! get the recommended widget default size
    virtual Vector2i getDefaultSize() override;

	//! get the widget client size. the client area is the area which may be drawn on by
	//! the programmer (excluding title bar, border, scrollbars, etc)
    virtual Vector2i getClientSize() const override;


	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! Set the title.
	void setLabel(const String &label);
	//! Get the title.
	const String& getLabel() const;

	//! Set the title horizontal alignment (default is left).
	void setTitleAlign(TextZone::HorizontalAlignmentMode align);
	//! Get the title horizontal aligment.
	TextZone::HorizontalAlignmentMode getTitleAlign() const;

	//! get the pane style
	inline PaneStyle getPaneStyle() const { return m_paneStyle; }

	//------------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

    virtual Vector2i getOrigin() const override;

	//! Is widget targeted ?
    virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget) override;

	//! Mouse Events
    virtual void mouseMoveIn() override;
    virtual void mouseMoveOut() override;

    virtual void focused() override;
    virtual void lostFocus() override;

    virtual void sizeChanged() override;

    //! Get the widget corresponding to next tab index or nullptr if none.
    virtual Widget* findNextTabIndex(Widget *widget, Int32 direction) override;


	// Draw
    virtual void draw() override;
    virtual void updateCache() override;

private:

	Pane(BaseObject *parent);

	AutoPtr<Layout> m_pLayout;  //!< Layout used to manage the pane content

	PaneStyle m_paneStyle;
	TextZone m_label;

	Vector2i m_textSize;

	Box2i m_labelRect;
	Box2i m_borderRect;

	void init();
};

} // namespace o3d

#endif // _O3D_PANE_H
