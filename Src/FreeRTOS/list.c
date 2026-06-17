/*
 * FreeRTOS Kernel V10.6.2
 * List implementation
 */

#include "FreeRTOS.h"
#include "list.h"

/*-----------------------------------------------------------*/

void vListInitialise(List_t * const pxList)
{
  /* The list structure contains a list item which is used to mark the
   * end of the list. To initialise the list the list end is inserted
   * as the only entry in the list. */
  pxList->pxIndex = (ListItem_t *)&(pxList->xListEnd);

  /* The end of list marker has the maximum possible item value to
   * ensure it is always at the end of the list in sort order. */
  pxList->xListEnd.xItemValue = portMAX_DELAY;

  /* The list end next and previous pointers point to itself so we know
   * when the list is empty. */
  pxList->xListEnd.pxNext = (ListItem_t *)&(pxList->xListEnd);
  pxList->xListEnd.pxPrevious = (ListItem_t *)&(pxList->xListEnd);

  pxList->uxNumberOfItems = (UBaseType_t)0U;
}

/*-----------------------------------------------------------*/

void vListInitialiseItem(ListItem_t * const pxItem)
{
  /* Make sure the list item is not recorded as being on a list. */
  pxItem->pxContainer = NULL;
}

/*-----------------------------------------------------------*/

void vListInsertEnd(List_t * const pxList, ListItem_t * const pxNewListItem)
{
  ListItem_t * const pxIndex = pxList->pxIndex;

  /* Insert the new list item before the item pointed to by pxIndex. */
  pxNewListItem->pxNext = pxIndex;
  pxNewListItem->pxPrevious = pxIndex->pxPrevious;

  pxIndex->pxPrevious->pxNext = pxNewListItem;
  pxIndex->pxPrevious = pxNewListItem;

  /* Remember which list the item is in. */
  pxNewListItem->pxContainer = pxList;

  (pxList->uxNumberOfItems)++;
}

/*-----------------------------------------------------------*/

void vListInsert(List_t * const pxList, ListItem_t * const pxNewListItem)
{
  ListItem_t * pxIterator;
  const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;

  /* If the list is empty, just insert at the head */
  if (xValueOfInsertion == portMAX_DELAY)
  {
    pxIterator = pxList->xListEnd.pxPrevious;
  }
  else
  {
    /* Iterate through the list to find the correct position.
     * Items are sorted by xItemValue (ascending). */
    for (pxIterator = (ListItem_t *)&(pxList->xListEnd);
         pxIterator->pxNext->xItemValue <= xValueOfInsertion;
         pxIterator = pxIterator->pxNext)
    {
      /* Empty loop — just walking the list */
    }
  }

  pxNewListItem->pxNext = pxIterator->pxNext;
  pxNewListItem->pxNext->pxPrevious = pxNewListItem;
  pxNewListItem->pxPrevious = pxIterator;
  pxIterator->pxNext = pxNewListItem;

  /* Remember which list the item is in. */
  pxNewListItem->pxContainer = pxList;

  (pxList->uxNumberOfItems)++;
}

/*-----------------------------------------------------------*/

UBaseType_t uxListRemove(ListItem_t * const pxItemToRemove)
{
  /* The list item knows which list it is in.  Obtain the list from the list item. */
  List_t * const pxList = pxItemToRemove->pxContainer;

  pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
  pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;

  /* If the index was pointing to this item, advance it */
  if (pxList->pxIndex == pxItemToRemove)
  {
    pxList->pxIndex = pxItemToRemove->pxPrevious;
  }

  /* Mark the item as not being in a list */
  pxItemToRemove->pxContainer = NULL;
  (pxList->uxNumberOfItems)--;

  return pxList->uxNumberOfItems;
}
