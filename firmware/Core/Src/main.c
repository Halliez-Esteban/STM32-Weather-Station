/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include <string.h>
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

// I2C with BME280 (Press,Hum,Temp Module)
uint8_t TX_Buffer_BME280 = 0 ; // DATA to send
uint16_t TX_Address_BME280 = 0 ; // Address to send to
uint8_t RX_Buffer_BME280 = 0 ; // DATA to receive from BME280
uint16_t RX_Address_BME280 = 0 ; // Address to read
uint8_t RX_DevAddress_BME280 = 0xEC; // 0x76 shifted once (Address on [7:1])
uint8_t RX_Buffer_BME280_Burst[BURST_SIZE];


//BME280 Compensation Reading
uint8_t RX_Buffer_BME280_Burst_Compensation_1[COMPENSATION_BURST_SIZE_1];
uint8_t RX_Buffer_BME280_Burst_Compensation_2[COMPENSATION_BURST_SIZE_2];
//BME280 Compensation Variables
unsigned short dig_T1,dig_P1;
signed short dig_T2,dig_T3,dig_P2,dig_P3,dig_P4,dig_P5,dig_P6,dig_P7,dig_P8,dig_P9,dig_H2,dig_H4,dig_H5;
unsigned char dig_H1,dig_H3;
signed char dig_H6;

//BME280 Data Conversion
uint32_t t_fine;
uint32_t humidity_raw = 0; //16bits
uint32_t temperature_raw = 0; //20bits
uint32_t pressure_raw = 0; //20bits
uint32_t humidity_compensated, temperature_compensated, pressure_compensated;
uint32_t temp_int,temp_dec,humid_int,humid_dec,press_int,press_dec; //Real Data readable (e.g 22.67degC)

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  char test[] = "--- TEST UART STM32 OK ---\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)test, strlen(test), HAL_MAX_DELAY);

  //Set up BME280 Configuration
  BME280_Init();

  //Getting Weather Data From BME280
  BME280_GetData();

  HAL_Delay(100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void BME280_Init(void) {

    // Humidity ON register 0xF2
    TX_Buffer_BME280 = 0x01; // x1 Oversampling
    HAL_I2C_Mem_Write(&hi2c1, RX_DevAddress_BME280, 0xF2, I2C_MEMADD_SIZE_8BIT, &TX_Buffer_BME280, 1, 100);

    // Pression, Température Configuration and switch to NORMAL MODE (0x27)
    // Bit [7:5] x1 Temp oversampling (001)
    // Bit [4:2] x1 Press oversampling (001)
    // Bit [1:0] NORMAL MODE (11) -> 0010 0111 = 0x27
    TX_Buffer_BME280 = 0x27;
    HAL_I2C_Mem_Write(&hi2c1, RX_DevAddress_BME280, 0xF4, I2C_MEMADD_SIZE_8BIT, &TX_Buffer_BME280, 1, 100);

    HAL_Delay(100);
}

void BME280_GetData(void){

		BME280_isConnected();

		//Burst Reading BME280 Registers (Humidity, Pressure, Temperature Datas)

		if(RX_Buffer_BME280 == (uint8_t)0x60){

			//Burst Reading : 111011X1 Read-Mode
		    //From 0xF7 to 0xFE to obtain all informations at once
			RX_DevAddress_BME280 = 0xEC;
			RX_Address_BME280 = 0xF7;

			//Reading  Press,Humid,Temp from BME280
			HAL_I2C_Mem_Read(&hi2c1,RX_DevAddress_BME280,RX_Address_BME280,I2C_MEMADD_SIZE_8BIT,RX_Buffer_BME280_Burst,BURST_SIZE,1000);

			//RX_Buffer_BME280_Burst[6:7] HUMIDITY
			humidity_raw = RX_Buffer_BME280_Burst[6]<<8 | RX_Buffer_BME280_Burst[7];
			//RX_Buffer_BME280_Burst[3:5] TEMPERATURE
			temperature_raw = RX_Buffer_BME280_Burst[3]<<12 | RX_Buffer_BME280_Burst[4]<<4 | RX_Buffer_BME280_Burst[5]>>4;
			//RX_Buffer_BME280_Burst[0:2] PRESSURE
			pressure_raw = RX_Buffer_BME280_Burst[0]<<12 | RX_Buffer_BME280_Burst[1]<<4 | RX_Buffer_BME280_Burst[2]>>4;

			//Printing data to Terminal via UART
			BME280_print_raw_data();

			//Getting Compensation values from BME280;
			BME280_compensation_read();

			//Conversion from Raw to Human-Readable Data
			//Conversion formulas are provided by Bosch in the BME280 Datasheet provided in /docs

			temperature_compensated = BME280_compensate_T_int32(temperature_raw);
			pressure_compensated = BME280_compensate_P_int64(pressure_raw);
			humidity_compensated = BME280_compensate_H_int32(humidity_raw);

			BME280_print_compensated_data();

		}
}

void BME280_isConnected(void)
{
	// Receive BME280 Chip Id (0x60 on default)
	RX_Address_BME280 = 0xD0;

	if(HAL_I2C_Mem_Read(&hi2c1,RX_DevAddress_BME280,RX_Address_BME280,I2C_MEMADD_SIZE_8BIT,&RX_Buffer_BME280,1,1000) == HAL_OK) //Sending in Interrupt mode
	{
		//Send Chip_Id to PC via UART (PutTy)
		char buf[64];
		sprintf(buf, "Chip Id: %X\r\n", RX_Buffer_BME280);
		HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
	}
	else
	{
		char buf[64];
		sprintf(buf, "Chip not connected \r\n");
		HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
	}
}

void BME280_print_raw_data(void)
{
	char buf[64];
	sprintf(buf, "--- RAW DATA FROM BME280 --- \r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);

	sprintf(buf, "Humidity raw : %lu \r\n", humidity_raw);
	HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);

	sprintf(buf, "Temperature raw : %lu \r\n", temperature_raw);
	HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);

	sprintf(buf, "Pressure raw : %lu \r\n", pressure_raw);
	HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
}

void BME280_compensation_read(void)
{
	//Reading Coefficients in the BME280 EEPROM Memory (Burst Reading)
	RX_Address_BME280 = 0x88;
	HAL_I2C_Mem_Read(&hi2c1,RX_DevAddress_BME280,RX_Address_BME280,I2C_MEMADD_SIZE_8BIT,RX_Buffer_BME280_Burst_Compensation_1,COMPENSATION_BURST_SIZE_1,1000);
	RX_Address_BME280 = 0xE1;
	HAL_I2C_Mem_Read(&hi2c1,RX_DevAddress_BME280,RX_Address_BME280,I2C_MEMADD_SIZE_8BIT,RX_Buffer_BME280_Burst_Compensation_2,COMPENSATION_BURST_SIZE_2,1000);

	//Parsing Burst Reading into Compensation variables
	dig_T1 = (unsigned short) (RX_Buffer_BME280_Burst_Compensation_1[1]<<8 | RX_Buffer_BME280_Burst_Compensation_1[0]);
	dig_T2 = (signed short) (RX_Buffer_BME280_Burst_Compensation_1[3]<<8 | RX_Buffer_BME280_Burst_Compensation_1[2]);
	dig_T3 = (signed short) (RX_Buffer_BME280_Burst_Compensation_1[5]<<8 | RX_Buffer_BME280_Burst_Compensation_1[4]);

	dig_P1 = (unsigned short) (RX_Buffer_BME280_Burst_Compensation_1[7]<<8 | RX_Buffer_BME280_Burst_Compensation_1[6]);
	dig_P2 = (signed short) (RX_Buffer_BME280_Burst_Compensation_1[9]<<8 | RX_Buffer_BME280_Burst_Compensation_1[8]);
	dig_P3 = (signed short) (RX_Buffer_BME280_Burst_Compensation_1[11]<<8 | RX_Buffer_BME280_Burst_Compensation_1[10]);
	dig_P4 = (signed short) (RX_Buffer_BME280_Burst_Compensation_1[13]<<8 | RX_Buffer_BME280_Burst_Compensation_1[12]);
	dig_P5 = (signed short) (RX_Buffer_BME280_Burst_Compensation_1[15]<<8 | RX_Buffer_BME280_Burst_Compensation_1[14]);
	dig_P6 = (signed short) (RX_Buffer_BME280_Burst_Compensation_1[17]<<8 | RX_Buffer_BME280_Burst_Compensation_1[16]);
	dig_P7 = (signed short) (RX_Buffer_BME280_Burst_Compensation_1[19]<<8 | RX_Buffer_BME280_Burst_Compensation_1[18]);
	dig_P8 = (signed short) (RX_Buffer_BME280_Burst_Compensation_1[21]<<8 | RX_Buffer_BME280_Burst_Compensation_1[20]);
	dig_P9 = (signed short) (RX_Buffer_BME280_Burst_Compensation_1[23]<<8 | RX_Buffer_BME280_Burst_Compensation_1[22]);

	dig_H1 = (unsigned char) (RX_Buffer_BME280_Burst_Compensation_1[24]);
	dig_H2 = (signed short) (RX_Buffer_BME280_Burst_Compensation_2[1]<<8 | RX_Buffer_BME280_Burst_Compensation_2[0]);
	dig_H3 = (unsigned char) (RX_Buffer_BME280_Burst_Compensation_2[2]);
	dig_H4 = (signed short) ((signed char)RX_Buffer_BME280_Burst_Compensation_2[3]<<4) | (RX_Buffer_BME280_Burst_Compensation_2[4] & 0x0F);
	dig_H5 = (signed short) ((signed char)RX_Buffer_BME280_Burst_Compensation_2[5]<<4) | (RX_Buffer_BME280_Burst_Compensation_2[4]>>4);
	dig_H6 = (signed char) (RX_Buffer_BME280_Burst_Compensation_2[6]);
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
uint32_t BME280_compensate_T_int32(uint32_t adc_T)
{
	uint32_t var1, var2, T;
	var1  = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
	var2  = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1)))>> 12) *((int32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T  = (t_fine * 5 + 128) >> 8;
	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa

uint32_t BME280_compensate_P_int64(int32_t adc_P)
{
	int64_t var1, var2, p;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
	var2 = var2 + (((int64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
	return (uint32_t)p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
uint32_t BME280_compensate_H_int32(int32_t adc_H)
{
	int32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((int32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r *((int32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (uint32_t)(v_x1_u32r>>12);
}

void BME280_print_compensated_data(void)
{
	char buf[64];

	sprintf(buf, "--- COMPENSATED DATA FROM BME280 --- \r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);

	temp_int = temperature_compensated / 100;
	temp_dec = temperature_compensated % 100;
	sprintf(buf, "Temperature : %d.%d °C\r\n", temp_int,temp_dec);
	HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);

	press_int = pressure_compensated/25600;
	press_dec = ((pressure_compensated % 25600)*100) / 25600;
	sprintf(buf, "Pressure : %d.%d hPa\r\n", press_int,press_dec);
	HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);

	humid_int = humidity_compensated/1024;
	humid_dec = ((humidity_compensated % 1024)*100) / 1024;
	sprintf(buf, "Humidity : %d.%d %RH\r\n", humid_int,humid_dec);
	HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
}

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
#ifdef USE_FULL_ASSERT
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
