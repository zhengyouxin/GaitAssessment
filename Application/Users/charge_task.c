/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : charge_task.c
  * @brief          : charge_task app
  * @author			: ZYX
  * @time			: 2021.01
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 SZCHWS.
  * All rights reserved.
  ******************************************************************************/
#include "imu_task.h"
#include "cmsis_os.h"
#include "charge_task.h"
#include "bq27421.h"
#include "board.h"
#include "ble.h"
#include "utility.h"

extern volatile bleData ble_data;
extern UART_HandleTypeDef huart4;
extern volatile uint8_t SYNCFnished;

extern volatile QueueHandle_t pwrDataQueue;
static uint16_t pwrDatValue=0;

bq27421_info bq_dat;

uint8_t vol_percent=100;
uint8_t old_percent=100;

void getVolPercent(void)
{
	static uint8_t sendCnt=0;
	
	sendCnt ++;
	
	if(bq_dat.voltage_mV >= 3100)
	{

//		vol_percent = (bq_dat.voltage_mV-3100)/10;
		if(bq_dat.isCharging != 1)	//非充电状态
		{
			if(bq_dat.voltage_mV>=3810)	
			{
				//100%--50%
				vol_percent = 100 - (4200-bq_dat.voltage_mV)*100/780;
			}
			else if(bq_dat.voltage_mV<3810 && bq_dat.voltage_mV>3680)
			{
				//50%--10%
				vol_percent = 100 - (3973-bq_dat.voltage_mV)*100/325;
			}
			else if(bq_dat.voltage_mV<3680 && bq_dat.voltage_mV>3630)
			{
				vol_percent = 6;
			}
			else if(bq_dat.voltage_mV<3630 && bq_dat.voltage_mV>3400)
			{
				vol_percent = 3;
			}
			else if(bq_dat.voltage_mV<3400)
			{
				vol_percent = 0;
			}

			//规避静电带来的电量突变问题。测试
			if(vol_percent<=6)
			{
				if(old_percent>=20 && old_percent!=100)//
				{
					vol_percent = old_percent;
					//重新初始化电量芯片
					bq27421_init(650,3000,500);
//					LED_R_ON();	
				}
//				else
//				{
//					LED_R_OFF();
//				}
			}
				

			if(vol_percent>100)
				vol_percent=100;

			if(vol_percent>=old_percent)
				vol_percent = old_percent;
			
			old_percent = vol_percent;
		}
		else		//充电状态
		{
			if(bq_dat.voltage_mV<3690)
			{
				vol_percent = 3;
			}
			else if(bq_dat.voltage_mV>=3690 && bq_dat.voltage_mV<3770)
			{
				vol_percent = 8;
			}
			else if(bq_dat.voltage_mV>=3770 && bq_dat.voltage_mV<4180)
			{
				vol_percent = (bq_dat.voltage_mV - 3757)/5;
			}
			else if(bq_dat.voltage_mV>=4180)
			{
				vol_percent = 100;
			}
			
			if(vol_percent>100)
				vol_percent=100;
			
			if(vol_percent<=old_percent && old_percent!=100)
				vol_percent = old_percent;
			
			old_percent = vol_percent;
		}
		
	}
	else
	{
		if(bq_dat.voltage_mV<3100 && bq_dat.voltage_mV>3000)
		{
			vol_percent=0;
		}
		else if(bq_dat.voltage_mV==0)
		{
			vol_percent=99;
		}
	}

	if(pwrDatValue != vol_percent)
	{
		pwrDatValue = vol_percent;

		if(xQueueSend(pwrDataQueue,(void *)&pwrDatValue,(TickType_t)0) != pdPASS )
		{
			;
		}
	}
	
	
}

void StartChargeTask(void const * argument)
{
	uint32_t chargcnt;
	
	bq27421_init(650,3000,500);
//	bq27421_i2c_control_write( BQ27421_CONTROL_UNSEAL );
	
	while(1)
	{
//		i2c_write_test(0xAA33);
		chargcnt ++;

		if(chargcnt>=100)
		{
			bq27421_update(&bq_dat);
			getVolPercent();
			chargcnt = 0;
		}

		
		osDelay(10);
		
	}
}

