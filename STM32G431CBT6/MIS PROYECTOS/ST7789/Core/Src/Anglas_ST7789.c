/*
 * Anglas_ST7789.c
 *
 *  Created on: Jul 22, 2024
 *  Author: Golinskiy Konstantin (golinskiy.konstantin@gmail.com)
 *  Modificado por: Giovanny Anglas
 */

#include "main.h"
#include "Anglas_ST7789.h"
#include "stdlib.h"
#include "string.h"


uint16_t ST7789_X_Start = ST7789_XSTART;
uint16_t ST7789_Y_Start = ST7789_YSTART;
uint16_t ST7789_Width, ST7789_Height;

void ST7789_SendCmd(uint8_t Cmd){
	 // pin DC LOW
	 HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
	 HAL_SPI_Transmit(&hspi1, &Cmd, 1, HAL_MAX_DELAY);
	 while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY){};
	 // pin DC HIGH
	 HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
}

void ST7789_SendData(uint8_t Data ){
	HAL_SPI_Transmit(&hspi1, &Data, 1, HAL_MAX_DELAY);
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY){};
}

void ST7789_SendDataMASS(uint8_t* buff, size_t buff_size){

	if( buff_size <= 0xFFFF ){
		HAL_SPI_Transmit(&hspi1, buff, buff_size, HAL_MAX_DELAY);
	}
	else{
		while( buff_size > 0xFFFF ){
			HAL_SPI_Transmit(&hspi1, buff, 0xFFFF, HAL_MAX_DELAY);
			buff_size-=0xFFFF;
			buff+=0xFFFF;
		}
		HAL_SPI_Transmit(&hspi1, buff, buff_size, HAL_MAX_DELAY);
	}

	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY){};
}

// Datos para inicializar la pantalla ST7789

// La inicialización es la misma para todas las pantallas, ya que el controlador está diseñado para un tamaño máximo de 240x320.
// Para ajustarlo a tu tamaño, muévelo usando la función de rotación de la pantalla -> ST7789_rotation()
static const uint8_t init_cmds[] = {
		9,                       			// 9 commands in list:
		ST7789_SWRESET,   DELAY,    		// 1: Software reset, no args, w/delay
		  150,                     			//    150 ms delay
		ST7789_SLPOUT ,  DELAY,    			// 2: Out of sleep mode, no args, w/delay
		  255,                            	//    255 = 500 ms delay
		ST7789_COLMOD , 1+DELAY,    		// 3: Set color mode, 1 arg + delay:
		  (ST7789_ColorMode_65K | ST7789_ColorMode_16bit),           //    16-bit color 0x55
		  10,                             	//    10 ms delay
		ST7789_MADCTL , 1,                 	// 4: Memory access ctrl (directions), 1 arg:
		  0x00,                           	//    Row addr/col addr, bottom to top refresh
		ST7789_CASET  , 4,                 	// 5: Column addr set, 4 args, no delay:
		  0,0,                             	//    XSTART = 0
		  0,240,                           	//    XEND = 240
		ST7789_RASET  , 4,                 	// 6: Row addr set, 4 args, no delay:
		  0,0,                             	//    YSTART = 0
		  320>>8,320&0xff,                 	//    YEND = 240   320>>8,320&0xff,
		ST7789_INVON ,   DELAY,     		// 7: Inversion ON
		  10,
		ST7789_NORON  ,   DELAY,    		// 8: Normal display on, no args, w/delay
		  10,                              	// 10 ms delay
		ST7789_DISPON ,   DELAY,    		// 9: Main screen turn on, no args, w/delay
		  10
	};

// Procedimiento de envío de datos para inicializar la pantalla
static void ST7789_ExecuteCommandList(const uint8_t *addr) {

    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--) {
        uint8_t cmd = *addr++;
        ST7789_SendCmd(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
            ST7789_SendDataMASS((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            HAL_Delay(ms);
        }
    }
}


// Procedimiento para configurar las direcciones inicial y final de las columnas.
static void ST7789_ColumnSet(uint16_t ColumnStart, uint16_t ColumnEnd){

  if (ColumnStart > ColumnEnd){
    return;
  }

  if (ColumnEnd > ST7789_Width){
    return;
  }

  ColumnStart += ST7789_X_Start;
  ColumnEnd += ST7789_X_Start;

  ST7789_SendCmd(ST7789_CASET);
  ST7789_SendData(ColumnStart >> 8);
  ST7789_SendData(ColumnStart & 0xFF);
  ST7789_SendData(ColumnEnd >> 8);
  ST7789_SendData(ColumnEnd & 0xFF);

}

// Procedimiento de inicialización de pantalla
void ST7789_Init(void){

	// Retraso después del encendido (si la pantalla no siempre comienza al inicio, aumenta el tiempo de retraso)
	HAL_Delay(300);

	ST7789_Width = ST7789_WIDTH;
	ST7789_Height = ST7789_HEIGHT;

    ST7789_Select();
    ST7789_HardReset();
    ST7789_ExecuteCommandList(init_cmds);
    ST7789_Unselect();
}

//Procedimiento para configurar direcciones de inicio y fin de filas.
static void ST7789_RowSet(uint16_t RowStart, uint16_t RowEnd){

  if (RowStart > RowEnd){
    return;
  }

  if (RowEnd > ST7789_Height){
    return;
  }

  RowStart += ST7789_Y_Start;
  RowEnd += ST7789_Y_Start;

  ST7789_SendCmd(ST7789_RASET);
  ST7789_SendData(RowStart >> 8);
  ST7789_SendData(RowStart & 0xFF);
  ST7789_SendData(RowEnd >> 8);
  ST7789_SendData(RowEnd & 0xFF);

}

// Procedimiento de control SPI para poner a bajo el chip select y seleccionar el dispositvo
void ST7789_Select(void) {
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
}

// Procedimiento de control SPI para poner a alto el chip select y deseleccionar el dispositvo
void ST7789_Unselect(void) {
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

// Procedimiento para dibujar una imagen a colores (usando "IMG_to_HEX_converter.py")
void ST7789_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {

    if((x >= ST7789_Width) || (y >= ST7789_Height)){
		return;
	}

    if((x + w - 1) >= ST7789_Width){
		return;
	}

    if((y + h - 1) >= ST7789_Height){
		return;
	}

    ST7789_SetWindow(x, y, x+w-1, y+h-1);

	ST7789_Select();

    ST7789_SendDataMASS((uint8_t*)data, sizeof(uint16_t)*w*h);

    ST7789_Unselect();
}
// Funcion para dar un reset al controlador ST7789
void ST7789_HardReset(void){

	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(20);
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);

}

// Procedimiento para habilitar el modo de suspensión
void ST7789_SleepModeEnter( void ){
	ST7789_Select();
	ST7789_SendCmd(ST7789_SLPIN);
	ST7789_Unselect();
	HAL_Delay(250);
}

// Procedimiento para desactivar el modo de suspensión
void ST7789_SleepModeExit( void ){
	ST7789_Select();
	ST7789_SendCmd(ST7789_SLPOUT);
	ST7789_Unselect();
	HAL_Delay(250);
}

// Procedimiento para habilitar/deshabilitar el modo de llenado parcial de pantalla
void ST7789_InversionMode(uint8_t Mode){

  ST7789_Select();

  if (Mode){
    ST7789_SendCmd(ST7789_INVON);
  }
  else{
    ST7789_SendCmd(ST7789_INVOFF);
  }

  ST7789_Unselect();
}

// Procedimiento para rellenar toda la pantalla de un color
void ST7789_FillScreen(uint16_t color){
  ST7789_FillRect(0, 0,  ST7789_Width, ST7789_Height, color);
}

// Procedimiento de limpieza de pantalla: pinta la pantalla de negro
void ST7789_Clear(void){
  ST7789_FillRect(0, 0,  ST7789_Width, ST7789_Height, 0);
}

// Procedimiento para rellenar un rectángulo con color.
void ST7789_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){

  if ((x >= ST7789_Width) || (y >= ST7789_Height)){
	  return;
  }

  if ((x + w) > ST7789_Width){
	  w = ST7789_Width - x;
  }

  if ((y + h) > ST7789_Height){
	  h = ST7789_Height - y;
  }

  ST7789_SetWindow(x, y, x + w - 1, y + h - 1);

//  for (uint32_t i = 0; i < (h * w); i++){
//	  ST7789_RamWrite(&color, 1);
//  }

  ST7789_RamWrite(&color, (h * w));
}

// Procedimiento para establecer límites de pantalla para el llenado
void ST7789_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
	ST7789_Select();
	ST7789_ColumnSet(x0, x1);
	ST7789_RowSet(y0, y1);
	// write to RAM
	ST7789_SendCmd(ST7789_RAMWR);
	ST7789_Unselect();
}

// Procedimiento para escribir datos en la pantalla.
void ST7789_RamWrite(uint16_t *pBuff, uint32_t Len){

  ST7789_Select();

  uint8_t buff[2];
  buff[0] = *pBuff >> 8;
  buff[1] = *pBuff & 0xFF;

  while (Len--){
//    ST7789_SendData( buff[0] );
//    ST7789_SendData( buff[1] );
	  ST7789_SendDataMASS( buff, 2);
  }

  ST7789_Unselect();
}

// Procedimiento de control de retroiluminación (PWM)
void ST7789_SetBL(uint8_t Value){
//  if (Value > 100)
//    Value = 100;

//	tmr2_PWM_set(ST77xx_PWM_TMR2_Chan, Value);
}

// Procedimiento de encendido/apagado de la pantalla
void ST7789_DisplayPower(uint8_t On){

  ST7789_Select();

  if (On){
    ST7789_SendCmd(ST7789_DISPON);
  }
  else{
    ST7789_SendCmd(ST7789_DISPOFF);
  }

  ST7789_Unselect();
}

// Procedimiento para dibujar un rectángulo (hueco)
void ST7789_DrawRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

  ST7789_DrawLine(x1, y1, x1, y2, color);
  ST7789_DrawLine(x2, y1, x2, y2, color);
  ST7789_DrawLine(x1, y1, x2, y1, color);
  ST7789_DrawLine(x1, y2, x2, y2, color);

}

// Procedimiento auxiliar -> Procedimiento para dibujar un rectángulo (relleno)
static void SwapInt16Values(int16_t *pValue1, int16_t *pValue2){
  int16_t TempValue = *pValue1;
  *pValue1 = *pValue2;
  *pValue2 = TempValue;
}

// Procedimiento para dibujar un rectángulo (relleno)
void ST7789_DrawRectangleFilled(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t fillcolor) {

  if (x1 > x2){
    SwapInt16Values(&x1, &x2);
  }

  if (y1 > y2){
    SwapInt16Values(&y1, &y2);
  }

  ST7789_FillRect(x1, y1, x2 - x1, y2 - y1, fillcolor);
}

// Procedimiento auxiliar -> Procedimiento de dibujo lineal
static void ST7789_DrawLine_Slow(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

  const int16_t deltaX = abs(x2 - x1);
  const int16_t deltaY = abs(y2 - y1);
  const int16_t signX = x1 < x2 ? 1 : -1;
  const int16_t signY = y1 < y2 ? 1 : -1;

  int16_t error = deltaX - deltaY;

  ST7789_DrawPixel(x2, y2, color);

  while (x1 != x2 || y1 != y2) {

    ST7789_DrawPixel(x1, y1, color);
    const int16_t error2 = error * 2;

    if (error2 > -deltaY) {

      error -= deltaY;
      x1 += signX;
    }
    if (error2 < deltaX){

      error += deltaX;
      y1 += signY;
    }
  }
}

// Procedimiento de dibujo lineal
void ST7789_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

  if (x1 == x2){

    if (y1 > y2){
      ST7789_FillRect(x1, y2, 1, y1 - y2 + 1, color);
	}
    else{
      ST7789_FillRect(x1, y1, 1, y2 - y1 + 1, color);
	}

    return;
  }

  if (y1 == y2){

    if (x1 > x2){
      ST7789_FillRect(x2, y1, x1 - x2 + 1, 1, color);
	}
    else{
      ST7789_FillRect(x1, y1, x2 - x1 + 1, 1, color);
	}

    return;
  }

  ST7789_DrawLine_Slow(x1, y1, x2, y2, color);
}

// Procedimiento para dibujar un triángulo (hueco)
void ST7789_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color){
	/* Draw lines */
	ST7789_DrawLine(x1, y1, x2, y2, color);
	ST7789_DrawLine(x2, y2, x3, y3, color);
	ST7789_DrawLine(x3, y3, x1, y1, color);
}

// Procedimiento para dibujar un triángulo (relleno)
void ST7789_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color){

	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

	deltax = abs(x2 - x1);
	deltay = abs(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	}
	else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	}
	else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	}
	else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		ST7789_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

// El procedimiento colorea 1 píxel de la pantalla
void ST7789_DrawPixel(int16_t x, int16_t y, uint16_t color){

  if ((x < 0) ||(x >= ST7789_Width) || (y < 0) || (y >= ST7789_Height)){
    return;
  }

  ST7789_SetWindow(x, y, x, y);
  ST7789_RamWrite(&color, 1);
}

// Procedimiento para dibujar un círculo (relleno)
void ST7789_DrawCircleFilled(int16_t x0, int16_t y0, int16_t radius, uint16_t fillcolor) {

  int x = 0;
  int y = radius;
  int delta = 1 - 2 * radius;
  int error = 0;

  while (y >= 0){

    ST7789_DrawLine(x0 + x, y0 - y, x0 + x, y0 + y, fillcolor);
    ST7789_DrawLine(x0 - x, y0 - y, x0 - x, y0 + y, fillcolor);
    error = 2 * (delta + y) - 1;

    if (delta < 0 && error <= 0) {

      ++x;
      delta += 2 * x + 1;
      continue;
    }

    error = 2 * (delta - x) - 1;

    if (delta > 0 && error > 0) {

      --y;
      delta += 1 - 2 * y;
      continue;
    }

    ++x;
    delta += 2 * (x - y);
    --y;
  }
}

// Procedimiento para dibujar un círculo (hueco)
void ST7789_DrawCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color) {

  int x = 0;
  int y = radius;
  int delta = 1 - 2 * radius;
  int error = 0;

  while (y >= 0){

    ST7789_DrawPixel(x0 + x, y0 + y, color);
    ST7789_DrawPixel(x0 + x, y0 - y, color);
    ST7789_DrawPixel(x0 - x, y0 + y, color);
    ST7789_DrawPixel(x0 - x, y0 - y, color);
    error = 2 * (delta + y) - 1;

    if (delta < 0 && error <= 0) {

      ++x;
      delta += 2 * x + 1;
      continue;
    }

    error = 2 * (delta - x) - 1;

    if (delta > 0 && error > 0) {

      --y;
      delta += 1 - 2 * y;
      continue;
    }

    ++x;
    delta += 2 * (x - y);
    --y;
  }
}

// Procedimiento para dibujar un personaje (1 letra o signo)
void ST7789_DrawChar(uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, unsigned char ch){

	uint32_t i, b, j;

	uint32_t X = x, Y = y;

	uint8_t xx, yy;

	if( multiplier < 1 ){
		multiplier = 1;
	}

	/* Check available space in LCD */
	if (ST7789_Width >= ( x + Font->FontWidth) || ST7789_Height >= ( y + Font->FontHeight)){


			/* Go through font */
			for (i = 0; i < Font->FontHeight; i++) {

				if( ch < 127 ){
					b = Font->data[(ch - 32) * Font->FontHeight + i];
				}

				else if( (uint8_t) ch > 191 ){
					// +96 esto se debe a que los caracteres y signos latinos en las fuentes ocupan 96 posiciones
					// y si en una fuente que contiene primero el alfabeto latino y caracteres especiales y luego
					// solo cirílico, entonces necesitas agregar 95 si la fuente es
					// contiene solo cirílico, entonces +96 no es necesario
					b = Font->data[((ch - 192) + 96) * Font->FontHeight + i];
				}

				else if( (uint8_t) ch == 168 ){	// 168 символ по ASCII - Ё
					// 160 elementos ( caracter Ё )
					b = Font->data[( 160 ) * Font->FontHeight + i];
				}

				else if( (uint8_t) ch == 184 ){	// 184 символ по ASCII - ё
					// 161 elementos  ( caracter ё )
					b = Font->data[( 161 ) * Font->FontHeight + i];
				}
				//-------------------------------------------------------------------

				for (j = 0; j < Font->FontWidth; j++) {

					if ((b << j) & 0x8000) {

						for (yy = 0; yy < multiplier; yy++){
							for (xx = 0; xx < multiplier; xx++){
									ST7789_DrawPixel(X+xx, Y+yy, TextColor);
							}
						}

					}
					else if( TransparentBg ){

						for (yy = 0; yy < multiplier; yy++){
							for (xx = 0; xx < multiplier; xx++){
									ST7789_DrawPixel(X+xx, Y+yy, BgColor);
							}
						}

					}
					X = X + multiplier;
				}
				X = x;
				Y = Y + multiplier;
			}
	}
}

// Procedimiento de dibujo lineal
void ST7789_print(uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, char *str){

	if( multiplier < 1 ){
		multiplier = 1;
	}

	unsigned char buff_char;

	uint16_t len = strlen(str);

	while (len--) {

		//------------------------------------------------ ---------------------
		// comprueba si hay cirílico UTF-8, si es latino, omite si
		// Caracteres ASCII extendidos Win-1251 Cirílico (código de caracteres 128-255)
		// verifica el primer byte de dos (ya que UTF-8 son dos bytes)
		// si es mayor o igual a 0xC0 (el primer byte en cirílico será igual a 0xD0 o 0xD1 en el alfabeto)
		if ( (uint8_t)*str >= 0xC0 ){	// код 0xC0 соответствует символу кириллица 'A' по ASCII Win-1251

			// comprueba qué byte es el primer 0xD0 o 0xD1
			switch ((uint8_t)*str) {
				case 0xD0: {
					// aumenta la matriz porque necesitamos el segundo byte
					str++;
					// revisa el segundo byte, ahí está el símbolo mismo
					if ((uint8_t)*str == 0x81) { buff_char = 0xA8; break; }		// byte de caracter E (si se necesitan más caracteres, agréguelos aquí y en la función DrawChar())
					if ((uint8_t)*str >= 0x90 && (uint8_t)*str <= 0xBF){ buff_char = (*str) + 0x30; }	// byte de carácter A...I a...p cambia a +48
					break;
				}
				case 0xD1: {
					// aumenta la matriz porque necesitamos el segundo byte
					str++;
					// revisa el segundo byte, ahí está el símbolo mismo
					if ((uint8_t)*str == 0x91) { buff_char = 0xB8; break; }		// byte de carácter е (si se necesitan más caracteres, agréguelos aquí y en la función DrawChar())
					if ((uint8_t)*str >= 0x80 && (uint8_t)*str <= 0x8F){ buff_char = (*str) + 0x70; }	// byte de caracteres p...i cambiemos a +112
					break;
				}
			}
			// reduce otra variable ya que usamos 2 bytes para el alfabeto cirílico
			len--;

			ST7789_DrawChar(x, y, TextColor, BgColor, TransparentBg, Font, multiplier, buff_char);
		}
		//---------------------------------------------------------------------
		else{
			ST7789_DrawChar(x, y, TextColor, BgColor, TransparentBg, Font, multiplier, *str);
		}

		x = x + (Font->FontWidth * multiplier);
		/* Increase string pointer */
		str++;
	}
}

// Procedimiento para rotar (posición) de la pantalla, modo predeterminado 1 (total 1, 2, 3, 4)
void ST7789_rotation( uint8_t rotation ){

	ST7789_Select();
	ST7789_SendCmd(ST7789_MADCTL);

	// el controlador está diseñado para una pantalla de 320 x 240 (tamaño máximo)
	// para que quepa cualquier otro, necesitas restar la diferencia de píxeles
	  switch (rotation) {

		case 1:
			//== 1.13" 135 x 240 ST7789 =================================================
			#ifdef ST7789_IS_135X240
				ST7789_SendData(ST7789_MADCTL_RGB);
				ST7789_Width = 135;
				ST7789_Height = 240;
				ST7789_X_Start = 52;
				ST7789_Y_Start = 40;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================

			//== 1.3" 240 x 240 ST7789 =================================================
			#ifdef ST7789_IS_240X240
				ST7789_SendData(ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 240;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================

			//== 2" 240 x 320 ST7789 =================================================
			#ifdef ST7789_IS_240X320
				ST7789_SendData(ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 320;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================

			//== 1.9" 170 x 320 ST7789 =================================================
			#ifdef ST7789_IS_170X320
				ST7789_SendData(ST7789_MADCTL_RGB);
				ST7789_Width = 170;
				ST7789_Height = 320;
				ST7789_X_Start = 35;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
		 break;

		case 2:
			//== 1.13" 135 x 240 ST7789 =================================================
			#ifdef ST7789_IS_135X240
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 135;
				ST7789_X_Start = 40;
				ST7789_Y_Start = 53;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================

			//== 1.3" 240 x 240 ST7789 =================================================
			#ifdef ST7789_IS_240X240
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 240;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================

			//== 2" 240 x 320 ST7789 =================================================
			#ifdef ST7789_IS_240X320
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 320;
				ST7789_Height = 240;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================

			//== 1.9" 170 x 320 ST7789 =================================================
			#ifdef ST7789_IS_170X320
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 320;
				ST7789_Height = 170;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 35;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
		 break;

	   case 3:
		   //== 1.13" 135 x 240 ST7789 =================================================
			#ifdef ST7789_IS_135X240
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
				ST7789_Width = 135;
				ST7789_Height = 240;
				ST7789_X_Start = 53;
				ST7789_Y_Start = 40;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================

			//== 1.3" 240 x 240 ST7789 =================================================
			#ifdef ST7789_IS_240X240
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 240;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 80;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================

			//== 2" 240 x 320 ST7789 =================================================
			#ifdef ST7789_IS_240X320
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 320;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================

			//== 1.9" 170 x 320 ST7789 =================================================
			#ifdef ST7789_IS_170X320
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
				ST7789_Width = 170;
				ST7789_Height = 320;
				ST7789_X_Start = 35;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
		 break;

	   case 4:
		   //== 1.13" 135 x 240 ST7789 =================================================
			#ifdef ST7789_IS_135X240
				ST7789_SendData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 135;
				ST7789_X_Start = 40;
				ST7789_Y_Start = 52;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================

			//== 1.3" 240 x 240 ST7789 =================================================
			#ifdef ST7789_IS_240X240
				ST7789_SendData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 240;
				ST7789_X_Start = 80;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================

			//== 2" 240 x 320 ST7789 =================================================
			#ifdef ST7789_IS_240X320
				ST7789_SendData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 320;
				ST7789_Height = 240;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================

			//== 1.9" 170 x 320 ST7789 =================================================
			#ifdef ST7789_IS_170X320
				ST7789_SendData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 320;
				ST7789_Height = 170;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 35;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
		 break;

	   default:
		 break;
	  }

	  ST7789_Unselect();
}

// Procedimiento para dibujar un icono monocromo
void ST7789_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color){

    int16_t byteWidth = (w + 7) / 8; 	// Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j=0; j<h; j++, y++){

        for(int16_t i=0; i<w; i++){

            if(i & 7){
               byte <<= 1;
            }
            else{
               byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }

            if(byte & 0x80){
				ST7789_DrawPixel(x+i, y, color);
			}
        }
    }
}

/*
//Procedimiento para mostrar el frame buffer (Necesitas crear el búfer globalmente uint16_t buff_frame[ST7789_WIDTH*ST7789_HEIGHT];)
void ST7789_Update(uint16_t color) {

	for( uint16_t i =0; i < ST7789_Width*ST7789_Height; i ++ ){
		buff_frame[i] = color;
	}

    ST7789_SetWindow(0, 0, ST7789_Width, ST7789_Height);
	ST7789_Select();
    ST7789_SendDataMASS((uint8_t*)buff_frame, sizeof(uint16_t)*ST7789_Width*ST7789_Height);
    ST7789_Unselect();
}

// La prueba muestra alternativamente imágenes de la unidad flash SD.
void Test_displayImage(const char* fname)
{
  FRESULT res;

  FIL file;
  res = f_open(&file, fname, FA_READ);
  if (res != FR_OK)
    return;

  unsigned int bytesRead;
  uint8_t header[34];
  res = f_read(&file, header, sizeof(header), &bytesRead);
  if (res != FR_OK)
  {
    f_close(&file);
    return;
  }

  if ((header[0] != 0x42) || (header[1] != 0x4D))
  {
    f_close(&file);
    return;
  }

  uint32_t imageOffset = header[10] | (header[11] << 8) | (header[12] << 16) | (header[13] << 24);
  uint32_t imageWidth  = header[18] | (header[19] << 8) | (header[20] << 16) | (header[21] << 24);
  uint32_t imageHeight = header[22] | (header[23] << 8) | (header[24] << 16) | (header[25] << 24);
  uint16_t imagePlanes = header[26] | (header[27] << 8);

  uint16_t imageBitsPerPixel = header[28] | (header[29] << 8);
  uint32_t imageCompression  = header[30] | (header[31] << 8) | (header[32] << 16) | (header[33] << 24);

  if((imagePlanes != 1) || (imageBitsPerPixel != 24) || (imageCompression != 0))
  {
    f_close(&file);
    return;
  }

  res = f_lseek(&file, imageOffset);
  if(res != FR_OK)
  {
    f_close(&file);
    return;
  }

  // Подготавливаем буфер строки картинки для вывода
  uint8_t imageRow[(240 * 3 + 3) & ~3];
  uint16_t PixBuff[240];

  for (uint32_t y = 0; y < imageHeight; y++)
  {
    res = f_read(&file, imageRow, (imageWidth * 3 + 3) & ~3, &bytesRead);
    if (res != FR_OK)
    {
      f_close(&file);
      return;
    }

    uint32_t rowIdx = 0;
    for (uint32_t x = 0; x < imageWidth; x++)
    {
      uint8_t b = imageRow[rowIdx++];
      uint8_t g = imageRow[rowIdx++];
      uint8_t r = imageRow[rowIdx++];
      PixBuff[x] = RGB565(r, g, b);
    }

    dispcolor_DrawPartXY(0, imageHeight - y - 1, imageWidth, 1, PixBuff);
  }

  f_close(&file);
}

// Prueba de visualización de imágenes

void Test240x240_Images(void)
{
  FATFS fatfs;
  DIR DirInfo;
  FILINFO FileInfo;
  FRESULT res;

  res = f_mount(&fatfs, "0", 1);
  if (res != FR_OK)
    return;

  res = f_chdir("/240x240");
  if (res != FR_OK)
    return;

  res = f_opendir(&DirInfo, "");
  if (res != FR_OK)
    return;

  while (1)
  {
    res = f_readdir(&DirInfo, &FileInfo);
    if (res != FR_OK)
      break;

    if (FileInfo.fname[0] == 0)
      break;

    char *pExt = strstr(FileInfo.fname, ".BMP");
    if (pExt)
    {
      Test_displayImage(FileInfo.fname);
      delay_ms(2000);
    }
  }
}
*/

