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

// ONLY IF O3D_WINAPI_SYS IS SELECTED
#ifdef O3D_WINAPI_SYS

#include "o3d/core/architecture.h"

using namespace o3d;

static Int64 timerStartTime;
static Int64 timerFrequency;

System::ByteOrder System::getNativeByteOrder()
{
	return ORDER_LITTLE_ENDIAN;
}

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

Int64 System::getTime()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return time.QuadPart - timerStartTime;
}

Int64 System::getTime()
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return time.QuadPart;
}

Int64 System::getEpochTime()
{
    const UInt64 OA_ZERO_TICKS = 94353120000000000;  // 12/30/1899 12:00am in ticks
    const UInt64 TICKS_PER_DAY = 864000000000;       // ticks per day at 0.1 us

    LPFILETIME ft;
    GetSystemTimePreciseAsFileTime(&ft);

    ULARGE_INTEGER dt; // needed to avoid alignment faults
    dt.LowPart = ft.dwLowDateTime;
    dt.HighPart = ft.dwHighDateTime;

    return static_cast<Int64>((dt.QuadPart - OA_ZERO_TICKS) / TICKS_PER_DAY);
}

Int64 System::getTime(TimeUnit unit)
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);

    Int64 diffT = time.QuadPart - timerStartTime;

    if (unit == TIME_SECOND) {
        return diffT / timerFrequency;
    } else if (unit == TIME_MILLISECOND) {
        return diffT / (timerFrequency * 1000);
    } else if (unit == TIME_MICROSECOND) {
        return diffT / (timerFrequency * 1000000);
    } else if (unit == TIME_NANOSECOND) {
        return diffT / (timerFrequency * 1000000000);
    } else {
        return diffT;
    }
}

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

Int64 System::getEpochTimeMs()
{
    const UInt64 OA_ZERO_TICKS = 94353120000000000;  // 12/30/1899 12:00am in ticks
    const UInt64 TICKS_PER_DAY = 86400000;           // ticks per day at 1 ms

    LPFILETIME ft;
    GetSystemTimePreciseAsFileTime(&ft);

    ULARGE_INTEGER dt; // needed to avoid alignment faults
    dt.LowPart = ft.dwLowDateTime;
    dt.HighPart = ft.dwHighDateTime;

    return static_cast<Int64>((dt.QuadPart - OA_ZERO_TICKS) / TICKS_PER_DAY);
}

Int64 System::getTimeFrequency()
{
	return timerFrequency;
}

void System::waitMs(Int32 ms)
{
	Sleep(ms);
}

void System::print(
		const String &content,
		const String &title,
        MessageLevel level)
{
	String out;

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    switch (level){
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
    WriteConsoleW(console, out.getData(), out.length()+1, &ignore, nullptr);
}

#endif // O3D_WINAPI_SYS
