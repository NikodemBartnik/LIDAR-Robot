/**
  ******************************************************************************
  * @file    bsp_LIDAR_usart.c
  * @author  
  * @version V1.0
  * @date    
  * @brief   接收LIDAR定位信息串口
  ****************************************************************************
**/
  
#include "bsp_uart.h"
#include "lidar.h"

T_TX_BUFF TxBuffer;
T_RX_BUFF RxBuffer;


 /**
  * @brief  USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无 
  */
void USART1_Init(int baud)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);//使能USART1时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
		
 	USART_DeInit(USART1);  //复位串口1

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = baud;            //设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;         //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //开启接收中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  //开启接受空闲中断
	USART_Cmd(USART1, ENABLE);                      //使能串口

	USART_ClearITPendingBit(USART1, USART_IT_RXNE);   //清除接收中断标志
	USART_ClearITPendingBit(USART1, USART_IT_IDLE);   //清除接收空闲中断标志

	/* Enable USART1 DMA Rxrequest */
//	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);    //开启USART1的DMA发送使能
//	USART_ClearFlag(USART1, USART_FLAG_TC);           //清除发送完成标志
	
	  //USART1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00 ; //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure); //初始化NVIC寄存器
}

//重定向c库函数printf到串口
int fputc(int ch, FILE *f) 
{	
		USART_SendData(USART1, (uint8_t) ch);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
		return (ch);
}
/********************************************************************************************************************/
//置位帧接收完成标志
//参数:    无
//返回值:  无
/********************************************************************************************************************/
 
void RxDataComplete(void)
{
	if((RxBuffer.Rdy == 0)&&(RxBuffer.Len > 0))
	{
		if(RxBuffer.Len > 3)
			RxBuffer.Rdy = 1;
		else
			RxBuffer.Len = 0;
	}
}

/********************************************************************************************************************/
//写数据到接收缓存
//参数:
//         data:    接收到的数据
//返回值:  无
/********************************************************************************************************************/
void UartReceive(u8 data)
{
	if(RxBuffer.Rdy == 0)
	{
		RxBuffer.Buff[RxBuffer.Len++] = data;
		if(RxBuffer.Len >= sizeof(RxBuffer.Buff))
			RxBuffer.Rdy = 1;
	}
}

/***********************************************************************************************************************
* @brief  This function handles USART1 Handler.
* @param  None
* @retval None
***********************************************************************************************************************/
void USART1_IRQHandler(void)
{
    volatile u8 Temp;
    if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) != RESET)
    {
        UartReceive(USART1->DR);
        USART_ClearFlag(USART1,USART_FLAG_RXNE);
    }
    if(USART_GetFlagStatus(USART1,USART_FLAG_IDLE) != RESET)
    {
        RxDataComplete();
        USART_ClearFlag(USART1,USART_FLAG_IDLE);
    }

    Temp = USART1->SR;
    Temp = USART1->DR;
}

/********************************************************************************************************************/
//接收数据处理函数
//参数:    无
//返回值:  无
/********************************************************************************************************************/
void ProcessUartRxData(void)
{
	if(RxBuffer.Rdy == 0)
		return;

	if(RxBuffer.Rdy > 0)
		P_Cmd_Process();
	RxBuffer.Len = 0;
	RxBuffer.Rdy = 0;
}


/*********************************************END OF FILE**********************/
