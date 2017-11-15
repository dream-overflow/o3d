/**
 * @file time.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TIME_H
#define _O3D_TIME_H

#include "base.h"

namespace o3d {

/**
 * @brief The Time utilities class.
 */
class O3D_API Time
{
public:

    template<class R, class T>
    static R toNanos(T time, TimeUnit unit)
    {
        if (unit == TIME_SECOND)
            return R(time) * 1000000000;
        else if (unit == TIME_MILLISECOND)
            return R(time) * 1000000;
        else if (unit == TIME_MICROSECOND)
            return R(time) * 1000;
        else if (unit == TIME_NANOSECOND)
            return R(time);
        return 0;
    }

    template<class R, class T>
    static R toMicros(T time, TimeUnit unit)
    {
        if (unit == TIME_SECOND)
            return R(time) * 1000000;
        else if (unit == TIME_MILLISECOND)
            return R(time) * 1000;
        else if (unit == TIME_MICROSECOND)
            return R(time) * 1;
        else if (unit == TIME_NANOSECOND)
            return R(time) / 1000;
        return 0;
    }

    template<class R, class T>
    static R toMillis(T time, TimeUnit unit)
    {
        if (unit == TIME_SECOND)
            return R(time) * 1000;
        else if (unit == TIME_MILLISECOND)
            return R(time);
        else if (unit == TIME_MICROSECOND)
            return R(time) / 1000;
        else if (unit == TIME_NANOSECOND)
            return R(time) / 1000000;
        return 0;
    }

    //! Convert a system time (high resolution), to days, hours, minutes, seconds.
    static void convertTime(
            Int64 time,
            Int32 &day,
            Int32 &hours,
            Int32 &mins,
            Int32 &sec);

    //! Convert a millisecond time, to days, hours, minutes, seconds.
    static void convertMsTime(
            Int32 time,
            Int32 &day,
            Int32 &hours,
            Int32 &mins,
            Int32 &sec);
};

} // namespace o3d

#endif // _O3D_TIME_H

