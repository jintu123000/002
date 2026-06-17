/*
 * FreeRTOS Kernel V10.6.2
 * Port for Cortex-M3 (STM32F103)
 *
 * Supports:
 *   - GCC (arm-none-eabi-gcc)
 *   - ARM Compiler 5 (armcc — Keil MDK)
 *   - ARM Compiler 6 (armclang — Keil MDK, acts like GCC)
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "FreeRTOSConfig.h"

/*-----------------------------------------------------------*/

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

/*-----------------------------------------------------------*/

/* Architecture specifics */
#define portBYTE_ALIGNMENT             8
#define portSTACK_GROWTH               (-1)
#define portTICK_PERIOD_MS             ((TickType_t)1000 / configTICK_RATE_HZ)
#define portHAS_STACK_OVERFLOW_CHECKING 1

/* Convert ms to ticks */
#define pdMS_TO_TICKS(xTimeInMs) \
  (((TickType_t)(xTimeInMs) * configTICK_RATE_HZ) / (TickType_t)1000)

/*-----------------------------------------------------------*/

/* Task function type (forward declared here, used by pxPortInitialiseStack) */
typedef void (*TaskFunction_t)(void *);

/*-----------------------------------------------------------*/

/*
 * Critical section management.
 *
 * ARM Compiler 5 (armcc) — use __disable_irq / __enable_irq (CMSIS intrinsics)
 * GCC / ARM Compiler 6 (armclang) — use inline asm or external functions
 */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION < 6000000)
  /* ARM Compiler 5 (armcc) */

  #define portDISABLE_INTERRUPTS()      __disable_irq()
  #define portENABLE_INTERRUPTS()       __enable_irq()
  #define portENTER_CRITICAL()          __disable_irq()
  #define portEXIT_CRITICAL()           __enable_irq()

  #define portSET_INTERRUPT_MASK_FROM_ISR()        0; __disable_irq()
  #define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)      (void)(x); __enable_irq()

  #define portYIELD()                   __asm { svc 0 }

  #define portYIELD_FROM_ISR(x)         portEND_SWITCHING_ISR(x)
  #define portEND_SWITCHING_ISR(x)      \
    do {                                \
      if ((x) != pdFALSE) {             \
        portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT; \
      }                                 \
    } while(0)

  #define portSUPPRESS_TICKS_AND_SLEEP(x) ((void)(x))

  /* port.c implements the below for ARMCC */
  void vPortEnterCritical(void);
  void vPortExitCritical(void);
  uint32_t ulPortSetInterruptMask(void);
  void vPortClearInterruptMask(uint32_t ulNewMask);

#else
  /* GCC / ARM Compiler 6 (armclang) / other GCC-compatible */

  #define portDISABLE_INTERRUPTS()      __asm volatile ("cpsid i" ::: "memory")
  #define portENABLE_INTERRUPTS()       __asm volatile ("cpsie i" ::: "memory")
  #define portENTER_CRITICAL()          vPortEnterCritical()
  #define portEXIT_CRITICAL()           vPortExitCritical()

  extern void vPortEnterCritical(void);
  extern void vPortExitCritical(void);

  #define portSET_INTERRUPT_MASK_FROM_ISR()       ulPortSetInterruptMask()
  #define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)    vPortClearInterruptMask(x)

  extern uint32_t ulPortSetInterruptMask(void);
  extern void vPortClearInterruptMask(uint32_t ulNewMask);

  #define portYIELD()                   \
    __asm volatile ("svc 0" ::: "memory")

  #define portYIELD_FROM_ISR(x)         portEND_SWITCHING_ISR(x)
  #define portEND_SWITCHING_ISR(x)      \
    do {                                \
      if ((x) != pdFALSE) {             \
        portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT; \
      }                                 \
    } while(0)

  #define portSUPPRESS_TICKS_AND_SLEEP(x) ((void)(x))
#endif

/*-----------------------------------------------------------*/

/* Task function macro */
#define portTASK_FUNCTION_PROTO(vFunction, pvParameters) \
  void vFunction(void * pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters) \
  void vFunction(void * pvParameters)

/*-----------------------------------------------------------*/

/* NVIC registers (same for all compilers) */
#define portNVIC_INT_CTRL_REG       (*((volatile uint32_t *)0xE000ED04UL))
#define portNVIC_PENDSVSET_BIT      (1UL << 28UL)
#define portNVIC_PENDSVCLEAR_BIT    (1UL << 27UL)

#define portNVIC_SYSTICK_CTRL_REG   (*((volatile uint32_t *)0xE000E010UL))
#define portNVIC_SYSTICK_LOAD_REG   (*((volatile uint32_t *)0xE000E014UL))
#define portNVIC_SYSTICK_CURRENT    (*((volatile uint32_t *)0xE000E018UL))

#define portNVIC_SYSTICK_CLK_BIT    (1UL << 2UL)
#define portNVIC_SYSTICK_INT_BIT    (1UL << 1UL)
#define portNVIC_SYSTICK_ENABLE_BIT (1UL << 0UL)

/*-----------------------------------------------------------*/

/* Memory allocation (implemented in port.c) */
void * pvPortMalloc(size_t xWantedSize);
void vPortFree(void * pv);

/* Stack initialization (implemented in port.c) */
StackType_t * pxPortInitialiseStack(StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters);

/* Scheduler start/end (port-specific, implemented in port.c) */
BaseType_t xPortStartScheduler(void);
void vPortEndScheduler(void);

/* From tasks.c — called from assembly ISRs in port.c */
void vTaskSwitchContext(void);
void xTaskIncrementTick(void);

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
