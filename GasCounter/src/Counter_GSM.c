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
  if (GPIO_ReadInputDataBit(SIM_STATUS_PORT, GSM_STATUS_PIN) == 1) return ON; // уже включен
  
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
    
    while (     (GPIO_ReadInputDataBit(SIM_STATUS_PORT, GSM_STATUS_PIN) != 1 ) // ждем установки пина STATUS жсм модуля - типа он включен.
            &&  ( waitingtime-- > 1) ) 
    { /* wait*/ }
      
    if (GPIO_ReadInputDataBit(SIM_STATUS_PORT, GSM_STATUS_PIN) == 1) // если все ок - возвращаем что жсм модуль вкючен
    { 
      return ON;
    }
    else  // а если не поучилось включить - выключим бустер , зажжем светодиод для отладки, и вернем OFF
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


// отсылка регулярного смс
uint16_t CNT_GSM_SendDefaultSMS(void) // send sms with default params
{   
    //int32_t waitingtime; // для таймаута
    //unsigned char* phone = DEFPHONENUMBER;

    uint16_t result = OFF;
   
        CNT_GSM_CreateSMSText(SMSVAR_DEF);
        result = CNT_GSM_Put2SMS( /*(unsigned char*)currentphonenumber,*/ (unsigned char*)smstext); 
       
    return result; 
                  
}
     
// отсылка  смс по расписанию
uint16_t  CNT_GSM_SendClockSMS(void)
{
    uint16_t result = OFF;

        CNT_GSM_CreateSMSText(SMSVAR_CLOCK); // делаем переменную smstext
        result = CNT_GSM_Put2SMS( /*(unsigned char*)currentphonenumber,*/ (unsigned char*)smstext); 
       
    return result; 
  
  
}



uint16_t CNT_GSM_PutSMS(unsigned char* phone, unsigned char* smstext)  // подать смс в буффер и отправить return on - off
{
        uint16_t result = OFF;  // результат отправки
        uint32_t waitingtime; 
        uint16_t smsID = 0; // ID номер смс, присваиваемый при отправке. передаается в ответе +CMGS
        AT_Answer AnswerStringResult = AT_NULL;
        uint16_t registered = OFF;
        //uint16_t i;
        
        
        if (CNT_GSM_OnAndRegister() != ON) return result;  // включить модуль и зарегится в сети. выйти если нет регистрации
        
        
          put_atcmd("AT+CSQ");   // Signal Quality?
          CNT_DummyDelay(10000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
        
          put_atcmd("AT+GSN");    // IMEI request
          CNT_DummyDelay(10000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
     
          //put_atcmd("AT+CCLK?");    // current clock request - не работает - оператор редко передает время.
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
        
          /* put_atcmd("ATD*100#");    // current balance request - для мегафона не работает стандартный #100#
                                    // на остальных операторах - ок. #100# возвращает баланс латиницей
          CNT_DummyDelay(1000000); // delay x10 nops - debug
          get_string();  // get balance
          CNT_DummyDelay(10000000); // delay x10 nops - debug
          get_string();  // get balance
          */ 
          
          put_atcmd_noCRLF("AT+CMGS=");
          put_atcmd_noCRLF(phone);
          put_char(CR);
          
          GPIO_HIGH(LED_PORT,LED1_PIN); // здесь ждем приглашения - зажжем LED1*****************
          waitingtime = 100; // для таймаута ожидания готовности жсм модуля к смсинью
          while ( (get_char() != '>')   &&(waitingtime-- > 1 )  ) {;}// wait for welcome
          // здесь сделаем таймаут поменьше - чтобы долго не ждал. 
          
          // *****************************************************************************************************************************************
          if (waitingtime ==0 ) { // отморгнем вторым отсутствие приглашения длинный - короткий - длинный
                                  CMT_Misc_LEDIndication(2, 5, 1, 5);
                                  GPIO_LOW(LED_PORT,LED1_PIN); // погасим первый            
                                  return result; // если не дождались от жсм модуля внятного ответа - возвратим OFF - светодиод  остается гореть
                                 } 
          //********************************************************************************************************************************************
          
          GPIO_LOW(LED_PORT,LED1_PIN); // если приглашение пришло - погасим светодиод. Приглашение приходит быстро - иногда светодиод наверное не увидишь********
         
          put_atcmd_noCRLF(smstext); // отправим текст СМС в модуль
          put_char(CTRLZ); // спецсимвоы завершения текста смс
          put_char(CR); // отсылаем смс
          
          GPIO_HIGH(LED_PORT,LED2_PIN); // отправили смс - зажжем второй светодиод************
          CNT_DummyDelay(100000); // delay x10 nops - debug
          
          waitingtime = 15; // для передачи отправки смс в сеть
          while ( waitingtime-- > 1 ) // ждем положительного ответа после отправки смс в сеть или ошибки
            {   AnswerStringResult = CNT_GSM_GetAnswer();
                if (AnswerStringResult==AT_ERROR) break;
                if (AnswerStringResult==AT_OK   ) // положительный ответ - должен быть номер смс ("+CMGS: номерOK")
                  { //получим номер смс
                    smsID = CNT_GSM_GetDigitFromATIN(SMSIDCMGS); 
                    break;
                  }
            }
          
          waitingtime = 30; // wait delivery report
          while ((waitingtime-- >0) && (result==OFF) && (Volt_INSTR>VOLTIONISTROFF) ) // пока не передано не таймаут и хватает напряжения 
          {     CNT_GetVoltages();
                //CNT_DummyDelay(2000); // delay x10 nops - debug
                if (CNT_GSM_GetAnswer() == AT_CDS)   // ждем и проверяем, есть ли сообщение о доставке check +CDS receiving - sms delivery report
                   if (smsID ==  CNT_GSM_GetDigitFromATIN(SMSIDCDS) )  // для обновления времени  проверим номер смс, взятый из CMGS
                        { CNT_TIME_SetTimeFromSMSDeliveryReport(); // если доставка есть, скорректируем врямя из сообщения о доставке 
                                    //!!!!!!!! отправка может же быть и неуспешной. придет ли отгда CDS или будет таймаут, надо проверить!!!!!!! 
                          GPIO_LOW(LED_PORT,LED2_PIN); // ок - просто погасим второй светодиод
                          result = ON;   // sms sended and delivery
                        }
                
                 // а тут неплохо бы проверить, живой ли еще жсм модуль, не отключился ли например от недостатка питания.
                
          }  // тут конец цикла ожидания отчета о доставке
               
         
  // сюда пришли если не дождались отчета об отправке - 
  // отморгнем этот случай *****************************************************************************************************************************
                // таймаут:  пригорящем втором - короткий-короткий-длинный
                if ( waitingtime <= 0) {       
                                          CMT_Misc_LEDIndication(1, 1, 1, 5);
                                          GPIO_LOW(LED_PORT,LED2_PIN); // погасим второй  
                                       }
                // потеря напряжения:  пригорящем втором - длинный-короткий-длинный 
                if ( Volt_INSTR<VOLTIONISTROFF) {       
                                          CMT_Misc_LEDIndication(1, 5, 1, 5);
                                          GPIO_LOW(LED_PORT,LED2_PIN); // погасим второй  
                                                }
  // *************************************************************************************************************************************                              
                                  
         
        
        /*                                                    */
        /* здесь можно вставить потом ожидание смс с командой */
        /*                                                    */ 
        
          // CNT_UART_state (OFF);  // отключаем уарт общения с жсм модулем.. - перенес в выключение бустера
        
 // конец передачи 
                            
        GPIO_LOW(LED_PORT,LED2_PIN); //  погасим второй светодиод

        if (Volt_INSTR<VOLTIONISTROFF) CNT_PWR_BOOSTER(OFF); // если малое напряжение - здесь выключаем бустер 
        return result;
}

// создать текст смс в соответствии с его типом variant
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
  unsigned char head[] = "CNT Online. Pls= "; // ПЕРИОДИЧЕСКАЯ СМС
  unsigned char headalarm[] = "CNT ALARM! Pls="; // смс по аларму
  unsigned char headclock[] = "CNT Clock. Pls= "; // смс по расписанию
  unsigned char openshort[] = "Ch open/short = ";
  
        smstext[0] = '\0';
        SMS_Counter++; // инкремент счетчика смсок
        
        CNT_Get_RTC_Time();
        CNT_Get_RTC_Date();
       
        itoa(day, hs1); concat(smstext, hs1); concat(smstext, slash); 
        itoa(month, hs1); concat(smstext, hs1); concat(smstext, slash); 
        itoa(year, hs1); concat(smstext, hs1); concat(smstext, space); 
        itoa(hours, hs1); concat(smstext, hs1); concat(smstext, twopo); 
        itoa(minutes, hs1); concat(smstext, hs1); concat(smstext, twopo);
        itoa(seconds, hs1); concat(smstext, hs1); concat(smstext, comma);
        itoa(SMS_Counter, hs1); concat(smstext, hs1); concat(smstext, comma); // счетчик смс
        
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
                // количество импульсов
        itoa(Pulse1Overall, hs1); concat(smstext, hs1); concat(smstext, slash);
        itoa(Pulse2Overall, hs1); concat(smstext, hs1); concat(smstext, slash);
        itoa(Pulse3Overall, hs1); concat(smstext, hs1); concat(smstext, comma);

        concat(smstext, alcnt); // количество алармов
        itoa(Alarms1Count, hs1); concat(smstext, hs1); concat(smstext, slash);
        itoa(Alarms2Count, hs1); concat(smstext, hs1); concat(smstext, comma);
        
        concat(smstext, chstat); // инфа о состоянии каналов
        concat(smstext, CNT_GPIO_GetChannelsStatus()); concat(smstext, comma);
        
        
        concat(smstext, vlts); // инфа о напряжениях
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

// отправить смс тревоги. отправляем пока не будет подтверждения о приеме
uint16_t CNT_GSM_SendAlarmSMS   (void)  // immediatelly charge battery and send sms
{
    //int32_t waitingtime; // для таймаута
    //unsigned char* phone = DEFPHONENUMBER;
    //uint16_t result = OFF;
   
 #ifdef ___LBPRESENT // вариант если есть ЛБ
    while (Volt_INSTR < VOLTIONSTRMIN) {CNT_PWR_ChgIonstr();} // если напруги ионистора мало, charge ionistor from LiBat  - вариант только для лития
 #endif //___LBPRESENT // конец варианта если есть ЛБ  
   
    CNT_GSM_CreateSMSText(SMSVAR_ALARM);
    return CNT_GSM_Put2SMS((unsigned char*)smstext); 
    //return result;  
}


// простой парсинг ответов от SIM900R по типам
AT_Answer CNT_GSM_GetAnswer()    // ожидание ответа от модул и возврат error Ok и т.д.
  {
    get_string(); // ждем строку ответа от жсм модуля.
    // теперь проверим, есть ли в ней слово ERROR
    if (strstr ( (char*)at_in, "OK")    > 0 )   return AT_OK;           // успешный ответ
    if (strstr ( (char*)at_in, "ERROR") > 0 )   return AT_ERROR;        // обнаружена ошибка
    if (strstr ( (char*)at_in, "+CDS:") > 0 )   return AT_CDS;          // отчет о доставке
    
    if (at_in[0] == 0x00 ) return AT_NOANSWER; // ??? если нет ответа. не уверен что будет так работать нормально. Нужно проверить 
    return AT_UNDEFINED; // нет ответа или в ответе нет ни Error ни Ok
  }


uint32_t CNT_GSM_GetDigitFromATIN (uint16_t position) // получение цифрового значения из строки ответа модуля жсм, начиная с заданной позиции
  {
    return (atoi((char const*)&at_in[position]));
  }




uint16_t CNT_GSM_Put2SMS(unsigned char* smstext)  // подать две смс - по обоим номерам в буффер и отправить return on - off
{
        uint16_t result = OFF;  // результат отправки
        uint32_t waitingtime; 
        uint16_t smsID1 = 0; // ID номер смс номер 1, присваиваемый при отправке. передаается в ответе +CMGS
        uint16_t smsID2 = 0; // ID номер смс номер 2, присваиваемый при отправке. передаается в ответе +CMGS
        uint16_t smsID = 0; // ID номер смс будем получать из ответа +CMGS и сравнивать с первым и вторым
        unsigned char phonenumber1[]= DEFPHONENUMBER1; 
        unsigned char phonenumber2[]= DEFPHONENUMBER2;
        
        CNT_MEM_Set2PhoneFromEEPROM((unsigned char*)&phonenumber1, (unsigned char*)&phonenumber2); // проверка работы EEPROM для отладки

        if (CNT_GSM_OnAndRegister() != ON) return result;  // включить модуль и зарегится в сети. выйти если нет регистрации
         
          put_atcmd("AT+CSQ");   // Signal Quality?
          CNT_DummyDelay(10000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
        
          put_atcmd("AT+GSN");    // IMEI request
          CNT_DummyDelay(10000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
     
          //put_atcmd("AT+CCLK?");    // current clock request - не работает - оператор редко передает время.
          //CNT_DummyDelay(300000); // delay x10 nops - debug
          //get_string();  // get clock
        
          put_atcmd("AT+CMTE?");    // current temp request
          CNT_DummyDelay(100000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
                        
        
          put_atcmd("AT+CMGD=1,4");  // delete !!!ALL!!! SMS from sim
          CNT_DummyDelay(100000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
 
          
          smsID1 = CNT_GSM_TransmitSMSToOPSOS(smstext, phonenumber1); // полетела первая смс
          smsID2 = CNT_GSM_TransmitSMSToOPSOS(smstext, phonenumber2); // и дубль
          

// дальше ждем ответа о доставке - любого из двух. дожидаемся только одного и по нему синхронизируем время.          
          waitingtime = 30; // wait delivery report
          while ((waitingtime-- >0) && (result==OFF) && (Volt_INSTR>VOLTIONISTROFF) ) // пока не передано не таймаут и хватает напряжения 
          {  CNT_GetVoltages();
             //CNT_DummyDelay(2000); // delay x10 nops - debug
             if (CNT_GSM_GetAnswer() == AT_CDS)   // ждем и проверяем, есть ли сообщение о доставке check +CDS receiving - sms delivery report
             {  smsID = CNT_GSM_GetDigitFromATIN(SMSIDCDS); 
                if ((smsID1 ==  smsID) || (smsID2 ==  smsID))  // для обновления времени  проверим номер смс, взятый из CMGS
                { CNT_TIME_SetTimeFromSMSDeliveryReport(); // если доставка есть, скорректируем врямя из сообщения о доставке 
                   //!!!!!!!! отправка может же быть и неуспешной. придет ли отгда CDS или будет таймаут, надо проверить!!!!!!! 
                  GPIO_LOW(LED_PORT,LED2_PIN); // ок - просто погасим второй светодиод
                  result = ON;   // sms sended and delivery
                }
              // а тут неплохо бы проверить, живой ли еще жсм модуль, не отключился ли например от недостатка питания.
             }
          }  // тут конец цикла ожидания отчета о доставке
               
         
  // сюда пришли если не дождались отчета об отправке - 
  // отморгнем этот случай *****************************************************************************************************************************
                // таймаут:  пригорящем втором - короткий-короткий-длинный
                if ( waitingtime <= 0) {       
                                          CMT_Misc_LEDIndication(1, 1, 1, 5);
                                          GPIO_LOW(LED_PORT,LED2_PIN); // погасим второй  
                                       }
                // потеря напряжения:  пригорящем втором - длинный-короткий-длинный 
                if ( Volt_INSTR<VOLTIONISTROFF) {       
                                          CMT_Misc_LEDIndication(1, 5, 1, 5);
                                          GPIO_LOW(LED_PORT,LED2_PIN); // погасим второй  
                                                }
  // *************************************************************************************************************************************                              
                                  
    
         
       
        
        /*                                                    */
        /* здесь можно вставить потом ожидание смс с командой */
        /*                                                    */ 
        
          // CNT_UART_state (OFF);  // отключаем уарт общения с жсм модулем.. - перенес в выключение бустера
        
  // конец передачи 
                            
        GPIO_LOW(LED_PORT,LED2_PIN); //  погасим второй светодиод

        if (Volt_INSTR<VOLTIONISTROFF) CNT_PWR_BOOSTER(OFF); // если малое напряжение - здесь выключаем бустер 
        return result;
}



uint16_t CNT_GSM_OnAndRegister(void)  // включить модуль и зарегится в сети
{       
   uint16_t registered = OFF;
   uint32_t waitingtime;        

   if (CNT_GSM_Module_ON() != ON) return registered;  // если не включился, сразу выйдем
        
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
         
   waitingtime = 350; // ожидаем регистрацию в сети где то с минуту
   registered = OFF;
   while ( (registered == OFF) && (waitingtime-- > 0) ) 
              {
                  put_atcmd("AT+CREG?");  // registered in network? !!! нужна проверка и ожидание регистрации в сети
                  CNT_DummyDelay(50000); // delay x10 nops - debug
                  AnswerStringResult = CNT_GSM_GetAnswer();
                  if (AnswerStringResult == AT_OK) // если модуль отвечает
                    if (strstr ( (char*)at_in, "+CREG: 0,1") > 0) registered = ON; // проверим наличие регистрации
                  if ((AnswerStringResult == AT_ERROR) || (AnswerStringResult == AT_NOANSWER)) 
                    break; // иначе прекратим цикл ожидания  
              }

return registered; // вернем результат регистрации
}




uint16_t CNT_GSM_TransmitSMSToOPSOS(unsigned char* smstext, unsigned char* phone)
{// начинаем собственно отдавать смс к опсосу          
        uint32_t waitingtime; 
        uint16_t smsID = 0; // ID номер смс, присваиваемый при отправке. передаается в ответе +CMGS
        AT_Answer AnswerStringResult = AT_NULL;
        
          put_atcmd("AT+CMGF=1"); // select SMS format - text
          CNT_DummyDelay(100000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
        
          put_atcmd("AT+CSMP=49,169,0,0");  // sms delivery request config
          CNT_DummyDelay(100000); // delay x10 nops - debug
          AnswerStringResult = CNT_GSM_GetAnswer();
 
          put_atcmd_noCRLF("AT+CMGS=");                 // пошлем команду
          put_atcmd_noCRLF(phone);               // и номер телефона
          put_char(CR);                                 // подтвердим
          
          GPIO_HIGH(LED_PORT,LED1_PIN); // здесь ждем приглашения - зажжем LED1*****************
          waitingtime = 100; // для таймаута ожидания готовности жсм модуля к смсинью
          while ( (get_char() != '>')   &&(waitingtime-- > 1 )  ) {;}// wait for welcome
          // здесь сделаем таймаут поменьше - чтобы долго не ждал. 
          
          // *****************************************************************************************************************************************
          if (waitingtime ==0 ) { // отморгнем вторым отсутствие приглашения длинный - короткий - длинный
                                  CMT_Misc_LEDIndication(2, 5, 1, 5);
                                  GPIO_LOW(LED_PORT,LED1_PIN); // погасим первый            
                                  return smsID; // если не дождались от жсм модуля внятного ответа - возвратим ноль - светодиод  остается гореть
                                 } 
          //********************************************************************************************************************************************
          
          GPIO_LOW(LED_PORT,LED1_PIN); // если приглашение пришло - погасим светодиод. Приглашение приходит быстро - иногда светодиод наверное не увидишь********
         
          put_atcmd_noCRLF(smstext); // отправим текст СМС в модуль
          put_char(CTRLZ); // спецсимвоы завершения текста смс
          put_char(CR); // отсылаем смс
          
          GPIO_HIGH(LED_PORT,LED2_PIN); // отправили смс - зажжем второй светодиод************
          CNT_DummyDelay(100000); // delay x10 nops - debug
          
          waitingtime = 15; // ождание для передачи отправки смс в сеть
          while ( waitingtime-- > 1 ) // ждем положительного ответа после отправки смс в сеть или ошибки
            {   AnswerStringResult = CNT_GSM_GetAnswer();
                if (AnswerStringResult==AT_ERROR) break;
                if (AnswerStringResult==AT_OK   ) // положительный ответ - должен быть номер смс ("+CMGS: номерOK")
                  { //получим номер смс
                    smsID = CNT_GSM_GetDigitFromATIN(SMSIDCMGS); // здесь получим ID ушедшей смски 
                    break;
                  }
            }
return smsID; // вернем ID отправленной смски
}
