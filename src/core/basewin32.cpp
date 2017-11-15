/**
 * @file basewin32.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/string.h"
#include "o3d/core/debug.h"

// ONLY IF O3D_WIN32_SYS IS SELECTED
#ifdef O3D_WIN32_SYS

#include "o3d/core/architecture.h"

using namespace o3d;

static Int64 timerStartTime;
static Int64 timerFrequency;

System::ByteOrder System::getNativeByteOrder()
{
	return ORDER_LITTLE_ENDIAN;
}

// Initialize time.
void System::initTime()
{
	// Get the query performance timer frequency
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	timerFrequency = freq.QuadPart;

	// The time when the application start
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	timerStartTime = time.QuadPart;
}

// get time with precision defined by getTimeFrequency()
Int64 System::getTime()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return time.QuadPart - timerStartTime;
}

Int64 System::getTime(TimeUnit unit)
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);

    Int64 diffT = time.QuadPart - timerStartTime;

    if (unit == TIME_SECOND)
        return diffT / timerFrequency;
    else if (unit == TIME_MILLISECOND)
        return diffT / (timerFrequency * 1000);
    else if (unit == TIME_MICROSECOND)
        return diffT / (timerFrequency * 1000000);
    else if (unit == TIME_NANOSECOND)
        return diffT / (timerFrequency * 1000000000);
    else
        return diffT;
}

// get time width a precision of 1ms
Int32 System::getMsTime()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);

    return (time.QuadPart - timerStartTime) / (timerFrequency * 1000);

//	time.QuadPart -= timerStartTime;
//	time.QuadPart *= 1000;
//	time.QuadPart /= timerFrequency;

//	return (DWORD)time.QuadPart;
}

//  get time frequency for gettime
Int64 System::getTimeFrequency()
{
	return timerFrequency;
}

// wait a delay (in ms)
void System::waitMs(Int32 ms)
{
	Sleep(ms);
}

// Print a message on the standard output.
void System::print(
		const String &content,
		const String &title,
        MessageLevel level)
{
	String out;

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    switch (level)
	{
        case MSG_DEBUG:
            out << "[DEBUG] " << title << ": " << content << "\n";
			break;

        case MSG_INFO:
            out << "[INFO] " << title << ": " << content << "\n";
			break;

		case MSG_WARNING:
			out << "[WARNING] " << title << ": " << content << "\n";
			break;

        case MSG_ERROR:
            out << "[ERROR] " << title << ": " << content << "\n";
			break;

        case MSG_CRITICAL:
            out << "[CRITICAL] " << title << ": " << content << "\n";
			break;

		default:
            out << "[DEBUG] " << title << ": " << content << "\n";
			break;
	}

	DWORD ignore;
	WriteConsoleW(console, out.getData(), out.length()+1, &ignore, NULL);
}

#endif // O3D_WIN32_SYS

