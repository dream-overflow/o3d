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
    virtual ~TouchScreen();

    //! Is multi-touch capacity.
    inline Bool isMultiTouch() const { return m_multiTouch; }

    //! Set the last touch screen local position.
    void setPosition(Int32 index, Float x, Float y);

    //! Set the touch up.
    void setUp();

    //! Set the touch down.
    void setDown();

    //! Set the touch pointer up.
    void setPointerUp(Float pressure);

    //! Set the touch pointer down.
    void setPointerDown(Float pressure);

    //! Get the local coordinates.
    inline Vector2f getPosition() const { return m_pos; }

    //! Get the X local coordinates.
    inline Float getPositionX() const { return m_pos.x(); }
    //! Get the Y local coordinates.
    inline Float getPositionY() const { return m_pos.y(); }

    //! Get the delta distance since the last update.
    inline Vector2f getDelta() const { return m_deltaPos; }

    //! Get the X delta.
    inline Float getDeltaX() const { return m_deltaPos.x(); }
    //! Get the Y delta.
    inline Float getDeltaY() const { return m_deltaPos.y(); }

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

    Bool m_multiTouch;             //!< Multi-points touch capacity

    Vector2f m_pos;                //!< Local position.
    Vector2f m_oldPos;             //!< Previous local position.
    Vector2f m_deltaPos;           //!< Delta position since last update.

    void commonInit(Int32 xlimit, Int32 ylimit);
};

/**
 * @brief Touch-screen pointer event
 * @todo
 */
class TouchScreenEvent
{
public:

    TouchScreenEvent() {}

private:
};


} // namespace o3d

#endif // _O3D_TOUCHSCREEN_H
