/*----------------------------------------------------------------------------/
/ TJpgDec - Tiny JPEG Decompressor R0.03 include file         (C)ChaN, 2021
/----------------------------------------------------------------------------*/
/*
* Portions copyright (C) 2022-2023, VadRov, all right reserved.
  * Optimization (including for stm32), improvements, additions:
  * - In the JDEC structure, the workbuf buffer is doubled, an offset pointer is introduced
  * in this buffer to use DMA. The decoder does not stand idle, waiting for output to
  * display of previously decoded block;
  * - An additional pixel color format Grayscale_16 has been introduced. 32 shades of gray in 16-bit
  * color format R5G6B5. Better performance compared to RGB565 color
  * up to 1.5 - 2 times.
  * - The mcu_output procedure has been optimized: performance has been significantly improved
  * for color in R5G6B5 format.
  * The intermediate step of color conversion from R8G8B8 to R5G6B5 format has been eliminated. R5G6B5 now
  * formed directly without additional transformations;
  * - The mcu_load procedure has been optimized;
  * - Implemented procedures for quickly converting YCbCr to R5G6B5 and R8G8B8, Y to 16
  * bit monochrome (32 shades of gray);
  * - Implemented quick filling of memory areas with a given width:
  * - per byte (memset_8 - asm);
  * - half word (memset_16 - asm);
  * - into a word (memset_32 - asm).
  * - Implemented fast copying of memory areas (memcpy_8 - byte-by-byte, memcpy_16 by half-word);
  * - Implemented an asm version of the block_idct function (inverse discrete cosine transform).
  * To enable stm32 optimization, use the JD_FAST_STM32 definition in the header file
 *  tjpgdcnf.h
 *
 * https://www.youtube.com/@VadRov
 * https://dzen.ru/vadrov
 * https://vk.com/vadrov
 *----------------------------------------------------------------------------------------------*/

#ifndef DEF_TJPGDEC
#define DEF_TJPGDEC

#ifdef __cplusplus
extern "C" {
#endif

#include "tjpgdcnf.h"
#include <string.h>

#if defined(_WIN32)	/* VC++ or some compiler without stdint.h */
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef short			int16_t;
typedef unsigned long	uint32_t;
typedef long			int32_t;
#else				/* Embedded platform */
#include <stdint.h>
#endif

#if JD_FASTDECODE >= 1
typedef int16_t jd_yuv_t;
#else
typedef uint8_t jd_yuv_t;
#endif


/* Error code */
typedef enum {
	JDR_OK = 0,	/* 0: Succeeded */
	JDR_INTR,	/* 1: Interrupted by output function */	
	JDR_INP,	/* 2: Device error or wrong termination of input stream */
	JDR_MEM1,	/* 3: Insufficient memory pool for the image */
	JDR_MEM2,	/* 4: Insufficient stream input buffer */
	JDR_PAR,	/* 5: Parameter error */
	JDR_FMT1,	/* 6: Data format error (may be broken data) */
	JDR_FMT2,	/* 7: Right format but not supported */
	JDR_FMT3	/* 8: Not supported JPEG standard */
} JRESULT;



/* Rectangular region in the output image */
typedef struct {
	uint16_t left;		/* Left end */
	uint16_t right;		/* Right end */
	uint16_t top;		/* Top end */
	uint16_t bottom;	/* Bottom end */
} JRECT;



/* Decompressor object structure */
typedef struct JDEC JDEC;
struct JDEC {
	size_t dctr;				/* Number of bytes available in the input buffer */
	uint8_t* dptr;				/* Current data read ptr */
	uint8_t* inbuf;				/* Bit stream input buffer */
	uint8_t dbit;				/* Number of bits availavble in wreg or reading bit mask */
	uint8_t scale;				/* Output scaling ratio */
	uint8_t msx, msy;			/* MCU size in unit of block (width, height) */
	uint8_t qtid[3];			/* Quantization table ID of each component, Y, Cb, Cr */
	uint8_t ncomp;				/* Number of color components 1:grayscale, 3:color */
	int16_t dcv[3];				/* Previous DC element of each component */
	uint16_t nrst;				/* Restart inverval */
	uint16_t width, height;		/* Size of the input image (pixel) */
	uint8_t* huffbits[2][2];	/* Huffman bit distribution tables [id][dcac] */
	uint16_t* huffcode[2][2];	/* Huffman code word tables [id][dcac] */
	uint8_t* huffdata[2][2];	/* Huffman decoded data tables [id][dcac] */
	int32_t* qttbl[4];			/* Dequantizer tables [id] */
#if JD_FASTDECODE >= 1
	uint32_t wreg;				/* Working shift register */
	uint8_t marker;				/* Detected marker (0:None) */
#if JD_FASTDECODE == 2
	uint8_t longofs[2][2];		/* Table offset of long code [id][dcac] */
	uint16_t* hufflut_ac[2];	/* Fast huffman decode tables for AC short code [id] */
	uint8_t* hufflut_dc[2];		/* Fast huffman decode tables for DC short code [id] */
#endif
#endif
	void* workbuf;				/* Working buffer for IDCT and RGB output */
//++++++++++++++++++++++ Изменения для двойного буфера ++++++++++++++++++++++++
	int offset;
	int offset_value;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	uint8_t color_format;		//содержит при инициализации значение макроопределения JD_FORMAT
								//используется для смены формата цвета в процессе работы
	jd_yuv_t* mcubuf;			/* Working buffer for the MCU */
	void* pool;					/* Pointer to available memory pool */
	size_t sz_pool;				/* Size of momory pool (bytes available) */
	size_t (*infunc)(JDEC*, uint8_t*, size_t);	/* Pointer to jpeg stream input function */
	void* device;				/* Pointer to I/O device identifiler for the session */
};



/* TJpgDec API functions */
JRESULT jd_prepare (JDEC* jd, size_t (*infunc)(JDEC*,uint8_t*,size_t), void* pool, size_t sz_pool, void* dev);
JRESULT jd_decomp (JDEC* jd, int (*outfunc)(JDEC*,void*,JRECT*), uint8_t scale);


#ifdef __cplusplus
}
#endif

#endif /* _TJPGDEC */
