/*
 * rgb_leds_driver.c: Rgb Leds driver for the Bot.
 *
 * This driver controls the 2 RGB LEDs
 * for the Bot and holds the actual regs state
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */

/****************** Include Files ********************/
#include "rgb_leds_driver.h"


/**
 *
 * Init LedsDriver register and set initial values
 *
 * @param   driver holds configuration and values
 * @param   gpio Gpio register
 * @param   deviceID XGpio Device ID for leds register
 * @param   ChannelId XGpio channel supporting the LEDS
 * @param   state  initial rgb leds state 6 bits
 * @return  None.
 *
 *
 */
void RGB_LEDS_DRIVER_init(RgbLedsDriver* driver, XGpio* gpio,
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

	RGB_LEDS_DRIVER_set_rgb_leds_state(driver, state);

}



////////////////////// RGB LEDS STATE ////////////////////////////


void RGB_LEDS_DRIVER_set_rgb_leds_state(RgbLedsDriver* driver, u32 state) {
	driver->state = state;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);

}


u32 RGB_LEDS_DRIVER_get_rgb_leds_state(RgbLedsDriver* driver) {
	return driver->state;
}


///////////////// ON RGB LED 1 ///////////////


void RGB_LEDS_DRIVER_set_rgb_led1_red_on(RgbLedsDriver* driver) {
	driver->state |= RGBLED1_RED_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void RGB_LEDS_DRIVER_set_rgb_led1_green_on(RgbLedsDriver* driver) {
	driver->state |= RGBLED1_GREEN_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void RGB_LEDS_DRIVER_set_rgb_led1_blue_on(RgbLedsDriver* driver) {
	driver->state |= RGBLED1_BLUE_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

///////////////// ON RGB LED 2 ///////////////

void RGB_LEDS_DRIVER_set_rgb_led2_red_on(RgbLedsDriver* driver) {
	driver->state |= RGBLED2_RED_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void RGB_LEDS_DRIVER_set_rgb_led2_green_on(RgbLedsDriver* driver) {
	driver->state |= RGBLED2_GREEN_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void RGB_LEDS_DRIVER_set_rgb_led2_blue_on(RgbLedsDriver* driver) {
	driver->state |= RGBLED2_BLUE_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

///////////////// OFF RGB LED 1 ///////////////


void RGB_LEDS_DRIVER_set_rgb_led1_red_off(RgbLedsDriver* driver) {
	driver->state &= ~ RGBLED1_RED_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void RGB_LEDS_DRIVER_set_rgb_led1_green_off(RgbLedsDriver* driver) {
	driver->state &= ~ RGBLED1_GREEN_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void RGB_LEDS_DRIVER_set_rgb_led1_blue_off(RgbLedsDriver* driver) {
	driver->state &= ~ RGBLED1_BLUE_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

///////////////// OFF RGB LED 2 ///////////////

void RGB_LEDS_DRIVER_set_rgb_led2_red_off(RgbLedsDriver* driver){
	driver->state &= ~ RGBLED2_RED_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void RGB_LEDS_DRIVER_set_rgb_led2_green_off(RgbLedsDriver* driver) {
	driver->state &= ~ RGBLED2_GREEN_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void RGB_LEDS_DRIVER_set_rgb_led2_blue_off(RgbLedsDriver* driver){
	driver->state &= ~ RGBLED2_BLUE_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

///////////////// TOGGLE RGB LED 1 ///////////////

void RGB_LEDS_DRIVER_set_rgb_led1_red_toggle(RgbLedsDriver* driver){
	driver->state ^= RGBLED1_RED_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}
void RGB_LEDS_DRIVER_set_rgb_led1_green_toggle(RgbLedsDriver* driver){
	driver->state ^= RGBLED1_GREEN_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void RGB_LEDS_DRIVER_set_rgb_led1_blue_toggle(RgbLedsDriver* driver){
	driver->state ^= RGBLED1_BLUE_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}


///////////////// TOGGLE RGB LED 2 ///////////////

void RGB_LEDS_DRIVER_set_rgb_led2_red_toggle(RgbLedsDriver* driver){
	driver->state ^= RGBLED2_RED_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}
void RGB_LEDS_DRIVER_set_rgb_led2_green_toggle(RgbLedsDriver* driver){
	driver->state ^= RGBLED2_GREEN_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void RGB_LEDS_DRIVER_set_rgb_led2_blue_toggle(RgbLedsDriver* driver){
	driver->state ^= RGBLED2_BLUE_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}


void RGB_LEDS_DRIVER_set_rgb_led1_set_color(RgbLedsDriver* driver, u32 colorMask) {
	RGB_LEDS_DRIVER_set_rgb_leds_state(driver, (driver->state & 0b111000) | colorMask);
}


void RGB_LEDS_DRIVER_set_rgb_led2_set_color(RgbLedsDriver* driver, u32 colorMask){
	RGB_LEDS_DRIVER_set_rgb_leds_state(driver, (driver->state & 0b000111) | (colorMask<<3));

}


