/*
 *
 * switches_driver.c: Switches driver for the Bot.
 *
 * This driver controls the 4 switches
 * for the Bot and holds the actual regs state
 *
 * Author: Enrique Albertos
 * Date: 2022/07/06
 *
 */

/****************** Include Files ********************/

#include "switches_driver.h"

void SWITCHES_DRIVER_init(SwitchesDriver* driver, XGpio* gpio, u16 deviceID,
		unsigned channelID) {

	XGpio_Config *cfg_ptr;

	// Initialize Button Device
	cfg_ptr = XGpio_LookupConfig(deviceID);
	XGpio_CfgInitialize(gpio, cfg_ptr, cfg_ptr->BaseAddress);

	// Set Button Tristate
	XGpio_SetDataDirection(gpio, channelID, SWITCHES_MASK);

	driver->channelID = channelID;
	driver->deviceID = deviceID;
	driver->gpio = gpio;
	driver->previousDelta = 0;
	driver->rawState = 0;
	driver->state = 0;
	driver->switchClockA = 0;
	driver->switchClockB = 0;
}


u32 SWITCHES_DRIVER_get_state(SwitchesDriver* driver) {
	return driver->state;
}

/**
 * Polls the current state of the buttons and determines which have changed.
 * This function should be called periodically by the application to poll the
 * pushbuttons.  It determines both the current debounced state of the buttons
 * and also which buttons have changed state since the last time the function
 * was called.
 *
 * In order for button debouncing to work properly, this function should be
 * called at a regular interval, even if the state of the buttons is not needed
 * that often.
 *
 * The value returned in driver->state will be a bit mask where a 1 indicates
 * the buttons is pressed.
 *
 * Returns the current debounced state of the buttons where a 1 in the
 * button ID's position indicates that the button is pressed and a 0
 * indicates that it is released.
 */

u32 SWITCHES_DRIVER_poll(SwitchesDriver* driver) {

	u32 delta;
	u32 data;

	// Read the raw state of the push buttons.
	data = XGpio_DiscreteRead(driver->gpio, driver->channelID);

	driver->rawState = (uint8_t) data;
	//
	// Determine the switches that are at a different state than the debounced
	// state.
	//
	delta = data ^ driver->state;

	//
	// Increment the clocks by one.
	//
	driver->switchClockA ^= driver->switchClockB;
	driver->switchClockB = ~driver->switchClockB;

	//
	// Reset the clocks corresponding to switches that have not changed state.
	//
	driver->switchClockA &= delta;
	driver->switchClockB &= delta;

	//
	// Get the new debounced switch state.
	//
	driver->state &= driver->switchClockA | driver->switchClockB;
	driver->state |= (~(driver->switchClockA | driver->switchClockB)) & data;

	//
	// Determine the switches that just changed debounced state.
	//
	delta ^= (driver->switchClockA | driver->switchClockB);

	//
	// Store the bit mask for the buttons that have changed for return to
	// caller.
	//
	driver->previousDelta = delta;

	//
	// Return the debounced buttons states to the caller.
	// '1' indicates the button is pressed
	//
	return driver->state;
}

u32 SWITCHES_DRIVER_poll_switch1(SwitchesDriver* driver) {
	return SWITCHES_DRIVER_poll(driver) & SWITCH1_MASK;
}

u32 SWITCHES_DRIVER_poll_switch2(SwitchesDriver* driver) {
	return SWITCHES_DRIVER_poll(driver) & SWITCH2_MASK;
}

u32 SWITCHES_DRIVER_poll_switch3(SwitchesDriver* driver) {
	return SWITCHES_DRIVER_poll(driver) & SWITCH3_MASK;
}

u32 SWITCHES_DRIVER_poll_switch4(SwitchesDriver* driver) {
	return SWITCHES_DRIVER_poll(driver) & SWITCH4_MASK;
}

