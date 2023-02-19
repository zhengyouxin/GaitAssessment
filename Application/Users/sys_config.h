#ifndef _SYS_CONFIG_H
#define _SYS_CONFIG_H

//SD卡功能开关  1：打开 0：关闭 
#define ENABLE_SD 	1 
#define WRITE_LOG_EN		1
#define WRITE_PRO_EN		1

#define HV1_0 	1	//2020.9.30
#define HV1_1 	2	//2020.12.18版本
#define HV1_2 	3
#define HV1_3 	4

typedef enum
{
	SV100=100,
	SV101,
	SV102,
	SV103,
	SV104,
	SV105,
	SV110=110,		//
	SV111,
	SV112,
	SV113,
	SV114,
	SV115,
	SV120=120,		//
	SV121,
	SV122,
	SV123,
	SV124,
	SV125,
	SV130=130,		//
	SV131,
	SV132,
	SV133,
	SV134,
	SV135,
}SoftVersion;

#define HARDWARE_V	HV1_1	//和V1_2兼容
#define SOFTWARE_V	SV122

#define HARDWARE_VERSION 	"V1.2.0"
#define SOFTWARE_VERSION	"V1.2.2"//2021.07.16

#define NOMALMODE 	1
#define TESTMODE	2
#define SYSTEMMODE	NOMALMODE

#define SingleIMUMode	1
#define DoubleIMUMode	2
#define TripleIMUMode	3

#define IMUModeSelect	DoubleIMUMode

//定义一些常用的数据类型短关键字
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;
typedef const int16_t sc16;
typedef const int8_t sc8;

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;
typedef __I int16_t vsc16;
typedef __I int8_t vsc8;

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;
typedef const uint16_t uc16;
typedef const uint8_t uc8;

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;
typedef __I uint16_t vuc16;
typedef __I uint8_t vuc8;


#endif

