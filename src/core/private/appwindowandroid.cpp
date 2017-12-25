/**
 * @file appwindowandroid.cpp
 * @brief
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2017-12-09
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/appwindow.h"

#include "o3d/core/timer.h"

// ONLY IF O3D_ANDROID IS SELECTED
#ifdef O3D_ANDROID

#include "o3d/core/application.h"
#include "o3d/core/debug.h"
#include "o3d/core/display.h"

//#ifdef O3D_EGL
#include "o3d/core/private/egldefines.h"
#include "o3d/core/private/egl.h"
//#endif

#include "android/android_native_app_glue.h"

using namespace o3d;

// Window settings
void AppWindow::setTitle(const String &title)
{
    // if window created
    m_title = title;

    if (m_hWnd) {
        // @todo
    }
}

void AppWindow::setIcon(const Image &icon)
{
    // @todo

    m_icon = icon;
}

void AppWindow::setSize(Int32 width, Int32 height)
{
    m_width = width;
    m_height = height;

    m_clientWidth = width;
    m_clientHeight = height;
}

// Resize window
void AppWindow::resize(Int32 clientWidth, Int32 clientHeight)
{
    if (m_hWnd) {
        // @todo when multiple activities

        m_width = m_clientWidth = clientWidth;
        m_height = m_clientHeight = clientHeight;

        callBackResize();
    } else {
        m_width = m_clientWidth = clientWidth;
        m_height = m_clientHeight = clientHeight;
    }
}

void AppWindow::setMinSize(const Vector2i &minSize)
{
    m_minSize = minSize;

    if (m_hWnd && m_resizable) {
        // @todo when multiple activities
    }
}

void AppWindow::setMaxSize(const Vector2i &maxSize)
{
    m_maxSize = maxSize;

    if (m_hWnd && m_resizable) {
        // @todo when multiple activities
    }
}

// Apply window settings
void AppWindow::applySettings(Bool fullScreen)
{
    if (!isSet()) {
        O3D_ERROR(E_InvalidPrecondition("Window not set"));
    }

    struct android_app *appState = reinterpret_cast<android_app*>(Application::getApp());
    if (!appState || !appState->window) {
        O3D_ERROR(E_InvalidPrecondition("Invalid application state or window"));
    }

    int r = 8, g = 8, b = 8, a = 8;

    switch (m_colorFormat) {
    case COLOR_RGBA4:
        r = g = b = 4;
        a = 4;
        break;
    case COLOR_RGB5A1:
        r = g = b = 5;
        a = 1;
        break;
    case COLOR_RGB565:
        r = b = 5;
        g = 6;
        a = 0;
        break;
    case COLOR_RGB8:
        r = g = b = 8;
        a = 0;
        break;
    case COLOR_RGBA8:
        r = g = b = 8;
        a = 8;
        break;
    case COLOR_RGB16:
        r = g = b = 16;
        a = 0;
        break;
    case COLOR_RGBA16:
        r = g = b = 16;
        a = 16;
        break;
    case COLOR_RGB32:
        r = g = b = 32;
        a = 0;
        break;
    case COLOR_RGBA32:
        r = g = b = 32;
        a = 32;
        break;
    default:
        break;
    }

    if (GL::getImplementation() == GL::IMPL_EGL_15) {
    #ifdef O3D_EGL
        EGLint apiType = EGL_OPENGL_BIT;
        if (GL::getType() == GL::API_GLES_3) {
            apiType = EGL_OPENGL_ES2_BIT;  // not 3 defined in Android
        }

        // Get a matching config, double buffer is a default with EGL on Surface
        EGLint configAttributes[] = {
            EGL_BUFFER_SIZE, 0,
            EGL_RED_SIZE, r,
            EGL_GREEN_SIZE, g,
            EGL_BLUE_SIZE, b,
            EGL_ALPHA_SIZE, a,
            EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
            EGL_DEPTH_SIZE, (EGLint)getDepth(),
            EGL_LEVEL, 0,
            EGL_RENDERABLE_TYPE, apiType,
            EGL_SAMPLE_BUFFERS, m_samples == NO_MSAA ? 0 : 1,
            EGL_SAMPLES, (EGLint)m_samples,
            EGL_STENCIL_SIZE, (EGLint)getStencil(),
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_TRANSPARENT_TYPE, EGL_NONE,
            EGL_TRANSPARENT_RED_VALUE, EGL_DONT_CARE,
            EGL_TRANSPARENT_GREEN_VALUE, EGL_DONT_CARE,
            EGL_TRANSPARENT_BLUE_VALUE, EGL_DONT_CARE,
            EGL_CONFIG_CAVEAT, EGL_DONT_CARE,
            EGL_CONFIG_ID, EGL_DONT_CARE,
            EGL_MAX_SWAP_INTERVAL, EGL_DONT_CARE,
            EGL_MIN_SWAP_INTERVAL, EGL_DONT_CARE,
            EGL_NATIVE_RENDERABLE, EGL_DONT_CARE,
            EGL_NATIVE_VISUAL_TYPE, EGL_DONT_CARE,
            EGL_NONE
        };

        EGLDisplay eglDisplay = EGL::getDisplay(reinterpret_cast<EGLNativeDisplayType>(Application::getDisplay()));
        // EGLDisplay eglDisplay = EGL::getDisplay(EGL_DEFAULT_DISPLAY);

        // Selection of the visual config
        EGLint numConfigs;

        if (!EGL::chooseConfig(eglDisplay, configAttributes, nullptr, 1, &numConfigs) || !numConfigs) {
            O3D_ERROR(E_InvalidResult("Failed to retrieve a frame buffer config"));
        }

        EGLConfig *eglConfigs = new EGLConfig[numConfigs];

        if (!EGL::chooseConfig(eglDisplay, configAttributes, eglConfigs, numConfigs, &numConfigs)) {
            deleteArray(eglConfigs);
            O3D_ERROR(E_InvalidResult("EGL get configs returns false"));
        }

        // Pick the FB config with the most samples per pixel
        int bestFBC = -1, worstFBC = -1, bestNumSamp = -1, worstNumSamp = 999;

        for (EGLint i = 0; i < numConfigs; i++) {
            EGLint sampBuf, samples, visualId;

            EGL::getConfigAttrib(eglDisplay, eglConfigs[i], EGL_NATIVE_VISUAL_ID, &visualId);
            EGL::getConfigAttrib(eglDisplay, eglConfigs[i], EGL_SAMPLE_BUFFERS, &sampBuf);
            EGL::getConfigAttrib(eglDisplay, eglConfigs[i], EGL_SAMPLES, &samples);

            O3D_MESSAGE(String::print(
                            "Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d, SAMPLES = %d",
                            i,
                            visualId,
                            sampBuf,
                            samples));

            if (((bestFBC < 0) || sampBuf) && (samples > bestNumSamp)) {
                bestFBC = i;
                bestNumSamp = samples;
            }

            if ((worstFBC < 0) || !sampBuf || (samples < worstNumSamp)) {
                worstFBC = i;
                worstNumSamp = samples;
            }
        }

        bestFBC = 0;
        EGLint visualId;
        EGLConfig eglConfig = eglConfigs[bestFBC];
        EGL::getConfigAttrib(eglDisplay, eglConfigs[bestFBC], EGL_NATIVE_VISUAL_ID, &visualId);
        deleteArray(eglConfigs);

        O3D_MESSAGE(String::print("Chosen visual ID = 0x%x", visualId));

        // Create a window surface (EGL_RENDER_BUFFER is default to EGL_BACK_BUFFER if possible)
        EGLint surfaceAttributes[] = { EGL_NONE };
        EGLSurface eglSurface = EGL::createWindowSurface(eglDisplay, eglConfig, appState->window, surfaceAttributes);

        if (eglSurface == EGL_NO_SURFACE) {
            O3D_ERROR(E_InvalidResult("Failed to create a GLES surface"));
        }

        EGLint w, h;

        EGL::querySurface(eglDisplay, eglSurface, EGL_WIDTH, &w);
        EGL::querySurface(eglDisplay, eglSurface, EGL_HEIGHT, &h);

        m_clientWidth = w;
        m_clientHeight = h;

        m_hWnd = reinterpret_cast<_HWND>(appState->window);
        m_HDC = reinterpret_cast<_HDC>(eglSurface);
        m_PF = reinterpret_cast<_PF>(eglConfig);
    #else
        O3D_ERROR(E_UnsuportedFeature("Support for EGL is missing"));
    #endif
    } else {
        O3D_ERROR(E_UnsuportedFeature("Support for EGL only"));
    }

    if (fullScreen) {
        // @todo if multiple activities at time
    }

    if (m_resizable && !fullScreen) {
        // @todo if multiple activities at time
    }

    // min size hint
    if (m_minSize.x() >= 0 && m_minSize.y() >= 0) {
        // @todo if multiple activities at time
    }

    // max size hint
    if (m_maxSize.x() >= 0 && m_maxSize.y() >= 0) {
        // @todo if multiple activities at time
    }

    // get the current window pos
    m_posX = m_posY = 0;

    // window size
    m_width = m_clientWidth;
    m_height = m_clientHeight;

    setIcon(m_icon);

    Application::addAppWindow(this);

    // create event
    callBackCreate();

    m_running = True;
}

// Destroy the window.
void AppWindow::destroy()
{
    // delete inputs
    m_inputManager.destroy();

    m_icon.destroy();

    if (m_HDC != NULL_HDC) {
        if (isFullScreen()) {
            Display::instance()->restoreDisplayMode();
        }

        EventData event;
        processEvent(EVT_DESTROY, event);

        // @todo

        m_hWnd = NULL_HWND;
        m_HDC = NULL_HDC;
        m_PF = NULL_PF;
    }

    m_running = False;
}

Bool AppWindow::pasteToClipboard(const String &text, Bool primary)
{
    if (!m_hWnd) {
        O3D_ERROR(E_InvalidOperation("The window must be valid"));
    }

    // nothing to paste
    if (text.isEmpty()) {
        return False;
    }

    CString utf8 = text.toUtf8();
    int result = 0;

    return result == 0;
}

String AppWindow::copyFromClipboard(Bool primary)
{
    if (!m_hWnd) {
        O3D_ERROR(E_InvalidOperation("The window must be valid"));
    }

    String text;  // result

    // @todo

    return text;
}

// Set to full screen
void AppWindow::setFullScreen(Bool fullScreen, UInt32 freq)
{
    if (m_hWnd != NULL_HWND) {
        O3D_ERROR(E_InvalidOperation("The window must be valid"));
    }

    // nothing
}

// Swap buffers
void AppWindow::swapBuffers()
{
    if (m_running) {
        GL::swapBuffers(Application::getDisplay(), m_hWnd, m_HDC);
    }
}

// Process internals deferred events
void AppWindow::processEvent(EventType eventType, EventData &eventData)
{
    if (!m_running) {
        return;
    }

    switch (eventType) {
        case EVT_CREATE:
            callBackCreate();
            break;
        case EVT_CLOSE:
            callBackClose();
            break;
        case EVT_DESTROY:
            callBackDestroy();
            Application::pushEvent(Application::EVENT_CLOSE_WINDOW, m_hWnd, nullptr);
            break;

        case EVT_UPDATE:
            if (isUpdateNeeded()) {
                callBackUpdate(0);
            }
            break;
        case EVT_PAINT:
            if (isPaintNeeded()) {
                callBackPaint(0);
            }
            break;

        case EVT_MOVE:
            m_posX = eventData.x;
            m_posY = eventData.y;
            callBackMove();
            break;

        case EVT_MAXIMIZE:
            m_width = m_clientWidth = eventData.w;
            m_height = m_clientHeight = eventData.h;
            callBackMaximize();
            break;
        case EVT_MINIMIZE:
            m_width = m_clientWidth = eventData.w;
            m_height = m_clientHeight = eventData.h;
            callBackMinimize();
            break;
        case EVT_RESTORE:
            m_width = m_clientWidth = eventData.w;
            m_height = m_clientHeight = eventData.h;
            callBackRestore();
            break;
        case EVT_RESIZE:
            m_width = m_clientWidth = eventData.w;
            m_height = m_clientHeight = eventData.h;
            callBackResize();
            break;

        case EVT_INPUT_FOCUS_GAIN:
            m_inputManager.getKeyboard()->acquire();
            callBackFocus();
            break;
        case EVT_INPUT_FOCUS_LOST:
            m_inputManager.getKeyboard()->release();
            callBackLostFocus();
            break;

        case EVT_MOUSE_GAIN:
            m_inputManager.getMouse()->acquire();
            callBackMouseGain();
            break;
        case EVT_MOUSE_LOST:
            m_inputManager.getMouse()->release();
            callBackMouseLost();
            break;

        case EVT_MOUSE_BUTTON_DOWN:
            {
/*                Bool dblClick;

                switch (eventData.button) {
                    // left button
                    case Button1:
                        dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::LEFT, True);
                        callBackMouseButton(Mouse::LEFT, True, dblClick);
                        break;

                        // right button
                    case Button3:
                        dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::RIGHT, True);
                        callBackMouseButton(Mouse::RIGHT, True, dblClick);
                        break;

                        // middle button
                    case Button2:
                        dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::MIDDLE, True);
                        callBackMouseButton(Mouse::MIDDLE, True, dblClick);
                        break;

                        // X1 button
                    case 8:
                        dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::X1, True);
                        callBackMouseButton(Mouse::X1, True, dblClick);
                        break;

                        // X2 button
                    case 9:
                        dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::X2, True);
                        callBackMouseButton(Mouse::X2, True, dblClick);
                        break;

                        // Wheel up
                    case Button4:
                        m_inputManager.getMouse()->setMouseWheel(-120);
                        callBackMouseWheel();
                        break;

                    case Button5:
                        m_inputManager.getMouse()->setMouseWheel(120);
                        callBackMouseWheel();
                        break;

                    default:
                        break;
                }*/
            }
            break;
        case EVT_MOUSE_BUTTON_UP:
         /*   switch (eventData.button) {
                // left button
                case Button1:
                    m_inputManager.getMouse()->setMouseButton(Mouse::LEFT, False);
                    callBackMouseButton(Mouse::LEFT, False, False);
                    break;

                    // right button
                case Button3:
                    m_inputManager.getMouse()->setMouseButton(Mouse::RIGHT, False);
                    callBackMouseButton(Mouse::RIGHT, False, False);
                    break;

                    // middle button
                case Button2:
                    m_inputManager.getMouse()->setMouseButton(Mouse::MIDDLE, False);
                    callBackMouseButton(Mouse::MIDDLE, False, False);
                    break;

                    // X1 button
                case 8:
                    m_inputManager.getMouse()->setMouseButton(Mouse::X1, False);
                    callBackMouseButton(Mouse::X1, False, False);
                    break;

                    // X2 button
                case 9:
                    m_inputManager.getMouse()->setMouseButton(Mouse::X2, False);
                    callBackMouseButton(Mouse::X2, False, False);
                    break;

                default:
                    break;
            }*/
            break;
        case EVT_MOUSE_MOTION:
            if (eventData.x || eventData.y) {
                //m_inputManager.Mouse()->setMouseDelta(eventData.x, eventData.y);
                m_inputManager.getMouse()->setMousePos(eventData.x, eventData.y);
                callBackMouseMotion();
            }
            break;
            //case EVT_MOUSE_WHEEL:
            //	m_inputManager.Mouse()->setMouseWheel(eventData.x);
            //	callBackMouseWheel();
            //	break;

        case EVT_KEYDOWN:
            {
                VKey vkey = VKey(eventData.key);
                VKey character = VKey(eventData.character);

                Bool pressed = m_inputManager.getKeyboard()->setKeyState(vkey, True);

                // if not pressed we have a key repeat by the system

                callBackKey(vkey, character, pressed, !pressed);

                // some characters that are not recognized on text input event
                /*if (key == KEY_BACKSPACE) {  // OK for \b
                    callBackCharacter(key, character, static_cast<WChar>('\b'), !pressed);
                } else*/ if ((vkey == KEY_NUMPAD_ENTER) || (vkey == KEY_RETURN)) {
                    callBackCharacter(vkey, character, static_cast<WChar>('\n'), !pressed);
                /*} else if (key == KEY_DELETE) {  // OK for delete
                    callBackCharacter(key, character, static_cast<WChar>(127), !pressed);*/
                } else if (vkey == KEY_TAB) {
                    callBackCharacter(vkey, character, static_cast<WChar>('\t'), !pressed);
                }

                // an unicode character is compound
                if (eventData.unicode != 0) {
                    callBackCharacter(
                            vkey,
                            character,
                            static_cast<WChar> (eventData.unicode),
                            !pressed);
                }
            }
            break;
        case EVT_KEYUP:
            {
                VKey vkey = VKey(eventData.key);
                VKey character = VKey(eventData.character);

                m_inputManager.getKeyboard()->setKeyState(vkey, False);

                callBackKey(vkey, character, False, False);
            }
            break;
        case EVT_CHARDOWN:
            {
                VKey vkey = VKey(eventData.key);
                VKey character = VKey(eventData.character);

                Bool repeat = m_inputManager.getKeyboard()->isKeyDown(vkey);
                callBackCharacter(vkey, character, static_cast<WChar>(eventData.unicode), repeat);
            }
            break;

        default:
            break;
    }
}

void AppWindow::processMotionEvent(EventType eventType, MotionEventData &eventData)
{
    switch(eventType) {
        case EVT_TOUCH_POINTER_DOWN:
            if (m_inputManager.isInput(Input::INPUT_TOUCHSCREEN)) {
                for (UInt32 i = 0; i < eventData.array.size(); ++i) {
                    m_inputManager.getTouchScreen()->setPointerState(
                            i,
                            True,
                            eventData.array[i].x,
                            eventData.array[i].y,
                            eventData.array[i].p,
                            eventData.time);
                }

                callBackTouchScreenChange(TouchScreenEvent::TYPE_POINTER_DOWN, eventData);
            }
            break;
        case EVT_TOUCH_MOVE:
            if (m_inputManager.isInput(Input::INPUT_TOUCHSCREEN)) {
                for (UInt32 i = 0; i < eventData.array.size(); ++i) {
                    m_inputManager.getTouchScreen()->setPosition(
                            i,
                            eventData.array[i].x,
                            eventData.array[i].y,
                            eventData.array[i].p,
                            eventData.time);
                }

                callBackTouchScreenMotion();
            }
            break;
        case EVT_TOUCH_POINTER_UP:
            if (m_inputManager.isInput(Input::INPUT_TOUCHSCREEN)) {
                for (UInt32 i = 0; i < eventData.array.size(); ++i) {
                    m_inputManager.getTouchScreen()->setPointerState(
                            i,
                            False,
                            eventData.array[i].x,
                            eventData.array[i].y,
                            eventData.array[i].p,
                            eventData.time);
                }

                callBackTouchScreenChange(TouchScreenEvent::TYPE_POINTER_UP, eventData);
            }
            break;

        default:
            break;
    }
}

#endif // O3D_ANDROID
