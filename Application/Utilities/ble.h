#ifndef _BLE_H
#define _BLE_H

#include "stm32l4xx_hal.h"


typedef enum 
{
	disconnect = 0,
	connected,
	
	
}BLE_STATE;
	
typedef enum 
{
	syncstart = 1,
	syncstop,
	
	
}SYNC_STATE;

typedef struct
{
	uint8_t type;
	
	BLE_STATE state;
	SYNC_STATE syncState;
	
	uint8_t synctimeFlag;
	uint32_t ble_time;
}bleData;

void ble_update_systime(void);
uint8_t ble_isconnected(void);
void ble_rx_tx_handle(uint16_t timecnt);

#endif

