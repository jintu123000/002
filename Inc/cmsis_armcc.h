/*
 * CMSIS intrinsics for Arm Compiler 5 (armcc — Keil MDK)
 *
 * Only defines wrappers where the CMSIS name DIFFERS from the ARMCC5 built-in
 * (e.g. CMSIS uses uppercase __NOP, ARMCC5 has lowercase __nop).
 *
 * For register-access functions where names are identical
 * (__get_PRIMASK, __set_PRIMASK, __get_CONTROL, etc.), ARMCC5 already
 * provides them as compiler built-ins — they work without a header.
 */

#ifndef __CMSIS_ARMCC_H
#define __CMSIS_ARMCC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*-----------------------------------------------------------------------------
 * Core Instruction Intrinsics (safe — names differ from ARMCC5 built-ins)
 *----------------------------------------------------------------------------*/

/** \brief  No Operation */
__STATIC_FORCEINLINE void __NOP(void)
{
  __nop();
}

/** \brief  Wait For Interrupt */
__STATIC_FORCEINLINE void __WFI(void)
{
  __wfi();
}

/** \brief  Wait For Event */
__STATIC_FORCEINLINE void __WFE(void)
{
  __wfe();
}

/** \brief  Send Event */
__STATIC_FORCEINLINE void __SEV(void)
{
  __sev();
}

/** \brief  Instruction Synchronization Barrier */
__STATIC_FORCEINLINE void __ISB(void)
{
  __isb(0xF);
}

/** \brief  Data Synchronization Barrier */
__STATIC_FORCEINLINE void __DSB(void)
{
  __dsb(0xF);
}

/** \brief  Data Memory Barrier */
__STATIC_FORCEINLINE void __DMB(void)
{
  __dmb(0xF);
}

/** \brief  Reverse byte order (32 bit) */
__STATIC_FORCEINLINE uint32_t __REV(uint32_t value)
{
  return __rev(value);
}

/** \brief  Reverse byte order (16 bit) */
__STATIC_FORCEINLINE uint32_t __REV16(uint32_t value)
{
  return (uint32_t)( (uint16_t)(((value) >> 8) & 0xFFU) |
                      (uint16_t)(((value) & 0xFFU) << 8) );
}

/** \brief  Reverse byte order (16 bit) — signed */
__STATIC_FORCEINLINE int32_t __REVSH(int32_t value)
{
  return (int32_t)(int16_t)__REV16((uint32_t)value);
}

/** \brief  Rotate Right value */
__STATIC_FORCEINLINE uint32_t __ROR(uint32_t op1, uint32_t op2)
{
  uint32_t s = op2 % 32U;
  return (op1 >> s) | (op1 << (32U - s));
}

/** \brief  Reverse bit order */
__STATIC_FORCEINLINE uint32_t __RBIT(uint32_t value)
{
  return __rbit(value);
}

/** \brief  Count leading zeros */
__STATIC_FORCEINLINE uint8_t __CLZ(uint32_t value)
{
  return (uint8_t)__clz(value);
}

/** \brief  Breakpoint */
#define __BKPT(value)  __breakpoint(value)

/*-----------------------------------------------------------------------------
 * Interrupt Control
 *   ARMCC5 has __enable_irq / __disable_irq as built-in functions.
 *   We provide macros that expand to inline assembly so they work
 *   regardless of include order.
 *----------------------------------------------------------------------------*/

#ifndef __enable_irq
  #define __enable_irq()   __asm { CPSIE i }
#endif

#ifndef __disable_irq
  #define __disable_irq()  __asm { CPSID i }
#endif

/*
 * NOTE: Register-access intrinsics below are provided by ARMCC5 as
 * compiler built-ins with the SAME names used by CMSIS:
 *
 *   __get_PRIMASK    __set_PRIMASK       __get_CONTROL    __set_CONTROL
 *   __get_PSP        __set_PSP           __get_MSP        __set_MSP
 *   __get_BASEPRI    __set_BASEPRI       __set_BASEPRI_MAX
 *   __get_FAULTMASK  __set_FAULTMASK     __get_IPSR
 *   __get_APSR       __get_xPSR          __get_FPSCR      __set_FPSCR
 *
 * They work without explicit inclusion; the compiler may emit
 * "implicit declaration" warnings which are non-fatal.
 */

#ifdef __cplusplus
}
#endif

#endif /* __CMSIS_ARMCC_H */
