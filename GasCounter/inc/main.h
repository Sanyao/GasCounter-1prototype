/* main header file
  includes and common use parameters in this
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GCOUNTER_MAIN_H
#define __GCOUNTER_MAIN_H


#include "stm32l1xx.h"
#include "stm32l1xx_conf.h" //modified - commented unused includes
#include "Counter_GPIO_define.h"
#include "Counter_Init.h"
#include "Counter_GPIO_utils.h"
#include "Counter_PwrMgmt.h"
#include "Counter_Measurement.h"
#include "stm32l1xx_syscfg.h"
#include "Counter_Misc.h"
#include "Counter_GSM.h"
#include "Counter_UART.h"
#include "stm32l1xx_dbgmcu.h"
#include "Counter_Memory.h"



/* Конфигурация дефайнов для ПО системы
   будет отвечать за разные варианты прошивки
   придумать бы какой регистр для вариантов конфигурации??? 
*/

//Параметры наличия литиевой батареи. Может быть один из двух
// #define ___LBPRESENT // ЛБ в наличии
#define ___LBNOTPRESENT // ЛБ нет в наличии






#define DUMMYPULSECOUNTDELAY 50000
#define DUMMYALARMCOUNTDELAY 65000


/* define border params */
#define VOLTIONSTRMIN   2350 // minimal voltage on ionistor to success GSM session  - задираем планку до 2,35 вольта при работе без ЛБ
#define VOLTIONSTRGIST  100  // Гистерезис для зарядки 
#define VOLTIONSTRMAX   2470 // напряжение на ионисторе когда можно комфортно жить и нужно следить за напругой для предотвращения перезаряда. Может где-то использовать для ожидания команд
#define VOLTIONSTROV    2530 // караул-перенапряжение!!! Сделать расход лепестричества по перенапрягу

#define VOLTIONISTROFF 2000 // предполагаем минимальное значение на ионисторе при котором еще возможна работа. ниже- отключаем и сброс.
/* т.к. питание предполагается от ионистора без батарейки, предлагаю минимальное напряжение поднять хотяб до 2 вольт, чтобы обеспечить работу проца*/


#define NUMOFPULSECHANNELS 4 // количество каналов импульсов
#define NUMOFALARMCHANNELS 8 // количество каналов аларма

extern unsigned char at_out[MAX_AT_SIZE];
extern unsigned char at_in[MAX_AT_SIZE];






/* define main Procedures */

//void EXTI20_IRQHandler(void); // wakeup intr hanhler
void RTC_WKUP_IRQHandler(void); // wakeup intr hanhler

void RTC_Alarm_IRQHandler(void); // alarm handler

void EXTI0_IRQHandler(void); // Pulse Count handler - Ch1 - PC0
void EXTI1_IRQHandler(void); // Pulse Count handler CH2 - PC1
void EXTI2_IRQHandler(void); // Pulse Count handler CH3 - PC2

void EXTI4_IRQHandler(void); // Alarm handler CH1 - PC4
void EXTI5_IRQHandler(void); // Alarm handler CH2 - PC5



#endif /* __GCOUNTER_MAIN_H */