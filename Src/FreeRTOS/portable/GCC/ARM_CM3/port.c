/*
 * FreeRTOS Kernel V10.6.2
 * Port for GCC + ARM Cortex-M3 (STM32F103)
 *
 * Implements:
 *  - pxPortInitialiseStack()
 *  - xPortStartScheduler()
 *  - vPortEndScheduler()
 *  - SVC_Handler, PendSV_Handler, SysTick_Handler
 *  - Critical section management
 *  - Memory allocation (heap_1: static allocation, no free)
 */

#include "FreeRTOS.h"
#include "task.h"

/* Macros used to manipulate the NVIC bits */
#ifndef portNVIC_SYSTICK_CTRL_REG
  #define portNVIC_SYSTICK_CTRL_REG    (*((volatile uint32_t *)0xE000E010UL))
#endif
#ifndef portNVIC_SYSTICK_LOAD_REG
  #define portNVIC_SYSTICK_LOAD_REG    (*((volatile uint32_t *)0xE000E014UL))
#endif
#ifndef portNVIC_SYSTICK_CURRENT
  #define portNVIC_SYSTICK_CURRENT     (*((volatile uint32_t *)0xE000E018UL))
#endif
#ifndef portNVIC_INT_CTRL_REG
  #define portNVIC_INT_CTRL_REG        (*((volatile uint32_t *)0xE000ED04UL))
#endif
#ifndef portNVIC_PENDSVSET_BIT
  #define portNVIC_PENDSVSET_BIT       (1UL << 28UL)
#endif
#ifndef portNVIC_PENDSVCLEAR_BIT
  #define portNVIC_PENDSVCLEAR_BIT     (1UL << 27UL)
#endif

#define portNVIC_SYSTICK_CLK_BIT       (1UL << 2UL)
#define portNVIC_SYSTICK_INT_BIT       (1UL << 1UL)
#define portNVIC_SYSTICK_ENABLE_BIT    (1UL << 0UL)

/*-----------------------------------------------------------*/

/* Each task maintains its own interrupt status in the critical nesting variable. */
static UBaseType_t uxCriticalNesting = 0xAAAAAAAAUL;

/*
 * The maximum number of tasks at (configMAX_PRIORITIES) so the idle task can
 * be included.
 */
static volatile uint32_t ulCriticalNesting;

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t * pxPortInitialiseStack(StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters)
{
  /* Simulate the stack frame as it would be created by a context switch interrupt */

  /* Offset the stack to align it */
  pxTopOfStack--;

  *pxTopOfStack = 0x01000000UL;    /* xPSR — Thumb bit set */
  pxTopOfStack--;
  *pxTopOfStack = (StackType_t)pxCode;   /* PC (task entry point) */
  pxTopOfStack--;
  *pxTopOfStack = (StackType_t)0xFFFFFFF9UL;  /* LR (EXC_RETURN to thread mode, use PSP) */
  pxTopOfStack -= 5;              /* R12, R3, R2, R1, R0 */
  *pxTopOfStack = (StackType_t)pvParameters;  /* R0 = task parameter */
  pxTopOfStack -= 8;              /* R11..R4 */
  pxTopOfStack--;                 /* R14 (LR) */
  *pxTopOfStack = (StackType_t)0xFFFFFFF9UL;  /* EXC_RETURN */
  pxTopOfStack--;                 /* R13 (SP) — not stored */

  return pxTopOfStack;
}

/*-----------------------------------------------------------*/

void vPortSetupTimerInterrupt(void)
{
  /* Stop and clear SysTick */
  portNVIC_SYSTICK_CTRL_REG = 0UL;
  portNVIC_SYSTICK_CURRENT = 0UL;

  /* Calculate reload value:
   * SysTick clock = CPU clock / 8 (AHB clock at /1, SysTick uses AHB/8)
   * configCPU_CLOCK_HZ = 8000000
   * Tick rate = 1000 Hz
   * Reload = (8000000 / 8) / 1000 - 1 = 1000 - 1 = 999
   */
  portNVIC_SYSTICK_LOAD_REG = (configCPU_CLOCK_HZ / configTICK_RATE_HZ) - 1UL;
  portNVIC_SYSTICK_CURRENT = 0UL;

  /* Enable SysTick: use processor clock (AHB/8 is implicit), enable interrupt, enable counter */
  portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK_BIT |
                               portNVIC_SYSTICK_INT_BIT |
                               portNVIC_SYSTICK_ENABLE_BIT;
}

/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler(void)
{
  /* Set PendSV and SysTick to the lowest priority */
  /* SCB->SHP: SysTick at index 11, PendSV at index 10 */
  *(volatile uint32_t *)0xE000ED20UL = (*(volatile uint32_t *)0xE000ED20UL) |
                                        ((uint32_t)0xFFUL << 16UL); /* SYSTICK + PENDSV = 0xFF */

  /* Start the timer that generates the tick ISR */
  vPortSetupTimerInterrupt();

  /* Initialize the critical nesting count ready for the first task */
  uxCriticalNesting = 0UL;

  /* Start the first task */
  __asm volatile (
    "   ldr r0, =0xE000ED08      \n"  /* VTOR address */
    "   ldr r0, [r0]              \n"  /* Vector table offset */
    "   ldr r0, [r0]              \n"  /* Load initial MSP from vector table */
    "   msr msp, r0               \n"  /* Set MSP */
    "   cpsie i                   \n"  /* Enable interrupts */
    "   cpsie f                   \n"
    "   dsb                       \n"
    "   isb                       \n"
    "   svc 0                     \n"  /* Trigger SVC to start first task */
    ::: "r0"
  );

  /* Should not return! */
  return 0;
}

/*-----------------------------------------------------------*/

void vPortEndScheduler(void)
{
  /* Not implemented — on embedded targets this function is generally unused */
  portDISABLE_INTERRUPTS();
  for (;;);
}

/*-----------------------------------------------------------*/

/*
 * Enter a critical section — disable interrupts and increment nesting count.
 */
void vPortEnterCritical(void)
{
  portDISABLE_INTERRUPTS();
  uxCriticalNesting++;

  if (uxCriticalNesting == 1U)
  {
    /* First nesting level — interrupts are now disabled. */
  }
}

/*-----------------------------------------------------------*/

/*
 * Exit a critical section — decrement nesting count, re-enable
 * interrupts when nesting returns to 0.
 */
void vPortExitCritical(void)
{
  if (uxCriticalNesting == 0U)
  {
    /* Should never try to exit a critical section when not in one. */
    return;
  }

  uxCriticalNesting--;

  if (uxCriticalNesting == 0U)
  {
    portENABLE_INTERRUPTS();
  }
}

/*-----------------------------------------------------------*/

uint32_t ulPortSetInterruptMask(void)
{
  uint32_t ulState;

  __asm volatile ("mrs %0, primask" : "=r" (ulState) :: "memory");
  portDISABLE_INTERRUPTS();
  return ulState;
}

/*-----------------------------------------------------------*/

void vPortClearInterruptMask(uint32_t ulNewMask)
{
  if (ulNewMask == 0U)
  {
    portENABLE_INTERRUPTS();
  }
}

/*-----------------------------------------------------------*/

/*
 * SVC Handler — used to start the first task.
 * This is called at the very beginning of the scheduler.
 */
void SVC_Handler(void) __attribute__((naked));
void SVC_Handler(void)
{
  __asm volatile (
    "   ldr r3, =pxCurrentTCB       \n"  /* Load address of pxCurrentTCB */
    "   ldr r1, [r3]                 \n"  /* Load pxCurrentTCB value */
    "   ldr r0, [r1]                 \n"  /* Load first stack item (pxTopOfStack) */
    "   ldmia r0!, {r4-r11}          \n"  /* Pop registers R4-R11 */
    "   msr psp, r0                   \n"  /* Set PSP to new top of stack */
    "   isb                           \n"
    "   mov r0, #0                    \n"
    "   msr basepri, r0               \n"  /* Clear BASEPRI */
    "   bx lr                         \n"  /* Return to thread mode */
    ::: "r0", "r1", "r3"
  );
}

/*-----------------------------------------------------------*/

/*
 * PendSV Handler — performs the context switch.
 * This is called from vPortYield() and the tick ISR to switch tasks.
 */
void PendSV_Handler(void) __attribute__((naked));
void PendSV_Handler(void)
{
  __asm volatile (
    "   mrs r0, psp                  \n"  /* Get PSP (current task's stack pointer) */
    "   isb                          \n"
    "   ldr r3, =pxCurrentTCB        \n"  /* Get the address of pxCurrentTCB */
    "   ldr r2, [r3]                 \n"  /* Get the TCB */
    "   stmdb r0!, {r4-r11}          \n"  /* Save remaining registers onto the task stack */
    "   str r0, [r2]                 \n"  /* Save new top of stack in the TCB */
    "   stmdb sp!, {r3, r14}         \n"  /* Save R3 and LR (EXC_RETURN) */
    "   mov r0, %0                   \n"  /* Enter critical section */
    "   msr basepri, r0              \n"
    "   bl vTaskSwitchContext         \n"  /* Select next task to run */
    "   mov r0, #0                    \n"  /* Exit critical section */
    "   msr basepri, r0              \n"
    "   ldmia sp!, {r3, r14}         \n"  /* Restore R3 and LR */
    "   ldr r1, [r3]                 \n"  /* Load pxCurrentTCB */
    "   ldr r0, [r1]                 \n"  /* Load top of stack from new TCB */
    "   ldmia r0!, {r4-r11}          \n"  /* Pop registers R4-R11 */
    "   msr psp, r0                   \n"  /* Set PSP to new top of stack */
    "   isb                           \n"
    "   bx r14                        \n"  /* Return to thread mode */
    :: "i" (configMAX_SYSCALL_INTERRUPT_PRIORITY)
    : "r0", "r1", "r2", "r3"
  );
}

/*-----------------------------------------------------------*/

/*
 * SysTick Handler — the RTOS tick.
 * Increments the tick count and checks if a context switch is needed.
 */
void SysTick_Handler(void) __attribute__((naked));
void SysTick_Handler(void)
{
  __asm volatile (
    "   mrs r0, psp                  \n"
    "   isb                          \n"
    "   ldr r3, =pxCurrentTCB        \n"
    "   ldr r2, [r3]                 \n"
    "   stmdb r0!, {r4-r11}          \n"  /* Save context */
    "   str r0, [r2]                 \n"  /* Save SP */

    "   mov r0, %0                   \n"  /* Enter critical section */
    "   msr basepri, r0              \n"
    "   bl xTaskIncrementTick        \n"  /* Increment tick */
    "   cmp r0, #0                    \n"  /* Context switch needed? */
    "   beq skip_switch              \n"
    "   bl vTaskSwitchContext         \n"  /* Switch to next task */
    "skip_switch:                     \n"
    "   mov r0, #0                    \n"  /* Exit critical section */
    "   msr basepri, r0              \n"

    "   ldr r1, [r3]                 \n"  /* Load pxCurrentTCB */
    "   ldr r0, [r1]                 \n"  /* Load new top of stack */
    "   ldmia r0!, {r4-r11}          \n"  /* Restore context */
    "   msr psp, r0                   \n"
    "   isb                           \n"
    "   bx r14                        \n"
    :: "i" (configMAX_SYSCALL_INTERRUPT_PRIORITY)
    : "r0", "r1", "r2", "r3"
  );
}

/*-----------------------------------------------------------*/

/*
 * Simple heap implementation (heap_1.c style)
 * Allocates from a static array, never frees.
 * Suitable for applications that create tasks once and never delete them.
 */

#define configTOTAL_HEAP_SIZE 4096

static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
static size_t xFreeBytesRemaining = configTOTAL_HEAP_SIZE;

void * pvPortMalloc(size_t xWantedSize)
{
  void * pvReturn = NULL;

  vPortEnterCritical();

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

  vPortExitCritical();

  return pvReturn;
}

/*-----------------------------------------------------------*/

void vPortFree(void * pv)
{
  /* heap_1: never frees */
  (void)pv;
}

/*-----------------------------------------------------------*/

void vPortInitialiseBlocks(void)
{
  xFreeBytesRemaining = configTOTAL_HEAP_SIZE;
}

/*-----------------------------------------------------------*/

size_t xPortGetFreeHeapSize(void)
{
  return xFreeBytesRemaining;
}

/*-----------------------------------------------------------*/

size_t xPortGetMinimumEverFreeHeapSize(void)
{
  /* Minimum = current in heap_1 since it never frees */
  return xFreeBytesRemaining;
}

/*-----------------------------------------------------------*/

/* Override the default SVC_Handler alias */
void vPortSVCHandler(void) __attribute__((alias("SVC_Handler")));
void vPortPendSVHandler(void) __attribute__((alias("PendSV_Handler")));
void vPortSysTickHandler(void) __attribute__((alias("SysTick_Handler")));
