/**
 * @file baseposix.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/base.h"

// ONLY IF O3D_POSIX_SYS IS SELECTED
#ifdef O3D_POSIX_SYS

#include "o3d/core/string.h"

#include <sys/time.h>
#include <errno.h>
#include <iostream>
#include <endian.h>

using namespace o3d;

#ifdef __MACOSX__ // macosx 10.4 no have it, and 10.5 ?
	#define HAVE_CLOCK_GETTIME 0
	#define pthread_yield pthread_yield_np
#else
#define HAVE_CLOCK_GETTIME 1
#endif

#if HAVE_CLOCK_GETTIME
static struct timespec timerStartTime;
static struct timespec timerResolution;
static Int64 timerFrequency = 0;
#else
static struct timeval timerStartTime;
#endif // HAVE_CLOCK_GETTIME

#define O3D_USE_USLEEP 1

#ifdef O3D_USE_USLEEP
#include <unistd.h>
#endif

System::ByteOrder System::getNativeByteOrder()
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	return ORDER_LITTLE_ENDIAN;
#else
	return ORDER_BIG_ENDIAN;
#endif
}

void System::initTime()
{
    // Set first ticks value
#if HAVE_CLOCK_GETTIME
    clock_gettime(CLOCK_MONOTONIC, &timerStartTime);
    clock_getres(CLOCK_MONOTONIC, &timerResolution);

    // nanosecond precision
    timerFrequency = timerResolution.tv_sec + 1000000000 / timerResolution.tv_nsec;
#else
    gettimeofday(&timerStartTime, NULL);
    timerResolutionInt64 = 1000000; // microsecond
#endif
}

// get time with precision defined by getTimeFrequency()
Int64 System::getTime()
{
#if HAVE_CLOCK_GETTIME
    // get time in nanoseconds
    struct timespec curTime;
    clock_gettime(CLOCK_MONOTONIC, &curTime);

    return (curTime.tv_sec - timerStartTime.tv_sec) * 1000000000 +
           (curTime.tv_nsec - timerStartTime.tv_nsec);
#else
    // get time in microsecond
    struct timeval curTime;
    gettimeofday(&curTime, NULL);

    return (curTime.tv_sec - timerStartTime.tv_sec) * 1000000 +
           (curTime.tv_usec - timerStartTime.tv_usec);
#endif
}

Int64 System::getTime(TimeUnit unit)
{
#if HAVE_CLOCK_GETTIME
    struct timespec curTime;
    clock_gettime(CLOCK_MONOTONIC, &curTime);

    if (unit == TIME_SECOND)
        return (curTime.tv_sec - timerStartTime.tv_sec) +
               (curTime.tv_nsec - timerStartTime.tv_nsec) / 1000000000;
    else if (unit == TIME_MILLISECOND)
        return (curTime.tv_sec - timerStartTime.tv_sec) * 1000 +
               (curTime.tv_nsec - timerStartTime.tv_nsec) / 1000000;
    else if (unit == TIME_MICROSECOND)
        return (curTime.tv_sec - timerStartTime.tv_sec) * 1000000 +
               (curTime.tv_nsec - timerStartTime.tv_nsec) / 1000;
    else if (unit == TIME_NANOSECOND)
        return (curTime.tv_sec - timerStartTime.tv_sec) * 1000000000 +
               (curTime.tv_nsec - timerStartTime.tv_nsec);
    else // nanosecond
        return (curTime.tv_sec - timerStartTime.tv_sec) * 1000000000 +
               (curTime.tv_nsec - timerStartTime.tv_nsec);
#else
    struct timeval curTime;
    gettimeofday(&curTime, NULL);

    if (unit == TIME_SECOND)
        return (curTime.tv_sec - timerStartTime.tv_sec) +
               (curTime.tv_usec - timerStartTime.tv_usec) / 1000000;
    else if (unit == TIME_MILLISECOND)
        return (curTime.tv_sec - timerStartTime.tv_sec) * 1000 +
               (curTime.tv_usec - timerStartTime.tv_usec) / 1000;
    else if (unit == TIME_MICROSECOND)
        return (curTime.tv_sec - timerStartTime.tv_sec) * 1000000 +
               (curTime.tv_usec - timerStartTime.tv_usec);
    else if (unit == TIME_NANOSECOND)
        return (curTime.tv_sec - timerStartTime.tv_sec) * 1000000000 +
               (curTime.tv_usec - timerStartTime.tv_usec) * 1000;
    else // microsecond
        return (curTime.tv_sec - timerStartTime.tv_sec) * 1000000 +
               (curTime.tv_usec - timerStartTime.tv_usec);
#endif
}

// get time width a precision of 1ms
Int32 System::getMsTime()
{
#if HAVE_CLOCK_GETTIME
    struct timespec curTime;
    clock_gettime(CLOCK_MONOTONIC, &curTime);

    return (curTime.tv_sec - timerStartTime.tv_sec) * 1000 +
           (curTime.tv_nsec - timerStartTime.tv_nsec) / 1000000;
#else
    struct timeval curTime;
    gettimeofday(&curTime, NULL);

    return (curTime.tv_sec - timerStartTime.tv_sec) * 1000 +
           (curTime.tv_usec - timerStartTime.tv_usec) / 1000;
#endif
}

// get time frequency for getTime()
Int64 System::getTimeFrequency()
{
//    return timerFrequency;
#if HAVE_CLOCK_GETTIME
    return 1000000000;  // 1 ns
#else
    return 1000000; // 1 us
#endif
}

// wait a delay (in ms)
void System::waitMs(Int32 ms)
{
	if (ms == 0)
	{
		pthread_yield();
	}
	else
	{
#ifdef O3D_USE_USLEEP
		usleep(ms*1000);
#else
		int result;
		struct timespec elapsed, tv;

		// Set the timeout interval
		elapsed.tv_sec = ms / 1000;
		elapsed.tv_nsec = (ms % 1000) * 1000000;

		// do while a signal interrupt the sleep
		do {
			errno = 0;

			tv.tv_sec = elapsed.tv_sec;
			tv.tv_nsec = elapsed.tv_nsec;
			result = nanosleep(&tv, &elapsed);

		} while (result && (errno == EINTR));
#endif
	}
}

// Print a message on the standard output.
void System::print(
		const String &content,
		const String &title,
        MessageLevel level)
{
	CString c = content.isValid() ? content.toUtf8() : "<undefined>";
	CString t = title.isValid() ? title.toUtf8() : "<untitled>";

    switch (level)
	{
        case MSG_DEBUG:
            std::cout << "[DEBUG] " << t.getData() << ": " << c.getData() << std::endl;
			return;

        case MSG_INFO:
            std::cout << "[INFO] " << t.getData() << ": " << c.getData() << std::endl;
			return;

		case MSG_WARNING:
			std::cout << "[WARNING] " << t.getData() << ": " << c.getData() << std::endl;
			return;

		case MSG_ERROR:
			std::cout << "[ERROR] " << t.getData() << ": " << c.getData() << std::endl;
			return;

        case MSG_CRITICAL:
            std::cout << "[CRITICAL] " << t.getData() << ": " << c.getData() << std::endl;
            return;

		default:
            std::cout << "[DEBUG] " << t.getData() << ": " << c.getData() << std::endl;
			return;
	}
}

#undef HAVE_CLOCK_GETTIME

#endif // O3D_POSIX_SYS

