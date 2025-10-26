/*
  * Display control driver via SPI
  * Author: VadRov
  * Copyright (C) 2019 - 2023, VadRov, all rights reserved.
  *
  * Free distribution allowed.
  * For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
  *
  * Version: 1.4 Turbo (CMSIS, LL, assembler) for STM32F4
  *
  * Configuration file
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */

#ifndef DISPLAY_CONFIG_H_
#define DISPLAY_CONFIG_H_

/* -------------------- Using an assembler version of a number of driver functions ---------------------------*/
#define LCD_USE_ASSEMBLER  1		//0 - not used
									//1 - used
/* -----------------------------------------------------------------------------------------------------------*/

/* -------------------------------------------------------------------------------------------------------------
							Settings for reading data from the display controller
 -------------------------------------------------------------------------------------------------------------*/
/* -- Using spi Half-Duplex Master mode when reading data from the display controller --
   Used for procedures for reading information from the display controller, for example: LCD_ReadImage,
   if the display is connected via a bidirectional data line, e.g. SDA в st7789
   1 - half duplex is used (Half-Duplex), 0 - full duplex is used(Full-Duplex)
---------------------------------------------------------------------------------------------*/
#define SPI_HALF_DUPLEX_READ	1

/* ----------------------------- Speed of spi when reading from display -------------------------
   from 0 (max) to 7 (min), where: 0 -> clk/2, 1 -> clk/4, 2 -> clk/8, ..., 7 -> clk/256
   Used for procedures for reading information from the display controller, for example: LCD_ReadImage
 -------------------------------------------------------------------------------------------*/
#define SPI_SPEED_DISPLAY_READ	3 //According to the specifications, read speed from controllers
								  //st7789 and ili9341 displays are not recommended higher than 6.7 Mbit/s
/*------------------------------------------------------------------------------------------*/

/* -------------------------------------------------------------------------------------------------------------------
						          Selecting a Memory Allocation Control Mechanism
----------------------------------------------------------------------------------------------------------------------*/
//#define LCD_DYNAMIC_MEM				//Using dynamic memory allocation (malloc, free) by the driver.
										//If you want to use static memory allocation,
										//then comment out this line.
/* --------------------------------------------------------------------------------------------------------------------- */

#endif /* DISPLAY_CONFIG_H_ */
