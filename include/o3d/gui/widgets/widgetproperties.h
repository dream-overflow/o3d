/**
 * @file widgetproperties.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_WIDGETPROPERTIES_H
#define _O3D_WIDGETPROPERTIES_H

#include "o3d/engine/scene/sceneentity.h"
#include "o3d/core/templatebitset.h"

namespace o3d {

/**
 * @brief Contain the base properties to all widget and layout objects.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2008-02-21
 */
class O3D_API WidgetProperties : public SceneEntity
{
public:

	O3D_DECLARE_ABSTRACT_CLASS_NO_COPY(WidgetProperties)

	//! Define the borders sides (use OR bitwise operator). AllBorders is default.
	enum Borders
	{
		BORDER_LEFT = 1,
		BORDER_RIGHT = 2,
		BORDER_TOP = 4,
		BORDER_BOTTOM = 8,
		BORDER_ALL = BORDER_LEFT | BORDER_RIGHT | BORDER_TOP | BORDER_BOTTOM
	};

	//! Define the vertical alignment. MiddleAlign is default.
	enum VerticalAlign
	{
		TOP_ALIGN,
		MIDDLE_ALIGN,
		BOTTOM_ALIGN,
		VERTICAL_EXPAND
	};

	//! Define the horizontal alignment. Left is default.
	enum HorizontalAlign
	{
		LEFT_ALIGN,
		CENTER_ALIGN,
		RIGHT_ALIGN,
		HORIZONTAL_EXPAND
	};

	//! default constructor
	WidgetProperties(BaseObject *parent);

	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! Define the transparency (default is 1 full opacity).
	inline void setAlpha(Float alpha) { m_alpha = alpha; }

	//! Get the transparency.
	inline Float getAlpha() const { return m_alpha; }

	//! Show/hide the widget.
    virtual void show(Bool show = True);

	//! Is the widget activity.
	inline Bool isShown() const { return m_capacities.getBit(STATE_VISIBILITY); }

	//! Enable/disable the widget.
    virtual void enable(Bool active = True);

	//! Is the widget activity.
	inline Bool isActive() const { return m_capacities.getBit(STATE_ACTIVITY); }


	//-----------------------------------------------------------------------------------
	// Borders
	//-----------------------------------------------------------------------------------

	//! define the border size
	inline void setBorderSize(Int32 size)
	{
		m_borderSize = size;
		setDirty();
	}
	//! get the border size
	inline Int32 getBorderSize() const { return m_borderSize; }

	//! define the borders elements (use OR bitwise operator)
	inline void setBordersElt(Int32 elt)
	{
		m_borderElt = (Borders)elt;
		setDirty();
	}
	//! get the borders elements
	inline Int32 getBorderElt() const { return m_borderElt; }

	//! check for left for border
	inline Bool hasLeftBorder() const { return m_borderElt & BORDER_LEFT; }
	//! check for right for border
	inline Bool hasRightBorder() const { return m_borderElt & BORDER_RIGHT; }
	//! check for top for border
	inline Bool hasTopBorder() const { return m_borderElt & BORDER_TOP; }
	//! check for bottom for border
	inline Bool hasBottomBorder() const { return m_borderElt & BORDER_BOTTOM; }


	//-----------------------------------------------------------------------------------
	// Alignment
	//-----------------------------------------------------------------------------------

	//! define the vertical alignment into its parent
	inline void setVerticalAlign(VerticalAlign align)
	{
		m_vertAlign = align;
		setDirty();
	}
	//! get the vertical alignment into its parent
	inline VerticalAlign getVerticalAlign() const { return m_vertAlign; }

	//! define the horizontal alignment into its parent
	inline void setHorizontalAlign(HorizontalAlign align)
	{
		m_horiAlign = align;
		setDirty();
	}
	//! get the horizontal alignment into its parent
	inline HorizontalAlign getHorizontalAlign() const { return m_horiAlign; }


	//-----------------------------------------------------------------------------------
	// Stretch
	//-----------------------------------------------------------------------------------

	//! define the stretch factor
	inline void setStrecthFactor(Int32 stretch)
	{
		m_stretchFactor = stretch;
		setDirty();
	}
	//! get the stretch factor
	inline Int32 getStretchFactor() const { return m_stretchFactor; }


	//-----------------------------------------------------------------------------------
	// Shaped (aspect ratio)
	//-----------------------------------------------------------------------------------

	//! set shaped
	inline void setShaped(Bool shaped)
	{
		m_capacities.setBit(CAPS_SHAPED, shaped);
		setDirty();
	}

	//! is shaped
	inline Int32 isShaped() const { return m_capacities.getBit(CAPS_SHAPED); }

	//! TODO
	inline Bool isFixedMinSize() const { return False; }
	//! TODO
	inline Bool isAdjustMinSize() const { return False; }

	//! Is the widget need to be updated.
	inline Bool isDirty() const { return m_capacities.getBit(STATE_DIRTY); }

protected:

	//! Capacities and state related to the bit set
	enum Capacities
	{
        STATE_ACTIVITY = 0,    //!< Enabled/disabled widget
        STATE_VISIBILITY,      //!< Shown/hidded
        STATE_DIRTY,           //!< Need an update
        CAPS_SHAPED,           //!< Shape capacity
		CAPS_WIDGET_PROPERTIES_LAST = CAPS_SHAPED
	};

	BitSet32 m_capacities;

	Int32 m_borderSize;        //!< Size of the borders.
    Borders m_borderElt;       //!< Parts of the border.

	VerticalAlign m_vertAlign;     //!< Vertical alignment.
	HorizontalAlign m_horiAlign;   //!< Horizontal alignment.

	Int32 m_stretchFactor;     //!< Stretching factor.

	Float m_alpha;             //!< Widget transparency.

	inline void setDirty() { m_capacities.enable(STATE_DIRTY); }
	inline void setClean() { m_capacities.disable(STATE_DIRTY); }
};

} // namespace o3d

#endif // _O3D_WIDGETPROPERTIES_H
