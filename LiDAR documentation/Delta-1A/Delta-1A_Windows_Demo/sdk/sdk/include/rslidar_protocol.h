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
#define COMM_FRAME_TYPE_ATTR		0x20	//attribute frame
#define COMM_FRAME_TYPE_CMD			0x40	//command farme
#define COMM_FRAME_TYPE_MESSAGE		0x61	//message frame

#define COMM_FRAME_TYPE_ERROR       0xAE    //error frame

// Parameter lengths
//-----------------------------------------
#define CMMD_SCAN_REQ_RSP_LENS			(0U)
#define CMMD_MOTOR_CONTROL_REQ_lENS		(1U)
#define CMMD_SET_MOTOR_RPM_REQ_LENS		(2U)
#define CMMD_SET_MEASURE_UNIT_REQ_LENS	(1U)
#define CMMD_RESET_REQ_LENS				(0U)
#define CMMD_DEV_RSP_LENS				(1U)

#define ATTR_DEV_INFO_PARAM_REQ_LENS	(0U)
#define ATTR_DEV_INFO_PARAM_RSP_LENS	(39U)
#define ATTR_DEV_INFO_HEALTH_RSP_LENS	(1U)
#define ATTR_DEV_INFO_MOTOR_RSP_LENS	(2U)

// Commmand response info
//-----------------------------------------
#define CMMD_FRAME_RSP_ERROR			0xC1
#define CMMD_FRAME_RSP_SUCCESS			0x00
#define CMMD_FRAME_RSP_CMD_ERROR		0x01
#define CMMD_FRAME_RSP_PARAM_LEN_ERROR	0x02
#define CMMD_FRAME_RSP_PARAM_ERROR		0x03
#define CMMD_FRAME_RSP_CRC_ERROR		0x04

#define ATTR_FRAME_RSP_ERROR			0xA1
#define ATTR_FRAME_RSP_CMD_ERROR		0x01
#define ATTR_FRAME_RSP_CRC_ERROR		0x02

// Motor start or stop control
//-----------------------------------------
#define CMMD_FRAME_PARAM_START_MOTOR	0x01
#define CMMD_FRAME_PARAM_STOP_MOTOR		0x00

// MACRO
//-----------------------------------------
#define MAX_SAMPLE_NUMBERS_PER_NOTE		128
#define NUMBER_OF_TEETH					16
#define NOTE_BUFFER_PING				0x00
#define NOTE_BUFFER_PONG				0x01

/////////////命令帧命令字定义////////////////////////
typedef enum _cmd_code
{
	CMD_STOP = 0x01,						//停止
	CMD_START_SCAN,							//开始扫描
	CMD_INIT_DLIS2K,						//初始化DLIS2K
	CMD_WRITE_CAIL_DATA,					//写标定数据 //三角激光雷达
	CMD_WRITE_CAIL_DATA_TOF,				//写标定数据 //TOF雷达
	CMD_LASER_CTRL,							//激光开关控制
	CMD_DLIS2K_SAMPLE_DATA_8BIT,			//设置输出CCD原始数据低8bit
	CMD_DLIS2K_PIXEL_POS,					//输出CCD的像素位置
	CMD_LENS_FOCUS_MODE,					//进入镜头调焦模式
	CMD_PIXEL_POS_CAIL,						//根据像素位置标定      
	CMD_MOTOR_WORK_CTRL,					//马达控制              
	CMD_MOTOR_DUTY_SET,						//设置马达驱动信号占空比
	CMD_MOTOR_RPM_SET,						//设置马达转速            	
	CMD_DEBUG_MESSAGE_EN,					//打印调试信息使能      
	CMD_EARE_CAIL_DATA,						//擦除标定数据          
	CMD_REGAIN_DEFAUT_SET,					//恢复出厂设置          
	CMD_DEVICE_ADDR_SET,					//设定设备地址          	
	CMD_SAMPLE_RATE_SET,					//设定测量速率          	
	CMD_DISTANCE_OFFSET,					//设置测量距离偏移量    
	CMD_HIGH_VOLT_ADJUST,					//微调高压              
	CMD_MEAS_PRINTF_EN,						//设置测量信息打印使能  
	CMD_HIGH_VOLT_RATE,						//设定高压系数          
	CMD_DISTANCE_RANGE,						//设定测量单位          
	CMD_MEAS_UNIT,							//设定测量单位            
	CMD_CAIL_MEAS,							//设置不带、带标定测量    
	CMD_WIRELESS_POWER_CTRL,				//无线供电开关控制        
	CMD_AUTO_MEAS,							//开启、关闭上电自动测量  
	CMD_WRITE_FLASH,						//写数据到FLASH
	CMD_WRITE_DEVICE_INFO,					//写产品相关信息
	CMD_SYSTEM_RST,							//系统复位
}CMD_CODE;

/////////////属性帧宏定义////////////////////////
typedef enum _attr_code
{

	ATTR_READ_DEVICE_INFO = 0x53,//获取设备信息            
	ATTR_READ_DLIS2K_REG,					//获取DLIS2K寄存器值      
	ATTR_READ_ONCE_MEAS,					//获取单次测量值          
	ATTR_READ_CAIL_DATA,					//获取标定数据            
	ATTR_READ_MOTOR_DUTY,					//获取马达驱动信号占空比  
	ATTR_READ_MOTOR_RPM,					//获取马达转速设定值      
	ATTR_READ_DEVICE_ADDR,					//获取设备地址            
	ATTR_READ_SAMPLE_RATE,					//获取测量速率            
	ATTR_READ_DISTANCE_OFFSET,				//获取偏移量            
	ATTR_READ_HIGH_VOLT,					//获取高压值            
	ATTR_READ_MEAS_KEYE_MESSAGE,			//获取测量关键信息        
	ATTR_READ_HIGHT_VOLT_RATE,				//获取高压系数          
	ATTR_READ_MEAS_RANGE,					//获取测量量程            
	ATTR_READ_MEAS_UNIT,					//获取测量单位            
	ATTR_READ_MEAS_MODE,					//获取测量模式            
	ATTR_READ_FLASH_DATA,					//读FLASH数据  
	ATTR_READ_DEVICE_HEALTH,				//获取设备健康信息
}ATTR_CODE;

/////////////信息帧宏定义////////////////////////
typedef enum _message_code
{

	MESSAGE_DEVICE_ERROR = 0xA4,			//报告设备故障
	MESSAGE_DLIS2K_SAMPLE_DATA_8BIT,		//报告CCD原始数据
	MESSAGE_DLIS2K_PIXEL_POS,				//报告CCD像素位置
	MESSAGE_DLIS2K_PIXEL_POS_DIS,			//报告CCD像素位置和距离
	MESSAGE_TOF_DISTANCE,					//报告TOF测距
	MESSAGE_LIDAR_DISTANCE,					//报告三角测距
}MESSAGE_CODE;

// Commonds
//-----------------------------------------


/////////////错误代码宏定义////////////////////////
typedef enum _running_state
{
	ExecSuccess = 0,
	RxDataError,
	Crc_Error,
	Motor_Error,
}RUNNING_STATE;

/////////////通信帧结构////////////////////////////
typedef struct _comm_frame_t
{
	uint8_t		frameStart;		//帧头
	uint16_t	frameLen;		//帧长度
	uint8_t		addr;			//地址码
	uint8_t		frameType;		//帧类型
	uint8_t		cmd;			//命令字
	uint16_t	paramLen;		//参数长度
	uint8_t		paramBuf[0];	//参数
} __attribute__((packed)) COMM_FRAME_T;



//sdk applicantion interface--------------
//----------------------------------------
#define START_MOTOR		true
#define STOP_MOTOR		false
//获取设备信息命令响应帧结构
typedef struct _rslidar_response_devive_info_t {
	_u8		productName[4];		//产品命名
	_u8		productDate[4];		//生产日期
	_u8		serialNum[8];		//序列号
	_u8		softwareVersion[11];//软件版本
	_u8		hardwareVersion[3];	//硬件版本
	_u8		manufacturerInfo[3];//制造商信息
	_u8		gearNum[3];			//码齿数
	_u8		measureRange[3];	//测量范围
} __attribute__((packed)) RSLIDAR_RESPONSE_DEVICE_INFO_T;

//获取雷达健康信息命令响应帧结构
typedef struct _rslidar_response_health_info_t {
	_u8		deviceHealthInfo;	//雷达健康信息
} __attribute__((packed)) RSLIDAR_RESPONSE_HEALTH_INFO_T;

//获取马达转数命令响应帧结构
typedef struct _rslidar_response_motor_info_t {
	_u16		motorSpeed;		//马达转数
} __attribute__((packed)) RSLIDAR_RESPONSE_MOTOR_INFO_T;

//获取测量单位命令响应帧结构
typedef struct _rslidar_response_meature_unit_t {
	_u16		meatureUint;	//测量单位
} __attribute__((packed)) RSLIDAR_RESPONSE_MEATURE_INIT_T;

//激光雷达单个测量点的信息
typedef struct _rslidar_signal_distance_unit_t {
	_u8			signalValue;	//角度值
	float			speed;
	float		angleoffset;
	_u16		angle;			//角度
	_u16		distanceValue;	//距离值
} __attribute__((packed)) RSLIDAR_SIGNAL_DISTANCE_UNIT_T;


#if defined(_WIN32)
#pragma pack()
#endif
