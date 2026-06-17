/*
 * FreeRTOS Kernel V10.6.2
 */

#ifndef PORTABLE_H
#define PORTABLE_H

#include "portmacro.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Each port implements these */
StackType_t * pxPortInitialiseStack(StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters);

BaseType_t xPortStartScheduler(void);

void vPortEndScheduler(void);

void vPortYield(void);

void vPortEnterCritical(void);

void vPortExitCritical(void);

uint32_t ulPortSetInterruptMask(void);

void vPortClearInterruptMask(uint32_t ulNewMask);

#ifdef __cplusplus
}
#endif

#endif /* PORTABLE_H */
