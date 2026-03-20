/*----------------------------------------------*/
/* TJpgDec System Configurations R0.03          */
/*----------------------------------------------*/
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
 *
 * https://www.youtube.com/@VadRov
 * https://dzen.ru/vadrov
 * https://vk.com/vadrov
 *----------------------------------------------------------------------------------------------*/

/* Optimization for stm32 cortex-m3/m4/m7
/ 0: Off
/ 1: Enabled
  */
#define JD_FAST_STM32   1


/* Specifies size of stream input buffer */
#define	JD_SZBUF		1024


/* Specifies output pixel format.
/  0: RGB888 (24-bit/pix)
/  1: RGB565 (16-bit/pix)
/  2: Grayscale (8-bit/pix)
/  3: Grayscale_16 (16-bit/pix) //(32 shades of gray in 16-bit RGB565 format). Added by VadRov
/								//not supported when optimization is disabled JD_FAST_STM32
*/
#define JD_FORMAT		3


/* Switches output descaling feature.
/  0: Disable
/  1: Enable
*/
#define	JD_USE_SCALE	1


/* Use table conversion for saturation arithmetic. A bit faster, but increases 1 KB of code size.
/  0: Disable
/  1: Enable
*/
#define JD_TBLCLIP		1 //The option does not affect performance in JD_FAST_STM32 mode, because mode
						  //completely superior to the "table version" due to the simd instructions of stm32.
						  //The functionality of this option is completely disabled in JD_FAST_STM32 mode.


/* Optimization level
/  0: Basic optimization. Suitable for 8/16-bit MCUs.
/  1: + 32-bit barrel shifter. Suitable for 32-bit MCUs.
/  2: + Table conversion for huffman decoding (wants 6 << HUFF_BIT bytes of RAM)
*/
#define JD_FASTDECODE	2
