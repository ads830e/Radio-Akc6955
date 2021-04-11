
#include "DigitDisplay.H"

#include "stm32f0xx.h"




void DigitDisplayInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  //使能GPIOA的时钟
  /* PA5,PA6,PA7的端口配置 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOA,GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
  return;
}

static uint8_t DigitDisplayTableValueWrap(uint8_t val,bool dot)
{
  const uint8_t table[]={
    0xc0,
    0xf9,
    0xa4,
    0xb0,
    
    0x99,
    
    0x92,
    0x82,
    
    0xf8,
    
    0x80,
    0x90,
    
    0x7f,
  };
  
  uint8_t val1=table[val%10];
  
   if(dot) val1&=0x7f;
  else val1|=0x80;
  
  uint8_t ret=0;
  uint8_t ret1=0;
  if(val1&0x01) ret|=0x08;
  if(val1&0x02) ret|=0x04;
  if(val1&0x04) ret|=0x02;
  if(val1&0x08) ret|=0x10;
  if(val1&0x10) ret|=0x20;
  if(val1&0x20) ret|=0x80;
  if(val1&0x40) ret|=0x40;
  if(val1&0x80) ret|=0x01;
  
 
  
  for(uint8_t i=0;i<8;i++)
  {
    if(ret&1) ret1|=1;
    ret>>=1;
    if(i>=7) break;
    ret1<<=1;
  }
  
  return ret1;
}


void DigitDisplaySetNumber(bool enable,uint32_t freq_khz)
{
    uint8_t data[4]={0xff,0xff,0xff,0xff};
    if(!enable)
    {
      data[0]=0xff;
      data[1]=0xff;
      data[2]=0xff;
      data[3]=0xff;
    }
    else
    {
      uint8_t digit[4]={0,0,0,0};
    
      if(freq_khz<=9999)
      {
        digit[0]=(freq_khz/1000)%10;
        digit[1]=(freq_khz/100)%10;
        digit[2]=(freq_khz/10)%10;
        digit[3]=(freq_khz/1)%10;
        
        data[0]=DigitDisplayTableValueWrap(digit[0],1);
        data[1]=DigitDisplayTableValueWrap(digit[1],0);
        data[2]=DigitDisplayTableValueWrap(digit[2],0);
        data[3]=DigitDisplayTableValueWrap(digit[3],0);
      }
      else if(freq_khz<=99994)
      {
        freq_khz+=5;
        digit[0]=(freq_khz/10000)%10;
        digit[1]=(freq_khz/1000)%10;
        digit[2]=(freq_khz/100)%10;
        digit[3]=(freq_khz/10)%10;
        
        data[0]=DigitDisplayTableValueWrap(digit[0],0);
        data[1]=DigitDisplayTableValueWrap(digit[1],1);
        data[2]=DigitDisplayTableValueWrap(digit[2],0);
        data[3]=DigitDisplayTableValueWrap(digit[3],0);
      }
      else if(freq_khz<=999949)
      {
        freq_khz+=50;
        digit[0]=(freq_khz/100000)%10;
        digit[1]=(freq_khz/10000)%10;
        digit[2]=(freq_khz/1000)%10;
        digit[3]=(freq_khz/100)%10;
        
        data[0]=DigitDisplayTableValueWrap(digit[0],0);
        data[1]=DigitDisplayTableValueWrap(digit[1],0);
        data[2]=DigitDisplayTableValueWrap(digit[2],1);
        data[3]=DigitDisplayTableValueWrap(digit[3],0);
      }
      else if(freq_khz<=9999499)
      {
        freq_khz+=500;
        
        digit[0]=(freq_khz/1000000)%10;
        digit[1]=(freq_khz/100000)%10;
        digit[2]=(freq_khz/10000)%10;
        digit[3]=(freq_khz/1000)%10;
        
        data[0]=DigitDisplayTableValueWrap(digit[0],0);
        data[1]=DigitDisplayTableValueWrap(digit[1],0);
        data[2]=DigitDisplayTableValueWrap(digit[2],0);
        data[3]=DigitDisplayTableValueWrap(digit[3],0);
      }
      else
      {
        digit[0]=(freq_khz/1000000)%10;
        digit[1]=(freq_khz/100000)%10;
        digit[2]=(freq_khz/10000)%10;
        digit[3]=(freq_khz/1000)%10;
        
        data[0]=DigitDisplayTableValueWrap(digit[0],0);
        data[1]=DigitDisplayTableValueWrap(digit[1],0);
        data[2]=DigitDisplayTableValueWrap(digit[2],0);
        data[3]=DigitDisplayTableValueWrap(digit[3],0);
      }
      
    }
    
    GPIO_ResetBits(GPIOA,GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
    
    for(uint8_t i=0;i<4;i++)
    {
      for(uint8_t j=0;j<8;j++)
      {
        if(((data[i]>>j)&1)!=0)
        {
          GPIO_SetBits(GPIOA,GPIO_Pin_5);
        }
        else
        {
          GPIO_ResetBits(GPIOA,GPIO_Pin_5);
        }
        GPIO_SetBits(GPIOA,GPIO_Pin_7);
        GPIO_ResetBits(GPIOA,GPIO_Pin_7);
        
      }
    }
    
    GPIO_SetBits(GPIOA,GPIO_Pin_6);
    GPIO_ResetBits(GPIOA,GPIO_Pin_6);
    
    GPIO_ResetBits(GPIOA,GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
    
}





