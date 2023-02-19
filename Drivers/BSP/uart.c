/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : uart.c
  * @brief          : uart bsp
  * @author			: ZYX
  * @time			: 2020.11
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 SZCHWS.
  * All rights reserved.
  ******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "stm32l4xx_hal.h"
#include "uart.h"
#include "sys_config.h"
#include "ble.h"
#include "utility.h"

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

uint8_t send2Datax[70] = {0xfa,0xff,0x36,0x1e,0x40,0x20,0x0c,0xbe,0x0e,0xc6,0x9a,0xbd,0x52,0x6a,0xbe,0x41,0x1f,
	0x27,0x08,0x80,0x20,0x0c,0x3d,0xa4,0xbf,0x4d,0xbc,0x4c,0x2f,0xe0,0xbc ,0x90 ,0x29 ,0x79 ,0xb1 ,0xfa ,0xff ,
	0x36,0x1e ,0x40 ,0x20 ,0x0c ,0xbe ,0x03 ,0xff ,0x98 ,0xbd ,0x22 ,0x98 ,0x22 ,0x41 ,0x1e ,0x78 ,0xa4 ,0x80 ,
	0x20 ,0x0c,	0x3d ,0x37 ,0x21 ,0xd6 ,0xbc ,0x85 ,0x2e ,0x89 ,0xbc ,0x8f ,0x24 ,0x89 ,0xce};


uint8_t uart1_rxbuffer[UART_RX_LENMAX]={0};
uint8_t uart2_rxbuffer[UART_RX_LENMAX]={0};
uint8_t uart3_rxbuffer[UART_RX_LENMAX]={0};
uint8_t uart4_rxbuffer[10]={0};

uint8_t imu1_rxbuf[IMU_RX_BUFLEN]={0};
uint8_t imu2_rxbuf[IMU_RX_BUFLEN]={0};
uint8_t uRec_num=0;

uint8_t aRx1Buffer=0;
uint8_t aRx2Buffer=0;
uint8_t aRx3Buffer=0;
uint8_t aRx4Buffer=0;

uint8_t rxRec1Cnt=0;
uint8_t rxRec2Cnt=0;
uint8_t rxRec3Cnt=0;
uint8_t rxRec4Cnt=0;

uint8_t rxBleCommBuf[UART4_RX_BUFLEN_MAX]={0};
uint8_t RXBLE_Flag = 0;

extern volatile bleData ble_data;

extern volatile uint8_t SYNCFnished;
extern volatile uint32_t sysTime_ms;
extern volatile uint32_t sysTime_uart_it;
extern volatile uint32_t sysTime_sync_it;
extern volatile uint32_t sysTime_ua_min_sy;
extern volatile uint8_t sysTimegetFlag;

extern volatile uint32_t uart1Com;
extern volatile uint32_t uart2Com;
extern volatile uint32_t uart3Com;
extern volatile uint32_t uart4Com;

extern void imu_data_handle(uint8_t *rebuf, uint8_t pnum, uint16_t lenth);
extern void imu_data_handle_sequence(uint8_t *rebuf, uint8_t pnum, uint16_t lenth);
extern void imu_data_check_lzf(uint8_t *rebuf, uint8_t pnum, uint16_t lenth);

void UARTError_Handler(void)
{

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{
	uint8_t UART4_InitResult =0;

	/* USER CODE BEGIN UART4_Init 0 */

	/* USER CODE END UART4_Init 0 */

	/* USER CODE BEGIN UART4_Init 1 */

	/* USER CODE END UART4_Init 1 */
	huart4.Instance = UART4;
	huart4.Init.BaudRate = 115200;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart4) != HAL_OK)
	{
		UARTError_Handler();
	}

	UART4_InitResult = HAL_UART_Receive_IT(&huart4, (uint8_t*)(&aRx4Buffer), 1);
	if(UART4_InitResult != HAL_OK)
	{
		UARTError_Handler();
	}

	/* USER CODE BEGIN UART4_Init 2 */

	/* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{
	uint8_t UART1_InitResult =0;

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
	UARTError_Handler();
	}

	memset(uart1_rxbuffer,0,UART1_RX_BUFLEN);

//	UART1_InitResult = HAL_UART_Receive_IT(&huart1, (uint8_t*)&uart1_rxbuffer, UART1_RX_BUFLEN);
	UART1_InitResult = HAL_UART_Receive_IT(&huart1, (uint8_t*)&aRx1Buffer, 1);

	if(UART1_InitResult != HAL_OK)
	{
	  UARTError_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
	uint8_t UART2_InitResult =0;

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    UARTError_Handler();
  }

  UART2_InitResult = HAL_UART_Receive_IT(&huart2, (uint8_t*)&aRx2Buffer, 1);
  if(UART2_InitResult != HAL_OK)
  {
	UARTError_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{
	uint8_t UART3_InitResult =0;

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart3) != HAL_OK)
	{
	UARTError_Handler();
	}


	memset(uart3_rxbuffer,0,UART3_RX_BUFLEN);

//  UART3_InitResult = HAL_UART_Receive_IT(&huart3, (uint8_t*)&uart3_rxbuffer, UART3_RX_BUFLEN);
	UART3_InitResult = HAL_UART_Receive_IT(&huart3, (uint8_t*)&aRx3Buffer, 1);

	if(UART3_InitResult != HAL_OK)
	{
	UARTError_Handler();
	}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

void uart1_sendStr(uint8_t *sbuf,uint16_t len)
{
	uint32_t ret;

	ret = HAL_UART_Transmit_DMA(&huart1,sbuf,len);

	if(ret!=HAL_OK)
	{
		UARTError_Handler();
	}

}


void uart2_sendStr(uint8_t *sbuf,uint16_t len)
{
	uint32_t ret;

	ret = HAL_UART_Transmit_DMA(&huart2,sbuf,len);

	if(ret!=HAL_OK)
	{
		UARTError_Handler();
	}

}

void uart3_sendStr(uint8_t *sbuf,uint16_t len)
{
	uint32_t ret;

	ret = HAL_UART_Transmit_DMA(&huart3,sbuf,len);

	if(ret!=HAL_OK)
	{
		UARTError_Handler();
	}

}

void uart4_sendStr(uint8_t *sbuf,uint16_t len)
{
	uint32_t ret;

	ret = HAL_UART_Transmit_DMA(&huart4,sbuf,len);

	if(ret!=HAL_OK)
	{
		UARTError_Handler();
	}

}

uint8_t buff[35]={0};

void uart1_sendtest(void)
{
	uint32_t ret;

	for(uint8_t i=0;i<35;i++)
	{
		buff[i] = i;
	}
	buff[0] = 0xFA;
	buff[1] = 0xFF;
	
	ret = HAL_UART_Transmit_DMA(&huart1,buff,35);

	if(ret!=HAL_OK)
	{
		UARTError_Handler();
	}
}

void uart2_sendtest(void)
{
	uint32_t ret;

	for(uint8_t i=0;i<35;i++)
	{
		buff[i] = i;
	}
	buff[0] = 0xFA;
	buff[1] = 0xFF;
	
	ret = HAL_UART_Transmit_DMA(&huart2,buff,35);

	if(ret!=HAL_OK)
	{
		UARTError_Handler();
	}

}

void uart3_sendtest(void)
{
	uint32_t ret;

	for(uint8_t i=0;i<35;i++)
	{
		buff[i] = i;
	}
	buff[0] = 0xFA;
	buff[1] = 0xFF;
	
	ret = HAL_UART_Transmit_DMA(&huart3,buff,35);

	if(ret!=HAL_OK)
	{
		UARTError_Handler();
	}

}

void uart4_sendtest(void)
{
	uint32_t ret;

	for(uint8_t i=0;i<35;i++)
	{
		buff[i] = i;
	}
	buff[0] = 0xFA;
	buff[1] = 0xFF;
	
	ret = HAL_UART_Transmit_DMA(&huart4,buff,35);

	if(ret!=HAL_OK)
	{
		UARTError_Handler();
	}

}




void uart_init(void)
{

	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_UART4_Init();

}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* ADD_OPTION */
	
	if(huart->Instance==USART1)	//板载IMU
	{
		uart1_rxbuffer[rxRec1Cnt++] = aRx1Buffer;

		if(uart1_rxbuffer[0]!=0xFA || rxRec1Cnt>UART1_RX_BUFLEN)
		{
			rxRec1Cnt = 0;
			memset(uart1_rxbuffer,0,sizeof(uart1_rxbuffer));

		}
		else if(uart1_rxbuffer[0]==0xFA && uart1_rxbuffer[1]==0xFF && rxRec1Cnt==UART1_RX_BUFLEN && uart1_rxbuffer[UART1_RX_BUFLEN-1] == CheckSum(&uart1_rxbuffer[1], UART1_RX_BUFLEN-2))
		{
			if(sysTimegetFlag)
			{
				sysTimegetFlag = 0;
				sysTime_uart_it = sysTime_ms;	//sync_it - uart_it = 58ms
				sysTime_ua_min_sy = sysTime_uart_it - sysTime_sync_it;
			}

	#if (IMUModeSelect == SingleIMUMode)
			HAL_UART_Transmit_DMA(&huart4,uart1_rxbuffer,UART1_RX_BUFLEN);	//串口1发送数据，BLE透传用。
	#else

			imu_data_handle_sequence(uart1_rxbuffer,1,UART1_RX_BUFLEN);	//板载IMU数据接收处理
	//		HAL_UART_Transmit_DMA(&huart3,uart1_rxbuffer,UART1_RX_BUFLEN);
			
	//		memcpy(imu1_rxbuf,uart1_rxbuffer,UART1_RX_BUFLEN);
	//		memset(uart1_rxbuffer,0,sizeof(uart1_rxbuffer));
	//		uRec_num ++;
	#endif
			uart1Com = 0;
			rxRec1Cnt = 0;
			
		}
		
		HAL_UART_Receive_IT(&huart1, (uint8_t*)&aRx1Buffer,1);	// 重新使能串口1接收中断
	}
	else if(huart->Instance==USART2)
	{
	
		
		uart2_rxbuffer[rxRec2Cnt++] = aRx2Buffer;
		

		if(uart2_rxbuffer[0]!=0xFA || rxRec2Cnt>UART2_RX_BUFLEN)
		{
			rxRec2Cnt = 0;
			memset(uart2_rxbuffer,0,sizeof(uart2_rxbuffer));

		}
		else if(uart2_rxbuffer[0]==0xFA && uart2_rxbuffer[1]==0xFF && rxRec2Cnt==UART2_RX_BUFLEN)
		{
			if(sysTimegetFlag)
			{
				sysTimegetFlag = 0;
				sysTime_uart_it = sysTime_ms;	//sync_it - uart_it = 208ms
				sysTime_ua_min_sy = sysTime_uart_it - sysTime_sync_it;
			}

			rxRec2Cnt = 0;
			uart2Com = 0;
		}
		
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&aRx2Buffer,1);	// 重新使能串口2接收中断
		
	}
	else if(huart->Instance==USART3)	
	{
		uart3_rxbuffer[rxRec3Cnt++] = aRx3Buffer;

		if(uart3_rxbuffer[0]!=0xFA || rxRec3Cnt>UART3_RX_BUFLEN)
		{
			rxRec3Cnt = 0;
			memset(uart3_rxbuffer,0,sizeof(uart3_rxbuffer));

		}
		else if(uart3_rxbuffer[0]==0xFA && uart3_rxbuffer[1]==0xFF && rxRec3Cnt==UART3_RX_BUFLEN)
		{
			
		#if (IMUModeSelect == DoubleIMUMode)
			imu_data_handle_sequence(uart3_rxbuffer,2,UART3_RX_BUFLEN);	//挂载IMU数据处理
//			memcpy(imu2_rxbuf,uart3_rxbuffer,UART3_RX_BUFLEN);
//			memset(uart3_rxbuffer,0,sizeof(uart3_rxbuffer));
//			uRec_num ++;
		#endif
			uart3Com = 0;
			rxRec3Cnt = 0;
			
		}
		
		HAL_UART_Receive_IT(&huart3, (uint8_t*)&aRx3Buffer,1);	// 重新使能串口3接收中断
	}
	else if(huart->Instance==UART4)
	{
		//蓝牙透传串口数据处理
		uart4_rxbuffer[rxRec4Cnt++] = aRx4Buffer;

#if 0
		if(uart4_rxbuffer[0]!=0xFA || rxRec4Cnt>(UART4_RX_BUFLEN+2))
		{
			rxRec4Cnt = 0;
			memset(uart4_rxbuffer,0,sizeof(uart4_rxbuffer));
		}

		if(uart4_rxbuffer[0]==0xFA && uart4_rxbuffer[2]<rBLETime && rxRec4Cnt==UART4_RX_BUFLEN)
		{
			memset(rxBleCommBuf,0,sizeof(rxBleCommBuf));
			memcpy(rxBleCommBuf,uart4_rxbuffer,UART4_RX_BUFLEN);
			memset(uart4_rxbuffer,0,sizeof(uart4_rxbuffer));
			RXBLE_Flag = 1;
			rxRec4Cnt = 0;
			uart4Com ++;
		}
		else if(uart4_rxbuffer[0]==0xFA && uart4_rxbuffer[2] == rBLETime && rxRec4Cnt==(UART4_RX_BUFLEN+2))
		{
			memset(rxBleCommBuf,0,sizeof(rxBleCommBuf));
			memcpy(rxBleCommBuf,uart4_rxbuffer,UART4_RX_BUFLEN+2);
			memset(uart4_rxbuffer,0,sizeof(uart4_rxbuffer));
			RXBLE_Flag = 1;
			rxRec4Cnt = 0;
			uart4Com ++;
		}

#else		
		if(uart4_rxbuffer[0]!=0xFA || rxRec4Cnt>(UART4_RX_BUFLEN+2))
		{
			rxRec4Cnt = 0;
			memset(uart4_rxbuffer,0,sizeof(uart4_rxbuffer));
		}
		else if(uart4_rxbuffer[0]==0xFA && uart4_rxbuffer[2]<rBLETime && rxRec4Cnt==UART4_RX_BUFLEN)
		{
			
			//接收到APP蓝牙指令处理代码
			if(uart4_rxbuffer[2] == rBLEHr)
			{
				//BLE连接状态心跳数据
				ble_data.state = connected;
			}
			else if(uart4_rxbuffer[2] == rSYNCStart)
			{
				ble_data.syncState = syncstart;
			}
			else if(uart4_rxbuffer[2] == rSYNCStop)
			{
				ble_data.syncState = syncstop;
			}
//			memset(uart4_rxbuffer,0,sizeof(uart4_rxbuffer));
			rxRec4Cnt = 0;
			uart4Com ++;
		}
		else if(uart4_rxbuffer[0]==0xFA && uart4_rxbuffer[1]==0xFF && rxRec4Cnt==(UART4_RX_BUFLEN+2))
		{
			if(uart4_rxbuffer[2] == rBLETime)
			{
				uint32_t bTime=0;
				bTime |= (uint32_t)(uart4_rxbuffer[3]<<24);
				bTime |= (uint32_t)(uart4_rxbuffer[4]<<16);
				bTime |= (uint32_t)(uart4_rxbuffer[5]<<8);
				bTime |= (uint32_t)(uart4_rxbuffer[6]);

				ble_data.ble_time = bTime;
				ble_data.state = connected;//BLE时间数据
				ble_data.synctimeFlag = 1;
				memset(uart4_rxbuffer,0,sizeof(uart4_rxbuffer));
			}

			rxRec4Cnt = 0;
			uart4Com ++;
		}
#endif		
		HAL_UART_Receive_IT(&huart4, (uint8_t*)&aRx4Buffer,1);	// 重新使能串口4接收中断
	}

}


