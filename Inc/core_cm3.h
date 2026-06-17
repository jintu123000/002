/*
 * CMSIS Core(M) Peripheral Access Layer for Cortex-M3
 * Minimal version for STM32F103 + FreeRTOS
 */

#ifndef __CORE_CM3_H_GENERIC
#define __CORE_CM3_H_GENERIC

#include "cmsis_version.h"
#include "cmsis_compiler.h"

/* Only include GCC-specific intrinsics for GCC/Clang */
#if defined(__GNUC__) || defined(__clang__)
  #include "cmsis_gcc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* CMSIS I/O access qualifiers */
#ifdef __cplusplus
  #define __IOM   volatile       /* Read / Write */
#else
  #define __IOM   volatile       /* Read / Write */
#endif
#define   __IM     volatile const /* Read only */
#define   __OM     volatile       /* Write only */

/* Compiler barrier */
#define __COMPILER_BARRIER() __ASM volatile("":::"memory")

/* Cortex-M3 configuration */
#define __CM3_REV                 0x0201U
#define __MPU_PRESENT             1U
#define __NVIC_PRIO_BITS          4U
#define __Vendor_SysTickConfig    0U
#define __VTOR_PRESENT            1U
#define __FPU_PRESENT             0U
#define __FPU_USED                0U

/*******************************************************************************
 *                Register Structures (must come before macros)
 ******************************************************************************/

/* SCB — System Control Block */
typedef struct
{
  __IM  uint32_t CPUID;
  __IOM uint32_t ICSR;
  __IOM uint32_t VTOR;
  __IOM uint32_t AIRCR;
  __IOM uint32_t SCR;
  __IOM uint32_t CCR;
  __IOM uint8_t  SHP[12U];
  __IOM uint32_t SHCSR;
  __IOM uint32_t CFSR;
  __IOM uint32_t HFSR;
  __IOM uint32_t DFSR;
  __IOM uint32_t MMFAR;
  __IOM uint32_t BFAR;
  __IOM uint32_t AFSR;
  __IM  uint32_t PFR[2U];
  __IM  uint32_t DFR;
  __IM  uint32_t ADR;
  __IM  uint32_t MMFR[4U];
  __IM  uint32_t ISAR[5U];
       uint32_t RESERVED0[5U];
  __IOM uint32_t CPACR;
} SCB_Type;

/* NVIC — Nested Vectored Interrupt Controller */
typedef struct
{
  __IOM uint32_t ISER[8U];
       uint32_t RESERVED0[24U];
  __IOM uint32_t ICER[8U];
       uint32_t RESERVED1[24U];
  __IOM uint32_t ISPR[8U];
       uint32_t RESERVED2[24U];
  __IOM uint32_t ICPR[8U];
       uint32_t RESERVED3[24U];
  __IOM uint32_t IABR[8U];
       uint32_t RESERVED4[56U];
  __IOM uint8_t  IP[240U];
       uint32_t RESERVED5[644U];
  __OM  uint32_t STIR;
} NVIC_Type;

/* SysTick */
typedef struct
{
  __IOM uint32_t CTRL;
  __IOM uint32_t LOAD;
  __IOM uint32_t VAL;
  __IM  uint32_t CALIB;
} SysTick_Type;

/* Debug types */
typedef struct { __IOM uint32_t DHCSR; } CoreDebug_Type;
typedef struct { uint32_t _dummy; } SCnSCB_Type;
typedef struct { uint32_t _dummy[1024]; } ITM_Type;
typedef struct { uint32_t CTRL; uint32_t CYCCNT; } DWT_Type;
typedef struct { uint32_t _dummy; } TPI_Type;

/*******************************************************************************
 *                Base Addresses
 ******************************************************************************/
#define SCS_BASE            (0xE000E000UL)
#define SysTick_BASE        (SCS_BASE + 0x0010UL)
#define NVIC_BASE           (SCS_BASE + 0x0100UL)
#define SCB_BASE            (SCS_BASE + 0x0D00UL)
#define CoreDebug_BASE      (0xE000EDF0UL)

/* Peripheral access macros */
#define SCB                 ((SCB_Type *)      SCB_BASE)
#define SysTick             ((SysTick_Type *)  SysTick_BASE)
#define NVIC                ((NVIC_Type *)     NVIC_BASE)
#define CoreDebug           ((CoreDebug_Type *)CoreDebug_BASE)

/* SCB AIRCR bits */
#define SCB_AIRCR_VECTKEY_Pos              16U
#define SCB_AIRCR_VECTKEY_Msk              (0xFFFFUL << SCB_AIRCR_VECTKEY_Pos)
#define SCB_AIRCR_PRIGROUP_Pos              8U
#define SCB_AIRCR_PRIGROUP_Msk             (7UL << SCB_AIRCR_PRIGROUP_Pos)
#define SCB_AIRCR_SYSRESETREQ_Pos           2U
#define SCB_AIRCR_SYSRESETREQ_Msk          (1UL << SCB_AIRCR_SYSRESETREQ_Pos)

/* SysTick bits */
#define SysTick_CTRL_CLKSOURCE_Pos          2U
#define SysTick_CTRL_CLKSOURCE_Msk         (1UL << SysTick_CTRL_CLKSOURCE_Pos)
#define SysTick_CTRL_TICKINT_Pos            1U
#define SysTick_CTRL_TICKINT_Msk           (1UL << SysTick_CTRL_TICKINT_Pos)
#define SysTick_CTRL_ENABLE_Pos             0U
#define SysTick_CTRL_ENABLE_Msk            (1UL << SysTick_CTRL_ENABLE_Pos)

/*******************************************************************************
 *                IRQn Type (must be before NVIC functions)
 ******************************************************************************/
typedef enum IRQn
{
  /* Cortex-M3 Processor Exceptions */
  NonMaskableInt_IRQn           = -14,
  MemoryManagement_IRQn         = -12,
  BusFault_IRQn                 = -11,
  UsageFault_IRQn               = -10,
  SVCall_IRQn                   = -5,
  DebugMonitor_IRQn             = -4,
  PendSV_IRQn                   = -2,
  SysTick_IRQn                  = -1,
} IRQn_Type;

/*******************************************************************************
 *                NVIC Inline Functions
 ******************************************************************************/

__attribute__((always_inline)) __STATIC_INLINE void NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0) {
    NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}

__attribute__((always_inline)) __STATIC_INLINE void NVIC_DisableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0) {
    NVIC->ICER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __DSB();
    __ISB();
  }
}

__attribute__((always_inline)) __STATIC_INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if ((int32_t)(IRQn) >= 0) {
    NVIC->IP[((uint32_t)IRQn)] = (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & 0xFFUL);
  } else {
    SCB->SHP[(((uint32_t)IRQn) & 0xFUL) - 4UL] = (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & 0xFFUL);
  }
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t NVIC_GetPriority(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0) {
    return ((uint32_t)NVIC->IP[((uint32_t)IRQn)] >> (8U - __NVIC_PRIO_BITS));
  } else {
    return ((uint32_t)SCB->SHP[(((uint32_t)IRQn) & 0xFUL) - 4UL] >> (8U - __NVIC_PRIO_BITS));
  }
}

__attribute__((always_inline)) __STATIC_INLINE void NVIC_SystemReset(void)
{
  __DSB();
  SCB->AIRCR = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) |
                           (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                            SCB_AIRCR_SYSRESETREQ_Msk);
  __DSB();
  for (;;) { __NOP(); }
}

/* __NVIC prefixed versions — used by FreeRTOS port */
__attribute__((always_inline)) __STATIC_INLINE void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0) {
    NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}

__attribute__((always_inline)) __STATIC_INLINE void __NVIC_DisableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0) {
    NVIC->ICER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __DSB();
    __ISB();
  }
}

__attribute__((always_inline)) __STATIC_INLINE void __NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if ((int32_t)(IRQn) >= 0) {
    NVIC->IP[((uint32_t)IRQn)] = (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & 0xFFUL);
  } else {
    SCB->SHP[(((uint32_t)IRQn) & 0xFUL) - 4UL] = (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & 0xFFUL);
  }
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __NVIC_GetPriority(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0) {
    return ((uint32_t)NVIC->IP[((uint32_t)IRQn)] >> (8U - __NVIC_PRIO_BITS));
  } else {
    return ((uint32_t)SCB->SHP[(((uint32_t)IRQn) & 0xFUL) - 4UL] >> (8U - __NVIC_PRIO_BITS));
  }
}

__attribute__((always_inline)) __STATIC_INLINE void __NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0) {
    NVIC->ISPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}

__attribute__((always_inline)) __STATIC_INLINE void __NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0) {
    NVIC->ICPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0) {
    return (uint32_t)((NVIC->ISPR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) ? 1UL : 0UL);
  } else {
    return 0UL;
  }
}

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CM3_H_GENERIC */
