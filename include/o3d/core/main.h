/**
 * @file main.h
 * @brief Application main helper.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2012-12-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_MAIN_H_
#define _O3D_MAIN_H_

#include "application.h"
#include "debug.h"

#ifdef O3D_ANDROID
    #include <android_native_app_glue.h>
#endif

namespace o3d {

void onExit()
{
	// terminate cleanly if the program call exit before an Application::quit().
    if (Application::isInit()) {
		Application::quit();
    }
}

template <class T_Main, class T_Settings>
int main(int argc, char *argv[])
{
    atexit(onExit);

	T_Settings settings;

    try {
        o3d::Application::init(settings, argc, argv, nullptr);
    } catch (o3d::E_BaseException &ex) {
		Application::message(
			String("Failed to initialize the application: ") + ex.what(),
			Application::getAppName(),
			Application::ICON_ERROR);

		return -1;
	}

	int lExitCode = 0;

    try {
		lExitCode = T_Main::main();
    } catch (E_BaseException &ex) {
		Application::message(
			String("Exception not caught during application execution: ") + ex.what(),
			Application::getAppName(),
			Application::ICON_ERROR);

		Application::quit();
		return -1;
	}

	Application::quit();

	return lExitCode;
}

#ifdef O3D_WINDOWS
template <class T_Main, class T_Settings>
int main(LPCSTR lpCmdLine)
{
	atexit(onExit);

	T_Settings settings;

    try {
        o3d::Application::init(settings, 0, nullptr, nullptr);
    } catch (o3d::E_BaseException &ex) {
		Application::message(
			String("Failed to initialize the application: ") + ex.what(),
			Application::getAppName(),
			Application::ICON_ERROR);

		return -1;
	}

	int lExitCode = 0;

    try {
		lExitCode = T_Main::main();
    } catch (E_BaseException &ex) {
		Application::message(
			String("Exception not caught during application execution: ") + ex.what(),
			Application::getAppName(),
			Application::ICON_ERROR);

		Application::quit();
		return -1;
	}

	Application::quit();

	return lExitCode;
}
#elif defined(O3D_ANDROID)
template <class T_Main, class T_Settings>
void android_main(android_app *state)
{
    // ensure the android glue is linked
    app_dummy();

    atexit(onExit);

    T_Settings settings;

    try {
        o3d::Application::init(settings, 0, nullptr, state);
    } catch (o3d::E_BaseException &ex) {
        Application::message(
            String("Failed to initialize the application: ") + ex.what(),
            Application::getAppName(),
            Application::ICON_ERROR);

        return /*-1*/;
    }

    int lExitCode = 0;

    try {
        lExitCode = T_Main::main();
    } catch (E_BaseException &ex) {
        Application::message(
            String("Exception not caught during application execution: ") + ex.what(),
            Application::getAppName(),
            Application::ICON_ERROR);

        Application::quit();

        return /*-1*/;
    }

    Application::quit();

    return /*lExitCode*/;
}
#endif

//! Helper define that use default settings for the application.
#define O3D_DEFAULT_CLASS_SETTINGS o3d::AppSettings

#ifdef O3D_ANDROID
    //! Call the main object with a console (Android version).
    #define O3D_CONSOLE_MAIN(CLASS_NAME, CLASS_SETTINGS) \
        void android_main(android_app *state) { o3d::main<CLASS_NAME, CLASS_SETTINGS>(state); }

    //! Call the main object without a console (Android version).
    #define O3D_NOCONSOLE_MAIN(CLASS_NAME, CLASS_SETTINGS) \
        void android_main(android_app *state) { o3d::main<CLASS_NAME, CLASS_SETTINGS>(state); }
#elif defined(O3D_WINDOWS)
    //! Call the main object with a console (Standard version).
    #define O3D_CONSOLE_MAIN(CLASS_NAME, CLASS_SETTINGS) \
        int main(int argc, char *argv[]) { return o3d::main<CLASS_NAME, CLASS_SETTINGS>(argc, argv); }

    //! Call the main object without a console (Windows version).
	#define O3D_NOCONSOLE_MAIN(CLASS_NAME, CLASS_SETTINGS) \
		int APIENTRY WinMain( \
				HINSTANCE hInstance, \
				HINSTANCE hPrevInstance, \
				LPSTR lpCmdLine, \
				int nCmdShow) { return o3d::main<CLASS_NAME, CLASS_SETTINGS>(lpCmdLine); }
#else
    //! Call the main object with a console (Standard version).
    #define O3D_CONSOLE_MAIN(CLASS_NAME, CLASS_SETTINGS) \
        int main(int argc, char *argv[]) { return o3d::main<CLASS_NAME, CLASS_SETTINGS>(argc, argv); }

    //! Call the main object without a console (Unixes version).
	#define O3D_NOCONSOLE_MAIN(CLASS_NAME, CLASS_SETTINGS) \
		int main(int argc, char *argv[]) { return o3d::main<CLASS_NAME, CLASS_SETTINGS>(argc, argv); }
#endif

} // namespace o3d

#endif // _O3D_MAIN_H_
