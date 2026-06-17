/*
 * FreeRTOS Kernel V10.6.2
 * Queue implementation — minimal stub for LED blink project
 */

#include "FreeRTOS.h"
#include "queue.h"

/*-----------------------------------------------------------*/

QueueHandle_t xQueueGenericCreate(UBaseType_t uxQueueLength,
                                   UBaseType_t uxItemSize,
                                   uint8_t ucQueueType)
{
  /* Queue not used in this project */
  (void)uxQueueLength;
  (void)uxItemSize;
  (void)ucQueueType;

  /* Return NULL — queues not needed for LED blink */
  return NULL;
}
