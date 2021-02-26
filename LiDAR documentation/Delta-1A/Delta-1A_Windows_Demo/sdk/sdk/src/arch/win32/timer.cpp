/*
*  RSLIDAR System
*  Driver Interface
*
*  Copyright 2015 RS Team
*  All rights reserved.
*
*	Author: ruishi, Data:2015-12-25
*
*/

#include "sdkcommon.h"
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

namespace rs{ namespace arch{

static LARGE_INTEGER _current_freq;

void HPtimer_reset()
{
    BOOL ans=QueryPerformanceFrequency(&_current_freq);
    _current_freq.QuadPart/=1000;
}

_u32 getHDTimer()
{
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);

    return (_u32)(current.QuadPart/_current_freq.QuadPart);
}

BEGIN_STATIC_CODE(timer_cailb)
{
    HPtimer_reset();
}END_STATIC_CODE(timer_cailb)

}}
