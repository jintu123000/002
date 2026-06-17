/*
 * System Init for STM32F103C8T6 — HSI 8MHz, no PLL
 * Keeps things simple: 8 MHz HSI used directly as SYSCLK
 */

#include "stm32f1xx.h"

uint32_t SystemCoreClock = 8000000UL;

/**
  * Setup the microcontroller system:
  *  - Enable HSI
  *  - Configure Flash latency for 0 wait state (valid for 0 < SYSCLK <= 24 MHz)
  *  - Set SYSCLK = HSI = 8 MHz
  *  - AHB, APB1, APB2 prescalers = /1
  */
void SystemInit(void)
{
  /* Reset the RCC clock configuration to the default reset state */
  /* Set HSION bit */
  RCC->CR |= RCC_CR_HSION_Msk;

  /* Reset CFGR: SW=HSI, all prescalers = /1 */
  RCC->CFGR = 0x00000000U;

  /* Reset HSEON, CSSON, PLLON bits */
  RCC->CR &= ~(RCC_CR_HSEON_Msk | RCC_CR_PLLON_Msk);
  /* HSEBYP = 0 */
  /* Reset PLLSRC, PLLXTPRE, PLLMUL, USBPRE bits */
  RCC->CFGR &= ~(RCC_CFGR_PLLSRC_Msk | RCC_CFGR_PLLMULL_Msk);

  /* Disable all interrupts */
  RCC->CIR = 0x00000000U;

  /* Configure Flash latency = 0 wait state (for SYSCLK <= 24 MHz) */
  FLASH->ACR = FLASH_ACR_LATENCY_0;

  /* Wait for HSI to be ready */
  while ((RCC->CR & RCC_CR_HSIRDY_Msk) == 0U) {
    /* wait */
  }
}

/**
  * Update SystemCoreClock variable according to current register values.
  */
void SystemCoreClockUpdate(void)
{
  uint32_t tmp;
  uint32_t pllmull = 0U, pllsource = 0U;
  uint32_t prediv = 1U;

  /* Get SYSCLK source */
  tmp = RCC->CFGR & RCC_CFGR_SWS_Msk;

  switch (tmp)
  {
    case RCC_CFGR_SWS_HSI:  /* HSI used as system clock */
      SystemCoreClock = 8000000UL;
      break;
    case RCC_CFGR_SWS_HSE:  /* HSE used as system clock */
      SystemCoreClock = 8000000UL;  /* Assume 8 MHz HSE */
      break;
    case RCC_CFGR_SWS_PLL:  /* PLL used as system clock */
      /* Get PLL clock source and multiplication factor */
      pllmull = (RCC->CFGR & RCC_CFGR_PLLMULL_Msk) >> RCC_CFGR_PLLMULL_Pos;
      pllsource = (RCC->CFGR & RCC_CFGR_PLLSRC_Msk);

      if (pllsource == 0U) { /* HSI/2 */
        SystemCoreClock = (8000000UL / (prediv * 2U)) * (pllmull + 2U);
      } else { /* HSE */
        SystemCoreClock = ((8000000UL / prediv) * (pllmull + 2U));
      }
      break;
    default:
      SystemCoreClock = 8000000UL;
      break;
  }

  /* Compute HCLK, PCLK1, PCLK2 frequencies (not stored globally, for reference) */
  /* HCLK = SYSCLK / AHB prescaler */
  tmp = (RCC->CFGR & RCC_CFGR_HPRE_Msk) >> RCC_CFGR_HPRE_Pos;
  /* APB prescalers can be derived similarly if needed */
  (void)tmp;
}
