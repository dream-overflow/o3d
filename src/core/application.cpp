/**
 * @file application.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/application.h"

#include "o3d/core/classfactory.h"
#include "o3d/core/taskmanager.h"
#include "o3d/core/display.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/thread.h"
#include "o3d/core/timer.h"
#include "o3d/core/uuid.h"
#include "o3d/core/date.h"
#include "o3d/core/datetime.h"
#include "o3d/core/math.h"
#include "o3d/core/stringmap.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/debug.h"
#include "o3d/core/gl.h"

#include <algorithm>

using namespace o3d;

String *Application::ms_appsName = nullptr;
String *Application::ms_appsPath = nullptr;
Application::T_AppWindowMap Application::ms_appWindowMap;
_DISP Application::ms_display = NULL_DISP;
Activity *Application::ms_activity = nullptr;
void* Application::ms_app = nullptr;
Int32 Application::ms_appState = 0;
AppWindow* Application::ms_currAppWindow = nullptr;
CommandLine *Application::ms_appsCommandLine = nullptr;
Bool Application::ms_init = False;
Bool Application::ms_displayInit = False;
StringMap<BaseObject*> *ms_mappedObject = nullptr;
Bool Application::ms_displayError = False;

Activity::~Activity()
{

}

// Objective-3D initialization
void Application::init(AppSettings settings, Int32 argc, Char **argv, void *app)
{
    if (ms_init) {
		return;
    }

    ms_appState = 0;
    ms_app = app;
    ms_mappedObject = new StringMap<BaseObject*>;

    // get the main thread id
    ThreadManager::init();

	System::initTime();
	Date::init();
    DateTime::init();
    Uuid::init();

    // Get the application name and path
    getBaseNamePrivate(argc, argv);

    if (settings.clearLog) {
        Debug::instance()->getDefaultLog().clearLog();
    }

    // Log start time
    // DateTime current(True);
    // O3D_MESSAGE(String("Starting of application on ") + current.buildString("%Y-%m-%d %H:%M:%S.%f"));

	// Initialize fast memory allocator
	MemoryManager::instance()->initFastAllocator(
        settings.sizeOfFastAlloc16,
        settings.sizeOfFastAlloc32,
        settings.sizeOfFastAlloc64);

	// Registration of the main thread to activate events
    EvtManager::instance()->registerThread(nullptr);

#ifdef O3D_ANDROID
    // @todo
    ms_appsCommandLine = new CommandLine("");
#elif defined(O3D_WINDOWS)
	String commandLine(GetCommandLineW());
	ms_appsCommandLine = new CommandLine(commandLine);
#else
	ms_appsCommandLine = new CommandLine(argc, argv);
#endif

	// Math initialization
	Math::init();

    // only if display
    if (settings.useDisplay) {
        apiInitPrivate();
        ms_displayInit = True;

        GL::init();

        String typeString = "Undefined";
        if (GL::getType() == GL::API_GL) {
            typeString = "GL3+";
        } else if (GL::getType() == GL::API_GLES_3) {
            typeString = "GLES3+";
        }

        O3D_MESSAGE(String("Choose {0} implementation with a {1} API").arg(GL::getImplementationName()).arg(typeString));

        Display::instance();
    }

	// Active the PostMessage
	EvtManager::instance()->enableAutoWakeUp();

    ms_init = True;
}

// Objective-3D terminate
void Application::quit()
{
    deletePtr(ms_activity);

    if (!ms_init) {
		return;
    }

    // Log quit time
    // DateTime current(True);
    // O3D_MESSAGE(String("Terminating of application on ") + current.buildString("%Y-%m-%d at %H:%M:%S.%f"));

    ms_init = False;

	// Disable the PostMessage
	EvtManager::instance()->disableAutoWakeUp();

    if (!ms_appWindowMap.empty()) {
		O3D_WARNING("Still always exists application windows");
    }

    if (!ms_mappedObject->empty()) {
        O3D_WARNING("Still always exists mapped object");
    }

	// terminate the task manager if running
	TaskManager::destroy();

    // timer manager before thread
    TimerManager::destroy();

	// wait all threads terminate
	ThreadManager::waitEndThreads();

	// delete class factory
	ClassFactory::destroy();

    // display manager
    Display::destroy();

    // Specific quit
    if (ms_displayInit) {
        apiQuitPrivate();
        GL::quit();

        ms_displayInit = False;
    }

	// deletion of the main thread
    EvtManager::instance()->unRegisterThread(nullptr);

	// debug manager
	Debug::destroy();

	// file manager
	FileManager::destroy();
	// event manager
	EvtManager::destroy();

	// math release
	Math::quit();

	// date quit
	Date::quit();
    DateTime::quit();
    Uuid::quit();

    // object mapping
    deletePtr(ms_mappedObject);

	// release memory manager allocators
	MemoryManager::instance()->releaseFastAllocator();

    // common members
    deletePtr(ms_appsCommandLine);
    deletePtr(ms_appsName);
    deletePtr(ms_appsPath);

    ms_app = nullptr;
}

Bool Application::isInit()
{
	return ms_init;
}

// Return the command line
CommandLine* Application::getCommandLine()
{
	return ms_appsCommandLine;
}

// Get the application name
const String& Application::getAppName()
{
    if (ms_appsName) {
        return *ms_appsName;
    } else {
        return String::getNull();
    }
}

const String &Application::getAppPath()
{
    if (ms_appsPath) {
        return *ms_appsPath;
    } else {
        return String::getNull();
    }
}

void Application::addAppWindow(AppWindow *appWindow)
{
    if (appWindow && appWindow->getHWND()) {
        if (ms_appWindowMap.find(appWindow->getHWND()) != ms_appWindowMap.end()) {
			O3D_ERROR(E_InvalidParameter("Cannot add an application window with a similar handle"));
        }

		ms_appWindowMap[appWindow->getHWND()] = appWindow;
    } else {
		O3D_ERROR(E_InvalidParameter("Cannot add an invalid application window"));
    }
}

void Application::removeAppWindow(_HWND hWnd)
{
	IT_AppWindowMap it = ms_appWindowMap.find(hWnd);

    if (it != ms_appWindowMap.end()) {
		AppWindow *appWindow = it->second;

		ms_appWindowMap.erase(it);

		// and delete it
		deletePtr(appWindow);
    } else {
		O3D_ERROR(E_InvalidParameter("Unable to find the window handle"));
    }
}

Activity *Application::getActivity()
{
    return ms_activity;
}

AppWindow* Application::getAppWindow(_HWND window)
{
	IT_AppWindowMap it = ms_appWindowMap.find(window);
    if (it != ms_appWindowMap.end()) {
		return it->second;
    } else {
        return nullptr;
    }
}

void Application::throwDisplayError(void *generic_data)
{
    ms_displayError = True;
}

// Get the default primary display.
_DISP Application::getDisplay()
{
    return ms_display;
}

void *Application::getApp()
{
    return ms_app;
}

Bool Application::isDisplayError()
{
    return ms_displayError;
}

void Application::registerObject(const String &name, BaseObject *object)
{
    if (ms_mappedObject) {
        if (ms_mappedObject->find(name) != ms_mappedObject->end()) {
            O3D_ERROR(E_InvalidOperation(name + " is a registred object"));
        }

        ms_mappedObject->insert(std::make_pair(name, object));
    }
}

void Application::unregisterObject(const String &name)
{
    if (ms_mappedObject) {
        auto it = ms_mappedObject->find(name);
        if (it != ms_mappedObject->end()) {
            ms_mappedObject->erase(it);
        }
    }
}

BaseObject *Application::getObject(const String &name)
{
    if (ms_mappedObject) {
        auto it = ms_mappedObject->find(name);
        if (it != ms_mappedObject->end()) {
            return it->second;
        }
    }

    return nullptr;
}

void Application::setState(Int32 state)
{
    ms_appState = state;
}

Int32 Application::getState()
{
    return ms_appState;
}

#ifndef O3D_ANDROID
Int32 Application::startPrivate()
{
    if (ms_activity) {
        return ms_activity->onStart();
    } else {
        return 0;
    }
}

Int32 Application::stopPrivate()
{
    if (ms_activity) {
        return ms_activity->onStop();
    } else {
        return 0;
    }
}
#endif

void Application::setActivity(Activity *activity)
{
    if (ms_activity) {
        delete ms_activity;
    }

    ms_activity = activity;
}

void Application::start()
{
    if (startPrivate() != 0) {
        // @todo need exit now
    }
}

void Application::run(Bool runOnce)
{
    runPrivate(runOnce);
}

void Application::stop()
{
    if (stopPrivate() != 0) {
        // @todo need exit now
    }
}

void Application::pushEvent(Application::EventType type, _HWND hWnd, void *data)
{
    pushEventPrivate(type, hWnd, data);
}
