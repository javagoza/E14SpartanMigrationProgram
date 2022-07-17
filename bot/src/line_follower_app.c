/************************************************************************/
/*                                                                      */
/*  line_follower_app.c    --  simple line follower app for the emubot  */
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

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
/************************************************************************/
/*  Module Description:                                                 */
/*    simple line follower app for the emubot uses a PmodCOLOR module   */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2022/07/16: (EAC) created                                         */
/*                                                                      */
/************************************************************************/

#include "line_follower_app.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct {
    COLOR_Data min, max;
} CalibrationData;

CalibrationData DemoInitCalibrationData(COLOR_Data firstSample);
void DemoCalibrate(COLOR_Data newSample, CalibrationData *calib);
u16 getThreshold(BotDrivers* botDrivers, int increments);
void rotate15degreesLeft(BotDrivers* botDrivers);
void searchTheLine(BotDrivers* botDrivers, u16 threshold, int increments);



void line_follower_application() {

    BotDrivers botDrivers;
    BOT_init(&botDrivers);

    COLOR_GetID(botDrivers.drivingDriver.colorSensor);

    int increments = 1;
    while (1) {
        OLED_SetCharUpdate(&botDrivers.oled, 1);
        OLED_ClearBuffer(&botDrivers.oled);
        OLED_SetCursor(&botDrivers.oled, 0, 0);
        OLED_PutString(&botDrivers.oled, " BTN1 CALIBRATE");

        while (!BUTTONS_DRIVER_button1_pressed(&botDrivers.buttonsDriver)) {
        } // wait for push button 1
        BUTTONS_DRIVER_reset(&botDrivers.buttonsDriver);
        OLED_ClearBuffer(&botDrivers.oled);
        OLED_SetCursor(&botDrivers.oled, 0, 0);
        OLED_PutString(&botDrivers.oled, " CALIBRATING...");

        rotate15degreesLeft(&botDrivers);

        u16 threshold = getThreshold(&botDrivers, increments);
        OLED_ClearBuffer(&botDrivers.oled);
        OLED_SetCursor(&botDrivers.oled, 0, 0);
        OLED_PutString(&botDrivers.oled, "   CALIBRATED");
        OLED_SetCursor(&botDrivers.oled, 0, 1);
        OLED_PutString(&botDrivers.oled, "TH: ");
        OLED_putIntVariable(&botDrivers.oled, threshold);
        sleep(2);
        COLOR_SetLED(botDrivers.drivingDriver.colorSensor, 1);
        usleep(120000);

        searchTheLine(&botDrivers, threshold, increments);

        OLED_ClearBuffer(&botDrivers.oled);
        OLED_SetCursor(&botDrivers.oled, 0, 0);
        OLED_PutString(&botDrivers.oled, " BTN2 RUN");
        while (!BUTTONS_DRIVER_button2_pressed(&botDrivers.buttonsDriver)) {
        } // wait for button 2

        if (!SWITCHES_DRIVER_poll_switch2(&botDrivers.switchesDriver)) {
            OLED_PutString(&botDrivers.oled, " SW2 TO START  ");
        }
        while (!SWITCHES_DRIVER_poll_switch2(&botDrivers.switchesDriver)) {
        } // wait for swicth 2

        OLED_ClearBuffer(&botDrivers.oled);
        OLED_SetCursor(&botDrivers.oled, 0, 0);
        OLED_PutString(&botDrivers.oled, "     RUNNING");

        OLED_SetCursor(&botDrivers.oled, 0, 1);
        OLED_PutString(&botDrivers.oled, "  SW2 TO STOP   ");

        COLOR_SetLED(botDrivers.drivingDriver.colorSensor, 1);
        usleep(12000);

        LIGHT_PID_CONTROLLER_init(botDrivers.drivingDriver.lightPIDController,
        BOT_LIGHT_PID_K_PROPORTIONAL,
        BOT_LIGHT_PID_K_INTEGRAL,
        BOT_LIGHT_PID_K_DERIVATIVE, .10);

        while (SWITCHES_DRIVER_poll_switch2(&botDrivers.switchesDriver)) {
            DRIVING_DRIVER_drive_forward_continuous_light(
                    &botDrivers.drivingDriver, .2, threshold);
        }

        DRIVING_DRIVER_delay_until_stop(&botDrivers.drivingDriver);

        COLOR_SetLED(botDrivers.drivingDriver.colorSensor, 0);
    }

}

CalibrationData DemoInitCalibrationData(COLOR_Data firstSample) {
    CalibrationData calib;
    calib.min = firstSample;
    calib.max = firstSample;
    return calib;
}

void DemoCalibrate(COLOR_Data newSample, CalibrationData *calib) {
    if (newSample.c < calib->min.c)
        calib->min.c = newSample.c;
    if (newSample.r < calib->min.r)
        calib->min.r = newSample.r;
    if (newSample.g < calib->min.g)
        calib->min.g = newSample.g;
    if (newSample.b < calib->min.b)
        calib->min.b = newSample.b;

    if (newSample.c > calib->max.c)
        calib->max.c = newSample.c;
    if (newSample.r > calib->max.r)
        calib->max.r = newSample.r;
    if (newSample.g > calib->max.g)
        calib->max.g = newSample.g;
    if (newSample.b > calib->max.b)
        calib->max.b = newSample.b;
}

void rotate15degreesLeft(BotDrivers* botDrivers) {
    COLOR_SetLED(botDrivers->drivingDriver.colorSensor, 1);
    sleep(1);
    DRIVING_DRIVER_turn_left_degrees(&botDrivers->drivingDriver, 15);
}

u16 getThreshold(BotDrivers* botDrivers, int increments) {
    CalibrationData calib;

    u16 prevLight = DRIVING_DRIVER_light(
            COLOR_GetData(botDrivers->drivingDriver.colorSensor));

    int32_t maxLightDelta = 0;
    u16 actualLight = 0;
    u16 thresholdMin = 0;
    u16 thresholdMax = 0;

    COLOR_Data data;
// go 30 degrees to the right
    for (int degrees = 0; degrees < 30 / increments; degrees += increments) {
        DRIVING_DRIVER_turn_right_continuous_degrees(&botDrivers->drivingDriver,
                increments);

        usleep(120000);
        data = COLOR_GetData(botDrivers->drivingDriver.colorSensor);
        actualLight = DRIVING_DRIVER_light(data);
        if (abs(actualLight - prevLight) > maxLightDelta) {
            thresholdMin = MIN(actualLight, prevLight);
            thresholdMax = MAX(actualLight, prevLight);
            maxLightDelta = thresholdMax - thresholdMin;
        }
        DemoCalibrate(data, &calib);
    }

    COLOR_SetLED(botDrivers->drivingDriver.colorSensor, 0);
    DRIVING_DRIVER_delay_until_stop(&botDrivers->drivingDriver);

    u16 threshold = thresholdMin + (thresholdMax - thresholdMin) / 8;
    return threshold;
}

void searchTheLine(BotDrivers* botDrivers, u16 threshold, int increments) {
    COLOR_Data data;
    data = COLOR_GetData(botDrivers->drivingDriver.colorSensor);
    OLED_SetCharUpdate(&botDrivers->oled, 0);

    while (DRIVING_DRIVER_light(data) > threshold) {
        DRIVING_DRIVER_turn_left_continuous_degrees(&botDrivers->drivingDriver,
                increments);

        OLED_ClearBuffer(&botDrivers->oled);

        OLED_SetCursor(&botDrivers->oled, 0, 0);
        OLED_PutString(&botDrivers->oled, "DAT:");
        OLED_putIntVariable(&botDrivers->oled,
                (int32_t) DRIVING_DRIVER_light(data));

        OLED_SetCursor(&botDrivers->oled, 0, 3);
        OLED_PutString(&botDrivers->oled, "THD:");
        OLED_putIntVariable(&botDrivers->oled, (int32_t) threshold);

        OLED_Update(&botDrivers->oled);
        usleep(120000);
        data = COLOR_GetData(botDrivers->drivingDriver.colorSensor);
    }
    COLOR_SetLED(botDrivers->drivingDriver.colorSensor, 0);
}
