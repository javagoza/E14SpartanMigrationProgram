/*
 *
 * hbridge_driver.h: Hbridge driver for the Bot.
 *
 * This driver controls the 4 signals of the two motor H-bridge drivers
 * for the Bot and holds the actual regs state
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */

#ifndef HBRIDGE_DRIVER_H
#define HBRIDGE_DRIVER_H

/****************** Include Files ********************/

#include "xstatus.h"
#include "xgpio.h"

typedef struct HBridgeDriver {
	XGpio* gpio;
	u32 deviceID;
	u32 channelID;
	u32 state; // holds the current value of the HBridge register
} HBridgeDriver;


#define RIGHT_MOTOR_DIR_MASK     0b0001
#define RIGHT_MOTOR_NSLEEP_MASK  0b0010 // RFU
#define LEFT_MOTOR_DIR_MASK      0b0100
#define LEFT_MOTOR_NSLEEP_MASK   0b1000 // RFU


void HBRIDGE_DRIVER_init(HBridgeDriver* driver, XGpio* gpio, u16 deviceID, unsigned channelID, u32 state);

void HBRIDGE_DRIVER_set_hbridge_state(HBridgeDriver* driver, u32 state);


u32 HBRIDGE_DRIVER_get_hbridge_state(HBridgeDriver* driver);


void HBRIDGE_DRIVER_left_motor_forward(HBridgeDriver* driver);

void HBRIDGE_DRIVER_right_motor_forward(HBridgeDriver* driver);

void HBRIDGE_DRIVER_left_motor_backward(HBridgeDriver* driver);

void HBRIDGE_DRIVER_right_motor_backward(HBridgeDriver* driver);

void HBRIDGE_DRIVER_left_motor_direction_toggle(HBridgeDriver* driver);

void HBRIDGE_DRIVER_right_motor_direction_toggle(HBridgeDriver* driver);

#endif // HBRIDGE_DRIVER_H
