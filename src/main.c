/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "diag/trace.h"

#include "stm32l4xx_hal.h"

#include "os.h"

#include "app_cfg.h"

/* --------------------------------------------
 * static function prototype declaration
 * --------------------------------------------*/

/* Hardware initialization functions */
static void SystemClock_Config(void);
static void Error_Handler(void);

/* task function declaration */
static void AppTaskStartup(void *);
static void AppTaskApp1(void *);
static void AppTaskApp2(void *);


/* ---------------------------------------------
 * static variable declaration
 * ---------------------------------------------*/

/* Task TCB declaration */
static OS_TCB StartupTaskTCB;
static OS_TCB App1TaskTCB;
static OS_TCB App2TaskTCB;

/* Task stack declaration */
static CPU_STK AppTaskStartStk[APP_CFG_STARTUP_TASK_STK_SIZE];
static CPU_STK AppTaskApp1Stk[APP_CFG_APP1_TASK_STK_SIZE];
static CPU_STK AppTaskApp2Stk[APP_CFG_APP2_TASK_STK_SIZE];

// ----------------------------------------------------------------------------
//
// Print a greeting message on the trace device and enter a loop
// to count seconds.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the ITM output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace-impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//
// ----------------------------------------------------------------------------

int
main (int argc, char* argv[])
{
  OS_ERR err;

  (void)argc;
  (void)argv;

  // Move further hardware initialization step from src/initialize-hardware.c
  // onto here.
  HAL_Init();

  // Set new clock source. In here, the clock source is set from PLL, and the PLL
  // source is from HSI. The clock is at maximum 80MHz.
  // There is no need to call SystemClockUpdate to update SystemCoreClock or
  // Init_Ticks because HAL_RCC_Config already sets up them.
  SystemClock_Config();

  // In this sample the SystemInit() function is just a placeholder,
  // if you do not add the real one, the clock will remain configured with
  // the reset value, usually a relatively low speed RC clock (8-12MHz).

  // Send a greeting to the trace device (skipped on Release).
  trace_puts("stdout: Hello Arm World!");

  // At this stage the system clock should have already been configured
  // at high speed.
  trace_printf("stdout: System clock: %u Hz\n", SystemCoreClock);

  // uC-OSIII data structures initialization
  OSInit(&err);
  if (err != OS_ERR_NONE) {
      trace_printf("stderr: OSInit error [%u] \n", err);
      abort();
  }

  // create startup task
  OSTaskCreate(
      (OS_TCB *     ) &StartupTaskTCB,      // TCB structure
      (CPU_CHAR *   ) "Startup Task",       // ASCII string as Task name
      (OS_TASK_PTR  ) AppTaskStartup,       // task code
      (void *       ) 0,                    // task parameter
      (OS_PRIO      ) APP_CFG_STARTUP_TASK_PRIO,
                                            // task priority

      (CPU_STK *    ) &AppTaskStartStk[0],  // task stack base address
      (CPU_STK_SIZE ) APP_CFG_STARTUP_TASK_STK_SIZE / 10,
                                            // task stack limit size

      (CPU_STK_SIZE ) APP_CFG_STARTUP_TASK_STK_SIZE,
                                            // task stack size

      (OS_MSG_QTY   ) 0,                    // task queue size
      (OS_TICK      ) 0,                    // RR schedule time quantum
                                            // use default value by setting 0
      (void *       ) 0,                    // TCB memory extension
      (OS_OPT       ) (OS_OPT_TASK_NONE),   // Task option, ex: stk_chk,
                                            // stk_clr, save_fp, no_tls
      (OS_ERR *     ) &err                  // error code
  );
  if (err != OS_ERR_NONE) {
      trace_printf("stderr: OSTaskCreate error [%u] \n", err);
      while (1) ;
  }

  // Start scheduling first task.
  // Note that systick is disable and will be enable in startup task.
  OSStart(&err);

  // Never reach here because OSStart contains infinite loop
  if (err != OS_ERR_NONE) {
      trace_printf("stderr: OSStart error [%u] \n", err);
      while (1) ;
  }

  // Never reach end.
  // add return -1 for eliminating compile warning
  return -1;
}

/**
  * @brief System Clock Configuration
  * @note  This function overrides the weak one in initialize-hardware.c
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

void AppTaskStartup(void *p_arg)
{
  OS_ERR err;
  OS_TICK tick;

  // depress compiling unused parameter warning
  (void)p_arg;

  // Initial system tick and enable tick interrupt
  OS_CPU_SysTickInitFreq(SystemCoreClock);

  // create app1 task
  OSTaskCreate(
      (OS_TCB *     ) &App1TaskTCB,      // TCB structure
      (CPU_CHAR *   ) "App1 Task",       // ASCII string as Task name
      (OS_TASK_PTR  ) AppTaskApp1,       // task code
      (void *       ) 0,                    // task parameter
      (OS_PRIO      ) APP_CFG_APP1_TASK_PRIO,
                                            // task priority

      (CPU_STK *    ) &AppTaskApp1Stk[0],  // task stack base address
      (CPU_STK_SIZE ) APP_CFG_APP1_TASK_STK_SIZE / 10,
                                            // task stack limit size

      (CPU_STK_SIZE ) APP_CFG_APP1_TASK_STK_SIZE,
                                            // task stack size

      (OS_MSG_QTY   ) 0,                    // task queue size
      (OS_TICK      ) 0,                    // RR schedule time quantum
                                            // use default value by setting 0
      (void *       ) 0,                    // TCB memory extension
      (OS_OPT       ) (OS_OPT_TASK_NONE),   // Task option, ex: stk_chk,
                                            // stk_clr, save_fp, no_tls
      (OS_ERR *     ) &err                  // error code
  );
  if (err != OS_ERR_NONE) {
      trace_printf("stderr: Startup Task - create App1 Task [%u]\n", err);
      while (1) ;
  }

  // create app2 task
  OSTaskCreate(
      (OS_TCB *     ) &App2TaskTCB,      // TCB structure
      (CPU_CHAR *   ) "App2 Task",       // ASCII string as Task name
      (OS_TASK_PTR  ) AppTaskApp2,       // task code
      (void *       ) 0,                    // task parameter
      (OS_PRIO      ) APP_CFG_APP2_TASK_PRIO,
                                            // task priority

      (CPU_STK *    ) &AppTaskApp2Stk[0],  // task stack base address
      (CPU_STK_SIZE ) APP_CFG_APP2_TASK_STK_SIZE / 10,
                                            // task stack limit size

      (CPU_STK_SIZE ) APP_CFG_APP1_TASK_STK_SIZE,
                                            // task stack size

      (OS_MSG_QTY   ) 0,                    // task queue size
      (OS_TICK      ) 0,                    // RR schedule time quantum
                                            // use default value by setting 0
      (void *       ) 0,                    // TCB memory extension
      (OS_OPT       ) (OS_OPT_TASK_NONE),   // Task option, ex: stk_chk,
                                            // stk_clr, save_fp, no_tls
      (OS_ERR *     ) &err                  // error code
  );
  if (err != OS_ERR_NONE) {
      trace_printf("stderr: Startup Task - create App2 Task [%u]\n", err);
      while (1) ;
  }

  while (1) {

      tick = OSTimeGet(&err);
      if (err != OS_ERR_NONE) {
          trace_printf("stderr: OSTimeGet error [%u] \n", err);
          while (1) ;
      }
      trace_printf("stdout: Startup Task - Tick %u\n", tick);

      OSTimeDly((OS_TICK) 1000, (OS_OPT)OS_OPT_TIME_DLY, &err);
      if (err != OS_ERR_NONE) {
          trace_printf("stderr: Startup Task - OSTimeDly error [%u] \n", err);
          while (1) ;
      }
  }
}

void AppTaskApp1(void *p_arg)
{
  OS_ERR err;
  OS_TICK tick;

  (void)p_arg;

  while (1) {

    tick = OSTimeGet(&err);
    if (err != OS_ERR_NONE) {
        trace_printf("stderr: App1 Task - OSTimeGet error [%u] \n", err);
        while (1) ;
    }
    trace_printf("stdout: App1 Task - Tick %u\n", tick);

    OSTimeDly((OS_TICK) 500, (OS_OPT)OS_OPT_TIME_DLY, &err);
    if (err != OS_ERR_NONE) {
        trace_printf("stderr: App1 Task - OSTimeDly error [%u] \n", err);
        while (1) ;
    }
  }
}

void AppTaskApp2(void *p_arg)
{
  OS_ERR err;
  OS_TICK tick;

  (void)p_arg;

  while (1) {

    tick = OSTimeGet(&err);
    if (err != OS_ERR_NONE) {
        trace_printf("stderr: App2 Task - OSTimeGet error [%u] \n", err);
        while (1) ;
    }
    trace_printf("stdout: App2 Task - Tick %u\n", tick);

    OSTimeDly((OS_TICK) 250, (OS_OPT)OS_OPT_TIME_DLY, &err);
    if (err != OS_ERR_NONE) {
        trace_printf("stderr: App2 Task - OSTimeDly error [%u] \n", err);
        while (1) ;
    }
  }
}

/* The timer6 is only used for HAL Library. The uCOS-III tick uses SysTick. */
/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 /* USER CODE BEGIN Callback 0 */

 /* USER CODE END Callback 0 */
 if (htim->Instance == TIM6) {
   HAL_IncTick();
 }
 /* USER CODE BEGIN Callback 1 */

 /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

// ----------------------------------------------------------------------------
