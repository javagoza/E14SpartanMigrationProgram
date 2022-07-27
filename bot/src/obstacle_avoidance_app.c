/************************************************************************/
/*                                                                      */
/*  obstacle_avoidance_app.c    --    obstacle_avoidance application                                */
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
/* This file contains the obstacle_avoidance application                */
/* The robot always moves forward in a straight line using a PID        */
/* controller whose goal is to maintain the same distance traveled by   */
/* both wheels. If it detects an object by polling the sensor within    */
/* 15 cm, then it stops the robot, swings it on the right wheel ninety  */
/* degrees and continues forward again.                                 */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2022/07/24: (EAC) created                                         */
/*                                                                      */
/************************************************************************/
#include "obstacle_avoidance_app.h"

#include "sleep.h"

#define CHAR_FORWARD 0
#define CHAR_LEFT 3
#define CHAR_RIGHT 2

void obstacle_avoidance_application() {
    BotDrivers botDrivers;

    xil_printf("Obstacle Avoidance application Started\r\n");

    BOT_init(&botDrivers);
    sleep(1);
    PmodToF_Initialize();

    OLED_SetCharUpdate(&botDrivers.oled, 0);
    OLED_ClearBuffer(&botDrivers.oled);
    OLED_SetCursor(&botDrivers.oled, 0, 0);

    while (1) {
        if (SWITCHES_DRIVER_poll_switch4(&botDrivers.switchesDriver)) {
            OLED_SetCharUpdate(&botDrivers.oled, 1);
            OLED_Clear(&botDrivers.oled);
            OLED_SetCursor(&botDrivers.oled, 0, 3);
            OLED_PutString(&botDrivers.oled, "RUNNING");
            OLED_SetCursor(&botDrivers.oled, 0, 0);
            while (SWITCHES_DRIVER_poll_switch4(&botDrivers.switchesDriver)) {
                int canceled = DRIVING_DRIVER_drive_to_obstacle(&botDrivers.drivingDriver, 1.0, 15.0);
                if (canceled > 0 ) {
                      DRIVING_DRIVER_swing_turn_right_degrees(&botDrivers.drivingDriver, 90);
                }
                LEDS_DRIVER_set_led4_toggle(&botDrivers.ledsDriver);

            }
        }

        OLED_SetCharUpdate(&botDrivers.oled, 0);
        OLED_ClearBuffer(&botDrivers.oled);
        OLED_SetCursor(&botDrivers.oled, 0, 0);
        OLED_PutString(&botDrivers.oled, "    OBSTACLE");
        OLED_SetCursor(&botDrivers.oled, 0, 1);
        OLED_PutString(&botDrivers.oled, " AVOIDANCE v1.0");
        OLED_SetCursor(&botDrivers.oled, 0, 3);
        OLED_PutString(&botDrivers.oled, "SW3 -> RUN");
        OLED_Update(&botDrivers.oled);
        BUTTONS_DRIVER_reset(&botDrivers.buttonsDriver);
    }

}
