/*
 * FreeRTOS Kernel V10.6.2
 */

#ifndef LIST_H
#define LIST_H

#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

struct xLIST_ITEM
{
  TickType_t xItemValue;                   /*!< The value being listed */
  struct xLIST_ITEM * pxNext;              /*!< Pointer to the next item */
  struct xLIST_ITEM * pxPrevious;          /*!< Pointer to the previous item */
  void * pvOwner;                          /*!< Pointer to the object (TCB) that owns the list item */
  struct xLIST * pxContainer;              /*!< Pointer to the list in which this item is placed */
};
typedef struct xLIST_ITEM ListItem_t;

struct xMINI_LIST_ITEM
{
  TickType_t xItemValue;
  struct xLIST_ITEM * pxNext;
  struct xLIST_ITEM * pxPrevious;
};
typedef struct xMINI_LIST_ITEM MiniListItem_t;

struct xLIST
{
  UBaseType_t uxNumberOfItems;
  ListItem_t * pxIndex;                     /*!< Used to walk through the list */
  MiniListItem_t xListEnd;                  /*!< List item that marks the end of the list */
};
typedef struct xLIST List_t;

/* List initialization */
void vListInitialise(List_t * const pxList);
void vListInitialiseItem(ListItem_t * const pxItem);

/* List insertion */
void vListInsert(List_t * const pxList, ListItem_t * const pxNewListItem);
void vListInsertEnd(List_t * const pxList, ListItem_t * const pxNewListItem);

/* List removal */
UBaseType_t uxListRemove(ListItem_t * const pxItemToRemove);

/* Value macros — used by the scheduler */
#define listSET_LIST_ITEM_OWNER(pxListItem, pxOwner)  ((pxListItem)->pvOwner = (void *)(pxOwner))
#define listGET_LIST_ITEM_OWNER(pxListItem)           ((pxListItem)->pvOwner)
#define listSET_LIST_ITEM_VALUE(pxListItem, xValue)   ((pxListItem)->xItemValue = (xValue))
#define listGET_LIST_ITEM_VALUE(pxListItem)           ((pxListItem)->xItemValue)
#define listGET_ITEM_VALUE_OF_HEAD_ENTRY(pxList)      (((pxList)->xListEnd.pxNext)->xItemValue)
#define listLIST_IS_EMPTY(pxList)                     (((pxList)->uxNumberOfItems == (UBaseType_t)0) ? pdTRUE : pdFALSE)
#define listGET_OWNER_OF_HEAD_ENTRY(pxList)           ((pxList)->xListEnd.pxNext->pvOwner)
#define listGET_OWNER_OF_NEXT_ENTRY(pxTCB, pxList)    /* defined in tasks.c */
#define listCURRENT_LIST_LENGTH(pxList)               ((pxList)->uxNumberOfItems)

#ifdef __cplusplus
}
#endif

#endif /* LIST_H */
