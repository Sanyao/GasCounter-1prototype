 /* initialization procedure file header */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GCOUNTER_TIMEMGMT_H
#define __GCOUNTER_TIMEMGMT_H

#include "stm32l1xx_rtc.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_pwr.h"
#include "stm32l1xx_exti.h"
#include "Counter_Misc.h"
#include "main.h"
#include "Counter_UART.h"

// time default constant definitions
#define         CZEROHOUR       9
#define         CZEROMIN        59
#define         CZEROSEC        30
#define         CZERODAY        13
#define         CZEROMONTH      RTC_Month_May
#define         CZEROYEAR       15
#define         CZEROWEEKDAY    RTC_Weekday_Wednesday


#define         CZEROWKUP       3600  // периодичность пробуждения seconds

// будильник по умолчанию на 10 утра
#define         CZEROALARMHOUR  10 // тестируем дефолт
#define         CZEROALARMMIN   0
#define         CZEROALARMSEC   0



extern uint8_t hours, minutes, seconds;  // from main
extern uint8_t weekday, day, month, year;
extern unsigned char at_out[MAX_AT_SIZE], at_in[MAX_AT_SIZE];

void CNT_First_Set_RTC (void); 
ErrorStatus CNT_Set_RTC_Time(uint8_t hours, uint8_t minutes, uint8_t seconds);
ErrorStatus CNT_Set_RTC_Date(uint8_t weekday, uint8_t day, uint8_t month, uint8_t year);

ErrorStatus CNT_RTC_SetWakeUpPeriod(uint32_t WakeUpPeriodSeconds);
ErrorStatus CNT_RTC_SetAlarmATime(uint8_t hours, uint8_t minutes, uint8_t seconds);  // настройка просыпаться в заданное время ежедневно - будильник А

void CNT_RTC_ClearWakeupFlags(void); // clear flags after intr
void CNT_RTC_ClearAlarmAFlags(void); // сброс флагов прерывания по париодическому таймеру


void CNT_Get_RTC_Time(void); // to global vars
void CNT_Get_RTC_Date(void);


// делать
void CNT_TIME_SetTimeFromSMSDeliveryReport(void); // Set time from SMS Delivery Report



// не сделано пока
void CNT_Time_LPSLEEPms(uint32_t ms); // low power sleep сколько то миллисекунд - на таймере





#endif /* __GCOUNTER_TIMEMGMT_H*/