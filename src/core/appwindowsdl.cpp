/**
 * @file appwindowsdl.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/appwindow.h"

#include "o3d/core/timer.h"

// ONLY IF O3D_SDL2 IS SELECTED
#ifdef O3D_SDL2

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#include "o3d/core/application.h"
#include "o3d/core/debug.h"
#include "o3d/core/video.h"

using namespace o3d;

// Window settings
void AppWindow::setTitle(const String &title)
{
	// if window created
	m_title = title;

    if (m_HDC != NULL_HDC) {
		SDL_SetWindowTitle(
				reinterpret_cast<SDL_Window*>(m_HDC),
				m_title.toUtf8().getData());
    }
}

void AppWindow::setIcon(const Image &icon)
{
    if ((icon.getPixelFormat() != PF_RGB_U8) && (icon.getPixelFormat() != PF_RGBA_U8)) {
		O3D_ERROR(E_InvalidFormat("Icon must be PF_RGB_U8 or PF_RGBA_U8"));
    }

    if ((m_HDC != NULL_HDC) && icon.isValid()) {
		Image localPicture(icon);

		// get the first pixel as color key
		Rgb colorKey = icon.getRgbPixel(0, 0);
		localPicture.convertToRGBA(colorKey);

		void *data = SDL_malloc(localPicture.getSize());
		memcpy(data, localPicture.getData(), localPicture.getSize());

		SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
				data,
				localPicture.getWidth(),
				localPicture.getHeight(),
				localPicture.getBpp(),
				localPicture.getPitch(),
				0x000000ff,
				0x0000ff00,
				0x00ff0000,
				0xff000000);

		UInt32 sdlColorkey;

		// Get the first pixel color as transparent code
        if (icon.getPixelFormat() == PF_RGB_U8) {
			//UInt8 r, g, b;

			//SDL_GetRGB(0, surface->format, &r, &g, &b);
			//sdlColorkey = SDL_MapRGB(surface->format, r, g, b);

			sdlColorkey = SDL_MapRGB(
					surface->format,
					(UInt8)o3d::red(colorKey),
					(UInt8)o3d::green(colorKey),
					(UInt8)o3d::blue(colorKey));

			SDL_SetColorKey(surface, 1, sdlColorkey);
        } else {
			SDL_SetColorKey(surface, 0, 0);
        }

		// set icon
		SDL_SetWindowIcon(reinterpret_cast<SDL_Window*>(m_HDC), surface);
		SDL_FreeSurface(surface);
    } else if (m_hWnd) {
		// remove icon
		SDL_SetWindowIcon(reinterpret_cast<SDL_Window*>(m_HDC), NULL);
	}

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
    if (m_HDC != NULL_HDC) {
		// window is in fullscreen state
        if (Video::instance()->getAppWindow() == this) {
			VideoMode videoMode;
			videoMode.width = clientWidth;
			videoMode.height = clientHeight;
            videoMode.bpp = getBpp();
			videoMode.freq = 0;

			CIT_VideoModeList cit = Video::instance()->findDisplayMode(videoMode);
            if (cit == Video::instance()->end()) {
				O3D_ERROR(E_InvalidParameter("Cannot resize a full screen window with this width/height"));
            }

			Video::instance()->setDisplayMode(this, cit);

			SDL_SetWindowSize(reinterpret_cast<SDL_Window*>(m_HDC), clientWidth, clientHeight);
			SDL_SetWindowPosition(reinterpret_cast<SDL_Window*>(m_HDC), 0, 0);

			m_width = m_clientWidth = clientWidth;
			m_height = m_clientHeight = clientHeight;
        } else {
			SDL_SetWindowSize(reinterpret_cast<SDL_Window*>(m_HDC), clientWidth, clientHeight);
			SDL_SetWindowPosition(
				reinterpret_cast<SDL_Window*>(m_HDC),
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED);

			m_width = clientWidth;
			m_height = clientHeight;

			m_clientWidth = clientWidth;
			m_clientHeight = clientHeight;
		}

		callBackResize();
    } else {
		m_clientWidth = clientWidth;
		m_clientHeight = clientHeight;
	}
}

void AppWindow::setMinSize(const Vector2i &minSize)
{
	m_minSize = minSize;

	// min size hint
    if ((m_HDC != nullptr) && (m_minSize.x() >= 0) && (m_minSize.y() >= 0)) {
		SDL_SetWindowMinimumSize(reinterpret_cast<SDL_Window*>(m_HDC), m_minSize.x(), m_minSize.y());
    }
}

void AppWindow::setMaxSize(const Vector2i &maxSize)
{
	m_maxSize = maxSize;

	// max size hint
    if ((m_HDC != nullptr) && (m_maxSize.x() >= 0) && (m_maxSize.y() >= 0)) {
		SDL_SetWindowMaximumSize(reinterpret_cast<SDL_Window*>(m_HDC), m_maxSize.x(), m_maxSize.y());
    }
}

// Apply window settings
void AppWindow::applySettings(Bool fullScreen)
{
    if(!isSet()) {
		O3D_ERROR(E_InvalidPrecondition("Window not set"));
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

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, r);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, g);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, b);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, a);

    // MSAA
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, m_samples == NO_MSAA ? 0 : 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, (int)m_samples);

	// always need double-buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// set depth buffer size
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, getDepth());

	// 8 bits stencil buffer size
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, getStencil());

	// VSync
	//SDL_GL_SetSwapInterval(1);

	Int32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

    if (fullScreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
    }

    if (m_resizable && !fullScreen) {
		flags |= SDL_WINDOW_RESIZABLE;
    }

	SDL_Window *window = SDL_CreateWindow(
			m_title.toUtf8().getData(),
			fullScreen ? 0 : SDL_WINDOWPOS_CENTERED,
			fullScreen ? 0 : SDL_WINDOWPOS_CENTERED,
	        m_clientWidth,
	        m_clientHeight,
	        flags);

    if (!window) {
		O3D_ERROR(E_InvalidResult("Unable to create the window"));
    }

	// min size hint
    if (m_minSize.x() >= 0 && m_minSize.y() >= 0) {
		SDL_SetWindowMinimumSize(window, m_minSize.x(), m_minSize.y());
    }

	// max size hint
    if (m_maxSize.x() >= 0 && m_maxSize.y() >= 0) {
		SDL_SetWindowMaximumSize(window, m_maxSize.x(), m_maxSize.y());
    }

	// get the current window pos
	SDL_GetWindowPosition(window, &m_posX, &m_posY);

	// window size
	m_width = m_clientWidth;
	m_height = m_clientHeight;

	m_PF = (_PF)SDL_GetWindowPixelFormat(window);
	m_HDC = (_HDC)window;
	m_hWnd = (_HWND)SDL_GetWindowID(window);

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
			Video::instance()->restoreDisplayMode();
        }

		EventData event;
		processEvent(EVT_DESTROY, event);

		SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(m_HDC));

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
    int result = SDL_SetClipboardText(utf8.getData());

    return result == 0;
}

String AppWindow::copyFromClipboard(Bool primary)
{
    if (!m_hWnd) {
        O3D_ERROR(E_InvalidOperation("The window must be valid"));
    }

    String text;  // result

    if (SDL_HasClipboardText()) {
        char *buffer = SDL_GetClipboardText();
        if (buffer) {
            text.fromUtf8(buffer);
            SDL_free(charResult);
        }
    }

    return text;
}

// Set to full screen
void AppWindow::setFullScreen(Bool fullScreen, UInt32 freq)
{
    if (m_hWnd != NULL_HWND) {
		O3D_ERROR(E_InvalidOperation("The window must be valid"));
    }

    if ((Video::instance()->getAppWindow() != nullptr) &&
        (Video::instance()->getAppWindow() != this)) {

		O3D_ERROR(E_InvalidOperation("Another window is currently taking the fullscreen"));
	}

    if (fullScreen && (Video::instance()->getAppWindow() == nullptr)) {
		VideoMode videoMode;
		videoMode.width = m_clientWidth;
		videoMode.height = m_clientHeight;
        videoMode.bpp = getBpp();
		videoMode.freq = 0;

		CIT_VideoModeList cit = Video::instance()->findDisplayMode(videoMode);
        if (cit != Video::instance()->end()) {
			Video::instance()->setDisplayMode(this, cit);
        } else {
			O3D_ERROR(E_InvalidParameter("Invalid video mode"));
        }

		SDL_SetWindowSize(reinterpret_cast<SDL_Window*>(m_HDC), m_clientWidth, m_clientHeight);
		SDL_SetWindowPosition(reinterpret_cast<SDL_Window*>(m_HDC), 0, 0);

		m_posX = m_posY = 0;
    } else if (!fullScreen && (Video::instance()->getAppWindow() == this)) {
		Video::instance()->restoreDisplayMode();

		SDL_SetWindowSize(reinterpret_cast<SDL_Window*>(m_HDC), m_clientWidth, m_clientHeight);
		SDL_SetWindowPosition(
				reinterpret_cast<SDL_Window*>(m_HDC),
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED);

		SDL_GetWindowPosition(reinterpret_cast<SDL_Window*>(m_HDC), &m_posX, &m_posY);
	}
}

// Swap buffers
void AppWindow::swapBuffers()
{
    if (m_running) {
		SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(m_HDC));
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
				Bool dblClick;

                switch (eventData.button) {
					// left button
					case SDL_BUTTON_LEFT:
						dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::LEFT, True);
						callBackMouseButton(Mouse::LEFT, True, dblClick);
						break;

					// right button
					case SDL_BUTTON_RIGHT:
						dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::RIGHT, True);
						callBackMouseButton(Mouse::RIGHT, True, dblClick);
						break;

					// middle button
					case SDL_BUTTON_MIDDLE:
						dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::MIDDLE, True);
						callBackMouseButton(Mouse::MIDDLE, True, dblClick);
						break;

					// X1 button
					case SDL_BUTTON_X1:
						dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::X1, True);
						callBackMouseButton(Mouse::X1, True, dblClick);
						break;

					// X2 button
					case SDL_BUTTON_X2:
						dblClick = m_inputManager.getMouse()->setMouseButton(Mouse::X2, True);
						callBackMouseButton(Mouse::X2, True, dblClick);
						break;

					default:
						break;
				}
			}
			break;
		case EVT_MOUSE_BUTTON_UP:
            switch (eventData.button) {
				// left button
				case SDL_BUTTON_LEFT:
					m_inputManager.getMouse()->setMouseButton(Mouse::LEFT, False);
					callBackMouseButton(Mouse::LEFT, False, False);
					break;

				// right button
				case SDL_BUTTON_RIGHT:
					m_inputManager.getMouse()->setMouseButton(Mouse::RIGHT, False);
					callBackMouseButton(Mouse::RIGHT, False, False);
					break;

				// middle button
				case SDL_BUTTON_MIDDLE:
					m_inputManager.getMouse()->setMouseButton(Mouse::MIDDLE, False);
					callBackMouseButton(Mouse::MIDDLE, False, False);
					break;

				// X1 button
				case SDL_BUTTON_X1:
					m_inputManager.getMouse()->setMouseButton(Mouse::X1, False);
					callBackMouseButton(Mouse::X1, False, False);
					break;

				// X2 button
				case SDL_BUTTON_X2:
					m_inputManager.getMouse()->setMouseButton(Mouse::X2, False);
					callBackMouseButton(Mouse::X2, False, False);
					break;
			
				default:
					break;
			}
			break;
		case EVT_MOUSE_MOTION:
            if (eventData.x || eventData.y) {
				m_inputManager.getMouse()->setMouseDelta(eventData.x, eventData.y);
				callBackMouseMotion();
			}
			break;
		case EVT_MOUSE_WHEEL:
			m_inputManager.getMouse()->setMouseWheel(eventData.x);
			callBackMouseWheel();
			break;

		case EVT_KEYDOWN:
			{
				VKey key = static_cast<VKey>(eventData.key);
                Bool pressed = m_inputManager.getKeyboard()->setKeyState(key, True);

				// if not pressed we have a key repeat by the system

                callBackKey(key, key, pressed, !pressed);

				// some characters that are not recognized on text input event
				if (key == KEY_BACKSPACE)
                    callBackCharacter(key, key, static_cast<WChar>('\b'), !pressed);
				else if ((key == KEY_NUMPAD_ENTER) || (key == KEY_RETURN))
                    callBackCharacter(key, key, static_cast<WChar>('\n'), !pressed);
				else if (key == KEY_DELETE)
                    callBackCharacter(key, key, static_cast<WChar>(127), !pressed);
				else if (key == KEY_TAB)
                    callBackCharacter(key, key, static_cast<WChar>('\t'), !pressed);
			}
			break;
		case EVT_KEYUP:
			{
				VKey key = static_cast<VKey>(eventData.key);
                m_inputManager.getKeyboard()->setKeyState(key, False);

                callBackKey(key, key, False, False);
			}
			break;
		case EVT_CHARDOWN:
			{
				VKey key = VKey(eventData.key);
				Bool repeat = m_inputManager.getKeyboard()->isKeyDown(key);

                callBackCharacter(key, key, static_cast<WChar>(eventData.unicode), repeat);
			}

		default:
			break;
	}
}

#endif // O3D_SDL2
