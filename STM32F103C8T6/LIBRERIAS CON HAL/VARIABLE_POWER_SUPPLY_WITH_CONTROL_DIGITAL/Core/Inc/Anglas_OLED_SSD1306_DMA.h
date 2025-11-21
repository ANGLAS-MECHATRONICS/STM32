/*
 * Anglas_OLED_SSD1306_DMA.h
 *
 *  Created on: Nov 14, 2025
 *      Author: Giovanny Anglas
 */

#ifndef INC_ANGLAS_OLED_SSD1306_DMA_H_
#define INC_ANGLAS_OLED_SSD1306_DMA_H_

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;
//extern volatile uint8_t oled_tx_done;
/* Exported constant ---------------------------------------------------------*/

#define SSD1306_I2C_ADDRESS         0x78 //0x7A

#define CMD							0x00
#define DAT							0x40

#define SSD1306_LCDWIDTH            128
#define SSD1306_LCDHEIGHT           64 //32

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
//Macro para mostrar un dato solo si cambia su valor (para no enviar el mismo dato y saturar el i2c)
#define OLED_UpdateIfChanged(pag, col, size, format, value, last)   \
    do {                                                            \
        if ((value) != *(last)) {                             		\
            char buff[20];                                    		\
            sprintf(buff, format, value);                     		\
            OLED_Print_Text_DMA(pag, col, size, buff);           	\
            *(last) = (value);                                		\
        }                                                     		\
    } while(0)

/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

void OLED_Init_DMA(void);
void OLED_Print_Text_DMA(uint8_t pag, uint8_t col, uint8_t font_size, char *texto);
void OLED_Clear_DMA(void);

void OLED_Imagen_DMA(const unsigned char imagen[]);
void OLED_Imagen_Invert_DMA(const unsigned char imagen[]);

void OLED_Imagen_Small_DMA(uint8_t pag, uint8_t col, const unsigned char imagen[], uint8_t size_x, uint8_t size_y);
void OLED_Imagen_Small_Invert_DMA(uint8_t pag, uint8_t col, const unsigned char imagen[], uint8_t size_x, uint8_t size_y);

#endif /* INC_ANGLAS_OLED_SSD1306_DMA_H_ */
