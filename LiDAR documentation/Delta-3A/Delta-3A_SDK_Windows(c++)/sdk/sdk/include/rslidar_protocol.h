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

// RP-Lidar Input Packets

#if defined(_WIN32)
#pragma pack(1)
#endif

//added by ruishi
//----------------------------------------
// Commands
//-----------------------------------------
#define COMM_HEAD_FLAGE				0xAA	//Frame header
#define COMM_FRAME_PC_TO_DEV		0x00	//attribute frame
#define COMM_FRAME_DEV_TO_PC		0x40	//command farme
#define COMM_FRAME_ERROR			0x80    //error frame

// MACRO
//-----------------------------------------
#define MAX_SAMPLE_NUMBERS_PER_NOTE		128
#define NUMBER_OF_TEETH					16
#define NOTE_BUFFER_PING				0x00
#define NOTE_BUFFER_PONG				0x01
#define	COMM_MASK						0x3F

/////////////命令帧命令字定义////////////////////////
typedef enum _cmd_code
{
	CMD_WORK_MODE = 0x01,						//停止
	CMD_SET_MOTOR_SPEED = 0x04,					//设置雷达转速
	CMD_READ_DEV_VER = 0x0D,					//读取设备版本信息
	CMD_REPORT_DIST = 0x14,						//雷达扫描距离信息
	CMD_REPORT_DEV_ERROR = 0x16,				//设雷故障
}CMD_CODE;

/////////////属性帧宏定义////////////////////////
typedef enum _work_mode_
{
	IDEL_MODE = 0x00,
	SLOW_SCAN_MODE = 0x01,
	RAPID_SCAN_MODE = 0x01,//现在只有一种模式
	RESET_MODE = 0x07,
}WORK_MODE;

/////////////错误代码宏定义////////////////////////
typedef enum _running_state
{
	PORT_RDY = 0,
	WORK_WELL,
	RX_DATA_TIMOUT,
	CHKSUM_ERROR,
	MOTOR_STALL,
}RUNNING_STATE;

/////////////通信帧结构////////////////////////////
typedef struct _comm_frame_t
{
	uint8_t		framehead;			//帧头
	uint16_t	frameLen;			//帧长度
	uint8_t		protocolver;		//协议版本
	uint8_t		command;			//命令字
	uint16_t	paramLen;			//参数长度
	uint8_t		paramBuf[0];		//参数
} __attribute__((packed)) COMM_FRAME_T;

//获取设备信息命令响应帧结构
typedef struct _lidar_response_devive_info_t {
	_u8		productsn[26];			//产品SN
	_u8		softwarever[15];		//软件版本
} __attribute__((packed)) LIDAR_RESPONSE_DEV_INFO_T;

//激光雷达单个测量点的信息
typedef struct _lidar_signal_distance_unit_t {
	_u8			signal;	//角度值
	float		motorspeed;
	float		angleoffset;
	_u16		angle;			//角度
	_u16		distance;	//距离值
} __attribute__((packed)) LIDAR_MEASURE_INFO_T;

#if defined(_WIN32)
#pragma pack()
#endif
