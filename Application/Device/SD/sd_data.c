/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : sd_data.c
  * @brief          : sd_data
  * @author			: ZYX
  * @time			: 2021.03.31
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 SZCHWS.
  * All rights reserved.
  ******************************************************************************/
#include "sd_data.h"
#include "sd_log.h"
#include "utility.h"


char vProSdFileHeadInfoPar[SD_HEAD_PRO_INFO_PAR_MAX_BYTES];		//sd卡内头部信息参数信息部分

FATS_PRO_FILE_CREAT_E vFatsProFileCreat = E_FATS_PRO_FILE_CREAT_NO;	//sd卡里面过程文件创建结果	

SD_WRITE_FILE_INFO_T vStProWrFileInf;		//当前写过程数据文件信息，所有写文件接口都要调用此变量	

uint8_t imuSave_tmp[SD_SAVE_DATA_LENTH]={0};
uint16_t imuSave_len = 0;
uint16_t imuSave_startFlag = STOPSAVE;

FIL vSDReadFile; 
FIL vSDWirteFile; 
FIL vSDWirteProFile; 

extern DATA_TIME_T vSysDataTime;		//系统当前日期时间，北京时区
extern uint8_t vSdcardExitFlag;
extern char vProFileTypeName[USER_TYPE_NAME_LEN];	// 过程文件类型名称
extern FATS_OPRATION_STATU_E vFatsOpStatu;

extern volatile QueueHandle_t SdWriteProDataQueue;

extern SD_READ_FILE_T vStReFile;				//读文件列表或文件信息
extern FIL vStWirteTestFile;

uint8_t Hex_Data[]={0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,\
	0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,\
	0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,\
	0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,\
	0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,0xA1,0xB2,0xC3,0xD4,0xE5,\
	};
char temple_buf[3]={0};
char objectBuf[100]={0};
char imuSaveDatBuf[IMU_SAVEDAT_LENTH_TMP]={0};

//hex类型转化为char类型
unsigned char HexToChar(unsigned char byTemp)
{
	byTemp &= 0x0f;
	if(byTemp >= 10)		// show 'a' - 'f'
	{
		byTemp = byTemp - 0xa + 0x61;
	}
	else		// show '0' - '9'
	{
		byTemp = byTemp + 0x30;
	}
	return(byTemp);
}

//将单个字节hex类型转化为char类型
void hex_u8print(unsigned char byte)
{
	temple_buf[0] = HexToChar((byte & 0xf0) >> 4);
	temple_buf[1] = HexToChar(byte & 0x0f);
	temple_buf[2] = ' ';
}

//将原始数据（16进制）转化为char类型，用于SD卡保存
void imuDataHexToChar(uint8_t *hexDat, uint16_t hexLen)
{
	uint16_t i=0;
	
	imuSaveDatBuf[0] = '\n';
	
	for(i=0;i<hexLen;i++)
	{
		memset(temple_buf,0,sizeof(temple_buf));
		hex_u8print(hexDat[i]);
		imuSaveDatBuf[i*3+1] = temple_buf[0];
		imuSaveDatBuf[i*3+2] = temple_buf[1];
		imuSaveDatBuf[i*3+3] = temple_buf[2];
	}
	imuSaveDatBuf[hexLen*3+1] = '\0';
}

uint16_t tmpSendInfolen=0;
SD_RW_PRO_DATA_T tmpProWriteInfo;

//IMU数据组包，一帧31字节
void imuDataSaveTmp(uint8_t *rebuf, uint16_t lenth)
{

	if(lenth==0)
		return;
	

	if(lenth!=0)
	{

		memcpy(&imuSave_tmp[imuSave_len],rebuf,lenth);

		imuSave_len += lenth;

		if(imuSave_len==SD_SAVE_DATA_LENTH)
		{
			imuSave_len = 0;
			imuSave_startFlag = STARTSAVE;
			
//			tmpProWriteInfo.dat_len = 0;
//			memset(tmpProWriteInfo.prodata_buf,0,sizeof(tmpProWriteInfo.prodata_buf));
			
//			tmpProWriteInfo.dat_len = SD_SAVE_DATA_LENTH;
//			memcpy(tmpProWriteInfo.prodata_buf,imuSave_tmp,SD_SAVE_DATA_LENTH);

			tmpSendInfolen = SD_SAVE_DATA_LENTH;
			
		}
	
	}
}

bool imuDataStartSaveSDCard(void)
{
	SD_WRITE_PRO_FILE_INFO_T TmpStWrFileInf;
	uint16_t sdatalen;
	
	UINT TmpWriteBytes;
	bool TmpRet = FALSE;
	FRESULT TmpRetFr; 	/* Return value */

	if(vFatsProFileCreat == E_FATS_PRO_FILE_CREAT_NO)
		return FALSE;
	
	
	if(imuSave_startFlag == STARTSAVE)
	{
//		memset(TmpStWrFileInf.vFileInfoBuf,0,sizeof(TmpStWrFileInf.vFileInfoBuf));
//		sdatalen = sizeof(imuSave_tmp);
//		TmpStWrFileInf.vFileInfoLen = sdatalen;
//		memcpy(TmpStWrFileInf.vFileInfoBuf,imuSave_tmp,sdatalen);

//		TmpRet = xQueueSend(SdWriteProDataQueue, (void *)(&TmpStWrFileInf), 0);
//		if(TmpRet==0)
//			return TRUE;

		imuDataHexToChar(imuSave_tmp,sizeof(imuSave_tmp));
		imuSave_startFlag = STOPSAVE;

		if((TmpRetFr = f_open(&vSDWirteProFile, vStProWrFileInf.vFileNameBuf, FA_READ|FA_WRITE|FA_OPEN_ALWAYS)) == FR_OK)
		{
			if((TmpRetFr = f_lseek(&vSDWirteProFile, f_size(&vSDWirteProFile))) == FR_OK)
			{
				if((TmpRetFr = f_write(&vSDWirteProFile, imuSaveDatBuf, strlen(imuSaveDatBuf), &TmpWriteBytes)) == FR_OK)
				{
				}
			}
		}
		else
		{
		}
		f_close(&vSDWirteProFile);

#if 0
		/*文件系统基本操作：打开文件，定位到文件结尾，写入内容，关闭文件*/	
		TmpRetFr = f_open(&vSDWirteProFile, "123.txt"/*vStProWrFileInf.vFileNameBuf*/, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);

		if (TmpRetFr == FR_OK) 
		{	 
			//vStWirteFile.obj.id
			if((TmpRetFr = f_lseek(&vSDWirteProFile, f_size(&vSDWirteProFile))) == FR_OK)//追加vStWirteFile.buf尾部写入f_size(&vStWirteFile)个BYTE数据	
			{
				if((TmpRetFr = f_write(&vSDWirteProFile, "123456asdf\r\n"/*TmpStWrFileInf.vFileInfoBuf*/, sizeof("123456asdf\r\n")/*TmpStWrFileInf.vFileInfoLen*/, &TmpWriteBytes)) == FR_OK)//想要追加TmpStWrFileInf.vFileInfoBuf到vStWirteFile.buf尾部，实际写入了TmpWriteBytes个BYTE内容
				{	 
					if((TmpRetFr = f_close(&vSDWirteProFile)) == FR_OK)//关闭文件  
					{	 
//						vFatsOpType = E_FATS_OPRATION_TYPE_NUMS;  
						vFatsOpStatu = E_FATS_OPRATION_STATU_WRITE_OK;	
					}
					else
					{
						//close fats file err
						vFatsOpStatu = E_FATS_OPRATION_STATU_OTHER_ERR;	
						return FALSE;
					}
				}  
				else
				{
					//write fats file err
					f_close(&vSDWirteProFile);
					vFatsOpStatu = E_FATS_OPRATION_STATU_WRITE_ERR;	
					return FALSE;
				}
			}  
			else
			{
				//lseek fats file err
				f_close(&vSDWirteProFile);
				vFatsOpStatu = E_FATS_OPRATION_STATU_OTHER_ERR;	
				return FALSE;
			}
		}
		else
		{
			//open fats file err
			f_close(&vSDWirteProFile);
			vFatsOpStatu = E_FATS_OPRATION_STATU_OTHER_ERR;  
			return FALSE;
		}
#endif
	}
	return TRUE;
}

/* ***************************************************************
* Funciton : ProcessDataWriteFileInfo()
* Description : 写入过程数据内容
* Call By : any 
* Parameters : ArgWrLen:待写入文件内容长度,ArgWrInfo:待写入文件内容
* Return : bool 是否成功写入，暂时只会判断待写入长度是否超出和写入的文件还不存在
* Other : 
****************************************************************/
bool ProcessDataWriteFileInfo( uint8_t *arrayWrInfo)
{

	uint16_t arrayWrLen;
    
	bool TmpRet = FALSE;
	SD_WRITE_PRO_FILE_INFO_T TmpStWrFileInf;		//当前写文件信息	
	
	arrayWrLen = strlen(arrayWrInfo);//

	memset(&TmpStWrFileInf, 0, sizeof(TmpStWrFileInf));

	if ((arrayWrLen > D_CHWS_APP_SD_WRITE_FILE_INFO_LEN) || (vStProWrFileInf.vFileNameLen == 0))
		return TmpRet;
	
//	TmpStWrFileInf.vFileNameLen = vStProWrFileInf.vFileNameLen;
//	memcpy(TmpStWrFileInf.vFileNameBuf, vStProWrFileInf.vFileNameBuf, vStProWrFileInf.vFileNameLen);

	TmpStWrFileInf.vFileInfoLen = arrayWrLen;
	memcpy(TmpStWrFileInf.vFileInfoBuf, arrayWrInfo, arrayWrLen);
	TmpRet = xQueueSend(SdWriteProDataQueue, (void *)(&TmpStWrFileInf), 0);
	
	if(TmpRet ==0)
		TmpRet = TRUE;

	return TmpRet;
} 


uint8_t WriteProFileParaHeadInfo(void)
{
	uint8_t FileHeadOffset = 0;
	uint8_t TmpStrlen = 0;

	char arrayTmpbuf[30]={0};
	
	const char StringStarthead[]= "P00 prodata File\n";
	const char StringStarttime[]= "\nTime: ";
	const char StringProdata[]= "\nProdata:\n";
	
	GetSystemDataTime(&vSysDataTime);
	
	memset(vProSdFileHeadInfoPar,0,sizeof(vProSdFileHeadInfoPar));

	strcat(&vProSdFileHeadInfoPar[0],StringStarthead);	//"P00 Log File"
	FileHeadOffset += strlen(StringStarthead);

	strcat(vProSdFileHeadInfoPar,StringStarttime);		//"Starttime:"
	FileHeadOffset += strlen(StringStarttime);

	sprintf(arrayTmpbuf, "%1d%1d%1d%1d-%1d%1d-%1d%1d %1d%1d:%1d%1d:%1d%1d", vSysDataTime.vDataYear[3], 
	vSysDataTime.vDataYear[2], vSysDataTime.vDataYear[1], vSysDataTime.vDataYear[0],vSysDataTime.vDataMonth[1], 
	vSysDataTime.vDataMonth[0], vSysDataTime.vDataDay[1], vSysDataTime.vDataDay[0], vSysDataTime.vDataHour[1], 
	vSysDataTime.vDataHour[0], vSysDataTime.vDataMin[1], vSysDataTime.vDataMin[0], vSysDataTime.vDataSec[1], vSysDataTime.vDataSec[0]);

	TmpStrlen = strlen(arrayTmpbuf);
	memcpy(&vProSdFileHeadInfoPar[FileHeadOffset],arrayTmpbuf,TmpStrlen);
	FileHeadOffset += TmpStrlen;
	
	strcat(vProSdFileHeadInfoPar,StringProdata);		//"\nProdata:\n"
	FileHeadOffset += strlen(StringProdata);
	
	return 0;
}

void SdcardCreatProFile_Zyx(void)
{
	
	FRESULT TmpRetFr = FR_INVALID_PARAMETER;	/* Return value */
	char ArgSerchProSdFileName[D_CHWS_APP_SD_FILE_NAME_LEN];	//搜索日志文件名字
	UINT TmpWriteBytes;
	
	if(vSdcardExitFlag == SD_NOT_PRESENT)
		return;

//	vFatsFileCreat = vFatsLogFileCreat ;
	
	if(vFatsProFileCreat == E_FATS_PRO_FILE_CREAT_OK)
		return;
	
	
	memset(ArgSerchProSdFileName, 0, sizeof(ArgSerchProSdFileName));

//	memcpy(vLogFileTypeName, vFileTypeLog, strlen(vFileTypeLog));
//	memcpy(vproFileTypeName, vFileTypePro, strlen(vFileTypePro));
	
	
	SdcardGenerFileName(ArgSerchProSdFileName,vProFileTypeName);

	WriteProFileParaHeadInfo();

	
	//创建日志和过程文件一起创建，
	// 1 寻找没有同名的文件
	if((TmpRetFr = f_open(&vSDWirteFile, ArgSerchProSdFileName, FA_OPEN_EXISTING)) == FR_NO_FILE)
	{
		f_close(&vSDWirteFile);
		// 2 创建新的文件名
		if((TmpRetFr = f_open(&vSDWirteFile, ArgSerchProSdFileName, FA_READ|FA_WRITE|FA_OPEN_ALWAYS)) == FR_OK)
		{
			vStProWrFileInf.vFileNameLen = strlen(ArgSerchProSdFileName);
			memcpy(vStProWrFileInf.vFileNameBuf, ArgSerchProSdFileName, strlen(ArgSerchProSdFileName));

			if((TmpRetFr = f_lseek(&vSDWirteFile, f_size(&vSDWirteFile))) == FR_OK)
			{
				if((TmpRetFr = f_write(&vSDWirteFile, vProSdFileHeadInfoPar, sizeof(vProSdFileHeadInfoPar), &TmpWriteBytes)) == FR_OK)
				{
					vFatsProFileCreat = E_FATS_PRO_FILE_CREAT_OK;
				}
			}
		}
		else
		{
			vFatsProFileCreat = E_FATS_PRO_FILE_CREAT_NO;
		}
		f_close(&vSDWirteFile);
				
	}
	else
	{
//		vStProWrFileInf.vFileNameLen = strlen(ArgSerchProSdFileName);
//		memcpy(vStProWrFileInf.vFileNameBuf, ArgSerchProSdFileName, strlen(ArgSerchProSdFileName));
		vFatsProFileCreat = E_FATS_PRO_FILE_CREAT_OK;
		f_close(&vSDWirteFile);
		
	}
	
}

/* ***************************************************************
* Funciton : SD_WriteLogFile()
* Description : sdcard操作主入口
* 				读写SDCARD文件接口,用于存放外骨骼运行期间过程数据
* Call By : safety_task
* Parameters : NA 
* Return : void
* Other : 不可重入
****************************************************************/
bool SD_WriteProFile(void)
{
	
	UINT TmpWriteBytes;
	FRESULT TmpRetFr; 	/* Return value */

	BaseType_t TmpRecRet = pdFAIL;

	static SD_READ_FILE_T TmpStReFile;

	SD_WRITE_FILE_INFO_T TmpStWrFileInf;		//当前写文件信息

	SD_WRITE_PRO_FILE_INFO_T TmpStWrProFileInf;

	uint32_t TmpWrOffNow = 0; //当前写完文件的偏移量
	uint8_t TmpLoopWrCnt = 15; //当前循环几次写过程文件

	memset(&TmpStWrFileInf, 0, sizeof(TmpStWrFileInf));

	SdcardStatuCheck();


	if (vFatsProFileCreat == E_FATS_FILE_CREAT_NO)
	{
		SdcardCreatProFile_Zyx();
		return 0;
	}

	for(; TmpLoopWrCnt>0;TmpLoopWrCnt--)
	{		
		TmpRecRet = xQueueReceive(SdWriteProDataQueue, (void *)(&TmpStWrProFileInf), 0);
		if (TmpRecRet == pdPASS)
		{
			/*文件系统基本操作：打开文件，定位到文件结尾，写入内容，关闭文件*/	
			TmpRetFr = f_open(&vStWirteFile, vStProWrFileInf.vFileNameBuf, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);

			if (TmpRetFr == FR_OK) 
			{	 
				//vStWirteFile.obj.id
				if((TmpRetFr = f_lseek(&vStWirteFile, f_size(&vStWirteFile))) == FR_OK)//追加vStWirteFile.buf尾部写入f_size(&vStWirteFile)个BYTE数据	
				{
					if((TmpRetFr = f_write(&vStWirteFile, TmpStWrProFileInf.vFileInfoBuf, TmpStWrProFileInf.vFileInfoLen, &TmpWriteBytes)) == FR_OK)//想要追加TmpStWrFileInf.vFileInfoBuf到vStWirteFile.buf尾部，实际写入了TmpWriteBytes个BYTE内容
					{	 
						if((TmpRetFr = f_close(&vStWirteFile)) == FR_OK)//关闭文件  
						{	 
	//						vFatsOpType = E_FATS_OPRATION_TYPE_NUMS;  
							vFatsOpStatu = E_FATS_OPRATION_STATU_WRITE_OK;	
						}
						else
						{
							//close fats file err
							vFatsOpStatu = E_FATS_OPRATION_STATU_OTHER_ERR;	
						}
					}  
					else
					{
						//write fats file err
						f_close(&vStWirteFile);
						vFatsOpStatu = E_FATS_OPRATION_STATU_WRITE_ERR;	
					}
				}  
				else
				{
					//lseek fats file err
					f_close(&vStWirteFile);
					vFatsOpStatu = E_FATS_OPRATION_STATU_OTHER_ERR;	
				}
			}
			else
			{
				//open fats file err
				f_close(&vStWirteFile);
				vFatsOpStatu = E_FATS_OPRATION_STATU_OTHER_ERR;  
			}
		
			if (vStReFile.vStFileNoInfo.vSdStatu == E_SDCARD_STATU_EXIT_OK)
			{
				
				if ((TmpRetFr == FR_OK) && (vFatsOpStatu == E_FATS_OPRATION_STATU_WRITE_OK))
				{
					//判断文件是否已经超出限制大小，超出则调用vFatsFileCreat = E_FATS_FILE_CREAT_NO来重新生成文件名
					TmpWrOffNow = f_tell(&vStWirteFile);
					if (TmpWrOffNow > D_CHWS_APP_SD_PER_FILE_MAX_BYTES)
						vFatsProFileCreat = E_FATS_PRO_FILE_CREAT_NO;
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
		
	}

	return TRUE;
}

