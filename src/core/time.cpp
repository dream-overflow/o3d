/**
 * @file time.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/time.h"
#include "o3d/core/base.h"

using namespace o3d;

// get a time in day/hours/minutes/seconds by a time from Int64
void Time::convertTime(
    Int64 time,
    Int32 &day,
    Int32 &hours,
    Int32 &mins,
    Int32 &sec)
{
    Int64 ms = time / System::getTimeFrequency();

    day = (Int32)(ms / 86400); if (day != 0) ms = ms - day * 86400;
    hours = (Int32)(ms / 3600); if (hours != 0) ms = ms - hours * 3600;
    mins = (Int32)(ms / 60); if (mins != 0) ms = ms - mins * 60;
    sec = (Int32)(ms);
}

void Time::convertMsTime(
        Int32 time,
        Int32 &day,
        Int32 &hours,
        Int32 &mins,
        Int32 &sec)
{
    Int32 ms = time;

    day = (Int32)(ms / 86400); if (day != 0) ms = ms - day * 86400;
    hours = (Int32)(ms / 3600); if (hours != 0) ms = ms - hours * 3600;
    mins = (Int32)(ms / 60); if (mins != 0) ms = ms - mins * 60;
    sec = (Int32)(ms);
}

