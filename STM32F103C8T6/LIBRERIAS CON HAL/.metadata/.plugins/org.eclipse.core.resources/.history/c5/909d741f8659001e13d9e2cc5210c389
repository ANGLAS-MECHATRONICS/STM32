/*
 * Anglas_LCD_I2C.c
 *
 *  Created on: Oct 21, 2022
 *      Author: PC
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Anglas_LCD_I2C.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constant ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static void LCD_I2C_WriteCommand(uint8_t command){//Enviar un byte de comando al controlador hitachi HD44780
    uint8_t highnibble,lownibble,datos[4];

    highnibble = (command & 0xF0);
    lownibble  = (uint8_t)((command & 0x0F)<<4);

    datos[0] = highnibble | BACKLIGHT | PIN_EN; //backlight ON,    habilito EN (EN=1, RS=0)
    datos[1] = highnibble | BACKLIGHT;          //backlight ON, deshabilito EN (EN=0, RS=0)
    datos[2] = lownibble  | BACKLIGHT | PIN_EN; //backlight ON,    habilito EN (EN=1, RS=0)
    datos[3] = lownibble  | BACKLIGHT;		    //backlight ON, deshabilito EN (EN=0, RS=0)

    HAL_I2C_Master_Transmit (&hi2c1, PCF8574_ADDRESS, (uint8_t *)datos, 4, 100);
}


static void LCD_I2C_WriteData(uint8_t data){//Enviar un byte de datos al controlador hitachi HD44780
	uint8_t highnibble,lownibble,datos[4];

    highnibble = (data & 0xF0);
    lownibble  = (uint8_t)((data & 0x0F)<<4);

    datos[0] = highnibble | BACKLIGHT | PIN_EN | PIN_RS; //backlight ON,    habilito EN (EN=1, RS=1)
    datos[1] = highnibble | BACKLIGHT | PIN_RS;          //backlight ON, deshabilito EN (EN=0, RS=1)
    datos[2] = lownibble  | BACKLIGHT | PIN_EN | PIN_RS; //backlight ON,    habilito EN (EN=1, RS=1)
    datos[3] = lownibble  | BACKLIGHT | PIN_RS;          //backlight ON, deshabilito EN (EN=0, RS=1)

    HAL_I2C_Master_Transmit (&hi2c1, PCF8574_ADDRESS, (uint8_t *)datos, 4, 100);
}

/* Exported functions --------------------------------------------------------*/
void LCD_I2C_Init(void){
	HAL_Delay(1000);                     //tiempo necesario para no obtener caracteres erroneos al energizar el lcd
	HAL_I2C_Master_Transmit (&hi2c1, PCF8574_ADDRESS, 0, 1, 100); //mando un "0" (backlight apagado)
	HAL_Delay(500);                      // esperar un tiempo mayor  a 100ms para VDD estable
    LCD_I2C_WriteCommand(MODE_4_BITS);   // function set, 4 bit mode
    HAL_Delay(1);                        // esperar un tiempo >100uS
    LCD_I2C_WriteCommand(MODE_4_BITS);   // function set, 4 bit mode otra vez segun datasheet
    HAL_Delay(1);                        // esperar un tiempo >100uS
    LCD_I2C_WriteCommand(MODE_4_BITS);   // function set, 4 bit mode otra vez segun datasheet
    HAL_Delay(1);                        // esperar un tiempo >100uS

    LCD_I2C_WriteCommand(0b00100000);    // mando el valor que me indica en la incializacion
    HAL_Delay(1);                        // esperar un tiempo >100uS

    LCD_I2C_WriteCommand(MODE_4_BITS);   // function set, 4 bit mode otra vez segun datasheet
    HAL_Delay(1);                        // esperar un tiempo >53uS

    LCD_I2C_WriteCommand(DISPLAY_ON);    // display control on, cursor off, blink off
    HAL_Delay(1);                        // esperar un tiempo >53uS

    LCD_I2C_WriteCommand(DISPLAY_CLEAR); // display control on, cursor off, blink off
    HAL_Delay(1);                        // esperar un tiempo >3mS

    LCD_I2C_WriteCommand(ENTRY_MODE_SET);// cursor se mueve hacia la derecha, el contador de direcciones DDRAM (AC) más 1
    HAL_Delay(1);                        // esperar un tiempo >53uS, fin de la inicializacion

    LCD_I2C_WriteCommand(0b00001100);    // mando el valor que me indica en la incializacion
    HAL_Delay(1);                        // esperar un tiempo >53uS

    LCD_I2C_ClearText();
}

void LCD_I2C_ClearText(){// Limpiar el texto de la pantalla
  LCD_I2C_WriteCommand(DISPLAY_CLEAR);
  HAL_Delay(5);
}

static void LCD_I2C_Blink(void){
	LCD_I2C_WriteCommand(0x0F);
}
static void LCD_I2C_NoBlink(void){
	LCD_I2C_WriteCommand(0x0C);
}

void LCD_I2C_Set_Cursor_Blink(uint8_t row, uint8_t col, uint8_t blink){
	row = row - 1;//para empezar desde la fila    1 y no desde la fila    0
	col = col - 1;//para empezar desde la columna 1 y no desde la columna 0
    //row -> 1,2,3,4        col -> 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
    switch(row){
        case 0: col = col + 0x80;break; // operacion para agregar ala fila 0 su columna "x"
        case 1: col = col + 0xC0;break; // operacion para agregar ala fila 1 su columna "x"
        case 2: col = col + 0x94;break; // operacion para agregar ala fila 2 su columna "x"
        case 3: col = col + 0xD4;break; // operacion para agregar ala fila 3 su columna "x"
        default:col = col + 0x80;break; // fila 0 si ingresa un valor que no es del 0 al 3
    }

    LCD_I2C_WriteCommand(col);

    switch(blink){
		case 0: LCD_I2C_NoBlink();break;
		case 1: LCD_I2C_Blink()  ;break;
    }
}

void LCD_I2C_WriteText(uint8_t row, uint8_t col, char* string){//Escribir en la pantalla
	row = row - 1;//para empezar desde la fila    1 y no desde la fila    0
	col = col - 1;//para empezar desde la columna 1 y no desde la columna 0
	//row -> 1,2,3,4        col -> 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
    switch (row) {
        case 0: col = col + 0x80;break; // operacion para agregar ala fila 0 su columna "x"
        case 1: col = col + 0xC0;break; // operacion para agregar ala fila 1 su columna "x"
        case 2: col = col + 0x94;break; // operacion para agregar ala fila 2 su columna "x"
        case 3: col = col + 0xD4;break; // operacion para agregar ala fila 3 su columna "x"
        default:col = col + 0x80;break; // fila 0 si ingresa un valor que no es del 0 al 3
    }

    LCD_I2C_WriteCommand(col);
    //Funcion para mandar strings sin saber el tamaño de la cadena
    //while(*string != '\0')
    while (*string)
        LCD_I2C_WriteData(*string++);
}


void LCD_I2C_Shift_Right(void){LCD_I2C_WriteCommand(0x1C);}
void LCD_I2C_Shift_Left(void){LCD_I2C_WriteCommand(0x18);}



void LCD_I2C_CGRAM_CreateChar(uint8_t pos, const uint8_t *msg){
	pos = pos - 1;//para empezar a enumarar desde el 1 y no desde 0
    if(pos < 8){//puedo usar maximo 8 caracteres especiales
    	LCD_I2C_WriteCommand(0x40 + (pos*8));
        for(unsigned char i=0; i<8; i++){
        	LCD_I2C_WriteData(msg[i]);
        }
    }
}

void LCD_I2C_CGRAM_WriteChar(uint8_t row, uint8_t col, uint8_t pos){
	pos = pos - 1;//para empezar a enumarar desde el 1 y no desde 0

	row = row - 1;//para empezar desde la fila    1 y no desde la fila    0
	col = col - 1;//para empezar desde la columna 1 y no desde la columna 0
	//row -> 1,2,3,4        col -> 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
    switch (row) {
        case 0: col = col + 0x80;break; // operacion para agregar ala fila 0 su columna "x"
        case 1: col = col + 0xC0;break; // operacion para agregar ala fila 1 su columna "x"
        case 2: col = col + 0x94;break; // operacion para agregar ala fila 2 su columna "x"
        case 3: col = col + 0xD4;break; // operacion para agregar ala fila 3 su columna "x"
        default:col = col + 0x80;break; // fila 0 si ingresa un valor que no es del 0 al 3
    }
    LCD_I2C_WriteCommand(col);

	LCD_I2C_WriteData(pos);
}
