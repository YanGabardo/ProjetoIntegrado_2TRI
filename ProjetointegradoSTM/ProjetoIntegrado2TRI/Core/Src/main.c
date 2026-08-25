/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define PROTO_START     0xAA
#define PROTO_END       0x55
#define PROTO_CMD_ADC   0x01
#define PROTO_LEN_ADC   0x02

/* LEDs */
#define LED1_PIN        GPIO_PIN_3
#define LED2_PIN        GPIO_PIN_4
#define LED3_PIN        GPIO_PIN_5
#define LED4_PIN        GPIO_PIN_6

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

/* USER CODE BEGIN PV */
uint32_t lastSendTime = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
 * Faz uma conversão ADC única:
 * inicia -> aguarda -> lê -> para
 *
 * PA0 = ADC_CHANNEL_0
 *
 * Retorna um valor entre 0 e 4095.
 */
uint16_t Read_ADC_Value(void)
{
    uint16_t adcValue = 0;

    HAL_ADC_Start(&hadc1);

    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
    {
        adcValue = HAL_ADC_GetValue(&hadc1);
    }

    HAL_ADC_Stop(&hadc1);

    return adcValue;
}


/*
 * Controla os LEDs conforme o valor do ADC.
 *
 * 0 - 1023       -> nenhum LED
 * 1024 - 2047    -> PB3
 * 2048 - 3071    -> PB3 + PB4
 * 3072 - 3583    -> PB3 + PB4 + PB5
 * 3584 - 4095    -> PB3 + PB4 + PB5 + PB6
 */
void Atualizar_LEDs(uint16_t valor)
{
    /* Desliga todos primeiro */
    HAL_GPIO_WritePin(GPIOB, LED1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, LED2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, LED3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, LED4_PIN, GPIO_PIN_RESET);

    if (valor >= 1024)
    {
        HAL_GPIO_WritePin(GPIOB, LED1_PIN, GPIO_PIN_SET);
    }

    if (valor >= 2048)
    {
        HAL_GPIO_WritePin(GPIOB, LED2_PIN, GPIO_PIN_SET);
    }

    if (valor >= 3072)
    {
        HAL_GPIO_WritePin(GPIOB, LED3_PIN, GPIO_PIN_SET);
    }

    if (valor >= 3584)
    {
        HAL_GPIO_WritePin(GPIOB, LED4_PIN, GPIO_PIN_SET);
    }
}


/*
 * Monta o pacote BINÁRIO proprietário:
 *
 * [START][CMD][LEN][DATA_HIGH][DATA_LOW][CHK][END]
 *
 * Exemplo para ADC = 2048:
 *
 * AA 01 02 08 00 0B 55
 *
 * O checksum continua sendo exatamente:
 *
 * CMD ^ LEN ^ DATA_HIGH ^ DATA_LOW
 */
void Send_ADC_Value_USB(uint16_t value)
{
    uint8_t packet[7];

    uint8_t data_high =
        (uint8_t)((value >> 8) & 0xFF);

    uint8_t data_low =
        (uint8_t)(value & 0xFF);

    uint8_t checksum;

    packet[0] = PROTO_START;
    packet[1] = PROTO_CMD_ADC;
    packet[2] = PROTO_LEN_ADC;
    packet[3] = data_high;
    packet[4] = data_low;

    /* Mesmo checksum do protocolo original */
    checksum =
        packet[1] ^
        packet[2] ^
        packet[3] ^
        packet[4];

    packet[5] = checksum;
    packet[6] = PROTO_END;

    /*
     * Envia exatamente os 7 bytes
     * do protocolo proprietário.
     */
    CDC_Transmit_FS(packet, sizeof(packet));
}

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

    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USB_DEVICE_Init();
    MX_ADC1_Init();

    /* USER CODE BEGIN 2 */

    /* Garante que todos os LEDs começam desligados */
    HAL_GPIO_WritePin(
        GPIOB,
        LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN,
        GPIO_PIN_RESET
    );

    /* USER CODE END 2 */


    /* Infinite loop */
    while (1)
    {
        if (HAL_GetTick() - lastSendTime >= 500)
        {
            lastSendTime = HAL_GetTick();

            /*
             * 1. Lê o trimpot
             */
            uint16_t valor = Read_ADC_Value();

            /*
             * 2. Atualiza os LEDs
             */
            Atualizar_LEDs(valor);

            /*
             * 3. Envia o ADC pelo protocolo proprietário
             */
            Send_ADC_Value_USB(valor);
        }
    }
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource =
        RCC_PLLSOURCE_HSI_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_PLLCLK;

    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;

    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV2;

    RCC_ClkInitStruct.APB2CLKDivider =
        RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection =
        RCC_PERIPHCLK_ADC |
        RCC_PERIPHCLK_USB;

    PeriphClkInit.AdcClockSelection =
        RCC_ADCPCLK2_DIV4;

    PeriphClkInit.UsbClockSelection =
        RCC_USBCLKSOURCE_PLL;

    if (HAL_RCCEx_PeriphCLKConfig(
            &PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}


/**
  * @brief ADC1 Initialization Function
  * @retval None
  */
static void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance = ADC1;

    hadc1.Init.ScanConvMode =
        ADC_SCAN_DISABLE;

    hadc1.Init.ContinuousConvMode =
        DISABLE;

    hadc1.Init.DiscontinuousConvMode =
        DISABLE;

    hadc1.Init.ExternalTrigConv =
        ADC_SOFTWARE_START;

    hadc1.Init.DataAlign =
        ADC_DATAALIGN_RIGHT;

    hadc1.Init.NbrOfConversion = 1;

    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    /*
     * PA0 = ADC_CHANNEL_0
     */
    sConfig.Channel = ADC_CHANNEL_0;

    sConfig.Rank =
        ADC_REGULAR_RANK_1;

    sConfig.SamplingTime =
        ADC_SAMPLETIME_55CYCLES_5;

    if (HAL_ADC_ConfigChannel(
            &hadc1,
            &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}


/**
  * @brief GPIO Initialization Function
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    /*
     * Libera PB3 e PB4 do JTAG,
     * mantendo o SWD para programação/debug.
     */
    __HAL_AFIO_REMAP_SWJ_NOJTAG();


    /*
     * PA0 = entrada analógica do trimpot
     */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;

    HAL_GPIO_Init(
        GPIOA,
        &GPIO_InitStruct
    );


    /*
     * PB3, PB4, PB5 e PB6 = LEDs
     */
    GPIO_InitStruct.Pin =
        LED1_PIN |
        LED2_PIN |
        LED3_PIN |
        LED4_PIN;

    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;

    GPIO_InitStruct.Pull =
        GPIO_NOPULL;

    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(
        GPIOB,
        &GPIO_InitStruct
    );


    /*
     * Todos os LEDs começam desligados.
     */
    HAL_GPIO_WritePin(
        GPIOB,
        LED1_PIN |
        LED2_PIN |
        LED3_PIN |
        LED4_PIN,
        GPIO_PIN_RESET
    );
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

    __disable_irq();

    while (1)
    {
    }

    /* USER CODE END Error_Handler_Debug */
}


#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */

    /* USER CODE END 6 */
}

#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/