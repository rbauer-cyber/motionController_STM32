/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motion.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define USE_BSP
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

static int32_t s_encoderPosition = 0;

int32_t encoderUpdate()
{
	s_encoderPosition = __HAL_TIM_GET_COUNTER(&htim3);

	if ( s_encoderPosition > 32767 )
	{
		s_encoderPosition = s_encoderPosition - 65535;
	}

	s_encoderPosition /= 4;

	return( s_encoderPosition );
}

void encoderInit()
{
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Use Timer 2 count register to measure elapsed time in micro-seconds
// Timer 2 runs at 50 MHZ, clk tick rate is 20 nano-seconds.
volatile uint32_t s_lastTimeNs = 0;
volatile uint64_t s_elapsedTimeNs = 0;
const uint32_t s_maxTimeNs = 4294967295;

uint64_t getMicros()
{
	// TIM2 is running at 50 MHz, each tick is 20 nanos.
	uint32_t currentTimeNs = (__HAL_TIM_GetCounter(&htim2) * 20);
	uint32_t elapsedTimeNs;
	uint64_t elapsedTimeUs;

	// Detect timer overflow, max time value is 4294967295 (32 bit timer)
	// and adjust the elapsed time for the overflow.
	// The time overflows in 85 seconds.
	if ( currentTimeNs > s_lastTimeNs )
	{
		elapsedTimeNs = currentTimeNs - s_lastTimeNs;
	}
	else
	{
		elapsedTimeNs = (s_maxTimeNs - s_lastTimeNs) + currentTimeNs + 1;
	}

	s_lastTimeNs = currentTimeNs;
	s_elapsedTimeNs += elapsedTimeNs;
	elapsedTimeUs = s_elapsedTimeNs / 1000;
	return elapsedTimeUs;
}

// Use interrupts from Timer 2 to measure elapsed time in micro-seconds
// Timer 2 runs at 50 MHZ, period is 20 micro-seconds.
//volatile uint32_t s_elapsedTimeUs = 0;
//
//uint32_t getMicros()
//{
//	// Resolution is 2 micro-secs using TIM2 at 50 MHz, counter period set 99, 2us
//	return s_elapsedTimeUs;
//}
//
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	/* Prevent unused argument(s) compilation warning */
//	UNUSED(htim);
//
//	if ( htim == &htim2 )
//	{
//		//Timer 2 period is 2 micro-secs, 100 counts with auto-reload
//		s_elapsedTimeUs += 2;
//#if 0
//		// Use this technique for Stepper acceleration control.
//		if ( s_timerCount >= 10000 )
//		{
//			s_timerCount = 0;
//			g_timerInterruptFlag = 1;
//
//			if ( ++s_timerChangeCount > 7 )
//			{
//				// The Frequency of the timer is 50 MHz.
//				// Change the period of the timer pulses
//				s_timerPeriod = (s_timerPeriod == 5000) ? 2500 : 5000;
//				__HAL_TIM_SET_AUTORELOAD(&htim4, s_timerPeriod);
//				s_timerChangeCount = 0;
//			}
//		}
//#endif
//	}
//}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  // htim2 is configured for reading a quadrature encoder by the HAL
  // The knob device is the interface to the encoder using htim2.
  //encoderInit();
  // Initializes encoder.
  BSP_initKnob();
  HAL_TIM_Base_Start(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // This is a C++ function allowing project to support C++ classes
	  // defined in the Cpp project subfolder; altMain never returns.
	  altMain();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
