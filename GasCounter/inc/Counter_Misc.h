/* initialization procedure file header */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GCOUNTER_MISC_H
#define __GCOUNTER_MISC_H

#include "Counter_GPIO_define.h"

//#define CLOCKFREQUENCY  1000000


/* define STATES for modes description*/
#define OFF             0
#define ON              1
#define SLEEPING        2

#define MAX_AT_SIZE      (uint16_t)500
// #define EEPROM_LOG_BUFFER_SIZE  (uint16_t) 500





/* Includes ------------------------------------------------------------------*/



//void Delay(uint32_t nTime);

void CNT_DummyDelay(uint32_t tensnops); // delay x10 nops
uint8_t CNT_DummyDelayLP_TO(uint32_t ms) ; // delay with timeout
void itoa(int32_t n, unsigned char* s); // int tostring conversion
void reverse(unsigned char* s);
void concat(unsigned char* dest, unsigned char* src); // string concatenate
void CMT_Misc_LEDIndication(uint8_t LEDNo, uint8_t first, uint8_t second, uint8_t third); // ��� ������� �����������



#endif /* __GCOUNTER_MISC_H*/