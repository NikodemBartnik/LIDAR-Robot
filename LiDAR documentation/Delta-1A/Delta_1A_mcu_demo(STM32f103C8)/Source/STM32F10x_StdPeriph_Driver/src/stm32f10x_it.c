/**
  ******************************************************************************
  * @file    FLASH/Dual_Boot/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"


/**********************************************************************************************************************
* @brief  This function handles NMI exception.
* @param  None
* @retval None
***********************************************************************************************************************/
void NMI_Handler(void)
{
}

/**********************************************************************************************************************
* @brief  This function handles Hard Fault exception.
* @param  None
* @retval None
**********************************************************************************************************************/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/***********************************************************************************************************************
* @brief  This function handles Memory Manage exception.
* @param  None
* @retval None
***********************************************************************************************************************/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/***********************************************************************************************************************
* @brief  This function handles Bus Fault exception.
* @param  None
* @retval None
***********************************************************************************************************************/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/***********************************************************************************************************************
* @brief  This function handles Usage Fault exception.
* @param  None
* @retval None
***********************************************************************************************************************/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/***********************************************************************************************************************
* @brief  This function handles SVCall exception.
* @param  None
* @retval None
***********************************************************************************************************************/
void SVC_Handler(void)
{
}

/***********************************************************************************************************************
* @brief  This function handles Debug Monitor exception.
* @param  None
* @retval None
***********************************************************************************************************************/
void DebugMon_Handler(void)
{
}

/***********************************************************************************************************************
* @brief  This function handles PendSV_Handler exception.
* @param  None
* @retval None
***********************************************************************************************************************/
void PendSV_Handler(void)
{
}

/***********************************************************************************************************************
* @brief  This function handles SysTick Handler.
* @param  None
* @retval None
***********************************************************************************************************************/
void SysTick_Handler(void)
{
    
}

/***********************************************************************************************************************
* @brief  This function handles USART1 Handler.
* @param  None
* @retval None
***********************************************************************************************************************/




///***********************************************************************************************************************
//* @描述: 
//*
//* @参数: 无
//*
//* @返回值: 无
//**********************************************************************************************************************/
//void TIM1_UP_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)      //记数溢出
//	{
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//	}
//}

///***********************************************************************************************************************
//* @描述:
//*
//* @参数: 无
//*
//* @返回值: 无
//***********************************************************************************************************************/

//void TIM3_IRQHandler(void)   //TIM3中断
//{
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
//	{
//    	TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  
//    	
//	}
//}

///***********************************************************************************************************************
//* @描述: 
//*
//* @参数: 无
//*
//* @返回值: 无
//***********************************************************************************************************************/

//void TIM4_IRQHandler(void)
//{
//  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)      
//  {
//		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     

//  }
//}
/***********************************************************************************************************************/
