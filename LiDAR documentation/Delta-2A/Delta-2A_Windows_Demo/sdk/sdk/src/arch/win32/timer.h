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

#pragma once

#include "rstypes.h"

#define delay(x)   ::Sleep(x)

namespace rs{ namespace arch{
    void HPtimer_reset();
    _u32 getHDTimer();
}}

#define getms()   rs::arch::getHDTimer()

