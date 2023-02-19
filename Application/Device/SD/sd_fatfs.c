/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : sd_fatfs.c
  * @brief          : sd_fatfs bsp
  * @author			: ZYX
  * @time			: 2020.11.16
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 SZCHWS.
  * All rights reserved.
  ******************************************************************************/
#include "sd_fatfs.h"
#include "ff.h"			/* FatFS文件系统模块*/
#include "string.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */
FILINFO fileinfo;	//文件信息
DIR dir;  			//目录

TIME_VALUE RTC_TimeValue;
//SD_HandleTypeDef SDCARD_Handler;

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the USER driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
	DWORD time_buff=0;
	/* USER CODE BEGIN get_fattime */

	if(RTC_TimeValue.status != 0x01)
	{
	  RTC_TimeValue.year = 19;
	  RTC_TimeValue.month = 02;
	  RTC_TimeValue.week = 3;
	  RTC_TimeValue.date = 18;
	  RTC_TimeValue.hour = 15;
	  RTC_TimeValue.minute = 36;
	  RTC_TimeValue.second = 52;
	  RTC_TimeValue.status = 0x01;
	}   

	time_buff |= ((RTC_TimeValue.year+2000 - 1980)<<25);	  
	time_buff |= (RTC_TimeValue.month<<21); 
	time_buff |= (RTC_TimeValue.date<<16);  
	time_buff |= (RTC_TimeValue.hour<<11);   
	time_buff |= (RTC_TimeValue.minute<<5); 
	time_buff |= (RTC_TimeValue.second/2);	  

	/* USER CODE BEGIN get_fattime */
	return time_buff;
	/* USER CODE END get_fattime */
}

