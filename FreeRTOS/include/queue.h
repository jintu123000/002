/*
 * FreeRTOS Kernel V10.6.2
 * Minimal queue header — not used in LED blink, but required by kernel
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

/* For this minimal project, queues aren't used, but the kernel needs the types */

QueueHandle_t xQueueGenericCreate(UBaseType_t uxQueueLength,
                                   UBaseType_t uxItemSize,
                                   uint8_t ucQueueType);

#ifdef __cplusplus
}
#endif

#endif /* QUEUE_H */
