#ifndef _TIM_H
#define _TIM_H

#include "stm32l4xx_hal.h"

// 最终定时器频率计算为： 80MHz/（GENERAL_TIM_PRESCALER+1）/(GENERAL_TIM_PERIOD+1)
// 比如需要产生1ms周期定时，可以设置为： 80MHz/（79+1）/1000=1KHz，即1ms周期
// 这里设置 GENERAL_TIM_PRESCALER=79；GENERAL_TIM_PERIOD=1000-1；

/* Auto switch one second*/
#define GENERAL_TIM2_PRESCALER           8000-1   //80M/8000/10000 =1S
#define GENERAL_TIM2_PERIOD              (10000-1) 

/* Auto switch two second*/
#define GENERAL_TIM3_PRESCALER           8000-1   //80M/8000/10000 =1S
#define GENERAL_TIM3_PERIOD              (10000-1) 

void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM3_Init(void);


#endif
