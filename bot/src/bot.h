/************************************************************************/
/*                                                                        */
/*    Bot.h    --    Bot header file                                          */
/*  This file is part of the Arty S7 Bot Library                        */
/*                                                                        */
/************************************************************************/
/*    Author:     Enrique Albertos                                        */
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

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/************************************************************************/
/*  Module Description:                                                 */
/*                                                                        */
/*    This header file contains definitions for initializing the bot.      */
/*                                                                        */
/************************************************************************/
/*  Revision History:                                                    */
/*                                                                        */
/*    2022/07/10: (EAC) created                                            */
/*                                                                        */
/************************************************************************/

#ifndef __BOT_H_
#define __BOT_H_

#include "bot_parameters.h"
#include "drivers/leds_driver.h"
#include "drivers/rgb_leds_driver.h"
#include "drivers/buttons_driver.h"
#include "drivers/distance_pid_control.h"
#include "drivers/driving_driver.h"
#include "drivers/switches_driver.h"
#include "drivers/pwm_driver.h"
#include "drivers/hbridge_driver.h"
#include "drivers/motor_position.h"
#include "drivers/speed_pid_control.h"
#include "drivers/oled_extensions.h"

typedef struct BotDrivers {
    ButtonsDriver buttonsDriver;
    SwitchesDriver switchesDriver;
    LedsDriver ledsDriver;
    RgbLedsDriver rgbLedsDriver;
    DrivingDriver drivingDriver;
    PmodOLED oled;
} BotDrivers;

void BOT_init(BotDrivers* BotDrivers);

void BOT_init_gpio_inputs(SwitchesDriver* switchesDriver,
        ButtonsDriver* buttonsDriver);

void BOT_init_gpio_outputs(RgbLedsDriver* rgbLedsDriver, LedsDriver* ledsDriver);

void BOT_init_driving_driver(DrivingDriver* drivingDriver);

void BOT_init_oled_display(PmodOLED *oled);

#endif // __BOT_H_
