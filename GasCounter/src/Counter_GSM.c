#include "Counter_GSM.h"
#include <stdlib.h>     /* itoa */
#include <string.h> // strcat, strstr

unsigned char* gsmimei = "123456789012345";
unsigned char smstext[150]; //  = "reserve space for create sms text procedure" ;
unsigned char hs1[100]; //  "reserve space for create sms text procedure" ;
unsigned char hs2[100]; //"reserve space for create sms text procedure" ;   
        
AT_Answer AnswerStringResult = AT_NULL;



// GSM module power On
uint16_t CNT_GSM_Module_ON(void)
{
  // check last voltages
  uint32_t waitingtime;
  // PWR On Line define defines
  // SIM_PORT,  NRST_PIN,  PWRKEY_PIN
  
  CNT_GPIO_GSM_Lines(ON);  // activate all gsm module lines
  if (GPIO_ReadInputDataBit(SIM_STATUS_PORT, GSM_STATUS_PIN) == 1) return ON; // ��� �������
  
  /* Enable Booster for GSM power and wait power good*/
  CNT_PWR_BOOSTER(ON);
  
  /* PWRkey impulse for GSM Start*/
    CNT_DummyDelay(DYMMYPWRKEYWIDTH); // delay x10 nops
  //GPIO_HIGH(LED_PORT,LED1_PIN); // LED for info
  GPIO_HIGH(SIM_PORT,PWRKEY_PIN);
  CNT_DummyDelay(DYMMYPWRKEYWIDTH); // delay x10 nops
  GPIO_LOW(SIM_PORT,PWRKEY_PIN);
  
    //GPIO_LOW(LED_PORT,LED1_PIN); // LED for info
  /* Check status pin from GSM */
    waitingtime = DYMMYPWRKEYWIDTH*4; // max 4 sec to On
    
    while (     (GPIO_ReadInputDataBit(SIM_STATUS_PORT, GSM_STATUS_PIN) != 1 ) // ���� ��������� ���� STATUS ��� ������ - ���� �� �������.
            &&  ( waitingtime-- > 1) ) 
    { /* wait*/ }
      
    if (GPIO_ReadInputDataBit(SIM_STATUS_PORT, GSM_STATUS_PIN) == 1) // ���� ��� �� - ���������� ��� ��� ������ ������
    { 
      return ON;
    }
    else  // � ���� �� ��������� �������� - �������� ������ , ������ ��������� ��� �������, � ������ OFF
    {
      GPIO_HIGH(LED_PORT,LED1_PIN); // LED for info
      CNT_PWR_BOOSTER(OFF);
      return OFF;
    }
}

// GSM module power OFF
void CNT_GSM_Module_OFF(void)
{
   //CNT_GPIO_GSM_Lines(ON);  // activate all gsm module lines
   /* Disable Booster for GSM power*/
   CNT_PWR_BOOSTER(OFF);
   CNT_GPIO_GSM_Lines(OFF);  // de-activate all gsm module lines
   //GPIO_LOW(LED_PORT,LED2_PIN); // LED for info
  
}


// ������� ����������� ���
uint16_t CNT_GSM_SendDefaultSMS(void) // send sms with default params
{   
    //int32_t waitingtime; // ��� ��������
    //unsigned char* phone = DEFPHONENUMBER;

    uint16_t result = OFF;
   
        CNT_GSM_CreateSMSText(SMSVAR_DEF);
        result = CNT_GSM_Put2SMS( /*(unsigned char*)currentphonenumber,*/ (unsigned char*)smstext); 
       
    return result; 
                  
}
     
// �������  ��� �� ����������
uint16_t  CNT_GSM_SendClockSMS(void)
{
    uint16_t result = OFF;

        CNT_GSM_CreateSMSText(SMSVAR_CLOCK); // ������ ���������� smstext
        result = CNT_GSM_Put2SMS( /*(unsigned char*)currentphonenumber,*/ (unsigned char*)smstext); 
       
    return result; 
  
  
}



uint16_t CNT_GSM_PutSMS(unsigned char* phone, unsigned char* smstext)  // ������ ��� � ������ � ��������� return on - off
{
        uint16_t result = OFF;  // ��������� ��������
        uint32_t waitingtime; 
        uint16_t smsID = 0; // ID ����� ���, ������������� ��� ��������. ����������� � ������ +CMGS
        AT_Answer AnswerStringResult = AT_NULL;
        uint16_t registered = OFF;
        //uint16_t i;
        
        
        if (CNT_GSM_OnAndRegister() != ON) return result;  // �������� ������ � ��������� � ����. ����� ���� ��� �����������
        
        
          put_atcmd("AT+CSQ");   // Signal Quality?
          CNT_DummyDelay(10000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
        
          put_atcmd("AT+GSN");    // IMEI request
          CNT_DummyDelay(10000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
     
          //put_atcmd("AT+CCLK?");    // current clock request - �� �������� - �������� ����� �������� �����.
          //CNT_DummyDelay(300000); // delay x10 nops - debug
          //get_string();  // get clock
        
          put_atcmd("AT+CMTE?");    // current temp request
          CNT_DummyDelay(100000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
                        
        
          put_atcmd("AT+CMGD=1,4");  // delete !!!ALL!!! SMS from sim
          CNT_DummyDelay(100000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
        
          put_atcmd("AT+CMGF=1"); // select SMS format - text
          CNT_DummyDelay(100000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
        
          /*put_atcmd("AT+CSMP=?");
          CNT_DummyDelay(300000); // delay x10 nops - debug
          get_string();  // get ok
          */      
          
          put_atcmd("AT+CSMP=49,169,0,0");  // sms delivery request config
          CNT_DummyDelay(100000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
 
          /*put_atcmd("AT+CNMI=2,1,0,1,0"); // configere new message actions 
          CNT_DummyDelay(300000); // delay x10 nops - debug
          get_string();  // get ok
          CNT_DummyDelay(30); // delay x10 nops - debug
          */
        
          /* put_atcmd("ATD*100#");    // current balance request - ��� �������� �� �������� ����������� #100#
                                    // �� ��������� ���������� - ��. #100# ���������� ������ ���������
          CNT_DummyDelay(1000000); // delay x10 nops - debug
          get_string();  // get balance
          CNT_DummyDelay(10000000); // delay x10 nops - debug
          get_string();  // get balance
          */ 
          
          put_atcmd_noCRLF("AT+CMGS=");
          put_atcmd_noCRLF(phone);
          put_char(CR);
          
          GPIO_HIGH(LED_PORT,LED1_PIN); // ����� ���� ����������� - ������ LED1*****************
          waitingtime = 100; // ��� �������� �������� ���������� ��� ������ � �������
          while ( (get_char() != '>')   &&(waitingtime-- > 1 )  ) {;}// wait for welcome
          // ����� ������� ������� �������� - ����� ����� �� ����. 
          
          // *****************************************************************************************************************************************
          if (waitingtime ==0 ) { // ��������� ������ ���������� ����������� ������� - �������� - �������
                                  CMT_Misc_LEDIndication(2, 5, 1, 5);
                                  GPIO_LOW(LED_PORT,LED1_PIN); // ������� ������            
                                  return result; // ���� �� ��������� �� ��� ������ �������� ������ - ��������� OFF - ���������  �������� ������
                                 } 
          //********************************************************************************************************************************************
          
          GPIO_LOW(LED_PORT,LED1_PIN); // ���� ����������� ������ - ������� ���������. ����������� �������� ������ - ������ ��������� �������� �� �������********
         
          put_atcmd_noCRLF(smstext); // �������� ����� ��� � ������
          put_char(CTRLZ); // ���������� ���������� ������ ���
          put_char(CR); // �������� ���
          
          GPIO_HIGH(LED_PORT,LED2_PIN); // ��������� ��� - ������ ������ ���������************
          CNT_DummyDelay(100000); // delay x10 nops - debug
          
          waitingtime = 15; // ��� �������� �������� ��� � ����
          while ( waitingtime-- > 1 ) // ���� �������������� ������ ����� �������� ��� � ���� ��� ������
            {   AnswerStringResult = CNT_GSM_GetAnswer();
                if (AnswerStringResult==AT_ERROR) break;
                if (AnswerStringResult==AT_OK   ) // ������������� ����� - ������ ���� ����� ��� ("+CMGS: �����OK")
                  { //������� ����� ���
                    smsID = CNT_GSM_GetDigitFromATIN(SMSIDCMGS); 
                    break;
                  }
            }
          
          waitingtime = 30; // wait delivery report
          while ((waitingtime-- >0) && (result==OFF) && (Volt_INSTR>VOLTIONISTROFF) ) // ���� �� �������� �� ������� � ������� ���������� 
          {     CNT_GetVoltages();
                //CNT_DummyDelay(2000); // delay x10 nops - debug
                if (CNT_GSM_GetAnswer() == AT_CDS)   // ���� � ���������, ���� �� ��������� � �������� check +CDS receiving - sms delivery report
                   if (smsID ==  CNT_GSM_GetDigitFromATIN(SMSIDCDS) )  // ��� ���������� �������  �������� ����� ���, ������ �� CMGS
                        { CNT_TIME_SetTimeFromSMSDeliveryReport(); // ���� �������� ����, ������������� ����� �� ��������� � �������� 
                                    //!!!!!!!! �������� ����� �� ���� � ����������. ������ �� ����� CDS ��� ����� �������, ���� ���������!!!!!!! 
                          GPIO_LOW(LED_PORT,LED2_PIN); // �� - ������ ������� ������ ���������
                          result = ON;   // sms sended and delivery
                        }
                
                 // � ��� ������� �� ���������, ����� �� ��� ��� ������, �� ���������� �� �������� �� ���������� �������.
                
          }  // ��� ����� ����� �������� ������ � ��������
               
         
  // ���� ������ ���� �� ��������� ������ �� �������� - 
  // ��������� ���� ������ *****************************************************************************************************************************
                // �������:  ���������� ������ - ��������-��������-�������
                if ( waitingtime <= 0) {       
                                          CMT_Misc_LEDIndication(1, 1, 1, 5);
                                          GPIO_LOW(LED_PORT,LED2_PIN); // ������� ������  
                                       }
                // ������ ����������:  ���������� ������ - �������-��������-������� 
                if ( Volt_INSTR<VOLTIONISTROFF) {       
                                          CMT_Misc_LEDIndication(1, 5, 1, 5);
                                          GPIO_LOW(LED_PORT,LED2_PIN); // ������� ������  
                                                }
  // *************************************************************************************************************************************                              
                                  
         
        
        /*                                                    */
        /* ����� ����� �������� ����� �������� ��� � �������� */
        /*                                                    */ 
        
          // CNT_UART_state (OFF);  // ��������� ���� ������� � ��� �������.. - ������� � ���������� �������
        
 // ����� �������� 
                            
        GPIO_LOW(LED_PORT,LED2_PIN); //  ������� ������ ���������

        if (Volt_INSTR<VOLTIONISTROFF) CNT_PWR_BOOSTER(OFF); // ���� ����� ���������� - ����� ��������� ������ 
        return result;
}

// ������� ����� ��� � ������������ � ��� ����� variant
void CNT_GSM_CreateSMSText(uint16_t variant)
{
  unsigned char  space[] = " "; 
  unsigned char  comma[] = ", ";
  unsigned char  twopo[] = ":";
  unsigned char  slash[] = "/";  
  unsigned char alcnt[] = "Alr: ";
  unsigned char chstat[] = "Sts: ";
  unsigned char vlts[] = "VLIGS=";
  unsigned char tmps[] = "T=";
  unsigned char head[] = "CNT Online. Pls= "; // ������������� ���
  unsigned char headalarm[] = "CNT ALARM! Pls="; // ��� �� ������
  unsigned char headclock[] = "CNT Clock. Pls= "; // ��� �� ����������
  unsigned char openshort[] = "Ch open/short = ";
  
        smstext[0] = '\0';
        SMS_Counter++; // ��������� �������� �����
        
        CNT_Get_RTC_Time();
        CNT_Get_RTC_Date();
       
        itoa(day, hs1); concat(smstext, hs1); concat(smstext, slash); 
        itoa(month, hs1); concat(smstext, hs1); concat(smstext, slash); 
        itoa(year, hs1); concat(smstext, hs1); concat(smstext, space); 
        itoa(hours, hs1); concat(smstext, hs1); concat(smstext, twopo); 
        itoa(minutes, hs1); concat(smstext, hs1); concat(smstext, twopo);
        itoa(seconds, hs1); concat(smstext, hs1); concat(smstext, comma);
        itoa(SMS_Counter, hs1); concat(smstext, hs1); concat(smstext, comma); // ������� ���
        
   switch (variant)
   {
    case SMSVAR_DEF:
     {  concat(smstext, head);
        break;
     }
   case SMSVAR_ALARM:
    {
        concat(smstext, headalarm);
        break;
    }
   case SMSVAR_CLOCK:
    {
        concat(smstext, headclock);
        break;
    }
   }
                // ���������� ���������
        itoa(Pulse1Overall, hs1); concat(smstext, hs1); concat(smstext, slash);
        itoa(Pulse2Overall, hs1); concat(smstext, hs1); concat(smstext, slash);
        itoa(Pulse3Overall, hs1); concat(smstext, hs1); concat(smstext, comma);

        concat(smstext, alcnt); // ���������� �������
        itoa(Alarms1Count, hs1); concat(smstext, hs1); concat(smstext, slash);
        itoa(Alarms2Count, hs1); concat(smstext, hs1); concat(smstext, comma);
        
        concat(smstext, chstat); // ���� � ��������� �������
        concat(smstext, CNT_GPIO_GetChannelsStatus()); concat(smstext, comma);
        
        
        concat(smstext, vlts); // ���� � �����������
        itoa(Volt_VCC,   hs1); concat(smstext, hs1); concat(smstext, slash);
        itoa(Volt_LIBAT, hs1); concat(smstext, hs1); concat(smstext, slash);
        itoa(Volt_INSTR, hs1); concat(smstext, hs1); concat(smstext, slash);
        itoa(Volt_GSMMO, hs1); concat(smstext, hs1); concat(smstext, slash);
        itoa(Volt_SOBAT, hs1); concat(smstext, hs1); concat(smstext, comma);
  
        concat(smstext, openshort);  
        itoa(channel_opened, hs1); concat(smstext, hs1); concat(smstext, slash);
        itoa(channel_shorted, hs1); concat(smstext, hs1); concat(smstext, comma);
        
        concat(smstext, tmps);
        itoa(TemperatureInternal, hs1); concat(smstext, hs1);

}

// ��������� ��� �������. ���������� ���� �� ����� ������������� � ������
uint16_t CNT_GSM_SendAlarmSMS   (void)  // immediatelly charge battery and send sms
{
    //int32_t waitingtime; // ��� ��������
    //unsigned char* phone = DEFPHONENUMBER;
    //uint16_t result = OFF;
   
 #ifdef ___LBPRESENT // ������� ���� ���� ��
    while (Volt_INSTR < VOLTIONSTRMIN) {CNT_PWR_ChgIonstr();} // ���� ������� ��������� ����, charge ionistor from LiBat  - ������� ������ ��� �����
 #endif //___LBPRESENT // ����� �������� ���� ���� ��  
   
    CNT_GSM_CreateSMSText(SMSVAR_ALARM);
    return CNT_GSM_Put2SMS((unsigned char*)smstext); 
    //return result;  
}


// ������� ������� ������� �� SIM900R �� �����
AT_Answer CNT_GSM_GetAnswer()    // �������� ������ �� ����� � ������� error Ok � �.�.
  {
    get_string(); // ���� ������ ������ �� ��� ������.
    // ������ ��������, ���� �� � ��� ����� ERROR
    if (strstr ( (char*)at_in, "OK")    > 0 )   return AT_OK;           // �������� �����
    if (strstr ( (char*)at_in, "ERROR") > 0 )   return AT_ERROR;        // ���������� ������
    if (strstr ( (char*)at_in, "+CDS:") > 0 )   return AT_CDS;          // ����� � ��������
    
    if (at_in[0] == 0x00 ) return AT_NOANSWER; // ??? ���� ��� ������. �� ������ ��� ����� ��� �������� ���������. ����� ��������� 
    return AT_UNDEFINED; // ��� ������ ��� � ������ ��� �� Error �� Ok
  }


uint32_t CNT_GSM_GetDigitFromATIN (uint16_t position) // ��������� ��������� �������� �� ������ ������ ������ ���, ������� � �������� �������
  {
    return (atoi((char const*)&at_in[position]));
  }




uint16_t CNT_GSM_Put2SMS(unsigned char* smstext)  // ������ ��� ��� - �� ����� ������� � ������ � ��������� return on - off
{
        uint16_t result = OFF;  // ��������� ��������
        uint32_t waitingtime; 
        uint16_t smsID1 = 0; // ID ����� ��� ����� 1, ������������� ��� ��������. ����������� � ������ +CMGS
        uint16_t smsID2 = 0; // ID ����� ��� ����� 2, ������������� ��� ��������. ����������� � ������ +CMGS
        uint16_t smsID = 0; // ID ����� ��� ����� �������� �� ������ +CMGS � ���������� � ������ � ������
        unsigned char phonenumber1[]= DEFPHONENUMBER1; 
        unsigned char phonenumber2[]= DEFPHONENUMBER2;
        
        CNT_MEM_Set2PhoneFromEEPROM((unsigned char*)&phonenumber1, (unsigned char*)&phonenumber2); // �������� ������ EEPROM ��� �������

        if (CNT_GSM_OnAndRegister() != ON) return result;  // �������� ������ � ��������� � ����. ����� ���� ��� �����������
         
          put_atcmd("AT+CSQ");   // Signal Quality?
          CNT_DummyDelay(10000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
        
          put_atcmd("AT+GSN");    // IMEI request
          CNT_DummyDelay(10000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
     
          //put_atcmd("AT+CCLK?");    // current clock request - �� �������� - �������� ����� �������� �����.
          //CNT_DummyDelay(300000); // delay x10 nops - debug
          //get_string();  // get clock
        
          put_atcmd("AT+CMTE?");    // current temp request
          CNT_DummyDelay(100000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
                        
        
          put_atcmd("AT+CMGD=1,4");  // delete !!!ALL!!! SMS from sim
          CNT_DummyDelay(100000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
 
          
          smsID1 = CNT_GSM_TransmitSMSToOPSOS(smstext, phonenumber1); // �������� ������ ���
          smsID2 = CNT_GSM_TransmitSMSToOPSOS(smstext, phonenumber2); // � �����
          

// ������ ���� ������ � �������� - ������ �� ����. ���������� ������ ������ � �� ���� �������������� �����.          
          waitingtime = 30; // wait delivery report
          while ((waitingtime-- >0) && (result==OFF) && (Volt_INSTR>VOLTIONISTROFF) ) // ���� �� �������� �� ������� � ������� ���������� 
          {  CNT_GetVoltages();
             //CNT_DummyDelay(2000); // delay x10 nops - debug
             if (CNT_GSM_GetAnswer() == AT_CDS)   // ���� � ���������, ���� �� ��������� � �������� check +CDS receiving - sms delivery report
             {  smsID = CNT_GSM_GetDigitFromATIN(SMSIDCDS); 
                if ((smsID1 ==  smsID) || (smsID2 ==  smsID))  // ��� ���������� �������  �������� ����� ���, ������ �� CMGS
                { CNT_TIME_SetTimeFromSMSDeliveryReport(); // ���� �������� ����, ������������� ����� �� ��������� � �������� 
                   //!!!!!!!! �������� ����� �� ���� � ����������. ������ �� ����� CDS ��� ����� �������, ���� ���������!!!!!!! 
                  GPIO_LOW(LED_PORT,LED2_PIN); // �� - ������ ������� ������ ���������
                  result = ON;   // sms sended and delivery
                }
              // � ��� ������� �� ���������, ����� �� ��� ��� ������, �� ���������� �� �������� �� ���������� �������.
             }
          }  // ��� ����� ����� �������� ������ � ��������
               
         
  // ���� ������ ���� �� ��������� ������ �� �������� - 
  // ��������� ���� ������ *****************************************************************************************************************************
                // �������:  ���������� ������ - ��������-��������-�������
                if ( waitingtime <= 0) {       
                                          CMT_Misc_LEDIndication(1, 1, 1, 5);
                                          GPIO_LOW(LED_PORT,LED2_PIN); // ������� ������  
                                       }
                // ������ ����������:  ���������� ������ - �������-��������-������� 
                if ( Volt_INSTR<VOLTIONISTROFF) {       
                                          CMT_Misc_LEDIndication(1, 5, 1, 5);
                                          GPIO_LOW(LED_PORT,LED2_PIN); // ������� ������  
                                                }
  // *************************************************************************************************************************************                              
                                  
    
         
       
        
        /*                                                    */
        /* ����� ����� �������� ����� �������� ��� � �������� */
        /*                                                    */ 
        
          // CNT_UART_state (OFF);  // ��������� ���� ������� � ��� �������.. - ������� � ���������� �������
        
  // ����� �������� 
                            
        GPIO_LOW(LED_PORT,LED2_PIN); //  ������� ������ ���������

        if (Volt_INSTR<VOLTIONISTROFF) CNT_PWR_BOOSTER(OFF); // ���� ����� ���������� - ����� ��������� ������ 
        return result;
}



uint16_t CNT_GSM_OnAndRegister(void)  // �������� ������ � ��������� � ����
{       
   uint16_t registered = OFF;
   uint32_t waitingtime;        

   if (CNT_GSM_Module_ON() != ON) return registered;  // ���� �� ���������, ����� ������
        
   put_atcmd("AT");
   CNT_DummyDelay(20000); // delay x10 nops - debug
   AnswerStringResult = CNT_GSM_GetAnswer();
        
   put_atcmd("ATE0");
   CNT_DummyDelay(20000); // delay x10 nops - debug
   AnswerStringResult = CNT_GSM_GetAnswer();
                
   put_atcmd("AT+CBC");  // read supply voltage Voltgsm
   CNT_GetVoltages();
   CNT_DummyDelay(50000); // delay x10 nops - debug
   AnswerStringResult = CNT_GSM_GetAnswer();
   CNT_MGMT_GetRAIN(); // get adc correction
   CNT_GetVoltages(); 
   CNT_GPIO_CheckChannel();   // check channel circuit
         
   waitingtime = 350; // ������� ����������� � ���� ��� �� � ������
   registered = OFF;
   while ( (registered == OFF) && (waitingtime-- > 0) ) 
              {
                  put_atcmd("AT+CREG?");  // registered in network? !!! ����� �������� � �������� ����������� � ����
                  CNT_DummyDelay(50000); // delay x10 nops - debug
                  AnswerStringResult = CNT_GSM_GetAnswer();
                  if (AnswerStringResult == AT_OK) // ���� ������ ��������
                    if (strstr ( (char*)at_in, "+CREG: 0,1") > 0) registered = ON; // �������� ������� �����������
                  if ((AnswerStringResult == AT_ERROR) || (AnswerStringResult == AT_NOANSWER)) 
                    break; // ����� ��������� ���� ��������  
              }

return registered; // ������ ��������� �����������
}




uint16_t CNT_GSM_TransmitSMSToOPSOS(unsigned char* smstext, unsigned char* phone)
{// �������� ���������� �������� ��� � ������          
        uint32_t waitingtime; 
        uint16_t smsID = 0; // ID ����� ���, ������������� ��� ��������. ����������� � ������ +CMGS
        AT_Answer AnswerStringResult = AT_NULL;
        
          put_atcmd("AT+CMGF=1"); // select SMS format - text
          CNT_DummyDelay(100000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
        
          put_atcmd("AT+CSMP=49,169,0,0");  // sms delivery request config
          CNT_DummyDelay(100000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
 
          put_atcmd_noCRLF("AT+CMGS=");                 // ������ �������
          put_atcmd_noCRLF(phone);               // � ����� ��������
          put_char(CR);                                 // ����������
          
          GPIO_HIGH(LED_PORT,LED1_PIN); // ����� ���� ����������� - ������ LED1*****************
          waitingtime = 100; // ��� �������� �������� ���������� ��� ������ � �������
          while ( (get_char() != '>')   &&(waitingtime-- > 1 )  ) {;}// wait for welcome
          // ����� ������� ������� �������� - ����� ����� �� ����. 
          
          // *****************************************************************************************************************************************
          if (waitingtime ==0 ) { // ��������� ������ ���������� ����������� ������� - �������� - �������
                                  CMT_Misc_LEDIndication(2, 5, 1, 5);
                                  GPIO_LOW(LED_PORT,LED1_PIN); // ������� ������            
                                  return smsID; // ���� �� ��������� �� ��� ������ �������� ������ - ��������� ���� - ���������  �������� ������
                                 } 
          //********************************************************************************************************************************************
          
          GPIO_LOW(LED_PORT,LED1_PIN); // ���� ����������� ������ - ������� ���������. ����������� �������� ������ - ������ ��������� �������� �� �������********
         
          put_atcmd_noCRLF(smstext); // �������� ����� ��� � ������
          put_char(CTRLZ); // ���������� ���������� ������ ���
          put_char(CR); // �������� ���
          
          GPIO_HIGH(LED_PORT,LED2_PIN); // ��������� ��� - ������ ������ ���������************
          CNT_DummyDelay(100000); // delay x10 nops - debug
          
          waitingtime = 15; // ������� ��� �������� �������� ��� � ����
          while ( waitingtime-- > 1 ) // ���� �������������� ������ ����� �������� ��� � ���� ��� ������
            {   AnswerStringResult = CNT_GSM_GetAnswer();
                if (AnswerStringResult==AT_ERROR) break;
                if (AnswerStringResult==AT_OK   ) // ������������� ����� - ������ ���� ����� ��� ("+CMGS: �����OK")
                  { //������� ����� ���
                    smsID = CNT_GSM_GetDigitFromATIN(SMSIDCMGS); // ����� ������� ID ������� ����� 
                    break;
                  }
            }
return smsID; // ������ ID ������������ �����
}
