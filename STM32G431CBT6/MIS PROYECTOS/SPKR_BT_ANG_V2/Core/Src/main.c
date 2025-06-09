/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "app_fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "Anglas_ST7789.h"
#include "fonts.h"
#include "math.h"
#include "bitmap.h"

#include "Anglas_MAX17048.h"
#include "Anglas_INA226.h"

#include "testimg.h"
#include "fatfs_sd.h"
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

I2S_HandleTypeDef hi2s3;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint16_t contInt=0;
float contFloat=0.0;
char buff[64];
uint16_t freq_BLK = 5000;//5khz

float soc, voltage_pwrbank, current_pwrbank, voltage_bat;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM16_Init(void);
static void MX_SPI2_Init(void);
static void MX_I2S3_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void PWM_set_Freq_DutyCycle(uint16_t freq, uint8_t duty);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
FATFS fs;  // file system
FIL file; // File
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;
char buffer[100];
uint16_t farbe;

#define SPIbuffSize 264
#define BUFFER_SIZE 1032
#define BITMAP_HEADER_SIZE sizeof(BmpHeader)
#define MIN(a, b) (((a)<(b))?(a): (b))


int displayImage(const char* fname) {
    FIL file;
    FRESULT res = f_open(&file, fname, FA_READ);
    if(res != FR_OK) {
    	ST7789_print(0, 0, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, "f_open() failed");
        return -1;
    }

    //ST7789_print(0, 20, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, "File opened, reading...");

    unsigned int bytesRead;
    uint8_t header[34];
    res = f_read(&file, header, sizeof(header), &bytesRead);
    if(res != FR_OK) {
    	 ST7789_print(0, 40, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, "f_read() failed");
        f_close(&file);
        return -2;
    }

    if((header[0] != 0x42) || (header[1] != 0x4D)) {
    	ST7789_print(0, 60, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, "Wrong BMP signature: 0x%02X 0x%02X");
        f_close(&file);
        return -3;
    }

    uint32_t imageOffset = header[10] | (header[11] << 8) | (header[12] << 16) | (header[13] << 24);
    uint32_t imageWidth = header[18] | (header[19] << 8) | (header[20] << 16) | (header[21] << 24);
    uint32_t imageHeight = header[22] | (header[23] << 8) | (header[24] << 16) | (header[25] << 24);
    uint16_t imagePlanes = header[26] | (header[27] << 8);
    uint16_t imageBitsPerPixel = header[28] | (header[29] << 8);
    uint32_t imageCompression = header[30] | (header[31] << 8) | (header[32] << 16) | (header[33] << 24);

    //snprintf(buff, sizeof(buff), "Pixels offset: %lu\r\n ", imageOffset);
    //ST7789_print(0, 50, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, buff);
    //snprintf(buff, sizeof(buff), "WxH: %lux%lu\r\n ", imageWidth, imageHeight);
    //ST7789_print(0, 70, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, buff);
    //snprintf(buff, sizeof(buff), "Planes: %d\r\n ", imagePlanes);
    //ST7789_print(0, 90, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, buff);
    //snprintf(buff, sizeof(buff), "Bits per pixel: %d\r\n ", imageBitsPerPixel);
    //ST7789_print(0, 110, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, buff);
    //snprintf(buff, sizeof(buff), "Compression: %lu\r\n ", imageCompression);
    //ST7789_print(0, 130, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, buff);

    if((imageWidth != ST7789_HEIGHT) || (imageHeight != ST7789_WIDTH)) {
    	//UART_Printf("Wrong BMP size, %dx%d expected\r\n", ST7735_WIDTH, ST7735_HEIGHT);
    	ST7789_print(0, 60, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, "Wrong BMP size, %dx%d expected");
        f_close(&file);
        return -4;
    }

    if((imagePlanes != 1) || (imageBitsPerPixel != 24) || (imageCompression != 0)) {
    	ST7789_print(0, 60, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, "Wrong BMP size, %dx%d expected");
        f_close(&file);
        return -5;
    }

    res = f_lseek(&file, imageOffset);
    if(res != FR_OK) {
        snprintf(buff, sizeof(buff), "f_lseek() failed, res = %d\r\n ", res);
        //ST7789_print(0, 60, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, buff);
        f_close(&file);
        return -6;
    }

    // row size is aligned to 4 bytes
    uint8_t imageRow[(ST7789_HEIGHT * 3 + 3) & ~3];
    for(uint32_t y = 0; y < imageHeight; y++) {
        uint32_t rowIdx = 0;
        res = f_read(&file, imageRow, sizeof(imageRow), &bytesRead);
        if(res != FR_OK) {
            snprintf(buff, sizeof(buff), "f_read() failed, res = %d\r\n", res);
            //ST7789_print(0, 60, RGB565(0, 130, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, buff);
            f_close(&file);
            return -7;
        }

        for(uint32_t x = 0; x < imageWidth; x++) {
            uint8_t b = imageRow[rowIdx++];
            uint8_t g = imageRow[rowIdx++];
            uint8_t r = imageRow[rowIdx++];
            ST7789_DrawPixel(x, imageHeight - y - 1, RGB565(r, g, b));
        }
    }

    res = f_close(&file);
    if(res != FR_OK) {
    	//UART_Printf("f_close() failed, res = %d\r\n", res);
        snprintf(buff, sizeof(buff), "f_close() failed, res = %d\r\n", res);
        //ST7789_print(0, 60, RGB565(0, 130, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, buff);
        return -8;
    }

    return 0;
}

void displayRandomImage(uint32_t seed) {
    char filename[10];  // Buffer para el nombre del archivo
    srand(seed);  // Inicializa rand() con una semilla mejorada
    int imageNumber = (rand() % 30) + 1; // Genera número aleatorio entre 1 y 30
    snprintf(filename, sizeof(filename), "%d.bmp", imageNumber); // Genera el nombre del archivo
    displayImage(filename); // Muestra la imagen en el ST7789
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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM16_Init();
  MX_SPI2_Init();
  MX_I2S3_Init();
  MX_USART1_UART_Init();
  if (MX_FATFS_Init() != APP_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN 2 */

  /*
  PWM_set_Freq_DutyCycle(freq_BLK,0);//iniciar pantalla negra
  ST7789_Init();
  ST7789_rotation(4);
  PWM_set_Freq_DutyCycle(freq_BLK,100);
  ST7789_FillScreen(ST7789_BLACK);

  MAX17048_Init();
  //MAX17048_QuickStart();
  MAX17048_SetAlertThreshold(1);

  INA226_Init(3.2768,25,AVG_4,T_Vbus_8_244ms,T_Vshunt_8_244ms,MODE_SHUNT_BUS_CONTINUOUS);
  INA226_Mode_pinAlert(SHUNT_VOLTAGE_OVER);//superar la CORRIENTE de
  INA226_Alert_Limit(900);//escribo el umbral para activar el pin alert

  //Verificar micro SD
  FRESULT res = f_mount(&fs, "", 0);
  if(res != FR_OK) {
      //UART_Printf("f_mount() failed, res = %d\r\n", res);
      snprintf(buff, sizeof(buff), "f_mount() failed, res = %d\r\n", res);
      ST7789_print(0, 150, RGB565(0, 130, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, buff);
      return -2;
  }
  //ST7789_print(0, 150, RGB565(0, 130, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, "f_mount() done!\r\n");
  */


  /******************** PARA USAR LOS TRES SYS_WKUP **********************************/

  // Verifica si el sistema reanudó desde Standby Mode
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB); // Limpia la bandera

      /************************************* SYS_WKUP1 ***********************************************/
      //Esto hace cuando se presiona el pulsador del SYS_WKUP1 (necesita si o si resistencia PULL-DOWN)
      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 1) {//FUNCION 1, si presionamos SYS_WKUP1

    	  if (HAL_GPIO_ReadPin(IN_DRAIN_LATCH_GPIO_Port, IN_DRAIN_LATCH_Pin) == 0){//si no hay 3.7V en esta entrada IN_DRAIN_LATCH, entonces enciende el parlante
    		  //Inicializa la micro SD
    		  FRESULT res = f_mount(&fs, "", 0);
        	  if(res != FR_OK) {
        	      snprintf(buff, sizeof(buff), "f_mount() failed, res = %d\r\n", res);
        	      ST7789_print(0, 150, RGB565(0, 130, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, buff);
        	      return -2;
        	  }

    		  PWM_set_Freq_DutyCycle(freq_BLK,0);//pantalla apagada
    		  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, 1);//Encender parlante
    		  PWM_set_Freq_DutyCycle(freq_BLK,0);//iniciar pantalla negra
			  ST7789_Init();
			  ST7789_rotation(4);
			  PWM_set_Freq_DutyCycle(freq_BLK,100);
			  MAX17048_Init();
			  MAX17048_QuickStart();
			  MAX17048_SetAlertThreshold(1);
			  INA226_Init(3.2768,25,AVG_4,T_Vbus_8_244ms,T_Vshunt_8_244ms,MODE_SHUNT_BUS_CONTINUOUS);
			  INA226_Mode_pinAlert(SHUNT_VOLTAGE_OVER);
			  INA226_Alert_Limit(1500);

			  voltage_bat = MAX17048_GetVoltage();
			  voltage_pwrbank = INA226_Vbus();

			  displayRandomImage(voltage_bat*voltage_bat*voltage_pwrbank*voltage_pwrbank);
			  HAL_Delay(4000);

    	  }else{//si hay 3.7V en la entrada IN_DRAIN_LATCH, entonces esta encendido el parlante, y con las lineas de abajo lo apago
    		  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, 0);//Apagar parlante
			  MAX17048_SetAlertThreshold(1);//al colocar pone a 0 el pin alert y esto hace que consuma unos mA menos
			  HAL_Delay(2000);//tiempo para apagar luego de encender o viceversa
    	  }

      }

      /************************************* SYS_WKUP2 ***********************************************/
      //Esto hace cuando se presiona el pulsador del SYS_WKUP2 (necesita si o si resistencia PULL-DOWN)
      if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == 1) {//FUNCION 2, si presionamos SYS_WKUP2

    	  //Inicia SD CARD
    	  FRESULT res = f_mount(&fs, "", 0);
    	  if(res != FR_OK) {
    	      snprintf(buff, sizeof(buff), "f_mount() failed, res = %d\r\n", res);
    	      ST7789_print(0, 150, RGB565(0, 130, 255) , RGB565(255, 255, 255) , 1, &Font_11x18, 1, buff);
    	      return -2;
    	  }

    	  PWM_set_Freq_DutyCycle(freq_BLK,0);//iniciar pantalla negra

    	  //verifica si esta encendido lo mantiene asi y si esta apagado lo mantiene apagado
    	  if (HAL_GPIO_ReadPin(IN_DRAIN_LATCH_GPIO_Port, IN_DRAIN_LATCH_Pin) == 0){
    		  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, 0);//mantiene apagado
    	  }else{
    		  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, 1);//mantiene encendido
    	  }

    	  PWM_set_Freq_DutyCycle(freq_BLK,0);//iniciar pantalla negra
    	  ST7789_Init();
    	  ST7789_rotation(4);
    	  PWM_set_Freq_DutyCycle(freq_BLK,100);
    	  ST7789_FillScreen(ST7789_BLACK);
    	  MAX17048_Init();
    	  MAX17048_QuickStart();
    	  MAX17048_SetAlertThreshold(1);
		  INA226_Init(3.2768,25,AVG_4,T_Vbus_8_244ms,T_Vshunt_8_244ms,MODE_SHUNT_BUS_CONTINUOUS);
		  INA226_Mode_pinAlert(SHUNT_VOLTAGE_OVER);
		  INA226_Alert_Limit(1500);


		  soc = MAX17048_GetSoc();//leer una vez, aqui lee 2% o algo asi porq anteriormente hubo consumo de mA ya sea de powerbank o altavoces
		  HAL_Delay(500);//esperar para que el voltaje no de 2% porq se activo anteriormente el powerbank

		  soc = MAX17048_GetSoc();//aqui se estabilizar y lee bien luego de ese delay anterior
		  sprintf(buff,"%3i%%",(uint8_t)soc);
		  ST7789_print(100, 0, RGB565(0, 255, 0) , RGB565(0, 0, 0) , 1, &Font_16x28, 2, buff);

		  voltage_bat = MAX17048_GetVoltage();
		  //HAL_Delay(400);//esperar para que el voltaje no de 2% porq se activo anteriormente el powerbank

		  voltage_bat = MAX17048_GetVoltage();
		  sprintf(buff,"VOLTAGE: %1.2fV",voltage_bat);
		  ST7789_print(5, 60, RGB565(0, 0, 255) , RGB565(0, 0, 0) , 1, &Font_11x18, 2, buff);


		  voltage_pwrbank = INA226_Vbus();
		  sprintf(buff,"%2.1fV",voltage_pwrbank);
		  ST7789_print(50, 100, RGB565(255, 255, 255) , RGB565(0, 0, 0) , 1, &Font_11x18, 2, buff);

		  current_pwrbank = INA226_Current();
		  sprintf(buff,"%4.0fmA",current_pwrbank);
		  ST7789_print(140, 100, RGB565(255, 0, 255) , RGB565(0, 0, 0) , 1, &Font_11x18, 2, buff);
		  HAL_Delay(2000);

		  displayRandomImage(voltage_bat*voltage_bat*voltage_pwrbank*voltage_pwrbank);
    	  HAL_Delay(4000);

    	  PWM_set_Freq_DutyCycle(freq_BLK,0);//pantalla apagada
    	  HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
    	  ST7789_SleepModeEnter();
      }


      /*
      //Esto hace cuando se presiona el pulsador del SYS_WKUP4
      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == 1) {//FUNCION 3
    	  //verifica si esta encendido lo mantiene asi y si esta apagado lo mantiene apagado
    	  if (HAL_GPIO_ReadPin(IN_DRAIN_LATCH_GPIO_Port, IN_DRAIN_LATCH_Pin) == 0){
    		  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, 0);
    	  }else{
    		  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, 1);
    	  }

    	  PWM_set_Freq_DutyCycle(freq_BLK,0);//iniciar pantalla negra
		  ST7789_Init();
		  ST7789_rotation(4);
		  PWM_set_Freq_DutyCycle(freq_BLK,100);
		  ST7789_FillScreen(ST7789_BLACK);
		  MAX17048_Init();
		  MAX17048_SetAlertThreshold(1);
		  INA226_Init(3.2768,25,AVG_4,T_Vbus_8_244ms,T_Vshunt_8_244ms,MODE_SHUNT_BUS_CONTINUOUS);
		  INA226_Mode_pinAlert(SHUNT_VOLTAGE_OVER);
		  INA226_Alert_Limit(1500);

		  voltage_pwrbank = INA226_Vbus();
		  sprintf(buff,"%2.1fV",voltage_pwrbank);
		  ST7789_print(70, 20, RGB565(0, 255, 0) , RGB565(0, 0, 0) , 1, &Font_11x18, 4, buff);

		  current_pwrbank = INA226_Current();
		  sprintf(buff,"%4.0fmA",current_pwrbank);
		  ST7789_print(20, 115, RGB565(255, 0, 255) , RGB565(0, 0, 0) , 1, &Font_11x18, 3, buff);

		  HAL_Delay(2000);
		  PWM_set_Freq_DutyCycle(freq_BLK,0);//pantalla apagada
		  HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
		  ST7789_SleepModeEnter();
	  }
	  */

      HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);// Desactiva el Wake-Up Pin1
      HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);// Desactiva el Wake-Up Pin2
      //HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN4);// Desactiva el Wake-Up Pin4

  }//end if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)

    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);//AHORA RECIEN VAMOS A INGRESAR AL STANDBY MODE
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);//habilitamos el Wake-Up Pin1
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);//habilitamos el Wake-Up Pin2
    //HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4);//habilitamos el Wake-Up Pin4
    HAL_PWR_EnterSTANDBYMode();//FINALMENTE INGRESAMOS AL STANDBY MODE

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /*voltage_bat = MAX17048_GetVoltage();
	  sprintf(buff,"VOLTAGE: %1.2fV",voltage_bat);
	  ST7789_print(40, 50, RGB565(0, 255, 0) , RGB565(0, 0, 0) , 1, &Font_11x18, 1, buff);

	  soc = MAX17048_GetSoc();
	  sprintf(buff,"%3i%%",(uint8_t)soc);
	  ST7789_print(140, 5, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_16x28, 1, buff);*/

	  /*
	  voltage = INA226_Vbus();
	  sprintf(buff,"%2.1fV",voltage);
	  ST7789_print(70, 20, RGB565(0, 0, 255) , RGB565(0, 0, 0) , 1, &Font_11x18, 4, buff);

	  current = INA226_Current();
	  sprintf(buff,"%4.0fmA",current);
	  ST7789_print(20, 115, RGB565(255, 0, 255) , RGB565(0, 0, 0) , 1, &Font_11x18, 3, buff);

	  if(HAL_GPIO_ReadPin(ALERT_INA_GPIO_Port, ALERT_INA_Pin) == 1){
		  ST7789_print(280, 115, RGB565(255, 0, 255) , RGB565(0, 0, 0) , 1, &Font_11x18, 1, "ON ");
	  }else{
		  ST7789_print(280, 115, RGB565(255, 0, 255) , RGB565(0, 0, 0) , 1, &Font_11x18, 1, "OFF");
	  }
	  */

	  //la imagen tiene que ser horizontal, en 320x170
	  //primero convertir a esa resolucion 320x170 en png o jpg
	  //Luego editar con paint  y guardar en archivo bmp 24bits (Mapa de bits de 24bits .bmp)

	  //displayImage("1.bmp");
	  //displayImage("2.bmp");
	  //displayImage("3.bmp");
	  //displayImage("4.bmp");
	  //displayImage("5.bmp");


	  /*
	  soc = MAX17048_GetSoc();
	  sprintf(buff,"%3i%%",(uint8_t)soc);
	  ST7789_print(100, 20, RGB565(0, 0, 255) , RGB565(0, 0, 0) , 0, &Font_16x28, 2, buff);

	  voltage = MAX17048_GetVoltage();
	  sprintf(buff,"VOLTAGE: %1.2fV",voltage);
	  ST7789_print(5, 100, RGB565(0, 255, 0) , RGB565(0, 0, 0) , 0, &Font_11x18, 2, buff);
	  HAL_Delay(2000);
	  displayImage("2.bmp");
	  HAL_Delay(2000);
	  */


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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV5;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
  hi2c1.Init.Timing = 0x20B0D9FF;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2S3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S3_Init(void)
{

  /* USER CODE BEGIN I2S3_Init 0 */

  /* USER CODE END I2S3_Init 0 */

  /* USER CODE BEGIN I2S3_Init 1 */

  /* USER CODE END I2S3_Init 1 */
  hi2s3.Instance = SPI3;
  hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_44K;
  hi2s3.Init.CPOL = I2S_CPOL_LOW;
  if (HAL_I2S_Init(&hi2s3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S3_Init 2 */

  /* USER CODE END I2S3_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 170-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */
  HAL_TIM_MspPostInit(&htim16);

}

/**
  * @brief USART1 Initialization Function
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
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(IN_TS3A5018_GPIO_Port, IN_TS3A5018_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DC_Pin|RST_Pin|CS_SD_Pin|ON_OFF_3V7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : SW3_Pin IN_CHARGER_Pin STDBY_Pin CHRGB_Pin */
  GPIO_InitStruct.Pin = SW3_Pin|IN_CHARGER_Pin|STDBY_Pin|CHRGB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : IN_TS3A5018_Pin */
  GPIO_InitStruct.Pin = IN_TS3A5018_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IN_TS3A5018_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DC_Pin RST_Pin CS_Pin */
  GPIO_InitStruct.Pin = DC_Pin|RST_Pin|CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : ALERT_INA_Pin */
  GPIO_InitStruct.Pin = ALERT_INA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(ALERT_INA_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LINK_Pin IN_DRAIN_LATCH_Pin */
  GPIO_InitStruct.Pin = LINK_Pin|IN_DRAIN_LATCH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_SD_Pin ON_OFF_3V7_Pin */
  GPIO_InitStruct.Pin = CS_SD_Pin|ON_OFF_3V7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void PWM_set_Freq_DutyCycle(uint16_t freq, uint8_t duty){
	/*
	 * La frecuencia a la que opera el uC es de 48000000Hz
	 *
	 * El prescaler PSC-16bits divide a esta frecuencia
	 * 48000000/48 = 1000000Hz = 1Mhz
	 *
	 * El CounterPeriod solo es un contador de 0 - 65535 y cuando lo establecemos en un valor digamos 10000
	 * entonces va contar de 0  a 10000 y luego se desbordara es decir reiniciara y empezara de cero otra vez hasta 10000
	 * Ahora el CounterPeriod sirve para cambiar el periodo y con esto la frecuencia T = 1 / freq
	 * 		__HAL_TIM_GET_AUTORELOAD(&htim16): Esta función devuelve el valor actual del registro ARR (Auto-Reload Register)
	 * 		del temporizador, que representa el Counter Period.
	 * */
	uint16_t valor_CCR;
	uint32_t CounterPeriod;//en microsegundos
	uint32_t freqOsc = HAL_RCC_GetHCLKFreq();//obtiene la frecuencia del OSC en Hz (48000000)
	uint16_t prescaler = 170;//PSC-16bits

	if(duty > 100) duty = 100;
	CounterPeriod = freqOsc/(freq*prescaler);//aqui nos da el periodo en microsegundos
	//y este periodo(CounterPeriod) necesita la funcion __HAL_TIM_SET_AUTORELOAD

    HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);//inicia la generacion de la señal PWM
    //modificamos el valor del CounterPeriod y con esto modificamos la frecuencia de salida
	__HAL_TIM_SET_AUTORELOAD(&htim16,CounterPeriod);//Si quiero cambiar la frecuencia modifico: __HAL_TIM_SET_AUTORELOAD(&htim16, 1000000/frecuencia que quiero) 1000000porque es microsegundos
	//CounterPeriod = __HAL_TIM_GET_AUTORELOAD(&htim16)
	//visto con un analizador logico en un 50% de duty me da 49.5% y cuando le sumo 2 me da 50.05%
	valor_CCR = (CounterPeriod + 2)* duty/100.0;//divido entre 100 porque quiero de 0 a 100%, cambiar el rango si se desea
    __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, valor_CCR);
    //HAL_Delay(500);
    //HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);//aqui detengo la generacion del PWM, pero para mi caso necesito que siempre ete activado
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
