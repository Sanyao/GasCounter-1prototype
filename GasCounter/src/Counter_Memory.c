// counter MEMORY management

#include "Counter_Memory.h"



// ����� � ������ ��� �������� ����
#define EEPROM_LOG_BUFFER_SIZE 512 // ������ ������
volatile uint8_t    eeprom_buffer[EEPROM_LOG_BUFFER_SIZE];
volatile uint16_t   eeprom_wr_index=0, //������ ������ ������ (���� ������ ������)
                    eeprom_rd_index=0, //������ ������ ������ (������ ������ ������)
                    eeprom_counter=0; //���������� ������ � ������ 
volatile uint8_t    eeprom_buffer_overflow=0; //���������� � ������������ ������






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





// �������� ������� � ������ - ��� �������
void CNT_Misc_AddStringToEEPROMLog (unsigned char * logstring) 
{
    uint16_t i,l;
    
    
    l= strlen(logstring); // ������ ����� ������, ������� ���������� �������� � ������
    
    
    
    
    
}







