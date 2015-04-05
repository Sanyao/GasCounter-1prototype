#include "main.h"

uint16_t needTX = OFF; // on or off need tx flag - regular TX
uint16_t needTX_Alarm = OFF; // alarm tx needed flag

unsigned char *s;
volatile unsigned char* sms = "default SMS. maxlength = 64 symbols. rezerve area for this mess.";
/* define global variables */
uint32_t Pulse1Overall;
uint32_t Pulse2Overall;
uint32_t Pulse3Overall;
uint32_t Pulse4Overall;
uint32_t Pulse1Current;
uint32_t Pulse2Current;
uint32_t Pulse3Current;
uint32_t Pulse4Current;

uint32_t PulsesOverall[NUMOFPULSECHANNELS];
uint32_t AlarmsOverall[NUMOFALARMCHANNELS];

uint32_t Alarms1Count =0;
uint32_t Alarms2Count =0;

uint32_t SMS_Counter = 0; // счетчик смс - считаем отправленные

// global voltages variables
uint16_t Volt_LIBAT;
uint16_t Volt_SOBAT;
uint16_t Volt_INSTR;
uint16_t Volt_GSMMO;
uint16_t Volt_REFIN;
uint16_t Volt_TSENS;
int16_t  TemperatureInternal;
uint16_t Volt_VCC;

uint8_t hours, minutes, seconds;  // from main
uint8_t weekday, day, month, year;

float koeffRAIN = 1.64; // учет входного сопротивдения АЦП - костыль для измерения напряжения 
                      // примерно столько. будем потом корректировать по показаниям напряжения питания sim900 и Volt_GSMMO


uint16_t channel_shorted = 0; // закороченные каналы
uint16_t channel_opened = 0; // обрыв каналов

unsigned char currentphonenumber[]= DEFPHONENUMBER2 ;
unsigned char defphonenumber[]= DEFPHONENUMBER;
uint16_t waitingcommandSMStime = DEFWAITINGCOMMANDSMSTIME;



int main()
{
  int i, j;
  
  __disable_irq();
  SYSCFG_DeInit();                      // reset congiguration
  CNT_Init();                           // first setup for counter systems
  NVIC_EnableIRQ(RTC_WKUP_IRQn);        // enable rtc wkup interrupt
  NVIC_EnableIRQ(USART1_IRQn);
  __enable_irq();                       // global enable interrupts
     needTX=ON; 
     needTX_Alarm = ON;  // test alarm sms
                             // отморгнем начало
                                  CMT_Misc_LEDIndication(2, 5, 0, 0);
                                  GPIO_LOW(LED_PORT,LED1_PIN); // погасим первый            
                                  
                              
  
  DBGMCU_Config(DBGMCU_STOP, ENABLE);  
    DBGMCU_Config(DBGMCU_SLEEP, ENABLE);  
       
  //CNT_GetVoltages(); // debug

 //CNT_GPIO_CheckChannel();   // check channel circuit
 /*while (1)
  { 
    CNT_DummyDelayLP_TO(500);  //for sleep unchange ^) // first set  
        GPIO_TOGGLE(LED_PORT,LED1_PIN);
  }*/
  
  i= CNT_MEM_WritePhoneToEEPROM((unsigned char*)&defphonenumber); // store phone in eeprom
  i= CNT_MEM_SetPhoneFromEEPROM((unsigned char*)&currentphonenumber); // проверка работы EEPROM для отладки
 
 
  //while (1); // stop for debug
  
  
  while (1)
  {
   //CNT_GPIO_RetFromStop();  // сюда по идее возвращаемся после просыпания из STOP Mode

    if ( (needTX_Alarm==ON) || (needTX==ON) ) // заходим сюда только если что то есть на передачу - стоит какой то флаг
    {   CNT_GetVoltages(); // получим напряжения для анализа
      
        if (needTX_Alarm==ON)  // alarm info TX - immediatelly TX with ionistor charge if needed
          { while ( !(CNT_GSM_SendAlarmSMS() == ON) ) {;} // попытка передавать аларм до достижения результата
            needTX_Alarm=OFF ; // сброс флага аларма
          }
          
        if (needTX==ON)  //если есть задача на передачу регулярного сообщения
        { if (Volt_INSTR > VOLTIONSTRMIN) // если было достаточно напряжения на ионисторе при последнем измерении (в процедуре передачи смс)
          {  if (CNT_GSM_Module_ON()== ON) // и если успешно включили жсм модуль, или он ранее был включен
              { CNT_GSM_SendDefaultSMS();   // передаем регулярную смс
                needTX=OFF;                 // и сбрасываем флаг
              }  // Закончена передача регуляра. Флаг сброшен независимо от результата передачи... 
                  // наверное надо доработать, чтоб флаг сбрасывался только после успеха
          }
        }  
        CNT_GSM_Module_OFF(); // теперь можно выключить модуль жсм
    }
 
    CNT_GPIO_PrepToStop();   // Подготовимся ко сну
    //GPIO_TOGGLE(LED_PORT,LED2_PIN); // LED for info
    PWR_UltraLowPowerCmd(ENABLE);     // разрешим переход в сон
    PWR_ClearFlag(PWR_FLAG_WU);       // сбросим флаг вейкапа
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI); // и заснем (STOP)... главное чтоб не навсегда... ))

 }  // конец основного цикла while 
    //return 0; майн никуда ничего не возвращает, т.к. не заканчивается
}


void RTC_WKUP_IRQHandler(void) // wakeup intr hanhler
{ // здесь прерывание от RTC, периодическое (час по умолчанию в отладке было). 
   CNT_RTC_ClearWakeupFlags();  // сбросим флаги вейкапа 
   //GPIO_HIGH(LED_PORT,LED2_PIN);
   //CNT_GetVoltages(); // измерим напряжения
   /*  Сюда потом накидать регулярных действий типа:
      по результатам измерения напрягов наверное какие то действия бы предпринять, например если садится литий?
      здесь также может быть спасти в еепром текущие значения счетчиков в качестве резервных
      проверить целостность линии если давно не проверяли.
      сделать депассивацию батареи если давно стоит
      
   
   */
   needTX = ON ; // установм флаг необходимости регулярной передачи (пока без альтернатив)
         
   //GPIO_LOW(LED_PORT,LED2_PIN); // здесь моргал СИД для отладки
   EXTI_ClearFlag(EXTI_Line20); // обнулим флаг преывания

} // end interrupt


void EXTI0_IRQHandler(void) // Pulse Count handler - Ch1 - PC0
{
  //GPIO_HIGH(LED_PORT,LED1_PIN);
  NVIC_DisableIRQ(EXTI0_IRQn);
  Pulse1Overall +=1;                    // считаем импульсы по первому каналу
  CNT_DummyDelay(DUMMYPULSECOUNTDELAY);
  EXTI_ClearITPendingBit(EXTI_Line0);
  EXTI_ClearFlag(EXTI_Line0);  
  NVIC_EnableIRQ(EXTI0_IRQn);
  GPIO_LOW(LED_PORT,LED1_PIN);
}

void EXTI1_IRQHandler(void) // Pulse Count handler CH2 - PC1
{
  //GPIO_HIGH(LED_PORT,LED1_PIN);
  NVIC_DisableIRQ(EXTI1_IRQn);
  Pulse2Overall +=1;            // считаем импульсы по 2 каналу
  CNT_DummyDelay(DUMMYPULSECOUNTDELAY);
  EXTI_ClearITPendingBit(EXTI_Line1);
  EXTI_ClearFlag(EXTI_Line1);
  NVIC_EnableIRQ(EXTI1_IRQn);
  GPIO_LOW(LED_PORT,LED1_PIN);
}

void EXTI2_IRQHandler(void) // Pulse Count handler CH3 - PC2
{
  //GPIO_HIGH(LED_PORT,LED1_PIN);
  NVIC_DisableIRQ(EXTI2_IRQn);
  Pulse3Overall +=1;// считаем импульсы по 3 каналу
  CNT_DummyDelay(DUMMYPULSECOUNTDELAY);
  EXTI_ClearITPendingBit(EXTI_Line2);
  EXTI_ClearFlag(EXTI_Line2);
  NVIC_EnableIRQ(EXTI2_IRQn);
  GPIO_LOW(LED_PORT,LED1_PIN);
}


void EXTI4_IRQHandler(void) // Alarm handler CH1 - PC4
{
  uint32_t i;
  NVIC_DisableIRQ(EXTI4_IRQn);
  for (i=0; i<10; i++) { // :)
                        GPIO_TOGGLE(LED_PORT,LED1_PIN);
                        CNT_DummyDelay(DUMMYALARMCOUNTDELAY);
                        CNT_DummyDelay(DUMMYALARMCOUNTDELAY);
                        }
  Alarms1Count +=1;                     // считаем алармы по первому каналу
  needTX_Alarm = ON;
  EXTI_ClearITPendingBit(EXTI_Line4);
  EXTI_ClearFlag(EXTI_Line4);
  NVIC_EnableIRQ(EXTI4_IRQn);
  //GPIO_LOW(LED_PORT,LED1_PIN);
}


void EXTI9_5_IRQHandler(void) // Alarm handler CH2 - PC5
{
  uint32_t i;
  NVIC_DisableIRQ(EXTI9_5_IRQn);
  for (i=0; i<10; i++) { // :)
                        GPIO_TOGGLE(LED_PORT,LED1_PIN);
                        CNT_DummyDelay(DUMMYALARMCOUNTDELAY);
                        CNT_DummyDelay(DUMMYALARMCOUNTDELAY);
                        }
  Alarms2Count +=1;             // считаем алармы по второму каналу
  needTX_Alarm = ON;
  EXTI_ClearITPendingBit(EXTI_Line5);
  EXTI_ClearFlag(EXTI_Line5);
  NVIC_EnableIRQ(EXTI9_5_IRQn);
  //GPIO_LOW(LED_PORT,LED1_PIN);
}

void ADC1_IRQHandler(void) // ADC1_IRQHandler - EOC
{
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}



void TIM10_IRQHandler (void) // timer 9 handler - timeout detection
{ // пока в стадии отладки. не используется нигде. Планировал  для отсчтеа времен при общении с жсм модулем.
  //TIM_Cmd(TIM9, DISABLE);    // Выключение таймера
    NVIC_DisableIRQ(TIM10_IRQn);
    TIM_ClearFlag(TIM10, TIM_FLAG_Update); // сбросим флаг срабатывания таймера
    GPIO_TOGGLE(LED_PORT,LED1_PIN);
    TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
    TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);
    NVIC_EnableIRQ(TIM10_IRQn);
  //TIM_Cmd(TIM9, DISABLE);    // Выключение таймера
  //TIM_Cmd(TIM9, ENABLE);    // Выключение таймера
}




