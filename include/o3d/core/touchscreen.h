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

    void commonInit(Int32 xlimit, Int32 ylimit);
};

} // namespace o3d

#endif // _O3D_TOUCHSCREEN_H
