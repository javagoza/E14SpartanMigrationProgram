/************************************************************************/
/*                                                                      */
/*  hbridge_driver.h: Hbridge driver for the Bot.       */
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
/* This header file contains definitions for the funtions of            */
/* the dual hbridge driver for the bot motors and holds the actual      */
/* regs state                                                           */
/*                                                                      */
/* This driver controls the 4 signals of the two motor H-bridge drivers */
/* for the Bot and holds the actual regs state                          */
/*   DIR_R, nSLEEP_R, DIR_L, nSLEEP_L                                   */
/*                                                                      */
/* Can control direction and nSleep signals for the two motors          */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2022/07/03: (EAC) created                                         */
/*                                                                      */
/************************************************************************/

#ifndef HBRIDGE_DRIVER_H
#define HBRIDGE_DRIVER_H

/****************** Include Files ********************/

#include "xstatus.h"
#include "xgpio.h"

typedef enum Directions {
    DIRECT_DIRECTION = 0, REVERSE_DIRECTION
} Directions;

typedef struct HBridgeDriver {
    XGpio* gpio;
    u32 deviceID;
    u32 channelID;
    u32 state; // holds the current value of the HBridge register
    Directions direction;
} HBridgeDriver;

#define RIGHT_MOTOR_DIR_MASK     0b0001
#define RIGHT_MOTOR_NSLEEP_MASK  0b0010 // RFU
#define LEFT_MOTOR_DIR_MASK      0b0100
#define LEFT_MOTOR_NSLEEP_MASK   0b1000 // RFU

void HBRIDGE_DRIVER_init(HBridgeDriver* driver, XGpio* gpio, u16 deviceID,
        unsigned channelID, u32 state, Directions direction);

void HBRIDGE_DRIVER_set_hbridge_state(HBridgeDriver* driver, u32 state);

u32 HBRIDGE_DRIVER_get_hbridge_state(HBridgeDriver* driver);

void HBRIDGE_DRIVER_left_motor_forward(HBridgeDriver* driver);

void HBRIDGE_DRIVER_right_motor_forward(HBridgeDriver* driver);

void HBRIDGE_DRIVER_left_motor_backward(HBridgeDriver* driver);

void HBRIDGE_DRIVER_right_motor_backward(HBridgeDriver* driver);

void HBRIDGE_DRIVER_left_motor_direction_toggle(HBridgeDriver* driver);

void HBRIDGE_DRIVER_right_motor_direction_toggle(HBridgeDriver* driver);

void HBRIDGE_DRIVER_set_direction(HBridgeDriver* driver, Directions direction);

#endif // HBRIDGE_DRIVER_H
