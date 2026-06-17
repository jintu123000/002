/*
 * System Init Header for STM32F1xx
 */

#ifndef __SYSTEM_STM32F1XX_H
#define __SYSTEM_STM32F1XX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock) */

void SystemInit(void);
void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_STM32F1XX_H */
