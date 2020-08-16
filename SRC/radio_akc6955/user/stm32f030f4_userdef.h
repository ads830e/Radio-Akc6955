/***********************************************************
头文件和预定义
***********************************************************/
#ifndef USE_STDPERIPH_DRIVER
#define USE_STDPERIPH_DRIVER
#endif
#ifndef STM32F0XX
#define STM32F0XX
#include "stm32f0xx.h"
#endif

/***********************************************************
Common Define
***********************************************************/
#ifndef BOOL
#define BOOL char
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0 
#endif

#define FXTAL 4000000
/***********************************************************
Delay Functions
***********************************************************/
#define Delay(delay) Delay_Char(delay)
void Delay_Char(unsigned char delay){
	 while(delay--);
}
void Delay_Int(unsigned int delay){
	 while(delay--);
}
void Delay_Long(unsigned long delay){
	 while(delay--);
}
void Delay_1ms(unsigned int delay){
	 unsigned int count;
         unsigned int countmax=FXTAL/12000;
	 while(delay--){
 			 count=countmax;
			 while(count--);
	 }
}

/***********************************************************
Uart functions
***********************************************************/

/* USART初始化 */
void USART1_Init(uint32_t USART_BaudRate){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  //使能GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//使能USART的时钟
	/* USART1的端口配置 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);//配置PA9成第二功能引脚	TX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);//配置PA10成第二功能引脚  RX	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* USART1的基本配置 */
	USART_InitStructure.USART_BaudRate = USART_BaudRate;              //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);		
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);           //关闭接收中断
	USART_Cmd(USART1, ENABLE);                             //使能USART1
	/* USART1的NVIC中断配置 */
	//NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	//NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
	//NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_Init(&NVIC_InitStruct);
}
#define USART1_DeInit()	USART_DeInit(USART1)
void USART1_Send_Char(unsigned char ch){
    while(( USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET));
    USART_SendData(USART1,ch);
}
int USART1_Receive_Char(void){
    int ch;
    unsigned int timeout=0x2000;
    while( USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==0 ){
          if(!timeout) return -1;
          timeout--;
    }
    //if(USART_GetFlagStatus(USART1,USART_FLAG_RTOF)) return -1;
    ch=USART_ReceiveData(USART1);
    //USART_ClearFlag(USART1,USART_FLAG_RTOF);
    USART_ClearFlag(USART1,USART_FLAG_RXNE);
    return ch;
}

/******************抽象层*********************************************************/
#define UART_Init(baud) USART1_Init(baud)
#define UART_DeInit() USART1_DeInit()
#define UART_Send_Char(ch) USART1_Send_Char(ch)
#define UART_Receive_Char() USART1_Receive_Char()
/*********************Uart advanced functions**************/
void UART_Send_Num(long num){
	unsigned char count=0,dight[10];
	if(num<0){
		UART_Send_Char('-');
                num=-num;
		}
		do{
                    count++;
		    dight[count-1]=num%10+'0';
                    num/=10;
		}while(num);
		for(;count;count--) UART_Send_Char(dight[count-1]);
		UART_Send_Char('\0');
}
void UART_Send_Hex(unsigned char num){
	unsigned char HexTable[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	UART_Send_Char(HexTable[num/0x10]);
   	UART_Send_Char(HexTable[num&0x0f]);
	UART_Send_Char('\0');
}
void UART_Send_String(unsigned char *str_ptr){
	unsigned char count=0;
	while(str_ptr[count]){
		if(count==0xff) break;
		UART_Send_Char(str_ptr[count]);
		count++;
	}
	UART_Send_Char('\0');
}
#define UART_Send_Str(str_ptr) UART_Send_String(str_ptr)
unsigned char UART_Receive_String(unsigned char *ch,unsigned char length){
        unsigned int count;
	for(count=0;count<length;count++){
		if(ch[count]=UART_Receive_Char()==-1) return -1;
		if(ch[count]=='\0') break;
		length--;
	}
        return 0;
}
#define UART_Receive_Str(ch,length) UART_Receive_String(ch,length)


/***********************************************************
ADC函数
***********************************************************/
void ADC1_Init(unsigned char channel){
  const uint16_t ADC_GPIO_Pin[]={
    GPIO_Pin_0,
    GPIO_Pin_1,
    GPIO_Pin_2,
    GPIO_Pin_3,
    GPIO_Pin_4,
    GPIO_Pin_5,
    GPIO_Pin_6,
    GPIO_Pin_7 
  };
  const uint32_t ADC_Channel[]={
    ADC_Channel_0,
    ADC_Channel_1,
    ADC_Channel_2,
    ADC_Channel_3,
    ADC_Channel_4,
    ADC_Channel_5,
    ADC_Channel_6,
    ADC_Channel_7
  };
  
  ADC_InitTypeDef     ADC_InitStruct;
  GPIO_InitTypeDef    GPIO_InitStruct;
  ADC_DeInit(ADC1);//ADC1 DeInit
  channel&=0x07;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);// Enable  GPIOA clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);// ADC1 Periph clock enable
  //Configure PA.x  as analog input
  GPIO_InitStruct.GPIO_Pin = ADC_GPIO_Pin[channel];
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);				// PAx,输入时不用设置速率
  //Initialize ADC structure 
  ADC_StructInit(&ADC_InitStruct);
  //Configure the ADC1 in continous mode with a resolutuion equal to 12 bits 
  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStruct.ADC_ContinuousConvMode = DISABLE; 
  ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Backward;
  ADC_Init(ADC1, &ADC_InitStruct); 
  //Convert the ADC1 temperature sensor  with 55.5 Cycles as sampling time
  //ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor , ADC_SampleTime_55_5Cycles);  
  //ADC_TempSensorCmd(ENABLE);
  ADC_ChannelConfig(ADC1,ADC_Channel[channel],ADC_SampleTime_55_5Cycles);//Convert the ADC1 Vref  with 55.5 Cycles as sampling time
  //ADC_VrefintCmd(ENABLE);
  //ADC Calibration
  ADC_GetCalibrationFactor(ADC1);
  ADC_DiscModeCmd(ADC1,ENABLE);
  ADC_Cmd(ADC1, ENABLE);  // Enable ADC1   
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); // Wait the ADCEN falg
}

int ADC1_Get(void){
    ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
    ADC_StartOfConversion(ADC1);// ADC1 regular Software Start Conv 
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

#define ADC1_DeInit() ADC_DeInit(ADC1)// ADC1 DeInit
/******************抽象层*********************************************************/
#define ADC_Init_U(channel)  ADC1_Init(channel)
#define ADC_DeInit_U()   ADC1_DeInit()
unsigned int ADC_Once(unsigned char channel){
    ADC1_Init(channel);
    return ADC1_Get();
}
#define ADC_Get() ADC1_Get()
/***********************************************************
全局中断开启与关闭
***********************************************************/
#define Enable_Interrupts()      NVIC_RESETFAULTMASK()；    //开放总中断  
#define Disable_Interrupts()     NVIC_SETFAULTMASK()；      //关闭总中断  









