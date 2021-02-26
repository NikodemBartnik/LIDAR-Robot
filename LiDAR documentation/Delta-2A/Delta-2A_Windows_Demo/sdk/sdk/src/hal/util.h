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


//------
/* _countof helper */
#if !defined(_countof)
#if !defined(__cplusplus)
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#else
extern "C++"
{
template <typename _CountofType, size_t _SizeOfArray>
char (*__countof_helper( _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) sizeof(*__countof_helper(_Array))
}
#endif
#endif

/* _offsetof helper */
#if !defined(offsetof)
#define offsetof(_structure, _field) ((_word_size_t)&(((_structure *)0x0)->_field))
#endif


#define BEGIN_STATIC_CODE( _blockname_ ) \
    static class _static_code_##_blockname_ {   \
    public:     \
        _static_code_##_blockname_ () 


#define END_STATIC_CODE( _blockname_ ) \
    }   _instance_##_blockname_;

