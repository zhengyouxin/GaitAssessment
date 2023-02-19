#ifndef _BOARD_H
#define _BOARD_H

#include "stm32l4xx_hal.h"
#include "bsp_driver_sd.h"
#include "sys_config.h"
#include "wdg.h"
#include "tim.h"
#include "uart.h"
#include <stdbool.h>

#define CHARGING		1
#define DISCHARGING		0

#define SYSTEM_SUPPORT_OS 1

/* Private defines -----------------------------------------------------------*/
/* HARDWARE V1_0   -----------------------------------------------------------*/
#define BAT_DET_Pin GPIO_PIN_3
#define BAT_DET_GPIO_Port GPIOC
#define PB0_Pin GPIO_PIN_0
#define PB0_GPIO_Port GPIOB
#define PA15_Pin GPIO_PIN_15
#define PA15_GPIO_Port GPIOA
#define PB5_Pin GPIO_PIN_5
#define PB5_GPIO_Port GPIOB
#define SDRW_Pin GPIO_PIN_4
#define SDRW_GPIO_Port GPIOB
#define PB9_LED_Pin GPIO_PIN_9
#define PB9_LED_GPIO_Port GPIOB

#define SDRW_INPUT()	HAL_GPIO_ReadPin(SDRW_GPIO_Port,SDRW_Pin)
#define SYNC_INPUT()	HAL_GPIO_ReadPin(PB0_GPIO_Port,PB0_Pin)

#define LED_ON()  		HAL_GPIO_WritePin(GPIOB, PB9_LED_Pin, GPIO_PIN_RESET)
#define LED_OFF() 		HAL_GPIO_WritePin(GPIOB, PB9_LED_Pin, GPIO_PIN_SET)
#define LED_TOGGLE() 	HAL_GPIO_TogglePin(GPIOB, PB9_LED_Pin)

/* HARDWARE V1_1   -----------------------------------------------------------*/
#define BLE_SYNC_Pin		GPIO_PIN_13
#define BLE_SYNC_Port		GPIOC
#define Power_Detect_Pin 	GPIO_PIN_2
#define Power_Detect_Port	GPIOC
#define POWER_DETECT()		HAL_GPIO_ReadPin(Power_Detect_Port,Power_Detect_Pin)

#define LOAD_SW_Pin			GPIO_PIN_8	//BLE & SD POWER low-en
#define LOAD_SW_Port		GPIOB

#define LED_Red_Pin			GPIO_PIN_0
#define LED_Blue_Pin		GPIO_PIN_1
#define LED_Green_Pin		GPIO_PIN_2
#define LED_GPIO_Port		GPIOB

#define LED_R_ON()  		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Red_Pin, GPIO_PIN_RESET)
#define LED_R_OFF() 		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Red_Pin, GPIO_PIN_SET)
#define LED_R_TOGGLE() 		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Red_Pin)
#define LED_B_ON()  		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Blue_Pin, GPIO_PIN_RESET)
#define LED_B_OFF() 		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Blue_Pin, GPIO_PIN_SET)
#define LED_B_TOGGLE() 		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Blue_Pin)
#define LED_G_ON()  		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET)
#define LED_G_OFF() 		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET)
#define LED_G_TOGGLE() 		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Green_Pin)

#define LOAD_SW_LOW()		HAL_GPIO_WritePin(LOAD_SW_Port, LOAD_SW_Pin, GPIO_PIN_RESET)
#define LOAD_SW_OFF()		HAL_GPIO_WritePin(LOAD_SW_Port, LOAD_SW_Pin, GPIO_PIN_SET)


void HardwareInit(void);


typedef struct HardwareDevice
{
    uint8_t MCU_ClockInit;    
    uint8_t MCU_UART1_Init;
    uint8_t MCU_UART2_Init; 
    uint8_t MCU_UART3_Init;
	uint8_t MCU_UART4_Init;
//    uint8_t MCU_TimerInit;  
    uint8_t SD_Init; 
    
}HDeviceInit;

extern HDeviceInit HDeviceInitResult;

#endif
