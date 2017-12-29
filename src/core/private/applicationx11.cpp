/**
 * @file applicationx11.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/application.h"

#ifdef O3D_X11

#include "o3d/core/appwindow.h"
#include "o3d/core/evtmanager.h"
#include "o3d/core/display.h"

// @note X11 defines somes types that overwrite Bool, True and False
typedef o3d::Bool oBool;

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "o3d/core/private/glxdefines.h"
#include "o3d/core/private/glx.h"

using namespace o3d;

static Bool setDetectableAutoRepeat = False;

static Atom XA_UTF8_STRING = 0;
static Atom XA_CLIPBOARD = 0;
static Atom XA_COMPOUND_TEXT = 0;
static Atom XA_TARGETS = 0;
static Atom O3D_XEVENT = 0;
static Atom XKLAVIER_STATE = 0;
static Atom WM_PROTOCOLS = 0;

// X11 IO error handler
static int o3dXIOErrorHandler(::Display *display)
{
    // throwing an exception here will cause impredictible results
    O3D_WARNING("Fatal X11 IO error");

    Application::throwDisplayError(nullptr);

    // avoid exit
    return 0;
}

static int o3dXErrorHandler(::Display* display, XErrorEvent* error_event)
{
    String what = String::print(
                      "serial:%u error_code:%u request_code:%u minor_code:%u",
                      error_event->serial,
                      error_event->error_code,
                      error_event->request_code,
                      error_event->minor_code);

    O3D_WARNING(what);

    // throwing an exception here will cause impredictible results
    Application::throwDisplayError((void*)error_event);

    // avoid exit
    return 0;
}

void Application::apiInitPrivate()
{
	// because our app is multi-threaded
	XInitThreads();

    ms_display = reinterpret_cast<_DISP>(XOpenDisplay(NULL));

    if (!ms_display) {
		O3D_ERROR(E_InvalidResult("Failed to open a connection on X11 display"));
    }

	// enable detectable auto repeat
    ::Display *display = reinterpret_cast<::Display*>(ms_display);

    // capture errors to avoid application killing
    XSetErrorHandler(o3dXErrorHandler);
    XSetIOErrorHandler(o3dXIOErrorHandler);

	Bool supported_rtrn;
	XkbGetDetectableAutoRepeat(display, &setDetectableAutoRepeat);
	XkbSetDetectableAutoRepeat(display, True, &supported_rtrn);

    // init X atoms
    XA_UTF8_STRING = XInternAtom(display, "UTF8_STRING", True);
    XA_CLIPBOARD = XInternAtom(display, "CLIPBOARD", True);
    XA_COMPOUND_TEXT = XInternAtom(display, "COMPOUND_TEXT", True);
    XA_TARGETS = XInternAtom(display, "TARGETS", True);

    WM_PROTOCOLS = XInternAtom(display, "WM_PROTOCOLS", True);

    XKLAVIER_STATE = XInternAtom(display, "XKLAVIER_STATE", True);

    O3D_XEVENT = XInternAtom(display, "O3D_XEVENT", False);
}

void Application::apiQuitPrivate()
{
	// restore detectable auto repeat to its old state
    ::Display *display = reinterpret_cast<::Display*>(ms_display);

    if (display) {
        Bool supported_rtrn;
        XkbSetDetectableAutoRepeat(display, setDetectableAutoRepeat, &supported_rtrn);
        XCloseDisplay(display);
    }
}

static Int32 interpretUserEvent(XEvent &event, void *& data)
{
	data = reinterpret_cast<void*>(
			static_cast<UInt64>(event.xclient.data.l[0] << 32) |
			static_cast<UInt64>(event.xclient.data.l[1]));

    return event.xclient.data.l[4];
}

static Int32 pending(::Display *display, UInt32 timeout)
{
	UInt32 sTime = System::getMsTime();

	for(;;)	{
		XFlush(display);
		if (XEventsQueued(display, QueuedAlready)) {
			return 1;
		}

        // More drastic measures are required. see if X is ready to communicate
		{
			static struct timeval zero_time;        // static == 0
			int x11_fd;
			fd_set fdset;

			x11_fd = ConnectionNumber(display);
			FD_ZERO(&fdset);
			FD_SET(x11_fd, &fdset);
			if (select(x11_fd + 1, &fdset, NULL, NULL, &zero_time) == 1) {
				return (XPending(display));
			}
		}

        if (System::getMsTime() - sTime >= timeout) {
			return 0;
        }

        System::waitMs(2);
	}
}

// Run the application main loop.
void Application::runPrivate(oBool runOnce)
{
    Bool quit = False;
    ::Display *display = reinterpret_cast<::Display*>(ms_display);
	XEvent event;
	AppWindow::EventData eventData;

    while (!quit || EvtManager::instance()->isPendingEvent()) {
        ms_currAppWindow = nullptr;

        EvtManager::instance()->processEvent();

        if (display) {
            // handle the events in the queue
            while (pending(display, 2) > 0) { // while (XPending(display) > 0) {
                XNextEvent(display, &event);

                switch (event.type) {
                    case CreateNotify:  // not necessary
//                    ms_currAppWindow = searchAppWindow(static_cast<_HWND>(event.xcreatewindow.window));
//                    if (ms_currAppWindow) {
//                        ms_currAppWindow->processEvent(AppWindow::EVT_CREATE, eventData);
//                    }
                    break;

                    case MappingNotify:
                        XRefreshKeyboardMapping(&event.xmapping);
                        break;

                    case DestroyNotify:
                        ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xdestroywindow.window));
                        if (ms_currAppWindow) {
                            ms_currAppWindow->processEvent(AppWindow::EVT_DESTROY, eventData);
                            removeAppWindow(static_cast<_HWND>(event.xdestroywindow.window));
                            ms_currAppWindow = nullptr;
                        }
                        break;

                    case EnterNotify:
                        ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xcrossing.window));
                        if (ms_currAppWindow) {
                            ms_currAppWindow->processEvent(AppWindow::EVT_MOUSE_GAIN, eventData);
                        }
                        break;

                    case LeaveNotify:
                        ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xcrossing.window));
                        if (ms_currAppWindow) {
                            ms_currAppWindow->processEvent(AppWindow::EVT_MOUSE_LOST, eventData);
                        }
                        break;

                    case Expose:
                        ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xexpose.window));
                        if (ms_currAppWindow) {
                            if (event.xexpose.count == 0) {
                                ms_currAppWindow->processEvent(AppWindow::EVT_PAINT, eventData);
                            }
                        }
                        break;

                    case ConfigureNotify:
                        // The x and y members are set to the coordinates relative to the parent window's
                        // origin and indicate the position of the upper-left outside corner of the window.
                        // The width and height members are set to the inside size of the window, not including
                        // the border. The border_width member is set to the width of the window's border, in pixels.
                        ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xconfigure.window));
                        if (ms_currAppWindow) {
                            if ((ms_currAppWindow->getWidth() != event.xconfigure.width) ||
                                (ms_currAppWindow->getHeight() != event.xconfigure.height)) {

                                eventData.w = event.xconfigure.width;
                                eventData.h = event.xconfigure.height;

                                ms_currAppWindow->processEvent(AppWindow::EVT_RESIZE, eventData);
                            }

                            if ((ms_currAppWindow->getPositionX() != event.xconfigure.x) ||
                                (ms_currAppWindow->getPositionY() != event.xconfigure.y)) {

                                eventData.x = event.xconfigure.x;
                                eventData.y = event.xconfigure.y;

                                ms_currAppWindow->processEvent(AppWindow::EVT_MOVE, eventData);
                            }
                        }
                        break;

                    case KeyPress:
                        ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xkey.window));
                        if (ms_currAppWindow) {
                            //eventData.key = XLookupKeysym(&event.xkey, 0);
                            char text[8] = {0};
                            KeySym keysym = NoSymbol;
                            Status status = 0;

                            Xutf8LookupString((XIC)ms_currAppWindow->getIC(), &event.xkey,
                                              text, sizeof(text),
                                              &keysym, &status);

                            // XLookupString(&event.xkey, text, sizeof(text), &keysym, NULL);
                            // printf("compose %i %i", res, status);

                            if (text[0] != 0) {
                                String utf8;
                                utf8.fromUtf8(text);
                                eventData.unicode = utf8.toWChar();
                                // printf("%C (%s) %i %i\n", eventData.unicode, text, status, keysym);
                            } else {
                                eventData.unicode = 0;
                            }

                            eventData.character = keysym;
                            eventData.key = XLookupKeysym(&event.xkey, 0);

                            ms_currAppWindow->processEvent(AppWindow::EVT_KEYDOWN, eventData);
                        }
                        break;

                    case KeyRelease:
                        ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xkey.window));
                        if (ms_currAppWindow) {
                            eventData.key = XLookupKeysym(&event.xkey, 0);
                            eventData.character = eventData.key;
                            ms_currAppWindow->processEvent(AppWindow::EVT_KEYUP, eventData);
                        }
                        break;

                    case FocusIn:
                        ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xfocus.window));
                        if (ms_currAppWindow) {
                            ms_currAppWindow->processEvent(AppWindow::EVT_INPUT_FOCUS_LOST, eventData);
                        }
                        break;

                    case FocusOut:
                        ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xfocus.window));
                        if (ms_currAppWindow) {
                            ms_currAppWindow->processEvent(AppWindow::EVT_INPUT_FOCUS_GAIN, eventData);
                        }
                        break;

                    case ButtonPress:
                        ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xbutton.window));
                        if (ms_currAppWindow) {
                            eventData.button = event.xbutton.button;
                            ms_currAppWindow->processEvent(AppWindow::EVT_MOUSE_BUTTON_DOWN, eventData);
                        }
                        break;

                    case ButtonRelease:
                        ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xbutton.window));
                        if (ms_currAppWindow) {
                            eventData.button = event.xbutton.button;
                            ms_currAppWindow->processEvent(AppWindow::EVT_MOUSE_BUTTON_UP, eventData);
                        }
                        break;

                    case MotionNotify:
                        ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xmotion.window));
                        if (ms_currAppWindow) {
                            eventData.x = event.xmotion.x;
                            eventData.y = event.xmotion.y;
                            ms_currAppWindow->processEvent(AppWindow::EVT_MOUSE_MOTION, eventData);
                        }
                        break;

                    case ClientMessage:
                    {
                        // O3D_XEVENT
                        if (event.xclient.message_type == O3D_XEVENT) {
                            void *data = nullptr;
                            Int32 type = interpretUserEvent(event, data);

                            if (type == EVENT_CLOSE_WINDOW) {
                                //removeAppWindow((_HWND)data);
                                removeAppWindow(static_cast<_HWND>(event.xclient.window));
                            } else if (type == EVENT_EVT_MANAGER) {
                                EvtManager::instance()->processEvent();
                            } else if (type == EVENT_STD_TIMER) {
                                // TimerManager::instance()->callTimer(data);
                            }
                        } else if (event.xclient.message_type == WM_PROTOCOLS) {
                            // Close window button
                            ms_currAppWindow = getAppWindow(static_cast<_HWND>(event.xclient.window));
                            ms_currAppWindow->processEvent(AppWindow::EVT_CLOSE, eventData);
                        }
                    }
                        break;

                    case PropertyNotify:
                        if (event.xproperty.atom == XKLAVIER_STATE) {
//                            int keysyms_per_keycode = 0;
//                            int min_keycode, max_keycode;

//                            XDisplayKeycodes(display, &min_keycode, &max_keycode);
//                            KeySym *keySym = XGetKeyboardMapping(display, min_keycode, max_keycode - min_keycode + 1, &keysyms_per_keycode);
//                            printf("%i %i %i\n",min_keycode,max_keycode,keysyms_per_keycode);
//                            //keySym[(max_keycode-min_keycode-1)*keysyms_per_keycode]=1;
//                            //	            		XChangeKeyboardMapping(display, min_keycode, keysyms_per_keycode, keySym, max_keycode - min_keycode);
//                            XFree(keySym);
//                            //	            		XFlush(display);
//                            XMappingEvent *e = new XMappingEvent;
//                            e->type = 34;//MappingKeyboard;
//                            e->serial = 99;//109;
//                            e->send_event = 0;
//                            e->first_keycode = 8;//min_keycode;
//                            e->count = 248;//max_keycode - min_keycode;
//                            e->window = 0;//event.xproperty.window;
//                            e->display = display;
//                            e->request = 1;
//                            XRefreshKeyboardMapping(e);

                            ::Display *display = reinterpret_cast<::Display*>(ms_display);

                            //Atom atom = 0;
                            XEvent event;
                            memset(&event, 0, sizeof(event));

                            event.type = MappingNotify;
                            event.xmapping.display = display;
                            event.xmapping.window = RootWindow(display, DefaultScreen(display));
                            event.xmapping.serial = 0;
                            event.xmapping.send_event = True;
                            event.xmapping.type = MappingKeyboard;  // 34;
                            event.xmapping.first_keycode = 8;
                            event.xmapping.count = 248;
                            event.xmapping.request = 1;

                            XSendEvent(display, RootWindow(display, DefaultScreen(display)), False, 0, &event);
                            XFlush(display);
                        }
                        break;

                    case SelectionRequest:
                    {
                        // Get the request in question
                        XSelectionRequestEvent *request = &event.xselectionrequest;

                        // Generate a reply to the selection request
                        XSelectionEvent reply;

                        reply.type = SelectionNotify;
                        reply.serial = event.xany.serial;
                        reply.send_event = True;
                        reply.display = display;
                        reply.requestor = request->requestor;
                        reply.selection = request->selection;
                        reply.property = request->property;
                        reply.target = None;
                        reply.time = request->time;

                        // They want to know what we can provide/offer
                        if (request->target == XA_TARGETS) {
                            Atom possibleTargets[] = {XA_STRING, XA_UTF8_STRING, XA_COMPOUND_TEXT};

                            XChangeProperty(display, request->requestor,
                                            request->property, XA_ATOM, 32, PropModeReplace,
                                            (unsigned char *) possibleTargets, 3);
                        } else if (request->target == XA_STRING || request->target == XA_UTF8_STRING ||
                                   request->target == XA_COMPOUND_TEXT) {

                            // They want a string (all we can provide)
                            int len;
                            char *xdata = XFetchBytes(display, &len);

                            XChangeProperty(display, request->requestor,
                                            request->property, request->target, 8,
                                            PropModeReplace, (unsigned char *) xdata,
                                            len);
                            XFree(xdata);
                        } else {
                            // Did not have what they wanted, so no property set
                            reply.property = None;
                        }

                        // Dispatch the event
                        XSendEvent(request->display, request->requestor, 0, NoEventMask, (XEvent*)&reply);
                    }

                    default:
                        break;
                }
            }
        }

		// process update/paint event if necessary for each window
        for (IT_AppWindowMap it = ms_appWindowMap.begin(); it != ms_appWindowMap.end(); ++it) {
            if (it->second->isRunning()) {
				it->second->processEvent(AppWindow::EVT_UPDATE, eventData);
                it->second->processEvent(AppWindow::EVT_PAINT, eventData);
			}
		}

        if (ms_appWindowMap.empty()) {
            // dont waste the CPU when no window
            // System::waitMs(2);

            // break the main loop when no more window
			quit = True;
        }

        if (runOnce) {
            break;
        }
	}
}

// Push a user application event.
void Application::pushEventPrivate(EventType type, _HWND hWnd, void *data)
{
    ::Display *display = reinterpret_cast<::Display*>(ms_display);
    if (display == nullptr) {
        return;
    }

    XEvent event;
	memset(&event, 0, sizeof(event));

    event.xclient.type = ClientMessage;
    event.xclient.window = hWnd;
    event.xclient.message_type = O3D_XEVENT;
    event.xclient.format = 32;

    // data
    event.xclient.data.l[0] = static_cast<long>((reinterpret_cast<UInt64>(data) & 0xffff0000) >> 32);
    event.xclient.data.l[1] = static_cast<long>(reinterpret_cast<UInt64>(data) & 0x0000ffff);
    event.xclient.data.l[2] = CurrentTime;
    event.xclient.data.l[3] = 0;
    event.xclient.data.l[4] = type;

	XSendEvent(
			display,
            hWnd,
			False,
            0,
            &event);
}

#endif // O3D_X11
