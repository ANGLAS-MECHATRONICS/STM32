/*
  * Author: VadRov
  * Copyright (C) 2022 - 2023, VadRov, all rights reserved.
  *
  * Pairing the TJpgDec library with the display driver
  *
  * Free distribution allowed.
  * For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */

#ifndef INC_JPEG_CHAN_H_
#define INC_JPEG_CHAN_H_

#include "display.h"

#define JPEG_CHAN_WORK_BUFFER_SIZE	12000 //maximum memory allocated for the decoder

typedef enum {
	PICTURE_IN_FILE,
	PICTURE_IN_MEMORY
} PictureLocation;

typedef struct {
	const uint8_t *data;	//pointer to an array containing image data
	const uint32_t size;	//array size
} iPicture_jpg;

typedef struct {
	void *file;   			//pointer to file handler
	volatile uint32_t size;	//file size
} iFile_jpg;

typedef struct {
    void *file;   			//input stream
    LCD_Handler *lcd;		//pointer to display handler
    uint16_t x_offs, y_offs;
} IODEV;

uint8_t LCD_Load_JPG_chan (LCD_Handler *lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, void *image_stream, PictureLocation location);

#endif /* INC_JPEG_CHAN_H_ */
