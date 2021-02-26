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
	lidar_drv = RSlidarDriver::CreateDriver();
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
        lidar_drv->stopScan();
    }
}

bool LidarMgr::onConnect(const char * port)
{
    if (_isConnected) return true;

    if (IS_FAIL(lidar_drv->connect(port, 230400))) return false;
    _isConnected = true;
    return true;
}