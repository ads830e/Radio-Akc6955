
#ifndef __FEE_H__
#define __FEE_H__


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define FEE_NVM_BLOCK_SIZE 12


void Fee_Init(void);
bool Fee_Read(void);
void Fee_Write(void);

extern uint8_t Fee_Buf[FEE_NVM_BLOCK_SIZE];

#endif


