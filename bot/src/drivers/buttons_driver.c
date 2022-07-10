/************************************************************************/
/*                                                                      */
/*  buttons_driver.c: Push Buttons driver for the Bot.                  */
/*  This file is part of the Arty S7 Bot.                               */
/*                                                                      */
/************************************************************************/
/*    Author:     Enrique Albertos                                      */
/************************************************************************/

/*
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 */

/************************************************************************/
/*  Module Description:                                                 */
/*                                                                      */
/* This driver controls the 4 push buttons                              */
/* for the Bot and holds the actual regs state                          */
/* BUTTONS_DRIVER_poll polls the four push buttons                       */
/* Polls the current state of the buttons and determines which have     */
/* changed. This function should be called periodically by the          */
/* application to poll the pushbuttons.  It determines both the current */
/* debounced state of the buttons and also which buttons have changed   */
/* state since the last time the function was called.                   */
/*                                                                      */
/* In order for button debouncing to work properly, this function       */
/* should be called at a regular interval, even if the state of the     */
/* buttons is not needed that often.                                    */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2022/07/06: (EAC) created                                         */
/*                                                                      */
/************************************************************************/

/****************** Include Files ********************/

#include "buttons_driver.h"

void BUTTONS_DRIVER_init(ButtonsDriver* driver, XGpio* gpio, u16 deviceID,
        unsigned channelID) {

    XGpio_Config *cfg_ptr;

    // Initialize Button Device
    cfg_ptr = XGpio_LookupConfig(deviceID);
    XGpio_CfgInitialize(gpio, cfg_ptr, cfg_ptr->BaseAddress);

    // Set Button Tristate
    XGpio_SetDataDirection(gpio, channelID, BUTTONS_MASK);

    driver->channelID = channelID;
    driver->deviceID = deviceID;
    driver->gpio = gpio;
    BUTTONS_DRIVER_reset(driver);
}

u32 BUTTONS_DRIVER_get_state(ButtonsDriver* driver) {
    return driver->state;
}

/**
 * u32 BUTTONS_DRIVER_poll(ButtonsDriver* driver)
 *
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
 *
 * @param driver    A buttonss driver configuation
 * @return          The actual register buttons state
 */
u32 BUTTONS_DRIVER_poll(ButtonsDriver* driver) {

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

/**
 * BUTTONS_DRIVER_button1_pressed(ButtonsDriver* driver)
 * Returns 1 if the current debounced state of the button 1 BTN0
 * is pressed and a 0 indicates that it is released.
 *
 * @param driver    A buttonss driver configuation
 * @return 1        If the actual debounced state of the button is pressed
 */
int BUTTONS_DRIVER_button1_pressed(ButtonsDriver* driver) {
    return (BUTTONS_DRIVER_poll(driver) & BUTTON1_MASK) > 0;
}

/**
 * BUTTONS_DRIVER_button2_pressed(ButtonsDriver* driver)
 * Returns 1 if the current debounced state of the button 2 BTN1
 * is pressed and a 0 indicates that it is released.
 *
 * @param driver    A buttonss driver configuation
 * @return 1        If the actual debounced state of the button is pressed
 */
int BUTTONS_DRIVER_button2_pressed(ButtonsDriver* driver) {
    return (BUTTONS_DRIVER_poll(driver) & BUTTON2_MASK) > 0;
}


/**
 * BUTTONS_DRIVER_button3_pressed(ButtonsDriver* driver)
 * Returns 1 if the current debounced state of the button 3 BTN2
 * is pressed and a 0 indicates that it is released.
 *
 * @param driver    A buttonss driver configuation
 * @return 1        If the actual debounced state of the button is pressed
 */
int BUTTONS_DRIVER_button3_pressed(ButtonsDriver* driver) {
    return (BUTTONS_DRIVER_poll(driver) & BUTTON3_MASK) > 0;
}

/**
 * BUTTONS_DRIVER_button4_pressed(ButtonsDriver* driver)
 * Returns 1 if the current debounced state of the button 3 BTN2
 * is pressed and a 0 indicates that it is released.
 *
 * @param driver    A buttonss driver configuation
 * @return 1        If the actual debounced state of the button is pressed
 */
int BUTTONS_DRIVER_button4_pressed(ButtonsDriver* driver) {
    return (BUTTONS_DRIVER_poll(driver) & BUTTON4_MASK) > 0;
}

/**
 * BUTTONS_DRIVER_reset(ButtonsDriver* driver)
 * Reset the buttons driver
 *
 * @param driver    A buttonss driver configuation
 */
void BUTTONS_DRIVER_reset(ButtonsDriver* driver) {
    driver->previousDelta = 0;
    driver->rawState = 0;
    driver->state = 0;
    driver->switchClockA = 0;
    driver->switchClockB = 0;
}

