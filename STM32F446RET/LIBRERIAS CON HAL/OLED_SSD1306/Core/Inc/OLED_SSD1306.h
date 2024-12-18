/*
 * OLED_SSD1306.h
 *
 *  Created on: Oct 25, 2022
 *      Author: Giovanny Anglas
 */

#ifndef INC_OLED_SSD1306_H_
#define INC_OLED_SSD1306_H_

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

/* Exported constant ---------------------------------------------------------*/

#define SSD1306_I2C_ADDRESS         0x78 //0x7A

#define CMD							0x00
#define DAT							0x40

#define SSD1306_LCDWIDTH            128
#define SSD1306_LCDHEIGHT           32 //64

#define SSD1306_SETCONTRAST		    0x81
#define SET_ENTIRE_DISPLAY_ON	    0xA4
#define SSD1306_DISPLAYALLON	    0xA5
#define SSD1306_NORMALDISPLAY		0xA6
#define SSD1306_INVERTDISPLAY		0xA7

#define SSD1306_DISPLAYOFF			0xAE
#define SSD1306_DISPLAYON			0xAF

#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETCOMPINS			0xDA

#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETPRECHARGE		0xD9
#define SET_VCOMH_LEVEL				0xDB

#define SSD1306_SETMULTIPLEX		0xA8

#define SSD1306_SETLOWCOLUMN		0x00
#define SSD1306_SETHIGHCOLUMN		0x10

#define SSD1306_SETSTARTLINE		0x40

#define SSD1306_MEMORYMODE			0x20
#define SSD1306_COLUMNADDR			0x21
#define SSD1306_PAGEADDR			0x22

#define SSD1306_SEGREMAP			    0xA0
#define SET_COM_OUTPUT_SCAN_DIRECTION	0xC0

#define SSD1306_CHARGEPUMP		    0x8D

//Mapeo de la direccion de la columna
#define COLUMN_ADDRESS_0_MAPPED_TO_SEG0     0x00
#define COLUMN_ADDRESS_0_MAPPED_TO_SEG127   0x01

//Escaneo de direccion de las filas
#define SCAN_FROM_COM0_TO_63                0x00
#define SCAN_FROM_COM63_TO_0                0x08

//modos de operacion
#define HORIZONTAL_ADDRESSING_MODE  0x00
#define VERTICAL_ADDRESSING_MODE    0x01
#define PAGE_ADDRESSING_MODE        0x02

#define FONT_1_WIDTH    6
#define FONT_1_HEIGHT   8

#define FONT_2_WIDTH    9
#define FONT_2_HEIGHT   16

#define FONT_3_WIDTH    20
#define FONT_3_HEIGHT   24

//const uint8_t FONT_1[552];  //arreglo de 92x6     6x8  pixeles
//const uint8_t FONT_2[1710]; //arreglo de 95x18    9x16 pixeles
//const uint8_t FONT_3[5760]; //arreglo de 96x60    20x24

/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void OLED_Write(void);
void OLED_Init(void);
void OLED_Write_Command_3bytes(uint8_t byte1, uint8_t byte2, uint8_t byte3);
void OLED_Write_Data_1byte(uint8_t byte1);
void OLED_Draw_Pixel(uint8_t pag_inicio, uint8_t pag_final, uint8_t col_inicio, uint16_t col_final, uint8_t pixel);
void OLED_Clear(void);
void OLED_Draw_8_Pixel(uint8_t pag_inicio, uint8_t col_inicio, uint8_t pixel_8bits);
void OLED_Print_Letra(uint8_t pag, uint8_t col, uint8_t font_size, char letra);
void OLED_Print_Text(uint8_t pag, uint8_t col, uint8_t font_size, char *texto);
void OLED_Imagen(const unsigned char imagen[]);


#endif /* INC_OLED_SSD1306_H_ */
