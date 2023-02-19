/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : systime.c
  * @brief          : systime app
  * @author			: ZYX
  * @time			: 2021.01.14
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 SZCHWS.
  * All rights reserved.
  ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "systime.h"

const unsigned char NonLeapMonth[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};	  //每月的天数
const unsigned char LeapMonth[13]={0,31,29,31,30,31,30,31,31,30,31,30,31};	  //每月的天数

volatile systime_t systime;

/************************************************************************************************* 
* 函数功能	: 闰年判断        
*
* 参数			: year 年份                                             
*                               
* 返回			: 闰年返回1；反之返回0；                                       
*        			                                   
* 备注			:                                                
*                                                                        
**************************************************************************************************/ 
static unsigned char IsLeap(unsigned short year)
{
	if(((year%100==0)&&(year%400==0))||
	   ((year%100!=0) && (year%4==0))) 
		return 1;
	else
		return 0;
}

static int ReturnWeekDay( unsigned int year, unsigned int month, unsigned int day )  
{
    int week = 0;  
    unsigned int y = 0, c = 0, m = 0, d = 0;  
  
    if ( month == 1 || month == 2 )  
    {  
        c = ( year - 1 ) / 100;  
        y = ( year - 1 ) % 100;  
        m = month + 12;  
        d = day;  
    }  
    else  
    {  
        c = year / 100;  
        y = year % 100;  
        m = month;  
        d = day;  
    }  
      
    week = y + y / 4 + c / 4 - 2 * c + 26 * ( m + 1 ) / 10 + d - 1;    //蔡勒公式  
    week = week >= 0 ? ( week % 7 ) : ( week % 7 + 7 );    //iWeek为负时取模  
    if ( week == 0 )    //星期日不作为一周的第一天  
    {  
        week = 7;  
    }  
  
    return week;  
}

void systime_Init(void)
{
	systime.year = 21;
	systime.month = 3;
	systime.day = 1;
	systime.hour = 9;
	systime.minute = 30;
	systime.second = 0;
	
	systime.week = ReturnWeekDay(systime.year,systime.month,systime.day);
}

void systime_update(uint32_t timedata)
{
	if(timedata==0)
	{
		return;
	}
	else
	{
		systime.year = (uint8_t)(timedata>>26&0x3F);
		systime.month = (uint8_t)(timedata>>22&0x0F);
		systime.day = (uint8_t)(timedata>>17&0x1F);
		systime.hour = (uint8_t)(timedata>>12&0x1F);
		systime.minute = (uint8_t)(timedata>>6&0x3F);
		systime.second = (uint8_t)(timedata&0x3F);
		
		systime.week = ReturnWeekDay(systime.year,systime.month,systime.day);

	}
}

void Calendar_Timer(void)
{
	systime.week = ReturnWeekDay(systime.year,systime.month,systime.day);
	
    systime.second++;
	
    if (!(systime.second == 0x3C))
    {
        return;
    }
    systime.second = 0;
    systime.minute++;
    if (!(systime.minute == 0x3C))
    {
        return;
    }
    systime.minute=0;
    systime.hour++;
    if (!(systime.hour == 0x18))
    {
        return;
    }
    systime.hour = 0;
    systime.day++;

    if (IsLeap(systime.year))
    {
        if (systime.day <= LeapMonth[systime.month])
        {
            return;
        }
    }
    else
    {
        if (systime.day <= NonLeapMonth[systime.month])
        {
            return;
        }
    }
    systime.day=1;
    systime.month++;
    if (!(systime.month == 0x0D))
    {
        return;
    }

    systime.month=1;
    systime.year++;
}



