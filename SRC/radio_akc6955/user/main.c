
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <string.h>

#include "stm32f0xx.h"

#include "main.h"

#include "DigitDisplay.h"
#include "AKC6955.h"

#include "fee.h"

#define Enable_Interrupts()      __asm("cpsie i")    //开放总中断  
#define Disable_Interrupts()     __asm("cpsid i")      //关闭总中断  




//0~7999  0~40MHz
//8000~8000+7600 40~230Mhz

#define RADIO_CHANNEL_MAX (8000+7600)
#define RADIO_CHANNEL_MIN 40
#define RADIO_VOLUME_MAX 62



static int32_t radio_channel=8000+50*40;
static int32_t radio_channel_delta=1;
static int32_t radio_volume=24;

static int32_t radio_channel_old=-1;
static int32_t radio_volume_old=-1;

static bool flag00=0;
static bool flag10=0;


static int32_t radio_channel_nochange_ctr=0;


static bool systick_able_for_volume=false;
static bool systick_able_for_channelchange=false;
static bool systick_able_for_channeldelta=false;

static bool systick_10ms_able=false;
static bool systick_100ms_able=false;
static bool systick_1000ms_able=false;


void SysTick_Handler(void)
{
  static uint32_t cnt_for_1000ms=0;
  if(cnt_for_1000ms==0) systick_1000ms_able=true;
  cnt_for_1000ms=(cnt_for_1000ms+1)%1000;
  
  static uint32_t cnt_for_100ms=0;
  if(cnt_for_100ms==0) systick_100ms_able=true;
  cnt_for_100ms=(cnt_for_100ms+1)%100;
  
  static uint32_t cnt_for_10ms=0;
  if(cnt_for_10ms==0) systick_10ms_able=true;
  cnt_for_10ms=(cnt_for_10ms+1)%10;
  
  static uint32_t cnt_for_50ms=0;
  if(cnt_for_50ms==0) 
  {
    systick_able_for_volume=true;
    systick_able_for_channelchange=true;
    systick_able_for_channeldelta=true;
  }
  cnt_for_50ms=(cnt_for_50ms+1)%50;
}





static void ExtiInit(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    //将EXTI0~4指向PA0~4
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);  
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource1);  
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource2);  
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource3);  
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource4);  
    
    //EXTI0~4中断线配置  
    EXTI_InitTypeDef EXTI_InitStructure;  
    
    EXTI_InitStructure.EXTI_Line=EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd=ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    EXTI_InitStructure.EXTI_Line=EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd=ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    EXTI_InitStructure.EXTI_Line=EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd=ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    EXTI_InitStructure.EXTI_Line=EXTI_Line3;
    EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd=ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    EXTI_InitStructure.EXTI_Line=EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd=ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    //EXTI0中断向量配置  
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel=EXTI0_1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority=0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel=EXTI2_3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority=0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel=EXTI4_15_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority=0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}




void EXTI0_1_IRQHandler(void)
{
  if(EXTI_GetFlagStatus(EXTI_Line0)==SET)
  {
    EXTI_ClearFlag(EXTI_Line0);
    
    if(!flag00)
    {
      flag00=true;
      
      
      if(systick_able_for_channelchange)
      {
        systick_able_for_channelchange=false;
        
        
        
          int32_t radio_channel_tmp=radio_channel;
          if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)!=0)
          {
            radio_channel_tmp+=radio_channel_delta;
            if(radio_channel_tmp>=RADIO_CHANNEL_MAX)
            {
              radio_channel_tmp=RADIO_CHANNEL_MAX;
            }
          }
          else
          {
            radio_channel_tmp-=radio_channel_delta;
            if(radio_channel_tmp<RADIO_CHANNEL_MIN) 
            {
              radio_channel_tmp=RADIO_CHANNEL_MIN;
            }
            
          }
          radio_channel=radio_channel_tmp;
        
      }
      
    }

  }
  else if(EXTI_GetFlagStatus(EXTI_Line1)==SET)
  {
    EXTI_ClearFlag(EXTI_Line1);
    flag00=false;
    
  }
}
void EXTI2_3_IRQHandler(void)
{
  if(EXTI_GetFlagStatus(EXTI_Line2)==SET)
  {
    EXTI_ClearFlag(EXTI_Line2);
    
    if(systick_able_for_channeldelta)
    {
        systick_able_for_channeldelta=false;
      
        if(radio_channel_delta==1)
        {
          radio_channel_delta=10;
        }
        else if(radio_channel_delta==10)
        {
          radio_channel_delta=100;
        }
        else
        {
          radio_channel_delta=1;
        }
    }
    
  }
  else if(EXTI_GetFlagStatus(EXTI_Line3)==SET)
  {
    EXTI_ClearFlag(EXTI_Line3);
    if(!flag10)
    {
      flag10=true;
      
      
      if(systick_able_for_volume)
      {
          systick_able_for_volume=false;
        
        
          if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==0)
          {
            radio_volume+=2;
            if(radio_volume>=RADIO_VOLUME_MAX) radio_volume=RADIO_VOLUME_MAX;
          }
          else
          {
            radio_volume-=2;
            if(radio_volume<20) radio_volume=20;
          }
        
        
      }
      
    }
    
    
  }
}
void EXTI4_15_IRQHandler(void)
{
  if(EXTI_GetFlagStatus(EXTI_Line4)==SET)
  {
    EXTI_ClearFlag(EXTI_Line4);
    
    flag10=false;
    
  }
}





void UpdateVolume(void)
{
  AKC6955_SetVolume(radio_volume);
}



void UpdateChannel(void)
{
  uint32_t freq_khz=0;
  if(radio_channel<8000)
  {
    //0~40MHz
    freq_khz=radio_channel*5;
    AKC6955_SetChannel(false,radio_channel);
  }
  else if(radio_channel<RADIO_CHANNEL_MAX)
  {
    //
    uint16_t realfmchannel=400+(radio_channel-8000);
    freq_khz=30000+realfmchannel*25;
    AKC6955_SetChannel(true,realfmchannel);
  }
  else
  {
    freq_khz=30000+8000*25;
    AKC6955_SetChannel(true,8000);
  }
  DigitDisplaySetNumber(true,freq_khz);
}



int main(void){
  
  Disable_Interrupts();
  
  SystemInit();
  
  
  SysTick_Config(1000);//1ms
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
  
  
  DigitDisplayInit();
  ExtiInit();
  
  Fee_Init();
  
  
  if(Fee_Read())
  {
    memcpy(&radio_channel,&Fee_Buf[0],4);
    memcpy(&radio_channel_delta,&Fee_Buf[4],4);
    memcpy(&radio_volume,&Fee_Buf[8],4);
  }
  
  if(radio_channel>RADIO_CHANNEL_MAX) radio_channel=RADIO_CHANNEL_MAX;
  if(radio_channel<RADIO_CHANNEL_MIN) radio_channel=RADIO_CHANNEL_MIN;
  
  if((radio_channel_delta!=1)&&(radio_channel_delta!=10)&&(radio_channel_delta!=100)) radio_channel_delta=1;
  
  if(radio_volume<20) radio_volume=20;
  if(radio_volume>RADIO_VOLUME_MAX) radio_volume=RADIO_VOLUME_MAX;
  
  AKC6955_Init();
  
  UpdateVolume();
  UpdateChannel();
  
  
  Enable_Interrupts();
  
  while (1){
    
    if(systick_100ms_able)
    {
      systick_100ms_able=false;
      
      
      
      if(radio_channel_old==radio_channel)
      {
        if(radio_channel_nochange_ctr<=120) radio_channel_nochange_ctr++;
      }
      else
      {
        radio_channel_nochange_ctr=0;
      }
      
      if(radio_channel_nochange_ctr==120)
      {
        radio_channel_nochange_ctr++;
        //save channel
        
        memcpy(&Fee_Buf[0],&radio_channel,4);
        memcpy(&Fee_Buf[4],&radio_channel_delta,4);
        memcpy(&Fee_Buf[8],&radio_volume,4);
        
        Fee_Write();
      }
      
      
    
      if(radio_volume_old!=radio_volume)
      {
        radio_volume_old=radio_volume;
        UpdateVolume();
      }
      if(radio_channel_old!=radio_channel)
      {
        radio_channel_old=radio_channel;
        UpdateChannel();
      }

      if(AKC6955_IsResetNeeded())
      {
        AKC6955_Init();
      UpdateVolume();
      UpdateChannel();
    
      }
    }
    
  }
        
}
