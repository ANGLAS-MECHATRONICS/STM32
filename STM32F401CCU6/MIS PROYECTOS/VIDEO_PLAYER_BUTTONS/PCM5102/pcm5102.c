/*
  * Author: VadRov
  * Copyright (C) 2019, VadRov, all rights reserved.
  *
  * Audio DAC driver (Audio Stereo DAC) PCM5102
  *
  * Free distribution allowed.
  *For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

#include "pcm5102.h"

//initialization I2S3
void Init_DAC(void)
{
	LL_I2S_InitTypeDef I2S_InitStruct = {0};
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	//turn on clocking spi3/i2s3
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);
	//enable port B clocking
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	//enable port A clocking
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	//setting up the module PLLI2S
	//LL_RCC_PLLI2S_ConfigDomain_I2S(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLI2SM_DIV_25, 302, LL_RCC_PLLI2SR_DIV_2);
	//LL_RCC_PLLI2S_ConfigDomain_I2S(0, 0, 0, 8);//45.5khz
	//LL_RCC_PLLI2S_ConfigDomain_I2S(0, 0, 0, 9);//43.7khz
	LL_RCC_PLLI2S_ConfigDomain_I2S(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLI2SM_DIV_25, 302, 9);

	//turn on the modulePLLI2S
	RCC->CR |= RCC_CR_PLLI2SON;

	//waiting for module activation PLLI2S
	while (!(RCC->CR & RCC_CR_PLLI2SRDY)) { __NOP(); }

	//enable I2S clocking from the module PLLI2S
	RCC->CFGR &= ~RCC_CFGR_I2SSRC;

	/* I2S3 configuration GPIO (for mode transmit master):
	  PB3   ------> I2S3_CK (AF06)
	  PB5   ------> I2S3_SD (AF06)
	  PA15	------> I2S3_WS (AF06)

	  MAX98357A
	  LRC  -> LCK -> A15
	  BCLK -> BCK -> B3
	  DIN  -> DIN -> B5
	  GAIN -> GND
	  VCC  -> +3.3V
	*/
	//conclusions PB3 и PB5
	GPIO_InitStruct.Pin = LL_GPIO_PIN_3|LL_GPIO_PIN_5;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	//conclusion PA15
	GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//enable controller clocking DMA1
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
	//configure interrupt priority
	NVIC_SetPriority(DMA1_Stream5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
	//enable thread interrupts
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);

	//setting DMA
	LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_5, LL_DMA_CHANNEL_0);
	LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_5, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_5, LL_DMA_PRIORITY_HIGH);
	LL_DMA_SetMode(DMA1, LL_DMA_STREAM_5, LL_DMA_MODE_CIRCULAR);
	LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_5, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_5, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_5, LL_DMA_PDATAALIGN_HALFWORD);
	LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_5, LL_DMA_MDATAALIGN_HALFWORD);
	LL_DMA_EnableFifoMode(DMA1, LL_DMA_STREAM_5);								//use FIFO stack for DMA
	LL_DMA_SetFIFOThreshold(DMA1, LL_DMA_STREAM_5, LL_DMA_FIFOTHRESHOLD_FULL);	//FIFO stack full level - full
	LL_DMA_SetMemoryBurstxfer(DMA1, LL_DMA_STREAM_5, LL_DMA_MBURST_INC8);
	LL_DMA_SetPeriphBurstxfer(DMA1, LL_DMA_STREAM_5, LL_DMA_PBURST_SINGLE);

	//setting I2S3
	I2S_InitStruct.Mode = LL_I2S_MODE_MASTER_TX;
	I2S_InitStruct.Standard = LL_I2S_STANDARD_PHILIPS;
	I2S_InitStruct.MCLKOutput = LL_I2S_MCLK_OUTPUT_DISABLE;
	I2S_InitStruct.DataFormat = LL_I2S_DATAFORMAT_16B;
	I2S_InitStruct.ClockPolarity = LL_I2S_POLARITY_LOW;
	I2S_InitStruct.AudioFreq = LL_I2S_AUDIOFREQ_44K;
	LL_I2S_Init(SPI3, &I2S_InitStruct);
}

void Start_DAC(SPI_TypeDef *spi, DMA_TypeDef *dma_x, uint32_t stream, uint32_t *buff, uint32_t buff_size)
{
	spi->I2SCFGR |= (SPI_I2SCFGR_I2SMOD | SPI_I2SCFGR_I2SE); //select i2s mode and turn on i2s
	DMA_Stream_TypeDef *dma_TX = ((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)dma_x + STREAM_OFFSET_TAB[stream])));
	uint8_t shift[8] = {0, 6, 16, 22, 0, 6, 16, 22}; //bit offset in IFCR flag registers (L and H)
	volatile uint32_t *ifcr_tx = (stream > 3) ? &(dma_x->HIFCR) : &(dma_x->LIFCR);
	dma_TX->CR &= ~DMA_SxCR_EN; 				//turn off the stream DMA
	while (dma_TX->CR & DMA_SxCR_EN) {__NOP();} //waiting for the stream to turn off DMA
	*ifcr_tx = 0x3F<<shift[stream]; 			//reset interrupt flags tx
	spi->CR2 |= SPI_CR2_TXDMAEN; 				//allow spi to send requests to DMA
	//disable interruptions for some events of the tx transmission channel and double buffer mode
	dma_TX->CR &= ~(DMA_SxCR_DMEIE | DMA_SxCR_TEIE | DMA_SxCR_DBM);
	dma_TX->FCR &= ~DMA_SxFCR_FEIE;
	//enable interrupts at the end of the transfer and at the transfer of half the buffer
	dma_TX->CR |= (DMA_SxCR_TCIE | DMA_SxCR_HTIE);
	//configure addresses and length
	dma_TX->PAR = (uint32_t)(&spi->DR); //receiver periphery - address register DR spi
	dma_TX->M0AR = (uint32_t)buff; 		//source memory - address of the outgoing data buffer
	dma_TX->NDTR = (uint32_t)buff_size; //size of transferred data
	dma_TX->CR |= (DMA_SxCR_EN); 		//enable stream DMA
}

void Update_DAC(SPI_TypeDef *spi, DMA_TypeDef *dma_x, uint32_t stream, uint32_t *buff, uint32_t buff_size)
{
	DMA_Stream_TypeDef *dma_TX = ((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)dma_x + STREAM_OFFSET_TAB[stream])));
	uint8_t shift[8] = {0, 6, 16, 22, 0, 6, 16, 22}; //bit offset in flag registers IFCR (L и H)
	volatile uint32_t *ifcr_tx = (stream > 3) ? &(dma_x->HIFCR) : &(dma_x->LIFCR);
	dma_TX->CR &= ~DMA_SxCR_EN; 				//turn off the stream DMA
	while (dma_TX->CR & DMA_SxCR_EN) {__NOP();} //waiting for the channel to turn off DMA
	*ifcr_tx = 0x3F<<shift[stream];				//reset tx interrupt flags
	//---------------------------- Updating buffer data ------------------------------
	dma_TX->M0AR = (uint32_t)buff; 		//source memory - address of the outgoing data buffer
	dma_TX->NDTR = (uint32_t)buff_size; //size of transferred data
	//-----------------------------------------------------------------------------------
	dma_TX->CR |= (DMA_SxCR_EN); 		//turning on the transmission channel
}

void Pause_DAC(SPI_TypeDef *spi)
{
	spi->CR2 &= ~SPI_CR2_TXDMAEN; //prohibit spi from sending requests to DMA
}

void Resume_DAC(SPI_TypeDef *spi)
{
	spi->CR2 |= SPI_CR2_TXDMAEN; //allow spi to send requests to DMA
}

void Stop_DAC(SPI_TypeDef *spi, DMA_TypeDef *dma_x, uint32_t stream)
{
	DMA_Stream_TypeDef *dma_TX = ((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)dma_x + STREAM_OFFSET_TAB[stream])));
	uint8_t shift[8] = {0, 6, 16, 22, 0, 6, 16, 22}; //bit offset in IFCR flag registers (L and H)
	volatile uint32_t *ifcr_tx = (stream > 3) ? &(dma_x->HIFCR) : &(dma_x->LIFCR);
	spi->CR2 &= ~SPI_CR2_TXDMAEN; 				//prohibit spi from sending requests to DMA
	dma_TX->CR &= ~DMA_SxCR_EN; 				//turn off the DMA stream
	while (dma_TX->CR & DMA_SxCR_EN) {__NOP();} //waiting for the DMA channel to disconnect
	*ifcr_tx = 0x3F<<shift[stream];				//reset tx interrupt flags
	spi->I2SCFGR &= ~SPI_I2SCFGR_I2SE; 			//turn off i2s
}

uint8_t Setup_DAC (SPI_TypeDef *spi, uint32_t SampleRate, uint32_t DataFormat)
{
	LL_I2S_InitTypeDef I2S_InitStruct = {0};
	uint32_t pll_N = 0, pll_R = 0;

	typedef struct {
		uint32_t freq, pll_N16, pll_R16, pll_N32, pll_R32;
	} PLL_data;

	//Table with PLLI2S module settings
	const PLL_data pll_table[] = {	{  8000, 192, LL_RCC_PLLI2SR_DIV_2, 192, LL_RCC_PLLI2SR_DIV_3},
									{ 16000, 192, LL_RCC_PLLI2SR_DIV_3, 256, LL_RCC_PLLI2SR_DIV_2},
									{ 22050, 290, LL_RCC_PLLI2SR_DIV_3, 302, LL_RCC_PLLI2SR_DIV_2},
									{ 32000, 256, LL_RCC_PLLI2SR_DIV_2, 256, LL_RCC_PLLI2SR_DIV_5},
									{ 44100, 302, LL_RCC_PLLI2SR_DIV_2, 429, LL_RCC_PLLI2SR_DIV_4},
									{ 48000, 192, LL_RCC_PLLI2SR_DIV_5, 384, LL_RCC_PLLI2SR_DIV_5},
									{ 96000, 384, LL_RCC_PLLI2SR_DIV_5, 424, LL_RCC_PLLI2SR_DIV_3},
									{192000, 424, LL_RCC_PLLI2SR_DIV_3, 258, LL_RCC_PLLI2SR_DIV_3}  };
	const PLL_data *tbl_ptr = pll_table;
	uint8_t i = 0;
	//look in the table for settings for the PLLI2S module
	while (i < 8) {
		if (SampleRate == tbl_ptr->freq) {
			if (DataFormat == LL_I2S_DATAFORMAT_16B ||
				DataFormat == LL_I2S_DATAFORMAT_16B_EXTENDED) {
				pll_N = tbl_ptr->pll_N16;
				pll_R = tbl_ptr->pll_R16;
			}
			else {
				pll_N = tbl_ptr->pll_N32;
				pll_R = tbl_ptr->pll_R32;
			}
			break;
		}
		tbl_ptr++;
		i++;
	}
	if (!pll_N) {
		return 1;
	}

	spi->I2SCFGR &= ~SPI_I2SCFGR_I2SE; //turn off I2S for configuration
	RCC->CR &= ~RCC_CR_PLLI2SON; //turn off the PLLI2S module
	LL_RCC_PLLI2S_ConfigDomain_I2S(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLI2SM_DIV_25, pll_N, pll_R); //setting up the module PLLI2S
	RCC->CR = RCC_CR_PLLI2SON; // enable the moduleь PLLI2S
	while (!(RCC->CR & RCC_CR_PLLI2SRDY)) { __NOP(); } //waiting for module activation PLLI2S

	//setting I2S
	I2S_InitStruct.Mode = LL_I2S_MODE_MASTER_TX;
	I2S_InitStruct.Standard = LL_I2S_STANDARD_PHILIPS;
	I2S_InitStruct.MCLKOutput = LL_I2S_MCLK_OUTPUT_DISABLE;
	I2S_InitStruct.DataFormat = DataFormat;
	I2S_InitStruct.ClockPolarity = LL_I2S_POLARITY_LOW;
	I2S_InitStruct.AudioFreq = SampleRate;
	return LL_I2S_Init(spi, &I2S_InitStruct) == SUCCESS ? 0 : 1;
}
