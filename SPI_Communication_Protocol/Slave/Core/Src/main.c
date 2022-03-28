#include "main.h"
#include <stdint.h>
#define MISO_PORT GPIOA
#define MISO_PIN GPIO_PIN_4
#define SCLK_PORT GPIOA
#define SCLK_PIN GPIO_PIN_5
#define MOSI_PORT GPIOA
#define MOSI_PIN GPIO_PIN_6
#define SS_PORT GPIOA
#define SS_PIN GPIO_PIN_7

uint8_t data_Slave_out = 12;
uint8_t kq;

long last = 0;
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
uint8_t SPI_Salve_Transfer(uint8_t data_Slave_out);

int main(void)
{

      HAL_Init();
	  SystemClock_Config();
	  MX_GPIO_Init();
	  HAL_GPIO_WritePin(MISO_PORT, MISO_PIN, 0);
	  while (1)
	  {
			kq = SPI_Salve_Transfer(data_Slave_out);

	  }
}
uint8_t SPI_Salve_Transfer(uint8_t data_Slave_out)
{
	uint8_t data_Slave_in = 0;//0000.0000
	uint8_t i,res;
	while(HAL_GPIO_ReadPin(SS_PORT, SS_PIN) == 1);// Đợi trạng thái cb bắt đầu kết nối của Master khi chân SS đc kéo xuống mức 0
	for (i = 0x80; i > 0; i = i >> 1)// đưa các bit lên chân MISO của Slave
	{
		res = data_Slave_out & i; //X000.000 & 1000.0000, 0X00.0000 & 0100.0000,...
		if(res > 0)
		{
			HAL_GPIO_WritePin(MISO_PORT, MISO_PIN, 1);
		}
		else
		{
			HAL_GPIO_WritePin(MISO_PORT, MISO_PIN, 0);
		}
		while(HAL_GPIO_ReadPin(SCLK_PORT, SCLK_PIN) == 0);// đợi đến khi SCK lên 1 để đọc dữ liệu
		if(HAL_GPIO_ReadPin(MOSI_PORT, MOSI_PIN) == 1)
		{
			data_Slave_in = data_Slave_in | i; //set bit trong data_Slave_in lên 1 tại vị trí i
		}
		while(HAL_GPIO_ReadPin(SCLK_PORT, SCLK_PIN) == 1);// đợi SCK xuống mức thấp
		// tại đây, kết thúc 1 chu kỳ clock
	}
	return data_Slave_in;
}



void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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

