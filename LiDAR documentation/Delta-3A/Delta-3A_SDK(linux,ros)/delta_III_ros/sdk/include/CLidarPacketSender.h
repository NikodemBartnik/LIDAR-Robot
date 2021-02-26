/*********************************************************************************
File name:	  CLidarPacketSender.h
Author:       Kimbo
Version:      V1.7.1
Date:	 	  2017-02-03
Description:  lidar packet sender
Others:       None

History:
	1. Date:
	Author:
	Modification:
***********************************************************************************/
#ifndef EVEREST_LIDAR_CLIDARPACKETSENDER_H
#define EVEREST_LIDAR_CLIDARPACKETSENDER_H

/******************************* Current libs includes ****************************/
#include "CLidarPacket.h"

/******************************* Current libs includes ****************************/
#include "CCountDown.h"

/******************************* System libs includes *****************************/
#include <vector>
#include <fstream>
#include <string.h>
#include "CDeviceConnection.h"

namespace everest
{
	namespace hwdrivers
	{
      
		class CLidarPacketSender
		{
            public:
                /* Constructor */
                CLidarPacketSender();

                /* Destructor */
                ~CLidarPacketSender();
                //void setDeviceConnection(CDeviceConnection *device_connection) { m_device_conn = device_connection; } 
				void WorkModePacket(CLidarPacket &packet,u8 mode);
				void LaserControlPacket(CLidarPacket &packet,u8 mode);
				void LidarCalibrationDataPacket(CLidarPacket &packet);
				void LidarDeviceInfoPacket(CLidarPacket &packet);
				void setLidarRotationlSpeedPacket(CLidarPacket &packet,uint32_t speed);
				void setLidarMeasuringRatePacket(CLidarPacket &packet,uint32_t rate);
				void readLidarMeasuringRatePacket(CLidarPacket &packet);

            public:
				enum LiDARWorkMode
				{
					STOP_MEASURE=0x00,
					DYNAMIC_SCAN=0x01,
					LENS_FOCUS=0x03,
					LASER_FOCUS=0x04,
					CALIBRATION_DATA=0x05, 
					STATIC_MEASURE=0x06,
					LIDAR_RESET=0x07
				};

				enum LaserControl
				{
					OPEN=0x00,
					CLOSE=0x01
				};					

            private:
               // CDeviceConnection 	*m_device_conn;
			
		};
	}
}

#endif


