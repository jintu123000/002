/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __CMSIS_VERSION_H
#define __CMSIS_VERSION_H

#define __CM_CMSIS_VERSION_MAIN  (5U)                                   /*!< [31:16] CMSIS Core(M) main version */
#define __CM_CMSIS_VERSION_SUB   (6U)                                   /*!< [15:0]  CMSIS Core(M) sub version  */
#define __CM_CMSIS_VERSION       ((__CM_CMSIS_VERSION_MAIN << 16U) | \
                                   __CM_CMSIS_VERSION_SUB           )   /*!< CMSIS Core(M) version number       */

#endif /* __CMSIS_VERSION_H */
