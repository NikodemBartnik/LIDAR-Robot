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
namespace rs{ namespace hal{ 

class Locker
{
public:
    enum LOCK_STATUS
    {
        LOCK_OK = 1,
        LOCK_TIMEOUT = -1,
        LOCK_FAILED = 0
    };

    Locker(){
#ifdef _WIN32
        _lock = NULL;
#endif
        init();
    }

    ~Locker()
    {
        release();
    }

    Locker::LOCK_STATUS lock(unsigned long timeout = 0xFFFFFFFF)
    {
#ifdef _WIN32
        switch (WaitForSingleObject(_lock, timeout==0xFFFFFFF?INFINITE:(DWORD)timeout))
        {
        case WAIT_ABANDONED:
            return LOCK_FAILED;
        case WAIT_OBJECT_0:
            return LOCK_OK;
        case WAIT_TIMEOUT:
            return LOCK_TIMEOUT;
        }

#else
#ifdef _MACOS
        if (timeout !=0 ) {
            if (pthread_mutex_lock(&_lock) == 0) return LOCK_OK;
        }
#else
        if (timeout == 0xFFFFFFFF){
            if (pthread_mutex_lock(&_lock) == 0) return LOCK_OK;
        }
#endif
        else if (timeout == 0)
        {
            if (pthread_mutex_trylock(&_lock) == 0) return LOCK_OK;
        }
#ifndef _MACOS
        else
        {
            timespec wait_time;
            timeval now;
            gettimeofday(&now,NULL);

            wait_time.tv_sec = timeout/1000 + now.tv_sec;
            wait_time.tv_nsec = (timeout%1000)*1000000 + now.tv_usec*1000;
        
            if (wait_time.tv_nsec >= 1000000000)
            {
               ++wait_time.tv_sec;
               wait_time.tv_nsec -= 1000000000;
            }
#ifndef __ARM_LINUX_ANDROIDEABI_4_8__             
            switch (pthread_mutex_timedlock(&_lock,&wait_time))
            {
            case 0:
                return LOCK_OK;
            case ETIMEDOUT:
                return LOCK_TIMEOUT;
            }
#endif            
        }
#endif
#endif

        return LOCK_FAILED;
    }


    void unlock()
    {
#ifdef _WIN32
        ReleaseMutex(_lock);
#else
        pthread_mutex_unlock(&_lock);
#endif
    }

#ifdef _WIN32
    HANDLE getLockHandle()
    {
        return _lock;
    }
#else
    pthread_mutex_t *getLockHandle()
    {
        return &_lock;
    }
#endif



protected:
    void    init()
    {
#ifdef _WIN32
        _lock = CreateMutex(NULL,FALSE,NULL);
#else
        pthread_mutex_init(&_lock, NULL);
#endif
    }

    void    release()
    {
        unlock(); //force unlock before release
#ifdef _WIN32

        if (_lock) CloseHandle(_lock);
        _lock = NULL;
#else
        pthread_mutex_destroy(&_lock);
#endif
    }

#ifdef _WIN32
    HANDLE  _lock;
#else
    pthread_mutex_t _lock;
#endif
};

class AutoLocker
{
public :
    AutoLocker(Locker &l): _binded(l)
    {
        _binded.lock();
    }

    void forceUnlock() {
        _binded.unlock();
    }
    ~AutoLocker() {_binded.unlock();}
    Locker & _binded;
};


}}

