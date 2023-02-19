/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : sd_log.c
  * @brief          : sd_log bsp
  * @author			: ZYX
  * @time			: 2020.11.16
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 SZCHWS.
  * All rights reserved.
  ******************************************************************************/
#include "sd_log.h"
#include "ff.h"
#include "systime.h"
#include "sys_config.h"

FIL vStReadFile; 
FIL vStWirteFile;       /* write File object for SD �����ļ�ϵͳ�ļ�������open��һ���ļ�����ʱҪclose�������ʧ��*/

FATS_FILE_CREAT_E vFatsFileCreat;	//sd�������ļ��������	

FATS_LOG_FILE_CREAT_E vFatsLogFileCreat = E_FATS_LOG_FILE_CREAT_NO;	//sd��������־�ļ��������	

FATS_OPRATION_STATU_E vFatsOpStatu = E_FATS_OPRATION_STATU_NUMS;	//����sdcard��fats���

extern volatile QueueHandle_t SdWriteLogDataQueue;
extern volatile QueueHandle_t SdWriteProDataQueue;

extern uint8_t vSdcardExitFlag;
extern volatile systime_t systime;
extern volatile uint32_t error_code;
extern uint8_t vol_percent;

SD_WRITE_FILE_INFO_T vStLogWrFileInf;		//��ǰд��־�ļ���Ϣ������д�ļ��ӿڶ�Ҫ���ô˱���	

SD_READ_FILE_T vStReFile;				//���ļ��б���ļ���Ϣ

char vLogSdFileHeadInfoPar[SD_HEAD_INFO_PAR_MAX_BYTES];		//sd����ͷ����Ϣ������Ϣ����

volatile RTC_TIME BLE_RTC ={0,0,0,0,0,0,0,0};

DATA_TIME_T vSysDataTime;		//ϵͳ��ǰ����ʱ�䣬����ʱ��
char vLogFileTypeName[USER_TYPE_NAME_LEN];	// ��־�ļ���������
char vProFileTypeName[USER_TYPE_NAME_LEN];	// �����ļ���������

uint8_t LogWriteBuf[100]={0};

char vSdFileHeadInfoProd[SD_HEAD_INFO_PROD_MAX_BYTES];	//sd����ͷ����Ϣ��Ʒ��Ϣ����
char vLogSdFileHeadInfoPar[SD_HEAD_INFO_PAR_MAX_BYTES];		//sd����ͷ����Ϣ������Ϣ����

const char HardwareVersion[] =  "Ӳ���汾���ǣ�v1.1\n" ;
const char SoftwareVersion[] = "����汾���ǣ�v1.1.0\n" ;
const char ProductNumberName[] = "��Ʒ�ͺţ�P00\n";

char const *vFileTypeLog ="log";
char const *vFileTypePro ="prodata";

bool FileInfoInit( void)
{
	bool TmpRet = FALSE;

	strcat(vSdFileHeadInfoProd,HardwareVersion);	
	strcat(vSdFileHeadInfoProd,SoftwareVersion);	
	strcat(vSdFileHeadInfoProd,ProductNumberName);	
	
	memcpy(vLogFileTypeName, vFileTypeLog, strlen(vFileTypeLog));
	memcpy(vProFileTypeName, vFileTypePro, strlen(vFileTypePro));

	if(SdcardStatuCheck() !=TRUE)
    {
        return FALSE;
    
    }
	
	TmpRet = TRUE;

	return TmpRet;

}

uint8_t WriteLogFileParaHeadInfo(void)
{
	uint8_t FileHeadOffset = 0;
	uint8_t TmpStrlen = 0;
	uint8_t blestate=0;
	
	char arrayTmpbuf[30] = {0};
	char batTmpbuf[4]={0};
	char errTmpbuf[8]={0};
	uint8_t tmpBuf[3]={0};
	uint8_t errBuf[8]={0};

	const char StringStarthead[]= "P00 Log File\n";
	
	const char StringSoftWareVersion[]="\nSoftVersion: ";
	const char StringHardWareVersion[]="\nHardVersion: ";
	const char StringStarttime[]= "\nStarttime: ";
	const char StringStoptime[]= "\nStoptime: ";
	const char StringTime[]= "10min";
	const char StringBLEState[]= "\nBLEState: ";
	const char StringBLEConn[]= "Connected";
	const char StringBLEDis[]= "Disconnect";
	const char StringSysState[]= "\nSysState: ";
	const char StringSysNomal[]= "Nomal";
	const char StringSysError[]= "Error";
	const char StringSysErrcode[]= "\nErrcode: ";
	const char StringBattry[]= "\nCurrent Battry: ";
	
	
	GetSystemDataTime(&vSysDataTime);
	
	memset(vLogSdFileHeadInfoPar,0,sizeof(vLogSdFileHeadInfoPar));

	strcat(&vLogSdFileHeadInfoPar[0],StringStarthead);	//"P00 Log File"
	FileHeadOffset += strlen(StringStarthead);

	strcat(vLogSdFileHeadInfoPar,StringSoftWareVersion);	//"SoftVersion: "
	FileHeadOffset += strlen(StringSoftWareVersion);
	strcat(vLogSdFileHeadInfoPar,SOFTWARE_VERSION);
	FileHeadOffset += 6;

	strcat(vLogSdFileHeadInfoPar,StringHardWareVersion);	//"HardVersion: "
	FileHeadOffset += strlen(StringHardWareVersion);
	strcat(vLogSdFileHeadInfoPar,HARDWARE_VERSION);
	FileHeadOffset += 6;

	strcat(vLogSdFileHeadInfoPar,StringStarttime);		//"Starttime:"
	FileHeadOffset += strlen(StringStarttime);

	sprintf(arrayTmpbuf, "%1d%1d%1d%1d-%1d%1d-%1d%1d %1d%1d:%1d%1d:%1d%1d", vSysDataTime.vDataYear[3], 
	vSysDataTime.vDataYear[2], vSysDataTime.vDataYear[1], vSysDataTime.vDataYear[0],vSysDataTime.vDataMonth[1], 
	vSysDataTime.vDataMonth[0], vSysDataTime.vDataDay[1], vSysDataTime.vDataDay[0], vSysDataTime.vDataHour[1], 
	vSysDataTime.vDataHour[0], vSysDataTime.vDataMin[1], vSysDataTime.vDataMin[0], vSysDataTime.vDataSec[1], vSysDataTime.vDataSec[0]);

	TmpStrlen = strlen(arrayTmpbuf);
	memcpy(&vLogSdFileHeadInfoPar[FileHeadOffset],arrayTmpbuf,TmpStrlen);
	FileHeadOffset += TmpStrlen;

	
	strcat(vLogSdFileHeadInfoPar,StringStoptime);		//"Stoptime:"
	FileHeadOffset += strlen(StringStoptime);
	
	strcat(vLogSdFileHeadInfoPar,StringTime);			//"10min"
	FileHeadOffset += strlen(StringTime);
	
	strcat(vLogSdFileHeadInfoPar,StringBLEState);		//"BLEState:"
	FileHeadOffset += strlen(StringBLEState);
	blestate = ble_isconnected();
	if(blestate)
	{
		strcat(vLogSdFileHeadInfoPar,StringBLEConn);	//"Connected"
		FileHeadOffset += strlen(StringBLEConn);
	}
	else
	{
		strcat(vLogSdFileHeadInfoPar,StringBLEDis);		//"DisConnect"
		FileHeadOffset += strlen(StringBLEDis);
	}

	
	strcat(vLogSdFileHeadInfoPar,StringSysState);		//"SysState:"
	FileHeadOffset += strlen(StringSysState);
	
	if(error_code!=0)
	{
		errBuf[0]=(uint8_t)(error_code>>28);
		errBuf[1]=(uint8_t)((error_code>>24)&0x0f);
		errBuf[2]=(uint8_t)((error_code>>20)&0x0f);
		errBuf[3]=(uint8_t)((error_code>>16)&0x0f);
		errBuf[4]=(uint8_t)((error_code>>12)&0x0f);
		errBuf[5]=(uint8_t)((error_code>>8)&0x0f);
		errBuf[6]=(uint8_t)((error_code>>4)&0x0f);
		errBuf[7]=(uint8_t)(error_code&0x0f);
		
		sprintf(errTmpbuf,"%1d%1d%1d%1d%1d%1d%1d%1d",errBuf[0],errBuf[1],errBuf[2],errBuf[3],errBuf[4],errBuf[5],errBuf[6],errBuf[7]);

		strcat(vLogSdFileHeadInfoPar,StringSysError);	//"Error"
		FileHeadOffset += strlen(StringSysError);
		strcat(vLogSdFileHeadInfoPar,StringSysErrcode);	//"ErrCode:"
		FileHeadOffset += strlen(StringSysErrcode);
		
		TmpStrlen = strlen(errTmpbuf);
		memcpy(&vLogSdFileHeadInfoPar[FileHeadOffset],errTmpbuf,TmpStrlen);
		FileHeadOffset += TmpStrlen;
	}
	else
	{
		strcat(vLogSdFileHeadInfoPar,StringSysNomal);	//"Nomal"
		FileHeadOffset += strlen(StringSysNomal);
	}

	strcat(vLogSdFileHeadInfoPar,StringBattry);			//"Current Battry:"
	FileHeadOffset += strlen(StringBattry);
	if(vol_percent==100)
	{
		tmpBuf[0]=vol_percent/100;
		tmpBuf[1]=0;
		tmpBuf[2]=0;
		sprintf(batTmpbuf,"%1d%1d%1d%%",tmpBuf[0],tmpBuf[1],tmpBuf[2]);
	}
	else
	{
		tmpBuf[0]=vol_percent/10;
		tmpBuf[1]=vol_percent%10;
		sprintf(batTmpbuf,"%1d%1d%%",tmpBuf[0],tmpBuf[1]);
	}
	
	TmpStrlen = strlen(batTmpbuf);
	memcpy(&vLogSdFileHeadInfoPar[FileHeadOffset],batTmpbuf,TmpStrlen);
	FileHeadOffset += TmpStrlen;

	
	return 1;
}

void SdcardCreatLogFile_Zyx(void)
{
	FRESULT TmpRetFr = FR_INVALID_PARAMETER;	/* Return value */
	char ArgSerchLogSdFileName[D_CHWS_APP_SD_FILE_NAME_LEN];	//������־�ļ�����
	UINT TmpWriteBytes;

	
	if(vSdcardExitFlag == SD_NOT_PRESENT)
		return;

//	vFatsFileCreat = vFatsLogFileCreat ;
	
	if(vFatsLogFileCreat == E_FATS_LOG_FILE_CREAT_OK)
		return;
	
	
	memset(ArgSerchLogSdFileName, 0, sizeof(ArgSerchLogSdFileName));

//	memcpy(vLogFileTypeName, vFileTypeLog, strlen(vFileTypeLog));
//	memcpy(vproFileTypeName, vFileTypePro, strlen(vFileTypePro));
	
	
	SdcardGenerFileName(ArgSerchLogSdFileName,vLogFileTypeName);
	

	WriteLogFileParaHeadInfo();
	
//������־�͹����ļ�һ�𴴽���
	// 1 Ѱ��û��ͬ�����ļ�
	if((TmpRetFr = f_open(&vStWirteFile, ArgSerchLogSdFileName, FA_OPEN_EXISTING)) == FR_NO_FILE)
	{
		f_close(&vStWirteFile);
		// 2 �����µ��ļ���
		if((TmpRetFr = f_open(&vStWirteFile, ArgSerchLogSdFileName/*"log_2102251543.txt"*/, FA_READ|FA_WRITE|FA_OPEN_ALWAYS)) == FR_OK)
		{
			vStLogWrFileInf.vFileNameLen = strlen(ArgSerchLogSdFileName);
			memcpy(vStLogWrFileInf.vFileNameBuf, ArgSerchLogSdFileName, strlen(ArgSerchLogSdFileName));

			if((TmpRetFr = f_lseek(&vStWirteFile, f_size(&vStWirteFile))) == FR_OK)
			{
				if((TmpRetFr = f_write(&vStWirteFile, vLogSdFileHeadInfoPar, sizeof(vLogSdFileHeadInfoPar), &TmpWriteBytes)) == FR_OK)
				{
					vFatsLogFileCreat = E_FATS_LOG_FILE_CREAT_OK;
				}
			}
		}
		else
		{
			vFatsLogFileCreat = E_FATS_LOG_FILE_CREAT_NO;
		}
		f_close(&vStWirteFile);
		

//�����ļ�ʱ��д���Ʒ��Ϣ	
//		LogWriteFileInfo(SD_HEAD_INFO_PROD_MAX_BYTES, vSdFileHeadInfoProd);
//		LogWriteFileInfo(SD_HEAD_INFO_PROD_MAX_BYTES, vSdFileHeadInfoPar);		

		
//		LogWriteFileInfo(vSdFileHeadInfoProd);
//		SD_WriteLogFile();
		
	}
	else
	{
		vStLogWrFileInf.vFileNameLen = strlen(ArgSerchLogSdFileName);
		memcpy(vStLogWrFileInf.vFileNameBuf, ArgSerchLogSdFileName, strlen(ArgSerchLogSdFileName));
		vFatsLogFileCreat = E_FATS_LOG_FILE_CREAT_OK;
		f_close(&vStWirteFile);
		
	}



}

/* ***************************************************************
* Funciton : SD_WriteLogFile()
* Description : sdcard���������
* 				��дSDCARD�ļ��ӿ�,���ڴ������������ڼ���־�ļ���
* Call By : safety_task
* Parameters : NA 
* Return : void
* Other : ��������
****************************************************************/
bool SD_WriteLogFile(void)
{  
//test	
    volatile	uint8_t testtmp;
	
	UINT TmpWriteBytes;  
//	UINT TmpReadBytes;  
//	char TmpReadBuf[D_CHWS_APP_SD_READ_FILE_INFO_LEN];	//SD�����ζ��ļ����ݻ���

	FRESULT TmpRetFr; 	/* Return value */
//	FILINFO TmpFileInfo;	/* File information */

//	REQ_SD_FILE_T TmpReqInfo;
	BaseType_t TmpRecRet = pdFAIL;

	static SD_READ_FILE_T TmpStReFile;

	SD_WRITE_FILE_INFO_T TmpStWrFileInf;		//��ǰд�ļ���Ϣ

//	SD_WRITE_PRO_FILE_INFO_T TmpStWrProFileInf;
	
	uint32_t TmpWrOffNow = 0; //��ǰд���ļ���ƫ����
	uint8_t TmpLoopWrCnt = 5; //��ǰѭ������д�����ļ�
	
//	EventBits_t TmpEvtWaitFatsBit = 0;
//	EventBits_t TmpEvtWaitOtgBit = 0;

//	memset(&TmpWriteBytes, 0, sizeof(TmpWriteBytes));
//	memset(&TmpReadBytes, 0, sizeof(TmpReadBytes));
//	memset(TmpReadBuf, 0, sizeof(TmpReadBuf));
	memset(&TmpStWrFileInf, 0, sizeof(TmpStWrFileInf));
//	memset(&TmpReqInfo, 0, sizeof(TmpReqInfo));

	SdcardStatuCheck();

//	if (FALSE == Chws_App_Sdcard_Wait_Fats(vSdcardExitFlag))// ע��˺���Ҫ����MX_Sdcard_Statu_Check�����
//		return;

	vFatsFileCreat = vFatsLogFileCreat;
	//û�д����ļ�ʱ�������������д�����ݵ�sd���ļ����档
	if (vFatsFileCreat == E_FATS_FILE_CREAT_NO)
	{
//		SdcardCreatLogFile();
//		return FALSE;
	}
	
// 1��д ��־�ļ�����
    for (; TmpLoopWrCnt>0; TmpLoopWrCnt--)
    {			
//        TmpRecRet = xQueueReceive(SdWriteLogDataQueue, (void *)(&TmpStWrFileInf), 0);     
        if (TmpRecRet != pdPASS)
        {
          testtmp =1;
//          return FALSE;
           continue ;
        }
/*�ļ�ϵͳ�������������ļ�����λ���ļ���β��д�����ݣ��ر��ļ�*/	
        TmpRetFr = f_open(&vStWirteFile, vStLogWrFileInf.vFileNameBuf, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
        if (TmpRetFr != FR_OK) 
        {	
            //open fats file err
            f_close(&vStWirteFile);
            vFatsOpStatu = E_FATS_OPRATION_STATU_OTHER_ERR; 
//            return FALSE; 
            continue ;            
        
        } 
        
//vStWirteFile.obj.id
        if((TmpRetFr = f_lseek(&vStWirteFile, f_size(&vStWirteFile))) != FR_OK)//׷��vStWirteFile.bufβ��д��f_size(&vStWirteFile)��BYTE����	
        {
            //lseek fats file err
            f_close(&vStWirteFile);
            vFatsOpStatu = E_FATS_OPRATION_STATU_OTHER_ERR;	
//            return FALSE;  
           continue ;  
        }

        if((TmpRetFr = f_write(&vStWirteFile, TmpStWrFileInf.vFileInfoBuf, TmpStWrFileInf.vFileInfoLen, &TmpWriteBytes)) == FR_OK)//��Ҫ׷��TmpStWrFileInf.vFileInfoBuf��vStWirteFile.bufβ����ʵ��д����TmpWriteBytes��BYTE����
        {
            //write fats file err
            f_close(&vStWirteFile);
            vFatsOpStatu = E_FATS_OPRATION_STATU_WRITE_ERR;	
//            return FALSE;   
           continue ;  
        }
        
        if((TmpRetFr = f_close(&vStWirteFile)) == FR_OK)//�ر��ļ�  
        {	 
    //						vFatsOpType = E_FATS_OPRATION_TYPE_NUMS;  
            vFatsOpStatu = E_FATS_OPRATION_STATU_WRITE_OK;	
        }
        else
        {
            //close fats file err
            vFatsOpStatu = E_FATS_OPRATION_STATU_OTHER_ERR;	
//            return FALSE;  
            continue ; 
        }	
        
        if (vStReFile.vStFileNoInfo.vSdStatu == E_SDCARD_STATU_EXIT_OK)
        {
            
            if ((TmpRetFr == FR_OK) && (vFatsOpStatu == E_FATS_OPRATION_STATU_WRITE_OK))
            {
                //�ж��ļ��Ƿ��Ѿ��������ƴ�С�����������vFatsFileCreat = E_FATS_FILE_CREAT_NO�����������ļ���
                TmpWrOffNow = f_tell(&vStWirteFile);
                if (TmpWrOffNow > D_CHWS_APP_SD_PER_FILE_MAX_BYTES)
                    vFatsLogFileCreat = E_FATS_FILE_CREAT_NO;
            }
            else
            {

            }
        }
        else
        {
            TmpStReFile.vStFileNoInfo.vSdStatu = vStReFile.vStFileNoInfo.vSdStatu;
            TmpStReFile.vStFileNoInfo.vFileOperType = E_FILE_OPER_TYPE_FILE_INFO;
    //			vStReFile.vStFileNoInfo.vFileNums = 0;
        }
    }
    return TRUE;

} 

/* ***************************************************************
* Funciton : LogWriteFileInfo()
* Description : д����־�ļ�����
* Call By : any 
* Parameters : ArgWrLen:��д���ļ����ݳ���,ArgWrInfo:��д���ļ�����
* Return : bool �Ƿ�ɹ�д�룬��ʱֻ���жϴ�д�볤���Ƿ񳬳���д����ļ���������
* Other : 
****************************************************************/
bool LogWriteFileInfo( uint8_t *ArgWrInfo)
{
	bool TmpRet = FALSE;
	SD_WRITE_FILE_INFO_T TmpStWrFileInf;		//��ǰд�ļ���Ϣ	
	uint16_t ArgWrLen;

	ArgWrLen = strlen(ArgWrInfo);// 20 BYTE

	memset(&TmpStWrFileInf, 0, sizeof(TmpStWrFileInf));

	if ((ArgWrLen > SD_WRITE_LOG_FILE_INFO_LEN) || (vStLogWrFileInf.vFileNameLen == 0))
		return TmpRet;
	
	TmpStWrFileInf.vFileNameLen = vStLogWrFileInf.vFileNameLen;
	memcpy(TmpStWrFileInf.vFileNameBuf, vStLogWrFileInf.vFileNameBuf, vStLogWrFileInf.vFileNameLen);

	TmpStWrFileInf.vFileInfoLen = ArgWrLen;
	memcpy(TmpStWrFileInf.vFileInfoBuf, ArgWrInfo, ArgWrLen);
//	xQueueSend(SdWriteLogDataQueue, (void *)(&TmpStWrFileInf), 100/portTICK_RATE_MS);

	TmpRet = TRUE;

	return TmpRet;
} 

/* ***************************************************************
* Funciton : SdcardCreatLogFile()
* Description : sdcard���������
* 				jack.qiu 20190430
* 				yintao 20190522
* 				
*	
* Call By : SD_WriteLogFile
* Parameters : NA 
* Return : void
* Other : ������
****************************************************************/
void SdcardCreatLogFile(void)
{
	FRESULT TmpRetFr = FR_INVALID_PARAMETER;	/* Return value */
	char ArgSerchLogSdFileName[D_CHWS_APP_SD_FILE_NAME_LEN];	//������־�ļ�����
	UINT TmpWriteBytes;

	
	if(vSdcardExitFlag == SD_NOT_PRESENT)
		return;

	vFatsFileCreat = vFatsLogFileCreat ;
	
	if(vFatsFileCreat == E_FATS_FILE_CREAT_OK)
		return;
	
	
	memset(ArgSerchLogSdFileName, 0, sizeof(ArgSerchLogSdFileName));

//	memcpy(vLogFileTypeName, vFileTypeLog, strlen(vFileTypeLog));
//	memcpy(vproFileTypeName, vFileTypePro, strlen(vFileTypePro));
	
	
	SdcardGenerFileName(ArgSerchLogSdFileName,vLogFileTypeName);
	

	
	
//������־�͹����ļ�һ�𴴽���
	// 1 Ѱ��û��ͬ�����ļ�
	if((TmpRetFr = f_open(&vStWirteFile, ArgSerchLogSdFileName, FA_OPEN_EXISTING)) == FR_NO_FILE)
	{
		f_close(&vStWirteFile);
		// 2 �����µ��ļ���
		if((TmpRetFr = f_open(&vStWirteFile, /*"log_2102251543.txt"*/ArgSerchLogSdFileName, FA_READ|FA_WRITE|FA_OPEN_ALWAYS)) == FR_OK)
		{
			vFatsLogFileCreat = E_FATS_LOG_FILE_CREAT_OK;
			vStLogWrFileInf.vFileNameLen = strlen(ArgSerchLogSdFileName);
			memcpy(vStLogWrFileInf.vFileNameBuf, ArgSerchLogSdFileName, strlen(ArgSerchLogSdFileName));

			if((TmpRetFr = f_lseek(&vStWirteFile, f_size(&vStWirteFile))) == FR_OK)
			{
				if((TmpRetFr = f_write(&vStWirteFile, "123456asdf\r\n", sizeof("123456asdf\r\n"), &TmpWriteBytes)) == FR_OK)
				{
				}
			}
		}
		else
		{
			vFatsLogFileCreat = E_FATS_LOG_FILE_CREAT_NO;
		}
		f_close(&vStWirteFile);
		

//�����ļ�ʱ��д���Ʒ��Ϣ	
//		LogWriteFileInfo(SD_HEAD_INFO_PROD_MAX_BYTES, vSdFileHeadInfoProd);
//		LogWriteFileInfo(SD_HEAD_INFO_PROD_MAX_BYTES, vSdFileHeadInfoPar);		

		
		LogWriteFileInfo(vSdFileHeadInfoProd);
		SD_WriteLogFile();
//		LogWriteFileInfo(vLogSdFileHeadInfoPar);	
//		SD_WriteLogFile();
		
	}
	else
	{
		vStLogWrFileInf.vFileNameLen = strlen(ArgSerchLogSdFileName);
		memcpy(vStLogWrFileInf.vFileNameBuf, ArgSerchLogSdFileName, strlen(ArgSerchLogSdFileName));
		vFatsLogFileCreat = E_FATS_LOG_FILE_CREAT_OK;
		f_close(&vStWirteFile);
		
	}



}

void SdcardGenerFileName(char *ArgSerchSdFileName,char *vFileTypeName)
{
	uint8_t TmpCpLen = 0;
	uint8_t TmpCpOffset = 0;
//	USER_INFO_T TmpStUserInfo;
//	char vUserTypeName[USER_TYPE_NAME_LEN];	//�ļ�����
	
	static SDCARD_FILE_NAME_INFO_T TmpSdFileNameInfo;

//	GetFileTypeInfo(vUserTypeName);
//	GetFileTypeInfo(TmpSdFileNameInfo.vUserTypeName);	
	memset(TmpSdFileNameInfo.vUserTypeName,0,sizeof(TmpSdFileNameInfo.vUserTypeName));
	memcpy(TmpSdFileNameInfo.vUserTypeName, vFileTypeName, strlen(vFileTypeName));		

	
	GetSystemDataTime(&vSysDataTime);

//	memcpy(TmpSdFileNameInfo.vUserTypeName, vUserTypeName, strlen(vUserTypeName));

#if 1
	sprintf(TmpSdFileNameInfo.vUserDate, "%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d", vSysDataTime.vDataYear[3], 
	vSysDataTime.vDataYear[2], vSysDataTime.vDataYear[1], vSysDataTime.vDataYear[0],vSysDataTime.vDataMonth[1], 
	vSysDataTime.vDataMonth[0], vSysDataTime.vDataDay[1], vSysDataTime.vDataDay[0], vSysDataTime.vDataHour[1], 
	vSysDataTime.vDataHour[0], vSysDataTime.vDataMin[1], vSysDataTime.vDataMin[0], vSysDataTime.vDataSec[1], vSysDataTime.vDataSec[0]);
#else
	sprintf(TmpSdFileNameInfo.vUserDate, "%1d%1d%1d%1d%1d%1d", vSysDataTime.vDataDay[1], vSysDataTime.vDataDay[0], vSysDataTime.vDataHour[1], 
	vSysDataTime.vDataHour[0], vSysDataTime.vDataMin[1], vSysDataTime.vDataMin[0]);
#endif
	
	//�������ɵ���־�ļ����ṹ��
	//--------------------------------1
//	//����ǰ׺
//	ArgSerchSdFileName[TmpCpOffset] = D_CHWS_APP_SD_FILE_USR_NAME_PREFIX;
//	TmpCpLen = 1;
//	TmpCpOffset += TmpCpLen;
	
	//�����ļ�����
	if (strlen(TmpSdFileNameInfo.vUserTypeName) > (USER_TYPE_NAME_LEN - 1))
		TmpCpLen = USER_TYPE_NAME_LEN - 1;
	else
		TmpCpLen = strlen(TmpSdFileNameInfo.vUserTypeName);
	
	memcpy(&ArgSerchSdFileName[TmpCpOffset], TmpSdFileNameInfo.vUserTypeName, TmpCpLen);
	TmpCpOffset += TmpCpLen;

	
	//--------------------------------2
	//����ǰ׺" log "
//	memcpy(&ArgSerchSdFileName[TmpCpOffset], SD_FILE_USR_LOG_PREFIX, 3);
//	TmpCpLen = 3;
//	TmpCpOffset += TmpCpLen;
	
	//�������ӷ�" _ "
	ArgSerchSdFileName[TmpCpOffset] = SD_FILE_CON;
	TmpCpLen = 1;
	TmpCpOffset += TmpCpLen;

	//--------------------------------2
	//����ǰ׺" d "
	ArgSerchSdFileName[TmpCpOffset] = SD_FILE_USR_DATE_PREFIX;
	TmpCpLen = 1;
	TmpCpOffset += TmpCpLen;
	
	//��������
	if (strlen(TmpSdFileNameInfo.vUserDate) > (USER_DATE_LEN- 1))
		TmpCpLen = USER_DATE_LEN - 1;
	else
		TmpCpLen = strlen(TmpSdFileNameInfo.vUserDate);
	memcpy(&ArgSerchSdFileName[TmpCpOffset], TmpSdFileNameInfo.vUserDate, TmpCpLen);
	TmpCpOffset += TmpCpLen;
	//--------------------------------5
	//�����ļ����ͺ�׺
	memcpy(&ArgSerchSdFileName[TmpCpOffset], SD_FILE_SUFFIX, USER_SUFF_LEN);
	
}

/* ***************************************************************
* Funciton : SdcardStatuCheck()
* Description : sdcard״̬���
* 				jack.qiu 20190430
* 				yintao 20190522
* 				����sdcard״̬��
* Call By : SD_WriteLogFile,
* Parameters : NA 
* Return : void
* Other : ��������
****************************************************************/
bool SdcardStatuCheck(void)
{
	//EventBits_t EventWaitOtgRecSdstatuBit = 0;

	if (vSdcardExitFlag == SD_NOT_PRESENT)//����־Ϊ������
	{
		if (BSP_SD_IsDetected() == SD_PRESENT)//��⵽�����ڣ��������ñ�־����
		{
			if (f_mount(&SDFatFS, SDPath, 0) == FR_OK)
			{
				vSdcardExitFlag = SD_PRESENT; //���سɹ��������ÿ����ڱ�־
//				vStReFile.vStFileNoInfo.vSdStatu = E_SDCARD_STATU_EXIT_OK;
//				vStReFile.vStFileNoInfo.vFileNums = 0;
                return TRUE;
			}
		}
	}
	else
	{
		if (BSP_SD_IsDetected() != SD_PRESENT)//��⵽�������ڣ��������ñ�־������
		{
//			vStReFile.vStFileNoInfo.vSdStatu = E_SDCARD_STATU_NO_EXIT;
//			vStReFile.vStFileNoInfo.vFileNums = 0;
			vSdcardExitFlag = SD_NOT_PRESENT; //�ر��ļ��ɹ��������ÿ������ڱ�־
			vFatsFileCreat = E_FATS_FILE_CREAT_NO;
            return FALSE;

		}
	}
	
	//EventWaitOtgRecSdstatuBit = xEventGroupWaitBits(ChwsEventGroup, E_CHWS_EVENT_SD_WAIT_OTG_REC_SDSTATU, FALSE, TRUE, 1/portTICK_RATE_MS);
	//if (0 != (EventWaitOtgRecSdstatuBit & E_CHWS_EVENT_SD_WAIT_OTG_REC_SDSTATU))
	//{
		//��ʱ�ر�
		//xQueueSend(ChwsqRepSdStatuQueue, (void *)(&vSdcardExitFlag), 100/portTICK_RATE_MS);//���û�б��������ʾ��δ�յ���������
	//}
	return 0;
}


void GetSystemDataTime(DATA_TIME_T *ArgDataTime)
{

//	if(BLE_RTC.Status != 0x01)
	{
		BLE_RTC.years = systime.year;
		BLE_RTC.months  = systime.month;
		BLE_RTC.days = systime.day;
		BLE_RTC.hours  = systime.hour;
		BLE_RTC.minutes = systime.minute;
		BLE_RTC.sec = systime.second;
		BLE_RTC.weeks = systime.week;
	}
	

	ArgDataTime->vDataYear[0] = BLE_RTC.years % 10;				// 9	
	ArgDataTime->vDataYear[1] = BLE_RTC.years /10 ; 	// 1
	ArgDataTime->vDataYear[2] = 0;	// 0
	ArgDataTime->vDataYear[3] = 2;		// 2	-->2019��


	ArgDataTime->vDataMonth[0] = BLE_RTC.months % 10;
	ArgDataTime->vDataMonth[1] = BLE_RTC.months / 10 ;

	ArgDataTime->vDataDay[0] = BLE_RTC.days % 10;
	ArgDataTime->vDataDay[1] = BLE_RTC.days / 10 ;

	ArgDataTime->vDataHour[0] = BLE_RTC.hours % 10;
	ArgDataTime->vDataHour[1] = BLE_RTC.hours / 10 ;

	ArgDataTime->vDataMin[0] = BLE_RTC.minutes % 10;
	ArgDataTime->vDataMin[1] = BLE_RTC.minutes / 10;

	ArgDataTime->vDataSec[0] = BLE_RTC.sec % 10;
	ArgDataTime->vDataSec[1] = BLE_RTC.sec / 10;

}


