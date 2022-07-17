/*
 * test.c: simple test application for the Bot Drivers
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 */

/*
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"

#include "bot.h"
#include "emubot_app.h"
#include "line_follower_app.h"

typedef enum Tests {
    TEST_LEDS = 0,
    TEST_LEDS_BUTTONS,
    TEST_MOTOR_POSITION_OLED,
    TEST_PWM_HBRIDGE,
    TEST_BOT,
    EMUBOT_APP,
    LINE_FOLLOWER
} Tests;

Tests testCase = EMUBOT_APP;

void test_leds();
void test_leds_and_buttons();
void test_motor_position_oled();
void test_motor_pwm_hbridge();
void test_motor_pwm_hbridge();
void test_bot();

void oled_display_motor_position(PmodOLED* myOled, int speeds[2],
        int16_t positions[2]);

void oled_show_action(PmodOLED* oled, char* text);
void oled_show_home(PmodOLED* oled, char* text);

void play_button_1_show(DrivingDriver* botDriver, PmodOLED* oled);
void play_button_2_show(DrivingDriver* botDriver, PmodOLED* oled);
void play_button_3_show(DrivingDriver* botDriver, PmodOLED* oled);


int main() {
    init_platform();

    switch (testCase) {
    case TEST_LEDS:
        test_leds();
        break;
    case TEST_LEDS_BUTTONS:
        test_leds_and_buttons();
        break;
    case TEST_MOTOR_POSITION_OLED:
        test_motor_position_oled();
        break;
    case TEST_PWM_HBRIDGE:
        test_motor_pwm_hbridge();
        break;
    case TEST_BOT:
        test_leds();
        test_bot();
        break;
    case EMUBOT_APP:
        test_leds();
        emu_bot_application();
        break;
    case LINE_FOLLOWER:
        test_leds();
        line_follower_application();
        break;
    default:
        break;
    }

    cleanup_platform();
    return 0;
}

void test_bot() {
    BotDrivers botDrivers;
    xil_printf("BOT TEST Started\n\r");

    BOT_init(&botDrivers);

    while (1) {
        LEDS_DRIVER_set_led1_toggle(&botDrivers.ledsDriver);
        oled_show_home(&botDrivers.oled, "  PRESS BUTTON");
        if (BUTTONS_DRIVER_button1_pressed(&botDrivers.buttonsDriver)) {
            play_button_1_show(&botDrivers.drivingDriver, &botDrivers.oled);
            BUTTONS_DRIVER_reset(&botDrivers.buttonsDriver);
        }
        if (BUTTONS_DRIVER_button2_pressed(&botDrivers.buttonsDriver)) {
            play_button_2_show(&botDrivers.drivingDriver, &botDrivers.oled);
            BUTTONS_DRIVER_reset(&botDrivers.buttonsDriver);
        }
        if (BUTTONS_DRIVER_button3_pressed(&botDrivers.buttonsDriver)) {
            play_button_3_show(&botDrivers.drivingDriver, &botDrivers.oled);
            BUTTONS_DRIVER_reset(&botDrivers.buttonsDriver);
        }
        if (BUTTONS_DRIVER_button4_pressed(&botDrivers.buttonsDriver)) {
            emu_bot_application();
            BUTTONS_DRIVER_reset(&botDrivers.buttonsDriver);
        }
        usleep(5000);
    }
    DRIVING_DRIVER_end(&botDrivers.drivingDriver);
    OLED_End(&botDrivers.oled);
    xil_printf("BOT TEST Finished\n\r");
    OLED_End(&botDrivers.oled);

}

void oled_show_home(PmodOLED* oled, char* text) {
    OLED_SetCharUpdate(oled, 0);
    OLED_ClearBuffer(oled);
    OLED_SetCursor(oled, 0, 0);
    OLED_PutString(oled, text);
    OLED_SetCursor(oled, 0, 2);
    OLED_PutString(oled, "  ARTYBOT-2022  ");
    OLED_SetCursor(oled, 0, 3);
    OLED_PutString(oled, " e14 / community");
    OLED_Update(oled);
}

void oled_show_action(PmodOLED* oled, char* text) {
    OLED_SetCharUpdate(oled, 0);
    OLED_ClearBuffer(oled);
    OLED_SetCursor(oled, 0, 0);
    OLED_PutString(oled, text);
    OLED_Update(oled);
}

void test_leds() {

    LedsDriver ledsDriver;
    RgbLedsDriver rgbLedsDriver;

    XGpio GpioLedsAndRgbLeds; /* Dual GPIO Driver for LEDs and RGB LEDS*/

    unsigned long semiperiod = 60000;
    xil_printf("LED TEST Started\n\r");

    LEDS_DRIVER_init(&ledsDriver, &GpioLedsAndRgbLeds,
    BOT_LEDS_RGBLEDS_DEVICE_ID,
    BOT_LED_CHANNEL, 0b0000);

    RGB_LEDS_DRIVER_init(&rgbLedsDriver, &GpioLedsAndRgbLeds,
    BOT_LEDS_RGBLEDS_DEVICE_ID, BOT_RGB_LED_CHANNEL, 0b000000);

    LEDS_DRIVER_set_led1_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led1_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led2_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led2_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led3_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led3_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led4_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led4_toggle(&ledsDriver);
    usleep(semiperiod);

    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_RED);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_GREEN);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_BLUE);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_CYAN);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_MAGENTA);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_MAGENTA);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_YELLOW);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_WHITE);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_BLACK);
    usleep(semiperiod);

    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_RED);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_GREEN);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_BLUE);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_CYAN);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_MAGENTA);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_MAGENTA);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_YELLOW);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_WHITE);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_BLACK);
    usleep(semiperiod);

    xil_printf("LED TEST Finished\n\r");
}

void play_button_1_show(DrivingDriver* botDriver, PmodOLED* oled) {
    xil_printf("BTN0 pressed\n\r");
    sleep(1);
    oled_show_action(oled, " FORWARD 10 cm");
    DRIVING_DRIVER_drive_forward_cm(botDriver, 10);
    sleep(1);
    oled_show_action(oled, " BACKWARD 10 cm");
    DRIVING_DRIVER_drive_backward_cm(botDriver, 10);
    sleep(1);
    oled_show_action(oled, " FORWARD 20 cm");
    DRIVING_DRIVER_drive_forward_cm(botDriver, 20);
    sleep(1);
    oled_show_action(oled, " TURN LEFT 180");
    DRIVING_DRIVER_turn_left_degrees(botDriver, 180);
    sleep(1);
    oled_show_action(oled, " FORWARD 20 cm");
    DRIVING_DRIVER_drive_forward_cm(botDriver, 20);
    sleep(1);
    oled_show_action(oled, " TURN RIGHT 180");
    DRIVING_DRIVER_turn_right_degrees(botDriver, 180);
    sleep(1);
}

void play_button_2_show(DrivingDriver* botDriver, PmodOLED* oled) {
    xil_printf("BTN1 pressed\n\r");
    sleep(1);
    DRIVING_DRIVER_set_speed(botDriver, .5);
    oled_show_action(oled, "TURN LEFT 360       SPEED 50");
    DRIVING_DRIVER_turn_left_degrees(botDriver, 360);
    sleep(1);
    oled_show_action(oled, "TURN RIGHT 360      SPEED 50");
    DRIVING_DRIVER_turn_right_degrees(botDriver, 360);
    sleep(1);
    DRIVING_DRIVER_set_speed(botDriver, BOT_BASE_DUTY_CYCLE);
}

void play_button_3_show(DrivingDriver* botDriver, PmodOLED* oled) {
    xil_printf("BTN2 pressed\n\r");
    sleep(1);
    oled_show_action(oled, "  SWING LEFT 90");
    DRIVING_DRIVER_swing_turn_left_degrees(botDriver, 90);
    usleep(50000);
    oled_show_action(oled, "  FORWARD 7 cm");
    DRIVING_DRIVER_drive_forward_cm(botDriver, 7);
    usleep(50000);
    oled_show_action(oled, "  TURN RIGHT 90");
    DRIVING_DRIVER_turn_right_degrees(botDriver, 90);
    usleep(50000);
    oled_show_action(oled, "  FORWARD 7 cm");
    DRIVING_DRIVER_drive_forward_cm(botDriver, 7);
    usleep(50000);
    sleep(1);
    oled_show_action(oled, "  TURN RIGHT 90");
    DRIVING_DRIVER_swing_turn_right_degrees(botDriver, 90);
    usleep(50000);
    oled_show_action(oled, "  FORWARD 7 cm");
    DRIVING_DRIVER_drive_forward_cm(botDriver, 7);
    usleep(50000);
    oled_show_action(oled, "TURN LEFT 90");
    DRIVING_DRIVER_turn_left_degrees(botDriver, 90);
    usleep(50000);
    oled_show_action(oled, "  FORWARD 7 cm");
    DRIVING_DRIVER_drive_forward_cm(botDriver, 7);
    usleep(50000);
    sleep(1);
}

void test_leds_and_buttons() {
    LedsDriver ledsDriver;
    RgbLedsDriver rgbLedsDriver;
    ButtonsDriver buttonsDriver;
    SwitchesDriver switchesDriver;

    XGpio GpioLedsAndRgbLeds; /* Dual GPIO Driver for LEDs and RGB LEDS*/
    XGpio GpioSwitchsAndButtons; /* Dual GPIO Driver for Switchs and Push Buttons */

    LEDS_DRIVER_init(&ledsDriver, &GpioLedsAndRgbLeds,
    BOT_LEDS_RGBLEDS_DEVICE_ID,
    BOT_LED_CHANNEL, 0b0000);

    RGB_LEDS_DRIVER_init(&rgbLedsDriver, &GpioLedsAndRgbLeds,
    BOT_LEDS_RGBLEDS_DEVICE_ID,
    BOT_RGB_LED_CHANNEL, 0b000000);

    BUTTONS_DRIVER_init(&buttonsDriver, &GpioSwitchsAndButtons,
    BOT_BUTTONS_SWITCHES_DEVICE_ID, BOT_BUTTONS_CHANNEL);

    SWITCHES_DRIVER_init(&switchesDriver, &GpioSwitchsAndButtons,
    BOT_BUTTONS_SWITCHES_DEVICE_ID, BOT_SWITCHES_CHANNEL);

    unsigned long semiperiod = 250000;
    xil_printf("LED TEST Started\n\r");

    LEDS_DRIVER_set_led1_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led1_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led2_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led2_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led3_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led3_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led4_toggle(&ledsDriver);
    usleep(semiperiod);
    LEDS_DRIVER_set_led4_toggle(&ledsDriver);
    usleep(semiperiod);

    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_RED);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_GREEN);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_BLUE);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_CYAN);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_MAGENTA);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_MAGENTA);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_YELLOW);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_WHITE);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led1_set_color(&rgbLedsDriver, COLOR_BLACK);
    usleep(semiperiod);

    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_RED);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_GREEN);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_BLUE);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_CYAN);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_MAGENTA);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_MAGENTA);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_YELLOW);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_WHITE);
    usleep(semiperiod);
    RGB_LEDS_DRIVER_set_rgb_led2_set_color(&rgbLedsDriver, COLOR_BLACK);
    usleep(semiperiod);
    while (!BUTTONS_DRIVER_button3_pressed(&buttonsDriver)) {
        LEDS_DRIVER_set_leds_state(&ledsDriver,
                BUTTONS_DRIVER_poll(&buttonsDriver));
        RGB_LEDS_DRIVER_set_rgb_leds_state(&rgbLedsDriver,
                SWITCHES_DRIVER_poll(&switchesDriver) << 2
                        | SWITCHES_DRIVER_poll(&switchesDriver));
    }

    xil_printf("LED TEST Finished\n\r");
}

void oled_display_motor_position(PmodOLED* myOled, int speeds[2],
        int16_t positions[2]) {

    OLED_ClearBuffer(myOled);
    OLED_SetCursor(myOled, 0, 0);
    OLED_PutString(myOled, "  M1  |  M2 ");

    OLED_SetCursor(myOled, 0, 1);
    OLED_putIntVariable(myOled, speeds[0]);
    OLED_PutString(myOled, " | ");
    OLED_putIntVariable(myOled, speeds[1]);
    OLED_PutString(myOled, " rp");

    OLED_SetCursor(myOled, 0, 2);
    OLED_putIntVariable(myOled, positions[0]);
    OLED_PutString(myOled, " | ");
    OLED_putIntVariable(myOled, positions[1]);
    OLED_PutString(myOled, " st");
    OLED_Update(myOled);
}

void test_motor_position_oled() {
    PmodOLED myOled;
    MotorPosition motorPosition;

    xil_printf("OLED Init Started\n\r");
    OLED_Begin(&myOled, BOT_OLED_GPIO_BASEADDR, BOT_OLED_SPI_BASEADDR,
    BOT_OLED_ORIENTATION, BOT_OLED_INVERT);
    xil_printf("OLED Init Done\n\r");

    MOTOR_POSITION_init(&motorPosition, BOT_MOTORPOSITION_BASEADDR,
    BOT_CORE_CLOCK_FREQ_HZ, BOT_MOTORPOSITION_EDGES_PER_REVOLUTION,
    BOT_MOTORPOSITION_GEAR_BOX_RATIO);

    init_platform();
    int speeds[2];
    int16_t positions[2];

    while (1) {
        MOTOR_POSITION_get_speeds(&motorPosition, speeds);
        MOTOR_POSITION_get_positions(&motorPosition, positions);
        oled_display_motor_position(&myOled, speeds, positions);
        usleep(50000);

    }

    cleanup_platform();
}


void test_motor_pwm_hbridge() {
    HBridgeDriver hbridgeDriver;
    PWMDriver pwmDriverRightMotor;
    PWMDriver pwmDriverLeftMotor;
    LedsDriver ledsDriver;

    XTmrCtr timerPwmRightMotor;
    XTmrCtr timerPwmLeftMotor;
    XGpio GpioHbridge;
    XGpio GpioLeds; /* Dual GPIO Driver for LEDs and RGB LEDS*/

    xil_printf("PWM TEST Started\n\r");

    LEDS_DRIVER_init(&ledsDriver, &GpioLeds, BOT_LEDS_RGBLEDS_DEVICE_ID,
    BOT_LED_CHANNEL, 0b0000);

    HBRIDGE_DRIVER_init(&hbridgeDriver, &GpioHbridge, BOT_HBRIDGE_DEVICE_ID,
    BOT_HBRIDGE_CHANNEL, 0b0000, REVERSE_DIRECTION);

    HBRIDGE_DRIVER_left_motor_backward(&hbridgeDriver);

    HBRIDGE_DRIVER_right_motor_backward(&hbridgeDriver);

    PWM_DRIVER_init(&pwmDriverRightMotor,
    BOT_RIGHTMOTORTMRCTR_DEVICE_ID, &timerPwmRightMotor,
    BOT_PWM_PERIOD, 0, DISABLED);

    PWM_DRIVER_init(&pwmDriverLeftMotor,
    BOT_LEFTMOTORTMRCTR_DEVICE_ID, &timerPwmLeftMotor,
    BOT_PWM_PERIOD, 0, DISABLED);

    PWM_DRIVER_set_duty_pct(&pwmDriverLeftMotor, 0.20);
    PWM_DRIVER_enable(&pwmDriverLeftMotor);

    PWM_DRIVER_set_duty_pct(&pwmDriverRightMotor, 0.15);
    PWM_DRIVER_enable(&pwmDriverRightMotor);

    unsigned long semiperiod = 1;
    int i = 0;
    while (i < 5) {
        LEDS_DRIVER_set_led1_toggle(&ledsDriver);
        sleep(semiperiod);
        ++i;
    }

    PWM_DRIVER_disable(&pwmDriverRightMotor);
    PWM_DRIVER_disable(&pwmDriverLeftMotor);
    xil_printf("PWM TEST finished\n\r");

}
