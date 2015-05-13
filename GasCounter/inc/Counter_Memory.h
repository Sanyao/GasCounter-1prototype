// Counter memory management header file

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GCOUNTER_MEM_H
#define __GCOUNTER_MEM_H

#include "stm32l1xx_flash.h"
#include "main.h"

#define         EEPROM_BASE_ADDRES      0x08080000 // start eeprom
#define         EERPOM_PHONE_OFFSET     0x00 // смещение для хранения номера тедлефона если он один - для совместимости, тож самоечто и сторочка ниже
#define         EERPOM_PHONE1_OFFSET    0x00 // смещение для первого номера тела 
#define         EERPOM_PHONE2_OFFSET    0x18 // смещение для второго номера тела - на 24 символа (18h) - этого должно хватитьна запись одного номера в полном формате

#define         EEPROM_PULSE_OFFSET     0x64 // смещение для расположения резервных значений счетчика импульсов /алармов еще каких то значений. 50 байт
#define         EEPROM_LOG_OFFSET       0x96 // начало лога

#define EEPROM_LOG_BUFFER_SIZE 512 // размер буфера лога - 512 байт

/* формат лога в еепроме:
хз... придкмать надо

*/



// functions return 0 if error (OFF) and 1 if OK (ON)
// use 'phone' with special symbol - example ----"\"+79853520273\",145"---- 
uint8_t CNT_MEM_WritePhoneToEEPROM(unsigned char* phone); // store phone in eeprom
uint8_t CNT_MEM_Write2PhoneToEEPROM (unsigned char* phone1, unsigned char* phone2); // store two phones in eeprom

uint8_t CNT_MEM_SetPhoneFromEEPROM(unsigned char* phone); // read phone from EEPROM and set to use
uint8_t CNT_MEM_Set2PhoneFromEEPROM(unsigned char* phone1, unsigned char* phone2); // read two phones from EEPROM and set to use

uint8_t CNT_MEM_WritePulsesToEEPROM (); // store backup pulses in eeprom
uint8_t CNT_MEM_ReadPulsesFromEEPROM (); // retrieve backup pulses from eeprom





#endif /* __GCOUNTER_MEM_H*/
