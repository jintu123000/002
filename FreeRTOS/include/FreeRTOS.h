/*
 * FreeRTOS Kernel V10.6.2
 * Main header — includes port layer + provides core API
 */

#ifndef INC_FREERTOS_H
#define INC_FREERTOS_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Port layer provides: BaseType_t, UBaseType_t, TickType_t, StackType_t, etc. */
#include "portmacro.h"

/* Handle types */
typedef void * TaskHandle_t;
typedef void * QueueHandle_t;
typedef void * SemaphoreHandle_t;
typedef void * TimerHandle_t;
typedef void * EventGroupHandle_t;

/* Boolean values */
#define pdFALSE             ((BaseType_t)0)
#define pdTRUE              ((BaseType_t)1)
#define pdPASS              (pdTRUE)
#define pdFAIL              (pdFALSE)
#define errQUEUE_EMPTY      ((BaseType_t)0)
#define errQUEUE_FULL       ((BaseType_t)0)
#define errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY  (-1)

/* Memory allocation */
void * pvPortMalloc(size_t xWantedSize);
void   vPortFree(void * pv);

/* Scheduler control */
void   vTaskStartScheduler(void);
void   vTaskEndScheduler(void);
void   vTaskSuspendAll(void);
BaseType_t xTaskResumeAll(void);
TickType_t xTaskGetTickCount(void);
TickType_t xTaskGetTickCountFromISR(void);

/* Task creation/deletion */
BaseType_t xTaskCreate(TaskFunction_t pxTaskCode,
                       const char * const pcName,
                       uint16_t usStackDepth,
                       void * const pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t * const pxCreatedTask);

void vTaskDelete(TaskHandle_t xTaskToDelete);

/* Task delay */
void vTaskDelay(TickType_t xTicksToDelay);
void vTaskDelayUntil(TickType_t * const pxPreviousWakeTime, TickType_t xTimeIncrement);

/* Task priority */
void vTaskPrioritySet(TaskHandle_t xTask, UBaseType_t uxNewPriority);
UBaseType_t uxTaskPriorityGet(TaskHandle_t xTask);

/* Task suspend/resume */
void vTaskSuspend(TaskHandle_t xTaskToSuspend);
void vTaskResume(TaskHandle_t xTaskToResume);

/* Critical sections */
void vTaskEnterCritical(void);
void vTaskExitCritical(void);
#define taskENTER_CRITICAL()              vTaskEnterCritical()
#define taskEXIT_CRITICAL()               vTaskExitCritical()
#define taskENTER_CRITICAL_FROM_ISR()     portSET_INTERRUPT_MASK_FROM_ISR()
#define taskEXIT_CRITICAL_FROM_ISR(x)     portCLEAR_INTERRUPT_MASK_FROM_ISR(x)
#define taskENABLE_INTERRUPTS()           portENABLE_INTERRUPTS()
#define taskDISABLE_INTERRUPTS()          portDISABLE_INTERRUPTS()

/* Yield */
#define taskYIELD()                       portYIELD()

/* Scheduler state */
BaseType_t xTaskGetSchedulerState(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_FREERTOS_H */
