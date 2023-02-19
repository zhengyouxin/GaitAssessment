/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : default_task.c
  * @brief          : default_task app
  * @author			: ZYX
  * @time			: 2021.01
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 SZCHWS.
  * All rights reserved.
  ******************************************************************************/
#include "default_task.h"
#include "safety_task.h"
#include "cmsis_os.h"
#include "board.h"
#include "utility.h"
#include "ble.h"

extern ALARM_ERROR_TYPE FaultType;
extern volatile uint8_t SYNCFnished;
extern uint8_t vol_percent;

extern volatile bleData ble_data;

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
//	imu_data_handle();
	static uint16_t count,sCount;
	static TickType_t xLastWakeTime;
	
	xLastWakeTime = xTaskGetTickCount();
	
	
	for(;;)
	{
	
		#if (HARDWARE_V == HV1_0)
			count ++;

			if(count<100)
				LED_ON();
			else if(count>100 && count<1000)
				LED_OFF();
			else if(count>=1000)
				count = 0;
			
	//		vTaskDelayUntil(&xLastWakeTime, 1);
	//		vTaskDelayUntil(&xLastWakeTime, 100);
			osDelay(1);

		#elif (HARDWARE_V >= HV1_1)
	//		uart4_sendtest();
		
			count ++;
			sCount ++;

			if(count<10)
			{
				LED_B_ON();//LED_G_ON();//LED_B_ON();
			}
			else if(count>10 && count<100)
			{
				//LED_G_OFF();//LED_G_OFF();//LED_B_OFF();
				if(SYNCFnished)
				{
					LED_B_ON();
				}
				else
				{
					LED_B_OFF();
				}
			}
			else if(count>=100)
				count = 0;
			
			if(FaultType.BattryLowWarnFial || FaultType.BattryVeryLowWarnFial)
			{
				LED_G_ON();LED_B_OFF();
			}
			
			if(POWER_DETECT()==CHARGING)
			{
				if(vol_percent<=95)
				{
					LED_R_ON();LED_G_OFF();LED_B_OFF();	//³äµçÖÐ£¬ºìµÆÁÁ
				}
				else
				{
					LED_B_ON();LED_R_OFF();LED_G_OFF();	//³äµçÍê³É£¬ÂÌµÆÁÁ
				}
			}
			else
			{
				LED_R_OFF();
			}

			ble_rx_tx_handle(sCount);
			
			if(sCount>=600)
				sCount = 0;
			
			IWDG_Feed();
	//		vTaskDelayUntil(&xLastWakeTime, 1);
	//		vTaskDelayUntil(&xLastWakeTime, 100);
			osDelay(10);
		
		#endif

	}
	/* USER CODE END 5 */
}

