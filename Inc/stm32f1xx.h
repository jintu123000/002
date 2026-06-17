/*
 * STM32F1xx Device Header - focused for STM32F103C8T6
 */

#ifndef __STM32F1xx_H
#define __STM32F1xx_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core_cm3.h"
#include <stdint.h>

/*******************************************************************************
 *                Memory Map
 ******************************************************************************/
#define FLASH_BASE            0x08000000UL
#define SRAM_BASE             0x20000000UL
#define PERIPH_BASE           0x40000000UL

/* APB1 peripherals */
#define APB1PERIPH_BASE       PERIPH_BASE
/* APB2 peripherals */
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000UL)
/* AHB peripherals */
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x00020000UL)

/*******************************************************************************
 *                RCC Register Offsets
 ******************************************************************************/
#define RCC_BASE              (AHBPERIPH_BASE + 0x00001000UL)

/*******************************************************************************
 *                GPIO Register Offsets
 ******************************************************************************/
#define GPIOA_BASE            (APB2PERIPH_BASE + 0x00000800UL)
#define GPIOB_BASE            (APB2PERIPH_BASE + 0x00000C00UL)
#define GPIOC_BASE            (APB2PERIPH_BASE + 0x00001000UL)
#define GPIOD_BASE            (APB2PERIPH_BASE + 0x00001400UL)
#define GPIOE_BASE            (APB2PERIPH_BASE + 0x00001800UL)
#define GPIOF_BASE            (APB2PERIPH_BASE + 0x00001C00UL)
#define GPIOG_BASE            (APB2PERIPH_BASE + 0x00002000UL)

/*******************************************************************************
 *                AFIO Register Offsets
 ******************************************************************************/
#define AFIO_BASE             (APB2PERIPH_BASE + 0x00000000UL)

/*******************************************************************************
 *                FLASH Register Offsets
 ******************************************************************************/
#define FLASH_R_BASE          (AHBPERIPH_BASE + 0x00002000UL)

/*******************************************************************************
 *                Peripheral Declarations
 ******************************************************************************/
#define RCC                   ((RCC_TypeDef *)     RCC_BASE)
#define GPIOA                 ((GPIO_TypeDef *)    GPIOA_BASE)
#define GPIOB                 ((GPIO_TypeDef *)    GPIOB_BASE)
#define GPIOC                 ((GPIO_TypeDef *)    GPIOC_BASE)
#define GPIOD                 ((GPIO_TypeDef *)    GPIOD_BASE)
#define GPIOE                 ((GPIO_TypeDef *)    GPIOE_BASE)
#define GPIOF                 ((GPIO_TypeDef *)    GPIOF_BASE)
#define GPIOG                 ((GPIO_TypeDef *)    GPIOG_BASE)
#define AFIO                  ((AFIO_TypeDef *)    AFIO_BASE)
#define FLASH                 ((FLASH_TypeDef *)   FLASH_R_BASE)

/*******************************************************************************
 *                RCC Register Structure
 ******************************************************************************/
typedef struct
{
  __IOM uint32_t CR;             /*!< RCC Clock Control Register,                   Address offset: 0x00 */
  __IOM uint32_t CFGR;           /*!< RCC Clock Configuration Register,              Address offset: 0x04 */
  __IOM uint32_t CIR;            /*!< RCC Clock Interrupt Register,                  Address offset: 0x08 */
  __IOM uint32_t APB2RSTR;       /*!< RCC APB2 Peripheral Reset Register,            Address offset: 0x0C */
  __IOM uint32_t APB1RSTR;       /*!< RCC APB1 Peripheral Reset Register,            Address offset: 0x10 */
  __IOM uint32_t AHBENR;         /*!< RCC AHB Peripheral Clock Enable Register,       Address offset: 0x14 */
  __IOM uint32_t APB2ENR;        /*!< RCC APB2 Peripheral Clock Enable Register,      Address offset: 0x18 */
  __IOM uint32_t APB1ENR;        /*!< RCC APB1 Peripheral Clock Enable Register,      Address offset: 0x1C */
  __IOM uint32_t BDCR;           /*!< RCC Backup Domain Control Register,             Address offset: 0x20 */
  __IOM uint32_t CSR;            /*!< RCC Control/Status Register,                    Address offset: 0x24 */
} RCC_TypeDef;

/* RCC_CR bit definitions */
#define RCC_CR_HSION_Pos          0U
#define RCC_CR_HSION_Msk          (0x01UL << RCC_CR_HSION_Pos)
#define RCC_CR_HSIRDY_Pos         1U
#define RCC_CR_HSIRDY_Msk         (0x01UL << RCC_CR_HSIRDY_Pos)
#define RCC_CR_HSEON_Pos          16U
#define RCC_CR_HSEON_Msk          (0x01UL << RCC_CR_HSEON_Pos)
#define RCC_CR_HSERDY_Pos         17U
#define RCC_CR_HSERDY_Msk         (0x01UL << RCC_CR_HSERDY_Pos)
#define RCC_CR_PLLON_Pos          24U
#define RCC_CR_PLLON_Msk          (0x01UL << RCC_CR_PLLON_Pos)
#define RCC_CR_PLLRDY_Pos         25U
#define RCC_CR_PLLRDY_Msk         (0x01UL << RCC_CR_PLLRDY_Pos)

/* RCC_CFGR bit definitions */
#define RCC_CFGR_SW_Pos           0U
#define RCC_CFGR_SW_Msk           (0x03UL << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SW_HSI           0x00000000U
#define RCC_CFGR_SW_HSE           0x00000001U
#define RCC_CFGR_SW_PLL           0x00000002U
#define RCC_CFGR_SWS_Pos          2U
#define RCC_CFGR_SWS_Msk          (0x03UL << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_SWS_HSI          0x00000000U
#define RCC_CFGR_SWS_HSE          0x00000004U
#define RCC_CFGR_SWS_PLL          0x00000008U
#define RCC_CFGR_PLLSRC_Pos       16U
#define RCC_CFGR_PLLSRC_Msk       (0x01UL << RCC_CFGR_PLLSRC_Pos)
#define RCC_CFGR_PLLSRC_HSI       0x00000000U
#define RCC_CFGR_PLLSRC_HSE       0x00010000U
#define RCC_CFGR_PLLMULL_Pos      18U
#define RCC_CFGR_PLLMULL_Msk      (0x0FUL << RCC_CFGR_PLLMULL_Pos)
#define RCC_CFGR_PLLMULL2         0x00000000U
#define RCC_CFGR_PLLMULL9         0x001C0000U
#define RCC_CFGR_PLLMULL16        0x00380000U
#define RCC_CFGR_HPRE_Pos         4U
#define RCC_CFGR_HPRE_Msk         (0x0FUL << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_DIV1        0x00000000U
#define RCC_CFGR_PPRE1_Pos        8U
#define RCC_CFGR_PPRE1_Msk        (0x07UL << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE1_DIV1       0x00000000U
#define RCC_CFGR_PPRE1_DIV2       0x00000400U
#define RCC_CFGR_PPRE2_Pos        11U
#define RCC_CFGR_PPRE2_Msk        (0x07UL << RCC_CFGR_PPRE2_Pos)
#define RCC_CFGR_PPRE2_DIV1       0x00000000U
#define RCC_CFGR_PPRE2_DIV2       0x00002000U

/* RCC_AHBENR bit definitions */
#define RCC_AHBENR_DMA1EN_Pos     0U
#define RCC_AHBENR_DMA1EN_Msk     (0x01UL << RCC_AHBENR_DMA1EN_Pos)
#define RCC_AHBENR_FLITFEN_Pos    4U
#define RCC_AHBENR_FLITFEN_Msk    (0x01UL << RCC_AHBENR_FLITFEN_Pos)
#define RCC_AHBENR_SRAMEN_Pos     2U
#define RCC_AHBENR_SRAMEN_Msk     (0x01UL << RCC_AHBENR_SRAMEN_Pos)

/* RCC_APB2ENR bit definitions */
#define RCC_APB2ENR_AFIOEN_Pos    0U
#define RCC_APB2ENR_AFIOEN_Msk    (0x01UL << RCC_APB2ENR_AFIOEN_Pos)
#define RCC_APB2ENR_IOPAEN_Pos    2U
#define RCC_APB2ENR_IOPAEN_Msk    (0x01UL << RCC_APB2ENR_IOPAEN_Pos)
#define RCC_APB2ENR_IOPBEN_Pos    3U
#define RCC_APB2ENR_IOPBEN_Msk    (0x01UL << RCC_APB2ENR_IOPBEN_Pos)
#define RCC_APB2ENR_IOPCEN_Pos    4U
#define RCC_APB2ENR_IOPCEN_Msk    (0x01UL << RCC_APB2ENR_IOPCEN_Pos)
#define RCC_APB2ENR_IOPDEN_Pos    5U
#define RCC_APB2ENR_IOPDEN_Msk    (0x01UL << RCC_APB2ENR_IOPDEN_Pos)
#define RCC_APB2ENR_IOPEEN_Pos    6U
#define RCC_APB2ENR_IOPEEN_Msk    (0x01UL << RCC_APB2ENR_IOPEEN_Pos)
#define RCC_APB2ENR_IOPFEN_Pos    7U
#define RCC_APB2ENR_IOPFEN_Msk    (0x01UL << RCC_APB2ENR_IOPFEN_Pos)
#define RCC_APB2ENR_IOPGEN_Pos    8U
#define RCC_APB2ENR_IOPGEN_Msk    (0x01UL << RCC_APB2ENR_IOPGEN_Pos)

/* RCC_APB1ENR bit definitions */
#define RCC_APB1ENR_TIM2EN_Pos    0U
#define RCC_APB1ENR_TIM2EN_Msk    (0x01UL << RCC_APB1ENR_TIM2EN_Pos)

/*******************************************************************************
 *                GPIO Register Structure
 ******************************************************************************/
typedef struct
{
  __IOM uint32_t CRL;            /*!< GPIO Port Configuration Register Low,       Address offset: 0x00 */
  __IOM uint32_t CRH;            /*!< GPIO Port Configuration Register High,      Address offset: 0x04 */
  __IOM uint32_t IDR;            /*!< GPIO Port Input Data Register,              Address offset: 0x08 */
  __IOM uint32_t ODR;            /*!< GPIO Port Output Data Register,             Address offset: 0x0C */
  __IOM uint32_t BSRR;           /*!< GPIO Port Bit Set/Reset Register,           Address offset: 0x10 */
  __IOM uint32_t BRR;            /*!< GPIO Port Bit Reset Register,               Address offset: 0x14 */
  __IOM uint32_t LCKR;           /*!< GPIO Port Configuration Lock Register,      Address offset: 0x18 */
} GPIO_TypeDef;

/*******************************************************************************
 *                AFIO Register Structure (minimal)
 ******************************************************************************/
typedef struct
{
  __IOM uint32_t EVCR;
  __IOM uint32_t MAPR;
  __IOM uint32_t EXTICR[4];
       uint32_t RESERVED0;
  __IOM uint32_t MAPR2;
} AFIO_TypeDef;

/*******************************************************************************
 *                FLASH Register Structure (minimal)
 ******************************************************************************/
typedef struct
{
  __IOM uint32_t ACR;
} FLASH_TypeDef;

/* FLASH_ACR bit definitions */
#define FLASH_ACR_LATENCY_Pos     0U
#define FLASH_ACR_LATENCY_Msk     (0x07UL << FLASH_ACR_LATENCY_Pos)
#define FLASH_ACR_LATENCY_0       0x00000000U
#define FLASH_ACR_LATENCY_1       0x00000001U
#define FLASH_ACR_LATENCY_2       0x00000002U

/*******************************************************************************
 *                GPIO Bit-level Macros
 ******************************************************************************/
#define GPIO_Pin_0               ((uint16_t)0x0001)
#define GPIO_Pin_1               ((uint16_t)0x0002)
#define GPIO_Pin_2               ((uint16_t)0x0004)
#define GPIO_Pin_3               ((uint16_t)0x0008)
#define GPIO_Pin_4               ((uint16_t)0x0010)
#define GPIO_Pin_5               ((uint16_t)0x0020)
#define GPIO_Pin_6               ((uint16_t)0x0040)
#define GPIO_Pin_7               ((uint16_t)0x0080)
#define GPIO_Pin_8               ((uint16_t)0x0100)
#define GPIO_Pin_9               ((uint16_t)0x0200)
#define GPIO_Pin_10              ((uint16_t)0x0400)
#define GPIO_Pin_11              ((uint16_t)0x0800)
#define GPIO_Pin_12              ((uint16_t)0x1000)
#define GPIO_Pin_13              ((uint16_t)0x2000)
#define GPIO_Pin_14              ((uint16_t)0x4000)
#define GPIO_Pin_15              ((uint16_t)0x8000)
#define GPIO_Pin_All             ((uint16_t)0xFFFF)

/** @defgroup GPIO_mode_define GPIO mode define
  * @{
  */
#define GPIO_MODE_INPUT          0x00000000U   /*!< Input Floating Mode */
#define GPIO_MODE_OUTPUT_10MHz   0x00000001U   /*!< Output Mode, max speed 10MHz */
#define GPIO_MODE_OUTPUT_2MHz    0x00000002U   /*!< Output Mode, max speed 2MHz */
#define GPIO_MODE_OUTPUT_50MHz   0x00000003U   /*!< Output Mode, max speed 50MHz */

/** @defgroup GPIO_cnf_define GPIO configuration define
  * @{
  */
#define GPIO_CNF_INPUT_ANALOG    0x00000000U   /*!< Analog Mode */
#define GPIO_CNF_INPUT_FLOATING  0x00000004U   /*!< Floating Input */
#define GPIO_CNF_INPUT_PU_PD     0x00000008U   /*!< Input with Pull-up / Pull-down */

#define GPIO_CNF_OUTPUT_PP       0x00000000U   /*!< General Purpose Output Push-Pull */
#define GPIO_CNF_OUTPUT_OD       0x00000004U   /*!< General Purpose Output Open-Drain */
#define GPIO_CNF_ALTF_PP         0x00000008U   /*!< Alternate Function Output Push-Pull */
#define GPIO_CNF_ALTF_OD         0x0000000CU   /*!< Alternate Function Output Open-Drain */

/*******************************************************************************
 *                Convenience Macros for GPIO Setup
 ******************************************************************************/
#define GPIO_OUTPUT_PP_2MHZ      (GPIO_MODE_OUTPUT_2MHz | GPIO_CNF_OUTPUT_PP)
#define GPIO_OUTPUT_PP_10MHZ     (GPIO_MODE_OUTPUT_10MHz | GPIO_CNF_OUTPUT_PP)
#define GPIO_OUTPUT_PP_50MHZ     (GPIO_MODE_OUTPUT_50MHz | GPIO_CNF_OUTPUT_PP)

/* Set pin mode + CNF in CRL (pins 0-7) or CRH (pins 8-15) */
#define GPIO_SET_PIN_MODE(gpio, pin, mode_cnf) do {                    \
    uint32_t _pin_shift = ((uint32_t)((pin) & 0x07U)) * 4U;            \
    uint32_t _mask = ~(0x0FUL << _pin_shift);                          \
    uint32_t _val  = ((uint32_t)(mode_cnf)) << _pin_shift;             \
    if ((pin) < 8U) {                                                  \
      (gpio)->CRL = ((gpio)->CRL & _mask) | _val;                     \
    } else {                                                           \
      (gpio)->CRH = ((gpio)->CRH & _mask) | _val;                     \
    }                                                                  \
  } while(0)

#define GPIO_SET_BIT(gpio, pin)   ((gpio)->BSRR = (uint32_t)(pin))
#define GPIO_RESET_BIT(gpio, pin) ((gpio)->BRR  = (uint32_t)(pin))

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_H */
