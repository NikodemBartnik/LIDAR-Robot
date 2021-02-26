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
#include "common.h"
#include "..\..\sdk\src\hal\locker.h"

class LidarMgr {

public:
    static LidarMgr & GetInstance();
    
    LidarMgr();
    ~LidarMgr();
    bool onConnect(const char * port);
    bool isConnected() const { return _isConnected; }
    void onDisconnect();

public:
	LIDAR_RESPONSE_DEV_INFO_T devinfo;

	static RSlidarDriver * lidar_drv;
    
protected:
    static LidarMgr * g_instance;
    static rs::hal::Locker g_oplocker;
    bool  _isConnected;
};