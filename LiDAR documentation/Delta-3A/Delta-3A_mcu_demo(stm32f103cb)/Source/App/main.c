/********************************************************************************************************************/
//文件名:   Main.c
//描述:     主程序
//版本:    
//日期:     
//作者:     
/********************************************************************************************************************/
#include "lidar.h"
#include "bsp_uart.h"
#include "check.h"


/********************************************************************************************/
//设置雷达工作模式 函数 (空闲，扫描，复位)
//参数:     无
//返回值:   无
/*****************************************************************************************/
void SetLidarWorkMode(uint8_t workmode)
{
	uint8_t i=0;
	uint16_t Len=0;
	T_PROTOCOL Preq;
	
	Preq.Header=FRAME_HEAD;
	Preq.Len=7+1;	
	Preq.ProtoVer=PROTO_VER;
	Preq.CmdId=PC_TO_LD|CMD_SET_WORK_MODE;
	Preq.ParamLen=1;
	
	Len = Preq.Len+2;
	Big2LittleEndian_u16(Preq.Len); 
	Big2LittleEndian_u16(Preq.ParamLen);
	memcpy(TxBuffer.Buff,&Preq,7);
	
	TxBuffer.Buff[7]=workmode;
		
	Preq.CheckSum=Calc_Pack_Checksum(TxBuffer.Buff,&TxBuffer.Buff[7],Len);
	TxBuffer.Buff[8]=Preq.CheckSum&0xFF;
	TxBuffer.Buff[9]=Preq.CheckSum>>8;
	
	for(i=0;i<Len;i++)
	{
		USART_SendData(USART1,TxBuffer.Buff[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//等待发送完
	}
}
/********************************************************************************************/
//设置雷达转速 函数 
//参数:     无
//返回值:   无
/*****************************************************************************************/
void SetLidarRotaSpeed(uint16_t speed)
{
	uint8_t i=0;
	uint16_t Len=0;
	T_PROTOCOL Preq;
	
	Preq.Header=FRAME_HEAD;
	Preq.Len=7+2;
	Preq.ProtoVer=PROTO_VER;
	Preq.CmdId=PC_TO_LD|CMD_ADJ_ROTA_SPEED;
	Preq.ParamLen=2;
	
	Len = Preq.Len+2;
	Big2LittleEndian_u16(Preq.Len); 
	Big2LittleEndian_u16(Preq.ParamLen);
	memcpy(TxBuffer.Buff,&Preq,7);
	
	TxBuffer.Buff[7]=speed&0xFF;
	TxBuffer.Buff[8]=speed>>8;
	
	Preq.CheckSum=Calc_Pack_Checksum(TxBuffer.Buff,&TxBuffer.Buff[7],Len);
	TxBuffer.Buff[9]=Preq.CheckSum&0xFF;
	TxBuffer.Buff[10]=Preq.CheckSum>>8;
	
	for(i=0;i<Len;i++)
	{
		USART_SendData(USART1,TxBuffer.Buff[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//等待发送完
	}
}


/********************************************************************************************************************/
//主程序
//参数:     无
//返回值:   int
/********************************************************************************************************************/
int main(void)
{ 
	uint16_t i=0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	USART1_Init(230400); 
	
	Lidarscaninfo_Init();
		
	//scan mode  AA 08 00 04 01 01 00 01 B9 00 
	SetLidarWorkMode(_8K_SCAN_MODE);
	
	//set Rotate speed 10r/s  AA 09 00 04 04 02 00 E8 03 A8 01
	//SetLidarRotaSpeed(1000);
	
  while(1)
	{
		ProcessUartRxData();
		if(lidarscaninfo.Result == LIDAR_GRAB_SUCESS)//scan one circle:ok
		{
			lidarscaninfo.Result=LIDAR_GRAB_ING;//reset lidarscaninfo.Result:scanning
			
			/**************Print One cirle: total numbers of point********************************************/
			printf("one circle point num:%d\n",lidarscaninfo.OneCriclePointNum);
		
			/*****lidarscaninfo.OneCriclePoint[lidarscaninfo.OneCriclePointNum]：存放一圈总点数的角度和距离*********/
			
			//打印某个点信息：一圈从零点开始，打印第100个点的角度和距离
			/*printf("point %d: angle=%5.2f,distance=%5.2fmm\n",
					100,
					lidarscaninfo.OneCriclePoint[100].Angle,
					lidarscaninfo.OneCriclePoint[100].Distance);*/
			
		}
	}
}
 /********************************************************************************************************************/
//One frame:
/*
AA BD 00 04 54 B6 00 00 02 50 46 A7 01 A2 01 A3 01 A3 01 A3 01 A3 01 A2 01 A3 01 A4 01 A5 
01 A5 01 A5 01 A5 01 A4 01 A4 01 A4 01 A5 01 A6 01 A7 01 A8 01 A9 01 AA 01 AB 01 AC 01 AE
01 AE 01 AD 01 AE 01 AE 01 AF 01 AF 01 B0 01 B0 01 B1 01 B2 01 B2 01 B3 01 B2 01 B2 01 B2 
01 B3 01 B3 01 B3 01 B4 01 B5 01 B4 01 B7 01 B7 01 B7 01 B8 01 B8 01 B9 01 BA 01 BA 01 BB 
01 BB 01 BC 01 BD 01 BF 01 BF 01 C1 01 C3 01 C4 01 C4 01 C3 01 C5 01 C7 01 C9 01 CA 01 CE 
01 CF 01 C9 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A8 15 00 00 65 13 00 00 00 
00 00 00 00 00 00 00 00 00 C6 36
*/