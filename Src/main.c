/*
 * main.c — STM32F103C8T6 + FreeRTOS LED Blink
 *
 * Two LEDs on GPIOA Pin 8 and Pin 9 blink alternately.
 * Each toggles at 0.5 second intervals.
 *
 * Hardware setup:
 *   - LED1: GPIOA Pin 8 (active high — output push-pull)
 *   - LED2: GPIOA Pin 9 (active high — output push-pull)
 *
 * Clock: HSI 8 MHz, no PLL
 * FreeRTOS tick: 1000 Hz
 */

#include "stm32f1xx.h"
#include "FreeRTOS.h"
#include "task.h"

/*-----------------------------------------------------------*/
/* LED Pin Definitions                                        */
/*-----------------------------------------------------------*/
#define LED1_PORT           GPIOA
#define LED1_PIN            GPIO_Pin_8

#define LED2_PORT           GPIOA
#define LED2_PIN            GPIO_Pin_9

/*-----------------------------------------------------------*/
/* GPIO Initialization                                        */
/*-----------------------------------------------------------*/
static void GPIO_Init(void)
{
  /* Enable GPIOA clock on APB2 bus */
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN_Msk;

  /* Configure PA8 and PA9 as push-pull outputs @ 2 MHz */
  GPIO_SET_PIN_MODE(GPIOA, 8, GPIO_OUTPUT_PP_2MHZ);
  GPIO_SET_PIN_MODE(GPIOA, 9, GPIO_OUTPUT_PP_2MHZ);

  /* Ensure both LEDs are off initially */
  GPIO_RESET_BIT(LED1_PORT, LED1_PIN);
  GPIO_RESET_BIT(LED2_PORT, LED2_PIN);
}

/*-----------------------------------------------------------*/
/* LED Task 1 — Blink LED on PA8 every 500ms                 */
/*-----------------------------------------------------------*/
static void vLED1_Task(void * pvParameters)
{
  (void)pvParameters;

  for (;;)
  {
    /* Toggle LED1 (PA8) */
    if ((LED1_PORT->ODR & LED1_PIN) != 0U)
    {
      GPIO_RESET_BIT(LED1_PORT, LED1_PIN);
    }
    else
    {
      GPIO_SET_BIT(LED1_PORT, LED1_PIN);
    }

    /* Delay 500 ms */
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

/*-----------------------------------------------------------*/
/* LED Task 2 — Blink LED on PA9 every 500ms                 */
/*-----------------------------------------------------------*/
static void vLED2_Task(void * pvParameters)
{
  (void)pvParameters;

  for (;;)
  {
    /* Toggle LED2 (PA9) */
    if ((LED2_PORT->ODR & LED2_PIN) != 0U)
    {
      GPIO_RESET_BIT(LED2_PORT, LED2_PIN);
    }
    else
    {
      GPIO_SET_BIT(LED2_PORT, LED2_PIN);
    }

    /* Delay 500 ms */
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

/*-----------------------------------------------------------*/
/* Main Entry                                                 */
/*-----------------------------------------------------------*/
int main(void)
{
  /* Initialize hardware */
  GPIO_Init();

  /* Create LED tasks */
  /* LED1 — GPIOA Pin 8, priority 1 */
  xTaskCreate(vLED1_Task,
              "LED1",
              configMINIMAL_STACK_SIZE,
              NULL,
              1,
              NULL);

  /* LED2 — GPIOA Pin 9, priority 1 (same priority = round-robin) */
  xTaskCreate(vLED2_Task,
              "LED2",
              configMINIMAL_STACK_SIZE,
              NULL,
              1,
              NULL);

  /* Start the FreeRTOS scheduler — never returns */
  vTaskStartScheduler();

  /* Should never reach here */
  for (;;) {
    __NOP();
  }

  return 0;
}
