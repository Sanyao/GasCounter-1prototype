#include "main.h"

uint16_t needTX = ON; // on or off need tx flag - regular TX � ������ ���������
uint16_t needTX_Alarm = OFF; // alarm tx needed flag
// ���� �������� �� ����� - ����������. �� ��������� �������� � 9:50 ������� ��� 
// ����� ������� �������� �������� ��� ���������� �������� �� ������ - ����� ��� ��������.
// ���� ������� ������ �� 09:00:00 �� 09:55:00
uint16_t needTX_Clock = OFF;  


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

uint32_t SMS_Counter = 0; // ������� ��� - ������� ������������

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

float koeffRAIN = 1.64; // ���� �������� ������������� ��� - ������� ��� ��������� ���������� 
                      // �������� �������. ����� ����� �������������� �� ���������� ���������� ������� sim900 � Volt_GSMMO


uint16_t channel_shorted = 0; // ������������ ������
uint16_t channel_opened = 0; // ����� �������

unsigned char currentphonenumber[]= DEFPHONENUMBER;
unsigned char currentphonenumber1[]= DEFPHONENUMBER;
unsigned char currentphonenumber2[]= DEFPHONENUMBER;

unsigned char defphonenumber[]= DEFPHONENUMBER;
unsigned char defphonenumber1[]= DEFPHONENUMBER1;
unsigned char defphonenumber2[]= DEFPHONENUMBER2;

uint16_t waitingcommandSMStime = DEFWAITINGCOMMANDSMSTIME;



int main()
{
  int i, j;
  
  __disable_irq();
  SYSCFG_DeInit();                      // reset congiguration
  CNT_Init();                           // first setup for counter systems

  
  NVIC_EnableIRQ(USART1_IRQn);
  __enable_irq();                       // global enable interrupts
     
      needTX             = ON; 
      needTX_Alarm       = OFF;  // test alarm sms
      needTX_Clock       = OFF;
     
                             // ��������� ������
                                  CMT_Misc_LEDIndication(2, 5, 0, 0);
                                  GPIO_LOW(LED_PORT,LED1_PIN); // ������� ������            
                                  
                              
  
  DBGMCU_Config(DBGMCU_STOP, ENABLE);  
    DBGMCU_Config(DBGMCU_SLEEP, ENABLE);  
       
  //CNT_GetVoltages(); // debug

 //CNT_GPIO_CheckChannel();   // check channel circuit
 /*while (1)
  { 
    CNT_DummyDelayLP_TO(500);  //for sleep unchange ^) // first set  
        GPIO_TOGGLE(LED_PORT,LED1_PIN);
  }*/
  
  i= CNT_MEM_Write2PhoneToEEPROM((unsigned char*)&defphonenumber1, (unsigned char*)&defphonenumber2); // store phone in eeprom
  i= CNT_MEM_Set2PhoneFromEEPROM((unsigned char*)&currentphonenumber1, (unsigned char*)&currentphonenumber2); // �������� ������ EEPROM ��� �������
 
 
  //while (1); // stop for debug
  
  
  while (1)
  {
   //CNT_GPIO_RetFromStop();  // ���� �� ���� ������������ ����� ���������� �� STOP Mode

    if ( (needTX_Alarm==ON) || (needTX==ON) || (needTX_Clock==ON) ) // ������� ���� ������ ���� ��� �� ���� �� �������� - ����� ����� �� �� ������
    {   CNT_GetVoltages(); // ������� ���������� ��� �������
      
        // �������� �������
#ifdef ___LBPRESENT // ������� ���� ���� ��
        if (needTX_Alarm==ON)  // alarm info TX - immediatelly TX with ionistor charge if needed  
          { while ( !(CNT_GSM_SendAlarmSMS() == ON) ) {;} // ������� ���������� ����� �� ���������� ���������� � ���������� �� �������� �������
            needTX_Alarm=OFF ; // ����� ����� ������
          }
#endif //___LBPRESENT // ����� �������� ���� ���� ��
#ifdef ___LBNOTPRESENT // ������� ���� ��� ��
        if ((needTX_Alarm==ON)&&  (Volt_INSTR > VOLTIONSTRMIN))  // alarm info TX - ���� ���������� �������
           if (CNT_GSM_SendAlarmSMS() == ON) needTX_Alarm=OFF; // � ������� �������� ����� �������, �� ����� ����� ������
#endif //___LBNOTPRESENT // ����� �������� ���� ��� ��
        
          
        if ((needTX_Clock==ON) && (Volt_INSTR > VOLTIONSTRMIN) )  // ����� �������� ��� �� ���������� � ���� ���� ���������� ���������� �� ��������� ��� ��������� ��������� (����� ��� ��� ����� ����� ���)
            {if (CNT_GSM_Module_ON()== ON)              // � ���� ������� �������� ��� ������, ��� �� ����� ��� �������
              { if ( CNT_GSM_SendClockSMS()== ON)        // ��������  ��� �� ����������
                {  needTX_Clock=OFF;                      // � ���������� ���� � ������ ������
                }
              }
            }


        if (needTX==ON)  //���� ���� ������ �� �������� ����������� ���������
        { if (Volt_INSTR > VOLTIONSTRMIN) // ���� ���� ���������� ���������� �� ��������� ��� ��������� ��������� (� ��������� �������� ���)
          
          
          {  if (CNT_GSM_Module_ON()== ON) // � ���� ������� �������� ��� ������, ��� �� ����� ��� �������
              { if (CNT_GSM_SendDefaultSMS() == ON)  needTX=OFF;     // �������� ���������� ��� � ���������� ���� ��� ������
              }  // ��������� �������� ��������. ���� ������� ���������� �� ���������� ��������... 
          }
        }  
    
        CNT_GSM_Module_OFF(); // ������ ����� ��������� ������ ���
    }
 
    if ((needTX_Clock==ON)|| (needTX_Alarm==ON)) // ���� ��� �� ���������� ��� ����� �� ���� ��������
    { // ����� ������������ ��������� ����� ������� ���������� �� RTC_Wakeup 
      // ��� ������� �������� ��� ��� ��� ��������
  
      
    }
        
    

    CNT_GPIO_PrepToStop();   // ������������ �� ���
    //GPIO_TOGGLE(LED_PORT,LED2_PIN); // LED for info
    PWR_UltraLowPowerCmd(ENABLE);     // �������� ������� � ���
    PWR_ClearFlag(PWR_FLAG_WU);       // ������� ���� �������
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI); // � ������ (STOP)... ������� ���� �� ��������... ))

 }  // ����� ��������� ����� while 
    //return 0; ���� ������ ������ �� ����������, �.�. �� �������������
}


void RTC_Alarm_IRQHandler(void) // alarm handler
{ // ����������� �� ����������. 
  // ���� ���� �����, �������� ��� ��� ��� ��.
  // ���� ��� - ������ ������ ������ ������� - ��� �������� ���.
  
  CNT_RTC_ClearAlarmAFlags(); // ������ ������ ����� �
  
  needTX_Clock = ON;  // ������������� ���� ������������� �������� �� ����������.
    
  EXTI_ClearFlag(EXTI_Line17); // ������� ���� ���������      
}



void RTC_WKUP_IRQHandler(void) // wakeup intr hanhler
{ // ����� ���������� �� RTC, ������������� (��� �� ��������� � ������� ����). 
   CNT_RTC_ClearWakeupFlags();  // ������� ����� ������� 
   //GPIO_HIGH(LED_PORT,LED2_PIN);
     CNT_GetVoltages(); // ������� ����������
   /*  ���� ����� �������� ���������� �������� ����:
      �� ����������� ��������� �������� �������� ����� �� �������� �� �����������, �������� ���� ������� �����?
      ����� ����� ����� ���� ������ � ������ ������� �������� ��������� � �������� ���������
      ��������� ����������� ����� ���� ����� �� ���������. 
      ������� ������������ ������� ���� ����� �����
      ���� ����������� ������ � �.�.   
   */
   
     
   //needTX = ON ; // �������� ���� ������������� ���������� �������� (���� ��� �����������)
   //GPIO_LOW(LED_PORT,LED2_PIN); // ����� ������ ��� ��� �������

  
  
  
     EXTI_ClearFlag(EXTI_Line20); // ������� ���� ���������

} // end interrupt


void EXTI0_IRQHandler(void) // Pulse Count handler - Ch1 - PC0
{
  //GPIO_HIGH(LED_PORT,LED1_PIN);
  NVIC_DisableIRQ(EXTI0_IRQn);
  Pulse1Overall +=1;                    // ������� �������� �� ������� ������
  needTX_Alarm = ON; // ��� ����� ������!!!
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
  Pulse2Overall +=1;            // ������� �������� �� 2 ������
    needTX_Alarm = ON; // ��� ����� ������!!!

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
  Pulse3Overall +=1;// ������� �������� �� 3 ������
    needTX_Alarm = ON; // ��� ����� ������!!!

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
  /*for (i=0; i<10; i++) { // :)
                        GPIO_TOGGLE(LED_PORT,LED1_PIN);
                        CNT_DummyDelay(DUMMYALARMCOUNTDELAY);
                        CNT_DummyDelay(DUMMYALARMCOUNTDELAY);
                        }
  */
  Alarms1Count +=1;                     // ������� ������ �� ������� ������
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
  /*for (i=0; i<10; i++) { // :)
                        GPIO_TOGGLE(LED_PORT,LED1_PIN);
                        CNT_DummyDelay(DUMMYALARMCOUNTDELAY);
                        CNT_DummyDelay(DUMMYALARMCOUNTDELAY);
                        }
  */
  Alarms2Count +=1;             // ������� ������ �� ������� ������
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
{ // ���� � ������ �������. �� ������������ �����. ����������  ��� ������� ������ ��� ������� � ��� �������.
  //TIM_Cmd(TIM9, DISABLE);    // ���������� �������
    NVIC_DisableIRQ(TIM10_IRQn);
    TIM_ClearFlag(TIM10, TIM_FLAG_Update); // ������� ���� ������������ �������
    GPIO_TOGGLE(LED_PORT,LED1_PIN);
    TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
    TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);
    NVIC_EnableIRQ(TIM10_IRQn);
  //TIM_Cmd(TIM9, DISABLE);    // ���������� �������
  //TIM_Cmd(TIM9, ENABLE);    // ���������� �������
}




