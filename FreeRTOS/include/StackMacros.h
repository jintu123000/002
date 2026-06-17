/*
 * FreeRTOS Kernel V10.6.2
 * Stack checking macros — minimal, no MPU
 */

#ifndef STACK_MACROS_H
#define STACK_MACROS_H

#ifndef portUSING_MPU_WRAPPERS
  #define portUSING_MPU_WRAPPERS 0
#endif

#if (portUSING_MPU_WRAPPERS == 0)
  #define portPOINTER_SIZE_TYPE uint32_t
#endif

#endif /* STACK_MACROS_H */
