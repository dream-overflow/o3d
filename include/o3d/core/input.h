/**
 * @file input.h
 * @brief Base class for input device such as keyboard, mouse, joystick and also webcam.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2003-09-04
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_INPUT_H
#define _O3D_INPUT_H

#include "baseobject.h"
#include "memorydbg.h"

namespace o3d {

#define O3D_ISKEYDOWN(value) ((value)>0?True:False)

/* Mouse Data structure */
struct MouseData
{
	Int32 lX,lY;             //!< Mouse x/y position
	Int32 lZ;                //!< wheel mouse (unit of 120)
	UInt8 rgbButtons[8];     //!< Mouse buttons
};

/**
 * @brief Base class for input device such as keyboard, mouse, joystick and also webcam.
 */
class O3D_API Input : public BaseObject
{
public:

	O3D_DECLARE_ABSTRACT_CLASS(Input)

    enum InputType
    {
        INPUT_UNKNOWN = 0,
        INPUT_KEYBOARD = 1,
        INPUT_MOUSE = 2,
        INPUT_TOUCHSCREEN = 3,
        INPUT_CAMERA = 4,
        INPUT_JOYSTICK = 5,
        INPUT_JOYPAD = 6
    };

	//! constructor
    Input(BaseObject *parent = nullptr);
	//! destructor
	virtual ~Input() {}

	//
	// Parameters
	//

	//! set input activity to state
	inline Bool setActivity(Bool state)
	{
		Bool old = m_isActive;
		m_isActive = state;
		return old;
	}
	//! is input active
	inline Bool getActivity()const { return m_isActive; }
	//! toggle input state
	inline Bool toggle()
	{
        if (m_isActive) {
			return disable();
        } else {
			return enable();
        }
	}

	//! Enable the state
    virtual Bool enable();
	//! Disable the state
    virtual Bool disable();

	//
	// virtual methods
	//

    //! Get input type.
    virtual InputType getInputType() const = 0;

	//! Clear some data before process the SetMouseButton and next call Update.
	virtual void clear() = 0;

	//! update input data (only if acquired)
	virtual void update() = 0;

	//! acquire input data at initialization or when lost.
	virtual void acquire() = 0;

	//! release input data at exit or when leave window.
	virtual void release() = 0;

protected:

	Bool m_isActive;     //!< input enable
};

} // namespace o3d

#endif // _O3D_INPUT_H
