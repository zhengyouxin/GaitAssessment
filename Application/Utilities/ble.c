/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : ble.c
  * @brief          : ble app
  * @author			: ZYX
  * @time			: 2021.01.14
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 SZCHWS.
  * All rights reserved.
  ******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "ble.h"
#include "systime.h"
#include "uart.h"
#include "utility.h"
#include "board.h"

//下发APP/蓝牙模块指令，串口透传
uint8_t bleConnected[6]={0xFA,0xFF,0xA5,0x01,0x01,0x59};
uint8_t bleDisconnected[6]={0xFA,0xFF,0xA5,0x01,0x02,0x58};

uint8_t bleSendVol[6] 		=	{0xFA,0xFF,0xA1,0x01,0x01,0xff};
uint8_t bleSendVersion[6]	=	{0xFA,0xFF,0xA7,0x01,0x01,0xff};
uint8_t bleSendState[7]		=	{0xFA,0xFF,0xA8,0x02,0x00,0x00,0xff};

volatile bleData ble_data={0};

extern UART_HandleTypeDef huart4;
extern volatile uint8_t SYNCFnished;
extern volatile uint32_t error_code;
extern uint8_t rxBleCommBuf[UART4_RX_BUFLEN_MAX];
extern uint8_t RXBLE_Flag;
extern uint8_t vol_percent;

void ble_update_systime(void)
{

	if(ble_data.state && ble_data.synctimeFlag)
	{
		systime_update(ble_data.ble_time);
		ble_data.synctimeFlag = 0;
	}
}

uint8_t ble_isconnected(void)
{
	if(ble_data.state == connected || ble_data.syncState == syncstart)
	{
		return 1;
	}

	return 0;

}

void ble_rx_tx_handle(uint16_t timecnt)
{

	if(timecnt == 500)
	{
		if(ble_data.state==connected && SYNCFnished!=1) //5s 发一次电量//&& ble_data.syncState!=syncstart && sFlag==0
		{
			bleSendVol[4] = vol_percent;
			bleSendVol[5] = CheckSum(&bleSendVol[2],3);
		
			HAL_UART_Transmit_DMA(&huart4,bleSendVol,sizeof(bleSendVol));	//串口4发送数据，BLE透传用。
		}
		timecnt = 0;
	}

	if(timecnt == 550)
	{
		if(ble_data.state==connected && SYNCFnished!=1) //5s 发一次系统状态
		{
			bleSendState[4] = error_code>>8;
			bleSendState[5] = error_code;
			bleSendState[6] = CheckSum(&bleSendState[2],4);
		
			HAL_UART_Transmit_DMA(&huart4,bleSendState,sizeof(bleSendState));	//串口4发送数据，BLE透传用。
		}
		timecnt = 0;
	}

	if(timecnt == 600)
	{
		if(ble_data.state==connected && SYNCFnished!=1) //6s 发一次版本号
		{

			bleSendVersion[4] = SOFTWARE_V;
			bleSendVersion[5] = CheckSum(&bleSendVersion[2],3);
		
			HAL_UART_Transmit_DMA(&huart4,bleSendVersion,sizeof(bleSendVersion));	//串口4发送数据，BLE透传用。
		}

		timecnt = 0;
	}
}


