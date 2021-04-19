#ifndef OS_EXCEP_H_
#define OS_EXCEP_H_

#if defined(__cplusplus)
extern "C"
{
#endif

/* Include uC-OS3 header for having uC-OS systickhandler and pendsvhandler functions */
#include "os.h"

/* Use macros to replace SysTick_Handler and PendSV_Handler in vectors_stm32l475xx.c */
#define SysTick_Handler OS_CPU_SysTickHandler
#define PendSV_Handler OS_CPU_PendSVHandler

#if defined(__cplusplus)
}
#endif

#endif // end of OS_EXCEP_H_
