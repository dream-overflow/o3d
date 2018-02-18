/**
 * @file scrollbar.h
 * @brief An horizontal or vertical scroll bar widget.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2009-11-16
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_SCROLLBAR_H
#define _O3D_SCROLLBAR_H

#include "widget.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class ScrollBar
//-------------------------------------------------------------------------------------
//! An horizontal or vertical scroll bar widget.
//---------------------------------------------------------------------------------------
class O3D_API ScrollBar : public Widget
{
public:

	O3D_DECLARE_CLASS(ScrollBar)

	//! Scroll bar orientation.
	enum Orientation
	{
		HORIZONTAL_ALIGN = 0,
		VERTICAL_ALIGN
	};

	//! Construct given a parent widget.
	//! @param parent Parent widget.
	//! @param orientation Scroll bar orientation.
	//! @param pos Position.
	//! @param size Size.
	//! @param name Single widget name or empty.
	ScrollBar(
			Widget *parent,
			Orientation orientation,
			const Vector2i &pos = Widget::DEFAULT_POS,
			const Vector2i &size = Widget::DEFAULT_SIZE,
			const String &name = "");

	//! Virtual destructor.
	virtual ~ScrollBar();

	//-----------------------------------------------------------------------------------
	// Properties
	//-----------------------------------------------------------------------------------

	//! Set the maximum position. Minimum is adjusted if necessary, and the current
	//! slider position too.
	void setMaximum(Int32 max);

	//! Set the minimum position. Maximum is adjusted if necessary, and the current
	//! slider position too.
	void setMinimum(Int32 min);

	//! Set the range, (minimal and maximal value). Minimal must be lesser or equal to maximal.
	void setRange(Int32 min, Int32 max);

	//! Get the maximum position.
	inline Int32 getMaximum() const { return m_maximum; }

	//! Get the minimum position.
	inline Int32 getMinimum() const { return m_minimum; }

	//! Get the scroll bar orientation.
	inline Orientation getOrientation() const { return m_orientation; }

	//! Set the single step (when hit a button or arrow key).
	//! If the single step is greater than page step, then page step is
	//! set to the new single step value by internally using a call of SetPageStep(step).
	void setSingleStep(Int32 step);

	//! Get the single step value (when hit a button or arrow key).
	inline Int32 getSingleStep() const { return m_singleStep; }

	//! Set the page step (when drag the empty zone of the scroll bar or page button key).
	void setPageStep(Int32 pageStep);

	//! Get the single step value  (when drag the empty zone of the scroll bar or page button key).
	inline Int32 getPageStep() const { return m_pageStep; }

	//! Define the slider current position in min and max value.
	void setSliderPos(Int32 setPos);

	//! Get the slider current position in min and max value.
	inline Int32 getSliderPos() const { return m_sliderPos; }

	//! Enable/disable slider tracking. When the slider is being dragged, it emit the
	//! OnSliderMoved signal at each mouse move. If the tracking is disable then event
	//! is thrown only when the scroll bar button is released.
	void setTracking(Bool tracking);

	//! Set the delay between two repeats of current action (button up/left/down/right or page scroll).
	//! @param delay Delay in ms. Default is 100 ms.
	inline void setRepeatDelay(UInt32 delay) { m_repeatDelay = delay; }

	//! Get the delay between two repeats of the current action.
	inline UInt32 getRepeatDelay() const { return m_repeatDelay; }

	//! Set the delay to wait before start to repeat the current action.
	//! @param delay Delay in ms. Default is 500 ms.
	inline void setDelayBeforeRepeat(UInt32 delay) { m_beforeDelay = delay; }

	//! Get the delay to wait before start to repeats the current action.
	inline UInt32 getDelayBeforeRepeat() const { return m_beforeDelay; }

	//-----------------------------------------------------------------------------------
	// Widget
	//-----------------------------------------------------------------------------------

	//! get the recommended widget default size.
    virtual Vector2i getDefaultSize() override;

	//! Is widget targeted ?
    virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget) override;

	//! Mouse Events
    virtual Bool mouseLeftPressed(Int32 x,Int32 y) override;
    virtual Bool mouseLeftReleased(Int32 x,Int32 y) override;
    virtual Bool mouseMove(Int32 x,Int32 y) override;
    virtual Bool mouseWheel(Int32 x, Int32 y, Int32 z) override;
    virtual void mouseMoveIn() override;
    virtual void mouseMoveOut() override;

    virtual void focused() override;
    virtual void lostFocus() override;

	//! Events Management
    virtual void sizeChanged() override;

	// Draw
    virtual void draw() override;
    virtual void updateCache() override;

	//! Update for repeated action.
    virtual void update() override;

public:

	//! Emitted when the scroll bar is moved. The parameter is the move change.
    Signal<Int32 /*move*/> onSliderMoved{this};

	//! Emitted when the slider is pressed.
    Signal<> onSliderPressed{this};

	//! Emitted when the slider is released.
    Signal<> onSliderReleased{this};

	//! Emitted when the range is changed (min max value).
    Signal<Int32 /*min*/, Int32 /*max*/> onRangeChanged{this};

protected:

	//! Scroll bar element.
	enum ScrollBarPart
	{
		SCROLLBAR_PART_NONE = 0,    //!< No selected part.
		SCROLLBAR_PART_BUTTON_UP,   //!< Button up/left.
		SCROLLBAR_PART_BUTTON_DOWN, //!< Button down/right.
		SCROLLBAR_PART_SLIDER,      //!< Slider button.
		SCROLLBAR_PART_EMPTY_UP,    //!< Empty before slider button.
		SCROLLBAR_PART_EMPTY_DOWN   //!< Empty after slider button.
	};

	enum CapacitesScrollBar
	{
		STATE_PRESSED = CAPS_WIDGET_LAST + 1,
		STATE_TRACKING,
		STATE_REPEAT
	};

	//! default constructor
	ScrollBar(BaseObject *parent);

	Orientation m_orientation; //!< Scroll bar orientation.

	Bool m_hover;          //!< Hover of the current part.
	Bool m_pushed;         //!< Pushed of the current part.
	ScrollBarPart m_part;      //!< Current mouse over or pressed part.

	Int32 m_minimum;       //!< Minimum scroll bar position.
	Int32 m_maximum;       //!< Maximum scroll bar position.

	Int32 m_singleStep;    //!< Minimal scroll bar step.
	Int32 m_pageStep;      //!< Minimal scroll bar page step.

	Int32 m_sliderPos;     //!< Current slider position in [min..max].

	Bool m_tracking;       //!< Track any slider move.
	Int32 m_oldSliderPos;  //!< Used if tracking is disabled.

	Box2i m_buttonUp;       //!< Button up/left box.
	Box2i m_buttonDown;     //!< Button down/right box.
	Box2i m_emptyUp;        //!< Empty area before slider box (can be empty).
	Box2i m_emptyDown;      //!< Empty area after slider box (can be empty).
	Box2i m_slider;         //!< Slider box (can take all the slider zone).

	Int32 m_sliderZoneMin, m_sliderZoneMax;  //!< Slider zone in pixel position.

	Int32 m_scrollThreshold;  //!< Amount of delta mouse to trigger a single step.
	Vector2i m_lastMousePos;   //!< Last position of the mouse

	UInt32 m_beforeDelay;   //!< Delay before repeat the current action.
	UInt32 m_repeatDelay;   //!< Delay between two repeat of the current action.
	UInt32 m_lastTime;      //!< Time of the last processed current action or 0 if no current action.
	Bool   m_repeat;

	void init();

	//! Update element position (interal layout)
	void updatePos();

	//! Compute the size of the slider
	void computeSliderSize();

	//! Start to repeat the last action at regular interval.
	void startRepeatAction();

	//! Stop to repeat the last action at regular interval.
	void stopRepeatAction();

	//! Update the slider position.
	void updateSliderPos(Int32 amount);
};

/**
 * @brief Scrollbar dedicated to scrollable layout or component.
 */
class LayoutScrollBar : public ScrollBar
{
public:

	//! Construct given a parent widget.
	//! @param parent Parent widget.
	//! @param orientation Scroll bar orientation.
	//! @param pos Position.
	//! @param size Size.
	//! @param name Single widget name or empty.
	LayoutScrollBar(
			Widget *parent,
			Orientation orientation,
			const Vector2i &pos = Widget::DEFAULT_POS,
			const Vector2i &size = Widget::DEFAULT_SIZE,
			const String &name = "");

	//! Is widget targeted ?
	virtual Bool isTargeted(Int32 x, Int32 y, Widget *&widget);

	//! Mouse Events
	virtual Bool mouseLeftPressed(Int32 x,Int32 y);
	virtual Bool mouseLeftReleased(Int32 x,Int32 y);
	virtual Bool mouseMove(Int32 x,Int32 y);
	virtual Bool mouseWheel(Int32 x, Int32 y, Int32 z);
};

} // namespace o3d

#endif // _O3D_SCROLLBAR_H
