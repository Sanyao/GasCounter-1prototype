// Counter memory management header file

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GCOUNTER_MEM_H
#define __GCOUNTER_MEM_H

#include "stm32l1xx_flash.h"
#include "main.h"

#define         EEPROM_BASE_ADDRES      0x08080000 // start eeprom
#define         EERPOM_PHONE_OFFSET     0x00
#define         EEPROM_PULSE_OFFSET     0x64 // смещение для расположения резервных значений счетчика импульсов /алармов еще каких то значений. 50 байт
#define         EEPROM_LOG_OFFSET       0x96 // начало лога

#define EEPROM_LOG_BUFFER_SIZE 512 // размер буфера лога - 512 байт

/* формат лога в еепроме:
хз... придкмать надо

*/



// functions return 0 if error (OFF) and 1 if OK (ON)
// use 'phone' with special symbol - example ----"\"+79853520273\",145"---- 
uint8_t CNT_MEM_WritePhoneToEEPROM(unsigned char* phone); // store phone in eeprom
uint8_t CNT_MEM_SetPhoneFromEEPROM(unsigned char* phone); // read phone from EEPROM and set to use
uint8_t CNT_MEM_WritePulsesToEEPROM (); // store backup pulses in eeprom
uint8_t CNT_MEM_ReadPulsesFromEEPROM (); // retrieve backup pulses from eeprom





#endif /* __GCOUNTER_MEM_H*/
