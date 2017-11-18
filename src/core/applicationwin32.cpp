/**
 * @file applicationwin32.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/application.h"

// ONLY IF O3D_WINDOWS IS SELECTED
#ifdef O3D_WINDOWS

#include "o3d/core/string.h"
#include "o3d/core/debug.h"
#include "o3d/core/architecture.h"
#include "o3d/core/appwindow.h"

using namespace o3d;

// Show a message. On win32 application it draw a standard message box,
void Application::message(
		const String &content,
		const String &title,
		Application::IconStyle icon)
{
	Int32 ico = 0;

	switch (icon)
	{
		case Application::ICON_HELP:
			ico = MB_HELP;
			break;

		case Application::ICON_OK:
			ico = MB_OK;
			break;

		case Application::ICON_WARNING:
			ico = MB_ICONWARNING;
			break;

		case Application::ICON_INFORMATION:
			ico = MB_ICONINFORMATION;
			break;

		case Application::ICON_NOTICE:
			ico = MB_ICONASTERISK;
			break;

		case Application::ICON_QUESTION:
			ico = MB_ICONQUESTION;
			break;

		case Application::ICON_ERROR:
			ico = MB_ICONERROR;
			break;

		default:
			ico = MB_ICONASTERISK;
			break;
	}

	::MessageBoxW(
			NULL,
			content.isValid() ? content.getData() : L"<undefined>",
			title.isValid() ? title.getData() : L"<untitled>",
			ico);
}

// Map a single file. It is mostly used to make a single instance application.
void Application::mapSingleFile(const String &name)
{
	HANDLE hmap;
	LPVOID pmem;

	if ((hmap = ::CreateFileMappingW(
			INVALID_HANDLE_VALUE,
			0,
			PAGE_READWRITE,
			0,
			4,
			name.getData())) == NULL)
	{
		O3D_ERROR(E_InvalidOperation("Unable to create the file lock (mapped file)"));
	}

	pmem = ::MapViewOfFile(hmap, FILE_MAP_WRITE, 0, 0, 0);

	if (pmem == NULL)
		O3D_ERROR(E_InvalidOperation("The single file is already mapped by another instance"));
}

// Check for a mapped file existence.
Bool Application::isMappedFileExists(const String &name)
{
	HANDLE hmap;
	LPVOID pmem;

	if ((hmap = ::OpenFileMappingW(FILE_MAP_READ, 0, name.getData())) == NULL)
		return False;

	if ((pmem = ::MapViewOfFile(hmap, FILE_MAP_READ, 0, 0, 0)) == NULL)
	{
		::CloseHandle(hmap);
		return False;
	}

	::UnmapViewOfFile(pmem);
	::CloseHandle(hmap);

	return True;
}

Int32 Application::getPID()
{
	return ::GetCurrentProcessId();
}

#endif // O3D_WINDOWS

// ONLY IF O3D_WIN32 IS SELECTED
#ifdef O3D_WIN32

void Application::apiInitPrivate()
{
}

void Application::apiQuitPrivate()
{
}

// Run the application main loop.
void Application::runPrivate(Bool runOnce)
{
	// WIN32 messages peeking
	MSG message;
	Bool quit = False;

	AppWindow::EventData eventData;

    while (!quit || EvtManager::instance()->isPendingEvent()) {
        EvtManager::instance()->processEvent();

        while (!quit && PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
			// Retrieve the AppWindow object
			ms_currAppWindow = getAppWindow(reinterpret_cast<_HWND>(message.hwnd));

            if (message.message == WM_QUIT) {
				quit = True;
            }

            if ((message.message == WM_USER) && (message.hwnd == nullptr)) {
                if (message.wParam == EVENT_EVT_MANAGER) {
					EvtManager::instance()->processEvent();
                } else if (message.wParam == EVENT_CLOSE_WINDOW) {
                    ms_currAppWindow = nullptr;
					removeAppWindow(reinterpret_cast<_HWND>(message.lParam));
				}
            } else {
				TranslateMessage(&message);
				DispatchMessageW(&message);
			}

            if (ms_appWindowMap.empty()) {
				::PostQuitMessage(0);
            }
		}

		DWORD result = MsgWaitForMultipleObjectsEx(0, NULL, 2, QS_ALLEVENTS, MWMO_ALERTABLE);

		// process update/paint event if necessary for each window
        for (IT_AppWindowMap it = ms_appWindowMap.begin(); it != ms_appWindowMap.end(); ++it) {
            if (it->second->isRunning()) {
				it->second->processEvent(AppWindow::EVT_UPDATE, eventData);
				it->second->processEvent(AppWindow::EVT_PAINT, eventData);
			}
		}

		// The result tells us the type of event we have.
        if (result == WAIT_FAILED) {
			O3D_ERROR(E_InvalidResult("MsgWaitForMultipleObjectsEx WAIT_FAILED"));
        } else if (result == (WAIT_OBJECT_0 + 0)) {
			// In the case of this message loop, we are not waiting on any
			// objects, so the return code for new messages is
			// WAIT_OBJECT_0 + 0.

			// Continue to the top of the while loop to process the messages.
			continue;
        } else if (result == WAIT_IO_COMPLETION) {
			// Wait was terminated by an APC being queued to the thread.
        } else if (result == WAIT_TIMEOUT) {
			// Wait timed out. Do nothing in this case. Just continue back
			// to the top of the while loop.
        } else {
			// One of the handles became signaled.
			// If we were actually waiting on any handles, we would get the
			// object index using this expression:
			// int index = result - WAIT_OBJECT_0;
        }

        if (runOnce) {
            break;
        }
	}
}

// Push a user application event.
void Application::pushEventPrivate(EventType type, _HWND hWnd, void *data)
{
    if (ms_display == nullptr) {
        return;
    }

	PostThreadMessageW(
		ThreadManager::getMainThreadId(),
		WM_USER,
		type,
		reinterpret_cast<LPARAM>(data));
}

#endif // O3D_WIN32
