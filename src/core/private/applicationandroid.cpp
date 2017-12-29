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

static const Int32 STATE_APP_RUNNING = 0x01;
static const Int32 STATE_APP_PLAYING = 0x02;
static const Int32 STATE_WINDOW_INIT = 0x04;
static const Int32 STATE_WINDOW_FOCUS = 0x08;
static const Int32 STATE_WINDOW_STARTED = 0x10;

static int32_t handleInput(struct android_app* app, AInputEvent* event)
{
    AppWindow *appWindow = Application::getAppWindow(reinterpret_cast<_HWND>(app->window));

    Int32 source = AInputEvent_getSource(event);
    Int32 sourceClass = source & AINPUT_SOURCE_CLASS_MASK;
    // AINPUT_SOURCE_KEYBOARD ...

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        if (!appWindow) {
            return 1;
        }

        AppWindow::MotionEventData eventData;

        eventData.time = AMotionEvent_getEventTime(event);
        size_t pointerCount = AMotionEvent_getPointerCount(event);

        // AMOTION_EVENT_TOOL_TYPE_UNKNOWN 	unknown
        // AMOTION_EVENT_TOOL_TYPE_FINGER 	finger
        // AMOTION_EVENT_TOOL_TYPE_STYLUS 	stylus
        // AMOTION_EVENT_TOOL_TYPE_MOUSE 	mouse
        // AMOTION_EVENT_TOOL_TYPE_ERASER 	eraser

        Int32 action = AMotionEvent_getAction(event);
        // AMotionEvent_getDownTime(event);
        // AMotionEvent_getEdgeFlags(event);
        // AMotionEvent_getMetaState(event);

        // AMOTION_EVENT_AXIS_X ...
        // AMOTION_EVENT_EDGE_FLAG_NONE ...

        // Int32 flags = AMotionEvent_getFlags(event);  // AMOTION_EVENT_FLAG_WINDOW_IS_OBSCURED

        for (size_t i = 0; i < pointerCount; ++i) {
            Int32 pointerId = AMotionEvent_getPointerId(event, i);
            Int32 toolType = AMotionEvent_getToolType(event, i);

            AppWindow::MotionData pointer;

            pointer.x = AMotionEvent_getX(event, i);
            pointer.y = AMotionEvent_getY(event, i);
            pointer.p = AMotionEvent_getPressure(event, i);

            eventData.array.push_back(pointer);

            // AMotionEvent_getAxisValue(event, AXIS, i); => :
            // AMOTION_EVENT_AXIS_VSCROLL
            // AMOTION_EVENT_AXIS_HSCROLL
            // AMOTION_EVENT_AXIS_Z

            // eventData.fx = AMotionEvent_getToolMajor(event, i);
            // eventData.fy = AMotionEvent_getTouchMinor(event, i);
            // eventData.fz = AMotionEvent_getSize(event, i);

            // AMotionEvent_getOrientation(event);  what for us ?
        }

        // action pointer that is not the primary event
        Int32 pointIndex2 = action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        if (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) {
            pointIndex2 = action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        }

        // action event
        switch(action & AMOTION_EVENT_ACTION_MASK) {
            case AMOTION_EVENT_ACTION_CANCEL:
                O3D_MESSAGE("cancel motion");
                break;

            case AMOTION_EVENT_ACTION_DOWN:
                appWindow->processMotionEvent(AppWindow::EVT_TOUCH_POINTER_DOWN, eventData);
                break;

            case AMOTION_EVENT_ACTION_UP:
                appWindow->processMotionEvent(AppWindow::EVT_TOUCH_POINTER_UP, eventData);
                break;

            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                appWindow->processMotionEvent(AppWindow::EVT_TOUCH_POINTER_DOWN, eventData);
                break;

            case AMOTION_EVENT_ACTION_POINTER_UP:
                appWindow->processMotionEvent(AppWindow::EVT_TOUCH_POINTER_UP, eventData);
                break;

            case AMOTION_EVENT_ACTION_MOVE:
                appWindow->processMotionEvent(AppWindow::EVT_TOUCH_MOVE, eventData);
                break;

            case AMOTION_EVENT_ACTION_SCROLL:
                // appWindow->processMotionEvent(AppWindow::EVT_TOUCH_SCROLL, eventData); how to ?
                break;

            case AMOTION_EVENT_ACTION_HOVER_ENTER:
                // appWindow->processMotionEvent(AppWindow::EVT_HOVER_ENTER, eventData); @todo
                break;

            case AMOTION_EVENT_ACTION_HOVER_EXIT:
                // appWindow->processMotionEvent(AppWindow::EVT_HOVER_EXIT, eventData); @todo
                break;

            case AMOTION_EVENT_ACTION_BUTTON_PRESS:
                // AMOTION_EVENT_BUTTON_PRIMARY ...
                eventData.buttons = (UInt32)AMotionEvent_getButtonState(event);
//                if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE) {
//                    appWindow->processMotionEvent(AppWindow::EVT_MOUSE_BUTTON_DOWN, eventData);
//                } else {
//                    // @todo
//                }
                break;

            case AMOTION_EVENT_ACTION_BUTTON_RELEASE:
                eventData.buttons = (UInt32)AMotionEvent_getButtonState(event);
//                if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE) {
//                    appWindow->processMotionEvent(AppWindow::EVT_MOUSE_BUTTON_UP, eventData);
//                } else {
//                    // @todo
//                }
                break;

            default:
                break;
        }

        return 1;
    } else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        if (!appWindow) {
            return 1;
        }

        AppWindow::EventData eventData;
        eventData.time = AKeyEvent_getEventTime(event);

        Int32 flags = AKeyEvent_getFlags(event);
        Int32 action = AKeyEvent_getAction(event);

        // AKEY_STATE_UP AKEY_STATE_DOWN AKEY_STATE_VIRTUAL

        // interest into a long press key
        if ((flags & AKEY_EVENT_FLAG_LONG_PRESS) == AKEY_EVENT_FLAG_LONG_PRESS) {
            // @todo
        } else if ((flags & AKEY_EVENT_FLAG_CANCELED_LONG_PRESS) == AKEY_EVENT_FLAG_CANCELED_LONG_PRESS) {
            // or canceled
        }

        // AKEY_EVENT_FLAG_CANCELED could be interesting too

        eventData.key = (UInt32)AKeyEvent_getKeyCode(event);
        eventData.unicode = (UInt32)AKeyEvent_getKeyCode(event);
        eventData.w = AKeyEvent_getRepeatCount(event);
        eventData.h = AKeyEvent_getMetaState(event);  // AMETA_NONE ...

        if (action == AKEY_EVENT_ACTION_DOWN) {
            appWindow->processEvent(AppWindow::EVT_KEYDOWN, eventData);
        } else if (action == AKEY_EVENT_ACTION_UP) {
            appWindow->processEvent(AppWindow::EVT_KEYDOWN, eventData);
        } else if (action == AKEY_EVENT_ACTION_MULTIPLE) {
            // not clear... to test
        }

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
            // android_app->savedState = malloc(savedStateSize);
            // android_app->savedStateSize
            O3D_MESSAGE("App ask to save");
            break;

        case APP_CMD_INIT_WINDOW:
            O3D_MESSAGE("Init window");
            // get the window ready for showing, but wait for a start/resume
            Application::setState(Application::getState() | STATE_WINDOW_INIT);
            break;

        case APP_CMD_TERM_WINDOW:
            O3D_MESSAGE("Terminate window");
            Application::setState(Application::getState() & ~STATE_WINDOW_INIT);
            break;

        case APP_CMD_LOST_FOCUS:
            if (appWindow) {
                // if the app lost focus, avoid unnecessary processing
                appWindow->processEvent(AppWindow::EVT_INPUT_FOCUS_LOST, eventData);
            }

            Application::setState(Application::getState() & ~STATE_WINDOW_FOCUS);
            break;

        case APP_CMD_GAINED_FOCUS:
            if (appWindow) {
                // bring back certains functionalities
                appWindow->processEvent(AppWindow::EVT_INPUT_FOCUS_GAIN, eventData);
            }

            Application::setState(Application::getState() | STATE_WINDOW_FOCUS);
            break;

        case APP_CMD_START:
            O3D_MESSAGE("app started");
            break;

        case APP_CMD_RESUME:
            O3D_MESSAGE("app resume");
            Application::setState(Application::getState() | STATE_APP_PLAYING);

            if (Application::getState() & STATE_WINDOW_STARTED) {
                if (Application::getActivity()) {
                    Application::getActivity()->onResume();
                }
            }

            break;

        case APP_CMD_PAUSE:
            O3D_MESSAGE("app pause");
            Application::setState(Application::getState() & ~STATE_APP_PLAYING);

            if (Application::getState() & STATE_WINDOW_STARTED) {
                if (Application::getActivity()) {
                    Application::getActivity()->onPause();
                }
            }

            break;

        case APP_CMD_STOP:
            Application::setState(Application::getState() & ~STATE_APP_PLAYING);
            break;

        case APP_CMD_CONTENT_RECT_CHANGED:
            break;

        case APP_CMD_WINDOW_REDRAW_NEEDED:
            break;

        case APP_CMD_WINDOW_RESIZED:
            if ((Application::getState() & STATE_WINDOW_INIT) && appWindow) {
                // AppWindow::MotionEventData eventData;
                // eventData.x = ?;
                // eventData.y = ?;
                // @todo
                // ms_currAppWindow->processEvent(AppWindow::EVT_RESIZE, eventData);
            }
            break;

        case APP_CMD_DESTROY:
            O3D_MESSAGE("App destroy");
            Application::setState(Application::getState() & ~STATE_APP_RUNNING);
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
        state->onAppCmd = handleCmd;
        state->onInputEvent = handleInput;
        pthread_mutex_unlock(&state->mutex);

        Application::setState(STATE_APP_RUNNING);
    }
}

void Application::apiQuitPrivate()
{
    if (ms_display) {
        ms_display = NULL_DISP;

        // setup app state
        struct android_app* state = reinterpret_cast<struct android_app*>(ms_app);
        pthread_mutex_lock(&state->mutex);
        state->onAppCmd = nullptr;
        state->onInputEvent = nullptr;
        pthread_mutex_unlock(&state->mutex);

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

        if (Application::getState() == (STATE_APP_RUNNING | STATE_APP_PLAYING | STATE_WINDOW_INIT | STATE_WINDOW_FOCUS)) {
            if (!(Application::getState() & STATE_WINDOW_STARTED)) {
                if (Application::getActivity()) {
                    if (Application::getActivity()->onStart() != 0) {
                        // error we want to destroy the application
                        ANativeActivity_finish(state->activity);
                    }
                }

                Application::setState(Application::getState() | STATE_WINDOW_STARTED);
            }
        } else if ((Application::getState() & STATE_APP_RUNNING) && !(Application::getState() & STATE_WINDOW_INIT)) {
            if (Application::getState() & STATE_WINDOW_STARTED) {
                if (Application::getActivity()) {
                    if (Application::getActivity()->onStop() != 0) {
                        // error we want to destroy the application
                        ANativeActivity_finish(state->activity);
                    }
                }

                // destroy any remaining app window
                for (IT_AppWindowMap it = ms_appWindowMap.begin(); it != ms_appWindowMap.end(); ++it) {
                    delete it->second;
                }
                ms_appWindowMap.clear();

                Application::setState(Application::getState() & ~STATE_WINDOW_STARTED);
            }

            // dont waste the CPU until completeness
            System::waitMs(2);

        } else if (!(Application::getState() & STATE_APP_RUNNING)) {
            // break the loop when destroy was received
            if (!EvtManager::instance()->isPendingEvent()) {
                EvtManager::instance()->processEvent();
            }

            // destroy then out of the main loop
            break;
        }

        // process update/paint event if necessary for each window
        for (IT_AppWindowMap it = ms_appWindowMap.begin(); it != ms_appWindowMap.end(); ++it) {
            // only if started/resumed
            if (it->second->isRunning() && (Application::getState() & STATE_APP_PLAYING)) {
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

Int32 Application::startPrivate()
{
    // does nothing (wait run)
    return 0;
}

Int32 Application::stopPrivate()
{
    // does nothing (done during run)
    return 0;
}

#endif // O3D_ANDROID
