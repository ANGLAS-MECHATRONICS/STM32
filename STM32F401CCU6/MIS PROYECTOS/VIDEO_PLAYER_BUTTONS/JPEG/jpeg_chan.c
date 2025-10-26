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

#include "ff.h"
#include "jpeg_chan.h"
#include "tjpgd.h"
#include "display.h"
#include "avi.h"
#include "microgl2d.h"

extern MGL_OBJ *AVI_osd;
uint32_t OSD_cycle_counter; 	//counter of clock cycles spent on OSD display
								//when outputting the current frame
extern uint8_t AVI_color_mode;

typedef struct {
	uint8_t *data;
	uint32_t data_left;
} Data_Mem_src;

uint8_t work_buffer[JPEG_CHAN_WORK_BUFFER_SIZE];

//Receiving data from external media - sd card (jpeg file on sd card)
static inline unsigned int tjd_input_file (JDEC* jd, uint8_t* buff, unsigned int nd)
{
	IODEV *iodev = (IODEV *)jd->device;
	FIL *file = (FIL *)((iFile_jpg *)iodev->file)->file;
	jd = jd;
	if (((iFile_jpg *)iodev->file)->size < nd) {
		nd = ((iFile_jpg *)iodev->file)->size;
		((iFile_jpg *)iodev->file)->size = 0;
	}
	else {
		((iFile_jpg *)iodev->file)->size -= nd;
	}
	if (buff) {
		UINT read_b = 0;
		NVIC_DisableIRQ(AudioDAC_IRQn);
		f_read(file, buff, nd, &read_b);
		NVIC_EnableIRQ(AudioDAC_IRQn);
		return read_b;
	}
	else {
		NVIC_DisableIRQ(AudioDAC_IRQn);
		FRESULT res = f_lseek(file, f_tell(file) + (FSIZE_t)nd);
		NVIC_EnableIRQ(AudioDAC_IRQn);
		return res == FR_OK ? nd : 0;
	}
}

//Receiving data from memory (jpeg file in flash MK)
static inline unsigned int tjd_input_memory (JDEC* jd, uint8_t* buff, unsigned int nd)
{
	IODEV *iodev = (IODEV *)jd->device;
	jd = jd;
	if (((Data_Mem_src*)iodev->file)->data_left < nd) {
		nd = ((Data_Mem_src*)iodev->file)->data_left;
	}
	if (buff) {
		memcpy(buff, ((Data_Mem_src*)iodev->file)->data, nd);
	}
	((Data_Mem_src*)iodev->file)->data += nd;
	((Data_Mem_src*)iodev->file)->data_left -= nd;
	return nd;
}

//Output the bitmap memory block to the display at the specified position rect
static inline int tjd_output (JDEC* jd, void* bitmap, JRECT* rect)
{
	jd = jd;
	IODEV *iodev = (IODEV *)jd->device;

	if (AVI_osd) { //Overlay graphic objects from the AVI_osd list
		DWT->CYCCNT = 0;
		MGL_RenderObjects(	AVI_osd,
							rect->left + iodev->x_offs,
							rect->top + iodev->y_offs,
							rect->right + iodev->x_offs,
							rect->bottom + iodev->y_offs,
							(uint16_t*)bitmap );
		OSD_cycle_counter += DWT->CYCCNT;
	}

	//Output the block to the display
	LCD_DrawImage(iodev->lcd, rect->left + iodev->x_offs, rect->top + iodev->y_offs, rect->right - rect->left + 1,
			      rect->bottom - rect->top + 1, (uint16_t*)bitmap, 1);
	return JDR_OK;
}

//Output a jpeg image to the display.
//location determines the location of the file (on an SD card or in a flash MK)
uint8_t LCD_Load_JPG_chan (LCD_Handler *lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, void *image_stream, PictureLocation location)
{
	JDEC jd;
	JRESULT rc = JDR_PAR;
	uint8_t scale;

	IODEV iodev;
	iodev.lcd = lcd;
	iodev.x_offs = x;
	iodev.y_offs = y;
	OSD_cycle_counter = 0;
	if (location == PICTURE_IN_FILE) {
		iodev.file = image_stream;
		rc = jd_prepare(&jd, tjd_input_file, work_buffer, JPEG_CHAN_WORK_BUFFER_SIZE, &iodev);
	}
	else if (location == PICTURE_IN_MEMORY)	{
		Data_Mem_src data;
		data.data = (uint8_t *)((iPicture_jpg*)image_stream)->data;
		data.data_left = ((iPicture_jpg*)image_stream)->size;
		iodev.file = (void *)&data;
		rc = jd_prepare(&jd, tjd_input_memory, work_buffer, JPEG_CHAN_WORK_BUFFER_SIZE, &iodev);
	}
	if (rc == JDR_OK) {
		for (scale = 0; scale < 3; scale++) {
			if ((jd.width >> scale) <= w && (jd.height >> scale) <= h) break;
		}
		rc = jd_decomp(&jd, tjd_output, scale);
	}
	return rc;
}
