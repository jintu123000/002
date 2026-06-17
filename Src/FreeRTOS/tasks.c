/*
 * FreeRTOS Kernel V10.6.2
 * Task implementation — scheduler, task creation, context switching
 *
 * Minimal but functional version for LED blink project.
 */

#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"

/*-----------------------------------------------------------*/

/* TCB structure */
typedef struct tskTaskControlBlock
{
  volatile StackType_t * pxTopOfStack;
  ListItem_t            xStateListItem;
  ListItem_t            xEventListItem;
  UBaseType_t           uxPriority;
  StackType_t *         pxStack;
  char                  pcTaskName[configMAX_TASK_NAME_LEN];
  uint8_t               ucDelayAborted;
} tskTCB;

typedef tskTCB TCB_t;

/*-----------------------------------------------------------*/

/* Ready lists indexed by priority */
static List_t pxReadyTasksLists[configMAX_PRIORITIES];

/* Delayed tasks lists */
static List_t xDelayedTaskList1;
static List_t xDelayedTaskList2;
static List_t * volatile pxDelayedTaskList;
static List_t * volatile pxOverflowDelayedTaskList;

/* Tasks waiting termination */
static List_t xTasksWaitingTermination;

/* Pending ready list (for tasks unblocked while scheduler is suspended) */
static List_t xPendingReadyList;

/*-----------------------------------------------------------*/

/* Current task control block */
TCB_t * volatile pxCurrentTCB = NULL;

/* Idle task handle */
static TaskHandle_t xIdleTaskHandle = NULL;

/* Scheduler state */
static volatile UBaseType_t uxSchedulerSuspended = (UBaseType_t)0U;
static volatile BaseType_t xSchedulerRunning = pdFALSE;

/* Tick count */
static volatile TickType_t xTickCount = (TickType_t)0U;

/* Task count */
static volatile UBaseType_t uxCurrentNumberOfTasks = (UBaseType_t)0U;

/* Pending yield flag */
static volatile BaseType_t xYieldPending = pdFALSE;

/* Highest priority level that has a ready task */
static volatile UBaseType_t uxTopReadyPriority = 0U;

/*-----------------------------------------------------------*/

/* Internal functions */
static void prvInitialiseTaskLists(void);
static void prvIdleTask(void * pvParameters);
static void prvAddTaskToReadyList(TCB_t * pxNewTCB);

/*-----------------------------------------------------------*/

#define portRESET_READY_PRIORITY(uxPriority) \
  (uxPriority) = (UBaseType_t)(configMAX_PRIORITIES - 1U)

/*-----------------------------------------------------------*/

void vTaskSwitchContext(void)
{
  if (uxSchedulerSuspended != (UBaseType_t)0U)
  {
    xYieldPending = pdTRUE;
  }
  else
  {
    xYieldPending = pdFALSE;

    /* Find the highest priority list that contains ready tasks */
    while (listLIST_IS_EMPTY(&(pxReadyTasksLists[uxTopReadyPriority])) != pdFALSE)
    {
      if (uxTopReadyPriority == 0U) break;
      uxTopReadyPriority--;
    }

    /* Get the TCB from the head of the highest priority list */
    if (listLIST_IS_EMPTY(&(pxReadyTasksLists[uxTopReadyPriority])) == pdFALSE)
    {
      List_t * pxList = &(pxReadyTasksLists[uxTopReadyPriority]);
      pxCurrentTCB = (TCB_t *)listGET_OWNER_OF_HEAD_ENTRY(pxList);
    }
  }
}

/*-----------------------------------------------------------*/

static void prvInitialiseTaskLists(void)
{
  UBaseType_t uxPriority;

  for (uxPriority = (UBaseType_t)0U; uxPriority < (UBaseType_t)configMAX_PRIORITIES; uxPriority++)
  {
    vListInitialise(&(pxReadyTasksLists[uxPriority]));
  }

  vListInitialise(&xDelayedTaskList1);
  vListInitialise(&xDelayedTaskList2);
  vListInitialise(&xTasksWaitingTermination);
  vListInitialise(&xPendingReadyList);

  pxDelayedTaskList = &xDelayedTaskList1;
  pxOverflowDelayedTaskList = &xDelayedTaskList2;
}

/*-----------------------------------------------------------*/

static void prvAddTaskToReadyList(TCB_t * pxNewTCB)
{
  vListInsertEnd(&(pxReadyTasksLists[pxNewTCB->uxPriority]),
                 &(pxNewTCB->xStateListItem));

  if (pxNewTCB->uxPriority > uxTopReadyPriority)
  {
    uxTopReadyPriority = pxNewTCB->uxPriority;
  }
}

/*-----------------------------------------------------------*/

BaseType_t xTaskCreate(TaskFunction_t pxTaskCode,
                        const char * const pcName,
                        uint16_t usStackDepth,
                        void * const pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t * const pxCreatedTask)
{
  TCB_t * pxNewTCB;
  StackType_t * pxStack;

  if (usStackDepth < configMINIMAL_STACK_SIZE)
  {
    usStackDepth = configMINIMAL_STACK_SIZE;
  }

  /* Allocate stack */
  pxStack = (StackType_t *)pvPortMalloc(usStackDepth * sizeof(StackType_t));

  if (pxStack == NULL)
  {
    return errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
  }

  /* Allocate TCB */
  pxNewTCB = (TCB_t *)pvPortMalloc(sizeof(TCB_t));

  if (pxNewTCB == NULL)
  {
    vPortFree(pxStack);
    return errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
  }

  /* Store the task name */
  if (pcName != NULL)
  {
    size_t x;
    for (x = 0; x < (size_t)(configMAX_TASK_NAME_LEN - 1); x++)
    {
      pxNewTCB->pcTaskName[x] = pcName[x];
      if (pcName[x] == '\0') break;
    }
    pxNewTCB->pcTaskName[configMAX_TASK_NAME_LEN - 1] = '\0';
  }
  else
  {
    pxNewTCB->pcTaskName[0] = '\0';
  }

  /* Clamp priority */
  if (uxPriority >= (UBaseType_t)configMAX_PRIORITIES)
  {
    uxPriority = (UBaseType_t)configMAX_PRIORITIES - (UBaseType_t)1U;
  }
  pxNewTCB->uxPriority = uxPriority;

  /* Initialize the stack */
  pxStack = pxPortInitialiseStack(&pxStack[usStackDepth - 1], pxTaskCode, pvParameters);
  pxNewTCB->pxTopOfStack = pxStack;

  /* Initialize the TCB list items */
  vListInitialiseItem(&(pxNewTCB->xStateListItem));
  vListInitialiseItem(&(pxNewTCB->xEventListItem));

  /* Set the owner of the state list item to this TCB */
  listSET_LIST_ITEM_OWNER(&(pxNewTCB->xStateListItem), pxNewTCB);
  pxNewTCB->ucDelayAborted = pdFALSE;

  /* Add task */
  uxCurrentNumberOfTasks++;

  if (pxCreatedTask != NULL)
  {
    *pxCreatedTask = (TaskHandle_t)pxNewTCB;
  }

  /* If scheduler running, add directly */
  if (xSchedulerRunning != pdFALSE)
  {
    taskENTER_CRITICAL();
    prvAddTaskToReadyList(pxNewTCB);
    taskEXIT_CRITICAL();
  }

  return pdPASS;
}

/*-----------------------------------------------------------*/

void vTaskStartScheduler(void)
{
  BaseType_t xReturn;

  /* Create the idle task */
  xReturn = xTaskCreate(prvIdleTask,
                         "IDLE",
                         configMINIMAL_STACK_SIZE,
                         NULL,
                         tskIDLE_PRIORITY,
                         &xIdleTaskHandle);

  if (xReturn == pdPASS)
  {
    taskDISABLE_INTERRUPTS();

    /* Initialize task lists */
    prvInitialiseTaskLists();

    /* Set flag that scheduler has been started */
    xSchedulerRunning = pdTRUE;
    xTickCount = (TickType_t)0U;

    /* Add the idle task as current if none other */
    if (pxCurrentTCB == NULL)
    {
      /* Find highest priority */
      uxTopReadyPriority = (UBaseType_t)(configMAX_PRIORITIES - 1U);
      while (listLIST_IS_EMPTY(&(pxReadyTasksLists[uxTopReadyPriority])))
      {
        if (uxTopReadyPriority == 0U) break;
        uxTopReadyPriority--;
      }
      pxCurrentTCB = (TCB_t *)listGET_OWNER_OF_HEAD_ENTRY(
                                 &(pxReadyTasksLists[uxTopReadyPriority]));
    }

    /* Start the port-specific scheduler — never returns */
    xPortStartScheduler();
  }

  /* Should only reach here if idle task creation failed */
  for (;;);
}

/*-----------------------------------------------------------*/

static void prvIdleTask(void * pvParameters)
{
  (void)pvParameters;

  for (;;)
  {
    /* Clean up deleted tasks */
    while (uxListRemove((ListItem_t *)&(xTasksWaitingTermination.xListEnd.pxNext)) > 0U)
    {
      /* Tasks are removed from the termination list by the idle task */
    }

    #if (configUSE_IDLE_HOOK == 1)
    {
      extern void vApplicationIdleHook(void);
      vApplicationIdleHook();
    }
    #endif
  }
}

/*-----------------------------------------------------------*/

void vTaskEndScheduler(void)
{
  taskDISABLE_INTERRUPTS();
  xSchedulerRunning = pdFALSE;
  vPortEndScheduler();
}

/*-----------------------------------------------------------*/

void vTaskSuspendAll(void)
{
  uxSchedulerSuspended++;
}

/*-----------------------------------------------------------*/

BaseType_t xTaskResumeAll(void)
{
  TCB_t * pxTCB;
  BaseType_t xAlreadyYielded = pdFALSE;

  taskENTER_CRITICAL();
  {
    --uxSchedulerSuspended;

    if (uxSchedulerSuspended == (UBaseType_t)0U)
    {
      if (uxCurrentNumberOfTasks > (UBaseType_t)0U)
      {
        while (listLIST_IS_EMPTY(&xPendingReadyList) == pdFALSE)
        {
          pxTCB = (TCB_t *)listGET_OWNER_OF_HEAD_ENTRY(&xPendingReadyList);
          (void)uxListRemove(&(pxTCB->xEventListItem));
          (void)uxListRemove(&(pxTCB->xStateListItem));
          prvAddTaskToReadyList(pxTCB);

          if (pxTCB->uxPriority >= pxCurrentTCB->uxPriority)
          {
            xYieldPending = pdTRUE;
          }
        }
      }
    }
  }
  taskEXIT_CRITICAL();

  return xAlreadyYielded;
}

/*-----------------------------------------------------------*/

TickType_t xTaskGetTickCount(void)
{
  TickType_t xTicks;
  taskENTER_CRITICAL();
  xTicks = xTickCount;
  taskEXIT_CRITICAL();
  return xTicks;
}

/*-----------------------------------------------------------*/

TickType_t xTaskGetTickCountFromISR(void)
{
  return xTickCount;
}

/*-----------------------------------------------------------*/

void vTaskDelay(TickType_t xTicksToDelay)
{
  TCB_t * pxTCB;
  TickType_t xTimeToWake;

  if (xTicksToDelay == (TickType_t)0U)
  {
    taskYIELD();
    return;
  }

  taskENTER_CRITICAL();
  {
    pxTCB = pxCurrentTCB;

    (void)uxListRemove(&(pxTCB->xStateListItem));

    xTimeToWake = xTickCount + xTicksToDelay;
    listSET_LIST_ITEM_VALUE(&(pxTCB->xStateListItem), xTimeToWake);

    if (xTimeToWake < xTickCount)
    {
      vListInsert(pxOverflowDelayedTaskList, &(pxTCB->xStateListItem));
    }
    else
    {
      vListInsert(pxDelayedTaskList, &(pxTCB->xStateListItem));
    }
  }
  taskEXIT_CRITICAL();

  taskYIELD();
}

/*-----------------------------------------------------------*/

void vTaskDelayUntil(TickType_t * const pxPreviousWakeTime, TickType_t xTimeIncrement)
{
  TickType_t xTimeToWake;

  taskENTER_CRITICAL();
  {
    xTimeToWake = *pxPreviousWakeTime + xTimeIncrement;

    if (xTickCount < *pxPreviousWakeTime)
    {
      /* Overflow */
      if (xTimeToWake > *pxPreviousWakeTime)
      {
        /* OK */
      }
    }

    *pxPreviousWakeTime = xTimeToWake;

    if (xTimeToWake > xTickCount)
    {
      (void)uxListRemove(&(pxCurrentTCB->xStateListItem));
      listSET_LIST_ITEM_VALUE(&(pxCurrentTCB->xStateListItem), xTimeToWake);

      if (xTimeToWake < xTickCount)
      {
        vListInsert(pxOverflowDelayedTaskList, &(pxCurrentTCB->xStateListItem));
      }
      else
      {
        vListInsert(pxDelayedTaskList, &(pxCurrentTCB->xStateListItem));
      }
    }
  }
  taskEXIT_CRITICAL();

  taskYIELD();
}

/*-----------------------------------------------------------*/

void vTaskPrioritySet(TaskHandle_t xTask, UBaseType_t uxNewPriority)
{
  TCB_t * pxTCB;
  UBaseType_t uxCurrentPriority;

  taskENTER_CRITICAL();
  {
    pxTCB = (TCB_t *)xTask;

    if (uxNewPriority >= (UBaseType_t)configMAX_PRIORITIES)
    {
      uxNewPriority = (UBaseType_t)configMAX_PRIORITIES - (UBaseType_t)1U;
    }

    uxCurrentPriority = pxTCB->uxPriority;

    if (uxCurrentPriority != uxNewPriority)
    {
      pxTCB->uxPriority = uxNewPriority;

      /* If task is on a list, re-insert at new priority */
      if (pxTCB->xStateListItem.pxContainer != NULL)
      {
        (void)uxListRemove(&(pxTCB->xStateListItem));
        prvAddTaskToReadyList(pxTCB);
      }

      if (uxNewPriority > uxCurrentPriority)
      {
        taskYIELD();
      }
    }
  }
  taskEXIT_CRITICAL();
}

/*-----------------------------------------------------------*/

UBaseType_t uxTaskPriorityGet(TaskHandle_t xTask)
{
  TCB_t * pxTCB;
  UBaseType_t uxReturn;

  taskENTER_CRITICAL();
  pxTCB = (TCB_t *)xTask;
  uxReturn = pxTCB->uxPriority;
  taskEXIT_CRITICAL();

  return uxReturn;
}

/*-----------------------------------------------------------*/

void vTaskSuspend(TaskHandle_t xTaskToSuspend)
{
  TCB_t * pxTCB;

  taskENTER_CRITICAL();
  {
    pxTCB = (TCB_t *)xTaskToSuspend;

    if (uxListRemove(&(pxTCB->xStateListItem)) == (UBaseType_t)0U)
    {
      portRESET_READY_PRIORITY(uxTopReadyPriority);
    }

    if (pxTCB->xEventListItem.pxContainer != NULL)
    {
      (void)uxListRemove(&(pxTCB->xEventListItem));
    }
  }
  taskEXIT_CRITICAL();
}

/*-----------------------------------------------------------*/

void vTaskDelete(TaskHandle_t xTaskToDelete)
{
  TCB_t * pxTCB;

  taskENTER_CRITICAL();
  {
    pxTCB = (TCB_t *)xTaskToDelete;

    if (uxListRemove(&(pxTCB->xStateListItem)) == (UBaseType_t)0U)
    {
      portRESET_READY_PRIORITY(uxTopReadyPriority);
    }

    if (pxTCB->xEventListItem.pxContainer != NULL)
    {
      (void)uxListRemove(&(pxTCB->xEventListItem));
    }

    uxCurrentNumberOfTasks--;

    if (pxTCB == pxCurrentTCB)
    {
      vListInsertEnd(&xTasksWaitingTermination, &(pxTCB->xStateListItem));
    }
  }
  taskEXIT_CRITICAL();

  if (xSchedulerRunning != pdFALSE)
  {
    if (pxTCB == pxCurrentTCB)
    {
      taskYIELD();
    }
  }
}

/*-----------------------------------------------------------*/

void vTaskResume(TaskHandle_t xTaskToResume)
{
  TCB_t * const pxTCB = (TCB_t *)xTaskToResume;

  taskENTER_CRITICAL();
  {
    /* Remove from any list it's in and add to ready list */
    if (pxTCB->xStateListItem.pxContainer != NULL)
    {
      (void)uxListRemove(&(pxTCB->xStateListItem));
    }

    prvAddTaskToReadyList(pxTCB);

    if (pxTCB->uxPriority >= pxCurrentTCB->uxPriority)
    {
      taskYIELD();
    }
  }
  taskEXIT_CRITICAL();
}

/*-----------------------------------------------------------*/

void xTaskIncrementTick(void)
{
  TCB_t * pxTCB;
  TickType_t xItemValue;

  if (uxSchedulerSuspended == (UBaseType_t)0U)
  {
    xTickCount++;

    if (xTickCount == (TickType_t)0U)
    {
      /* Tick overflow — swap delayed lists */
      List_t * pxTemp = pxDelayedTaskList;
      pxDelayedTaskList = pxOverflowDelayedTaskList;
      pxOverflowDelayedTaskList = pxTemp;
    }

    /* Wake tasks whose delay has expired */
    for (;;)
    {
      if (listLIST_IS_EMPTY(pxDelayedTaskList) != pdFALSE)
      {
        break;
      }

      pxTCB = (TCB_t *)listGET_OWNER_OF_HEAD_ENTRY(pxDelayedTaskList);
      xItemValue = listGET_LIST_ITEM_VALUE(&(pxTCB->xStateListItem));

      if (xTickCount < xItemValue)
      {
        break;
      }

      (void)uxListRemove(&(pxTCB->xStateListItem));

      if (pxTCB->xEventListItem.pxContainer != NULL)
      {
        (void)uxListRemove(&(pxTCB->xEventListItem));
      }

      prvAddTaskToReadyList(pxTCB);
    }
  }
}

/*-----------------------------------------------------------*/

BaseType_t xTaskGetSchedulerState(void)
{
  BaseType_t xReturn;

  if (xSchedulerRunning == pdFALSE)
  {
    xReturn = 0;
  }
  else if (uxSchedulerSuspended != (UBaseType_t)0U)
  {
    xReturn = 2;
  }
  else
  {
    xReturn = 1;
  }

  return xReturn;
}

/*-----------------------------------------------------------*/

char * pcTaskGetName(TaskHandle_t xTaskToQuery)
{
  TCB_t * pxTCB = (xTaskToQuery == NULL) ? pxCurrentTCB : (TCB_t *)xTaskToQuery;
  return &(pxTCB->pcTaskName[0]);
}

/*-----------------------------------------------------------*/

TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
  return (TaskHandle_t)pxCurrentTCB;
}

/*-----------------------------------------------------------*/

UBaseType_t uxTaskGetNumberOfTasks(void)
{
  return uxCurrentNumberOfTasks;
}

/*-----------------------------------------------------------*/

void vTaskEnterCritical(void)
{
  portDISABLE_INTERRUPTS();
}

/*-----------------------------------------------------------*/

void vTaskExitCritical(void)
{
  portENABLE_INTERRUPTS();
}
