/*********************************************************************************
File name:	  C3iroboticsLidar.h
Author:       Kimbo
Version:      V1.7.1
Date:	 	  2017-02-03
Description:  3irobotics lidar sdk
Others:       None

History:
	1. Date:
	Author:
	Modification:
***********************************************************************************/

#ifndef EVEREST_LIDAR_C3IROBOTICSLIDAR_H
#define EVEREST_LIDAR_C3IROBOTICSLIDAR_H

/******************************* Current libs includes ****************************/
#include "CLidarPacket.h"
#include "CLidarPacketReceiver.h"
#include "CLidarPacketSender.h"
#include "CSerialConnection.h"
//#include "CSimulateSerial.h"

/******************************* System libs includes *****************************/
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
/******************************* Other libs includes ******************************/

namespace everest
{
	namespace hwdrivers
	{
		 enum TLidarGrabResult
        {
            LIDAR_GRAB_ING = 0,
            LIDAR_GRAB_SUCESS,
            LIDAR_GRAB_ERRO,
            LIDAR_GRAB_ELSE
        };

  		enum TLidarCommandID
    	{
			SET_LIDAR_WORK_MODE=0x01,
			LASER_CONTROL=0x05,
			ADJUST_MEASURING_RATE=0x07,
			ADJUST_LIDAR_ROTATIONL_SPEED=0x04,
			ADJUST_LASER_POWER_ID=0x05,
			EXIT_ADJUST_LASER_POWER=0x06,
			WRITE_CALIBRATION_DATA=0x08,
			WRITE_FIRMWARE_TO_FLASH=0x08,
			FIRMWARE_UPDATE_MARK=0x08,
			READ_LASER_PARAM=0x0A,
			READ_LASER_PARAM_SETVALUE=0x0B,
			READ_FIRM_FROM_FLASH=0x0A,
			READ_DEVICE_INFO=0x0D,
			READ_CALIBRATION_DATA=0x0F,
			READ_MEASURING_RATE=0x0F,
			READ_HEAT_ENGINE_TIME=0x10,
			REPORT_CCD_DATA_HI=0x011,
			REPORT_FIXED_POINT_CALIB_DATA=0x13,
			REPORT_FIXED_POINT_MEAS_DATA=0x012,
			REPORT_DYNAMIC_SCAN_DATA=0x14,
			REPORT_DEVCIE_VERSION=0x15,
			REPORT_DEVICE_WRONG_INFO=0x16		
		};
			
        
		enum TLidarWorkStatus
		{		
			LIDAR_IDLE_STATUS=1,
			LIDAR_LOWSPEED_SCAN_STATUS=2,
			LIDAR_HIGHSPEED_SCAN_STATUS=3,
			LIDAR_LENS_FOCUS_STATUS=4,
			LIDAR_LASER_FOCUS_STATUS=5,
			LIDAR_CALIBRATION_DATA_STATUS=6, 
			LIDAR_STATIC_MEASURE_STATUS=7,
			LIDAR_LASER_OPENED_STATUS=8,
			LIDAR_LASER_CLOSED_STATUS=9,
			LIDAR_DEVICE_INFO_RECVED_STATUS,
			LIDAR_ROT_SPEED_SETTING_STATUS,
			LIDAR_ROT_SPEED_SETTED_STATUS,
			LIDAR_MEAS_RATE_SETTING_STATUS,
			LIDAR_MEAS_RATE_SETTED_STATUS,
			LIDAR_ADJUST_LASER_POWER_STATUS,
			LIDAR_RESET_STATUS
		};
		enum TLaserWorkStatus
		{
			LASER_OPEN_STATUS=0,
			LASER_CLOSE_STATUS=1
		};
		enum TLidarWorkMode
		{
			IDLE_MODE=0x00,
			LOWSPEED_SCAN=0x02,
			HIGHSPEED_SCAN=0X01,
			LENS_FOCUS=0x05,
			LASER_FOCUS=0x03,
			CALIBRATION_DATA=0x04, 
			STATIC_MEASURE=0x05,
			ADJUST_LASER_POWER_MODE=0x08,
			LIDAR_RESET=0x07
		};
		enum TLaserControl
		{
			OPEN=0x00,
			CLOSE=0x01
		};					
		enum TLidarError
	    {	
		  	EXECUTE_SUCCESS=0x00,
			COMMONDID_ERROR=0x01,
			PARAM_LENGTH_ERROR=0x02,
			PARAM_ERROR=0x03,
			VERIFY_ERROR=0x04,
			TIME_OUT=0x05,
			WRITE_FAIL=0x06,
			INIT_VALUE=0X07,
			
	    };

		enum TLidarWrongInfo
		{
			//CCD_WRONG=0x01,
			LIDAR_ROT_SPEED=0x01,
			CAL_PARAM_ERROR=0x02
			
		};

		struct TLidarDeviceInfo
		{
			u8 serial_No[24];
			u8 software_version_id;
			u8 software_version_No[2];
			u8 year_latter_two[2];
			u8 month[2];
			u8 day[2];
			u8 hour[2];
			u8 minute[2];
			u8 hardware_version_id;
			u8 hardware_version_No[2];
			u8 protocol_id;
			u8 protocol_type_No[2];
			u8 measuring_range[3];
		};
		struct TLidarCalibrationData
		{
			TLidarCalibrationData()
			{

			}
			std::vector<u16> pixel;
			std::vector<u16> Z0;
			std::vector<u32> Fs;			
		
		};
		struct TLidarCCDDataHI
		{
			TLidarCCDDataHI()
			{
			}
			u16 pixel_position;
			std::vector<u8> raw_data;
		};
		struct TLidarStaticMeasInfo
		{
			u16 pixel_position;
			float distance;
		};	
		 
		class CLidarDynamicScan
	    {
	    	public:
				enum TGrabScanState
                {
                    GRAB_SCAN_FIRST = 0,
                    GRAB_SCAN_ELSE_DATA
                };
		        CLidarDynamicScan()
				{ 
					m_grating_angle = 0.0;
					m_rotationl_speed = 0.0;
					m_measuring_rate = 0;
					m_grab_scan_state = GRAB_SCAN_FIRST;
					m_start_time_ms = 0;
					m_end_time_ms = 0;
					m_grab_result = LIDAR_GRAB_ING;
					m_grab_scan_count = 0;
					m_last_scan_angle = 0.0;
					m_timeout_ms=5000;
					m_grating_num=16;
				}
				
				~CLidarDynamicScan()
				{		
				}

				void DynamicScanProcedure(CLidarDynamicScan &DynamicScan);								
				void getStartTime(void)
				{
					struct timespec starttime={0,0};
					clock_gettime(CLOCK_REALTIME, &starttime);
					m_start_time_ms = starttime.tv_sec*1000+starttime.tv_nsec/1000000;
				}
				void getEndTime(void)
				{
					struct timespec endtime={0,0};
					clock_gettime(CLOCK_REALTIME, &endtime);
					m_end_time_ms = endtime.tv_sec*1000+endtime.tv_nsec/1000000;
				}

				bool isTimeout(void)
				{
					if((m_end_time_ms-m_start_time_ms)>m_timeout_ms)
						return TRUE;
					else
						return FALSE;
				}

				int getSize() {return m_angle.size();}
				
				TLidarGrabResult getGrabResult(void){return m_grab_result;}
				void resetGrabResult(void){ m_grab_result=LIDAR_GRAB_ING;}
				 
				void insert(CLidarDynamicScan &DynamicScan)
		        {
	                this->m_angle.insert(this->m_angle.end(), DynamicScan.m_angle.begin(), DynamicScan.m_angle.end());
	                this->m_distance.insert(this->m_distance.end(), DynamicScan.m_distance.begin(), DynamicScan.m_distance.end());
	               // this->m_signal.insert(this->m_signal.end(), DynamicScan.m_signal.begin(), DynamicScan.m_signal.end());
		        }
				void clear()
		        {
		            m_angle.clear();
		            m_distance.clear();
		           // m_signal.clear();
		        } 		
			private:
				bool isFirstGratingScan(CLidarDynamicScan &DynamicScan)
				{
				    return DynamicScan.m_grating_angle < 0.0001? true: false;
				}
				void grabFirstGratingScan(CLidarDynamicScan &DynamicScan)
				{
					m_grab_scan_state = GRAB_SCAN_ELSE_DATA;
					m_grab_scan_count = 1;
					m_last_scan_angle = DynamicScan.m_grating_angle;
					insert(DynamicScan);
					
				}
				void resetScanGrab(void)
				{
					clear();
					m_grab_scan_state = GRAB_SCAN_FIRST;
					m_grab_scan_count = 0;
					m_last_scan_angle = 0.0;
				}

            public:
		        std::vector<float> 		m_angle;//per point angle
		        std::vector<float>		m_distance;
		        //std::vector<u8>   		m_signal;
				u8				m_grating_num;
		        float 					m_grating_angle;//per grating angle
		        float                   m_rotationl_speed;
				u16						m_measuring_rate;
				TLidarGrabResult 		m_grab_result;
				
			private:
				TGrabScanState 			m_grab_scan_state;
				
                int             		m_grab_scan_count;
                float          			m_last_scan_angle; 
				u64 					m_start_time_ms;
				u64						m_end_time_ms;
				u16				m_timeout_ms;
				

	    };

		class C3iroboticsLidar
		{
            public:               
                /* Constructor */
                C3iroboticsLidar();

                /* Destructor */
                ~C3iroboticsLidar();

                /* Set device connect */
                bool initilize(CDeviceConnection *device_connect);
				
				TLidarError setLidarWorkMode(TLidarWorkMode mode);
				TLidarError setLidarRotationlSpeed(uint32_t speed);       
				static void* RecvAndAnalysisPthreadCallback(void* p);
                bool RecvAndAnalysisPthread(C3iroboticsLidar* plidar);
                CLidarDynamicScan& getLidarDynamicScan(void) { return m_dynamic_scan; }
				
			private:
				void analysisPacket(CLidarPacket &lidar_packet);
				void analysisWorkMode(CLidarPacket &lidar_packet);
				void analysissetLidarRotationlSpeed(CLidarPacket &lidar_packet);
				void analysisReportNewtDynamicScanData(CLidarPacket &lidar_packet);
				void analysisReportDeviceWrongInfo(CLidarPacket &lidar_packet);
			
            public:
				struct TLidarDeviceInfo m_device_info;
				TLidarCCDDataHI			m_CCD_data_HI;
				struct TLidarStaticMeasInfo   m_static_meas_info;
				u8 						m_device_wrong_code;
				CLidarDynamicScan		m_dynamic_scan;

            private:
                CDeviceConnection       *m_device_connect;
                CLidarPacketReceiver    m_receiver;
				CLidarPacketSender      m_sender;
                TLidarError             m_lidar_erro;
                CLidarPacket            m_packet;				
				u8 						m_lidar_work_status;
				u8 						m_laser_work_status;
				u8						m_set_Rot_speed_status;
				u8						m_set_Meas_rate_status;
				u8						m_last_work_status;
				u8						m_lidar_work_mode;
				u8						m_laser_work_mode;
                
		};
	}
}

#endif


