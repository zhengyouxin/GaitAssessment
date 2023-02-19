/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : board.c
  * @brief          : board bsp
  * @author			: ZYX
  * @time			: 2020.11
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 SZCHWS.
  * All rights reserved.
  ******************************************************************************/
/* Includes ------------------------------------------------------------------*/

#include "board.h"
#include "sd_fatfs.h"
#include "sd_log.h"
#include "tim.h"

void HardwareInit(void)
{

#if 1

//	MX_FATFS_Init();
	
	if(FileInfoInit()!=TRUE)
    {

	}
#endif	

//	MX_TIM1_Init();
//	MX_TIM2_Init();
	MX_TIM3_Init();

}

