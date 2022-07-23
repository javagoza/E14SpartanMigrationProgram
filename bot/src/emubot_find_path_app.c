/************************************************************************/
/*                                                                      */
/*  emubot_find_path_app.c    --    emubot application                                */
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
/* This file contains the emubot find path application                  */
/* After initial powering, the sequence memory is cleared.              */
/* Toggle PROGRAM/RUN mode with toggle switch SW3.                      */
/* In PROGRAM mode the user can press a sequence of commands,           */
/* which are stored in the sequence memory.                             */
/* A maximum of 48 commands can be stored; each command being either    */
/* one forward movement or a left/right turn.                           */
/* User can delete commands with the DELETE button,                     */
/* BTN3 in PROGRAM MODE.                                                */
/* Each forward command causes the unit to move approx. 20cm forward.   */
/* Each turn command causes the unit to rotate 90 degres.               */
/* When the user finish programming the sequence the user can change    */
/* to RUN mode changing SW3 from PROG to RUN.                           */
/* The sequence will not start until the user press the RUN button,     */
/* BTN3 in RUN MODE.                                                    */
/* In all the modes the OLED display shows the function for each button.*/
/* When the RUN button is pressed, the Emubot will execute all the      */
/* commands stored in order.                                            */
/* After each forward command the color under the robot is checked.     */
/* Score starts with 1000 points. Each forward step substracts 50 points*/
/*  When the command sequence completes, the Emubot will stop,          */
/*  then show the score and the color squence detected                  */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2022/07/23: (EAC) created                                         */
/*                                                                      */
/************************************************************************/
#include "emubot_find_path_app.h"

#include "sleep.h"
#include "PmodCOLOR.h"

#define MAX_ACTIONS 16 * 3
static char bot_actions[MAX_ACTIONS];

/* Array containing a user defined font table. There can be up
 ** to 32 user defined characters. This table only defines 24
 ** of them.
 ** Each 8-byte row below defines one character glyph.
 */
uint8_t arrowsFont[cbOledFontUser] = { 0x00, 0x04, 0x06, 0x7F, 0x7F, 0x06, 0x04,
        0x00, // Char 000 (.)
        0x00, 0x10, 0x30, 0x7F, 0x7F, 0x30, 0x10, 0x00, // Char 001 (.)
        0x08, 0x08, 0x08, 0x2A, 0x3E, 0x1C, 0x08, 0x00, // Char 002 (.)
        0x08, 0x1C, 0x3E, 0x2A, 0x08, 0x08, 0x08, 0x00, // Char 003 (.)
        };

#define CHAR_FORWARD 0
#define CHAR_LEFT 3
#define CHAR_RIGHT 2
#define CELL_SIZE_WIDTH  18.0 // cell size width in cm
#define CELL_SIZE_HEIGTH 18.0 // cell size width in cm

typedef enum Orientation {
    VERTICAL = 0, HORIZONTAL = 1
} Orientation;
PmodCOLOR colorSensor;

void emu_bot_find_path_application() {
    COLOR_Data data;
    int score;

    Orientation orientation = VERTICAL;
    BotDrivers botDrivers;
    BOT_init(&botDrivers);

    int actions_number = 0;

    xil_printf("EMU BOT find path application Started\r\n");

    for (int ch = 0; ch < 0x04; ch++) {
        OLED_DefUserChar(&botDrivers.oled, ch, &arrowsFont[ch * cbOledChar]);
    }

    OLED_SetCharUpdate(&botDrivers.oled, 0);
    OLED_ClearBuffer(&botDrivers.oled);
    OLED_SetCursor(&botDrivers.oled, 0, 0);

    while (1) {
        if (SWITCHES_DRIVER_poll_switch4(&botDrivers.switchesDriver)) {
            OLED_SetCharUpdate(&botDrivers.oled, 1);
            OLED_Clear(&botDrivers.oled);
            OLED_SetCursor(&botDrivers.oled, 0, 3);
            OLED_PutString(&botDrivers.oled, "LFT FWD RGT DEL");
            OLED_SetCursor(&botDrivers.oled, 0, 0);
            while (SWITCHES_DRIVER_poll_switch4(&botDrivers.switchesDriver)
                    && actions_number < MAX_ACTIONS) {

                if (BUTTONS_DRIVER_button1_pressed(&botDrivers.buttonsDriver)) {
                    bot_actions[actions_number++] = 'L';
                    OLED_PutChar(&botDrivers.oled, CHAR_LEFT);
                }

                if (BUTTONS_DRIVER_button2_pressed(&botDrivers.buttonsDriver)) {
                    bot_actions[actions_number++] = 'F';
                    OLED_PutChar(&botDrivers.oled, CHAR_FORWARD);
                }

                if (BUTTONS_DRIVER_button3_pressed(&botDrivers.buttonsDriver)) {
                    bot_actions[actions_number++] = 'R';
                    OLED_PutChar(&botDrivers.oled, CHAR_RIGHT);
                }
                if (BUTTONS_DRIVER_button4_pressed(&botDrivers.buttonsDriver)) {
                    if (actions_number > 0) {
                        actions_number--;
                        OLED_PutChar(&botDrivers.oled, ' ');
                        OLED_RecedeCursor(&botDrivers.oled);
                        OLED_RecedeCursor(&botDrivers.oled);
                        OLED_PutChar(&botDrivers.oled, ' ');
                        OLED_RecedeCursor(&botDrivers.oled);
                    }
                }

                OLED_PutChar(&botDrivers.oled, '_');
                OLED_RecedeCursor(&botDrivers.oled);
                usleep(100000);

                OLED_PutChar(&botDrivers.oled, ' ');
                OLED_RecedeCursor(&botDrivers.oled);
                usleep(100000);

            }
        }
        if (actions_number > 0) {
            score = 1000;
            COLOR_SetLED(&botDrivers.color, 1);
            OLED_SetCursor(&botDrivers.oled, 0, 3);
            OLED_PutString(&botDrivers.oled, "STOP         RUN");
            OLED_SetCursor(&botDrivers.oled, 0, 0);
            // wait for run or stop button
            while (!((BUTTONS_DRIVER_poll(&botDrivers.buttonsDriver) & 0b1001)
                    > 0)) {
                usleep(1000);
            }
            OLED_SetCursor(&botDrivers.oled, 0, 3);
            OLED_PutString(&botDrivers.oled, "STOP            ");
            OLED_SetCursor(&botDrivers.oled, 0, 0);
            OLED_SetCursor(&botDrivers.oled, 0, 0);
            OLED_PutString(&botDrivers.oled, "                ");
            OLED_PutString(&botDrivers.oled, "                ");
            OLED_PutString(&botDrivers.oled, "                ");
            OLED_SetCursor(&botDrivers.oled, 0, 0);
            sleep(1);
            int colorSequence[3];
            int colorSequenceIndex = 0;
            for (int i = 0; i < actions_number; ++i) {
                if (BUTTONS_DRIVER_button1_pressed(&botDrivers.buttonsDriver)) {
                    // Cancel the sequence and exit
                    actions_number = 0;
                    OLED_Clear(&botDrivers.oled);
                    OLED_SetCursor(&botDrivers.oled, 0, 3);
                    OLED_PutString(&botDrivers.oled, "     CANCEL");
                    sleep(1);
                    break;
                }
                if (bot_actions[i] == 'L') {
                    // turn the robot to the left 90 degrees
                    LEDS_DRIVER_set_led1_on(&botDrivers.ledsDriver);
                    OLED_PutChar(&botDrivers.oled, CHAR_LEFT);
                    DRIVING_DRIVER_turn_left_degrees(&botDrivers.drivingDriver,
                            90);
                    LEDS_DRIVER_set_led1_off(&botDrivers.ledsDriver);
                    if (orientation == VERTICAL) {
                        orientation = HORIZONTAL;
                    } else {
                        orientation = VERTICAL;
                    }
                }
                if (bot_actions[i] == 'R') {
                    // turn the robot to the right 90 degrees
                    LEDS_DRIVER_set_led3_on(&botDrivers.ledsDriver);
                    OLED_PutChar(&botDrivers.oled, CHAR_RIGHT);
                    DRIVING_DRIVER_turn_right_degrees(&botDrivers.drivingDriver,
                            90);
                    LEDS_DRIVER_set_led3_off(&botDrivers.ledsDriver);
                    if (orientation == VERTICAL) {
                        orientation = HORIZONTAL;
                    } else {
                        orientation = VERTICAL;
                    }
                }
                if (bot_actions[i] == 'F') {
                    LEDS_DRIVER_set_led2_on(&botDrivers.ledsDriver);
                    OLED_PutChar(&botDrivers.oled, CHAR_FORWARD);
                    // move the robot forward if the cells are not square
                    // move depending on orientation
                    if (orientation == VERTICAL) {
                        DRIVING_DRIVER_drive_forward_cm(
                                &botDrivers.drivingDriver,
                                CELL_SIZE_HEIGTH);
                    } else {
                        DRIVING_DRIVER_drive_forward_cm(
                                &botDrivers.drivingDriver,
                                CELL_SIZE_WIDTH);
                    }
                    LEDS_DRIVER_set_led2_off(&botDrivers.ledsDriver);
                    // check color under the robot
                    usleep(500000);
                    data = COLOR_GetData(&botDrivers.color);
                    usleep(2400);
                    data = COLOR_GetData(&botDrivers.color);
                    usleep(2400);
                    // detect blue color
                    if (data.r >> 10 < data.b >> 10 && data.g >> 11 < data.b >> 11) {
                        if (colorSequenceIndex < 3){
                            colorSequence[colorSequenceIndex++] = COLOR_BLUE;
                        }
                        xil_printf("BLUE\r\n");
                        RGB_LEDS_DRIVER_set_rgb_led1_blue_on(&botDrivers.rgbLedsDriver);
                    }
                    // detect red color
                    if (data.r >> 11 > data.b >> 11 && data.r >> 11 > data.g >> 11) {
                        if (colorSequenceIndex < 3){
                            colorSequence[colorSequenceIndex++] = COLOR_RED;
                        }
                        xil_printf("RED\r\n");
                        RGB_LEDS_DRIVER_set_rgb_led2_red_on(&botDrivers.rgbLedsDriver);
                    }
                    // detect green color
                    if (data.g >> 11 > data.b >> 11 && data.g >> 11 > data.r >> 11) {
                        if (colorSequenceIndex < 3){
                            colorSequence[colorSequenceIndex++] = COLOR_GREEN;
                        }
                        xil_printf("GREEN\r\n");
                        RGB_LEDS_DRIVER_set_rgb_led1_green_on(&botDrivers.rgbLedsDriver);
                    }
                }
                score = score - 50;
            }

            // Display Score
            COLOR_SetLED(&botDrivers.color, 0);

            OLED_SetCursor(&botDrivers.oled, 0, 0);
            OLED_PutString(&botDrivers.oled, "SCORE: ");
            OLED_putIntVariable(&botDrivers.oled, score);
            BUTTONS_DRIVER_reset(&botDrivers.buttonsDriver);
            // Display registeres color sequence
            OLED_SetCursor(&botDrivers.oled, 0, 1);
            for (int i =0; i< colorSequenceIndex; i++ ){
                if(colorSequence[i] == COLOR_BLUE){
                    OLED_PutString(&botDrivers.oled, "BLUE ");
                }
                if(colorSequence[i] == COLOR_RED){
                    OLED_PutString(&botDrivers.oled, "RED ");
                }
                if(colorSequence[i] == COLOR_GREEN){
                    OLED_PutString(&botDrivers.oled, "GREEN ");
                }
            }
            // wait for button showing score
            while (!((BUTTONS_DRIVER_poll(&botDrivers.buttonsDriver) & 0b1001)
                    > 0)) {
                usleep(1000);
            }

        }
        RGB_LEDS_DRIVER_set_rgb_leds_state(&botDrivers.rgbLedsDriver, 0);

        OLED_SetCharUpdate(&botDrivers.oled, 0);
        OLED_ClearBuffer(&botDrivers.oled);
        OLED_SetCursor(&botDrivers.oled, 0, 0);
        OLED_PutString(&botDrivers.oled, "   EmuBot v1.0");
        OLED_SetCursor(&botDrivers.oled, 0, 2);
        OLED_PutString(&botDrivers.oled, "SW3 -> PROG MODE");
        OLED_Update(&botDrivers.oled);
        actions_number = 0;
        BUTTONS_DRIVER_reset(&botDrivers.buttonsDriver);
    }

}
