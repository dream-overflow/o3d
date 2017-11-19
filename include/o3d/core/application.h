/**
 * @file application.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_APPLICATION_H
#define _O3D_APPLICATION_H

#include "commandline.h"
#include "baseobject.h"

#include <map>

namespace o3d {

class AppWindow;

/**
 * @brief Base application object settings.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-08-12
 * The Application class need somes parameters, that is. This class defines the necessary
 * parameters with defaults values that should be suffiscient for most of the usage, but
 * if you want to customize this settings, you just have to inherit from this class and
 * resettings the values.
 * The fast allocator is essentially used for allocation of mathematics objects like
 * matrice, vector and quaternion. There is a versions for array of such objects :
 * AtomicArray.
 */
class O3D_API AppSettings
{
public:

	//! Default constructor
    AppSettings() :
		m_memoryManagerFastAlloc16(16384),
		m_memoryManagerFastAlloc32(16384),
        m_memoryManagerFastAlloc64(16384),
        m_display(True)
	{}

    size_t m_memoryManagerFastAlloc16;  //!< Fast allocator chunk sizes for block of 16 bytes
    size_t m_memoryManagerFastAlloc32;  //!< Fast allocator chunk sizes for block of 32 bytes
    size_t m_memoryManagerFastAlloc64;  //!< Fast allocator chunk sizes for block of 64 bytes

    //! If True the display part containing the main event loop and support
    //! for application window is initialized.
    //! @note This can be usefull when you want to use O3D API for non graphic client, or
    //! for services and don't need of a display support. If you disable the display
    //! support and need a main event loop, you have to initiate the EvtManager with the
    //! main thread (as nullptr), and to call its update in your own main loop.
    Bool m_display;
};


/**
 * @brief Application entry
 * @date 2010-08-12
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * Application is a static class offering support for :
 * - the command line interface (CLI),
 * - application initialization and terminaison (init, quit)
 * - file mapping (usefull for making single instance of an application)
 * - global object registration with auto deletion at exit
 * - a main event loop depending of the current compiled version (Windows API, X11,
 *   SDL...) that is able to manage many applications window (AppWindow).
 * This is where an O3D application must start and terminate.
 */
class O3D_API Application
{
public:

	//! Icon style.
	enum IconStyle
	{
		ICON_HELP,
		ICON_OK,
		ICON_WARNING,
		ICON_INFORMATION,
		ICON_NOTICE,
		ICON_QUESTION,
		ICON_ERROR
	};

    enum EventType
    {
        EVENT_STD_TIMER = 0xfffd,      //!< Std timer callback to process (not for WIN32 native timer)
        EVENT_CLOSE_WINDOW = 0xfffe,   //!< AppWindow terminaison
        EVENT_EVT_MANAGER = 0xffff     //!< EvtManger as a new event to process
    };

	//! Initialize the application.
	//! For O3D_WIN32 application, argc and argv are ignored.
	//! @param argc Number of argument in argv or -1 if unknown
	//! @param argv Array of char*
	static void init(
		AppSettings settings,
        Int32 argc = -1,
        Char **argv = nullptr);

	//! Terminate the application.
	static void quit();

	//! Returns true if the application is correctly initialized.
    static Bool isInit();

	//! Return the command line
	static CommandLine* getCommandLine();

	//! Get the application name
	static const String& getAppName();

    //! Get the application path
    static const String& getAppPath();

	//! Show a message. On win32 application it draw a standard message box,
	//! on UNIX's plate-forms it print on stdout.
	//! @param content Content of the message box.
	//! @param title Title of the message box.
	//! @param icon Icon style of the message box. Default is IconInformation.
	static void message(
			const String &content,
			const String &title,
			IconStyle icon = ICON_INFORMATION);

	//! Map a single file. It is mostly used to make a single instance application.
	//! Try to map with a unique string, using this method. If it return TRUE you can
	//! continue. Otherwise it mean the mapped file exists, then a first instance exists.
	//! @param name Name of the unique mapped file. Only ASCII characters and no spaces.
	//! An exception is thrown if the file name is already mapped.
	static void mapSingleFile(const String &name);

	//! Check for a mapped file existence.
	//! @param name Name of the unique mapped file to check for.
	//! @return TRUE if the file exists.
    static Bool isMappedFileExists(const String &name);

	//! Run the application main loop until there is at least one event or one appWindow.
    //! In the case you are using your own main loop or window manager, you have to
    //! setup the two callbacks (setEvtManagerCallback and setStdTimerCallback).
    static void run(Bool runOnce = False);

	//! Push a user application event.
    static void pushEvent(EventType type, _HWND hWnd, void *data);

	//! Get the default display server.
	static _DISP getDisplay();

    //! Is an error on display server.
    static Bool isDisplayError();

	//! Search an application window by its handle.
	//! This method is not thread-safe, and must be called only by the application thread.
	static AppWindow* getAppWindow(_HWND window);

	//! Add an application window to the application.
	//! The appWindow is automatically deleted when the window is closed.
	//! This method is not thread-safe, and must be called only by the application thread.
	static void addAppWindow(AppWindow *appWindow);

	//! Remove manually an application window from the application.
	//! The appWindow is closed if is opened, and deleted.
	//! This method is not thread-safe, and must be called only by the application thread.
	static void removeAppWindow(_HWND hWnd);

	//! Get the process id.
    static Int32 getPID();

    //! Register by a name, a pointer, to be retrieved by getObject.
    static void registerObject(const String &name, BaseObject *object);

    //! Unregister a previously registred object, by its name.
    static void unregisterObject(const String &name);

    //! Get a registred object, by its name.
    static BaseObject* getObject(const String &name);

private:

    static void runPrivate(Bool runOnce);
    static void pushEventPrivate(EventType type, _HWND hWnd, void *data);
    static void apiInitPrivate();
    static void apiQuitPrivate();
    static void getBaseNamePrivate(Int32 argc, Char **argv);

protected:

	typedef std::map<_HWND, AppWindow*> T_AppWindowMap;
	typedef T_AppWindowMap::iterator IT_AppWindowMap;
	typedef T_AppWindowMap::const_iterator CIT_AppWindowMap;

	static String *ms_appsName;
    static String *ms_appsPath;

	static T_AppWindowMap ms_appWindowMap;

	static _DISP ms_display;
    static Bool ms_displayError;

	static AppWindow *ms_currAppWindow;
	static CommandLine *ms_appsCommandLine;

    static Bool ms_init;
    static Bool ms_displayInit;

public:

    static void throwDisplayError(void *generic_data);
};

} // namespace o3d

#endif // _O3D_APPLICATION_H
