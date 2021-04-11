
#ifndef __DIGITDISPLAY_H__
#define __DIGITDISPLAY_H__


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


void DigitDisplayInit(void);


void DigitDisplaySetNumber(bool enable,uint32_t freq_khz);



#endif


