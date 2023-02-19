#ifndef _DELAY_H
#define _DELAY_H

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

void delay_init(uint8_t SYSCLK);
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);
void delay_xms(uint32_t nms);

#endif

