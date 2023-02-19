#ifndef _UART_H
#define _UART_H

#define IMU_RX_BUFLEN 	40//35

#define UART1_RX_BUFLEN 40//35
#define UART3_RX_BUFLEN 40//35

#define UART4_RX_BUFLEN 	6	//接收蓝牙模块数据
#define UART4_RX_BUFLEN_MAX 10	//接收蓝牙模块数据

#define UART_RX_LENMAX	100
#define UART2_RX_BUFLEN 35


#define sBTPercent	0xA1
#define sBTState	0xA2
#define sBTVersion	0xA7

#define rSYNCStart	0xA3
#define rSYNCStop	0xA4
#define rBLEHr		0xA5
#define rBLETime	0xA6


void uart_init(void);
void uart1_sendtest(void);
void uart2_sendtest(void);
void uart3_sendtest(void);
void uart4_sendtest(void);

void uart1_sendStr(uint8_t *sbuf,uint16_t len);
void uart2_sendStr(uint8_t *sbuf,uint16_t len);
void uart3_sendStr(uint8_t *sbuf,uint16_t len);
void uart4_sendStr(uint8_t *sbuf,uint16_t len);


#endif
