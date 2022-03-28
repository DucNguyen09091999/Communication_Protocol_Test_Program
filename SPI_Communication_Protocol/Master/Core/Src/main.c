#include "main.h"
#include <stdint.h>
#include "cJSON.h"
#define SCLK_PORT GPIOA
#define SCLK_PIN GPIO_PIN_5 // SCK OF MASTER
#define MOSI_PORT GPIOA
#define MOSI_PIN GPIO_PIN_6 // MOSI OF MASTER
#define MISO_PORT GPIOA
#define MISO_PIN GPIO_PIN_4 // MISO OF MASTER
#define SS_PORT GPIOA
#define SS_PIN GPIO_PIN_7 // SS OF MASTER



uint8_t data_Master_out = 12;
uint8_t rev;
long last = 0;
int i = 0;
char Str_data_Master_out[100];
char Str_rev[100];
char JSON[100];
char rx_buffer[200];
uint8_t rx_data;
unsigned int rx_index = 0;

cJSON *str_json, *st_data_Master_out , *st_rev ,*st_Mode;

UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void SPI_Soft_init(void);
void SPI_Start(void);
void SPI_End(void);
uint8_t SPI_Master_Transfer(uint8_t data_out);
void SendData(uint8_t data_Master_out,uint8_t rev);

int main(void)
{

	HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
	MX_USART1_UART_Init();
	SPI_Soft_init();
    last = HAL_GetTick();
  while (1)
  {

	  if(HAL_GetTick()- last >= 1000)
	 	  {
		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	 	  SPI_Start();// SS = 0
	 	  rev = SPI_Master_Transfer(data_Master_out);
	 	  SPI_End();// SS = 1
	 	  SendData(data_Master_out,rev);
	 	  last = HAL_GetTick();
	 	  }
  }

}
void SPI_Soft_init(void)
{
	HAL_GPIO_WritePin(SCLK_PORT,SCLK_PIN , 0);
	HAL_GPIO_WritePin(MOSI_PORT,MOSI_PIN , 0);
    HAL_GPIO_WritePin(SS_PORT, SS_PIN, 1);
}
void SPI_Start(void)
{
	HAL_GPIO_WritePin(SS_PORT, SS_PIN, 0);
}

void SPI_End(void)
{
	HAL_GPIO_WritePin(SCLK_PORT,SCLK_PIN , 0);
	HAL_GPIO_WritePin(SS_PORT, SS_PIN, 1);

}
//MODE CPOL = 0; CPHA= 0 , bitOder = MSB
uint8_t SPI_Master_Transfer(uint8_t data_Master_out)
{
	uint8_t data_Master_in = 0;//0000.0000
	uint8_t i,results;
	for(i=0x80; i>0; i= i>>1)     // i = 1000.0000, re = data_Master_out & 1000.0000 = X000.0000 #= or = 0 0x40
	{
		results = data_Master_out & i;// đẩy bit MSB và các bit còn lại lên chân MOSI của MASTER
		if(results > 0)
		{
			HAL_GPIO_WritePin(MOSI_PORT, MOSI_PIN, 1);
		}
		else
		{
			HAL_GPIO_WritePin(MOSI_PORT, MOSI_PIN, 0);
		}
		HAL_Delay(5);//delay nữa chu kì
		HAL_GPIO_WritePin(SCLK_PORT,SCLK_PIN , 1);// gửi đi tín hiệu xung clock ( mức cao)
		if(HAL_GPIO_ReadPin(MISO_PORT, MISO_PIN) == 1)
		{
			data_Master_in = data_Master_in | i; // 0000.0000 | 1000.0000, X000.0000 | 0100.0000, XX00.0000 | 0010.0000,.....
		}
		HAL_Delay(5);
		HAL_GPIO_WritePin(SCLK_PORT,SCLK_PIN , 0);// 1 clock end để chuyền 1 bit
	}
	return data_Master_in;
}

void SendData(uint8_t data_Master_out,uint8_t rev)
{
	for (int i = 0 ; i < 100 ; i++)
	{
		JSON[i] = 0;
		Str_data_Master_out[i] = 0;
		Str_rev[i] = 0;



	}

	sprintf(Str_data_Master_out, "%d",  data_Master_out);
	sprintf(Str_rev, "%d", rev);



	strcat(JSON,"{\"Master send to Slave\":\"");
	strcat(JSON,Str_data_Master_out);
	strcat(JSON,"\",");


	strcat(JSON,"\"Master receive from Slave\":\"");
	strcat(JSON,Str_rev);
	strcat(JSON,"\"}\n");

	HAL_UART_Transmit(&huart1,(uint8_t *)&JSON,strlen(JSON),1000); // PC

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
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */

static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

