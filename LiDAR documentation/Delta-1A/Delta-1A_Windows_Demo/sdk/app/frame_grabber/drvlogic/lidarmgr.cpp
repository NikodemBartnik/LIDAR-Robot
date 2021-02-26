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

#include "../stdafx.h"
#include "lidarmgr.h"

LidarMgr * LidarMgr::g_instance = NULL;
rs::hal::Locker LidarMgr::g_oplocker;
RSlidarDriver * LidarMgr::lidar_drv = NULL;

LidarMgr & LidarMgr::GetInstance()
{
    if (g_instance) return *g_instance;
    rs::hal::AutoLocker l(g_oplocker);

    if (g_instance) return *g_instance;
    g_instance = new LidarMgr();
	lidar_drv = RSlidarDriver::CreateDriver(RSlidarDriver::DRIVER_TYPE_SERIALPORT);
    return *g_instance;
}

LidarMgr::LidarMgr()
    : _isConnected(false)
{

}

LidarMgr::~LidarMgr()
{
    rs::hal::AutoLocker l(g_oplocker);
    onDisconnect();
	delete g_instance;
	g_instance = NULL;
	lidar_drv->DisposeDriver(lidar_drv);
}

void LidarMgr::onDisconnect()
{
    if (_isConnected) {
        lidar_drv->stop();
    }
}

bool  LidarMgr::checkDeviceHealth(int * errorCode)
{
  
    int errcode = 0;
    bool ans = false;

    if (!_isConnected) {
        errcode = RESULT_OPERATION_FAIL;
        return false;
    }

	RSLIDAR_RESPONSE_HEALTH_INFO_T healthInfo;
	if (IS_FAIL(lidar_drv->getHealth(healthInfo))) {
        errcode = RESULT_OPERATION_FAIL;
		return false;
    }

	if (!(0x02 == healthInfo.deviceHealthInfo || 0x00 == healthInfo.deviceHealthInfo))
	{
		errcode = -1;
		return false;
	}

    ans = true;

    if (errorCode) *errorCode = errcode;
    return ans;
}

bool LidarMgr::onConnect(const char * port)
{
    if (_isConnected) return true;

    if (IS_FAIL(lidar_drv->connect(port, 230400))) return false;
	//if (IS_FAIL(lidar_drv->connect(port, 921600))) return false;  //20160803,串口波特率修改为921600
    // retrieve the devinfo
    //u_result ans = lidar_drv->getDeviceInfo(&devinfo);

    //if (IS_FAIL(ans)) {
    //    return false;
    //}

    _isConnected = true;
    return true;
}