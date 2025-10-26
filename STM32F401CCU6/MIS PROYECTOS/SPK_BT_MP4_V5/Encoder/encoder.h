/*
  * Author: VadRov
  * Copyright (C) 2019, VadRov, all rights reserved.
  *
  * Incremental mechanical encoder driver.
  *
  * Free distribution allowed.
  * For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
  *
  *Version: for STM32F4
  *
  *	Requirements:
  * - three MK outputs, configured as inputs with external EXTI interrupts on falling and rising edges;
  * - system timer SysTick, configured to interrupt at a frequency of 1000 times per second (used for
  * "system time" when setting a time threshold for debouncing and determining time
  * pressing a button);
  * - incremental mechanical encoder with a button, i.e. with three outputs: A, B, C (button);
  * - ideally a pair of ceramic capacitors 0.02-0.1 µF (one in parallel between the outputs
  * encoder A and "ground", B and "ground");
  * - pull-up of MK inputs to power supply (internal - Pull up option or external - through resistors).
  *
  * Features and capabilities:
  * - EXTI interrupts are used, which completely eliminates the need for constant cyclic
  * polling the encoder state, as would be the case if excessively frequent interrupts were used
  * by updating the timer. Those. analysis of encoder states is carried out only when it is
  * required - by interrupts generated when the edges of signals at its outputs change.
  * - Noise immunity and clarity of encoder operation due to the algorithm for processing the states of the encoder outputs and
  * use of interruptions "in two directions" (on the falling - falling and rising - rising edges at the MK inputs).
  * - Due to the use of “two-way” interrupts simultaneously on two encoder outputs A and B, there is no need for
  * organizing artificial delays in the interrupt handler, as can be found in various sources
  * in a network where only a falling edge interrupt is used on one of the two encoder outputs. Also for
  * by using this approach, the requirements for encoder quality are reduced. The driver, in general, is equally good
  * works out both slow and fast rotations of the encoder knob.
  * - There are no artificial delays in the driver program.
  * - Low computational load on the MK core.
  * - The driver generates events: rotating the encoder knob forward/backward, briefly pressing the button and
  * holding the button, rotating the encoder knob forward/backward while the button is pressed.
  * - The driver allows you to swap the generation of forward/reverse rotation events and forward/reverse rotation events when
  * pressed button, i.e. allows software inversion of the direction of operation of the encoder without the need
  * hardware reconnection of outputs.
  *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "main.h"

#define ENCODER_THRESHOLD	4	/* The absolute threshold value of the encoder counter to confirm the direction of rotation.
								   For the used encoder and MK inputs with configured EXTI interrupts on edges
								   rising/falling direction of rotation is determined after at least 4 interruptions.
								 */

#define ENCODER_BTN_BOUNCE	50	/* The period of time in ms during which the transition is guaranteed to end
								   process (bouncing) when pressing and releasing the encoder button.
								 */

#define ENCODER_BTN_HOLD	2000 /* The time period in ms that defines the button hold event.
								    The event of a short press on the ENCODER_EV_BTN_CLICK button occurs
								    after pressing and releasing the button, if the period of time that has passed since
								    pressing the button until the button is released, less than the value ENCODER_BTN_HOLD.
								    If this period of time is equal to or exceeds the value ENCODER_BTN_HOLD,
								    then the button hold event takes place ENCODER_EV_BTN_HOLD.
								  */

/* Define 7 encoder events: */
#define	ENCODER_EV_NONE			0	/* - no events (insignificant event); */
		       /* The remaining 6 events are significant */
#define	ENCODER_EV_FWD			1	/* - rotation forward, clockwise, etc.; */
#define	ENCODER_EV_BCK			2	/* - rotation backwards, counterclockwise, etc.; */
#define	ENCODER_EV_BTN_CLICK	4	/* - short press of the button; */
#define	ENCODER_EV_BTN_HOLD		8	/* - button hold; */
#define ENCODER_EV_FWD_BTN		16  /* - rotation forward, clockwise, etc. when the button is pressed; */
#define ENCODER_EV_BCK_BTN		32  /* - rotation backwards, counterclockwise, etc. when the button is pressed. */

/* Determining the inversion mode (changing the direction of operation) of the encoder: */
#define	ENCODER_INV_OFF			0	/* - no inversion */
#define	ENCODER_INV_ON			1	/* - operation in inversion mode */

/* Determine which encoder outputs cause an interrupt */
#define ENCODER_IRQ_NONE		0	/* The output is uncertain; */
#define ENCODER_IRQ_A			1	/* Exit A; */
#define ENCODER_IRQ_B			2	/* Exit B; */
#define ENCODER_IRQ_C			3	/* Exit C (button). */

/* The structure describes the encoder handler */
typedef struct {
	GPIO_TypeDef *a_port;				/* The port and its pin number to which the encoder output A is connected. */
	uint16_t a_pin;
	GPIO_TypeDef *b_port;				/* Port and its pin number to which the encoder output B is connected. */
	uint16_t b_pin;
	GPIO_TypeDef *c_port;				/* Port and number of its pin to which output C of the encoder (button) is connected. */
	uint16_t c_pin;
	volatile uint8_t fl_click_btn;		/* Button click flag. Set when you press the button
										   resets when the button is released.
										 */
	volatile uint32_t time_click_btn;	/* Time in ms corresponding to the time the button is held.
										   When the button is clicked and the fl_click_btn flag is set to this variable
										   The system time value millis is entered. When releasing the button
										   the value in the millis variable is subtracted from the current system time
										   variable fl_click_btn. The resulting value determines the time the button is pressed,
										   by the duration of which you can determine which event the press is attributed to
										   on the button: short press or hold.
										 */
	volatile uint32_t time_bounce_btn;	/* The time corresponding to the period of time during which we believe that
										   transient processes (bouncing) caused by pressing and
										   releasing the button. At the moments of pressing/releasing the button and, accordingly,
										   when setting/resetting the fl_click_btn flag, the value is entered into this variable
										   system time millis increased by the amount of time period ENCODER_BTN_BOUNCE,
										   during which transient processes are guaranteed to end.
										   External interrupt handler caused by a change in the edge of the output signal
										   From the encoder (button), it is blocked by software until the system
										   the millis time will not reach the time value in the time_bounce_btn variable.
										   It is this algorithm that filters out chatter without the need
										   organizing artificial delays that slow down the program.
										 */
    uint8_t inversion;					/* A flag that reverses the direction of operation of the encoder. In progress
    									   this flag can be changed by the user with a prototype function:
    									   void EncoderSetInversion(ENCODER_Handler *enc, uint8_t inv);
    									 */
	volatile uint8_t inc_idx;			/* Index in the table to determine increments to the encCounter counter. */
    volatile int encCounter;			/* Encoder counter. By its absolute value we will determine the presence of rotation of the handle
    									   encoder, comparing it with the threshold value ENCODER_THRESHOLD. And by this sign
    									   values - direction of rotation.
    									 */
    volatile uint8_t prev_event;		/* Previous event */
} ENCODER_Handler;

/* Initializes the encoder enc. */
void EncoderInit(ENCODER_Handler *enc,
				 GPIO_TypeDef *a_port, uint16_t a_pin,
				 GPIO_TypeDef *b_port, uint16_t b_pin,
				 GPIO_TypeDef *c_port, uint16_t c_pin);

/*
  * Queries the encoder state and updates its handler. Returns the event
  * which was formed by the time the update request was received.
  */
uint8_t EncoderUpdate(ENCODER_Handler *enc, uint8_t ch_irq);

/*
  * Defines the encoder operating mode: direct (inv = ENCODER_INV_OFF) and reverse/inverse (inv = ENCODER_INV_ON).
  * In inverse operating mode, encoder events are swapped: ENCODER_EV_FWD with ENCODER_EV_BCK,
  * ENCODER_EV_FWD_BTN with ENCODER_EV_BCK_BTN.
  */
void EncoderSetInversion(ENCODER_Handler *enc, uint8_t inv);

#endif /* ENCODER_H_ */
