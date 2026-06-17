/*
 * FreeRTOS Kernel V10.6.2
 * Port for GCC + Cortex-M3 (STM32F103)
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "FreeRTOSConfig.h"

/* Type definitions */
#define portCHAR          char
#define portFLOAT         float
#define portDOUBLE        double
#define portLONG          long
#define portSHORT         short
#define portSTACK_TYPE    uint32_t
#define portBASE_TYPE     long

typedef portSTACK_TYPE   StackType_t;
typedef long             BaseType_t;
typedef unsigned long    UBaseType_t;

#if (configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS)
  typedef uint16_t       TickType_t;
  #define portMAX_DELAY  (TickType_t)0xffffUL
#elif (configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_32_BITS)
  typedef uint32_t       TickType_t;
  #define portMAX_DELAY  (TickType_t)0xffffffffUL
#else
  #error "configTICK_TYPE_WIDTH_IN_BITS must be 16 or 32"
#endif

/* Architecture specifics */
#define portBYTE_ALIGNMENT         8
#define portSTACK_GROWTH           (-1)
#define portTICK_PERIOD_MS         ((TickType_t)1000 / configTICK_RATE_HZ)
#define portNUM_CONFIGURABLE_REGIONS 0
#define portHAS_STACK_OVERFLOW_CHECKING 1

/* Convert milliseconds to ticks */
#define pdMS_TO_TICKS(xTimeInMs)   (((TickType_t)(xTimeInMs) * configTICK_RATE_HZ) / (TickType_t)1000)

/* Critical section management */
#define portDISABLE_INTERRUPTS()        __asm volatile ("cpsid i" ::: "memory")
#define portENABLE_INTERRUPTS()         __asm volatile ("cpsie i" ::: "memory")
#define portENTER_CRITICAL()            vPortEnterCritical()
#define portEXIT_CRITICAL()             vPortExitCritical()

extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);

#define portSET_INTERRUPT_MASK_FROM_ISR()         ulPortSetInterruptMask()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)      vPortClearInterruptMask(x)

extern uint32_t ulPortSetInterruptMask(void);
extern void vPortClearInterruptMask(uint32_t ulNewMask);

/* Yield */
#define portYIELD()                     \
  do {                                  \
    __asm volatile ("svc 0");           \
  } while(0)

/* NVIC / SysTick utilities */
#define portNVIC_INT_CTRL_REG           (*((volatile uint32_t *)0xE000ED04UL))
#define portNVIC_PENDSVSET_BIT          (1UL << 28UL)

#define portYIELD_FROM_ISR(x)           portEND_SWITCHING_ISR(x)
#define portEND_SWITCHING_ISR(x)        \
  do {                                  \
    if ((x) != pdFALSE) {               \
      portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT; \
    }                                   \
  } while(0)

/* Suppress compiler warning */
#define portSUPPRESS_TICKS_AND_SLEEP(x) ((void)(x))

/* Task function macro */
#define portTASK_FUNCTION_PROTO(vFunction, pvParameters)   void vFunction(void * pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters)         void vFunction(void * pvParameters)

/* Forward declaration */
typedef void (*TaskFunction_t)(void *);

/* Memory allocation */
void * pvPortMalloc(size_t xWantedSize);
void vPortFree(void * pv);

/* Stack initialisation function */
StackType_t * pxPortInitialiseStack(StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters);

/* Scheduler start/end (port-specific) */
BaseType_t xPortStartScheduler(void);
void vPortEndScheduler(void);

/* vTaskSwitchContext — defined in tasks.c, called from asm in port.c */
void vTaskSwitchContext(void);

/* xTaskIncrementTick — defined in tasks.c, called from SysTick ISR in port.c */
void xTaskIncrementTick(void);

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
