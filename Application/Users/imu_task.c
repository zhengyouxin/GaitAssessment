/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : imu_task.c
  * @brief          : imu_task app
  * @author			: ZYX
  * @time			: 2020.11
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 SZCHWS.
  * All rights reserved.
  ******************************************************************************/
#include "imu_task.h"
#include "cmsis_os.h"
#include "uart.h"
#include "board.h"
#include <string.h>
#include "utility.h"
#include "systime.h"
#include "sd_log.h"

uint8_t sConvertBuf[50]={0};
uint8_t convertBuf2Send[50]={0};

extern uint8_t uart1_rxbuffer[UART_RX_LENMAX];
extern uint8_t uart2_rxbuffer[UART_RX_LENMAX];
extern uint8_t uart3_rxbuffer[UART_RX_LENMAX];
extern uint8_t imu1_rxbuf[IMU_RX_BUFLEN];
extern uint8_t imu2_rxbuf[IMU_RX_BUFLEN];
extern uint8_t uRec_num;

extern UART_HandleTypeDef huart4;

extern volatile bleData ble_data;

extern volatile QueueHandle_t SdWriteProDataQueue;
extern uint16_t tmpSendInfolen;
extern SD_RW_PRO_DATA_T tmpProWriteInfo;
uint16_t sQueueCnt=0;

#define StartIMU 	1
#define StopIMU		2

volatile uint32_t sysTime_ms = 0;
volatile uint32_t sysTime_sync_it = 0;
volatile uint32_t sysTime_uart_it = 0;
volatile uint32_t sysTime_ua_min_sy = 0;
volatile uint8_t sysTimegetFlag = 0;
volatile uint32_t imuDataRecErro = 0;

volatile uint32_t uart1Com=0;
volatile uint32_t uart2Com=0;
volatile uint32_t uart3Com=0;
volatile uint32_t uart4Com=0;

extern void imuDataSaveTmp(uint8_t *rebuf, uint16_t lenth);

uint8_t getSYNCFlag = 0;
volatile uint8_t SYNCFnished = 0;

uint8_t GotoConfig[5]={0xFA,0xFF,0x30,0x00,0xD1};	//设置模式，不发数据
uint8_t GotoMesure[5]={0xFA,0xFF,0x10,0x00,0xF1};	//测量模式，发数据 100Hz

void buf_init(void)
{
	
	memset(sConvertBuf,0,sizeof(sConvertBuf));
	memset(convertBuf2Send,0,sizeof(convertBuf2Send));
	
	sConvertBuf[0] = 0xFA;
	sConvertBuf[1] = 0xFF;

	convertBuf2Send[0] = 0xFA;
	convertBuf2Send[1] = 0xFF;
}


union
{
	float		fa;
	uint8_t  	bb[4];
}sa;

uint16_t acc_16=0,acc_16_1=0,acc_16_2=0,acc_16_3=0;
uint16_t gyr_16=0,gyr_16_1=0,gyr_16_2=0,gyr_16_3=0;
float acc=0;
float saout=0;

uint8_t IMUSeqSave[256*10]={0};
uint16_t SeqErro = 0;
uint16_t seqCnt=0;

//IMU数据转换处理：添加序号
void imu_data_handle_sequence(uint8_t *rebuf, uint8_t pnum, uint16_t lenth)
{
	static uint8_t send_cnt=0;

	if(!SYNCFnished)
		return;
//acc
	sConvertBuf[0] = 0xFA;
	sConvertBuf[1] = 0xFF;
	sConvertBuf[2] = 0xA0;

	if(pnum==1)
	{
		sConvertBuf[3] = rebuf[7];	//2021.5.28 add sequence
		sConvertBuf[4] = rebuf[8];
		IMUSeqSave[seqCnt++]=rebuf[7];
		IMUSeqSave[seqCnt++]=rebuf[8];

		if(seqCnt>3 && IMUSeqSave[seqCnt-1]!=0)
		{
			if(IMUSeqSave[seqCnt-1]-IMUSeqSave[seqCnt-3]!=1)
			{
				SeqErro ++;	//序号出错计数
			}
		}

		if(seqCnt==2560)	//防止内存溢出
		{
			seqCnt = 0;
			memset(IMUSeqSave,0,sizeof(IMUSeqSave));
		}
		
	}

#if 1
	sa.bb[0] = rebuf[15];
	sa.bb[1] = rebuf[14];
	sa.bb[2] = rebuf[13];
	sa.bb[3] = rebuf[12];
	saout	= sa.fa;
	acc_16_1 = (uint16_t)(saout*2048.0f/9.81f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
//		sConvertBuf[4] = 0x01;
		send_cnt ++;
		sConvertBuf[5] = acc_16_1>>8;
		sConvertBuf[6] = acc_16_1;
	}
	else if(pnum==2)
	{
		sConvertBuf[17] = 0x02;
		sConvertBuf[18] = acc_16_1>>8;
		sConvertBuf[19] = acc_16_1;
	}
	
	sa.bb[0] = rebuf[19];
	sa.bb[1] = rebuf[18];
	sa.bb[2] = rebuf[17];
	sa.bb[3] = rebuf[16];
	saout	= sa.fa;
	acc_16_2 = (uint16_t)(saout*2048.0f/9.81f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[7] = acc_16_2>>8;
		sConvertBuf[8] = acc_16_2;
	}
	else if(pnum==2)
	{
		sConvertBuf[20] = acc_16_2>>8;
		sConvertBuf[21] = acc_16_2;
	}
	
	sa.bb[0] = rebuf[23];
	sa.bb[1] = rebuf[22];
	sa.bb[2] = rebuf[21];
	sa.bb[3] = rebuf[20];
	saout	= sa.fa;
	acc_16_3 = (uint16_t)(saout*2048.0f/9.81f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[9] = acc_16_3>>8;
		sConvertBuf[10] = acc_16_3;
	}
	else if(pnum==2)
	{
		sConvertBuf[22] = acc_16_3>>8;
		sConvertBuf[23] = acc_16_3;
	}
	
//gyr
	sa.bb[0] = rebuf[30];
	sa.bb[1] = rebuf[29];
	sa.bb[2] = rebuf[28];
	sa.bb[3] = rebuf[27];
	saout	= sa.fa;
	gyr_16_1 = (uint16_t)(saout*16.4f*180/3.14f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[11] = gyr_16_1>>8;
		sConvertBuf[12] = gyr_16_1;
	}
	else if(pnum==2)
	{
		sConvertBuf[24] = gyr_16_1>>8;
		sConvertBuf[25] = gyr_16_1;
	}

	sa.bb[0] = rebuf[34];
	sa.bb[1] = rebuf[33];
	sa.bb[2] = rebuf[32];
	sa.bb[3] = rebuf[31];
	saout	= sa.fa;
	gyr_16_2 = (uint16_t)(saout*16.4f*180/3.14f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[13] = gyr_16_2>>8;
		sConvertBuf[14] = gyr_16_2;
	}
	else if(pnum==2)
	{
		sConvertBuf[26] = gyr_16_2>>8;
		sConvertBuf[27] = gyr_16_2;
	}
	
	sa.bb[0] = rebuf[38];
	sa.bb[1] = rebuf[37];
	sa.bb[2] = rebuf[36];
	sa.bb[3] = rebuf[35];
	saout	= sa.fa;
	gyr_16_3 = (uint16_t)(saout*16.4f*180/3.14f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[15] = gyr_16_3>>8;
		sConvertBuf[16] = gyr_16_3;
	}
	else if(pnum==2)
	{
		sConvertBuf[28] = gyr_16_3>>8;
		sConvertBuf[29] = gyr_16_3;
	}

	pnum = 0;
	
//	if(sConvertBuf[17] == 0x02 && sConvertBuf[4] == 0x01)
//	if(sConvertBuf[4] == 0x01)
	if(send_cnt && sConvertBuf[17] == 0x02)
	{
		sConvertBuf[30] = CheckSum(&sConvertBuf[2], 28);	//Buf lenth:31
		memcpy(convertBuf2Send,sConvertBuf,sizeof(sConvertBuf));
		memset(sConvertBuf,0,sizeof(sConvertBuf));
		send_cnt = 0;
#if (IMUModeSelect == DoubleIMUMode)
		if(convertBuf2Send[30] == CheckSum(&convertBuf2Send[2], 28))
		{
			HAL_UART_Transmit_DMA(&huart4,convertBuf2Send,SEND_IMU_2_LENTH);	//串口4发送数据，BLE透传用。
		}
		else
		{
			imuDataRecErro ++;
			memset(convertBuf2Send,0,sizeof(convertBuf2Send));
		}
#if WRITE_PRO_EN
		imuDataSaveTmp(convertBuf2Send,SEND_IMU_2_LENTH);
#endif
#endif
	}
#endif
}

//IMU数据转换处理
void imu_data_handle(uint8_t *rebuf, uint8_t pnum, uint16_t lenth)
{
	static uint8_t send_cnt=0;
#if 0
	//test
	sa.bb[0]=0x9e;
	sa.bb[1]=0x38;
	sa.bb[2]=0x30;
	sa.bb[3]=0x3f;
	saout	= sa.fa;
	acc_16 = (uint16_t)(saout*2048.0f/9.81f + 32768);
	memset(&sa.bb[0],0,4);
#endif

//acc
	sConvertBuf[0] = 0xFA;
	sConvertBuf[1] = 0xFF;
	sConvertBuf[2] = 0xA0;
	#if 0
	sConvertBuf[3] = 0x1A;
	#else
	if(pnum==1)
	{
		sConvertBuf[3] = send_cnt;	//2021.5.17 data count test
	}
	#endif
	
	sa.bb[0] = rebuf[10];
	sa.bb[1] = rebuf[9];
	sa.bb[2] = rebuf[8];
	sa.bb[3] = rebuf[7];
	saout	= sa.fa;
	acc_16_1 = (uint16_t)(saout*2048.0f/9.81f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[4] = 0x01;
		sConvertBuf[5] = acc_16_1>>8;
		sConvertBuf[6] = acc_16_1;
	}
	else if(pnum==2)
	{
		sConvertBuf[17] = 0x02;
		sConvertBuf[18] = acc_16_1>>8;
		sConvertBuf[19] = acc_16_1;
	}
	
	sa.bb[0] = rebuf[14];
	sa.bb[1] = rebuf[13];
	sa.bb[2] = rebuf[12];
	sa.bb[3] = rebuf[11];
	saout	= sa.fa;
	acc_16_2 = (uint16_t)(saout*2048.0f/9.81f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[7] = acc_16_2>>8;
		sConvertBuf[8] = acc_16_2;
	}
	else if(pnum==2)
	{
		sConvertBuf[20] = acc_16_2>>8;
		sConvertBuf[21] = acc_16_2;
	}
	
	sa.bb[0] = rebuf[18];
	sa.bb[1] = rebuf[17];
	sa.bb[2] = rebuf[16];
	sa.bb[3] = rebuf[15];
	saout	= sa.fa;
	acc_16_3 = (uint16_t)(saout*2048.0f/9.81f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[9] = acc_16_3>>8;
		sConvertBuf[10] = acc_16_3;
	}
	else if(pnum==2)
	{
		sConvertBuf[22] = acc_16_3>>8;
		sConvertBuf[23] = acc_16_3;
	}
	
//gyr
	sa.bb[0] = rebuf[25];
	sa.bb[1] = rebuf[24];
	sa.bb[2] = rebuf[23];
	sa.bb[3] = rebuf[22];
	saout	= sa.fa;
	gyr_16_1 = (uint16_t)(saout*16.4f*180/3.14f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[11] = gyr_16_1>>8;
		sConvertBuf[12] = gyr_16_1;
	}
	else if(pnum==2)
	{
		sConvertBuf[24] = gyr_16_1>>8;
		sConvertBuf[25] = gyr_16_1;
	}

	sa.bb[0] = rebuf[29];
	sa.bb[1] = rebuf[28];
	sa.bb[2] = rebuf[27];
	sa.bb[3] = rebuf[26];
	saout	= sa.fa;
	gyr_16_2 = (uint16_t)(saout*16.4f*180/3.14f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[13] = gyr_16_2>>8;
		sConvertBuf[14] = gyr_16_2;
	}
	else if(pnum==2)
	{
		sConvertBuf[26] = gyr_16_2>>8;
		sConvertBuf[27] = gyr_16_2;
	}
	
	sa.bb[0] = rebuf[33];
	sa.bb[1] = rebuf[32];
	sa.bb[2] = rebuf[31];
	sa.bb[3] = rebuf[30];
	saout	= sa.fa;
	gyr_16_3 = (uint16_t)(saout*16.4f*180/3.14f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[15] = gyr_16_3>>8;
		sConvertBuf[16] = gyr_16_3;
	}
	else if(pnum==2)
	{
		sConvertBuf[28] = gyr_16_3>>8;
		sConvertBuf[29] = gyr_16_3;
	}

	pnum = 0;
	
	if(sConvertBuf[17] == 0x02 && sConvertBuf[4] == 0x01)
	{
		sConvertBuf[30] = CheckSum(&sConvertBuf[2], 28);	//Buf lenth:31
		memcpy(convertBuf2Send,sConvertBuf,sizeof(sConvertBuf));
		memset(sConvertBuf,0,sizeof(sConvertBuf));
#if (IMUModeSelect == DoubleIMUMode)
		//if(convertBuf2Send[0]==0xfa && convertBuf2Send[1]==0xff && convertBuf2Send[2]==0xa0)
		if(convertBuf2Send[30] == CheckSum(&convertBuf2Send[2], 28))
		{
			HAL_UART_Transmit_DMA(&huart4,convertBuf2Send,SEND_IMU_2_LENTH);	//串口4发送数据，BLE透传用。
			send_cnt ++;
		}
		else
		{
			imuDataRecErro ++;
			memset(convertBuf2Send,0,sizeof(convertBuf2Send));
		}
#if WRITE_PRO_EN
		imuDataSaveTmp(convertBuf2Send,SEND_IMU_2_LENTH);
#endif
#endif
	}
}
//用于验证IMU数据
void imu_data_check_lzf(uint8_t *rebuf, uint8_t pnum, uint16_t lenth)
{

//acc
	sConvertBuf[0] = 0xFA;
	sConvertBuf[1] = 0xFF;
	sConvertBuf[2] = 0xA0;
	sConvertBuf[3] = 0x1A;

	sa.bb[0] = rebuf[10];
	sa.bb[1] = rebuf[9];
	sa.bb[2] = rebuf[8];
	sa.bb[3] = rebuf[7];
	saout	= sa.fa;
	acc_16_1 = (uint16_t)(saout*2048.0f/9.81f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[4] = 0x01;
		sConvertBuf[5] = acc_16_1>>8;
		sConvertBuf[6] = acc_16_1;
	}
	else if(pnum==2)
	{
		sConvertBuf[17] = 0x02;
		sConvertBuf[18] = acc_16_1>>8;
		sConvertBuf[19] = acc_16_1;
	}
	
	sa.bb[0] = rebuf[14];
	sa.bb[1] = rebuf[13];
	sa.bb[2] = rebuf[12];
	sa.bb[3] = rebuf[11];
	saout	= sa.fa;
	acc_16_2 = (uint16_t)(saout*2048.0f/9.81f + 32768) ;
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[7] = rebuf[7];
		sConvertBuf[8] = rebuf[8];
	}
	else if(pnum==2)
	{
		sConvertBuf[20] = rebuf[7];
		sConvertBuf[21] = rebuf[8];
	}
	
	sa.bb[0] = rebuf[18];
	sa.bb[1] = rebuf[17];
	sa.bb[2] = rebuf[16];
	sa.bb[3] = rebuf[15];
	saout	= sa.fa;
	acc_16_3 = (uint16_t)(saout*2048.0f/9.81f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[9] = rebuf[9];
		sConvertBuf[10] = rebuf[10];
	}
	else if(pnum==2)
	{
		sConvertBuf[22] = rebuf[9];
		sConvertBuf[23] = rebuf[10];
	}
	
//gyr
	sa.bb[0] = rebuf[25];
	sa.bb[1] = rebuf[24];
	sa.bb[2] = rebuf[23];
	sa.bb[3] = rebuf[22];
	saout	= sa.fa;
	gyr_16_1 = (uint16_t)(saout*16.4f*180/3.14f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[11] = gyr_16_1>>8;
		sConvertBuf[12] = gyr_16_1;
	}
	else if(pnum==2)
	{
		sConvertBuf[24] = gyr_16_1>>8;
		sConvertBuf[25] = gyr_16_1;
	}

	sa.bb[0] = rebuf[29];
	sa.bb[1] = rebuf[28];
	sa.bb[2] = rebuf[27];
	sa.bb[3] = rebuf[26];
	saout	= sa.fa;
	gyr_16_2 = (uint16_t)(saout*16.4f*180/3.14f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[13] = rebuf[22];
		sConvertBuf[14] = rebuf[23];
	}
	else if(pnum==2)
	{
		sConvertBuf[26] = rebuf[22];
		sConvertBuf[27] = rebuf[23];
	}
	
	sa.bb[0] = rebuf[33];
	sa.bb[1] = rebuf[32];
	sa.bb[2] = rebuf[31];
	sa.bb[3] = rebuf[30];
	saout	= sa.fa;
	gyr_16_3 = (uint16_t)(saout*16.4f*180/3.14f + 32768);
	memset(&sa.bb[0],0,4);
	if(pnum==1)
	{
		sConvertBuf[15] = rebuf[24];
		sConvertBuf[16] = rebuf[25];
	}
	else if(pnum==2)
	{
		sConvertBuf[28] = rebuf[24];
		sConvertBuf[29] = rebuf[25];
	}

	if(sConvertBuf[17] == 0x02 && sConvertBuf[4] == 0x01)
	{
		sConvertBuf[30] = CheckSum(&sConvertBuf[2], 28);	//Buf lenth:31
		memcpy(convertBuf2Send,sConvertBuf,sizeof(sConvertBuf));
		memset(sConvertBuf,0,sizeof(sConvertBuf));
#if (IMUModeSelect == DoubleIMUMode)
		HAL_UART_Transmit_DMA(&huart4,convertBuf2Send,SEND_IMU_2_LENTH);	//串口4发送数据，BLE透传用。
#endif
	}
}

void imu_startMesure(void)
{
#if (HARDWARE_V == HV1_0)

	//open imu data ouput
	uart3_sendStr(GotoMesure,5);
	uart2_sendStr(GotoMesure,5);
#elif (HARDWARE_V >= HV1_1)
	//open imu data ouput
	uart1_sendStr(GotoMesure,5);
	uart3_sendStr(GotoMesure,5);

#endif
	sysTime_sync_it = sysTime_ms;
//	osDelay(5);
}

void imu_stopMesure(void)
{
#if (HARDWARE_V == HV1_0)
	//close imu data ouput
	uart3_sendStr(GotoConfig,5);
	uart2_sendStr(GotoConfig,5);
	osDelay(10);
	uart3_sendStr(GotoConfig,5);
	uart2_sendStr(GotoConfig,5);
	osDelay(10);
	
#elif (HARDWARE_V >= HV1_1)
	//close imu data ouput
	uart1_sendStr(GotoConfig,5);
	uart3_sendStr(GotoConfig,5);
//	osDelay(10);
//	uart1_sendStr(GotoConfig,5);
//	uart3_sendStr(GotoConfig,5);
//	osDelay(10);

#endif
}

void imu_init(void)
{
	//close imu data ouput
//	osDelay(100);	//delay power on
	uart1_sendStr(GotoConfig,5);
//	uart2_sendStr(GotoConfig,5);
	uart3_sendStr(GotoConfig,5);
	
	imu_stopMesure();
#if 0
	osDelay(5);
	//open imu data ouput
	uart2_sendStr(GotoMesure,5);
	uart3_sendStr(GotoMesure,5);
	SYNCFnished = 1;
#endif

}

void imu_stopstartMesure(void)
{
	//close imu data ouput
	uart2_sendStr(GotoConfig,5);
	uart3_sendStr(GotoConfig,5);
	osDelay(1000);
	//open imu data ouput
	uart2_sendStr(GotoMesure,5);
	uart3_sendStr(GotoMesure,5);
}


void getSync(void)
{
	static uint8_t SYNCFLAG = 1;

	if((0 == SYNC_INPUT()) && 1 == SYNCFLAG)	//start imu
	{
		imu_startMesure();
		SYNCFnished = 1;
		SYNCFLAG = 0;
	}
	else if((0 == SYNC_INPUT()) && 2 == SYNCFLAG)	//stop imu
	{
		imu_stopMesure();
		SYNCFnished = 0;
		SYNCFLAG = 1;
	}
	
	if(!SYNCFLAG && (1 == SYNC_INPUT()))	//startted
	{
		SYNCFLAG = 2;
	}
	
}

void syncHandle(void)
{
	static uint16_t syncWait1S = 0;
	
	if(getSYNCFlag == StartIMU)
	{
		syncWait1S ++;
		SYNCFnished = 1;
		sysTimegetFlag = 1;
		getSYNCFlag = 0;
		ble_data.syncState = syncstart;
		
//		imu_startMesure();
	}
	else if(getSYNCFlag == StopIMU)
	{
		SYNCFnished = 0;
		getSYNCFlag = 0;
		imuDataRecErro = 0;
		ble_data.syncState = syncstop;

		SeqErro = 0;
		seqCnt = 0;
		memset(IMUSeqSave,0,sizeof(IMUSeqSave));
		
		imu_stopMesure();
	}
	
	if(SYNCFnished == 0)
	{
		memset(uart1_rxbuffer,0,sizeof(uart1_rxbuffer));
		memset(uart2_rxbuffer,0,sizeof(uart2_rxbuffer));
		memset(uart3_rxbuffer,0,sizeof(uart3_rxbuffer));
		
		memset(sConvertBuf,0,sizeof(sConvertBuf));
		memset(convertBuf2Send,0,sizeof(convertBuf2Send));
		uart1Com = 0;
		uart3Com = 0;
	}
	else
	{
		uart1Com ++;
		uart3Com ++;
	}

}

void StartIMUTask(void const * argument)
{
	uint32_t count=0;
	uint16_t timer=0;

	buf_init();
	imu_init();
	osDelay(100);
//	imu_startMesure();
//		uart3_sendtest();
//		osDelay(30);
//		uart2_sendtest();
//		getSync();

	systime_Init();
	
	for(;;)
	{

		if(count>5000)		//防止开机时中断引脚电平不稳定，导致误触发
		{
			syncHandle();
		}
		else
		{
			getSYNCFlag = 0;	
			SYNCFnished = 0;
			
			SeqErro = 0;
		}

		if(SYNCFnished)
		{

			if(tmpSendInfolen>0)
			{
				uint32_t TmpRet = xQueueSend(SdWriteProDataQueue, (void *)(&tmpSendInfolen), 0);
				if(TmpRet == 1)
				{
					sQueueCnt ++;
				}
				tmpSendInfolen = 0;
			}

		}
		
		IWDG_Feed();
		sysTime_ms ++;
		count ++;
		timer ++;

		if(timer/1000==1)
		{
			timer = 0;
			ble_update_systime();
			Calendar_Timer();
		}
		osDelay(1);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint16_t ff=0;

#if (HARDWARE_V == HV1_0)
	if(GPIO_Pin == GPIO_PIN_0)
#elif (HARDWARE_V >= HV1_1)
	if(GPIO_Pin == GPIO_PIN_13)
#endif
	{
		if(sysTime_ms>5000 && ble_isconnected())
		{
			ff ++;
			
			if(ff%2)	//ff=1,3,5,7,9...
			{
				getSYNCFlag = StartIMU;
				imu_startMesure();
			}
			else		//ff=2,4,6,8,10...
			{
				getSYNCFlag = StopIMU;
				imu_stopMesure();
			}

		}

	}

}
