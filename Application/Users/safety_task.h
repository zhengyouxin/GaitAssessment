#ifndef _SAFETY_TASK_H
#define _SAFETY_TASK_H


#include "stm32l4xx_hal.h"

typedef enum
{
	bqInit,
	batLow,
	batVeryLow,
	bleShutdown,
	bleComm,
	uart1Comm,
	uart2Comm,
	uart3Comm,
	uart4Comm,
	imuComm,

	sdTaskb,
	defaultTaskb,
	imuTaskb,
	chargeTaskb,
	satetyTaskb,
	
}errorbit;
	

typedef struct ALARM_ERROR
{
	uint8_t Bq27421InitFail;
	uint8_t BattryLowWarnFial;
	uint8_t BattryVeryLowWarnFial;
	uint8_t BLEShutdownFial;
	
	uint8_t BleCommFail;

	uint8_t Uart1CommFail;
	uint8_t Uart2CommFail;
	uint8_t Uart3CommFail;
	uint8_t Uart4CommFail;
	uint8_t IMUCommFial;
	
	uint8_t DefaultTaskHaltFail;	
	uint8_t StartTaskHaltFail;	
	uint8_t ChargeTaskHaltFail;	
	uint8_t SDTaskHaltFail;	
	uint8_t SafetyTaskHaltFail;	


}ALARM_ERROR_TYPE;



#endif
