/************************************************************************/
/*                                                                      */
/*  buttons_driver.h: Header for Push Buttons driver for the Bot.       */
/*  This file is part of the Arty S7 Bot Library                        */
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
/* This header file contains definitions for the push buttons driver    */
/* for the Bot and holds the actual regs state                          */
/* Call BUTTONS_DRIVER_init to initialize the driver.                   */
/* Call BUTTONS_DRIVER_poll to poll the four push buttons for actual    */
/* state                                                                */
/* This function should be called periodically by the application       */
/* poll the pushbuttons.  It determines both the current debounced      */
/* state of the buttons and also which buttons have changed             */
/* state since the last time the function was called.                   */
/*                                                                      */
/* In order for button debouncing to work properly, this function       */
/* should be called at a regular interval, even if the state of the     */
/* buttons is not needed that often.                                    */
/* Call:                                                                */
/* BUTTONS_DRIVER_button1_pressed if only interested in button1 state   */
/* BUTTONS_DRIVER_button2_pressed if only interested in button2 state   */
/* BUTTONS_DRIVER_button3_pressed if only interested in button3 state   */
/* BUTTONS_DRIVER_button4_pressed if only interested in button4 state   */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2022/07/06: (EAC) created                                         */
/*                                                                      */
/************************************************************************/

#ifndef BUTTONS_DRIVER_H
#define BUTTONS_DRIVER_H

/****************** Include Files ********************/

#include "xstatus.h"
#include "xgpio.h"

typedef struct ButtonsDriver {
    XGpio* gpio;
    u32 deviceID;
    u32 channelID;
    u32 state; // Holds the current, debounced state of each button.  A 1 in a bit indicates
               // that that button is currently pressed, otherwise it is released.
    u32 switchClockA;
    u32 switchClockB;
    u32 rawState;
    u32 previousDelta;  // indicate* which button states changed since
                        // the last time poll function was called.
                        // This value is derived from the debounced state of the buttons.
} ButtonsDriver;

#define BUTTONS_MASK 0b1111

#define BUTTON1_MASK 0b0001
#define BUTTON2_MASK 0b0010
#define BUTTON3_MASK 0b0100
#define BUTTON4_MASK 0b1000

void BUTTONS_DRIVER_init(ButtonsDriver* driver, XGpio* gpio, u16 deviceID,
        unsigned channelID);

u32 BUTTONS_DRIVER_get_state(ButtonsDriver* driver);

u32 BUTTONS_DRIVER_poll(ButtonsDriver* driver);

int BUTTONS_DRIVER_button1_pressed(ButtonsDriver* driver);

int BUTTONS_DRIVER_button2_pressed(ButtonsDriver* driver);

int BUTTONS_DRIVER_button3_pressed(ButtonsDriver* driver);

int BUTTONS_DRIVER_button4_pressed(ButtonsDriver* driver);

void BUTTONS_DRIVER_reset(ButtonsDriver* driver);

#endif // BUTTONS_DRIVER_H
