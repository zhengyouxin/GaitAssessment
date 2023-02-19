#ifndef _SYSTIME_H
#define _SYSTIME_H
#include "stm32l4xx_hal.h"
#include "ble.h"

typedef struct 
{
	uint8_t year;
	uint8_t  month;
	uint8_t  day;
	uint8_t  hour;
	uint8_t  minute;
	uint8_t  second;
	uint8_t  week;
}
systime_t;

void systime_Init(void);
void systime_update(uint32_t timedata);
void Calendar_Timer(void);

#endif

