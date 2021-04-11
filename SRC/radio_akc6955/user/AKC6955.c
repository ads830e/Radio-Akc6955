
#include "AKC6955.H"

#include "stm32f0xx.h"





#define  I2C_AKC6955_Address 0x10
const uint32_t I2C_TIMEOUT_MAX=0x1000;



static bool radio_need_reset=false;



static void I2C_AKC6955_Delay(void)
{
  //Delay_Int(0);
}

static void AKC6955_Delay_Int(unsigned int delay){
	 while(delay--);
}

static void I2C_AKC6955_Init(void)
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
  
  AKC6955_Delay_Int(100);
  
  GPIO_SetBits(GPIOB,GPIO_Pin_1);
  
  AKC6955_Delay_Int(100);
}

static void I2C_AKC6955_SDA_IN(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
}


static void I2C_AKC6955_SDA_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
}

static bool I2C_AKC6955_SDA_Read(void)
{
  return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10)!=0;
}

static void I2C_AKC6955_Start(void)
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


static void I2C_AKC6955_Stop(void)
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


static void I2C_AKC6955_SendData(uint8_t data)
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

static void I2C_AKC6955_ReadData(uint8_t *data)
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


static bool I2C_AKC6955_WaitAck(void)
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


static bool I2C_AKC6955_WaitNack(void)
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

static void I2C_AKC6955_Ack(void)
{
  I2C_AKC6955_SDA_OUT();
  GPIO_ResetBits(GPIOA,GPIO_Pin_10);//sda
  I2C_AKC6955_Delay();
  GPIO_SetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
  GPIO_ResetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
}

static void I2C_AKC6955_Nack(void)
{
  I2C_AKC6955_SDA_OUT();
  GPIO_SetBits(GPIOA,GPIO_Pin_10);//sda
  I2C_AKC6955_Delay();
  GPIO_SetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
  GPIO_ResetBits(GPIOA,GPIO_Pin_9);//sclk
  I2C_AKC6955_Delay();
}


static bool I2C_AKC6955_Read_Reg(uint8_t address,uint8_t *data)
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


static bool I2C_AKC6955_Write_Reg(uint8_t address,uint8_t data)
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



void AKC6955_Init(void)
{
  I2C_AKC6955_Init();
  
  union reg0 _reg0;
  union reg1 _reg1;
  union reg2 _reg2;
  
  union reg6 _reg6;
  //union reg7 _reg7;
  //union reg8 _reg8;
  union reg9 _reg9;
  
  //union reg11 _reg11;
  //union reg13 _reg13;
  
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
      AKC6955_Delay_Int(1000);
      timeout++;
      if(timeout>6) 
      {
        radio_need_reset=true;
        return false;
      }
    }
  }
  
  //return false;
}

bool AKC6955_IsResetNeeded(void)
{
  bool ret=radio_need_reset;
  radio_need_reset=false;
  return ret;
}



