#include "Counter_Misc.h"
#include <string.h> 



void CNT_DummyDelay(uint32_t tensnops)
{ uint32_t i;   
  for (i=0; i<tensnops; i++) { __NOP();// __NOP(); __NOP(); 
                                 // __NOP(); __NOP(); __NOP();
                                //  __NOP(); __NOP(); __NOP();
                                  __NOP(); 
                                }
}


uint8_t CNT_DummyDelayLP_TO(uint32_t ms) // задержка с включенем низкого потребления на время задержки
{  // пока в стадии отладки
   /* Используем таймер для генерации прерывания.
     слип до первого прерывания - таймер или что-то другое. 
     если что-то другое, то сигнализируем ON, если таймер - то таймаут 
    Используем таймер 9 с клоком от LSE
  */    

    TIM_TimeBaseInitTypeDef TIM_InitStructure;  // Заводим структуру
    TIM_DeInit(TIM10);    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

    TIM_RemapConfig(TIM10, TIM10_LSE); // connect to LSE  
    TIM_TimeBaseStructInit(&TIM_InitStructure); // Инициализация структуры
    TIM_InitStructure.TIM_Prescaler = 3200;    // Предделитель ms 32768 / 1000
    TIM_InitStructure.TIM_Period = ms;        // Период таймера - из входного параметра
    
    TIM_TimeBaseInit(TIM10, &TIM_InitStructure); // Функция настройки таймера
    //TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE);   
    TIM_ClearFlag(TIM10, TIM_FLAG_Update); // сбросим флаг срабатывания таймера
   
    //TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
    //TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);   
    TIM_Cmd(TIM10, ENABLE); 

    //NVIC_EnableIRQ(TIM10_IRQn);   
    
    while (!TIM_GetFlagStatus(TIM10, TIM_FLAG_Update) ) {;}
    //PWR_UltraLowPowerCmd(DISABLE);
    //PWR_EnterSleepMode(PWR_Regulator_ON, PWR_SLEEPEntry_WFI);
   
    //NVIC_DisableIRQ(TIM10_IRQn);

    return ON;
}



void reverse(unsigned char* s)
{
  uint32_t i, j;
  char c;
  for (i = 0, j = strlen(s)-1; i<j; i++, j--) 
  {     c = s[i];
        s[i] = s[j];
        s[j] = c;
  }
}

void itoa(int32_t n, unsigned char* s)
{
  uint32_t i, neg;
  i = 0; neg = 0;
  if (n<0)  {   neg=1; // потом добавить минус если отрицательное
                n = -n;
            }
  do { //генерируем цифры в обратном порядке 
        s[i++] = n % 10 + '0'; //берем следующую цифру 
      }
  while ((n /= 10) > 0); // удаляем 
  if (neg == 1) {s[i++] = '-'; } // добавим минус если надо
  s[i] = '\0';
  reverse(s);
}



void concat(unsigned char* dest, unsigned char* src) // string concatenate !!! warn about memory space for string!!!
{
  uint16_t i,j;
  i = 0;
  j = strlen(dest);
  for (i=0; i<(strlen(src)); i++)
    { dest[i+j] = src[i];
      dest[i+j+1] = '\0';
    }
}


                              
void CMT_Misc_LEDIndication(uint8_t LEDNo, uint8_t first, uint8_t second, uint8_t third  ) // три вспышки светодиодом
{ // firs, second, third = длительность. 0=не мигать, 1 = короткий, 2=длинее ... 

  if (LEDNo == 1) {
        GPIO_HIGH(LED_PORT,LED1_PIN); CNT_DummyDelay(first  * 100000); GPIO_LOW(LED_PORT,LED1_PIN); CNT_DummyDelay(170000); // 
        GPIO_HIGH(LED_PORT,LED1_PIN); CNT_DummyDelay(second * 100000); GPIO_LOW(LED_PORT,LED1_PIN); CNT_DummyDelay(170000); // 
        GPIO_HIGH(LED_PORT,LED1_PIN); CNT_DummyDelay(third  * 100000); GPIO_LOW(LED_PORT,LED1_PIN); CNT_DummyDelay(170000); //   
                  }
  if (LEDNo == 2) {
        GPIO_HIGH(LED_PORT,LED2_PIN); CNT_DummyDelay(first  * 100000); GPIO_LOW(LED_PORT,LED2_PIN); CNT_DummyDelay(170000); // 
        GPIO_HIGH(LED_PORT,LED2_PIN); CNT_DummyDelay(second * 100000); GPIO_LOW(LED_PORT,LED2_PIN); CNT_DummyDelay(170000); // 
        GPIO_HIGH(LED_PORT,LED2_PIN); CNT_DummyDelay(third  * 100000); GPIO_LOW(LED_PORT,LED2_PIN); CNT_DummyDelay(170000); //   
                  }

}
                              

                              
                              
                              