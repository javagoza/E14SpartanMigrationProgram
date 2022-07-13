/************************************************************************/
/*                                                                      */
/*  bot_parameters.h    --  Parameters to configure the Bot             */
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
/*  This file contains the configuration of the Bot:                    */
/*                                                                      */
/*  - AXI GPIO Address and Channel for the 2 board RGB LEDS             */
/*  - AXI GPIO Address and Channel for the 4 board red LEDS             */
/*  - AXI GPIO Address and Channel for the 4 board switches             */
/*  - AXI GPIO Address and Channel for the 4 board push buttons         */
/*  - AXI GPIO Address and Channel for the 4                            */
/*      dual hbridge control signals                                    */
/*  - Right Motor Timer /Counter Device ID                              */
/*  - Left Motor Timer /Counter Device ID                               */
/*  - Motor Speed PWM Control signal period in nanoseconds              */
/*  - Motor position feedback AXI Base Address                          */
/*  - Gear box motor ratio                                              */
/*  - Motor Position Module Clock Frequency                             */
/*  - OLED AXI LITE GPIO Base Address                                   */
/*  - OLED AXI LITE SPI Base Address                                    */
/*  - OLED white background or black background                         */
/*  - Proportional Constant for the Rotational Motor Speed PID control  */
/*  - Integral factor for the Rotational Motor Speed PID control        */
/*  - Derivative factor  for the Rotational Motor Speed PID control     */
/*  - Proportional factor for the Traveled Distance PID control         */
/*  - Integral factor for the Traveled Distance PID control             */
/*  - Derivative factor for the Traveled Distance PID control           */
/*  - Base duty cycle, base speed for the PID controllers               */
/*  - Distance correction factor when rotating                          */
/*  - Distance correction factors when driving straight                 */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2022/07/10: (EAC) created                                         */
/*                                                                      */
/************************************************************************/

#ifndef __BOT_PARAMETERS_H_
#define __BOT_PARAMETERS_H_

#include "xparameters.h"

// AXI GPIO Address and Channel for the 2 board RGB LEDS
#define BOT_LEDS_RGBLEDS_DEVICE_ID      XPAR_AXI_GPIO_0_DEVICE_ID
// AXI GPIO Channel for the 2 board RGB LEDS
#define BOT_RGB_LED_CHANNEL             1
// AXI GPIO Channel for the 4 board red LEDS
#define BOT_LED_CHANNEL                 2

// AXI GPIO Address and Channel for the 4 switches
#define BOT_BUTTONS_SWITCHES_DEVICE_ID  XPAR_AXI_GPIO_1_DEVICE_ID
// AXI GPIO Address and Channel for the 4 board push buttons
#define BOT_BUTTONS_CHANNEL             1
// AXI GPIO Address and Channel for the 4 board switches
#define BOT_SWITCHES_CHANNEL            2

// AXI GPIO Address and Channel for the 4 dual hbridge control signals
#define BOT_HBRIDGE_DEVICE_ID           XPAR_AXI_GPIO_HBRIDGE_DEVICE_ID
// AXI GPIO Address and Channel for the 4 dual hbridge control signals
#define BOT_HBRIDGE_CHANNEL             1

// Right Motor Timer /Counter Device ID
#define BOT_RIGHTMOTORTMRCTR_DEVICE_ID  XPAR_TMRCTR_0_DEVICE_ID
// Left Motor Timer /Counter Device ID
#define BOT_LEFTMOTORTMRCTR_DEVICE_ID   XPAR_TMRCTR_1_DEVICE_ID

// Motor Speed PWM Control signal period in nanoseconds
#define BOT_PWM_PERIOD                  10000000     /* PWM period in ns (100 Hz) */

// Motor position feedback AXI Base Address
#define BOT_MOTORPOSITION_BASEADDR      XPAR_MOTORPOSITION_0_S00_AXI_BASEADDR

// Number of raising edges per revolution depends on the sensor
#define BOT_MOTORPOSITION_EDGES_PER_REVOLUTION 3      // magnetic disk with three north-south pairs
// Gear box motor ratio
#define BOT_MOTORPOSITION_GEAR_BOX_RATIO       120    // refer to you gear motor spec

// Motor Position Module Clock Frequency
#define BOT_CORE_CLOCK_FREQ_HZ          XPAR_CPU_CORE_CLOCK_FREQ_HZ

// OLED AXI LITE GPIO Base Address
#define BOT_OLED_GPIO_BASEADDR          XPAR_PMODOLED_0_AXI_LITE_GPIO_BASEADDR
// OLED AXI LITE SPI Base Address
#define BOT_OLED_SPI_BASEADDR            XPAR_PMODOLED_0_AXI_LITE_SPI_BASEADDR
// OLED orientation
#define BOT_OLED_ORIENTATION            (u8) 0x1 // Set up for Normal PmodOLED(false) vs normal Onboard OLED(true)
// OLED white background or black background
#define BOT_OLED_INVERT                 (u8) 0x0 // true = white background/black letters false = black background /white letters

// Proportional factor for the Rotational Motor Speed PID control
#define BOT_SPEED_PID_K_PROPORTIONAL    0.005
// Integral factor for the Rotational Motor Speed PID control
#define BOT_SPEED_PID_K_INTEGRAL        0.0005
// Derivative factor for the Rotational Motor Speed PID control
#define BOT_SPEED_PID_K_DERIVATIVE      0.00005

// Proportional factor for the Traveled Distance PID control
#define BOT_DISTANCE_PID_K_PROPORTIONAL 0.015
// Integral factor for the Traveled Distance PID control
#define BOT_DISTANCE_PID_K_INTEGRAL     0.001
// Derivative factor for the Traveled Distance PID control
#define BOT_DISTANCE_PID_K_DERIVATIVE   0.001

// Base duty cycle, base speed for the PID controllers
#define BOT_BASE_DUTY_CYCLE             0.4

// Distance correction factor when rotating
#define BOT_DISTANCE_ARC_CORRECTION     15.2
// Distance correction factors when driving straight
#define BOT_DISTANCE_CORRECTION         15.5

#endif // __BOT_PARAMETERS_H_
