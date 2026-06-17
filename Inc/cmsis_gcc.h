/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __CMSIS_GCC_H
#define __CMSIS_GCC_H

/* ignore some GCC warnings */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* Fallback for older GCC versions */
#ifndef __has_builtin
  #define __has_builtin(x) (0)
#endif

__attribute__((always_inline)) __STATIC_INLINE void __NOP(void)
{
  __ASM volatile ("nop");
}

__attribute__((always_inline)) __STATIC_INLINE void __WFI(void)
{
  __ASM volatile ("wfi");
}

__attribute__((always_inline)) __STATIC_INLINE void __WFE(void)
{
  __ASM volatile ("wfe");
}

__attribute__((always_inline)) __STATIC_INLINE void __SEV(void)
{
  __ASM volatile ("sev");
}

__attribute__((always_inline)) __STATIC_INLINE void __ISB(void)
{
  __ASM volatile ("isb 0xF":::"memory");
}

__attribute__((always_inline)) __STATIC_INLINE void __DSB(void)
{
  __ASM volatile ("dsb 0xF":::"memory");
}

__attribute__((always_inline)) __STATIC_INLINE void __DMB(void)
{
  __ASM volatile ("dmb 0xF":::"memory");
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV(uint32_t value)
{
  return __builtin_bswap32(value);
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV16(uint32_t value)
{
  uint32_t result;
  __ASM ("rev16 %0, %1" : "=r" (result) : "r" (value));
  return result;
}

__attribute__((always_inline)) __STATIC_INLINE int32_t __REVSH(int32_t value)
{
  return (int16_t)__builtin_bswap16((uint16_t)value);
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __ROR(uint32_t op1, uint32_t op2)
{
  op2 %= 32U;
  if (op2 == 0U) {
    return op1;
  }
  return (op1 >> op2) | (op1 << (32U - op2));
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __RBIT(uint32_t value)
{
  uint32_t result;
  __ASM ("rbit %0, %1" : "=r" (result) : "r" (value));
  return result;
}

__attribute__((always_inline)) __STATIC_INLINE uint8_t __CLZ(uint32_t value)
{
  if (value == 0U) { return 32U; }
  return (uint8_t)__builtin_clz(value);
}

#define __BKPT(value)   __ASM volatile ("bkpt "#value)

__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_CONTROL(void)
{
  uint32_t result;
  __ASM volatile ("MRS %0, control" : "=r" (result));
  return result;
}

__attribute__((always_inline)) __STATIC_INLINE void __set_CONTROL(uint32_t control)
{
  __ASM volatile ("MSR control, %0" : : "r" (control) : "memory");
  __ISB();
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_IPSR(void)
{
  uint32_t result;
  __ASM volatile ("MRS %0, ipsr" : "=r" (result));
  return result;
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_APSR(void)
{
  uint32_t result;
  __ASM volatile ("MRS %0, apsr" : "=r" (result));
  return result;
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_xPSR(void)
{
  uint32_t result;
  __ASM volatile ("MRS %0, xpsr" : "=r" (result));
  return result;
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_PSP(void)
{
  uint32_t result;
  __ASM volatile ("MRS %0, psp"  : "=r" (result));
  return result;
}

__attribute__((always_inline)) __STATIC_INLINE void __set_PSP(uint32_t topOfProcStack)
{
  __ASM volatile ("MSR psp, %0" : : "r" (topOfProcStack) : );
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_MSP(void)
{
  uint32_t result;
  __ASM volatile ("MRS %0, msp" : "=r" (result));
  return result;
}

__attribute__((always_inline)) __STATIC_INLINE void __set_MSP(uint32_t topOfMainStack)
{
  __ASM volatile ("MSR msp, %0" : : "r" (topOfMainStack) : );
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_PRIMASK(void)
{
  uint32_t result;
  __ASM volatile ("MRS %0, primask" : "=r" (result));
  return result;
}

__attribute__((always_inline)) __STATIC_INLINE void __set_PRIMASK(uint32_t priMask)
{
  __ASM volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_BASEPRI(void)
{
  uint32_t result;
  __ASM volatile ("MRS %0, basepri" : "=r" (result));
  return result;
}

__attribute__((always_inline)) __STATIC_INLINE void __set_BASEPRI(uint32_t basePri)
{
  __ASM volatile ("MSR basepri, %0" : : "r" (basePri) : "memory");
}

__attribute__((always_inline)) __STATIC_INLINE void __set_BASEPRI_MAX(uint32_t basePri)
{
  __ASM volatile ("MSR basepri_max, %0" : : "r" (basePri) : "memory");
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_FAULTMASK(void)
{
  uint32_t result;
  __ASM volatile ("MRS %0, faultmask" : "=r" (result));
  return result;
}

__attribute__((always_inline)) __STATIC_INLINE void __set_FAULTMASK(uint32_t faultMask)
{
  __ASM volatile ("MSR faultmask, %0" : : "r" (faultMask) : "memory");
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_FPSCR(void)
{
  uint32_t result;
  __ASM volatile ("VMRS %0, fpscr" : "=r" (result));
  return (result);
}

__attribute__((always_inline)) __STATIC_INLINE void __set_FPSCR(uint32_t fpscr)
{
  __ASM volatile ("VMSR fpscr, %0" : : "r" (fpscr) : "vfpcc");
}

/* Enable/disable interrupts (no-op on Cortex-M in thread mode) */
#define __enable_irq()   __ASM volatile ("cpsie i" : : : "memory")
#define __disable_irq()  __ASM volatile ("cpsid i" : : : "memory")

__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_FAULTMASK(void);
__attribute__((always_inline)) __STATIC_INLINE void __set_FAULTMASK(uint32_t faultMask);

#pragma GCC diagnostic pop

#endif /* __CMSIS_GCC_H */
