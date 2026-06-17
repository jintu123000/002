/*
 * FreeRTOS Kernel V10.6.2
 */

#ifndef INC_TASK_H
#define INC_TASK_H

#include <stdint.h>
#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Task states */
typedef enum
{
  eRunning = 0,
  eReady,
  eBlocked,
  eSuspended,
  eDeleted,
  eInvalid
} eTaskState;

#define tskIDLE_PRIORITY          ((UBaseType_t)0U)

/* Scheduler control */
void vTaskStartScheduler(void);
void vTaskEndScheduler(void);
void vTaskSuspendAll(void);
BaseType_t xTaskResumeAll(void);
TickType_t xTaskGetTickCount(void);
TickType_t xTaskGetTickCountFromISR(void);
UBaseType_t uxTaskGetNumberOfTasks(void);
char * pcTaskGetName(TaskHandle_t xTaskToQuery);
TaskHandle_t xTaskGetCurrentTaskHandle(void);

/* Task delay */
void vTaskDelay(TickType_t xTicksToDelay);
void vTaskDelayUntil(TickType_t * const pxPreviousWakeTime, TickType_t xTimeIncrement);

/* Task utilities */
UBaseType_t uxTaskPriorityGet(const TaskHandle_t xTask);
void vTaskPrioritySet(TaskHandle_t xTask, UBaseType_t uxNewPriority);
eTaskState eTaskGetState(TaskHandle_t xTask);
void vTaskSuspend(TaskHandle_t xTaskToSuspend);
void vTaskResume(TaskHandle_t xTaskToResume);
void vTaskDelete(TaskHandle_t xTaskToDelete);

/* Tick increment (called from SysTick ISR) */
void xTaskIncrementTick(void);

/* Context switch (called from PendSV ISR) */
void vTaskSwitchContext(void);

/* Critical section management */
void vTaskEnterCritical(void);
void vTaskExitCritical(void);

#define taskYIELD() portYIELD()

#ifdef __cplusplus
}
#endif

#endif /* INC_TASK_H */
