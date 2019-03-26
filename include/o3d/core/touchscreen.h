/**
 * @file touchscreen.h
 * @brief Touch screen input controller
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-12
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#ifndef _O3D_TOUCHSCREEN_H
#define _O3D_TOUCHSCREEN_H

#include "input.h"
#include "memorydbg.h"
#include "box.h"

namespace o3d {

class AppWindow;

/**
 * @brief TouchScreen input manager
 */
class O3D_API TouchScreen : public Input
{
public:

    O3D_DECLARE_CLASS(TouchScreen)

    //! Long tap minimum duration in millisecond;
    const Int32 LONG_TAP_DURATION = 500;

    //! Double tap default delay in millisecond;
    const Int32 DOUBLE_TAP_DELAY = 500;

    //! Default constructor.
    TouchScreen(BaseObject *parent = nullptr);

    /**
     * @brief Extended constructor.
     * @param appWindow A valid application window.
     * @param width Width of the initial window region.
     * @param height Height of the initial window region.
     * @param lock Lock the mouse to the center of the window's application to provide
     * an exclusive mode.
     */
    TouchScreen(AppWindow *appWindow, Int32 xlimit = 800, Int32 ylimit = 600);

    //! Virtual destructor.
    virtual ~TouchScreen() override;

    //! Is multi-touch capacity.
    inline Bool isMultiTouch() const { return m_multiTouch; }

    //! Set the last touch screen local position.
    void setPosition(UInt32 index, Float x, Float y, Float pressure, Int64 time);
    //! Set the state up or down of the pointer.
    void setPointerState(UInt32 index, Bool down, Float x, Float y, Float pressure, Int64 time);

    //! set the double tap delay (default 500ms)
    void setDoubleTapDelay(Int32 time);
    //! Get the double tap delay (default 500ms).
    Int32 getDoubleTapDelay() const;

    //! Is the pointer up.
    Bool isPointerUp(UInt32 index = 0) const;
    //! Is the pointer down.
    Bool isPointerDown(UInt32 index = 0) const;
    //! Is the pointer state.
    Bool getPointerState(UInt32 index = 0) const;

    //! Get the local coordinates.
    Vector2f getPosition(UInt32 index = 0) const;

    //! Get the pressure.
    Float getPressure(UInt32 index = 0) const;

    //! Get the X local coordinates.
    Float getPositionX(UInt32 index = 0) const;
    //! Get the Y local coordinates.
    Float getPositionY(UInt32 index = 0) const;

    //! Get the delta distance since the last update.
    Vector2f getDelta(UInt32 index = 0) const;

    //! Get the X delta.
    Float getDeltaX(UInt32 index = 0) const;
    //! Get the Y delta.
    Float getDeltaY(UInt32 index = 0) const;

    //! Is a normal tap occurs (less than 500ms).
    Bool isTap() const;

    //! Is a long tap occurs (greater than 500ms).
    Bool isLongTap() const;

    //! Is a double tap occurs.
    Bool isDoubleTap() const;

    //! True if at least two pointer are defined.
    Bool isSize() const;

    //! Size between two pointers (when two pointers are defined).
    Float getSize() const;

    //! Delta of the size (when two pointers are defined).
    Float getDeltaSize() const;

    //-------------------------------------------------------------------------------
    // virtual
    //-------------------------------------------------------------------------------

    virtual InputType getInputType() const override;

    //! Clear current mouse states.
    virtual void clear() override;

    //! Update input data (only if acquired).
    virtual void update() override;

    //! Acquire mouse position and buttons states.
    virtual void acquire() override;

    //! Release mouse hardware.
    virtual void release() override;

protected:

    AppWindow *m_appWindow;        //!< Related application window.

    Bool m_aquired;                //!< input acquired by application
    Bool m_multiTouch;             //!< Multi-touch capacity

    struct Pointer
    {
        Pointer();

        Vector2f pos;                //!< Local position.
        Vector2f oldPos;             //!< Previous local position.
        Vector2f deltaPos;           //!< Delta position since last update.

        Bool pointer;                //!< Pointer state (True down, False up).
        Int8 tap;

        Float pressure, oldPressure;
        Float deltaPressure;

        Int64 time;
        Int64 downTime;
        Int64 dblTapTime;
    };

    std::vector<Pointer> m_pointers;

    Float m_oldSize;
    Float m_deltaSize;

    Int32 m_dblTapDelay;

    void commonInit(Int32 xlimit, Int32 ylimit);
};

/**
 * @brief Touch-screen pointer event
 * @todo
 */
class TouchScreenEvent
{
public:

    enum Type
    {
        TYPE_NONE = 0,
        TYPE_POINTER_UP = 1,
        TYPE_POINTER_DOWN = 2
    };

    TouchScreenEvent(Type type, Float x, Float y, Float pressure, Int64 time) :
        m_type(type),
        m_pos(x, y),
        m_pressure(pressure),
        m_time(time)
    {
    }

    inline Bool isPointerUp() const { return m_type == TYPE_POINTER_UP; }
    inline Bool isPointerDown() const { return m_type == TYPE_POINTER_DOWN; }

    inline Bool getPointerState() const { return m_type == TYPE_POINTER_DOWN ? True : False; }

    inline Float getPressure() const { return m_pressure; }

    inline Int64 getTime() const { return m_time; }

    inline Vector2f getPosition() const { return m_pos; }

private:

    Type m_type;
    Vector2f m_pos;
    Float m_pressure;
    Int64 m_time;
};

} // namespace o3d

#endif // _O3D_TOUCHSCREEN_H
