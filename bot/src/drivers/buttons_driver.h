/*
 *
 * buttons_driver.h: Push Buttons driver with debouncing function for the Bot.
 *
 * This driver controls the 4 push buttons
 * for the Bot and holds the actual regs state
 *
 * Author: Enrique Albertos
 * Date: 2022/07/06
 *
 */

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

u32 BUTTONS_DRIVER_poll_button1(ButtonsDriver* driver);

u32 BUTTONS_DRIVER_poll_button2(ButtonsDriver* driver);

u32 BUTTONS_DRIVER_poll_button3(ButtonsDriver* driver);

u32 BUTTONS_DRIVER_poll_button4(ButtonsDriver* driver);

#endif // BUTTONS_DRIVER_H
