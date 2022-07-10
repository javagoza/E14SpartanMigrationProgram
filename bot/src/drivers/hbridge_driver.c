/************************************************************************/
/*                                                                      */
/*  hbridge_driver.c: Hbridge driver implementation for the Bot.        */
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
/* This file implements the functions of the dual hbridge driver for    */
/* the bot motors and holds the actual regs state                       */
/*                                                                      */
/* This driver controls the 4 signals of the two motor H-bridge drivers */
/* for the Bot and holds the actual regs state                          */
/*   DIR_R, nSLEEP_R, DIR_L, nSLEEP_L                                   */
/*                                                                      */
/* Can control direction and nSleep signals for the two motors          */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2022/07/03: (EAC) created                                         */
/*                                                                      */
/************************************************************************/


/****************** Include Files ********************/
#include "hbridge_driver.h"

/**
 * void HBRIDGE_DRIVER_init(HBridgeDriver* driver, XGpio* gpio, u16 deviceID,
        unsigned channelID, u32 state, Directions direction)
 *
 * @details Init driver register and set initial values
 *
 * @param   driver holds configuration and values
 * @param   gpio Gpio register
 * @param   deviceID XGpio Device ID for HBridge register
 * @param   channelID XGpio channel supporting the HBridge
 * @param   initial state of the hbridge 4 bits
 * @param   direction of movement of the vehicle with respect to the rotation of the motors
 * @return  None.
 *
 *
 */
void HBRIDGE_DRIVER_init(HBridgeDriver* driver, XGpio* gpio, u16 deviceID,
        unsigned channelID, u32 state, Directions direction) {

    driver->gpio = gpio;
    driver->channelID = channelID;
    driver->state = state;
    driver->direction = direction;

    XGpio_Config *cfg_ptr;

    // initialize hbridge device
    cfg_ptr = XGpio_LookupConfig(deviceID);
    XGpio_CfgInitialize(gpio, cfg_ptr, cfg_ptr->BaseAddress);

    // Set Led Tristate
    XGpio_SetDataDirection(gpio, channelID, 0);

    HBRIDGE_DRIVER_set_hbridge_state(driver, state);

}

/**
 * void HBRIDGE_DRIVER_set_direction(HBridgeDriver* driver, Directions direction)
 *
 * @details change both directions at the same time
 *
 * @param driver    HBridgeDriver with configuration and actual state
 * @param direction
 */
void HBRIDGE_DRIVER_set_direction(HBridgeDriver* driver, Directions direction) {
    driver->direction = direction;
}

/**
 * u32 HBRIDGE_DRIVER_get_hbridge_state(HBridgeDriver* driver)
 *
 * @details returns actual signals state, both direction and nSleep for the two motors
 *
 * @param driver    HBridgeDriver with configuration and actual state
 * @return
 */
u32 HBRIDGE_DRIVER_get_hbridge_state(HBridgeDriver* driver) {
    return driver->state;
}

/**
 * void HBRIDGE_DRIVER_set_hbridge_state(HBridgeDriver* driver, u32 state)
 *
 * @details set new hbridge state, changing  both direction and nSleep for the two motors
 *
 * @param driver    HBridgeDriver with configuration and actual state
 * @param state
 */
void HBRIDGE_DRIVER_set_hbridge_state(HBridgeDriver* driver, u32 state) {
    driver->state = state;
    XGpio_DiscreteWrite(driver->gpio, driver->channelID, driver->state);

}

///////////// HIGH SIGNALS ////////////////////

/**
 * void HBRIDGE_DRIVER_left_motor_backward(HBridgeDriver* driver)
 *
 * @details set left motor direction signal to high, backward direction
 *
 * @param driver    HBridgeDriver with configuration and actual state
 */
void HBRIDGE_DRIVER_left_motor_backward(HBridgeDriver* driver) {
    if (driver->direction == DIRECT_DIRECTION) {
        driver->state |= LEFT_MOTOR_DIR_MASK;
        XGpio_DiscreteWrite(driver->gpio, driver->channelID, driver->state);
    } else {
        driver->state &= ~ LEFT_MOTOR_DIR_MASK;
        XGpio_DiscreteWrite(driver->gpio, driver->channelID, driver->state);
    }
}

/**
 * void HBRIDGE_DRIVER_right_motor_backward(HBridgeDriver* driver)
 *
 * @details  set right motor direction signal to high, backward direction
 *
 * @param driver    HBridgeDriver with configuration and actual state
 */
void HBRIDGE_DRIVER_right_motor_backward(HBridgeDriver* driver) {
    if (driver->direction == DIRECT_DIRECTION) {
        driver->state |= RIGHT_MOTOR_DIR_MASK;
        XGpio_DiscreteWrite(driver->gpio, driver->channelID, driver->state);
    } else {
        driver->state &= ~ RIGHT_MOTOR_DIR_MASK;
        XGpio_DiscreteWrite(driver->gpio, driver->channelID, driver->state);
    }
}

///////////// LOW SiGNALS ////////////////////

/**
 * void HBRIDGE_DRIVER_left_motor_forward(HBridgeDriver* driver)
 *
 * @details set left motor direction signal to low, forward direction
 *
 * @param driver    HBridgeDriver with configuration and actual state
 */
void HBRIDGE_DRIVER_left_motor_forward(HBridgeDriver* driver) {
    if (driver->direction == DIRECT_DIRECTION) {
        driver->state &= ~ LEFT_MOTOR_DIR_MASK;
        XGpio_DiscreteWrite(driver->gpio, driver->channelID, driver->state);
    } else {
        driver->state |= LEFT_MOTOR_DIR_MASK;
        XGpio_DiscreteWrite(driver->gpio, driver->channelID, driver->state);
    }
}

/**
 * void HBRIDGE_DRIVER_right_motor_forward(HBridgeDriver* driver)
 *
 * @details set right motor direction signal to low, forward direction
 *
 * @param driver    HBridgeDriver with configuration and actual state
 */
void HBRIDGE_DRIVER_right_motor_forward(HBridgeDriver* driver) {
    if (driver->direction == DIRECT_DIRECTION) {
        driver->state &= ~ RIGHT_MOTOR_DIR_MASK;
        XGpio_DiscreteWrite(driver->gpio, driver->channelID, driver->state);
    } else {
        driver->state |= RIGHT_MOTOR_DIR_MASK;
        XGpio_DiscreteWrite(driver->gpio, driver->channelID, driver->state);
    }
}

///////////// TOGGLE DIRECTIONS ////////////////////


/**
 * void HBRIDGE_DRIVER_left_motor_direction_toggle
 *
 * @details  toggles left motor direction
 *
 * @param driver    HBridgeDriver with configuration and actual state
 */
void HBRIDGE_DRIVER_left_motor_direction_toggle(HBridgeDriver* driver) {
    driver->state ^= LEFT_MOTOR_DIR_MASK;
    XGpio_DiscreteWrite(driver->gpio, driver->channelID, driver->state);
}

/**
 * void HBRIDGE_DRIVER_right_motor_direction_toggle
 *
 * @details   toggles right motor direction
 *
 * @param driver    HBridgeDriver with configuration and actual state
 */
void HBRIDGE_DRIVER_right_motor_direction_toggle(HBridgeDriver* driver) {
    driver->state ^= RIGHT_MOTOR_DIR_MASK;
    XGpio_DiscreteWrite(driver->gpio, driver->channelID, driver->state);
}

