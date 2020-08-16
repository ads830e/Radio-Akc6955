#include "main.h"
#include "stm32f030f4_userdef.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

static void DigitDisplayInit(void)
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


static void DigitDisplaySetNumber(bool enable,uint32_t freq_khz)
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



static uint32_t radio_channel=8400;
static uint32_t radio_channel_delta=1;
static uint32_t radio_volume=24;

static bool flag00=0;
static bool flag10=0;

void EXTI0_1_IRQHandler(void)
{
  if(EXTI_GetFlagStatus(EXTI_Line0)==SET)
  {
    EXTI_ClearFlag(EXTI_Line0);
    
    if(!flag00)
    {
      flag00=true;
      uint32_t radio_channel_tmp=radio_channel;
      if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)!=0)
      {
        radio_channel_tmp+=radio_channel_delta;
        if(radio_channel_tmp>=6000+8191)
        {
          radio_channel_tmp=6000+8191;
        }
      }
      else
      {
        if(radio_channel_tmp>radio_channel_delta)
        {
          radio_channel_tmp-=radio_channel_delta;
        }
        else
        {
          radio_channel_tmp=0;
        }
      }
      radio_channel=radio_channel_tmp;
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
  else if(EXTI_GetFlagStatus(EXTI_Line3)==SET)
  {
    EXTI_ClearFlag(EXTI_Line3);
    if(!flag10)
    {
      flag10=true;
      
      if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==0)
      {
        if(radio_volume>=63) radio_volume=63;
        else radio_volume++;
      }
      else
      {
        if(radio_volume>20) radio_volume--;
        else radio_volume=20;
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



#define  I2C_AKC6955_Address 0x10
const uint32_t I2C_TIMEOUT_MAX=0x1000;


void I2C_AKC6955_Delay(void)
{
  //Delay_Int(0);
}

void I2C_AKC6955_Init(void)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOB,GPIO_Pin_1);
  
  Delay_Int(100);
  
  GPIO_SetBits(GPIOB,GPIO_Pin_1);
  
  Delay_Int(100);
}

void I2C_AKC6955_SDA_IN(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
}


void I2C_AKC6955_SDA_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
}

bool I2C_AKC6955_SDA_Read(void)
{
  return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10)!=0;
}

void I2C_AKC6955_Start(void)
{
  //sda out
  I2C_AKC6955_SDA_OUT();
  
  //init
  GPIO_SetBits(GPIOA,GPIO_Pin_10);//sda
  I2C_AKC6955_Delay();
  GPIO_SetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
  
  //start
  GPIO_ResetBits(GPIOA,GPIO_Pin_10);//sda
  I2C_AKC6955_Delay();
  GPIO_ResetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
}


void I2C_AKC6955_Stop(void)
{
  I2C_AKC6955_SDA_OUT();
  //end
  GPIO_ResetBits(GPIOA,GPIO_Pin_9);//sclk
  GPIO_ResetBits(GPIOA,GPIO_Pin_10);//sda
  I2C_AKC6955_Delay();
  GPIO_SetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
  GPIO_SetBits(GPIOA,GPIO_Pin_10);//sda
  I2C_AKC6955_Delay();
}


void I2C_AKC6955_SendData(uint8_t data)
{
  //sda out
  I2C_AKC6955_SDA_OUT();
  
  for(uint32_t i=0;i<8;i++)
  {
    if((data>>(7-i))&1)
    {
      GPIO_SetBits(GPIOA,GPIO_Pin_10);//sda
    }
    else
    {
      GPIO_ResetBits(GPIOA,GPIO_Pin_10);//sda
    }
    I2C_AKC6955_Delay();
    GPIO_SetBits(GPIOA,GPIO_Pin_9);//sclk
    I2C_AKC6955_Delay();
    GPIO_ResetBits(GPIOA,GPIO_Pin_9);//sclk
    I2C_AKC6955_Delay();
  }
}

void I2C_AKC6955_ReadData(uint8_t *data)
{
  //sda out
  I2C_AKC6955_SDA_IN();
  
  for(uint32_t i=0;i<8;i++)
  {
    I2C_AKC6955_Delay();
    GPIO_SetBits(GPIOA,GPIO_Pin_9);//sclk
    I2C_AKC6955_Delay();
    
    *data<<=1;
    if(I2C_AKC6955_SDA_Read())
    {
      *data|=1;
    }
    
    GPIO_ResetBits(GPIOA,GPIO_Pin_9);//sclk
    I2C_AKC6955_Delay();
  }
}


bool I2C_AKC6955_WaitAck(void)
{
  uint32_t I2C_TIMEOUT;
  //sda in for response
  I2C_AKC6955_SDA_IN();
  
  GPIO_SetBits(GPIOA,GPIO_Pin_10);//sda
  I2C_AKC6955_Delay();
  GPIO_SetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_TIMEOUT=0;
  while(I2C_AKC6955_SDA_Read())
  {
    I2C_AKC6955_Delay();
    I2C_TIMEOUT++;
    if(I2C_TIMEOUT>I2C_TIMEOUT_MAX)
    {
      //GPIO_ResetBits(GPIOA,GPIO_Pin_9);//sclk
      I2C_AKC6955_Stop();
      return false;
    }
  }
  GPIO_ResetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
  return true;
}


bool I2C_AKC6955_WaitNack(void)
{
  uint32_t I2C_TIMEOUT;
  //sda in for response
  I2C_AKC6955_SDA_IN();
  
  GPIO_ResetBits(GPIOA,GPIO_Pin_10);//sda
  I2C_AKC6955_Delay();
  GPIO_SetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_TIMEOUT=0;
  while(!I2C_AKC6955_SDA_Read())
  {
    I2C_AKC6955_Delay();
    I2C_TIMEOUT++;
    if(I2C_TIMEOUT>I2C_TIMEOUT_MAX)
    {
      I2C_AKC6955_Stop();
      return false;
    }
  }
  GPIO_ResetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
  return true;
}

void I2C_AKC6955_Ack(void)
{
  I2C_AKC6955_SDA_OUT();
  GPIO_ResetBits(GPIOA,GPIO_Pin_10);//sda
  I2C_AKC6955_Delay();
  GPIO_SetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
  GPIO_ResetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
}

void I2C_AKC6955_Nack(void)
{
  I2C_AKC6955_SDA_OUT();
  GPIO_SetBits(GPIOA,GPIO_Pin_10);//sda
  I2C_AKC6955_Delay();
  GPIO_SetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
  GPIO_ResetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
}


bool I2C_AKC6955_Read_Reg(uint8_t address,uint8_t *data)
{
  I2C_AKC6955_Start();
  I2C_AKC6955_SendData((I2C_AKC6955_Address<<1)|0);
  if(!I2C_AKC6955_WaitAck()) return false;
  I2C_AKC6955_SendData(address);
  if(!I2C_AKC6955_WaitAck()) return false;
  I2C_AKC6955_Stop();
  
  I2C_AKC6955_Start();
  I2C_AKC6955_SendData((I2C_AKC6955_Address<<1)|1);
  if(!I2C_AKC6955_WaitAck()) return false;
  I2C_AKC6955_ReadData(data);
  if(!I2C_AKC6955_WaitNack()) return false;
  I2C_AKC6955_Stop();
  
  return true;
}


bool I2C_AKC6955_Write_Reg(uint8_t address,uint8_t data)
{
  I2C_AKC6955_Start();
  I2C_AKC6955_SendData((I2C_AKC6955_Address<<1)|0);
  if(!I2C_AKC6955_WaitAck()) return false;
  I2C_AKC6955_SendData(address);
  if(!I2C_AKC6955_WaitAck()) return false;
  
  I2C_AKC6955_SendData(data);
  if(!I2C_AKC6955_WaitAck()) return false;
  I2C_AKC6955_Stop();
  
  return true;
}





#define AM  0
#define FM  1

enum band_value
{
  FM1 = 0x00,
  MW2 = 0x02,

  SW1 = 0x05,
  SW2 = 0x06,
  SW3 = 0x07,
  SW4 = 0x08,
  SW5 = 0x09,
  SW6 = 0x0a,
  SW7 = 0x0b,
  SW8 = 0x0c,
  SW9 = 0x0d,
  SW10 = 0x0e,
  SW11 = 0x0f,
  
  LW = 0x00,
  
  TV1 = 0x05,
  TV2 = 0x06,
  //MW1 = 0x01,
};

enum band
{
  _FM1 = 0,
  _MW2,

  _SW1,
  _SW2,
  _SW3,
  _SW4,
  _SW5,
  _SW6,
  _SW7,
  _SW8,
  _SW9,
  _SW10,
  _SW11,

  _LW,
  _TV1,
  _TV2,
  //_MW1,
};

union reg0
{
  unsigned char value;
  struct
  {
    unsigned char reserved:            2;
    unsigned char mute:             1;
    unsigned char seek_direction:       1; // 0 下 1 上
    unsigned char go_seek:            1; // 0->1 开始搜索 STC=1 停止搜索
    unsigned char tune:             1; // 0->1 到指定的频率
    unsigned char fm_mode:            1;
    unsigned char power_on:           1;
  };
};

union reg1
{
  unsigned char value;
  struct
  {
    unsigned char fm_band:            3; // 可以设为000 FM 187~108, 搜台间隔有space定
    unsigned char am_band:            5; // 可以设为00010 MW2，0.522~1.62, 9K搜台
  };
} ;

union reg2
{
  unsigned char value;
  struct
  {
    unsigned char chanel_freq_high:     5; // 信道号的高5位 FM mode: Channel Freq.=25kHz*CHAN + 30MHz   AM mode, 5K信道号模式时:Channel Freq.= (mode3k?3:5) kHz*CHAN
    unsigned char mode3k:             1; // MCU, MW2 工作时，送入的信道号一定要保证是 3 的倍数。否则电台会乱掉
    unsigned char ref_32kmode:        1; // 1—参考时钟为32.768K / 0 1—参考时钟为12M
    unsigned char reserved:           1;
  };
};

union reg3
{
  unsigned char value;
  struct
  {
    unsigned char chanel_freq_low:      8;  // 信道号的低 8 位
  };
};

union reg4
{
  unsigned char value;
  struct
  {
    unsigned char user_chanel_start:    8; // 自定义信道号起始 //chan=32*usr_chan_start
  };
};

union reg5
{
  unsigned char value;
  struct
  {
    unsigned char user_chanel_end:      8; // 自定义信道号终止 chan=32*usr_chan_stop
  };
};

union reg6
{
  unsigned char value;
  struct
  {
    unsigned char phase_inv:          1; // 0 同相 / 1反相
    unsigned char line_in:            1; // 0 OFF / 1 ON
    unsigned char sound_volume:       6; // <24:mute，24~63共40级音量控制，每级1.5dB pd_adc_vol=1 时用该寄存器音量
  };
};


union reg7
{
  unsigned char value;
  struct
  {
    unsigned char fm_band_width:        2; // 00 150K 01 200K 10 50K 11 100K
    unsigned char stereo:             2; // “00”自动立体声,门限有Stereo_th控制 “10”只要有导频就强制立体声 “x1”强制单声道解调
    unsigned char bass_boost:         1;
    unsigned char de:               1; // 去加重模式选择 0— 75 μ s (USA) 1— 50 μ s (China / Europe)
    unsigned char reserved0:            1;
    unsigned char reserved1:            1;
  };
};


union reg8
{
  unsigned char value;
  struct
  {
    unsigned char stereo_threshold:       2; // FM开始立体声解调的CNR门限值
    unsigned char fd_threshold:         2; // 芯片内判台和点灯用的频偏门限
    unsigned char am_cnr_threshold:       2; // AM模式时，芯片判台和点灯的载噪比门限
    unsigned char fm_cnr_threshold:       2; // FM模式时，芯片判台和点灯的载噪比门限
  };
};

union reg9
{
  unsigned char value;
  struct
  {
    unsigned char lv_en:            1; //在低压供电时，是否进入低压工作模式
    unsigned char reserved0:          1;
    unsigned char oscillator_type:      1; //Oscillator source selection
    unsigned char i2c_volume:         1;
    unsigned char reserved1:          4;
  };
};

union reg11
{
  unsigned char value;
  struct
  {
    unsigned char reserved0:          4;
    unsigned char fm_seek_skip:       2; // FM 搜台步进 00—25kHz 01—50kHz 10—100kHz 11—200kHz, 针对 TV1 和 TV2 该档为 100K
    unsigned char reserved1:          2;
  };
};

union reg12
{
  unsigned char value;
  struct
  {
    unsigned char reserved0:          5;
    unsigned char pd_rx:              1; // 1--模拟与射频通道关闭
    unsigned char reserved1:          1;
    unsigned char pd_adc:           1; // 0-- 信号通道 ADC 打开
  };
};

union reg13
{
  unsigned char value;
  struct
  {
    unsigned char reserved0:          2;
    unsigned char predefined_volume:    2; // 输出音量调整整值：00： 0dB 01： 3.5dB 10： 7dB 11： 10.5dB
    unsigned char reserved1:          2;
    unsigned char st_led:           1; // 0—tund管脚为调谐灯 1—FM 且非wtmode时，tund管脚为立体声解调指示灯，其余为调谐灯
    unsigned char reserved2:          1;
  };
};

// *************************************** 以下为只读 ********************************************************//
union reg20
{
  unsigned char value;
  struct
  {
    unsigned char read_chanel_high:     5;
    unsigned char tuned:              1;
    unsigned char finished:           1;
    unsigned char stereo:           1;
  };
};

union reg21
{
  unsigned char value;
  struct
  {
    unsigned char read_chanel_low:    8;
  };
};

union reg22
{
  unsigned char value;
  struct
  {
    unsigned char cnr_am:           7; // AM 制式时信号的载噪比，单位 dB
    unsigned char mode3k:           1;
  };
};

union reg23
{
  unsigned char value;
  struct
  {
    unsigned char cnr_fm:               7; // FM 制式时信号的载噪比，单位 dB
    unsigned char stereo_demode:        1; // 只有 FM 立体声解调时（立体声比例大于 30%）才显示 1
  };
};

union reg24
{
  unsigned char value;
  struct
  {
    unsigned char lvmod:              1; // 低电压最大音量限制模式指示位
    unsigned char reserved:           1;
    unsigned char pgalevel_if:        3; // 射频功率控制环增益等级, 等级越大，增益约高
    unsigned char pgalevel_rf:        3; // 射频功率控制环增益等级, 等级越大，增益约高
  };
};

union reg25
{
  unsigned char value;
  struct
  {
    unsigned char vbat:           6; // 电源电压指示：vcc_bat(V)=1.8+0.05*vbat
    unsigned char reserved:       2;
  };
};

union reg26
{
  unsigned char value;
  struct
  {
    unsigned char fd_num:           8; // 频偏指示，补码格式，大于 127 时，减 256 即可变为正常数值，注意此处 FM 以 1KHz 单位，AM 以100Hz 为单位
  };
};

union reg27
{
  unsigned char value;
  struct
  {
    unsigned char rssi:           7; // 可利用 rssi、pgalevel_rf、pgalevel_if 计算天线口信号电平 FM/SW: Pin(dBuV) = 103 - rssi - 6*pgalevel_rf - 6*pgalevel_if MW/LW: Pin(dBuV) = 123 - rssi - 6*pgalevel_rf - 6*pgalevel_if
    unsigned char reserved:       1;
  };
};


void AKC6955_Init(void)
{
  I2C_AKC6955_Init();
  
  union reg0 _reg0;
  union reg1 _reg1;
  union reg2 _reg2;
  
  union reg6 _reg6;
  union reg7 _reg7;
  union reg8 _reg8;
  union reg9 _reg9;
  
  union reg11 _reg11;
  union reg13 _reg13;
  
  _reg0.value=0;
  _reg0.mute = 0;
  _reg0.seek_direction = 1;
  _reg0.go_seek = 0;
  _reg0.tune = 0;
  _reg0.fm_mode = AM;
  _reg0.power_on = 1;
  I2C_AKC6955_Write_Reg(0,_reg0.value);

  _reg1.value=0;
  _reg1.fm_band = FM1;
  _reg1.am_band = MW2;
  I2C_AKC6955_Write_Reg(1,_reg1.value);

  _reg2.value=0;
  _reg2.mode3k = 0;
  _reg2.ref_32kmode = 0;
  I2C_AKC6955_Write_Reg(2,_reg2.value);

  _reg6.value=0;
  _reg6.phase_inv = 1;
  _reg6.line_in = 0;
  _reg6.sound_volume = 0;
  I2C_AKC6955_Write_Reg(6,_reg6.value);

  /*
  _reg7.value=0;
  _reg7.fm_band_width = 0x03;
  _reg7.stereo = 0x00;
  _reg7.bass_boost = 0;
  _reg7.de = 1;
  _reg7.reserved0 = 0;
  _reg7.reserved1 = 0;
  I2C_AKC6955_Write_Reg(7,_reg7.value);

  _reg8.value=0;
  _reg8.stereo_threshold = 0x00;
  _reg8.fd_threshold = 0x10;
  _reg8.am_cnr_threshold = 0x01;
  _reg8.fm_cnr_threshold = 0x01;
  I2C_AKC6955_Write_Reg(8,_reg8.value);

  */
  _reg9.value=0;
  _reg9.lv_en = 0;
  _reg9.reserved0 = 1;
  _reg9.oscillator_type = 1;
  _reg9.i2c_volume = 1;
  _reg9.reserved1 = 0;
  I2C_AKC6955_Write_Reg(9,_reg9.value);

  /*
  _reg11.value=0;
  _reg11.fm_seek_skip = 0x10;
  _reg11.reserved1 = 0x03;
  I2C_AKC6955_Write_Reg(11,_reg11.value);

  _reg13.value=0;
  _reg13.predefined_volume = 0x03;
  _reg13.st_led = 0;
  I2C_AKC6955_Write_Reg(13,_reg13.value);
  */
}

void AKC6955_SetVolume(uint32_t volume)
{
  union reg6 _reg6;
  _reg6.value=0;
  _reg6.sound_volume = volume;
  _reg6.line_in=0;
  _reg6.phase_inv=1;
  I2C_AKC6955_Write_Reg(6,_reg6.value);
}

static bool radio_reset=false;

bool AKC6955_SetChannel(bool fm,uint32_t channel)
{
  if(fm)
  {
    union reg0 _reg0;
    _reg0.value=0;
    _reg0.mute = 0;
    _reg0.seek_direction = 1;
    _reg0.go_seek = 0;
    _reg0.tune = 0;
    _reg0.fm_mode = FM;
    _reg0.power_on = 1;
    I2C_AKC6955_Write_Reg(0,_reg0.value);
  }
  else
  {
    union reg0 _reg0;
    _reg0.value=0;
    _reg0.mute = 0;
    _reg0.seek_direction = 1;
    _reg0.go_seek = 0;
    _reg0.tune = 0;
    _reg0.fm_mode = AM;
    _reg0.power_on = 1;
    I2C_AKC6955_Write_Reg(0,_reg0.value);
  }
  
  union reg2 _reg2;
  union reg3 _reg3;
  
  _reg2.mode3k=0;
  _reg2.ref_32kmode=0;
  _reg2.reserved=0;
  _reg2.chanel_freq_high=channel>>8;
  _reg3.chanel_freq_low=channel;
  I2C_AKC6955_Write_Reg(2,_reg2.value);
  I2C_AKC6955_Write_Reg(3,_reg3.value);
  
  {
    union reg0 _reg0;
    union reg20 _reg20;
    
    
    _reg0.value=0;
    _reg0.mute = 0;
    _reg0.seek_direction = 1;
    _reg0.go_seek = 0;
    if(fm)
    {
      _reg0.fm_mode = FM;
    }
    else
    {
      _reg0.fm_mode = AM;
    }
    _reg0.power_on = 1;
    _reg0.tune=1;
    I2C_AKC6955_Write_Reg(0,_reg0.value);
    
    uint32_t timeout=0;
    for(;;)
    {
      I2C_AKC6955_Read_Reg(20,&_reg20.value);
      if(_reg20.finished!=0) 
      {
        return true;
      }
      Delay_Int(1000);
      timeout++;
      if(timeout>6) 
      {
        radio_reset=true;
        return false;
      }
    }
  }
  
  return false;
}

void UpdateVolume(void)
{
  AKC6955_SetVolume(radio_volume);
}



void UpdateChannel(void)
{
  uint32_t freq_khz=0;
  if(radio_channel<6000)
  {
    freq_khz=radio_channel*5;
    AKC6955_SetChannel(false,radio_channel);
  }
  else if(radio_channel<6000+8192)
  {
    freq_khz=30000+(radio_channel-6000)*25;
    AKC6955_SetChannel(true,radio_channel-6000);
  }
  else
  {
    freq_khz=30000+8191*25;
    AKC6955_SetChannel(true,8191);
  }
  DigitDisplaySetNumber(true,freq_khz);
}


int main(void){
  static uint32_t radio_channel_old=0;
  static uint32_t radio_volume_old=0;
  
  SystemInit();
  DigitDisplayInit();
  ExtiInit();
  
  AKC6955_Init();
  
  
  UpdateVolume();
  UpdateChannel();
  
  while (1){
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

    if(radio_reset)
    {
      radio_reset=false;
      AKC6955_Init();
      UpdateVolume();
      UpdateChannel();
    }
    
  }
        
}
