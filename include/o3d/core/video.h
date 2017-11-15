/**
 * @file video.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_VIDEO_H
#define _O3D_VIDEO_H

#include "string.h"
#include "memorydbg.h"
#include "error.h"

#include <list>
#include <set>

namespace o3d {

typedef std::set<UInt32> T_FrequencySet;
typedef std::set<UInt32>::iterator IT_FrequencySet;
typedef std::set<UInt32>::const_iterator CIT_FrequencySet;

//! Contain info on one graphic mode with his frequency's list
struct VideoMode
{
	//! Default constructor.
	VideoMode() :
		width(0),
		height(0),
        mmWidth(0),
        mmHeight(0),
		bpp(0),
		freq(0),
		index(0)
	{
	}

	UInt32 width, height;      //!< Mode resolution.
    UInt32 mmWidth, mmHeight;  //!< Mode resolution in millimeters.
	UInt32 bpp;                //!< Bits per pixel.
	UInt32 freq;               //!< Rate.
	UInt32 index;              //!< Video mode index.
};

typedef std::list<VideoMode> T_VideoModeList;
typedef std::list<VideoMode>::iterator IT_VideoModeList;
typedef std::list<VideoMode>::const_iterator CIT_VideoModeList;

class AppWindow;

/**
 * @brief Display mode manager.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2002-08-01
 * TODO multi-screen (called screen on X11, display on SDL and WIN32.
 * List display mode and frequency.
 * Find, test or apply display mode.
 * Filters 16 and 32 bpp modes with an height greater or equal to 400 pixels.
 */
class O3D_API Video
{
public:

	//! Singleton instance.
	static Video* instance();
	//! Delete singleton instance.
	static void destroy();

	//-----------------------------------------------------------------------------------
	// Display mode
	//-----------------------------------------------------------------------------------

	//! Get the desktop display mode.
	inline CIT_VideoModeList getDesktopDisplayMode() const { return m_desktop; }

	//! Set display mode by mode iterator. If freq == 0, default system frequency
	//! is used else, force to freq.
	//! @param appWindow Related application window.
	//! @param mode Queried video mode.
	void setDisplayMode(AppWindow *appWindow, CIT_VideoModeList mode);

	//! Restore to desktop display mode.
	void restoreDisplayMode();

	//! Get display mode string.
	String getDisplayModeString(CIT_VideoModeList mode);

	//! Get the number of supported display modes.
	inline UInt32 getNumDisplayMode() const { return (UInt32)m_modes.size(); }

	//! Find a display mode by a string. format : "800x600 32bpp 60hz" for example.
	//! If mode.freq is set to the zero or not set it returns the mode with the higher frequency.
	CIT_VideoModeList findDisplayMode(const String &mode) const;

	//! Find a display mode given a struct that describe the mode.
	//! If mode.freq is set to the zero it returns the mode with the higher frequency.
	CIT_VideoModeList findDisplayMode(const VideoMode &mode) const;

	//! Begin iterator.
	inline CIT_VideoModeList begin() const { return m_modes.begin(); }
	//! End iterator.
	inline CIT_VideoModeList end() const { return m_modes.end(); }

	//
	// Current mode
	//

	//! Get the current application window that use the fullscreen mode or NULL if none.
	inline const AppWindow* getAppWindow() const { return m_appWindow; }

	//! Get an iterator on the current display mode.
	inline CIT_VideoModeList getCurrentDisplayMode() const { return m_currentMode; }

	//! Is display mode different than the desktop mode.
	inline Bool getState()const { return m_currentMode != m_desktop; }

	//! Get current display ratio (width/height).
	Float getCoef() const;

	//! Get the current display width resolution.
	inline UInt32 getWidth() const
	{
		if (m_currentMode != m_modes.end())
			return m_currentMode->width;
		else
			return 0;
	}

	//! Get the current display height resolution.
	inline UInt32 getHeight() const
	{
		if (m_currentMode != m_modes.end())
			return m_currentMode->height;
		else
			return 0;
	}

	//! Get the current display bits per pixel.
	inline UInt32 getBpp() const
	{
		if (m_currentMode != m_modes.end())
			return m_currentMode->bpp;
		else
			return 0;
	}

	//! Get the current display frequency in Hertz.
	inline UInt32 getFrequency() const
	{
		if (m_currentMode != m_modes.end())
			return m_currentMode->freq;
		else
			return 0;
	}

private:

	AppWindow *m_appWindow;            //!< Application window related to the current video mode.

	CIT_VideoModeList m_currentMode;  //!< Current setting.

	T_VideoModeList m_modes;          //!< Available display modes stack

	CIT_VideoModeList m_desktop;      //!< Desktop video mode.
    UInt32 m_desktopFreq;             //!< Desktop video mode rate.

#ifdef O3D_X11
	void *m_screenConfig;             //!< Screen configuration for X11.
	unsigned short m_savedRotation;   //!< Desktop screen saved rotation.
	unsigned short m_savedModeId;     //!< Desktop screen saved mode id.
	short m_savedRate;                //!< Desktop screen saved rate.
#endif

	//! Default contructor.
	Video();

	//! destructor.
	~Video();

	//! singleton
	Video(const Video& win);
	void operator=(const Video& win);

	static Video* m_instance;

	//! list all display modes.
	void listDisplayModes();
};

//! @class E_InvalidVideoMode used for an invalid video mode changing
class O3D_API E_InvalidVideoMode : public E_BaseException
{
    O3D_E_DEF_CLASS(E_InvalidVideoMode)

    //! Ctor
    E_InvalidVideoMode(const String& msg) throw() : E_BaseException(msg)
        O3D_E_DEF(E_InvalidVideoMode, "Invalid video mode")
};

} // namespace o3d

#endif // _O3D_VIDEO_H

