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

#include "keyboard.h"
#include "stdlib.h"

#if NOSTANDART_USE == 0
uint32_t KEYB_old_keys = 0;						//Previous state of the keys (bit set - the corresponding key is pressed).
												//The variable size of 32 bits allows you to store information about the state of 32 buttons

uint32_t KEYB_repeat_pre_count = 0; 			//Counter for the first auto-repeat
uint8_t  KEYB_repeat_fl = 0;					//Auto repeat flag
uint32_t KEYB_repeat_count = 0; 				//Auto repeat counter
uint8_t  KEYB_contact_counter = 0;  			//Debounce counter
#endif
uint32_t KEYB_key_buff[KEYB_SIZE_BUFFER] = {0}; //Buffer for storing button states.
										   	    //Allows you to store KEYB_SIZE_BUFFER of previous button states for
										   	    //organization of so-called buffered input
uint8_t  KEYB_count_key = 0;					//Counter-position in the button state buffer
uint8_t  KEYB_all_button = 0;					//Button counter
#if NOSTANDART_USE == 0
KEYB_key_map_ *KEYB_keys_map = 0;				//Pointer to an array containing button data

//function adds a button
void KEYB_Add_Button(GPIO_TypeDef *key_port, uint32_t key_pin, uint8_t key_bit, KEYB_BUTTON_STATUS key_status)
{
	KEYB_all_button++;
	KEYB_keys_map = (KEYB_key_map_*)realloc(KEYB_keys_map, KEYB_all_button*sizeof(KEYB_key_map_));
	KEYB_key_map_ *button = &KEYB_keys_map[KEYB_all_button-1];
	button->key_port = key_port;
	button->key_pin = key_pin;
	button->key_bit = key_bit;
	button->key_status = key_status;
}

//The function returns the status of the button
KEYB_BUTTON_STATUS KEYB_Get_Button_Status(uint8_t key_bit)
{
	uint8_t i = KEYB_all_button;
	KEYB_BUTTON_STATUS stat = KEYB_BUTTON_UNKNOW;
	while(i)
	{
		if (KEYB_keys_map[KEYB_all_button - i].key_bit == key_bit)
		{
			stat = KEYB_keys_map[KEYB_all_button - i].key_status;
			break;
		}
		i--;
	}
	return stat;
}

//The function sets the button status
void KEYB_Set_Button_Status(uint8_t key_bit, KEYB_BUTTON_STATUS key_status)
{
	uint8_t i = KEYB_all_button;
	while(i)
	{
		if (KEYB_keys_map[KEYB_all_button - i].key_bit == key_bit)
		{
			KEYB_keys_map[KEYB_all_button - i].key_status = key_status;
			return;
		}
		i--;
	}
}
#endif

//function returns the bit field of the keys from the current position of the input buffer
//Used in a user program to implement button polling.
//In order to find out whether a button is pressed or not, you need to poll the corresponding
//this bit is in the returned value.
//Poll buttons are added using the KEYB_Add_Button function.
uint32_t KEYB_Inkeys(void)
{
	uint32_t k_inkey = 0;
	//is there data in the buffer and is at least one button defined?
#if NOSTANDART_USE == 0
	KEYB_TIM->DIER &= ~TIM_DIER_UIE;
#endif
	if (KEYB_count_key && KEYB_all_button)
	{
		//read data from buffer position 0
		k_inkey = KEYB_key_buff[0];
		//shift the buffer 1 position to the left
		for (int i = 0; i < KEYB_SIZE_BUFFER - 1; i++)
		{
			KEYB_key_buff[i] = KEYB_key_buff[i + 1];
		}
		//reduce the amount of data in the buffer by 1
		KEYB_count_key--;
		//we add 0
		KEYB_key_buff[KEYB_SIZE_BUFFER - 1] = 0;
	}
#if NOSTANDART_USE == 0
	KEYB_TIM->DIER |= TIM_DIER_UIE;
#endif
	return k_inkey;
}

//The function returns 1 if any button is pressed. Otherwise it returns 0.
//In any case, the button state is not removed from the input buffer
uint8_t KEYB_kbhit(void)
{
	//is there data in the buffer and is at least one button defined?
	if (KEYB_count_key && KEYB_all_button)	{
		return 1;
	}
	return 0;
}

#if NOSTANDART_USE == 0
//polling buttons
void KEYB_Input_Keys(void)
{
	KEYB_TIM->SR &= ~TIM_SR_UIF; //reset interrupt flag on update
	uint32_t keys = 0;//variable to store button states
	int i = 0;
	KEYB_key_map_ *button; //stores a pointer to the data (port, pin, bit) of the current button
	if (!KEYB_all_button) return; //if there are no added buttons, then exit
	while(i < KEYB_all_button) //in a loop we read the state of all buttons whose parameters are specified in the KEYB_keys_map array
	{
		button = &KEYB_keys_map[i++]; //pointer to current button data
		if (button->key_status != KEYB_BUTTON_ACTIVE) continue; //if the button is inactive (not used), then skip it
		//The button is pressed if the input state is 0 (the input is pressed to ground). When this condition is met, set the bit
		//corresponding to the button in the keys variable.
		if (!(button->key_port->IDR & button->key_pin)) keys |= 1 << button->key_bit;
	}
	if (keys && (KEYB_old_keys == keys)) //is the previous button/combination equal to the current one?
	{
		KEYB_contact_counter++; //increase the bounce filter counter
		if (KEYB_contact_counter < KEYB_CONTACT_PER) return; //Has the time passed to eliminate the chatter?
		if (KEYB_repeat_fl) //Is the auto-repeat flag set?
		{
			KEYB_contact_counter = KEYB_CONTACT_PER;
			KEYB_repeat_count++; //increase the auto-repeat counter
			if (KEYB_repeat_count < KEYB_REPEAT_PER) return; //Has the auto-repeat time passed?
			KEYB_repeat_count = 0; //initialize the auto-repeat counter
		}
		else
		{
			if (!KEYB_repeat_pre_count) //Is this the first auto-repeat?
			{
				//remember the state of the buttons in the buffer
				if (KEYB_count_key < KEYB_SIZE_BUFFER) //is there any space in the input buffer?
				{
					KEYB_key_buff[KEYB_count_key] = keys; //remember the state of the buttons in the buffer
					KEYB_count_key++; //increase the counter of remembered button states
				}
			}
			KEYB_repeat_pre_count++; //increase the delay counter until the first auto-repeat
			if (KEYB_repeat_pre_count < KEYB_REPPRE_PER) return; //
			KEYB_repeat_pre_count = 0; //initialize the delay counter until the first autorepeat
			KEYB_repeat_fl = 1; //set the auto-repeat flag
			KEYB_repeat_count = 0; //initialize the auto-repeat counter
			KEYB_contact_counter = KEYB_CONTACT_PER;
			return;
		}
		if (KEYB_count_key < KEYB_SIZE_BUFFER) //is there any space in the input buffer?
		{
			KEYB_key_buff[KEYB_count_key] = keys; //remember the state of the buttons in the buffer
			KEYB_count_key++; //increase the counter of button states stored in the buffer
		}
		return;
	}
	//the current button/combination is different from the previous one
	KEYB_old_keys = keys; //old combination = current
	KEYB_contact_counter = 0; //initializing the debouncing counter
	KEYB_repeat_fl = 0; //resetting the auto-repeat flag
	KEYB_repeat_count = 0; //autorepeat counter initialization
	KEYB_repeat_pre_count = 0; //initialization of the delay counter until the first autorepeat
}
#endif
