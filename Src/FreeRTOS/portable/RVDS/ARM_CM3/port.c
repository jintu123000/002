/*
 * FreeRTOS Kernel V10.6.2
 * Port for ARM Compiler 5 (armcc) + Cortex-M3
 *
 * This file uses ARM Compiler 5 inline assembly syntax.
 *
 * Implements:
 *  - pxPortInitialiseStack()
 *  - xPortStartScheduler()
 *  - SVC_Handler, PendSV_Handler, SysTick_Handler
 *  - Critical sections
 *  - heap_1 (simple allocator, never frees)
 */

#include "FreeRTOS.h"
#include "task.h"

/*-----------------------------------------------------------*/

/* Constants required to manipulate the NVIC. */
#define portNVIC_SYSTICK_CTRL_REG       (*((volatile uint32_t *)0xE000E010UL))
#define portNVIC_SYSTICK_LOAD_REG       (*((volatile uint32_t *)0xE000E014UL))
#define portNVIC_SYSTICK_CURRENT        (*((volatile uint32_t *)0xE000E018UL))
#define portNVIC_SYSPRI2_REG            (*((volatile uint32_t *)0xE000ED20UL))
#define portNVIC_SYSPRI2_MEM_FAULT_Pos  24UL
#define portNVIC_SYSPRI2_BUS_FAULT_Pos  16UL
#define portNVIC_SYSPRI2_USAGE_FAULT_Pos 8UL

#define portNVIC_INT_CTRL_REG           (*((volatile uint32_t *)0xE000ED04UL))
#define portNVIC_PENDSVSET_BIT          (1UL << 28UL)
#define portNVIC_PENDSVCLEAR_BIT        (1UL << 27UL)

/*-----------------------------------------------------------*/

/*
 * pxPortInitialiseStack — Initialise the stack of a task to look
 * exactly as if a context switch had saved the entire context.
 */
StackType_t * pxPortInitialiseStack(StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters)
{
  /* Simulate the stack frame as it would be created by an exception */
  *pxTopOfStack = 0x01000000UL;                    /* xPSR (Thumb bit set) */
  pxTopOfStack--;
  *pxTopOfStack = (StackType_t)pxCode;             /* PC — task entry point */
  pxTopOfStack--;
  *pxTopOfStack = (StackType_t)0xFFFFFFF9UL;       /* LR (EXC_RETURN to Thread mode, PSP) */
  pxTopOfStack -= 5;                               /* R12, R3, R2, R1, R0 */
  *pxTopOfStack = (StackType_t)pvParameters;       /* R0 = parameter */
  pxTopOfStack -= 8;                               /* R11..R4 */

  return pxTopOfStack;
}

/*-----------------------------------------------------------*/

/*
 * Setup the SysTick timer for the RTOS tick.
 */
__asm void vPortSetupTimerInterrupt(void)
{
  PRESERVE8

  /* SysTick reload = (CPU_CLK / tick_rate) - 1 */
  ldr r0, =0xE000E014      /* SysTick LOAD */
  ldr r1, =0x000007CF      /* (8000000 / 1000) - 1 = 7999? No — configCPU_CLOCK_HZ=8MHz, */
                            /* SysTick uses AHB/8 = 1MHz, 1MHz/1000Hz = 1000, reload = 999 */
  str r1, [r0, #0]

  /* Clear current value */
  ldr r0, =0xE000E018
  mov r1, #0
  str r1, [r0, #0]

  /* Enable SysTick: CLKSOURCE | TICKINT | ENABLE */
  ldr r0, =0xE000E010
  mov r1, #0x07
  str r1, [r0, #0]

  bx lr
  ALIGN
}

/*-----------------------------------------------------------*/

/*
 * Start the first task by restoring its context from the stack.
 */
__asm BaseType_t xPortStartScheduler(void)
{
  PRESERVE8

  /* Set PendSV and SysTick to lowest priority */
  ldr r0, =0xE000ED20     /* System Handler Priority Register 2 */
  ldr r1, [r0, #0]
  orr r1, r1, #0xFF000000 /* PendSV = 0xFF, SysTick = 0xFF (lowest) */
  str r1, [r0, #0]

  /* Start SysTick timer */
  bl vPortSetupTimerInterrupt

  /* Restore the context of the first task */
  ldr r0, =pxCurrentTCB   /* Get address of pxCurrentTCB pointer */
  ldr r1, [r0, #0]        /* Load pxCurrentTCB — points to TCB */
  ldr r0, [r1, #0]        /* Load first item in TCB = pxTopOfStack */

  /* Pop registers from task stack */
  ldmia r0!, {r4-r11}     /* Pop R4-R11 */
  msr psp, r0              /* Set PSP to remaining stack */
  isb

  mov r0, #0
  msr basepri, r0          /* Clear BASEPRI */

  /* The LR value must be EXC_RETURN: 0xFFFFFFFD (return to Thread mode, use PSP) */
  orr r14, r14, #0x0D     /* Set bits [3:0] to return to Thread + PSP */

  bx r14                    /* Return to the first task */
  ALIGN
}

/*-----------------------------------------------------------*/

void vPortEndScheduler(void)
{
  __disable_irq();
  for (;;);
}

/*-----------------------------------------------------------*/

/*
 * SVC Handler — used to yield the CPU.
 */
__asm void SVC_Handler(void)
{
  PRESERVE8
  IMPORT pxCurrentTCB

  ldr r3, =pxCurrentTCB
  ldr r1, [r3]
  ldr r0, [r1]
  ldmia r0!, {r4-r11}
  msr psp, r0
  isb
  mov r0, #0
  msr basepri, r0
  orr r14, r14, #0x0D
  bx r14
  ALIGN
}

/*-----------------------------------------------------------*/

/*
 * PendSV Handler — performs the actual context switch between tasks.
 */
__asm void PendSV_Handler(void)
{
  PRESERVE8
  IMPORT pxCurrentTCB
  IMPORT vTaskSwitchContext

  mrs r0, psp               /* PSP → R0 */
  isb

  ldr r3, =pxCurrentTCB     /* &pxCurrentTCB */
  ldr r2, [r3]              /* pxCurrentTCB → R2 */

  stmdb r0!, {r4-r11}       /* Save R4-R11 on task stack */
  str r0, [r2]              /* Save top of stack to TCB */

  stmdb sp!, {r3, r14}      /* Save R3 (TCB ptr) and R14 (EXC_RETURN) on main stack */

  mov r0, #0xF0              /* configMAX_SYSCALL_INTERRUPT_PRIORITY << (8-4) = 191 << 4 */
  msr basepri, r0            /* Mask interrupts */
  dsb
  isb

  bl vTaskSwitchContext      /* Select next task to run */

  mov r0, #0
  msr basepri, r0            /* Unmask interrupts */

  ldmia sp!, {r3, r14}       /* Restore R3 and LR */

  ldr r1, [r3]               /* pxCurrentTCB → R1 (new task) */
  ldr r0, [r1]               /* TCB->pxTopOfStack → R0 */

  ldmia r0!, {r4-r11}        /* Pop R4-R11 from new task's stack */
  msr psp, r0                /* PSP → new task's stack */
  isb

  bx r14                      /* Return to thread mode */
  ALIGN
}

/*-----------------------------------------------------------*/

/*
 * SysTick Handler — the RTOS tick interrupt.
 */
__asm void SysTick_Handler(void)
{
  PRESERVE8
  IMPORT pxCurrentTCB
  IMPORT xTaskIncrementTick
  IMPORT vTaskSwitchContext

  mrs r0, psp
  isb

  ldr r3, =pxCurrentTCB
  ldr r2, [r3]

  stmdb r0!, {r4-r11}
  str r0, [r2]

  stmdb sp!, {r3, r14}

  mov r0, #0xF0
  msr basepri, r0
  dsb
  isb

  bl xTaskIncrementTick

  cmp r0, #0
  beq skip_context_switch
  bl vTaskSwitchContext

skip_context_switch
  mov r0, #0
  msr basepri, r0

  ldmia sp!, {r3, r14}

  ldr r1, [r3]
  ldr r0, [r1]

  ldmia r0!, {r4-r11}
  msr psp, r0
  isb

  bx r14
  ALIGN
}

/*-----------------------------------------------------------*/

/*
 * Critical section functions for ARM Compiler 5.
 * These are simple wrappers — the macros call __disable_irq/__enable_irq.
 */
void vPortEnterCritical(void)
{
  __disable_irq();
}

void vPortExitCritical(void)
{
  __enable_irq();
}

uint32_t ulPortSetInterruptMask(void)
{
  uint32_t ulState;
  ulState = __get_PRIMASK();
  __disable_irq();
  return ulState;
}

void vPortClearInterruptMask(uint32_t ulNewMask)
{
  __set_PRIMASK(ulNewMask);
}

/*-----------------------------------------------------------*/

/*
 * Simple heap_1 implementation (never frees memory).
 * Suitable for this LED blink project — tasks are created once.
 */

#define configTOTAL_HEAP_SIZE 4096

static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
static size_t xFreeBytesRemaining = configTOTAL_HEAP_SIZE;

void * pvPortMalloc(size_t xWantedSize)
{
  void * pvReturn = NULL;

  __disable_irq();

  /* Align to 8 bytes */
  if ((xWantedSize & 0x07U) != 0U)
  {
    xWantedSize = (xWantedSize + 8U) & ~0x07U;
  }

  if (xFreeBytesRemaining >= xWantedSize)
  {
    pvReturn = &ucHeap[configTOTAL_HEAP_SIZE - xFreeBytesRemaining];
    xFreeBytesRemaining -= xWantedSize;
  }

  __enable_irq();

  return pvReturn;
}

void vPortFree(void * pv)
{
  /* heap_1: never frees memory */
  (void)pv;
}

size_t xPortGetFreeHeapSize(void)
{
  return xFreeBytesRemaining;
}
