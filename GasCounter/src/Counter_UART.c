#include "Counter_UART.h"
#include "stm32l1xx_usart.h"

// ����� �� �����
#define RX_BUFFER_SIZE 512 // ������ ������
volatile uint8_t    rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t   rx_wr_index=0, //������ ������ ������ (���� ������ ������)
                    rx_rd_index=0, //������ ������ ������ (������ ������ ������)
                    rx_counter=0; //���������� ������ � ������ 
volatile uint8_t    rx_buffer_overflow=0; //���������� � ������������ ������

// ����� �� ��������
#define TX_BUFFER_SIZE 512 //������ ������
volatile uint8_t   tx_buffer[TX_BUFFER_SIZE];
volatile uint16_t  tx_wr_index=0, //������ ������ ������ (���� ������ ������)
                   tx_rd_index=0, //������ ������ ������ (������ ������ ������)
                   tx_counter=0; //���������� ������ � ������


unsigned char at_out[MAX_AT_SIZE], at_in[MAX_AT_SIZE];




//�������������� USART1
void CNT_UART_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); //��������� ������������  
 
//��������� �������������� �������
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1); //PA9 to TX USART1
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //PA10 to RX USART1

//��������� ���� ��������� 
// PA9 -> TX UART.
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
GPIO_Init(GPIOA, &GPIO_InitStructure); //��������������
 
//PA10  -> RX UART. 
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
GPIO_Init(GPIOA, &GPIO_InitStructure);//��������������


USART_InitTypeDef USART_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //��������� ������������
        
  /* ����������� ��������� COM ����� ��----------------------------------*/
  /* USART configured as follow:
  - BaudRate = 9600 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;// ��������
  USART_InitStructure.USART_WordLength = USART_WordLength_8b; //8 ��� ������
  USART_InitStructure.USART_StopBits = USART_StopBits_1; //���� ���� ���
  USART_InitStructure.USART_Parity = USART_Parity_No; //�������� - ���
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��������� ������� - ���
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;       // ��������� ����� � ��������
        
//USART_OverSampling8Cmd(ENABLE); //����� ��������� ������� �������������  
//USART_OneBitMethodCmd(ENABLE); //����� ��������� ���������� �������������
//USART_HalfDuplexCmd (ENABLE); // ����� ������� �������������� �����.
        
  USART_Init(USART1, &USART_InitStructure); //��������������
  
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //������������� ���������� ����� � �������� ����������, ���� � ��� ���� ���������� ��� ��� ����� ������ �� ����
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //���������� �� uart1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //������ ��������� � ������
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //������ ��������� � ���������
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //  ��������� ����������
  NVIC_Init(&NVIC_InitStructure); //��������������
  
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // ��������� �� ������
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);  //  � ��������
  
}


void CNT_UART_state(uint16_t state)
{       
  if (state == ON) 
  {     
    CNT_UART_init();
    USART_Cmd(USART1, ENABLE);
  }
  else 
  {USART_Cmd(USART1, DISABLE);
  }
}



void USART1_IRQHandler(void)
{ 
  if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET) //���������� �� ������ ������
  {
    if ( (USART1->SR & (USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE|USART_FLAG_ORE) ) == 0) //��������� ��� �� ������
    {                   
      rx_buffer[rx_wr_index++]= (uint8_t) (USART_ReceiveData(USART1)& 0xFF); //��������� ������ � �����, ������������� ����� ������
      //GPIO_TOGGLE(LED_PORT,LED1_PIN);
      if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0; //���� �� �����
      if (++rx_counter == RX_BUFFER_SIZE) //������������ ������
      {
        rx_counter=0; //�������� ������� (������� ��� ������)
        rx_buffer_overflow=1;  //�������� � ������������
      }
    }
    else USART_ReceiveData(USART1); //� ������ ����� ����� ���������� ������, � ������ ������ ������ ���������� ��������� ����.
  }

  if(USART_GetITStatus(USART1, USART_IT_ORE_RX) == SET) //���������� �� ������������ ������
  {
     USART_ReceiveData(USART1); //� ������ ����� ����� ���������� ������������ ������, �� �� ������ ���������� ���� ���� ���������� ������� �� �������� ������.
  }

  if(USART_GetITStatus(USART1, USART_IT_TXE) == SET) //���������� �� ��������
  {   
    if (tx_counter) //���� ���� ��� ��������
    {
      --tx_counter; // ��������� ���������� �� ���������� ������
      USART_SendData(USART1,tx_buffer[tx_rd_index++]); //�������� ������ ������������� ����� ������
      //GPIO_TOGGLE(LED_PORT,LED2_PIN);
      if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0; //���� �� �����
    }
    else //���� ������ ��������, ��������� ���������� �� ��������
    {
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);                    
    }
  }
}

uint8_t get_char(void) //����� ������
{
  uint8_t data; //���������� ��� ������
  int32_t waitingtime; // ��� ��������
  waitingtime = 2000000;
  while ( (rx_counter==0) && (waitingtime-- != 0)   );  //���� ������ ���, ����
  if (waitingtime <=0 ) return 0;  // ���� �� ��������
  data=rx_buffer[rx_rd_index++]; //����� ������ �� ������
  if (rx_rd_index == RX_BUFFER_SIZE) 
                                rx_rd_index=0; //���� �� �����
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); //��������� ����������
  --rx_counter; //����� ��� �� �������� �������� ����������
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������� ����������
return data;
}

void put_char(uint8_t c) //����� ������
{
while (tx_counter == TX_BUFFER_SIZE); //���� ����� ����������, ����
USART_ITConfig(USART1, USART_IT_TXE, DISABLE); //��������� ����������, ����� ��� �� ������ ������ ����������
if (tx_counter || (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)) //���� � ������ ��� ���-�� ���� ��� ���� � ������ ������ ���-�� ��� ����������
  {
  tx_buffer[tx_wr_index++]=c; //�� ������ ������ � �����
  if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0; //���� �� �����
  ++tx_counter; //����������� ������� ���������� ������ � ������
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE); //��������� ����������
  }
else //���� UART ��������
  USART_SendData(USART1,c); //�������� ������ ��� ����������
}


void put_string(unsigned char *s)
{
  while (*s != 0)
    put_char(*s++);
}

void get_string(void)
{
  uint8_t rxbyte;
  uint16_t strpoint=0;
  CNT_UART_initat_in();  // clear at in buffer
  //str = "";
  //if (rx_counter == 0) return;
  rxbyte = LF;
  while (rxbyte != NULL)
  {  rxbyte = get_char();
     if (rxbyte != NULL) at_in[strpoint++] = rxbyte;
     if (strpoint >= MAX_AT_SIZE) strpoint = 0; 
     if ( (at_in[strpoint-4]=='O' ) &&
          (at_in[strpoint-3]=='K' ) &&
          (at_in[strpoint-2]== CR ) &&
          (at_in[strpoint-1]== LF ) )
       break; // OK response
     if ( (at_in[strpoint-7]=='E' ) &&
          (at_in[strpoint-6]=='R' ) &&
          (at_in[strpoint-5]=='R' ) &&
          (at_in[strpoint-4]=='O' ) &&
          (at_in[strpoint-3]=='R' ) &&
          (at_in[strpoint-2]== CR ) &&
          (at_in[strpoint-1]== LF ) )
       break; // ERROR response    

     if ( (at_in[strpoint-5]=='"' ) &&
          (at_in[strpoint-4]==',' ) &&
          (at_in[strpoint-3]=='0' ) &&
          (at_in[strpoint-2]== CR ) &&
          (at_in[strpoint-1]== LF ) )
       break; // +CDS special response  
    
  }
}


void put_atcmd(unsigned char *s)
{
  
  while (*s != 0)
  put_char(*s++);
  put_char(CR); // CR 
}


void put_atcmd_noCRLF(unsigned char *s)
{
  while (*s != 0)
    if ( (*s != CR)&&(*s != LF)) put_char(*s++);
    else *s++;
}




void  CNT_UART_initat_in(void)  // clear at in buffer
{
  uint16_t point = (uint16_t)MAX_AT_SIZE;
  while (point-- != 0) at_in[point] = 0x00;
  at_in[0] = 0x00;
  
    
}





