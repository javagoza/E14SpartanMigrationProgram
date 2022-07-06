/*
 * leds_driver.c: Leds driver for the Bot.
 *
 * This driver controls the signals to control the HBridge for the two motor
 * for the Bot and holds the actual regs state
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */

/****************** Include Files ********************/
#include "hbridge_driver.h"


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
void HBRIDGE_DRIVER_init(HBridgeDriver* driver, XGpio* gpio,
		u16 deviceID, unsigned channelID, u32 state) {

	driver->gpio = gpio;
	driver->channelID = channelID;
	driver->state = state;

	XGpio_Config *cfg_ptr;

	// initialize hbridge device
	cfg_ptr = XGpio_LookupConfig(deviceID);
	XGpio_CfgInitialize(gpio, cfg_ptr, cfg_ptr->BaseAddress);

	// Set Led Tristate
	XGpio_SetDataDirection(gpio, channelID, 0);

	HBRIDGE_DRIVER_set_hbridge_state(driver, state);

}


u32 HBRIDGE_DRIVER_get_hbridge_state(HBridgeDriver* driver) {
	return driver->state;
}

void HBRIDGE_DRIVER_set_hbridge_state(HBridgeDriver* driver, u32 state) {
	driver->state = state;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);

}


///////////// HIGH SIGNALS ////////////////////

void HBRIDGE_DRIVER_left_motor_backward(HBridgeDriver* driver) {
	driver->state |= LEFT_MOTOR_DIR_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void HBRIDGE_DRIVER_right_motor_backward(HBridgeDriver* driver) {
	driver->state |= RIGHT_MOTOR_DIR_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

///////////// LOW SiGNALS ////////////////////

void HBRIDGE_DRIVER_left_motor_forward(HBridgeDriver* driver) {
	driver->state &= ~ LEFT_MOTOR_DIR_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}


void HBRIDGE_DRIVER_right_motor_forward(HBridgeDriver* driver) {
	driver->state &= ~ RIGHT_MOTOR_DIR_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}


///////////// TOGGLE LEDS ////////////////////

void HBRIDGE_DRIVER_left_motor_direction_toggle(HBridgeDriver* driver) {
	driver->state ^= LEFT_MOTOR_DIR_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}

void HBRIDGE_DRIVER_right_motor_direction_toggle(HBridgeDriver* driver) {
	driver->state  ^= RIGHT_MOTOR_DIR_MASK;
	XGpio_DiscreteWrite(driver->gpio,
			driver->channelID, driver->state);
}




