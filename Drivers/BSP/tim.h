#ifndef _TIM_H
#define _TIM_H

#include "stm32l4xx_hal.h"

// ���ն�ʱ��Ƶ�ʼ���Ϊ�� 80MHz/��GENERAL_TIM_PRESCALER+1��/(GENERAL_TIM_PERIOD+1)
// ������Ҫ����1ms���ڶ�ʱ����������Ϊ�� 80MHz/��79+1��/1000=1KHz����1ms����
// �������� GENERAL_TIM_PRESCALER=79��GENERAL_TIM_PERIOD=1000-1��

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
