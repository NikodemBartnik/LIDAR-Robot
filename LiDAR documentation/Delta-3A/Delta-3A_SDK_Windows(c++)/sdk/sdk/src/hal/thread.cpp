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
#include "hal/thread.h"

#if defined(_WIN32)
#include "arch/win32/winthread.hpp"
#elif defined(_MACOS)
#include "arch/macOS/thread.hpp"
#elif defined(__GNUC__)
#include "arch/linux/thread.hpp"
#else
#error no threading implemention found for this platform.
#endif


