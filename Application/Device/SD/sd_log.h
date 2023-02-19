/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : sd_log.h
  * @brief          : sd_log bsp
  * @author			: ZYX
  * @time			: 2020.11.16
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 SZCHWS.
  * All rights reserved.
  ******************************************************************************/
#ifndef _SD_LOG_H
#define _SD_LOG_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sd_fatfs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "cmsis_os.h"
#include "stm32l4xx_hal.h"
#include "std.h"

#define D_CHWS_APP_CLOSE_FILE_DELAY_TICKS	10000			// 10000��ϵͳ�δ��(10ms��)�ر��ļ�
#define D_CHWS_APP_SD_FILE_NAME_LEN			50				// SD���ļ�������
#define D_CHWS_APP_SD_WRITE_FILE_INFO_LEN	1000
#define SD_WRITE_LOG_FILE_INFO_LEN	256

#define MAX_SD_READ_RECORD_FILE_INFO_LEN 200
															// SD��д�ļ����ݳ���			
#define D_CHWS_APP_SD_READ_FILE_INFO_LEN	(D_PROT_MCU_AND_CPU_DATA_DETAIL_MAX_LEN - sizeof(SD_READ_FILE_NO_CONTAIN_INFO_T))	
															// SD�����ļ����ݳ���			
#define D_CHWS_APP_SD_PER_FILE_MAX_BYTES	(10240 * 1024)	// SD��ÿ���ļ���󳤶�2M BYTE

#define D_CHWS_APP_SDCARD_LEAST_DELAY_TICKS		2000			// SD�����������ļ�ϵͳf_openʱ��������������FAT��ʱ��δ�ȶ�������60��ϵͳ�δ����òŲ����������˴����������2000��



#define USER_TYPE_NAME_LEN	9	//�ļ����ͳ���,ASCII��1�ֽڽ�����
#define USER_DATE_LEN	15	//����ռ�ó���,ASCII��1�ֽڽ�����
#define USER_NUMS_LEN	6	//���ռ�ó���,ASCII��1�ֽڽ�����

#define USER_SUFF_LEN	5	//�ļ����ͺ�׺,ASCII��1�ֽڽ�����


#define SD_FILE_USR_DATE_PREFIX	'd'			//�ļ�������ǰ׺
#define SD_FILE_USR_NUMS_PREFIX	'n'			//�ļ������ǰ׺
#define SD_FILE_USR_LOG_PREFIX	"log"		//�ļ�������ǰ׺
#define SD_FILE_USR_DAT_PREFIX	"dat"		//�ļ�������ǰ׺
#define SD_FILE_CON				'_'			//�ļ����м����ӷ�
#define SD_FILE_SUFFIX			".txt"		//�ļ����ͺ�׺


#define SD_HEAD_INFO_PROD_MAX_BYTES 100
#define SD_HEAD_INFO_PAR_MAX_BYTES 300
#define SD_HEAD_PRO_INFO_PAR_MAX_BYTES 200

typedef enum{
	E_FATS_OPRATION_STATU_READ_ERR 	= 0,		//��ȡ�ļ�ϵͳ����																	//
	E_FATS_OPRATION_STATU_READ_OK,				//��ȡ�ļ�ϵͳ��ȷ
	E_FATS_OPRATION_STATU_WRITE_ERR,				//д���ļ�ϵͳ����	
	E_FATS_OPRATION_STATU_WRITE_OK,				//д���ļ�ϵͳ��ȷ

	E_FATS_OPRATION_STATU_OTHER_ERR,				//���������ļ�ϵͳ����	
	E_FATS_OPRATION_STATU_OTHER_OK, 				//���������ļ�ϵͳ����	

	E_FATS_OPRATION_STATU_NUMS,
}FATS_OPRATION_STATU_E;

typedef enum{
	E_FATS_FILE_CREAT_NO 	= 0,		//sd���ļ������ɹ�															//
	E_FATS_FILE_CREAT_OK,				//sd���ļ�û�д���

	E_FATS_FILE_CREAT_NUMS,
}FATS_FILE_CREAT_E;

typedef enum{
	E_FATS_LOG_FILE_CREAT_NO 	= 0,		//	sd����־�ļ�û�д���											//
	E_FATS_LOG_FILE_CREAT_OK,				//sd����־�ļ������ɹ�	

	E_FATS_LOG_FILE_CREAT_NUMS,
}FATS_LOG_FILE_CREAT_E;

typedef enum{
	E_FATS_PRO_FILE_CREAT_NO 	= 0,		//sd�����������ļ�û�д���											//
	E_FATS_PRO_FILE_CREAT_OK,				//sd�����������ļ������ɹ�	

	E_FATS_PRO_FILE_CREAT_NUMS,
}FATS_PRO_FILE_CREAT_E;

typedef enum{
	E_SDCARD_STATU_EXIT_OK 	= 0,		//sd������״̬����																	//
	E_SDCARD_STATU_NO_EXIT,				//sd��������״̬
	E_SDCARD_STATU_ABNORMAL,			//sd���쳣״̬

	E_SDCARD_STATU_NUMS,
}SDCARD_STATU_E;

typedef enum{
	E_FILE_OPER_TYPE_FILE_LIST 	= 0,		//�����ļ���														//
	E_FILE_OPER_TYPE_FILE_INFO,				//�����ļ�����

	E_FILE_OPER_TYPE_NUMS,
}FILE_OPER_TYPE_E;

typedef enum{
	E_FILE_STATU_RW_OK 	= 0,		//�ļ���дOK	
	E_FILE_STATU_HD_ERR,				//��Ӳ������
	E_FILE_STATU_NO_EXIT,				//�ļ�������
	E_FILE_STATU_BUSY,					//��æ��־������д��

	E_FILE_STATU_NUMS,
}FILE_STATU_E;

typedef struct {
	char vUserTypeName[USER_TYPE_NAME_LEN];	//�ļ�����
	char vUserDate[USER_DATE_LEN];	//�ļ���--����		[0]='2';[1]='0';...			��Ӧ20190508,			ASCII��1�ֽڽ�����			
	uint16_t vUserNums;							//�ļ���--���		[0]='1';[1]='2';[2]='3';...	��Ӧ���123,			ASCII��1�ֽڽ�����			
}SDCARD_FILE_NAME_INFO_T;

typedef struct {
	uint8_t vFileNameLen;									//SD������д�ļ����ֳ���
	char vFileNameBuf[D_CHWS_APP_SD_FILE_NAME_LEN];			//SD������д�ļ����ֻ���
	uint16_t vFileInfoLen;									//SD������д�ļ����ݳ���
	uint8_t vFileInfoBuf[SD_HEAD_INFO_PAR_MAX_BYTES];	//SD������д�ļ����ݻ���

}SD_WRITE_FILE_INFO_T;

typedef struct {
	
	
	uint16_t vFileInfoLen;									//SD������д�ļ����ݳ���
	uint8_t vFileInfoBuf[1024];	//SD������д�ļ����ݻ���

}SD_WRITE_PRO_FILE_INFO_T;

typedef struct{

	uint16_t dat_len;
	uint8_t	 prodata_buf[512];
	
}SD_RW_PRO_DATA_T;

typedef struct {
	SDCARD_STATU_E vSdStatu;								//SD��״̬����״̬�쳣���򲻴����ļ���Ϣ
	FILE_OPER_TYPE_E vFileOperType;							//SD�����β�������
	uint8_t vFileNameLen;									//SD�����ζ��ļ����ֳ���
	char vFileNameBuf[D_CHWS_APP_SD_FILE_NAME_LEN];			//SD�����ζ��ļ����ֻ���

//	uint16_t vFileNums;										//SD���ļ���ţ������ļ��б���Ч
//	uint8_t vFileTailFlag;									//SD���ļ����һ����־�������ļ��б���Ч

//	uint8_t vInfoTailFlag;									//SD���ļ����һ֡��־�������ļ�������Ч
	FILE_STATU_E vFileStatu;								//SD���ļ�״̬����дOK����������״̬

//	uint32_t vFileInfoTotalLen;								//SD���ļ������ܳ���
//	uint32_t vFileInfoTotalOffset;							//SD���ļ���������ܳ�����ʼλ��ƫ����
}SD_READ_FILE_NO_CONTAIN_INFO_T;	


typedef struct {
	SD_READ_FILE_NO_CONTAIN_INFO_T vStFileNoInfo;

	uint16_t vFileInfoLen;									//SD�����ζ��ļ����ݳ���
	char vFileInfoBuf[300];	//SD�����ζ��ļ����ݻ���
}SD_READ_FILE_T;


typedef struct {
	uint8_t vDataYear[4];	//����--��	[0]=9;[1]=1;[2]=0;[3]=2; ��Ӧ2019��12��13��21��15��25��				
	uint8_t vDataMonth[2];	//����--��	[0]=2;[1]=1;			
	uint8_t vDataDay[2];	//����--��	[0]=3;[1]=1;				
	uint8_t vDataHour[2];	//����--ʱ	[0]=1;[1]=2;				
	uint8_t vDataMin[2];	//����--��	[0]=5;[1]=1;				
	uint8_t vDataSec[2];	//����--��	[0]=5;[1]=2;				
}DATA_TIME_T;


typedef struct 
{
     uint8_t  sec;
	 uint8_t years ; 
	 uint8_t months ;
	 uint8_t days ;		
 	 uint8_t weeks ;
	 uint8_t hours ;		
 	 uint8_t minutes ;	
	 uint8_t Status;
}RTC_TIME;

typedef struct {
	uint8_t ProductNumberName[50];	//��Ʒ���
	uint8_t HardwareVersion[20];	//Ӳ���汾
	uint8_t SoftwareVersion[20];	//����汾
}SD_PRODUCT_INFO;
extern FIL vStWirteFile;       /* write File object for SD �����ļ�ϵͳ�ļ�������open��һ���ļ�����ʱҪclose�������ʧ��*/

void SdcardGenerFileName(char *ArgSerchSdFileName,char *vFileTypeName);
bool SdcardStatuCheck(void);
bool FileInfoInit( void);
void GetSystemDataTime(DATA_TIME_T *ArgDataTime);

extern FATS_FILE_CREAT_E vFatsFileCreat;	

extern bool SD_WriteLogFile(void);
extern void SdcardCreatLogFile(void);
extern void SdcardCreatLogFile_Zyx(void);
extern bool LogWriteFileInfo( uint8_t *ArgWrInfo);
extern char vLogSdFileHeadInfoPar[SD_HEAD_INFO_PAR_MAX_BYTES];		
extern uint8_t WriteLogFileParaHeadInfo(void);


#endif

