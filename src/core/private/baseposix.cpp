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
#include <endian.h>

using namespace o3d;

// macosx 10.4 no have it, and 10.5 ?
#if defined(O3D_MACOSX)
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
static Int64 timerFrequency = 0;
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
    timerFrequency = (Int64)timerResolution.tv_sec + 1000000000LL / (Int64)timerResolution.tv_nsec;
#else
    gettimeofday(&timerStartTime, nullptr);
    timerFrequency = 1000000; // microsecond
#endif
}

// get time with precision defined by getTimeFrequency()
Int64 System::getTime()
{
#if HAVE_CLOCK_GETTIME
    // get time in nanoseconds
    struct timespec curTime;
    clock_gettime(CLOCK_MONOTONIC, &curTime);

    return ((Int64)curTime.tv_sec - (Int64)timerStartTime.tv_sec) * 1000000000LL +
           (Int64)(curTime.tv_nsec - (Int64)timerStartTime.tv_nsec);
#else
    // get time in microsecond
    struct timeval curTime;
    gettimeofday(&curTime, nullptr);

    return (curTime.tv_sec - timerStartTime.tv_sec) * 1000000 +
           (curTime.tv_usec - timerStartTime.tv_usec);
#endif
}

// get time with precision defined by getTimeFrequency()
Int64 System::getEpochTime()
{
#if HAVE_CLOCK_GETTIME
    // get time in nanoseconds
    struct timespec curTime;
    clock_gettime(CLOCK_REALTIME, &curTime);

    return ((Int64)curTime.tv_sec) * 10000000LL +
           (Int64)(curTime.tv_nsec) / 100;
#else
    // get time in microsecond
    struct timeval curTime;
    gettimeofday(&curTime, nullptr);

    return (curTime.tv_sec) * 10000000LL +
           (curTime.tv_usec) * 10;
#endif
}

Int64 System::getTime(TimeUnit unit)
{
#if HAVE_CLOCK_GETTIME
    struct timespec curTime;
    clock_gettime(CLOCK_MONOTONIC, &curTime);

    if (unit == TIME_SECOND)
        return ((Int64)curTime.tv_sec - (Int64)timerStartTime.tv_sec) +
               ((Int64)curTime.tv_nsec - (Int64)timerStartTime.tv_nsec) / 1000000000LL;
    else if (unit == TIME_MILLISECOND)
        return ((Int64)curTime.tv_sec - (Int64)timerStartTime.tv_sec) * 1000 +
               ((Int64)curTime.tv_nsec - (Int64)timerStartTime.tv_nsec) / 1000000;
    else if (unit == TIME_MICROSECOND)
        return ((Int64)curTime.tv_sec - (Int64)timerStartTime.tv_sec) * 1000000 +
               ((Int64)curTime.tv_nsec - (Int64)timerStartTime.tv_nsec) / 1000;
    else if (unit == TIME_NANOSECOND)
        return ((Int64)curTime.tv_sec - (Int64)timerStartTime.tv_sec) * 1000000000LL +
               ((Int64)curTime.tv_nsec - (Int64)timerStartTime.tv_nsec);
    else // nanosecond
        return ((Int64)curTime.tv_sec - (Int64)timerStartTime.tv_sec) * 1000000000LL +
               ((Int64)curTime.tv_nsec - (Int64)timerStartTime.tv_nsec);
#else
    struct timeval curTime;
    gettimeofday(&curTime, nullptr);

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

    return ((Int64)curTime.tv_sec - (Int64)timerStartTime.tv_sec) * 1000 +
           ((Int64)curTime.tv_nsec - (Int64)timerStartTime.tv_nsec) / 1000000;
#else
    struct timeval curTime;
    gettimeofday(&curTime, nullptr);

    return (curTime.tv_sec - timerStartTime.tv_sec) * 1000 +
           (curTime.tv_usec - timerStartTime.tv_usec) / 1000;
#endif
}

// get time width a precision of 1ms
Int32 System::getEpochMsTime()
{
#if HAVE_CLOCK_GETTIME
    struct timespec curTime;
    clock_gettime(CLOCK_REALTIME, &curTime);

    return ((Int64)curTime.tv_sec) * 1000 +
           ((Int64)curTime.tv_nsec) / 100000;
#else
    struct timeval curTime;
    gettimeofday(&curTime, nullptr);

    return (curTime.tv_sec) * 1000 +
           (curTime.tv_usec) / 1000;
#endif
}

// get time frequency for getTime()
Int64 System::getTimeFrequency()
{
//    return timerFrequency;
#if HAVE_CLOCK_GETTIME
    return 1000000000LL;  // 1 ns
#else
    return 1000000; // 1 us
#endif
}

// wait a delay (in ms)
void System::waitMs(Int32 ms)
{
    if (ms == 0) {
    #ifdef O3D_ANDROID
        usleep(0);
    #else
        pthread_yield();
    #endif
    } else {
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
    String c = content.isValid() ? content : "<undefined>";
    String t = title.isValid() ? title : "<untitled>";
    String msg;

    switch (level)
	{
        case MSG_DEBUG:
            msg = String("[DEBUG] ") + t + ": " + c;
            puts(msg.toUtf8().getData());
            return;

        case MSG_INFO:
            msg = String("[INFO] ") + t + ": " + c;
            puts(msg.toUtf8().getData());
			return;

		case MSG_WARNING:
            msg = String("[WARNING] ") + t + ": " + c;
            puts(msg.toUtf8().getData());
			return;

		case MSG_ERROR:
            msg = String("[ERROR] ") + t + ": " + c;
            puts(msg.toUtf8().getData());
			return;

        case MSG_CRITICAL:
            msg = String("[CRITICAL] ") + t + ": " + c;
            puts(msg.toUtf8().getData());
            return;

		default:
            msg = String("[DEBUG] ") + t + ": " + c;
            puts(msg.toUtf8().getData());
			return;
	}
}

#undef HAVE_CLOCK_GETTIME

#endif // O3D_POSIX_SYS
