/**
 * Startup file for STM32F103C8T6 (Cortex-M3)
 * GCC assembler syntax
 */

.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.global g_pfnVectors
.global Default_Handler

/**
 * @brief  Provide _init and _fini stubs for libc.
 */
.section .text._init,"ax",%progbits
.weak _init
.type _init, %function
_init:
  bx lr
.size _init, .-_init

.section .text._fini,"ax",%progbits
.weak _fini
.type _fini, %function
_fini:
  bx lr
.size _fini, .-_fini

/**
 * @brief  This is the code that gets called when the processor first
 *         starts execution following a reset event.
 */
.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
  /* Copy the data segment initializers from flash to SRAM */
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  cmp r0, r1
  beq InitBss

CopyData:
  ldr r3, [r2], #4
  str r3, [r0], #4
  cmp r0, r1
  bne CopyData

InitBss:
  /* Zero fill the bss segment */
  ldr r0, =_sbss
  ldr r1, =_ebss
  movs r2, #0
  cmp r0, r1
  beq CallStaticInit

ZeroBss:
  str r2, [r0], #4
  cmp r0, r1
  bne ZeroBss

CallStaticInit:
  /* Call static constructors */
  bl __libc_init_array

  /* Call SystemInit */
  bl SystemInit

  /* Call main */
  bl main

  /* Infinite loop if main returns */
LoopForever:
  b LoopForever

.size Reset_Handler, .-Reset_Handler

/**
 * @brief  Default handler for all other exceptions/interrupts.
 *         Weak aliases make them overridable.
 */
.section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
.size Default_Handler, .-Default_Handler

/******************************************************************************
 * Weak aliases for each exception handler to Default_Handler.
 ******************************************************************************/
.macro def_handler handler_name
  .weak \handler_name
  .thumb_set \handler_name, Default_Handler
.endm

/* Must define these prototypes correctly */
def_handler NMI_Handler
def_handler HardFault_Handler
def_handler MemManage_Handler
def_handler BusFault_Handler
def_handler UsageFault_Handler
def_handler SVC_Handler
def_handler DebugMon_Handler
def_handler PendSV_Handler
def_handler SysTick_Handler

/******************************************************************************
 * The vector table.
 ******************************************************************************/
.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word _estack                 /* Top of Stack */
  .word Reset_Handler           /* Reset Handler */
  .word NMI_Handler             /* NMI Handler */
  .word HardFault_Handler       /* Hard Fault Handler */
  .word MemManage_Handler       /* MPU Fault Handler */
  .word BusFault_Handler        /* Bus Fault Handler */
  .word UsageFault_Handler      /* Usage Fault Handler */
  .word 0                       /* Reserved */
  .word 0                       /* Reserved */
  .word 0                       /* Reserved */
  .word 0                       /* Reserved */
  .word SVC_Handler             /* SVCall Handler */
  .word DebugMon_Handler        /* Debug Monitor Handler */
  .word 0                       /* Reserved */
  .word PendSV_Handler          /* PendSV Handler */
  .word SysTick_Handler         /* SysTick Handler */
  /* External Interrupts */
  .word WWDG_IRQHandler         /* Window Watchdog */
  .word PVD_IRQHandler          /* PVD through EXTI Line detect */
  .word TAMPER_IRQHandler       /* Tamper */
  .word RTC_IRQHandler          /* RTC */
  .word FLASH_IRQHandler        /* Flash */
  .word RCC_IRQHandler          /* RCC */
  .word EXTI0_IRQHandler        /* EXTI Line 0 */
  .word EXTI1_IRQHandler        /* EXTI Line 1 */
  .word EXTI2_IRQHandler        /* EXTI Line 2 */
  .word EXTI3_IRQHandler        /* EXTI Line 3 */
  .word EXTI4_IRQHandler        /* EXTI Line 4 */
  .word DMA1_Channel1_IRQHandler   /* DMA1 Channel 1 */
  .word DMA1_Channel2_IRQHandler   /* DMA1 Channel 2 */
  .word DMA1_Channel3_IRQHandler   /* DMA1 Channel 3 */
  .word DMA1_Channel4_IRQHandler   /* DMA1 Channel 4 */
  .word DMA1_Channel5_IRQHandler   /* DMA1 Channel 5 */
  .word DMA1_Channel6_IRQHandler   /* DMA1 Channel 6 */
  .word DMA1_Channel7_IRQHandler   /* DMA1 Channel 7 */
  .word ADC1_2_IRQHandler       /* ADC1 & ADC2 */
  .word USB_HP_CAN1_TX_IRQHandler  /* USB High Priority / CAN1 TX */
  .word USB_LP_CAN1_RX0_IRQHandler /* USB Low Priority / CAN1 RX0 */
  .word CAN1_RX1_IRQHandler     /* CAN1 RX1 */
  .word CAN1_SCE_IRQHandler     /* CAN1 SCE */
  .word EXTI9_5_IRQHandler      /* EXTI Line 9..5 */
  .word TIM1_BRK_IRQHandler     /* TIM1 Break */
  .word TIM1_UP_IRQHandler      /* TIM1 Update */
  .word TIM1_TRG_COM_IRQHandler /* TIM1 Trigger and Commutation */
  .word TIM1_CC_IRQHandler      /* TIM1 Capture Compare */
  .word TIM2_IRQHandler         /* TIM2 */
  .word TIM3_IRQHandler         /* TIM3 */
  .word TIM4_IRQHandler         /* TIM4 */
  .word I2C1_EV_IRQHandler      /* I2C1 Event */
  .word I2C1_ER_IRQHandler      /* I2C1 Error */
  .word I2C2_EV_IRQHandler      /* I2C2 Event */
  .word I2C2_ER_IRQHandler      /* I2C2 Error */
  .word SPI1_IRQHandler         /* SPI1 */
  .word SPI2_IRQHandler         /* SPI2 */
  .word USART1_IRQHandler       /* USART1 */
  .word USART2_IRQHandler       /* USART2 */
  .word USART3_IRQHandler       /* USART3 */
  .word EXTI15_10_IRQHandler    /* EXTI Line 15..10 */
  .word RTCAlarm_IRQHandler     /* RTC Alarm through EXTI Line */
  .word USBWakeUp_IRQHandler    /* USB Wakeup from suspend */
  /* ... remaining vectors */
  /* Pad to 16 + 44 = 60 vectors to accommodate all common F103 IRQs */
  .rept 46
  .word 0
  .endr

/******************************************************************************
 * Provide weak aliases for all external interrupts
 ******************************************************************************/
def_handler WWDG_IRQHandler
def_handler PVD_IRQHandler
def_handler TAMPER_IRQHandler
def_handler RTC_IRQHandler
def_handler FLASH_IRQHandler
def_handler RCC_IRQHandler
def_handler EXTI0_IRQHandler
def_handler EXTI1_IRQHandler
def_handler EXTI2_IRQHandler
def_handler EXTI3_IRQHandler
def_handler EXTI4_IRQHandler
def_handler DMA1_Channel1_IRQHandler
def_handler DMA1_Channel2_IRQHandler
def_handler DMA1_Channel3_IRQHandler
def_handler DMA1_Channel4_IRQHandler
def_handler DMA1_Channel5_IRQHandler
def_handler DMA1_Channel6_IRQHandler
def_handler DMA1_Channel7_IRQHandler
def_handler ADC1_2_IRQHandler
def_handler USB_HP_CAN1_TX_IRQHandler
def_handler USB_LP_CAN1_RX0_IRQHandler
def_handler CAN1_RX1_IRQHandler
def_handler CAN1_SCE_IRQHandler
def_handler EXTI9_5_IRQHandler
def_handler TIM1_BRK_IRQHandler
def_handler TIM1_UP_IRQHandler
def_handler TIM1_TRG_COM_IRQHandler
def_handler TIM1_CC_IRQHandler
def_handler TIM2_IRQHandler
def_handler TIM3_IRQHandler
def_handler TIM4_IRQHandler
def_handler I2C1_EV_IRQHandler
def_handler I2C1_ER_IRQHandler
def_handler I2C2_EV_IRQHandler
def_handler I2C2_ER_IRQHandler
def_handler SPI1_IRQHandler
def_handler SPI2_IRQHandler
def_handler USART1_IRQHandler
def_handler USART2_IRQHandler
def_handler USART3_IRQHandler
def_handler EXTI15_10_IRQHandler
def_handler RTCAlarm_IRQHandler
def_handler USBWakeUp_IRQHandler
