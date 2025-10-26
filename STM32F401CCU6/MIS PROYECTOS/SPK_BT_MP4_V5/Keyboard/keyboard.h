/* ------------------------------------------------ ------------------------------------/
  / Module for working with buttons on the stm32f4 microcontroller (and not only).
  *------------------------------------------------ -----------------------------------/
  *
  * Author: VadRov
  * Copyright (C) 2021, VadRov, all rights reserved.
  *
  * Free distribution allowed.
  *For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
  *
  * Module capabilities:
  * - Supports up to 32 buttons, taking into account the fact that 1 microcontroller output is allocated for 1 button,
  * configured for input with power supply.
  * - The debounce filter setting is available.
  * - A button state buffer with customizable depth is available.
  * - You can configure delay periods before the first auto-repeat of pressing buttons and subsequent auto-repeat.
  * - It is possible to add buttons for polling by calling the corresponding function.
  * - Setting button statuses is available: “active” - participates in the survey, “passive” - does not participate in the survey.
  * - Poll of button status (active or passive) is available.
  *
  * The microcontroller pins to which the buttons are connected must be configured as inputs with power supply. Alternatively, using
  * internal pull-up of the microcontroller, or external through a pull-up resistor, for example, rated 10 kOhm.
  * When the button is pressed, the input should be drawn to ground (GND).
  *
  * The library requires 1 timer with a configured update interrupt for operation.
  * In the timer interrupt handling function (for example, in the stm32f4xx_it.c file), you must include the following line:
  * KEYB_Input_Keys(); //call the button polling procedure
  * In the KEYB_Input_Keys() call file, you must define the function prototype: extern void KEYB_Input_Keys(void)
  * or include the keyboard.h header file: #include "keyboard.h"
  * Yes, and do not forget to enable timer update interrupts in your program and enable the timer.
  * For example, like this for stm32f4:
  * KEYB_TIM->DIER |= TIM_DIER_UIE;
KEYB_TIM->CR1 |= TIM_CR1_CEN;
  * where KEYB_TIM is the timer number defined in the keyboard.h file
  *
  * We add our own survey buttons using the function
  * KEYB_Add_Button(key_port, key_pin, key_bit, key_status);
  * where: key_port - pointer to the button port used, for example, GPIOA (port A);
  * key_pin - the button port pin used, for example, LL_GPIO_PIN_3 (3 pins);
  * key_bit - bit number corresponding to the button in the button state variable, for example, 0 (0 bit)
  * key_status - button status: active (KEYB_BUTTON_ACTIVE) or passive (KEYB_BUTTON_PASSIVE),
  * active is polled, but passive is not.
  *
  * The current state of the buttons (from the buffer) is obtained by calling the KEYB_Inkeys() function, which is in a 32-bit variable
  * returns the so-called “bit field”, in which each button corresponds to a bit, the number of which is the bit number
  * specified for the button in the "Description of Buttons". A set bit means the button is pressed, a reset bit means the button is not pressed.
  *
  * The status of the buttons (active/passive) can be changed by calling the function KEYB_Set_Button_Status(key_bit, key_status),
  * passing the button bit number from the description and the desired status.
  *
  * The status of a button (active/passive) can be obtained by calling the function KEYB_Get_Button_Status(key_bit),
  * passing the button bit number from the description.
  *
  * The module can be easily edited for use with any microcontroller (not only stm32).
  *
  * All module settings are defined in the file keyboard.h
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "main.h"

/*---------------------------------------------------------------------------------------------------------------
/                           Configurable module parameters
-----------------------------------------------------------------------------------------------------------------*/

#define NOSTANDART_USE	0

#if NOSTANDART_USE == 0
//Timer
#define KEYB_TIM		TIM10
#else
extern uint32_t KEYB_key_buff[];
extern uint8_t  KEYB_count_key;
extern uint8_t  KEYB_all_button;
#endif
//Description of the buttons. Option. Each button corresponds to a specific bit.
//The library allows working with 32 buttons (according to the number of bits in a 32-bit number)
#define KEYB_UP			0	//0 bit - up button
#define KEYB_DOWN		1	//1 bit - button down
#define KEYB_LEFT		2	//2 bit - left button
#define KEYB_RIGHT		3	//3 bit - right button
#define KEYB_F1   		4	//4 pages
#define KEYB_F2   		5	//4 pages
//......................6   //6 pages
//and so on up to 31 bits
#if NOSTANDART_USE == 0
//Input setting option (parameters are shown for a timer set to 200 Hz)
#define KEYB_CONTACT_PER  2 //Period to eliminate contact bounce. Depends on the polling timer frequency and button characteristics.
							//This is the time during which the button’s state is considered to be stable, i.e. there are no transient processes,
							//so-called contact bounce, and the button went from one stable state to another stable state.
							//Determined experimentally, including using a logic analyzer or oscilloscope.
							//Set in the number of polling timer periods.
#define KEYB_REPPRE_PER	100 //Period until the first auto-repeat
							//Set in the number of polling timer periods.
#define KEYB_REPEAT_PER	  8 //The period of subsequent auto-repetitions when the button is held down.
							//Set in the number of polling timer periods.
#endif

#define KEYB_SIZE_BUFFER 20 //Input buffer size. Defines the buffer size (in depth) for storing previous button states.
/* -----------------------------------------------------------------------------
 *                            End of module settings
 *------------------------------------------------------------------------------*/

#if NOSTANDART_USE == 0
//Structure describing the button
//key_port - pointer to the port to which the button is connected
//key_pin - number of the pin to which the button is connected
//key_bit - the bit number that defines the button in the button state variable.
//key_status - flag that determines whether the button is active (KEYB_BUTTON_ACTIVE) or not (KEYB_BUTTON_PASSIVE).

typedef enum {
	KEYB_BUTTON_ACTIVE = 0,
	KEYB_BUTTON_PASSIVE,
	KEYB_BUTTON_UNKNOW
} KEYB_BUTTON_STATUS;

typedef struct {
	GPIO_TypeDef *key_port;
	uint32_t key_pin;
	uint8_t key_bit;
	KEYB_BUTTON_STATUS key_status;
} KEYB_key_map_;

//The function adds a poll button
//input port, pin, bit, status
void KEYB_Add_Button(GPIO_TypeDef *key_port, uint32_t key_pin, uint8_t key_bit, KEYB_BUTTON_STATUS key_status);

//The function returns the button status
KEYB_BUTTON_STATUS KEYB_Get_Button_Status(uint8_t key_bit);

//The function sets the button status
void KEYB_Set_Button_Status(uint8_t key_bit, KEYB_BUTTON_STATUS key_status);

//The function polls buttons in a timer interrupt.
//The call to this function must be added to the timer interrupt handler.
void KEYB_Input_Keys(void);

#endif

//The function returns the bit field of the keys from the current position of the input buffer.
//Used in a user program to implement button polling.
//In order to find out whether a button is pressed or not, you need to poll the corresponding
//this bit is in the returned value. This bit is set in the button definitions (see Button Descriptions above).
//Poll buttons are added using the KEYB_Add_Button function.
uint32_t KEYB_Inkeys(void);

//The function returns 1 if any button is pressed. Otherwise it returns 0.
//Button states are not removed from the input buffer
uint8_t KEYB_kbhit(void);

#endif /* KEYBOARD_H_ */
