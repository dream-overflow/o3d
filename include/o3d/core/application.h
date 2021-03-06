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
 * @brief Activity interface necessary for any complete application processing.
 * The activity must be then setup to the application singleton.
 * Each method returns 0 on success. Otherwise it is an error, and the activty will
 * ask to exit.
 */
class O3D_API Activity
{
public:

    virtual ~Activity() = 0;

    /**
     * @brief Activity start here. Windows and content can be instancied now.
     */
    virtual Int32 onStart() = 0;

    /**
     * @brief Activty stop here. Windows and content must be destroyed.
     */
    virtual Int32 onStop() = 0;

    /**
     * @brief Activity is paused for a moment, could be destroy ou resumed after that.
     */
    virtual Int32 onPause() = 0;

    /**
     * @brief Activity is resume after a pause.
     */
    virtual Int32 onResume() = 0;

    /**
     * @brief Activity might be saved now, could not have the time later. But
     * this can also be done on pause.
     */
    virtual Int32 onSave() = 0;
};

/**
 * @brief Base application object settings.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2010-08-12
 * @details The Application class need somes parameters, that is. This class defines the
 * necessary parameters with defaults values that should be suffiscient for most of the usage,
 * but if you want to customize this settings, you just have to inherit from this class and
 * resettings the values.
 * The fast allocator is essentially used for allocation of mathematics objects like
 * matrice, vector and quaternion. There is a versions for array of such objects :
 * AtomicArray.
 */
class O3D_API AppSettings
{
public:

    /**
     * @brief Default constructor
     * Offers a common settings for most of the usages.
     */
    AppSettings() :
        sizeOfFastAlloc16(16384),
        sizeOfFastAlloc32(16384),
        sizeOfFastAlloc64(16384),
        useDisplay(True),
        clearLog(True)
    {
    }

    size_t sizeOfFastAlloc16;  //!< Fast allocator chunk sizes for block of 16 bytes
    size_t sizeOfFastAlloc32;  //!< Fast allocator chunk sizes for block of 32 bytes
    size_t sizeOfFastAlloc64;  //!< Fast allocator chunk sizes for block of 64 bytes

    /**
     * @details If True the display part containing the main event loop and support
     * for application window is initialized.
     * @note This can be usefull when you want to use O3D API for non graphic client, or
     * for services and don't need of a display support. If you disable the display
     * support and need a main event loop, you have to initiate the EvtManager with the
     * main thread (as nullptr), and to call its update in your own main loop.
     */
    Bool useDisplay;

    /**
     * @details Clear the default choosen log file at startup.
    */
    Bool clearLog;
};

/**
 * @brief Application singleton.
 * @date 2010-08-12
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @details Application is a static class offering support for :
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

    /**
     * @brief Initialize the application.
     * @note argc and argv are only for Unixes and Windows on console main.
     * @param argc Command line argument count or -1.
     * @param argv Command line argument values (array of Char*) or null.
     * @param app Extra pointer on specific strucure (for Android it is the app).
     */
    static void init(AppSettings settings, Int32 argc = -1, Char **argv = nullptr, void *app = nullptr);

    /**
     * @brief Terminate the application.
     */
	static void quit();

    /**
     * @brief Returns true if the application is correctly initialized.
     */
    static Bool isInit();

    /**
     * @brief Return the command line
     */
	static CommandLine* getCommandLine();

    /**
     * @brief Get the application name
     */
	static const String& getAppName();

    /**
     * @brief Get the application path
     */
    static const String& getAppPath();
    /**
     * @brief Show a message. On win32 application it draw a standard message box,
     * on UNIX's plate-forms it print on stdout.
     * @param content Content of the message box.
     * @param title Title of the message box.
     * @param icon Icon style of the message box. Default is IconInformation.
     */
    static void message(
			const String &content,
			const String &title,
			IconStyle icon = ICON_INFORMATION);

    /**
     * @brief Map a single file. It is mostly used to make a single instance application.
     * Try to map with a unique string, using this method. If it return TRUE you can
     * continue. Otherwise it mean the mapped file exists, then a first instance exists.
     * @param name Name of the unique mapped file. Only ASCII characters and no spaces.
     * @exception An exception is thrown if the file name is already mapped.
     */
	static void mapSingleFile(const String &name);

    /**
     * @brief Check for a mapped file existence.
     * @param name Name of the unique mapped file to check for.
     * @return True if the file exists.
     */
    static Bool isMappedFileExists(const String &name);

    /**
     * @brief Must be setup before start.
     */
    static void setActivity(Activity *activity);

    /**
     * @brief Must be called before run into the main.
     */
    static void start();

    /**
     * @brief Run the application main loop until there is at least one event or one appWindow.
     * @details In the case you are using your own main loop or window manager, you have to
     * setup the two callbacks (setEvtManagerCallback and setStdTimerCallback).
     */
    static void run(Bool runOnce = False);

    /**
     * @brief Must be called after run into the main.
     */
    static void stop();

    /**
     * @brief Push a user application event.
     * @param type
     * @param hWnd
     * @param data
     */
    static void pushEvent(EventType type, _HWND hWnd, void *data);

    /**
     * @brief Get the default display server.
     * @return
     */
	static _DISP getDisplay();

    /**
     * @brief On some implementation this can returns an specific structure.
     */
    static void* getApp();

    /**
     * @brief Is an error on display server.
     */
    static Bool isDisplayError();

    /**
     * @brief Search an application window by its handle.
     * @details This method is not thread-safe, and must be called only by the application thread.
     * @param window
     * @return
     */
    static AppWindow* getAppWindow(_HWND window);

    /**
     * @brief Add an application window to the application.
     * @details The appWindow is automatically deleted when the window is closed.
     * This method is not thread-safe, and must be called only by the application thread.
     * @param appWindow
     */
    static void addAppWindow(AppWindow *appWindow);

    /**
     * @brief Remove manually an application window from the application.
     * @details The appWindow is closed if is opened, and deleted.
     * This method is not thread-safe, and must be called only by the application thread.
     * @param hWnd
     */
    static void removeAppWindow(_HWND hWnd);

    /**
     * @brief Get activity (could be null).
     */
    static Activity* getActivity();

    /**
     * @brief Get the process id.
     */
    static Int32 getPID();

    /**
     * @brief Register by a name, a pointer, to be retrieved by getObject.
     * @param name
     * @param object
     */
    static void registerObject(const String &name, BaseObject *object);

    /**
     * @brief Unregister a previously registred object, by its name.
     * @param name
     */
    static void unregisterObject(const String &name);

    /**
     * @brief Get a registred object, by its name.
     * @param name
     * @return
     */
    static BaseObject* getObject(const String &name);

    /**
     * @brief Get current application state flags.
     * @param state
     */
    static void setState(Int32 state);

    /**
     * @brief Get current application state flags.
     */
    static Int32 getState();

private:

    static Int32 startPrivate();
    static void runPrivate(Bool runOnce);
    static Int32 stopPrivate();
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

    static Activity *ms_activity;
	static _DISP ms_display;
    static void* ms_app;
    static Int32 ms_appState;
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
