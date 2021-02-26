/********************************************************************************************************************/
//文件名:   Main.c
//描述:     主程序
//版本:    
//日期:     
//作者:     
/********************************************************************************************************************/
#include "lidar.h"
#include "bsp_uart.h"

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
	
  while(1)
	{
		ProcessUartRxData();
		if(lidarscaninfo.Result == LIDAR_GRAB_SUCESS)//扫描到完整一圈
		{
			lidarscaninfo.Result=LIDAR_GRAB_ING;//恢复正在扫描状态
			
			/**************打印扫描一圈的总点数：lidarscaninfo.OneCriclePointNum*********************************************/
			printf("one circle point num:%d\n",lidarscaninfo.OneCriclePointNum);
		
			/*****lidarscaninfo.OneCriclePoint[lidarscaninfo.OneCriclePointNum]：存放一圈总点数的角度和距离*********/
			
			//打印某个点信息：一圈从零点开始，打印第100个点的角度和距离
			/*printf("point %d: angle=%5.2f,distance=%5.2fmm\n",100,
					lidarscaninfo.OneCriclePoint[100].Angle,
					lidarscaninfo.OneCriclePoint[100].Distance);*/
			
		}
	}
}
 /********************************************************************************************************************/

