/************************************************************************/
/*                                                                      */
/*  emubot.c    --    emubot application                                */
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
/*                                                                      */
/* This file contains the emubot application                            */
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
/* When the command sequence completes, the Emubot will stop.           */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2022/07/10: (EAC) created                                         */
/*                                                                      */
/************************************************************************/
#include "emubot_app.h"

#include "sleep.h"

#define MAX_ACTIONS 16 * 3
static char bot_actions[MAX_ACTIONS];

/* Array containing a user defined font table. There can be up
 ** to 32 user defined characters. This table only defines 24
 ** of them.
 ** Each 8-byte row below defines one character glyph.
 */
uint8_t userFont[cbOledFontUser] = {
        0x00, 0x04, 0x06, 0x7F, 0x7F, 0x06, 0x04, 0x00, // Char 000 (.)
        0x00, 0x10, 0x30, 0x7F, 0x7F, 0x30, 0x10, 0x00, // Char 001 (.)
        0x08, 0x08, 0x08, 0x2A, 0x3E, 0x1C, 0x08, 0x00, // Char 002 (.)
        0x08, 0x1C, 0x3E, 0x2A, 0x08, 0x08, 0x08, 0x00, // Char 003 (.)
};



#define CHAR_FORWARD 0
#define CHAR_LEFT 3
#define CHAR_RIGHT 2

void emu_bot_application() {
    int actions_number = 0;
    BotDrivers botDrivers;
    xil_printf("EMU BOT application Started\n\r");

    BOT_init(&botDrivers);

    for (int ch = 0; ch < 0x04; ch++) {
        OLED_DefUserChar(&botDrivers.oled, ch, &userFont[ch * cbOledChar]);
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
            OLED_SetCursor(&botDrivers.oled, 0, 3);
            OLED_PutString(&botDrivers.oled, "STOP         RUN");
            OLED_SetCursor(&botDrivers.oled, 0, 0);

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
            for (int i = 0; i < actions_number; ++i) {
                if (BUTTONS_DRIVER_button1_pressed(&botDrivers.buttonsDriver)) {
                    actions_number = 0;
                    OLED_Clear(&botDrivers.oled);
                    OLED_SetCursor(&botDrivers.oled, 0, 3);
                    OLED_PutString(&botDrivers.oled, "     CANCEL");
                    sleep(1);
                    break;
                }
                if (bot_actions[i] == 'L') {
                    LEDS_DRIVER_set_led1_on(&botDrivers.ledsDriver);
                    OLED_PutChar(&botDrivers.oled, CHAR_LEFT);
                    //usleep(50000);
                    DRIVING_DRIVER_turn_left_degrees(&botDrivers.drivingDriver,
                            90);
                    LEDS_DRIVER_set_led1_off(&botDrivers.ledsDriver);
                }
                if (bot_actions[i] == 'R') {

                    LEDS_DRIVER_set_led3_on(&botDrivers.ledsDriver);
                    OLED_PutChar(&botDrivers.oled, CHAR_RIGHT);
                    //usleep(50000);
                    DRIVING_DRIVER_turn_right_degrees(&botDrivers.drivingDriver,
                            90);
                    LEDS_DRIVER_set_led3_off(&botDrivers.ledsDriver);
                }
                if (bot_actions[i] == 'F') {
                    LEDS_DRIVER_set_led2_on(&botDrivers.ledsDriver);
                    OLED_PutChar(&botDrivers.oled, CHAR_FORWARD);
                    //usleep(50000);
                    DRIVING_DRIVER_drive_forward_cm(&botDrivers.drivingDriver,
                            20);
                    LEDS_DRIVER_set_led2_off(&botDrivers.ledsDriver);
                }
            }
        }
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
