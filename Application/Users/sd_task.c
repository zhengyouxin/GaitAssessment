/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : sd_task.c
  * @brief          : sd_task app
  * @author			: ZYX
  * @time			: 2020.11
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 SZCHWS.
  * All rights reserved.
  ******************************************************************************/
#include "sd_task.h"
#include "safety_task.h"
#include "cmsis_os.h"
#include "bsp_driver_sd.h"
#include "sd_log.h"
#include "sd_data.h"
#include "ble.h"
#include "sys_config.h"
#include "wdg.h"

volatile uint8_t SdTaskCount = 0;
volatile uint8_t SD_InitializeResult =0;

volatile QueueHandle_t SdWriteLogDataQueue = NULL;//sd  接收待写入sd卡日志内容队列
volatile QueueHandle_t SdWriteProDataQueue = NULL;//sd  接收待写入sd卡过程数据内容队列

extern FATS_LOG_FILE_CREAT_E vFatsLogFileCreat;	//sd卡里面日志文件创建结果	
extern FATS_PRO_FILE_CREAT_E vFatsProFileCreat;	//sd卡里面过程文件创建结果	

extern volatile uint8_t SYNCFnished;
extern ALARM_ERROR_TYPE FaultType;
extern uint16_t imuSave_startFlag;

extern volatile SemaphoreHandle_t  SD_WriteMutex;
extern uint16_t sQueueCnt;

uint8_t vSdcardExitFlag = SD_NOT_PRESENT;
FIL vStWirteTestFile;		/* read File object for SD ，读文件系统文件变量，open另一个文件变量时要close，否则会失败*/
uint16_t sdRecCount=0;


void Sd_WriteTest(void)
{
	
	FRESULT TmpRetFr = FR_INVALID_PARAMETER;
	UINT TmpWriteBytes;
	uint8_t cnt;
	
	for(cnt=0;cnt<10;cnt++)
	{
		if((TmpRetFr = f_open(&vStWirteTestFile, "test.txt", FA_READ|FA_WRITE|FA_OPEN_ALWAYS)) == FR_OK)
		{
			if((TmpRetFr = f_lseek(&vStWirteTestFile, f_size(&vStWirteTestFile))) == FR_OK)
			{
				if((TmpRetFr = f_write(&vStWirteTestFile, "1234567890\r\n", sizeof("1234567890\r\n"), &TmpWriteBytes)) == FR_OK)
				{
				}
			}
		}
		else
		{
		}
		f_close(&vStWirteTestFile);
	}

}

void StartSDTask(void const * argument)
{
	FRESULT TmpRetFr = FR_INVALID_PARAMETER;
	UINT TmpWriteBytes;
	uint16_t sdCount=0;
	uint16_t sdWrLogWait=0;
	uint8_t syncFlag=0;
	uint8_t recTmp=10;
	
	//SD卡初始化成功 2020.11.16
	MX_FATFS_Init();

	
//	SdWriteLogDataQueue = xQueueCreate(10, sizeof(SD_WRITE_FILE_INFO_T));
//	SdWriteProDataQueue = xQueueCreate(15, sizeof(SD_WRITE_PRO_FILE_INFO_T));	

	//创建最多10个项目，每个项目长度为SD_SAVE_DATA_LENTH
	SdWriteProDataQueue = xQueueCreate(10, sizeof(uint16_t));
	//SdWriteProDataQueue = xQueueCreate(15, sizeof(SD_RW_PRO_DATA_T));
	if(SdWriteProDataQueue == NULL)
    {
		/* 没有创建成功,加入创建失败的处理机制 */
    }	

	/* Infinite loop */
	while(1)
	{
		if (vSdcardExitFlag == SD_NOT_PRESENT)//卡标志为不存在
		{
			if (BSP_SD_IsDetected() == SD_PRESENT)//检测到卡存在，并且设置标志存在
			{
				if (f_mount(&SDFatFS, SDPath, 0) == FR_OK)
				{
					vSdcardExitFlag = SD_PRESENT; //加载成功，才设置卡存在标志
				}
			}
		}
		osDelay(5);
		IWDG_Feed();
		sdCount ++;
		//	Fatfs_RW_test();
		//SD卡读写测试成功 2021.2.22
		if(vSdcardExitFlag == SD_PRESENT && sdCount>=10)
		{

//			if((TmpRetFr = f_open(&vStWirteTestFile, "test3.txt", FA_READ|FA_WRITE|FA_OPEN_ALWAYS)) == FR_OK)
//			{
//				if((TmpRetFr = f_lseek(&vStWirteTestFile, f_size(&vStWirteTestFile))) == FR_OK)
//				{
//					if((TmpRetFr = f_write(&vStWirteTestFile, "123456asdf\r\n", sizeof("123456asdf\r\n"), &TmpWriteBytes)) == FR_OK)
//					{
//					}
//				}
//			}
//			else
//			{
//			}
//			f_close(&vStWirteTestFile);
//			Sd_WriteTest();

			if(SYNCFnished)
			{
				sdWrLogWait ++;
				
				if(sdWrLogWait>50)
				{		
#if WRITE_LOG_EN
					SdcardCreatLogFile_Zyx();
#endif
					sdWrLogWait = 0;
				}
								
				if(sdWrLogWait>10)
				{
#if WRITE_PRO_EN
					SdcardCreatProFile_Zyx();
#endif
				}
				
				if(vFatsProFileCreat == E_FATS_PRO_FILE_CREAT_OK)
				{
#if WRITE_PRO_EN

					uint16_t tmpRecQueueProData;
					SD_RW_PRO_DATA_T tmpRecQueueProInfo;

					//receive queue data
						for(recTmp=15;recTmp>0;recTmp--)
						{
//							if(xQueueReceive(SdWriteProDataQueue,(void *)&tmpRecQueueProInfo, 0) == pdPASS)
							if(xQueueReceive(SdWriteProDataQueue,(void *)&tmpRecQueueProData, 0) == pdPASS)
							{
								sdRecCount ++;//start write sd card
							}
						}
                
					imuDataStartSaveSDCard();
#endif
				}
				
				syncFlag = 1;
			}
			else if(syncFlag == 1)
			{
				vFatsLogFileCreat = E_FATS_LOG_FILE_CREAT_NO;
				vFatsProFileCreat = E_FATS_PRO_FILE_CREAT_NO;
				syncFlag = 0;
				sdWrLogWait = 0;
			}
			
			sdCount = 0;
//			WriteLogFileParaHeadInfo();
//			LogWriteFileInfo(vLogSdFileHeadInfoPar);
//			SD_WriteLogFile();
		}
		else
		{
		}
	}


}


