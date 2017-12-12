/**
 * @file keyboard.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_KEYBOARD_H
#define _O3D_KEYBOARD_H

#include "input.h"
#include "memorydbg.h"
#include "hashset.h"
#include "templatebitset.h"

#ifdef O3D_SDL2
#include "private/keymapsdl.h"
#endif

#ifdef O3D_WINAPI
#include "private/keymapwin32.h"
#endif

#ifdef O3D_X11
#include "private/keymapx11.h"
#endif

#ifdef O3D_ANDROID
#include "private/keymapandroid.h"
#endif

namespace o3d {

class AppWindow;

/**
 * @brief Keys having a state.
 * @date 2014-02-07
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
enum KeyMods
{
    MOD_CAPSLOCK = 0,
    MOD_NUMLOCK = 1,
    MOD_INSERT = 2
};

/**
 * @brief Key event.
 * @date 2010-01-01
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class KeyEvent
{
public:

    /**
     * @brief KeyEvent Constructor
     * @param key Virtual pressed/release/repeated key code.
     * @param character Virtual compound key code.
     * @param pressed TRUE if the key is pressed, FALSE if the key is released.
     * @param repeat TRUE if the key is a repetition.
     */
    KeyEvent(VKey key, VKey character, Bool pressed, Bool repeat) :
        m_character(character),
        m_key(key)
	{
		m_state = repeat ? REPEAT | DOWN : (pressed ? PRESSED | DOWN : RELEASED | UP);
	}

    //! Get the compound key code.
    inline VKey character() const { return m_character; }

	//! Get the virtual key code.
	inline VKey key() const { return m_key; }

	//! Is the key is up.
	inline Bool isUp() const { return m_state & UP; }
	//! Is the key is pressed.
	inline Bool isPressed() const { return m_state & PRESSED; }

	//! Is the key is down.
	inline Bool isDown() const { return m_state & DOWN; }
	//! Is the key is released (now up).
	inline Bool isReleased() const { return m_state & RELEASED; }

	//! Is a key repetition.
	inline Bool isRepeat() const { return m_state & REPEAT; }

    /**
     * @brief action Helper to write inc or decr a value according to the key press,
     *        or key release event.
     * @param key Virtual key code
     * @param onPress Value returned when the key is pressed.
     * @param onRelease Value returned when the key is released.
     * @param de Default returned value if no key event occured.
     * @return Default return 0.
     */
    template <class T>
    inline T action(VKey key, T onPress, T onRelease, T def) const
    {
        if (m_key == key) {
            if (m_state & PRESSED) {
                return onPress;
            } else if (m_state & RELEASED) {
                return onRelease;
            }
        }

        return def;
    }

private:

	//! Key states.
	enum States
	{
		RELEASED = 0x01,
		PRESSED = 0x02,
		DOWN = 0x04,
		UP = 0x08,
		REPEAT = 0x10
	};

    VKey m_character;  //!< Virtual compound key code.
    VKey m_key;        //!< Virtual key code.
    Int32 m_state;     //!< Key state.
};

/**
 * @brief Character event.
 * @date 2010-01-01
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class CharacterEvent
{
public:

    /**
     * @brief CharacterEvent
     * @param key Virtual pressed/release/repeated key code.
     * @param character Virtual compound key code.
     * @param unicode unicode Unicode character.
     * @param repeat repeat TRUE if the event is a repeatition.
     */
    CharacterEvent(VKey key, VKey character, WChar unicode, Bool repeat) :
        m_character(character),
		m_key(key),
		m_unicode(unicode)
	{
		m_state = repeat ? REPEAT : 0;
	}

    //! Get the compound key code.
    inline VKey character() const { return m_character; }

	//! Get the virtual key code.
	inline VKey key() const { return m_key; }

	//! Get the unicode character.
	inline WChar unicode() const { return m_unicode; }

	//! Is a character repetition.
	inline Bool isRepeat() const { return m_state & REPEAT; }

private:

	//! Key states.
	enum States
	{
		REPEAT = 0x01
	};

    VKey m_character;  //!< Virtual compound key code.
    VKey m_key;        //!< Virtual key code.
	WChar m_unicode;   //!< Unicode character.
	Int32 m_state;     //!< Key state.
};

/**
 * @brief Keyboard input manager.
 * @date 2003-09-04
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 */
class O3D_API Keyboard : public Input
{
public:

	O3D_DECLARE_CLASS(Keyboard)

    typedef stdext::hash_set<UInt32> T_KeySet;
	typedef T_KeySet::iterator IT_KeySet;
	typedef T_KeySet::const_iterator CIT_KeySet;

	//! Default constructor
    Keyboard(BaseObject *parent = nullptr);

	//! Initialization constructor
	Keyboard(AppWindow *appWindow);

	//! Destructor
	virtual ~Keyboard();

    //! Get keys states. In key code, not virtual key sym.
	inline const T_KeySet& getKeysState() const { return m_keys; }

    //! Send a keys states arrays. In key code, not virtual key sym.
	inline void setKeysState(const T_KeySet &keys) { m_keys = keys; }

	//! Grab the keyboard. Syskeys/appkeys are handled by the window,
	//! and performances are improved.
	void setGrab(Bool grab = True);
	//! Is the keyboard in exclusive mode.
	inline Bool isGrabbed() const { return m_grab; }

    //! Get string name of virtual key.
	String getKeyName(VKey key);

    //! Is a key currently down.
    inline Bool isKeyDown(VKey key) const
	{
        return m_keys.find(key) != m_keys.end();
	}

    //! Is a key currently up.
    inline Bool isKeyUp(VKey key) const
	{
        return m_keys.find(key) == m_keys.end();
	}

    /**
     * @brief getModifier Get the state of a two state key (CapsLock...).
     * @param mod Key modifier (@see KeyMods).
     * @return True if the modifier is currently down.
     */
    Bool getModifier(KeyMods key) const { return m_modifiers.getBit((UInt32)key); }


	//-----------------------------------------------------------------------------------
	// virtual
	//-----------------------------------------------------------------------------------

	//! Clear current keys state.
	virtual void clear();

	//! Update input data (only if acquired).
	virtual void update();

	//! Acquire keyboard hardware.
	virtual void acquire();

	//! Release keyboard hardware.
	virtual void release();

    /**
     * @brief setKeyState Update on key change (on keyboard event).
     * @param vkey Virtual key code of the pressed, repeated or released key.
     * @param state True mean key pressed, False mean released.
     * @return True if the key if pressed, False if released or repeated.
     */
    Bool setKeyState(VKey vkey, Bool state);

protected:

    T_KeySet m_keys;  //!< Currently down keys.

	Bool m_grab;      //!< Raw input.
	Bool m_aquired;   //!< Input acquired.

	AppWindow *m_appWindow; //!< Related application window.

    BitSet32 m_modifiers;  //!< Modifiers state.

	//! Get the state of capslock, numlock key.
	void getKeyboardState();
};

} // namespace o3d

#endif // _O3D_KEYBOARD_H
