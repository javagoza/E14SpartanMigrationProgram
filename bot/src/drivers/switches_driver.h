/*
 *
 * SWITCHES_DRIVER.h: Switches driver with debouncing for the Bot.
 *
 * This driver controls the 4 push buttons
 * for the Bot and holds the actual regs state
 *
 * Author: Enrique Albertos
 * Date: 2022/07/06
 *
 */

#ifndef SWITCHES_DRIVER_H
#define SWITCHES_DRIVER_H

/****************** Include Files ********************/

#include "xstatus.h"
#include "xgpio.h"

typedef struct SwitchesDriver {
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
} SwitchesDriver;

#define SWITCHES_MASK 0b1111

#define SWITCH1_MASK 0b0001
#define SWITCH2_MASK 0b0010
#define SWITCH3_MASK 0b0100
#define SWITCH4_MASK 0b1000

void SWITCHES_DRIVER_init(SwitchesDriver* driver, XGpio* gpio, u16 deviceID,
        unsigned channelID);

u32 SWITCHES_DRIVER_get_state(SwitchesDriver* driver);

u32 SWITCHES_DRIVER_poll(SwitchesDriver* driver);

u32 SWITCHES_DRIVER_poll_switch1(SwitchesDriver* driver);

u32 SWITCHES_DRIVER_poll_switch2(SwitchesDriver* driver);

u32 SWITCHES_DRIVER_poll_switch3(SwitchesDriver* driver);

u32 SWITCHES_DRIVER_poll_switch4(SwitchesDriver* driver);

#endif // SWITCHES_DRIVER_H
