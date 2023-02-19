/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : safety_task.c
  * @brief          : safety_task app
  * @author			: ZYX
  * @time			: 2021.01
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 SZCHWS.
  * All rights reserved.
  ******************************************************************************/

#include "safety_task.h"
#include "cmsis_os.h"
#include "bq27421.h"
#include "board.h"
#include "ble.h"

extern bq27421_info bq_dat;
volatile ALARM_ERROR_TYPE FaultType={0};
volatile uint32_t error_code=0;

extern volatile uint32_t uart1Com;
extern volatile uint32_t uart2Com;
extern volatile uint32_t uart3Com;
extern volatile uint32_t uart4Com;

extern uint8_t vol_percent;
extern volatile uint8_t SYNCFnished;

volatile QueueHandle_t pwrDataQueue = NULL;

uint16_t recPwrData=0;


void BatteryStateCheck(void)
{
	static uint8_t stateCheck=0,stateBleCheck=0;
	static uint16_t pwrDatTmp;

	if(xQueueReceive(pwrDataQueue,(void *)&recPwrData, 0)== pdPASS )     
	{
		
		pwrDatTmp = recPwrData;
	}
	
	if(bq_dat.voltage_mV==0)
	{
		error_code |= (uint32_t)(1<<bqInit);
		FaultType.Bq27421InitFail = 1;
	}
	
	if((bq_dat.isLow && bq_dat.voltage_mV<3710) || vol_percent<=20)
	{
		stateCheck ++;
		if(stateCheck>=60)
		{
			FaultType.BattryLowWarnFial = 1;
			error_code |= (uint32_t)(1<<batLow);
//			stateCheck = 0;
		}
	}
	else
	{
		FaultType.BattryLowWarnFial = 0;
	}
	
	if((bq_dat.isCritical && bq_dat.voltage_mV<3680) || vol_percent<=10)
	{
		stateCheck ++;
		if(stateCheck>=60)
		{
			FaultType.BattryVeryLowWarnFial = 1;
			error_code |= (uint32_t)(1<<batVeryLow);
		}
	}
	else
	{
		FaultType.BattryVeryLowWarnFial = 0;
	}
	
	if(bq_dat.voltage_mV>3100 && vol_percent<=5)
	{
		stateBleCheck ++;
		if(stateBleCheck>=60)
		{
			FaultType.BLEShutdownFial = 1;
			error_code |= (uint32_t)(1<<bleShutdown);
			
			if(bq_dat.isCharging != 1)
				LOAD_SW_OFF();	//BLE Shutdown
			else if(bq_dat.isCharging == 1)
				LOAD_SW_LOW();	//BLE Start
		}
	}
	else
	{
		stateBleCheck = 0;
		FaultType.BLEShutdownFial = 0;
		if(bq_dat.isCharging)
			LOAD_SW_LOW();	//BLE Start
	}
	
}

void UartCommCheck(void)
{
	if(uart1Com > 2000)
	{
		FaultType.Uart1CommFail = 1;
		FaultType.IMUCommFial = 1;
	
		error_code |= (uint32_t)(1<<uart1Comm);
		error_code |= (uint32_t)(1<<imuComm);
	}
	if(uart3Com > 2000)
	{
		FaultType.Uart3CommFail = 1;
		FaultType.IMUCommFial = 1;
	
		error_code |= (uint32_t)(1<<uart3Comm);
		error_code |= (uint32_t)(1<<imuComm);
	}
	
}

void BleStateCheck(void)	//
{
	
	uint8_t ble_sta=0;

	ble_sta = ble_isconnected();

	if(SYNCFnished)
	{
		if(!ble_sta)
		{
			error_code |= (uint32_t)(1<<bleComm);
			SYNCFnished = 0;
		}

	}
}


void StartSafetyTask(void const * argument)
{
	uint32_t count=0;

	pwrDataQueue = xQueueCreate(1, sizeof(uint16_t));
	if(pwrDataQueue == NULL)
    {
        /* 没有创建成功,加入创建失败的处理机制 */
    }

	while(1)
	{
		count ++;
		
		if(count>=20)
		{
			BatteryStateCheck();
			UartCommCheck();
			BleStateCheck();
			count = 0;
		}
		
		
		osDelay(50);
	}
}

