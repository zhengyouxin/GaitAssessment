#ifndef _FATFS_H
#define _FATFS_H

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h" /* defines SD_Driver as external */

/* USER CODE BEGIN Includes */

	 
/* USER CODE END Includes */

	 
typedef struct 
{
	 uint8_t year ; 
	 uint8_t month ;
	 uint8_t date ;		
 	 uint8_t week ;
	 uint8_t hour ;		
 	 uint8_t minute ;	
     uint8_t second;	
	 uint8_t status;
}TIME_VALUE;	 
	 
	 
extern uint8_t retSD; /* Return value for SD */
extern char SDPath[4]; /* SD logical drive path */
extern FATFS SDFatFS; /* File system object for SD logical drive */
extern FIL SDFile; /* File object for SD */


	 
	 
void MX_FATFS_Init(void);


#endif

