/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "display.h" 		//display driver
#include "st7789.h"
#include "ili9341.h" 		//display support based on ili9341 controller
#include "pcm5102.h" 		//DAC driver PCM5102
#include "ff.h"		 		//FatFS library
#include "avi.h"	 		//AVI player
#include "filemanager.h"	//file manager
#include "keyboard.h"		//module for working with buttons

#include "stdio.h" //para sprintf
#include "stdlib.h"
#include "Anglas_INA226.h"
#include "Anglas_MAX17048.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//counter of "system time", ms
volatile uint32_t millis = 0;

//Variables for the FatFs library
char Path[4];
FATFS FatFS;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c3;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
float power, current,shunt,voltage;
char buff[50];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI2_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM10_Init(void);
static void MX_I2C3_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void DWT_init (void)
{
	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))	{
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	#ifdef __CORE_CM7_H_GENERIC
	        DWT->LAR = 0xC5ACCE55;  //unlocking access to the DWT register
	#endif
	   DWT->CYCCNT = 0;
	   DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	/* enable instruction caching */
	#if (INSTRUCTION_CACHE_ENABLE != 0U)
	((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR |= (0x1UL << (9U));
	#endif
	/* enable data caching */
	#if (DATA_CACHE_ENABLE != 0U)
	((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR |= (0x1UL << (10U));
	#endif
	/* enable the instruction prefetching system */
	#if (PREFETCH_ENABLE != 0U)
	((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR |= (0x1UL << (8U));
	#endif
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  //set up the system timer (interrupts 1000 times per second)
	SysTick_Config(SystemCoreClock/1000);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_TIM10_Init();
  MX_I2C3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  MAX17048_Init();
  MAX17048_QuickStart();
  MAX17048_SetAlertThreshold(1);
  INA226_Init(3.2768,100,AVG_4,T_Vbus_8_244ms,T_Vshunt_8_244ms,MODE_SHUNT_BUS_CONTINUOUS);//CAMBIAR SHUNT 25
  INA226_Mode_pinAlert(SHUNT_VOLTAGE_OVER);
  INA226_Alert_Limit(1500);

  /* ---------------------------- PULSADORES ------------------------------- */
  //KEYB_Add_Button(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin, KEYB_LEFT, KEYB_BUTTON_ACTIVE);
  KEYB_Add_Button(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin, KEYB_RIGHT, KEYB_BUTTON_ACTIVE);
  KEYB_Add_Button(KEY_UP_GPIO_Port, KEY_UP_Pin, KEYB_UP, KEYB_BUTTON_ACTIVE);
  KEYB_Add_Button(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin, KEYB_DOWN, KEYB_BUTTON_ACTIVE);

  TIM10->DIER |= TIM_DIER_UIE;
  TIM10->CR1  |= TIM_CR1_CEN;
  /* ------------------------------------------------------------------------------- */

  //Initialize I2S3
    Init_DAC();
    //Setup_DAC(SPI3, 44100, LL_I2S_DATAFORMAT_16B);



	DWT_init();

	//-------------------Setting parameters and initializing the display ---------------------
	LCD_BackLight_data bl_dat = { .htim_bk = TIM3,
		  	  	  	  	  	  	  .channel_htim_bk = LL_TIM_CHANNEL_CH1,
    							  .blk_port = 0,
    							  .blk_pin = 0,
    							  .bk_percent = 100 };

	LCD_DMA_TypeDef hdma_spi1_tx = { .dma = DMA2,
		  	  	  	  	  	  	     .stream = LL_DMA_STREAM_3 };

	LCD_SPI_Connected_data spi_dat = { .spi = SPI1,
  		  							   .dma_tx = hdma_spi1_tx,
   									   .reset_port = LCD_RES_GPIO_Port,
   									   .reset_pin = LCD_RES_Pin,
   									   .dc_port = LCD_DC_GPIO_Port,
   									   .dc_pin = LCD_DC_Pin,
   									   .cs_port = LCD_CS_GPIO_Port,
   									   .cs_pin = LCD_CS_Pin  };
	#ifndef LCD_DYNAMIC_MEM
	LCD_Handler lcd1;
	#endif

	//For display on controller ST7789 170x320
	//ffmpeg -i che.mp4 -c:a mp3 -c:v mjpeg -s 320x170 -r 23.98 -q 10 che.avi
	LCD = LCD_DisplayAdd( LCD,
	#ifndef LCD_DYNAMIC_MEM
			  	  	  	  	&lcd1,
	#endif
			  	  	  	  	170,//170
			   				320,//320
							ST7789_CONTROLLER_WIDTH,
							ST7789_CONTROLLER_HEIGHT,
							//Задаем смещение по ширине и высоте для нестандартных или бракованных дисплеев:
							35,//-35
							0,//0
							PAGE_ORIENTATION_LANDSCAPE_MIRROR,// PAGE_ORIENTATION_LANDSCAPE
							ST7789_Init,
							ST7789_SetWindow,
							ST7789_SleepIn,
							ST7789_SleepOut,
							&spi_dat,
							LCD_DATA_16BIT_BUS,
							bl_dat);

	/*
	//For display on controller ST7789 135x240
	//ffmpeg -i che.mp4 -c:a mp3 -c:v mjpeg -s 240x135 -r 23.98 -q 10 che.avi
	LCD = LCD_DisplayAdd( LCD,
	#ifndef LCD_DYNAMIC_MEM
			  	  	  	  	&lcd1,
	#endif
			  	  	  	  	135,//170
			   				240,//320
							ST7789_CONTROLLER_WIDTH,
							ST7789_CONTROLLER_HEIGHT,
							//Задаем смещение по ширине и высоте для нестандартных или бракованных дисплеев:
							-53,//-35
							40,//0
							PAGE_ORIENTATION_LANDSCAPE,// PAGE_ORIENTATION_LANDSCAPE
							ST7789_Init,
							ST7789_SetWindow,
							ST7789_SleepIn,
							ST7789_SleepOut,
							&spi_dat,
							LCD_DATA_16BIT_BUS,
							bl_dat);
	*/

	/*
	//For display on controller ILI9341
	LCD = LCD_DisplayAdd( LCD,
	#ifndef LCD_DYNAMIC_MEM
		  	  	  	  	  &lcd1,
	#endif
		  	  	  	  	  320,
		   				  240,
						  ILI9341_CONTROLLER_WIDTH,
						  ILI9341_CONTROLLER_HEIGHT,
						  //Set the offset in width and height for non-standard or defective displays:
						  0,		//display matrix width offset
						  0,		//display matrix height offset
						  //PAGE_ORIENTATION_PORTRAIT,
						  //PAGE_ORIENTATION_PORTRAIT_MIRROR,
						  PAGE_ORIENTATION_LANDSCAPE_MIRROR,
						  ILI9341_Init,
						  ILI9341_SetWindow,
						  ILI9341_SleepIn,
						  ILI9341_SleepOut,
						  &spi_dat,
						  LCD_DATA_16BIT_BUS,
						  bl_dat );
	*/


	LCD_Handler *lcd = LCD; //pointer to the first display in the list
	LCD_Init(lcd);
	LCD_Fill(lcd, 0x319bb1);
	//------------------------------------------------------------------------------------------

	//----------------------------------- Mounting the disc -----------------------------------
	(void)f_mount(&FatFS, Path, 0);
	//------------------------------------------------------------------------------------------

	/* ----------------------------Setting up a file manager ---------------------------- */
	/* Define a color scheme */
	File_Manager_Color_Scheme color_scheme_pl = { COLOR_YELLOW,		/* directory name text color */
			  	  	  	  	  	  	  	  	  	  COLOR_WHITE,		/* file name text color */
												  COLOR_BLACK,		/* text color of the selected file/directory */
												  0x319bb1,			/* manager window background color (for empty lines) */
												  0x319bb1,			/* background color of even rows */
												  0x319bb1,			/* background color of odd rows */
												  COLOR_LIGHTGREY,	/* cursor color of the current file/directory */
												  COLOR_LIGHTGREY,	/* vertical scroll slider color */
												  0x319bb1,			/* vertical scroll bar color */
												  1					/* cursor color flag:
																  	  	  0 - the text of the selected file/directory is circled
																  	  	  	  cursor color rectangle (cursor_color).
														 	 	  	  	  >0 -the text background of the selected file/directory is equal to
														 	 	  	  	  	  cursor color (cursor_color) */
												 	 };

	File_Manager_Handler *fm = FileManagerNew();	/* Creating a File Manager Handler */
	fm->SetDisplay(fm, lcd);						/* Display showing the file manager (pointer) */
	fm->SetWin(fm, 6, 0, lcd->Width-6, lcd->Height);/* File manager window options:
	  	  	  	  	  	  	  	  	  	  	  	  	   - horizontal position of the upper left corner of the window;
	  	  	  	  	  	  	  	  	  	  	  	  	   - vertical position of the upper left corner of the window;
	  	  	  	  	  	  	  	  	  	  	  	  	   - window width;
	  	  	  	  	  	  	  	  	  	  	  	  	   - window height. */
	fm->SetColor(fm, &color_scheme_pl);				 /* Color scheme (index) */
	fm->SetFont(fm, &Font_12x20);					 /* Font (pointer) */
	fm->SetKeys(fm, KEYB_UP, KEYB_DOWN, KEYB_RIGHT); /* Control buttons:
	   	   	   	   	   	   	   	   	   	   	   	   	    - button bit number up;
	   	   	   	   	   	   	   	   	   	   	   	   	    - button bit number down;
	   	   	   	   	   	   	   	   	   	   	   	   	    - bit number of the enter/select button. */
	//-----------------------------------------------------------------------------------------

	//Splash screen with information about the program and author
	//AVIIntro(lcd);
	LL_mDelay(30);
	uint8_t f_popup = 1;

	//Medicionoes de INA226
	voltage = INA226_Vbus();
	sprintf(buff,"V:%2.1fV ",voltage);
	LCD_WriteString(lcd, 0, 0, buff, &Font_15x25, COLOR_YELLOW, COLOR_BLUE, LCD_SYMBOL_PRINT_FAST);

	current = INA226_Current();
	sprintf(buff,"I:%4.0fmA",current);
	LCD_WriteString(lcd, 0, 25, buff, &Font_15x25, COLOR_GREEN, COLOR_RED, LCD_SYMBOL_PRINT_FAST);

	LL_GPIO_SetOutputPin(GPIOB, ON_OFF_3V7_1_Pin);//ENCIENDO
	LL_mDelay(100);


	//LL_mDelay(1000);
	//uint32_t keys=0;
	//keys = KEYB_Inkeys();
	//while (keys & (1 << KEYB_RIGHT));

	while(!KEYB_kbhit());
	LL_GPIO_SetOutputPin(GPIOB, IN_TS3A5018_Pin);//CAMBIO A AUDIO PCM

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    	/* Fill the display with blue (the background color of the file manager) */
    	LCD_Fill(lcd, 0x319bb1);
    	/* "Pop-up" window of the file manager */
		if (f_popup) {
			FileManagerPopupWin(fm, 5);
			f_popup = 0;
		}
		/* Launch file manager */
		fm->Show(fm);
		/* Check the shutdown status of the file manager */
		if (fm->GetStatus(fm) == File_Manager_OK) { /* Completing without errors with file selection? */
			/* Launch the video player */
			PlayAVI(fm->GetFilePath(fm), fm->GetFileName(fm), lcd, 0, 0, lcd->Width, lcd->Height);
		}
		else {
			/* Display a message that the file manager completed with an error */
			LCD_WriteString(lcd, 20, 0, "Error FileManager!", fm->font, fm->color_scheme->text_color_file, fm->color_scheme->bg_color, LCD_SYMBOL_PRINT_FAST);
			/* Wait for any button to be pressed */
			while(!KEYB_kbhit()) { }
		}

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
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_HSE_EnableCSS();
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 168, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(84000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

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

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA7   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LCD_SCL_Pin|LCD_SDA_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* SPI1 DMA Init */

  /* SPI1_TX Init */
  LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_3, LL_DMA_CHANNEL_3);

  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_STREAM_3, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetStreamPriorityLevel(DMA2, LL_DMA_STREAM_3, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA2, LL_DMA_STREAM_3, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_STREAM_3, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_STREAM_3, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA2, LL_DMA_STREAM_3, LL_DMA_PDATAALIGN_HALFWORD);

  LL_DMA_SetMemorySize(DMA2, LL_DMA_STREAM_3, LL_DMA_MDATAALIGN_HALFWORD);

  LL_DMA_EnableFifoMode(DMA2, LL_DMA_STREAM_3);

  LL_DMA_SetFIFOThreshold(DMA2, LL_DMA_STREAM_3, LL_DMA_FIFOTHRESHOLD_FULL);

  LL_DMA_SetMemoryBurstxfer(DMA2, LL_DMA_STREAM_3, LL_DMA_MBURST_INC8);

  LL_DMA_SetPeriphBurstxfer(DMA2, LL_DMA_STREAM_3, LL_DMA_PBURST_SINGLE);

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_16BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
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

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**SPI2 GPIO Configuration
  PB13   ------> SPI2_SCK
  PB14   ------> SPI2_MISO
  PB15   ------> SPI2_MOSI
  */
  GPIO_InitStruct.Pin = SD_CLK_Pin|SD_MISO_Pin|SD_MOSI_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* SPI2 DMA Init */

  /* SPI2_RX Init */
  LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_3, LL_DMA_CHANNEL_0);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_3, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_3, LL_DMA_PRIORITY_MEDIUM);

  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_3, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_3, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_3, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_3, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_3, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_EnableFifoMode(DMA1, LL_DMA_STREAM_3);

  LL_DMA_SetFIFOThreshold(DMA1, LL_DMA_STREAM_3, LL_DMA_FIFOTHRESHOLD_FULL);

  LL_DMA_SetMemoryBurstxfer(DMA1, LL_DMA_STREAM_3, LL_DMA_MBURST_INC8);

  LL_DMA_SetPeriphBurstxfer(DMA1, LL_DMA_STREAM_3, LL_DMA_PBURST_SINGLE);

  /* SPI2_TX Init */
  LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_4, LL_DMA_CHANNEL_0);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_4, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_4, LL_DMA_PRIORITY_MEDIUM);

  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_4, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_4, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_4, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_4, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_4, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_EnableFifoMode(DMA1, LL_DMA_STREAM_4);

  LL_DMA_SetFIFOThreshold(DMA1, LL_DMA_STREAM_4, LL_DMA_FIFOTHRESHOLD_FULL);

  LL_DMA_SetMemoryBurstxfer(DMA1, LL_DMA_STREAM_4, LL_DMA_MBURST_INC8);

  LL_DMA_SetPeriphBurstxfer(DMA1, LL_DMA_STREAM_4, LL_DMA_PBURST_SINGLE);

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV128;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI2, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  TIM_InitStruct.Prescaler = 999;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 209;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM3, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM3);
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH1);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 105;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH1);
  LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM3);
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**TIM3 GPIO Configuration
  PA6   ------> TIM3_CH1
  */
  GPIO_InitStruct.Pin = LCD_BLK_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
  LL_GPIO_Init(LCD_BLK_GPIO_Port, &GPIO_InitStruct);

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM10);

  /* TIM10 interrupt Init */
  NVIC_SetPriority(TIM1_UP_TIM10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  TIM_InitStruct.Prescaler = 999;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 420-LL_TIM_IC_FILTER_FDIV1_N2;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM10, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM10);
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

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
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* Init with LL driver */
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),2, 0));
  NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),2, 0));
  NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  NVIC_SetPriority(DMA2_Stream3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),3, 0));
  NVIC_EnableIRQ(DMA2_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(LIGHTER_GPIO_Port, LIGHTER_Pin);

  /**/
  LL_GPIO_ResetOutputPin(GPIOB, ON_OFF_3V7_1_Pin|A4988_SLEEP_2_Pin|IN_TS3A5018_Pin|EN_TS3A5018_Pin);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, A4988_STEP_2_Pin|A4988_DIR_2_Pin);

  /**/
  LL_GPIO_SetOutputPin(GPIOA, A4988_EN_2_Pin|LCD_DC_Pin|LCD_RES_Pin|LCD_CS_Pin);

  /**/
  LL_GPIO_SetOutputPin(SD_CS_GPIO_Port, SD_CS_Pin);

  /**/
  GPIO_InitStruct.Pin = LIGHTER_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LIGHTER_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = A4988_EN_2_Pin|A4988_STEP_2_Pin|A4988_DIR_2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LCD_DC_Pin|LCD_RES_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LCD_CS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = ON_OFF_3V7_1_Pin|A4988_SLEEP_2_Pin|IN_TS3A5018_Pin|EN_TS3A5018_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = IN_DRAIN_LATCH1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(IN_DRAIN_LATCH1_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = SD_CS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(SD_CS_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = KEY_RIGHT_Pin|KEY_DOWN_Pin|KEY_UP_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
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
