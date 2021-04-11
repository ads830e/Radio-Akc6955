
#include <string.h>

#include "stm32f0xx.h"
#include "fee.h"




#define STM32F0xx_PAGE_SIZE 0x400
#define STM32F0xx_FLASH_PAGE0_STARTADDR 0x8000000
#define STM32F0xx_FLASH_PAGE1_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE2_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+2*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE3_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+3*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE4_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+4*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE5_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+5*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE6_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+6*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE7_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+7*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE8_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+8*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE9_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+9*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE10_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+10*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE11_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+11*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE12_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+12*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE13_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+13*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE14_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+14*STM32F0xx_PAGE_SIZE)
#define STM32F0xx_FLASH_PAGE15_STARTADDR (STM32F0xx_FLASH_PAGE0_STARTADDR+15*STM32F0xx_PAGE_SIZE)


#define FEE_START_ADDR (STM32F0xx_FLASH_PAGE12_STARTADDR)
#define FEE_SIZE (STM32F0xx_PAGE_SIZE*4)



uint8_t Fee_Buf[FEE_NVM_BLOCK_SIZE]={0};



static uint32_t Fee_CalcNvmElementSize(uint32_t nvmblocksize)
{
    if(nvmblocksize>1024-4) for(;;);//error
  
    uint32_t nvmblocksize_1=nvmblocksize+2/*header 0xaa 0x55*/+2/*crc16*/;
    
    uint32_t nvmblocksize_2=4;
    if(nvmblocksize_1>512) nvmblocksize_2=1024;
    else if(nvmblocksize_1>256) nvmblocksize_2=512;
    else if(nvmblocksize_1>128) nvmblocksize_2=256;
    else if(nvmblocksize_1>64) nvmblocksize_2=128;
    else if(nvmblocksize_1>32) nvmblocksize_2=64;
    else if(nvmblocksize_1>16) nvmblocksize_2=32;
    else if(nvmblocksize_1>8) nvmblocksize_2=16;
    else if(nvmblocksize_1>4) nvmblocksize_2=8;
    else nvmblocksize_2=4;
      
    return nvmblocksize_2;
}


void Fee_Init(void)
{
  for(uint32_t index=0;index<FEE_NVM_BLOCK_SIZE;index++)
  {
    Fee_Buf[index]=0;
  }
}

static bool Fee_IsElementEmpty(uint8_t *elementaddr)
{
  bool ret=true;
  for(uint32_t index=0;index<FEE_NVM_BLOCK_SIZE;index++)
  {
    if(elementaddr[index]!=0xff)
    {
      ret=false;
    }
  }
  return ret;
}

static bool Fee_IsElementValid(uint8_t *elementaddr)
{
  if(elementaddr[0]!=0xaa) return false;
  if(elementaddr[1]!=0x55) return false;
  
  return true;
}

static bool Fee_IsErrorExist(void)
{
  uint32_t nvmelementsize=Fee_CalcNvmElementSize(FEE_NVM_BLOCK_SIZE);
  
  bool emptyflag=false;
  
  for(uint32_t index=0;index<(FEE_SIZE/nvmelementsize);index++)
  {
    uint8_t *elementaddr=(uint8_t *)(FEE_START_ADDR+nvmelementsize*index);
    
    if(Fee_IsElementEmpty(elementaddr))
    {
       emptyflag=true;
        continue;
    }
    else
    {
      if(emptyflag)
      {
        //non-empty after empty detected,need erase
        return true;
      }
    }
    
    if(!Fee_IsElementValid(elementaddr))
     {
        //detect error,need erase
        return true;
     }
  }
  
  return false;
}

static bool Fee_IsFull(void)
{
  uint32_t nvmelementsize=Fee_CalcNvmElementSize(FEE_NVM_BLOCK_SIZE);
  
  uint32_t maxelementcnt=FEE_SIZE/nvmelementsize;
  
  if(maxelementcnt<=0) for(;;);//error
  
  if(Fee_IsElementEmpty((uint8_t *)(FEE_START_ADDR+nvmelementsize*(maxelementcnt-1))))
  {
    return false;
  }
  else
  {
    return true;
  }
  
}



bool Fee_Read(void)
{
  uint32_t nvmelementsize=Fee_CalcNvmElementSize(FEE_NVM_BLOCK_SIZE);
  
  int32_t lastelementindex=-1;
  for(uint32_t index=0;index<(FEE_SIZE/nvmelementsize);index++)
  {
    uint8_t *elementaddr=(uint8_t *)(FEE_START_ADDR+nvmelementsize*index);
    
    if(Fee_IsElementEmpty(elementaddr))
    {
      break;
    }
    
    if(!Fee_IsElementValid(elementaddr))
    {
      //detect error,need erase
      return false;
    }
    
    lastelementindex=index;
  }
  
  if(lastelementindex<0)
  {
    return false;//empty fee
  }
  
  uint8_t *lastelementaddr=(uint8_t *)(FEE_START_ADDR+nvmelementsize*lastelementindex);
  
  memcpy(&Fee_Buf[0],&lastelementaddr[4],FEE_NVM_BLOCK_SIZE);
  
  return true;
}


static void Fee_Erase(void)
{  
  FLASH_Unlock();    

  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR );
  
  FLASH_ErasePage(STM32F0xx_FLASH_PAGE12_STARTADDR);
  FLASH_ErasePage(STM32F0xx_FLASH_PAGE13_STARTADDR);
  FLASH_ErasePage(STM32F0xx_FLASH_PAGE14_STARTADDR);
  FLASH_ErasePage(STM32F0xx_FLASH_PAGE15_STARTADDR);
  
  FLASH_Lock();
}

static void Fee_WriteElement(uint8_t *elementaddr)
{
  FLASH_Unlock();    

  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR );
  
  
  uint32_t addr_u32=(uint32_t)elementaddr;
  
  FLASH_ProgramHalfWord( addr_u32, 0x55aa /*lsb*/ );
  FLASH_ProgramHalfWord( addr_u32+2, 0x0000 /*lsb,crc16*/ );
  
  for(uint32_t index=0;index<(FEE_NVM_BLOCK_SIZE+1)/2;index++)
  {
    uint16_t tmp_u16=0;
    
    tmp_u16=Fee_Buf[index*2];
    
    if(index*2+1<FEE_NVM_BLOCK_SIZE)
    {
      tmp_u16|=((uint16_t)Fee_Buf[index*2+1])<<8;
    }
      
    FLASH_ProgramHalfWord( addr_u32+4+index*2, tmp_u16 );
  }
  
  FLASH_Lock();
}

static bool Fee_WriteNotNeeded()
{
  uint32_t nvmelementsize=Fee_CalcNvmElementSize(FEE_NVM_BLOCK_SIZE);
  
  int32_t lastelementindex=-1;
  for(uint32_t index=0;index<(FEE_SIZE/nvmelementsize);index++)
  {
    uint8_t *elementaddr=(uint8_t *)(FEE_START_ADDR+nvmelementsize*index);
    
    if(Fee_IsElementEmpty(elementaddr))
    {
      break;
    }
    
    if(!Fee_IsElementValid(elementaddr))
    {
      //detect error,need erase
      return false;
    }
    
    lastelementindex=index;
  }
  
  if(lastelementindex<0)
  {
    return false;//empty fee
  }
  
  uint8_t *lastelementaddr=(uint8_t *)(FEE_START_ADDR+nvmelementsize*lastelementindex);
  
  if(memcmp(&Fee_Buf[0],&lastelementaddr[4],FEE_NVM_BLOCK_SIZE)==0)
  {
    return true;
  }
  else
  {
    return false;
  }
}


void Fee_Write(void)
{
  if(Fee_WriteNotNeeded())
  {
    return ;
  }
  
  if(Fee_IsErrorExist()) 
  {
    Fee_Erase();
  }
  
  if(Fee_IsFull()) 
  {
    Fee_Erase();
  }
  
  uint32_t nvmelementsize=Fee_CalcNvmElementSize(FEE_NVM_BLOCK_SIZE);
  
  for(uint32_t index=0;index<(FEE_SIZE/nvmelementsize);index++)
  {
    uint8_t *elementaddr=(uint8_t *)(FEE_START_ADDR+nvmelementsize*index);
    
    if(Fee_IsElementEmpty(elementaddr))
    {
      
      Fee_WriteElement(elementaddr);
      
      break;
    }
  }
  return;
}











