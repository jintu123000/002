/*
 * FreeRTOS Kernel V10.6.2
 * Minimal MPU wrappers — no MPU usage in this project
 */

#ifndef MPU_WRAPPERS_H
#define MPU_WRAPPERS_H

/* No MPU: all definitions are just the bare symbol */
#ifndef portFORCE_INLINE
  #define portFORCE_INLINE inline __attribute__((always_inline))
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
