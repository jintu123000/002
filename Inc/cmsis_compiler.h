/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __CMSIS_COMPILER_H
#define __CMSIS_COMPILER_H

#include <stdint.h>

/*
 * Arm Compiler 6 (armclang)
 */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #ifndef __ASM
    #define __ASM            __asm
  #endif
  #ifndef __INLINE
    #define __INLINE         __inline
  #endif
  #ifndef __STATIC_INLINE
    #define __STATIC_INLINE  static __inline
  #endif
  #ifndef __STATIC_FORCEINLINE
    #define __STATIC_FORCEINLINE __attribute__((always_inline)) static __inline
  #endif
  #ifndef __NO_RETURN
    #define __NO_RETURN      __attribute__((__noreturn__))
  #endif
  #ifndef __USED
    #define __USED           __attribute__((used))
  #endif
  #ifndef __WEAK
    #define __WEAK           __attribute__((weak))
  #endif
  #ifndef __PACKED
    #define __PACKED         __attribute__((packed, aligned(1)))
  #endif
  #ifndef __PACKED_STRUCT
    #define __PACKED_STRUCT  struct __attribute__((packed, aligned(1)))
  #endif
  #ifndef __UNALIGNED_UINT32
    #define __UNALIGNED_UINT32(x) (*((__packed uint32_t *)(x)))
  #endif
  #ifndef __ALIGNED
    #define __ALIGNED(x)     __attribute__((aligned(x)))
  #endif

/* GNU Arm Compiler */
#elif defined (__GNUC__)
  #ifndef __ASM
    #define __ASM            __asm
  #endif
  #ifndef __INLINE
    #define __INLINE         inline
  #endif
  #ifndef __STATIC_INLINE
    #define __STATIC_INLINE  static inline
  #endif
  #ifndef __STATIC_FORCEINLINE
    #define __STATIC_FORCEINLINE __attribute__((always_inline)) static inline
  #endif
  #ifndef __NO_RETURN
    #define __NO_RETURN      __attribute__((__noreturn__))
  #endif
  #ifndef __USED
    #define __USED           __attribute__((used))
  #endif
  #ifndef __WEAK
    #define __WEAK           __attribute__((weak))
  #endif
  #ifndef __PACKED
    #define __PACKED         __attribute__((packed, aligned(1)))
  #endif
  #ifndef __PACKED_STRUCT
    #define __PACKED_STRUCT  struct __attribute__((packed, aligned(1)))
  #endif
  #ifndef __UNALIGNED_UINT32
    #define __UNALIGNED_UINT32(x) (*((__packed uint32_t *)(x)))
  #endif
  #ifndef __ALIGNED
    #define __ALIGNED(x)     __attribute__((aligned(x)))
  #endif

/* IAR Arm Compiler */
#elif defined (__ICCARM__)
  #ifndef __ASM
    #define __ASM            __asm
  #endif
  #ifndef __INLINE
    #define __INLINE         inline
  #endif
  #ifndef __STATIC_INLINE
    #define __STATIC_INLINE  static inline
  #endif
  #ifndef __STATIC_FORCEINLINE
    #define __STATIC_FORCEINLINE __attribute__((always_inline)) static inline
  #endif
  #ifndef __NO_RETURN
    #define __NO_RETURN      __noreturn
  #endif
  #ifndef __USED
    #define __USED           __root
  #endif
  #ifndef __WEAK
    #define __WEAK           __weak
  #endif
  #ifndef __PACKED
    #define __PACKED         __packed
  #endif
  #ifndef __PACKED_STRUCT
    #define __PACKED_STRUCT  __packed struct
  #endif
  #ifndef __UNALIGNED_UINT32
    #define __UNALIGNED_UINT32 __UNALIGNED __packed uint32_t
  #endif
  #ifndef __ALIGNED
    #define __ALIGNED(x)
  #endif
#endif

#ifndef __PACKED_STRUCT
  #define __PACKED_STRUCT  struct
#endif

#ifndef __RESTRICT
  #if defined (__ICCARM__)
    #define __RESTRICT   __restrict
  #elif defined (_MSC_VER)
    #define __RESTRICT   __restrict
  #elif defined (__GNUC__)
    #define __RESTRICT   __restrict__
  #else
    #define __RESTRICT
  #endif
#endif

#endif /* __CMSIS_COMPILER_H */
