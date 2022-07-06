/*
 *
 * rgb_leds_driver.h: RGB Leds driver for the Bot.
 *
 * This driver controls the 2 RGB LEDs
 * for the Bot and holds the actual regs state
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */

#ifndef RGB_LEDS_DRIVER_H
#define RGB_LEDS_DRIVER_H

/****************** Include Files ********************/

#include "xstatus.h"
#include "xgpio.h"

typedef struct RgbLedsDriver {
	XGpio* gpio;
	u32 deviceID;
	u32 channelID;
	u32 state; // holds the current value of the rgb led register
} RgbLedsDriver;

#define RGBLED2_RED_MASK   0b100000
#define RGBLED2_GREEN_MASK 0b010000
#define RGBLED2_BLUE_MASK  0b001000

#define RGBLED1_RED_MASK   0b000100
#define RGBLED1_GREEN_MASK 0b000010
#define RGBLED1_BLUE_MASK  0b000001

#define COLOR_RED       0b100
#define COLOR_GREEN     0b010
#define COLOR_BLUE      0b001
#define COLOR_YELLOW    0b110
#define COLOR_CYAN      0b011
#define COLOR_MAGENTA   0b101
#define COLOR_BLACK     0b000
#define COLOR_WHITE     0b111


void RGB_LEDS_DRIVER_init(RgbLedsDriver* driver, XGpio* gpio,
		u16 deviceID, unsigned channelID, u32 state);

void RGB_LEDS_DRIVER_set_rgb_leds_state(RgbLedsDriver* driver, u32 state);

u32 RGB_LEDS_DRIVER_get_rgb_leds_state(RgbLedsDriver* driver);

void RGB_LEDS_DRIVER_set_rgb_led1_red_on(RgbLedsDriver* driver);
void RGB_LEDS_DRIVER_set_rgb_led1_green_on(RgbLedsDriver* driver);
void RGB_LEDS_DRIVER_set_rgb_led1_blue_on(RgbLedsDriver* driver);

void RGB_LEDS_DRIVER_set_rgb_led2_red_on(RgbLedsDriver* driver);
void RGB_LEDS_DRIVER_set_rgb_led2_green_on(RgbLedsDriver* driver);
void RGB_LEDS_DRIVER_set_rgb_led2_blue_on(RgbLedsDriver* driver);

void RGB_LEDS_DRIVER_set_rgb_led1_red_off(RgbLedsDriver* driver);
void RGB_LEDS_DRIVER_set_rgb_led1_green_off(RgbLedsDriver* driver);
void RGB_LEDS_DRIVER_set_rgb_led1_blue_off(RgbLedsDriver* driver);

void RGB_LEDS_DRIVER_set_rgb_led2_red_off(RgbLedsDriver* driver);
void RGB_LEDS_DRIVER_set_rgb_led2_green_off(RgbLedsDriver* driver);
void RGB_LEDS_DRIVER_set_rgb_led2_blue_off(RgbLedsDriver* driver);

void RGB_LEDS_DRIVER_set_rgb_led1_red_toggle(RgbLedsDriver* driver);
void RGB_LEDS_DRIVER_set_rgb_led1_green_toggle(RgbLedsDriver* driver);
void RGB_LEDS_DRIVER_set_rgb_led1_blue_toggle(RgbLedsDriver* driver);

void RGB_LEDS_DRIVER_set_rgb_led2_red_toggle(RgbLedsDriver* driver);
void RGB_LEDS_DRIVER_set_rgb_led2_green_toggle(RgbLedsDriver* driver);
void RGB_LEDS_DRIVER_set_rgb_led2_blue_toggle(RgbLedsDriver* driver);


void RGB_LEDS_DRIVER_set_rgb_led1_set_color(RgbLedsDriver* driver, u32 colorMask);
void RGB_LEDS_DRIVER_set_rgb_led2_set_color(RgbLedsDriver* driver, u32 colorMask);


#endif // RGB_LEDS_DRIVER_H
