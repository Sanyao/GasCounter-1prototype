// counter MEMORY management

#include "Counter_Memory.h"
#include <string.h> 

// ����� � ������ ��� �������� ����

volatile uint8_t    eeprom_buffer[EEPROM_LOG_BUFFER_SIZE];
volatile uint16_t   eeprom_wr_index=0, //������ ������ ������ (���� ������ ������)
                    eeprom_rd_index=0, //������ ������ ������ (������ ������ ������)
                    eeprom_counter=0; //���������� ������ � ������ 
volatile uint8_t    eeprom_buffer_overflow=0; //���������� � ������������ ������

extern uint32_t PulsesOverall[NUMOFPULSECHANNELS];
extern uint32_t AlarmsOverall[NUMOFALARMCHANNELS];


uint8_t CNT_MEM_WritePhoneToEEPROM (unsigned char* phone) // store phone in eeprom - old wersion-only one phone
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


uint8_t CNT_MEM_Write2PhoneToEEPROM (unsigned char* phone1, unsigned char* phone2) // store two phones in eeprom
{
  uint32_t eepoint; // addres
  FLASH_Status result; 
  eepoint = EEPROM_BASE_ADDRES + EERPOM_PHONE1_OFFSET; // ������� � ������ ������ �����
  DATA_EEPROM_Unlock();
  DATA_EEPROM_FixedTimeProgramCmd(ENABLE);
  while (*phone1 != 0)
    { 
      //DATA_EEPROM_EraseByte(eepoint);
      result = DATA_EEPROM_ProgramByte(eepoint++, *phone1++);
    }
  DATA_EEPROM_ProgramByte(eepoint++, 0xFF);
  
  eepoint = EEPROM_BASE_ADDRES + EERPOM_PHONE2_OFFSET; // ������� � ������ ������ �����
  DATA_EEPROM_Unlock();
  DATA_EEPROM_FixedTimeProgramCmd(ENABLE);
  while (*phone2 != 0)
    { 
      //DATA_EEPROM_EraseByte(eepoint);
      result = DATA_EEPROM_ProgramByte(eepoint++, *phone2++);
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

uint8_t CNT_MEM_Set2PhoneFromEEPROM (unsigned char* phone1, unsigned char* phone2) // read TWO phoneS from EEPROM and set to use
{
  uint32_t eepoint; // addres
  uint8_t symbol;
  eepoint = EEPROM_BASE_ADDRES + EERPOM_PHONE1_OFFSET;
  symbol  = *(__IO uint8_t *)eepoint++;
  while (symbol != 0xFF)
    { *phone1++  = symbol;
      symbol  = *(__IO uint8_t *)eepoint++; 
    }
  eepoint = EEPROM_BASE_ADDRES + EERPOM_PHONE2_OFFSET;
  symbol  = *(__IO uint8_t *)eepoint++;
  while (symbol != 0xFF)
    { *phone2++  = symbol;
      symbol  = *(__IO uint8_t *)eepoint++; 
    }
  return 1;
}


uint8_t CNT_MEM_WritePulsesToEEPROM () // store pulses backup in eeprom
{
  uint32_t eepoint;
  uint8_t i;
  FLASH_Status result; 
  /* ���������� PulsesOverall[NUMOFPULSECHANNELS] � AlarmsOverall[NUMOFALARMCHANNELS] */
  eepoint = EEPROM_PULSE_OFFSET;
  DATA_EEPROM_Unlock();
  DATA_EEPROM_FixedTimeProgramCmd(ENABLE);
  i = NUMOFPULSECHANNELS;
  while (i-- > 0) 
    { result = DATA_EEPROM_ProgramWord(eepoint,PulsesOverall[i]); // ����� ��������
      eepoint += 4; // ���������� 4 � ������ �.�. ������ 32� ���������
    }
  
  i = NUMOFALARMCHANNELS;
  while (i-- > 0) 
    { result = DATA_EEPROM_ProgramWord(eepoint,AlarmsOverall[i]); // ����� ������
      eepoint += 4; // ���������� 4 � ������ �.�. ������ 32� ���������
    }  
  DATA_EEPROM_Lock();
  return result;
}


uint8_t CNT_MEM_ReadPulsesFromEEPROM () // retrieve backup pulses from eeprom
{
  
  
  
}






// �������� ������� � ������ - ��� �������
void CNT_Misc_AddStringToEEPROMLog (unsigned char * logstring) 
{
    uint16_t i,l;
    
    
    l= strlen(logstring); // ������ ����� ������, ������� ���������� �������� � ������
    
    
    
    
    
    
    
    
}







