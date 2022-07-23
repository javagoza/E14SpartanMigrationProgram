/************************************************************************/
/*                                                                      */
/*    bot.c    --    Bot Initializing Implementation                    */
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
/*    This file contains the implementation of the functions for        */
/*  initializing the bot.                                               */
/*                                                                      */
/*  Usage:                                                              */
/*      BotDrivers botDrivers;                                          */
/*                                                                      */
/*      BOT_init(&botDrivers);                                          */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2022/07/10: (EAC) created                                         */
/*                                                                      */
/************************************************************************/
#include "bot.h"


static MotorPosition motorPosition;
static PWMDriver pwmRightMotor;
static PWMDriver pwmLeftMotor;
static SpeedPIDController speedPIDController;
static DistancePIDController distancePIDController;
static LightPIDController lightPIDController;
static HBridgeDriver hbridge;
static XTmrCtr timerPwmRightMotor;
static XTmrCtr timerPwmLeftMotor;
static XGpio GpioHbridge;
static XGpio GpioSwitchsAndButtons;
static XGpio GpioLedsAndRgbLeds;

/**
 * Initialize the BOT with the given driver references
 * @param drivers for inputs, outpus, display and driving
 */
void BOT_init(BotDrivers* drivers) {
    BOT_init_gpio_inputs(&(drivers->switchesDriver), &(drivers->buttonsDriver));
    BOT_init_gpio_outputs(&(drivers->rgbLedsDriver), &(drivers->ledsDriver));
    BOT_init_oled_display(&(drivers->oled));
    BOT_init_driving_driver(&(drivers->drivingDriver));
    BOT_init_color_sensor(&(drivers->color));
}

/**
 * Initialize GPIO inputs, switches and push buttons
 * @param switchesDriver reference to switches driver
 * @param buttonsDriver  reference to push buttons driver
 */
void BOT_init_gpio_inputs(SwitchesDriver* switchesDriver,
        ButtonsDriver* buttonsDriver) {
    /* Dual GPIO Driver for Switches and Push Buttons */
    BUTTONS_DRIVER_init(buttonsDriver, &GpioSwitchsAndButtons,
    BOT_BUTTONS_SWITCHES_DEVICE_ID, BOT_BUTTONS_CHANNEL);
    SWITCHES_DRIVER_init(switchesDriver, &GpioSwitchsAndButtons,
    BOT_BUTTONS_SWITCHES_DEVICE_ID, BOT_SWITCHES_CHANNEL);
}

/**
 * Initialize GPIO outputs for red leds anf rgb leds
 * @param rgbLedsDriver reference to the 2 RGB leds driver
 * @param ledsDriver    reference to the 4 red leds driver
 */
void BOT_init_gpio_outputs(RgbLedsDriver* rgbLedsDriver, LedsDriver* ledsDriver) {
    LEDS_DRIVER_init(ledsDriver, &GpioLedsAndRgbLeds,
    BOT_LEDS_RGBLEDS_DEVICE_ID, BOT_LED_CHANNEL, 0b0000);
    RGB_LEDS_DRIVER_init(rgbLedsDriver, &GpioLedsAndRgbLeds,
    BOT_LEDS_RGBLEDS_DEVICE_ID, BOT_RGB_LED_CHANNEL, 0b000000);
}

/**
 * Initialize Pmod OLED display
 * @param oled    reference to Pmod OLED display driver
 */
void BOT_init_oled_display(PmodOLED* oled) {
    OLED_Begin(oled, BOT_OLED_GPIO_BASEADDR, BOT_OLED_SPI_BASEADDR,
    BOT_OLED_ORIENTATION, BOT_OLED_INVERT);
}

/**
 * Initialize Pmod Color sensor
 * @param colorSensor referce to Pmod Color sensor
 */
void BOT_init_color_sensor(PmodCOLOR *colorSensor)
{
    COLOR_Begin(colorSensor,
            BOT_PMODCOLOR_0_AXI_LITE_IIC_BASEADDR,
            BOT_PMODCOLOR_0_AXI_LITE_GPIO_BASEADDR, 0x29);

    COLOR_SetENABLE(colorSensor,
            COLOR_REG_ENABLE_PON_MASK);
    usleep(2400);
    COLOR_SetENABLE(colorSensor,
    COLOR_REG_ENABLE_PON_MASK | COLOR_REG_ENABLE_RGBC_INIT_MASK);
    usleep(2400);
}

/**
 * Initialize the Bot Driving Drive
 * @param botDriver    reference to Bot Driving Driver
 */
void BOT_init_driving_driver(DrivingDriver* botDriver) {
    MOTOR_POSITION_init(&motorPosition, BOT_MOTORPOSITION_BASEADDR,
    BOT_CORE_CLOCK_FREQ_HZ, BOT_MOTORPOSITION_EDGES_PER_REVOLUTION,
    BOT_MOTORPOSITION_GEAR_BOX_RATIO);

    PWM_DRIVER_init(&pwmRightMotor, BOT_RIGHTMOTORTMRCTR_DEVICE_ID,
            &timerPwmRightMotor, BOT_PWM_PERIOD, 0, DISABLED);
    PWM_DRIVER_init(&pwmLeftMotor, BOT_LEFTMOTORTMRCTR_DEVICE_ID,
            &timerPwmLeftMotor, BOT_PWM_PERIOD, 0, DISABLED);

    HBRIDGE_DRIVER_init(&hbridge, &GpioHbridge, BOT_HBRIDGE_DEVICE_ID,
    BOT_HBRIDGE_CHANNEL, 0b0000, DIRECT_DIRECTION);

    SPEED_PID_CONTROLLER_init(&speedPIDController, BOT_SPEED_PID_K_PROPORTIONAL,
    BOT_SPEED_PID_K_INTEGRAL, BOT_SPEED_PID_K_DERIVATIVE,
    BOT_BASE_DUTY_CYCLE);

    DISTANCE_PID_CONTROLLER_init(&distancePIDController,
    BOT_DISTANCE_PID_K_PROPORTIONAL, BOT_DISTANCE_PID_K_INTEGRAL,
    BOT_DISTANCE_PID_K_DERIVATIVE, BOT_BASE_DUTY_CYCLE);

    LIGHT_PID_CONTROLLER_init(&lightPIDController, BOT_LIGHT_PID_K_PROPORTIONAL,
    BOT_LIGHT_PID_K_INTEGRAL,
    BOT_LIGHT_PID_K_DERIVATIVE, BOT_BASE_DUTY_CYCLE);


    DRIVING_DRIVER_init(botDriver, &motorPosition, &pwmRightMotor,
            &pwmLeftMotor,
            BOT_PWM_PERIOD, &speedPIDController, &distancePIDController,
            &hbridge, FRONT_SENSORS, BOT_DISTANCE_CORRECTION,
            BOT_DISTANCE_ARC_CORRECTION, BOT_BASE_DUTY_CYCLE, botDriver->colorSensor);

}
