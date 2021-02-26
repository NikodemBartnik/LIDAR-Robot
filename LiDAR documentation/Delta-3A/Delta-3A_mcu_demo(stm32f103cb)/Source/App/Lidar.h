#ifndef _LIDAR_H
#define _LIDAR_H

#include <stdint.h>

/********************************************************************************************************************/
//通讯相关参数定义
/********************************************************************************************************************/
#define FRAME_HEAD		                        	0xAA                                        //帧头
#define PROTO_VER 	            								0x04                                        //协议版本                                     
#define PC_TO_LD	    				        					0x00
#define LD_TO_PC						        						0x40
#define P_NONE                                  0xFF

#define FRAME_POINT_NUM_MAX											110
#define ONE_CIRCLE_POINT_NUM_MAX								FRAME_POINT_NUM_MAX*16
/********************************************************************************************************************/
//命令宏定义
/********************************************************************************************************************/
#define  CMD_SET_WORK_MODE                      0x01	
#define  CMD_ADJ_ROTA_SPEED											0x04
#define  CMD_REPORT_SCAN_INFO										0x14
#define  CMD_REPORT_DEV_ERR_INFO								0x16

//定义雷达齿轮个数
#define  TOOTH_NUM																	16

//雷达扫描状态
enum SCAN_STATE
{
	GRAB_SCAN_FIRST = 0,//零点
	GRAB_SCAN_ELSE_DATA //非零点
};	
enum SCAN_RESULT
{
	LIDAR_GRAB_ING = 0,
	LIDAR_GRAB_SUCESS,
	LIDAR_GRAB_ERRO,
	LIDAR_GRAB_ELSE
};
//雷达工作模式
enum WORK_MODE
{
	IDEL = 0,
	_8K_SCAN_MODE=1,
	SYS_RESET
};
//返回帧错误码
enum ERR_CODE{NO_ERR = 0,CMD_ERR,PARAM_LEN_ERR,PARAM_ERR,CHK_ERR};


/********************************************************************************************************************/
//通讯帧定义
/********************************************************************************************************************/
#pragma pack (1)
typedef struct
{
	uint8_t Header;
	uint16_t Len;
	uint8_t ProtoVer;
	uint8_t CmdId;
	uint16_t ParamLen;
	uint8_t *Data;
	uint16_t CheckSum;
}T_PROTOCOL;

//点信息定义
typedef struct 
{
	float Angle;
	float Distance;
}T_POINT;
//上报扫描距离信息帧定义
typedef struct 
{
	float RotaSpeed;
	float FrameStartAngle;
	uint8_t PointNum;//一帧的点数
	T_POINT Point[FRAME_POINT_NUM_MAX];//一帧 点信息
}T_FRAME_MEAS_INFO;

//上报设备故障信息帧定义
typedef struct 
{
	uint8_t ErrCode;
}T_DEVICE_ERR_INFO;

//雷达扫描信息定义
typedef struct
{
	uint8_t WorkMode;
	uint8_t State;
	uint8_t Result;
	float LastScanAngle;
	uint8_t ToothCount;
	uint16_t OneCriclePointNum;//一圈点数
	T_FRAME_MEAS_INFO FrameMeasInfo;//一帧测量信息
	T_POINT OneCriclePoint[ONE_CIRCLE_POINT_NUM_MAX];//一圈测量点信息：从零点开始，由16帧点信息组成
}T_LIDARSCANINFO;
#pragma pack ()

extern T_LIDARSCANINFO lidarscaninfo;

void Lidarscaninfo_Init(void);
uint8_t P_Cmd_Process(void);
uint16_t Big2LittleEndian_u16(uint16_t dat);


#endif
