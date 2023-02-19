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

#define D_CHWS_APP_CLOSE_FILE_DELAY_TICKS	10000			// 10000个系统滴答后(10ms后)关闭文件
#define D_CHWS_APP_SD_FILE_NAME_LEN			50				// SD卡文件名长度
#define D_CHWS_APP_SD_WRITE_FILE_INFO_LEN	1000
#define SD_WRITE_LOG_FILE_INFO_LEN	256

#define MAX_SD_READ_RECORD_FILE_INFO_LEN 200
															// SD卡写文件内容长度			
#define D_CHWS_APP_SD_READ_FILE_INFO_LEN	(D_PROT_MCU_AND_CPU_DATA_DETAIL_MAX_LEN - sizeof(SD_READ_FILE_NO_CONTAIN_INFO_T))	
															// SD卡读文件内容长度			
#define D_CHWS_APP_SD_PER_FILE_MAX_BYTES	(10240 * 1024)	// SD卡每个文件最大长度2M BYTE

#define D_CHWS_APP_SDCARD_LEAST_DELAY_TICKS		2000			// SD卡开机调用文件系统f_open时会死机，发现是FAT此时还未稳定，至少60个系统滴答后调用才不会死机，此处保险起见用2000个



#define USER_TYPE_NAME_LEN	9	//文件类型长度,ASCII带1字节结束符
#define USER_DATE_LEN	15	//日期占用长度,ASCII带1字节结束符
#define USER_NUMS_LEN	6	//序号占用长度,ASCII带1字节结束符

#define USER_SUFF_LEN	5	//文件类型后缀,ASCII带1字节结束符


#define SD_FILE_USR_DATE_PREFIX	'd'			//文件名日期前缀
#define SD_FILE_USR_NUMS_PREFIX	'n'			//文件名序号前缀
#define SD_FILE_USR_LOG_PREFIX	"log"		//文件名日期前缀
#define SD_FILE_USR_DAT_PREFIX	"dat"		//文件名日期前缀
#define SD_FILE_CON				'_'			//文件名中间连接符
#define SD_FILE_SUFFIX			".txt"		//文件类型后缀


#define SD_HEAD_INFO_PROD_MAX_BYTES 100
#define SD_HEAD_INFO_PAR_MAX_BYTES 300
#define SD_HEAD_PRO_INFO_PAR_MAX_BYTES 200

typedef enum{
	E_FATS_OPRATION_STATU_READ_ERR 	= 0,		//读取文件系统错误																	//
	E_FATS_OPRATION_STATU_READ_OK,				//读取文件系统正确
	E_FATS_OPRATION_STATU_WRITE_ERR,				//写入文件系统错误	
	E_FATS_OPRATION_STATU_WRITE_OK,				//写入文件系统正确

	E_FATS_OPRATION_STATU_OTHER_ERR,				//其他操作文件系统错误	
	E_FATS_OPRATION_STATU_OTHER_OK, 				//其他操作文件系统错误	

	E_FATS_OPRATION_STATU_NUMS,
}FATS_OPRATION_STATU_E;

typedef enum{
	E_FATS_FILE_CREAT_NO 	= 0,		//sd卡文件创建成功															//
	E_FATS_FILE_CREAT_OK,				//sd卡文件没有创建

	E_FATS_FILE_CREAT_NUMS,
}FATS_FILE_CREAT_E;

typedef enum{
	E_FATS_LOG_FILE_CREAT_NO 	= 0,		//	sd卡日志文件没有创建											//
	E_FATS_LOG_FILE_CREAT_OK,				//sd卡日志文件创建成功	

	E_FATS_LOG_FILE_CREAT_NUMS,
}FATS_LOG_FILE_CREAT_E;

typedef enum{
	E_FATS_PRO_FILE_CREAT_NO 	= 0,		//sd卡过程数据文件没有创建											//
	E_FATS_PRO_FILE_CREAT_OK,				//sd卡过程数据文件创建成功	

	E_FATS_PRO_FILE_CREAT_NUMS,
}FATS_PRO_FILE_CREAT_E;

typedef enum{
	E_SDCARD_STATU_EXIT_OK 	= 0,		//sd卡存在状态正常																	//
	E_SDCARD_STATU_NO_EXIT,				//sd卡不存在状态
	E_SDCARD_STATU_ABNORMAL,			//sd卡异常状态

	E_SDCARD_STATU_NUMS,
}SDCARD_STATU_E;

typedef enum{
	E_FILE_OPER_TYPE_FILE_LIST 	= 0,		//操作文件名														//
	E_FILE_OPER_TYPE_FILE_INFO,				//操作文件内容

	E_FILE_OPER_TYPE_NUMS,
}FILE_OPER_TYPE_E;

typedef enum{
	E_FILE_STATU_RW_OK 	= 0,		//文件读写OK	
	E_FILE_STATU_HD_ERR,				//卡硬件错误
	E_FILE_STATU_NO_EXIT,				//文件不存在
	E_FILE_STATU_BUSY,					//卡忙标志，正在写入

	E_FILE_STATU_NUMS,
}FILE_STATU_E;

typedef struct {
	char vUserTypeName[USER_TYPE_NAME_LEN];	//文件类型
	char vUserDate[USER_DATE_LEN];	//文件名--日期		[0]='2';[1]='0';...			对应20190508,			ASCII带1字节结束符			
	uint16_t vUserNums;							//文件名--序号		[0]='1';[1]='2';[2]='3';...	对应序号123,			ASCII带1字节结束符			
}SDCARD_FILE_NAME_INFO_T;

typedef struct {
	uint8_t vFileNameLen;									//SD卡单次写文件名字长度
	char vFileNameBuf[D_CHWS_APP_SD_FILE_NAME_LEN];			//SD卡单次写文件名字缓存
	uint16_t vFileInfoLen;									//SD卡单次写文件内容长度
	uint8_t vFileInfoBuf[SD_HEAD_INFO_PAR_MAX_BYTES];	//SD卡单次写文件内容缓存

}SD_WRITE_FILE_INFO_T;

typedef struct {
	
	
	uint16_t vFileInfoLen;									//SD卡单次写文件内容长度
	uint8_t vFileInfoBuf[1024];	//SD卡单次写文件内容缓存

}SD_WRITE_PRO_FILE_INFO_T;

typedef struct{

	uint16_t dat_len;
	uint8_t	 prodata_buf[512];
	
}SD_RW_PRO_DATA_T;

typedef struct {
	SDCARD_STATU_E vSdStatu;								//SD卡状态，卡状态异常，则不处理卡文件信息
	FILE_OPER_TYPE_E vFileOperType;							//SD卡单次操作类型
	uint8_t vFileNameLen;									//SD卡单次读文件名字长度
	char vFileNameBuf[D_CHWS_APP_SD_FILE_NAME_LEN];			//SD卡单次读文件名字缓存

//	uint16_t vFileNums;										//SD卡文件序号，用于文件列表有效
//	uint8_t vFileTailFlag;									//SD卡文件最后一个标志，用于文件列表有效

//	uint8_t vInfoTailFlag;									//SD卡文件最后一帧标志，用于文件内容有效
	FILE_STATU_E vFileStatu;								//SD卡文件状态，读写OK，或者其他状态

//	uint32_t vFileInfoTotalLen;								//SD卡文件内容总长度
//	uint32_t vFileInfoTotalOffset;							//SD卡文件内容相对总长度起始位置偏移量
}SD_READ_FILE_NO_CONTAIN_INFO_T;	


typedef struct {
	SD_READ_FILE_NO_CONTAIN_INFO_T vStFileNoInfo;

	uint16_t vFileInfoLen;									//SD卡单次读文件内容长度
	char vFileInfoBuf[300];	//SD卡单次读文件内容缓存
}SD_READ_FILE_T;


typedef struct {
	uint8_t vDataYear[4];	//日期--年	[0]=9;[1]=1;[2]=0;[3]=2; 对应2019年12月13日21点15分25秒				
	uint8_t vDataMonth[2];	//日期--月	[0]=2;[1]=1;			
	uint8_t vDataDay[2];	//日期--日	[0]=3;[1]=1;				
	uint8_t vDataHour[2];	//日期--时	[0]=1;[1]=2;				
	uint8_t vDataMin[2];	//日期--分	[0]=5;[1]=1;				
	uint8_t vDataSec[2];	//日期--秒	[0]=5;[1]=2;				
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
	uint8_t ProductNumberName[50];	//产品编号
	uint8_t HardwareVersion[20];	//硬件版本
	uint8_t SoftwareVersion[20];	//软件版本
}SD_PRODUCT_INFO;
extern FIL vStWirteFile;       /* write File object for SD ，读文件系统文件变量，open另一个文件变量时要close，否则会失败*/

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

