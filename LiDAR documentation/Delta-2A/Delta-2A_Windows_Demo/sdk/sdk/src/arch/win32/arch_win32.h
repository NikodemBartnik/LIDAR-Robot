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

#pragma warning (disable: 4996)
#define _CRT_SECURE_NO_WARNINGS

#ifndef WINVER
#define WINVER		0x0500
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0501
#endif


#ifndef _WIN32_IE
#define _WIN32_IE	0x0501
#endif

#ifndef _RICHEDIT_VER
#define _RICHEDIT_VER	0x0200
#endif


#include <stddef.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>   //for memcpy etc..
#include <process.h>
#include <direct.h> 


#include "timer.h"
