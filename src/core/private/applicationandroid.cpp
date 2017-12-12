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

// Specific dependencies includes
#include <android/log.h>
#include <android/android_native_app_glue.h>

using namespace o3d;

// #include <android/log.h> @todo for Log
#define O3D_ALOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "NativeActivitySimpleExample", __VA_ARGS__))

static int32_t handleInput(struct android_app* app, AInputEvent* event)
{
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        size_t pointerCount = AMotionEvent_getPointerCount(event);

        for (size_t i = 0; i < pointerCount; ++i) {
            O3D_ALOG("Received motion event from pointer %zu: (%.2f, %.2f)", i, AMotionEvent_getX(event, i), AMotionEvent_getY(event, i));
        }

        return 1;
    } else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        O3D_ALOG("Received key event: %d", AKeyEvent_getKeyCode(event));
        return 1;
    }

    return 0;
}

static void handleCmd(struct android_app* app, int32_t cmd)
{
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // the OS asked us to save the state of the app
            break;

        case APP_CMD_INIT_WINDOW:
            // get the window ready for showing
            break;

        case APP_CMD_TERM_WINDOW:
            // clean up the window because it is being hidden/closed
            break;

        case APP_CMD_LOST_FOCUS:
            // if the app lost focus, avoid unnecessary processing (like monitoring the accelerometer)
            break;

        case APP_CMD_GAINED_FOCUS:
            // bring back a certain functionality, like monitoring the accelerometer
            break;

        default:
            break;
    }
}

void Application::apiInitPrivate()
{
    if (ms_app) {
        ms_display = reinterpret_cast<_DISPLAY>(EGL_DEFAULT_DISPLAY);

        // setup app state
        struct android_app* state = reinterpret_cast<struct android_app*>(ms_app);
        pthread_mutex_lock(&state->mutex);
        // state->userData = ;
        state->onAppCmd = handleCmd;
        state->onInputEvent = handleInput;
        pthread_mutex_unlock(&state->mutex);
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
}

// Push a user application event.
void Application::pushEventPrivate(EventType type, _HWND hWnd, void *data)
{
    // @todo
}

#endif // O3D_ANDROID