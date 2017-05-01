#ifndef __LOG_H
#define __LOG_H 	

#include "sdio_sdcard.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include "sram.h"     
#include "malloc.h"   
#include "ff.h"  
#include "exfuns.h"
#include "fattester.h"	

u32 filelog(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif
