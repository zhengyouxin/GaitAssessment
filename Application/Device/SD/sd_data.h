#ifndef _SD_DATA_H
#define _SD_DATA_H
#include "std.h"

#define SD_SAVE_DATA_LENTH 		31*15	//10ms*15 150msдһ�ι�������
#define IMU_SAVEDAT_LENTH_TMP 	31*15*4	//һ֡����31*5*3

#define STARTSAVE 	1
#define STOPSAVE	0

bool imuDataStartSaveSDCard(void);
void SdcardCreatProFile_Zyx(void);
bool SD_WriteProFile(void);


#endif

