// counter MEMORY management

#include "Counter_Memory.h"



// Буфер в еепром для хранения лога
#define EEPROM_LOG_BUFFER_SIZE 512 // размер буфера
volatile uint8_t    eeprom_buffer[EEPROM_LOG_BUFFER_SIZE];
volatile uint16_t   eeprom_wr_index=0, //индекс хвоста буфера (куда писать данные)
                    eeprom_rd_index=0, //индекс начала буфера (откуда читать данные)
                    eeprom_counter=0; //количество данных в буфере 
volatile uint8_t    eeprom_buffer_overflow=0; //информация о переполнении буфера






uint8_t CNT_MEM_WritePhoneToEEPROM (unsigned char* phone) // store phone in eeprom
{
  uint32_t eepoint; // addres
  FLASH_Status result; 
  eepoint = EEPROM_BASE_ADDRES + EERPOM_PHONE_OFFSET;
  DATA_EEPROM_Unlock();
  DATA_EEPROM_FixedTimeProgramCmd(ENABLE);
  while (*phone != 0)
    { 
      //DATA_EEPROM_EraseByte(eepoint);
      result = DATA_EEPROM_ProgramByte(eepoint++, *phone++);
    }
  DATA_EEPROM_ProgramByte(eepoint++, 0xFF);
  DATA_EEPROM_Lock();
  return result;
}




uint8_t CNT_MEM_SetPhoneFromEEPROM (unsigned char* phone) // read phone from EEPROM and set to use
{
  uint32_t eepoint; // addres

  uint8_t symbol;
  eepoint = EEPROM_BASE_ADDRES + EERPOM_PHONE_OFFSET;
  symbol  = *(__IO uint8_t *)eepoint++;
  while (symbol != 0xFF)
    { *phone++  = symbol;
      symbol  = *(__IO uint8_t *)eepoint++; 
    }
  
  return 1;
}





// добавить строчку в еепром - лог отладки
void CNT_Misc_AddStringToEEPROMLog (unsigned char * logstring) 
{
    uint16_t i,l;
    
    
    l= strlen(logstring); // найдем длину строки, которую собираемся добавить в еепром
    
    
    
    
    
}







