#ifndef _LIDAR_H
#define _LIDAR_H

#include <stdint.h>

/********************************************************************************************************************/
//通讯相关参数定义
/********************************************************************************************************************/
#define FRAME_PARAM_MAX_RX_LEN		            (2000 + 10)	                                	//接收数据参数的最大长度
#define FRAME_PARAM_MAX_TX_LEN		            100		                                			//发送数据参数的最大长度

#define FRAME_HEAD		                        	0xAA                                        //帧头
#define FRAME_PROTOCAL_VERSION	  	            0x00                                        //协议版本                                     
#define FRAME_TYPE						               		0x61                                        //帧类型

#define FRAME_MEASURE_INFO											0xAD
#define FRAME_DEVICE_HEALTH_INFO								0xAE

//雷达扫描状态
enum SCANSTATE
{
	GRAB_SCAN_FIRST = 0,
	GRAB_SCAN_ELSE_DATA
};	
enum SCANRESULT
{
	LIDAR_GRAB_ING = 0,
	LIDAR_GRAB_SUCESS,
	LIDAR_GRAB_ERRO,
	LIDAR_GRAB_ELSE
};

//定义雷达齿轮个数
#define  TOOTH_NUM																	16

/********************************************************************************************************************/
//通讯帧定义
/********************************************************************************************************************/
#pragma pack (1)
typedef struct
{
	uint8_t Header;
	uint16_t Len;
	uint8_t Addr;
	uint8_t CmdType;
	uint8_t CmdId;
	uint16_t ParamLen;
	uint8_t *Data;
}T_PROTOCOL;

//点信息帧定义
typedef struct 
{
	float Angle;
	float Distance;
}T_POINT;
//测量帧定义
typedef struct 
{
	float RotateSpeed;
	float ZeroOffset;
	float FrameStartAngle;
	uint8_t PointNum;//一帧的点数
	T_POINT Point[100];//一帧 点信息
}T_FRAME_MEAS_INFO;

//设备健康信息帧
typedef struct 
{
	uint8_t ErrCode;
}T_DEVICE_HEALTH_INFO;

//雷达扫描信息定义
typedef struct
{
	uint8_t State;
	uint8_t Result;
	float LastScanAngle;
	uint8_t ToothCount;
	uint16_t OneCriclePointNum;//一圈点数
	T_FRAME_MEAS_INFO FrameMeasInfo;//一帧测量信息
	T_POINT OneCriclePoint[1000];//一圈测量点信息：从零点开始，由16帧点信息组成
}T_LIDARSCANINFO;
#pragma pack ()

extern T_LIDARSCANINFO lidarscaninfo;

void Lidarscaninfo_Init(void);
uint8_t P_Cmd_Process(void);


#endif
