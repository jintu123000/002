/*
 * FreeRTOS Kernel V10.6.2
 * Minimal MPU wrappers — no MPU usage in this project
 */

#ifndef MPU_WRAPPERS_H
#define MPU_WRAPPERS_H

/* No MPU: all definitions are just the bare symbol */
#ifndef portFORCE_INLINE
  #if defined(__CC_ARM)
    /* ARM Compiler 5 — use __forceinline keyword */
    #define portFORCE_INLINE __forceinline
  #elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    /* ARM Compiler 6 — supports GCC-style attributes */
    #define portFORCE_INLINE inline __attribute__((always_inline))
  #else
    #define portFORCE_INLINE inline __attribute__((always_inline))
  #endif
#endif

#ifndef PRIVILEGED_FUNCTION
  #define PRIVILEGED_FUNCTION
#endif

#ifndef PRIVILEGED_DATA
  #define PRIVILEGED_DATA
#endif

#ifndef portUSING_MPU_WRAPPERS
  #define portUSING_MPU_WRAPPERS 0
#endif

#endif /* MPU_WRAPPERS_H */
