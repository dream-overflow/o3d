/**
 * @file applicationsdl.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/application.h"

// IF O3D_SDL IS SELECTED
#ifdef O3D_SDL

#include "o3d/core/architecture.h"
#include "o3d/core/debug.h"
#include "o3d/core/appwindow.h"

// Specific dependencies includes
#include <SDL2/SDL.h>

using namespace o3d;

void Application::apiInit()
{
	// SDL init with video and thread modules. You can un-comment the flag on X11 systems
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
		O3D_ERROR(E_InvalidResult("SDL_Init return -1"));
}

void Application::apiQuit()
{
	SDL_Quit();
}

// Run the application main loop.
void Application::run()
{
	Bool quit = False;
	SDL_Event event;
	AppWindow::EventData eventData;

	while (!quit || EvtManager::instance()->isPendingEvent())
	{
        EvtManager::instance()->processEvent();

		ms_currAppWindow = NULL;

		if (SDL_WaitEventTimeout(&event, 2)) do
		{
			switch (event.type)
			{
				case SDL_QUIT:
					quit = True;
					break;

				case SDL_WINDOWEVENT:
				{
					ms_currAppWindow = getAppWindow(event.window.windowID);

					if (!ms_currAppWindow)
						break;

					SDL_Window *window = (SDL_Window*)ms_currAppWindow->getHDC();

					switch (event.window.event)
					{
						//case SDL_WINDOWEVENT_HIDDEN:

						//	break;

						case SDL_WINDOWEVENT_CLOSE:
							ms_currAppWindow->processEvent(AppWindow::EVT_CLOSE, eventData);
							break;

						case SDL_WINDOWEVENT_MOVED:
							eventData.x = event.window.data1;
							eventData.y = event.window.data2;

							ms_currAppWindow->processEvent(AppWindow::EVT_MOVE, eventData);
							break;

						case SDL_WINDOWEVENT_RESIZED:
							eventData.w = event.window.data1;
							eventData.h = event.window.data2;

							ms_currAppWindow->processEvent(AppWindow::EVT_RESIZE, eventData);
							break;

						case SDL_WINDOWEVENT_MINIMIZED:
							eventData.w = 0;
							eventData.h = 0;

							ms_currAppWindow->processEvent(AppWindow::EVT_MINIMIZE, eventData);
							break;

						case SDL_WINDOWEVENT_MAXIMIZED:
							SDL_GetWindowSize(window, &eventData.w, &eventData.h);
							ms_currAppWindow->processEvent(AppWindow::EVT_MAXIMIZE, eventData);
							break;

						case SDL_WINDOWEVENT_RESTORED:
							SDL_GetWindowSize(window, &eventData.w, &eventData.h);
							ms_currAppWindow->processEvent(AppWindow::EVT_RESTORE, eventData);
							break;

						case SDL_WINDOWEVENT_EXPOSED:
							ms_currAppWindow->processEvent(AppWindow::EVT_UPDATE, eventData);
							ms_currAppWindow->processEvent(AppWindow::EVT_PAINT, eventData);
							break;

						case SDL_WINDOWEVENT_ENTER:
							ms_currAppWindow->processEvent(AppWindow::EVT_MOUSE_GAIN, eventData);
							break;
						case SDL_WINDOWEVENT_LEAVE:
							ms_currAppWindow->processEvent(AppWindow::EVT_MOUSE_LOST, eventData);
							break;

						case SDL_WINDOWEVENT_FOCUS_GAINED:
							ms_currAppWindow->processEvent(AppWindow::EVT_INPUT_FOCUS_GAIN, eventData);
							break;
						case SDL_WINDOWEVENT_FOCUS_LOST:
							ms_currAppWindow->processEvent(AppWindow::EVT_INPUT_FOCUS_LOST, eventData);
							break;
						}
					}
					break;

				case SDL_MOUSEBUTTONDOWN:

					ms_currAppWindow = getAppWindow(event.button.windowID);
					if (ms_currAppWindow)
					{
						eventData.button = event.button.button;
						ms_currAppWindow->processEvent(AppWindow::EVT_MOUSE_BUTTON_DOWN, eventData);
					}
					break;

				case SDL_MOUSEBUTTONUP:

					ms_currAppWindow = getAppWindow(event.button.windowID);
					if (ms_currAppWindow)
					{
						eventData.button = event.button.button;
						ms_currAppWindow->processEvent(AppWindow::EVT_MOUSE_BUTTON_UP, eventData);
					}
					break;

				case SDL_MOUSEMOTION:

					ms_currAppWindow = getAppWindow(event.motion.windowID);
					if (ms_currAppWindow)
					{
						eventData.x = event.motion.xrel;
						eventData.y = event.motion.yrel;
						ms_currAppWindow->processEvent(AppWindow::EVT_MOUSE_MOTION, eventData);
					}
					break;

				case SDL_MOUSEWHEEL:

					ms_currAppWindow = getAppWindow(event.wheel.windowID);
					if (ms_currAppWindow)
					{
						eventData.x = event.wheel.y;
						ms_currAppWindow->processEvent(AppWindow::EVT_MOUSE_WHEEL, eventData);
					}
					break;

				case SDL_KEYDOWN:
					ms_currAppWindow = getAppWindow(event.key.windowID);
					if (ms_currAppWindow)
					{
						eventData.key = event.key.keysym.sym;
						ms_currAppWindow->processEvent(AppWindow::EVT_KEYDOWN, eventData);
					}
					break;

				case SDL_KEYUP:
					ms_currAppWindow = getAppWindow(event.key.windowID);
					if (ms_currAppWindow)
					{
						eventData.key = event.key.keysym.sym;
						ms_currAppWindow->processEvent(AppWindow::EVT_KEYUP, eventData);
					}
					break;

				case SDL_TEXTINPUT:
					ms_currAppWindow = getAppWindow(event.text.windowID);
					if (ms_currAppWindow)
					{
						if (event.text.text[0] != 0)
						{
							String text;
							text.fromUtf8(event.text.text);

                            // we dont have scancode here
							eventData.unicode = text.toWChar(0);//text[0];
							ms_currAppWindow->processEvent(AppWindow::EVT_CHARDOWN, eventData);
						}
					}
					break;

				case SDL_TEXTEDITING:
					ms_currAppWindow = getAppWindow(event.edit.windowID);
					if (ms_currAppWindow)
					{
						if (event.edit.text[0] != 0)
						{
							String text;
							text.fromUtf8(event.text.text);

                            // we dont have scancode here
							eventData.unicode = text.toWChar(0);//text[0];
							ms_currAppWindow->processEvent(AppWindow::EVT_CHARDOWN, eventData);
						}
					}
					break;

				case SDL_USEREVENT:
					// 0xFFFE mean try quit
					if (event.user.code == 0xfffe)
					{
						removeAppWindow(event.user.windowID);
					}
					// 0xFFFF mean event manager events to process
					else if (event.user.code == 0xffff)
						EvtManager::instance()->processEvent();
					// negative mean timer event code
					else if (event.user.code < 0)
					{
						TimerManager::instance()->callTimer(
								reinterpret_cast<Timer*> (event.user.data1));
					}
					break;

				default:
					break;
			}
		} while (SDL_PollEvent(&event));

		// process update/paint event if necessary for each window
		for (IT_AppWindowMap it = ms_appWindowMap.begin(); it != ms_appWindowMap.end(); ++it)
		{
			if (it->second->isRunning())
			{
				it->second->processEvent(AppWindow::EVT_UPDATE, eventData);
				it->second->processEvent(AppWindow::EVT_PAINT, eventData);
			}
		}

		if (ms_appWindowMap.empty())
		{
			SDL_Event event;
			event.type = SDL_QUIT;
			SDL_PushEvent(&event);
		}
	}
}

// Push a user application event.
void Application::pushEvent(Int32 type, _HWND hWnd, void *data)
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.windowID = hWnd;
	event.user.code = type;
	event.user.data1 = data;
	event.user.data2 = NULL;

	SDL_PushEvent(&event);
}

#endif // O3D_SDL

