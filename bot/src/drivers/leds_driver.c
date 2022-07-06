/*
 * leds_driver.c: Leds driver for the Bot.
 *
 * This driver controls the 4 red LEDS
 * for the Bot and holds the actual regs state
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */

/****************** Include Files ********************/
#include "leds_driver.h"


/**
 *
 * Init driver register and set initial values
 *
 * @param   driver holds configuration and values
 * @param   gpio Gpio register
 * @param   deviceID XGpio Device ID for leds register
 * @param   channelID XGpio channel supporting the LEDS
 * @param   state  initial red leds state 4 bits
 * @return  None.
 *
 *
 */
void LEDS_DRIVER_init(LedsDriver* driver, XGpio* gpio,
		u16 deviceID, unsigned channelID, u32 state) {

	driver->gpio = gpio;
	driver->channelID = channelID;
	driver->state = state;

	XGpio_Config *cfg_ptr;

	// initialize leds device
	cfg_ptr = XGpio_LookupConfig(deviceID);
	XGpio_CfgInitialize(gpio, cfg_ptr, cfg_ptr->BaseAddress);

	// Set Led Tristate
	XGpio_SetDataDirection(gpio, channelID, 0);

	LEDS_DRIVER_set_leds_state(driver, state);

}


u32 LEDS_DRIVER_get_leds_state(LedsDriver* driver) {
	return driver->state;
}

void LEDS_DRIVER_set_leds_state(LedsDriver* driver, u32 state) {
	driver->state = state;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);

}


///////////// ON LEDS ////////////////////

void LEDS_DRIVER_set_led1_on(LedsDriver* driver) {
	driver->state |= LED1_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void LEDS_DRIVER_set_led2_on(LedsDriver* driver) {
	driver->state |= LED2_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void LEDS_DRIVER_set_led3_on(LedsDriver* driver) {
	driver->state |= LED3_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void LEDS_DRIVER_set_led4_on(LedsDriver* driver) {
	driver->state |= LED4_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

///////////// OFF LEDS ////////////////////

void LEDS_DRIVER_set_led1_off(LedsDriver* driver) {
	driver->state &= ~ LED1_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void LEDS_DRIVER_set_led2_off(LedsDriver* driver) {
	driver->state &= ~ LED2_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void LEDS_DRIVER_set_led3_off(LedsDriver* driver) {
	driver->state &= ~ LED3_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void LEDS_DRIVER_set_led4_off(LedsDriver* driver) {
	driver->state &= ~LED4_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

///////////// TOGGLE LEDS ////////////////////

void LEDS_DRIVER_set_led1_toggle(LedsDriver* driver) {
	driver->state ^= LED1_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void LEDS_DRIVER_set_led2_toggle(LedsDriver* driver) {
	driver->state  ^=  LED2_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void LEDS_DRIVER_set_led3_toggle(LedsDriver* driver) {
	driver->state  ^= LED3_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void LEDS_DRIVER_set_led4_toggle(LedsDriver* driver) {
	driver->state ^= LED4_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}




