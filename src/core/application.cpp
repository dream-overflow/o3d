/**
 * @file application.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/application.h"

#include "o3d/core/classfactory.h"
#include "o3d/core/taskmanager.h"
#include "o3d/core/video.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/thread.h"
#include "o3d/core/timer.h"
#include "o3d/core/objects.h"
#include "o3d/core/math.h"
#include "o3d/core/stringmap.h"
#include "o3d/core/appwindow.h"
#include "o3d/core/debug.h"

#include <algorithm>
#include <iostream>

using namespace o3d;

String *Application::ms_appsName = nullptr;
Application::T_AppWindowMap Application::ms_appWindowMap;
_DISP Application::ms_display = NULL_DISP;
AppWindow* Application::ms_currAppWindow = nullptr;
CommandLine *Application::ms_appsCommandLine = nullptr;
Bool Application::ms_init = False;
Bool Application::ms_displayInit = False;
StringMap<BaseObject*> *ms_mappedObject = nullptr;
Bool Application::ms_displayError = False;

// Objective-3D initialization
void Application::init(AppSettings settings, Int32 argc, Char **argv)
{
	if (ms_init)
		return;

    ms_mappedObject = new StringMap<BaseObject*>;

	System::initTime();
	Date::init();

    // only if display
    if (settings.m_display) {
        apiInit();
    }

	// Get the application name
#ifdef O3D_WIN32
	WChar buf[MAX_PATH];
	GetModuleFileNameW(NULL, buf, MAX_PATH);
	ms_appsName = new String(buf);
#else
	if (argv && (argc >= 1) && argv[0])	{
		ms_appsName = new String;
		ms_appsName->fromUtf8(argv[0]);
	}
#endif

	// Initialize fast memory allocator
	MemoryManager::instance()->initFastAllocator(
		settings.m_memoryManagerFastAlloc16,
		settings.m_memoryManagerFastAlloc32,
		settings.m_memoryManagerFastAlloc64);

	// Registration of the main thread to activate events
    EvtManager::instance()->registerThread(nullptr);

#ifdef O3D_WIN32
	String commandLine(GetCommandLineW());
	ms_appsCommandLine = new CommandLine(commandLine);
#else
	ms_appsCommandLine = new CommandLine(argc, argv);
#endif

	// Math initialization
	Math::init();

    // Video only if display
    if (settings.m_display) {
        Video::instance();
    }

	// Active the PostMessage
	EvtManager::instance()->enableAutoWakeUp();

    ms_init = True;
}

// Objective-3D terminate
void Application::quit()
{
	if (!ms_init)
		return;

    ms_init = False;

	// Disable the PostMessage
	EvtManager::instance()->disableAutoWakeUp();

    if (!ms_appWindowMap.empty()) {
		O3D_WARNING("Still always exists application windows");
    }

    if (!ms_mappedObject->empty()) {
        O3D_WARNING("Still always exists mapped object");
    }

	// Global destruction
	o3d::deletePtr(ms_appsCommandLine);
	o3d::deletePtr(ms_appsName);

	// terminate the task manager if running
	TaskManager::destroy();

	// wait all threads terminate
	ThreadManager::waitEndThreads();

	// delete class factory
	ClassFactory::destroy();

	// screen manager
	Video::destroy();
	// timer manager
	TimerManager::destroy();

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

    // object mapping
    deletePtr(ms_mappedObject);

	// release memory manager allocators
	MemoryManager::instance()->releaseFastAllocator();

	// Specific quit
    if (ms_displayInit) {
        apiQuit();
        ms_displayInit = False;
    }
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
	return *ms_appsName;
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

