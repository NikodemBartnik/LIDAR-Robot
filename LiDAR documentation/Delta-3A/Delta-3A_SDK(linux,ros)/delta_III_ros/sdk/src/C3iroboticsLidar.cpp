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

/********************************* File includes **********************************/
#include "C3iroboticsLidar.h"

/******************************* Current libs includes ****************************/
#include <iostream>

/********************************** Name space ************************************/
using namespace everest;
using namespace everest::hwdrivers;

const u8 DEVICE_INFO_SIZE = 50;
const u8 DEVICE_SERIAL_NO_SIZE = 24;

/***********************************************************************************
Function:     C3iroboticsLidar
Description:  The constructor of C3iroboticsLidar
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
C3iroboticsLidar::C3iroboticsLidar()
{
    m_device_connect = NULL;
	m_lidar_erro = INIT_VALUE;
	
	m_lidar_work_mode = IDLE_MODE;
	m_lidar_work_status = 0;//LIDAR_IDLE_STATUS;
	m_last_work_status = LIDAR_IDLE_STATUS;
	m_laser_work_mode = OPEN;
	m_laser_work_status = LASER_OPEN_STATUS;
	m_device_wrong_code = 0x00;
	m_set_Meas_rate_status = LIDAR_MEAS_RATE_SETTING_STATUS;
	m_set_Rot_speed_status = LIDAR_ROT_SPEED_SETTING_STATUS;
	memset(&m_static_meas_info,0,sizeof(struct TLidarStaticMeasInfo));
	memset(&m_device_info,0,sizeof(struct TLidarDeviceInfo)); 
}

/***********************************************************************************
Function:     C3iroboticsLidar
Description:  The destructor of C3iroboticsLidar
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
C3iroboticsLidar::~C3iroboticsLidar()
{
	
}

/***********************************************************************************
Function:     initilize
Description:  Set device connect
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
bool C3iroboticsLidar::initilize(CDeviceConnection *device_connect)
{
    if(device_connect == NULL || device_connect->getStatus() != CDeviceConnection::STATUS_OPEN)
    {
        printf("[C3iroboticsLidar] Init failed Can not open device connect!\n");
        return false;
    }
    else
    {
        printf("[C3iroboticsLidar] Init device conenct sucessful!\n");
		m_device_connect = device_connect;
        m_receiver.setDeviceConnection(m_device_connect);
        return true;
    }
}


/***********************************************************************************
Function:     
Description:  
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/

bool C3iroboticsLidar::RecvAndAnalysisPthread(C3iroboticsLidar* plidar)
{
	pthread_t id;
	int ret = pthread_create(&id,0,RecvAndAnalysisPthreadCallback,(void*)plidar);
	if(ret == 0)
		return TRUE;
	else
		return FALSE;
}

void* C3iroboticsLidar::RecvAndAnalysisPthreadCallback(void* p)
{   
    C3iroboticsLidar* pc3iroboticslidar = static_cast<C3iroboticsLidar*>(p); 
	bool ret = FALSE;
    while(1)
    {  
    	ret = pc3iroboticslidar->m_receiver.receivePacket(&pc3iroboticslidar->m_packet);
        if(ret)
        {
    	  	pc3iroboticslidar->analysisPacket(pc3iroboticslidar->m_packet);       
        }
		usleep(50);
    }
  
}

void C3iroboticsLidar::analysisPacket(CLidarPacket &lidar_packet)//debug chenyf
{
    TLidarCommandID command_id = TLidarCommandID(lidar_packet.getCommandID());
    switch(command_id)
    {
		case SET_LIDAR_WORK_MODE://
			analysisWorkMode(lidar_packet);
			break;
		
		case ADJUST_LIDAR_ROTATIONL_SPEED://
			analysissetLidarRotationlSpeed(lidar_packet);
			break;
		
		case REPORT_DYNAMIC_SCAN_DATA:
			analysisReportNewtDynamicScanData(lidar_packet);
			break;
		
		case REPORT_DEVICE_WRONG_INFO:
			analysisReportDeviceWrongInfo(lidar_packet);
			break;
        default:
            printf("[C3iroboticsLidar] Special command id %d!\n", command_id);
    }
}
void C3iroboticsLidar::analysisReportDeviceWrongInfo(CLidarPacket &lidar_packet)
{
	u16 length = lidar_packet.getParamLength();
    u8 *buffer = lidar_packet.getParamPtr();
	m_device_wrong_code = CLidarPacket::bufToUByte(buffer);
	switch (m_device_wrong_code)
	{
		//case CCD_WRONG:
		//	printf("Report Lidar wrong info:CCD_WRONG; ");
		//	break;
		case LIDAR_ROT_SPEED:
			printf("Report Lidar wrong info:LIDAR_ROT_SPEED; ");
			break;
		case CAL_PARAM_ERROR:			
			printf("Report Lidar wrong info:CAL_PARAM_ERROR; ");
			break;
		default:
			//printf("Report Lidar other wornginfo:%d \n",m_device_wrong_code);
			break;		
	}
	//CLidarPacket::swap(buffer+1);
	//m_dynamic_scan.m_rotationl_speed = (float)(CLidarPacket::bufToUByte2(buffer+1)/100.0);
	//printf("Lidar rotate speed:%d\n",m_dynamic_scan.m_rotationl_speed);
}

void C3iroboticsLidar::analysisWorkMode(CLidarPacket &lidar_packet)
{
	u8 param = 0;
	param = CLidarPacket::bufToUByte(lidar_packet.getParamPtr());
	switch(m_lidar_work_mode)
	{
		case IDLE_MODE:
			if(param == EXECUTE_SUCCESS)
			{
				m_lidar_work_status = LIDAR_IDLE_STATUS;
				return;
			}
			break;
		case LOWSPEED_SCAN:
			if(param == EXECUTE_SUCCESS)
			{
				m_lidar_work_status = LIDAR_LOWSPEED_SCAN_STATUS;
				return;
			}
	
			break;
		case HIGHSPEED_SCAN:
			if(param == EXECUTE_SUCCESS)
			{
				m_lidar_work_status = LIDAR_HIGHSPEED_SCAN_STATUS;
				return;
			}
		
			break;
		case LIDAR_RESET:
			if(param == EXECUTE_SUCCESS)
			{
				m_lidar_work_status = LIDAR_RESET_STATUS;	
				return;
			}
			break;
		default:
			{
				printf("analysisWorkMode error!\n");
				return;
			}	
			break;
	}	
    switch(param)
    {
        case 0x01:
            m_lidar_erro = COMMONDID_ERROR;
            break;
        case 0x02:
            m_lidar_erro = PARAM_LENGTH_ERROR;
            break;
        case 0x03:
            m_lidar_erro = PARAM_ERROR;
            break;
        case 0x04:
            m_lidar_erro = VERIFY_ERROR;
            break;
        default:
			printf("analysisWorkMode return param error\n");
            break;
    }
		
}



void C3iroboticsLidar::analysisReportNewtDynamicScanData(CLidarPacket &lidar_packet)
{
	CLidarDynamicScan new_dynamic_scan;
	u16 length = lidar_packet.getParamLength();
    u8 *buffer = lidar_packet.getParamPtr();
	CLidarPacket::swap(buffer);
	m_dynamic_scan.m_rotationl_speed = (float)(CLidarPacket::bufToUByte2(buffer)/100.0);
	CLidarPacket::swap(buffer+2);
	new_dynamic_scan.m_grating_angle = (float)(CLidarPacket::bufToUByte2(buffer+2)/100.0);
	u16 num = (length - 4)/2;
	float per_changed_angle = 360.0/((float)num)/((float)m_dynamic_scan.m_grating_num);
	new_dynamic_scan.m_angle.resize(num);
	new_dynamic_scan.m_distance.resize(num);
	for(int i=0;i<num;i++)
	{
		new_dynamic_scan.m_angle[i]=new_dynamic_scan.m_grating_angle+i*per_changed_angle; 
		CLidarPacket::swap(buffer+4+i*2);
		new_dynamic_scan.m_distance[i]=(float)(CLidarPacket::bufToUByte2(buffer+4+i*2)/1000.0);
	}

	m_dynamic_scan.DynamicScanProcedure(new_dynamic_scan);
}
void CLidarDynamicScan::DynamicScanProcedure(CLidarDynamicScan &one_grating_dynamic_scan)
{
    switch(m_grab_scan_state)
    {
        case GRAB_SCAN_FIRST:
        {
            getStartTime();
			
            /* First scan come */
            if(isFirstGratingScan(one_grating_dynamic_scan))
            {
                resetScanGrab();
                grabFirstGratingScan(one_grating_dynamic_scan);
            }
            else
            {
               // printf("[C3iroboticsLidar] GRAB_SCAN_FIRST tooth scan angle %5.2f!\n",one_grating_dynamic_scan.m_grating_angle);
            }
			
            m_grab_result = LIDAR_GRAB_ING;
			return;
        }
        case GRAB_SCAN_ELSE_DATA:
        {
        	getEndTime();
            if(isTimeout())//timeout
            {	
            	//printf("[C3iroboticsLidar] grab scan is time out ! Reset grab scan state, current is %5.2f, last is %5.2f! \n", one_grating_dynamic_scan.m_grating_angle, m_last_scan_angle);                   
                m_grab_scan_state = GRAB_SCAN_FIRST;
                m_grab_result =  LIDAR_GRAB_ING;
				return;
            }
            getStartTime();
            /* Handle angle suddenly reduces */
            if(one_grating_dynamic_scan.m_grating_angle < m_last_scan_angle)
            {    
            	//printf("[C3iroboticsLidar] recieve wrong in this scan !!! Restart scan, current_grating_angle: %5.2f, last_grating_angle: %5.2f!\n",one_grating_dynamic_scan.m_grating_angle, m_last_scan_angle); 
                if(isFirstGratingScan(one_grating_dynamic_scan))//这次小于上一次角度，且角度为零：表示这周数据不完�?刚好重新开始下一周（这周数据丢掉�?
                {   
                 	getStartTime();
					resetScanGrab();
                	grabFirstGratingScan(one_grating_dynamic_scan);
                }
				else //这次角度小于上次角度，且角度不为零：表示这周和下周数据都不完�?重新矫正到第一个光栅开始扫描，计数（这周和下周数据都丢掉）
				{
					m_grab_scan_state = GRAB_SCAN_FIRST;
				}
				m_grab_result =  LIDAR_GRAB_ING;
				return;				
            }

            /* Insert grating scan in scan */
            insert(one_grating_dynamic_scan);
            m_grab_scan_count++;
            m_last_scan_angle = one_grating_dynamic_scan.m_grating_angle;

            /* Judge whether finish grab one compelte scan */
            if(m_grab_scan_count == m_grating_num)
            {
                m_grab_scan_state = GRAB_SCAN_FIRST;
                m_grab_result =  LIDAR_GRAB_SUCESS;
				return;
            }
            else
            {
                 m_grab_result =  LIDAR_GRAB_ING;
				 return;
            }
        }
        default:
           // printf("[C3iroboticsLidar] Uknow grab scan data state %d!\n",
                      //m_grab_scan_state);
        break;
    }
   // printf("[C3iroboticsLidar] combineScan should not come to here!\n");
    m_grab_result =  LIDAR_GRAB_ERRO;
}


void C3iroboticsLidar::analysissetLidarRotationlSpeed(CLidarPacket &lidar_packet)
{
	u8 param = 0;
	param = CLidarPacket::bufToUByte(lidar_packet.getParamPtr());
	if(param == EXECUTE_SUCCESS)
	{
		m_set_Rot_speed_status= LIDAR_ROT_SPEED_SETTED_STATUS;	
	}
	else
	{			
        switch(param)
        {
            case 0x01:
                m_lidar_erro = COMMONDID_ERROR;
                break;
            case 0x02:
                m_lidar_erro = PARAM_LENGTH_ERROR;
                break;
            case 0x03:
                m_lidar_erro = PARAM_ERROR;
                break;
            case 0x04:
                m_lidar_erro = VERIFY_ERROR;
                break;
            default:
                break;
        }
	}
}


TLidarError C3iroboticsLidar::setLidarWorkMode(TLidarWorkMode mode)
{
	int ret = 0;
	struct timespec starttime={0,0};
	struct timespec endtime={0,0};
	u64 timeout = 0;

	m_lidar_work_mode = mode;
    m_sender.WorkModePacket(m_packet,m_lidar_work_mode);
	ret = m_device_connect->write((char*)&m_packet.m_sendbuf[0],m_packet.m_params.sendbuf_size);
	if(ret>0)
	{
		clock_gettime(CLOCK_REALTIME, &starttime);
		while(1)
		{
			
			usleep(50);
			//printf("timeout=%d\n",timeout);
			//printf("m_lidar_work_status=%d\n",m_lidar_work_status);
			if(timeout>=100)
			{
				printf("TIME_OUT\n");
				return TIME_OUT;//timeout
			}
			
			switch(m_lidar_work_mode)
			{
				case IDLE_MODE:
					if(m_lidar_work_status == LIDAR_IDLE_STATUS)
					{					
						return EXECUTE_SUCCESS;	
					}
					break;
				case LOWSPEED_SCAN:
					if(m_lidar_work_status == LIDAR_LOWSPEED_SCAN_STATUS)
					{
						return EXECUTE_SUCCESS;	
					}
					break;
				case HIGHSPEED_SCAN:
					if(m_lidar_work_status == LIDAR_HIGHSPEED_SCAN_STATUS)
					{
						return EXECUTE_SUCCESS;	
					}
					break;	
				case LIDAR_RESET:
					if(m_lidar_work_status == LIDAR_RESET_STATUS)
					{
						m_lidar_work_status = LIDAR_IDLE_STATUS;
						return EXECUTE_SUCCESS;	
					}	
					break;	
				default:
					break; 
					
			}
			if(m_lidar_erro == COMMONDID_ERROR)
			{
				m_lidar_erro = INIT_VALUE; 
				return COMMONDID_ERROR;
			}
			if(m_lidar_erro == PARAM_LENGTH_ERROR)
			{
				m_lidar_erro = INIT_VALUE; 
				return PARAM_LENGTH_ERROR;
			}
			if(m_lidar_erro == PARAM_ERROR)
			{
				m_lidar_erro = INIT_VALUE; 
				return PARAM_ERROR;
			}
			if(m_lidar_erro == VERIFY_ERROR)
			{
				m_lidar_erro = INIT_VALUE; 
				return VERIFY_ERROR;
			}
			clock_gettime(CLOCK_REALTIME, &endtime);
			timeout=(endtime.tv_sec - starttime.tv_sec)*1000 + (endtime.tv_nsec - starttime.tv_nsec)/1000000;
			
		}		
	}	
	return WRITE_FAIL;	
   
}
TLidarError C3iroboticsLidar::setLidarRotationlSpeed(uint32_t speed)
{
	int ret = 0;
	struct timespec starttime={0,0};
	struct timespec endtime={0,0};
	u64 timeout = 0;

	m_sender.setLidarRotationlSpeedPacket(m_packet,speed);
	ret = m_device_connect->write((char*)&m_packet.m_sendbuf[0],m_packet.m_params.sendbuf_size);
	if(ret>0)
	{
		clock_gettime(CLOCK_REALTIME, &starttime);
		while(1)
		{
			usleep(50);
			if(timeout>=100)
			{
				return TIME_OUT;//timeout
			}
			if(m_set_Rot_speed_status == LIDAR_ROT_SPEED_SETTED_STATUS)
			{	
				m_dynamic_scan.m_rotationl_speed = speed;
				m_set_Rot_speed_status == LIDAR_ROT_SPEED_SETTING_STATUS;
				m_lidar_erro = INIT_VALUE; 
				return EXECUTE_SUCCESS;
			}
			if(m_lidar_erro == COMMONDID_ERROR)
			{
				m_lidar_erro = INIT_VALUE; 
				return COMMONDID_ERROR;
			}
			if(m_lidar_erro == PARAM_LENGTH_ERROR)
			{
				m_lidar_erro = INIT_VALUE; 
				return PARAM_LENGTH_ERROR;
			}
			if(m_lidar_erro == PARAM_ERROR)
			{
				m_lidar_erro = INIT_VALUE; 
				return PARAM_ERROR;
			}
			if(m_lidar_erro == VERIFY_ERROR)
			{
				m_lidar_erro = INIT_VALUE; 
				return VERIFY_ERROR;
			}			
			clock_gettime(CLOCK_REALTIME, &endtime);
			timeout=(endtime.tv_sec - starttime.tv_sec)*1000 + (endtime.tv_nsec - starttime.tv_nsec)/1000000;
		}	
	}
	return WRITE_FAIL;

}





