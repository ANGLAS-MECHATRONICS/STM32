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

#ifndef PCM5102_H_
#define PCM5102_H_

#include "main.h"

void Init_DAC(void);
uint8_t Setup_DAC(SPI_TypeDef *spi, uint32_t SampleRate, uint32_t DataFormat);
void Start_DAC(SPI_TypeDef *spi, DMA_TypeDef *dma_x, uint32_t stream, uint32_t *buff, uint32_t buff_size);
void Update_DAC(SPI_TypeDef *spi, DMA_TypeDef *dma_x, uint32_t stream, uint32_t *buff, uint32_t buff_size);
void Pause_DAC(SPI_TypeDef *spi);
void Resume_DAC(SPI_TypeDef *spi);
void Stop_DAC(SPI_TypeDef *spi, DMA_TypeDef *dma_x, uint32_t stream);

#endif /* PCM5102_H_ */
