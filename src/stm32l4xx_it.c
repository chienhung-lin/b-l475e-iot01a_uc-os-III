/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/

/*
 * NMI exception handler is weak defined in system/src/cortexm/exception-handlers.c,
 * override the original by declaring your own handler without weak attribute.
 *
 * Example:
 * void __attribute__ ((section(".after_vectors"), naked)) NMI_Handler(void)
 * {
 *   ...
 * }
 */

/*
 * HardFault exception handler is weak defined in system/src/cortexm/exception-handlers.c,
 * override the original by declaring your own handler without weak attribute.
 *
 * Example:
 * void __attribute__ ((section(".after_vectors"), naked)) HardFault_Handler(void)
 * {
 *   ...
 * }
 */

/*
 * Memory manage fault exception handler is weak defined in
 * system/src/cortexm/exception-handlers.c, override the original by declaring
 * your own handler without weak attribute.
 *
 * Example:
 * void __attribute__ ((section(".after_vectors"), naked)) MemManage_Handler(void)
 * {
 *   ...
 * }
 */

/*
 * BusFault exception handler is weak defined in system/src/cortexm/exception-handlers.c,
 * override the original by declaring your own handler without weak attribute.
 *
 * Example:
 * void __attribute__ ((section(".after_vectors"), naked)) BusFault_Handler(void)
 * {
 *   ...
 * }
 */

/*
 * Usage fault exception handler is weak defined in system/src/cortexm/exception-handlers.c,
 * override the original by declaring your own handler without weak attribute.
 *
 * Example:
 * void __attribute__ ((section(".after_vectors"), naked)) UsageFault_Handler(void)
 * {
 *   ...
 * }
 */

/*
 * Debug monitor exception handler is weak defined in system/src/cortexm/exception-handlers.c,
 * override the original by declaring your own handler without weak attribute.
 *
 * Example:
 * void __attribute__ ((section(".after_vectors"), naked)) DebugMon_Handler(void)
 * {
 *   ...
 * }
 */

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM6 global interrupt, DAC channel1 and channel2 underrun error interrupts.
  */
void __attribute__ ((section(".after_vectors"))) TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
