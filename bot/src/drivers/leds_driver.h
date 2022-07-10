/*
 *
 * leds_driver.h: Leds driver for the Bot.
 *
 * This driver controls the 4 red LEDS
 * for the Bot and holds the actual regs state
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */

#ifndef LEDS_DRIVER_H
#define LEDS_DRIVER_H

/****************** Include Files ********************/

#include "xstatus.h"
#include "xgpio.h"

typedef struct LedsDriver {
    XGpio* gpio;
    u32 deviceID;
    u32 channelID;
    u32 state; // holds the current value of the led register
} LedsDriver;

#define LED1_MASK 0b0001
#define LED2_MASK 0b0010
#define LED3_MASK 0b0100
#define LED4_MASK 0b1000


void LEDS_DRIVER_init(LedsDriver* driver, XGpio* gpio, u16 deviceID, unsigned channelID, u32 state);

void LEDS_DRIVER_set_leds_state(LedsDriver* driver, u32 state);


u32 LEDS_DRIVER_get_leds_state(LedsDriver* driver);


void LEDS_DRIVER_set_led1_on(LedsDriver* driver);
void LEDS_DRIVER_set_led2_on(LedsDriver* driver);
void LEDS_DRIVER_set_led3_on(LedsDriver* driver);
void LEDS_DRIVER_set_led4_on(LedsDriver* driver);

void LEDS_DRIVER_set_led1_off(LedsDriver* driver);
void LEDS_DRIVER_set_led2_off(LedsDriver* driver);
void LEDS_DRIVER_set_led3_off(LedsDriver* driver);
void LEDS_DRIVER_set_led4_off(LedsDriver* driver);

void LEDS_DRIVER_set_led1_toggle(LedsDriver* driver);
void LEDS_DRIVER_set_led2_toggle(LedsDriver* driver);
void LEDS_DRIVER_set_led3_toggle(LedsDriver* driver);
void LEDS_DRIVER_set_led4_toggle(LedsDriver* driver);

#endif // LEDS_DRIVER_H
