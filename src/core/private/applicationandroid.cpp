/**
 * @file applicationandroid.cpp
 * @brief Android application specialization
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/application.h"
#include "o3d/core/filemanager.h"

// IF O3D_ANDROID IS SELECTED
#ifdef O3D_ANDROID

#include "o3d/core/debug.h"
#include "o3d/core/appwindow.h"

#ifdef O3D_EGL
#include "o3d/core/private/egldefines.h"
#endif

// Specific dependencies includes
#include <android/log.h>
#include "android/android_native_app_glue.h"

using namespace o3d;

static Bool windowReady = False;

#define O3D_ALOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "NativeActivitySimpleExample", __VA_ARGS__))

static int32_t handleInput(struct android_app* app, AInputEvent* event)
{
    AppWindow::EventData eventData;
    eventData.time = AMotionEvent_getEventTime(event);

    AppWindow *appWindow = Application::getAppWindow(reinterpret_cast<_HWND>(app->window));

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        size_t pointerCount = AMotionEvent_getPointerCount(event);

        for (size_t i = 0; i < pointerCount; ++i) {
            eventData.fx = AMotionEvent_getX(event, i);
            eventData.fy = AMotionEvent_getY(event, i);
            eventData.x = i;

            if (appWindow) {
                appWindow->processEvent(AppWindow::EVT_TOUCH_MOVE, eventData);
            }
        }

        return 1;
    } else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        O3D_ALOG("Received key event: %d", AKeyEvent_getKeyCode(event));
        // @todo key events
        return 1;
    }

    return 0;
}

static void handleCmd(struct android_app* app, int32_t cmd)
{
    AppWindow::EventData eventData;
    AppWindow *appWindow = Application::getAppWindow(reinterpret_cast<_HWND>(app->window));

    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // the OS asked us to save the state of the app
            O3D_MESSAGE("App ask to save");
            break;

        case APP_CMD_INIT_WINDOW:
            windowReady = True;
            // get the window ready for showing
            break;

        case APP_CMD_TERM_WINDOW:
            O3D_MESSAGE("App ask to terminate");
            appWindow->terminate();
            // clean up the window because it is being hidden/closed
            break;

        case APP_CMD_LOST_FOCUS:
            appWindow->processEvent(AppWindow::EVT_INPUT_FOCUS_LOST, eventData);
            // if the app lost focus, avoid unnecessary processing (like monitoring the accelerometer)
            break;

        case APP_CMD_GAINED_FOCUS:
            appWindow->processEvent(AppWindow::EVT_INPUT_FOCUS_GAIN, eventData);
            // bring back a certain functionality, like monitoring the accelerometer
            break;

        case APP_CMD_RESUME:
            O3D_MESSAGE("app resume");
            break;

        case APP_CMD_PAUSE:
        O3D_MESSAGE("app pause");
            break;

        case APP_CMD_STOP:
            O3D_MESSAGE("app stop");
            break;

        case APP_CMD_CONTENT_RECT_CHANGED:
            O3D_MESSAGE("app rect changed");
            break;

        case APP_CMD_WINDOW_REDRAW_NEEDED:
            O3D_MESSAGE("app redraw needed");
            break;

        case APP_CMD_WINDOW_RESIZED:
            O3D_MESSAGE("app resized");
            break;

        case APP_CMD_DESTROY:
            O3D_MESSAGE("app destroy");
            break;

        default:
            break;
    }
}

void Application::apiInitPrivate()
{
    if (ms_app) {
        ms_display = reinterpret_cast<_DISP>(EGL_DEFAULT_DISPLAY);

        // setup app state
        struct android_app* state = reinterpret_cast<struct android_app*>(ms_app);
        pthread_mutex_lock(&state->mutex);
        // state->userData = ;
        state->onAppCmd = handleCmd;
        state->onInputEvent = handleInput;
        pthread_mutex_unlock(&state->mutex);

        // process some events before the application can continue, until the window is ready
        Bool quit = False;

        while (1) {
            int ident;
            int fdesc;
            int events;

            struct android_poll_source* source;

            while ((ident = ALooper_pollAll(0, &fdesc, &events, (void**)&source)) >= 0) {
                // process this event
                if (source) {
                    source->process(state, source);
                }

                if (windowReady) {
                    break;
                }
            }

            if (windowReady) {
                break;
            }
        }

    }
}

void Application::apiQuitPrivate()
{
    if (ms_display) {
        ms_display = NULL_DISP;

        // is there something todo here ?

        ms_app = nullptr;
    }
}

// Run the application main loop.
void Application::runPrivate(Bool runOnce)
{
    Bool quit = False;
    AppWindow::EventData eventData;
    struct android_app* state = reinterpret_cast<struct android_app*>(ms_app);

    ms_currAppWindow = nullptr;

    while (!quit || EvtManager::instance()->isPendingEvent()) {
        ms_currAppWindow = nullptr;

        EvtManager::instance()->processEvent();

        int ident;
        int fdesc;
        int events;

        struct android_poll_source* source;

        while ((ident = ALooper_pollAll(0, &fdesc, &events, (void**)&source)) >= 0) {
            // process this event
            if (source) {
                source->process(state, source);
            }
        }

        if (ms_appWindowMap.empty()) {
            quit = True;
        }

        // process update/paint event if necessary for each window
        for (IT_AppWindowMap it = ms_appWindowMap.begin(); it != ms_appWindowMap.end(); ++it) {
            if (it->second->isRunning()) {
                it->second->processEvent(AppWindow::EVT_UPDATE, eventData);
                it->second->processEvent(AppWindow::EVT_PAINT, eventData);
            }
        }

        if (runOnce) {
            break;
        }
    }

    ms_currAppWindow = nullptr;
}

// Push a user application event.
void Application::pushEventPrivate(EventType type, _HWND hWnd, void *data)
{
    // @todo
}

#endif // O3D_ANDROID
