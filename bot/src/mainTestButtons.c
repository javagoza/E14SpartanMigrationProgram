/*
 * test_leds_driver.c: simple test application for the Bot Leds Driver
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xil_types.h"
#include "drivers/leds_driver.h"
#include "drivers/rgb_leds_driver.h"
#include "drivers/buttons_driver.h"
#include "drivers/switches_driver.h"
#include "sleep.h"

#define LEDS_RGBLEDS_DEVICE_ID      XPAR_AXI_GPIO_0_DEVICE_ID
#define BUTTONS_SWITCHES_DEVICE_ID  XPAR_AXI_GPIO_1_DEVICE_ID
#define HBRIDGE_CONTROL_DEVICE_ID   XPAR_AXI_GPIO_HBRIDGE_DEVICE_ID



// Get device IDs from xparameters.h

#define RGB_LED_CHANNEL 1
#define LED_CHANNEL 2

#define BUTTONS_CHANNEL 1
#define SWITCHES_CHANNEL 2


void test_leds_and_buttons();

int main() {
	init_platform();
	test_leds_and_buttons();
	cleanup_platform();
	return 0;
}

void test_leds_and_buttons() {
	LedsDriver ledsDriver;
	RgbLedsDriver rgbLedsDriver;
	ButtonsDriver buttonsDriver;
	SwitchesDriver switchesDriver;

	XGpio GpioLedsAndRgbLeds; /* Dual GPIO Driver for LEDs and RGB LEDS*/
	XGpio GpioSwitchsAndButtons; /* Dual GPIO Driver for Switchs and Push Buttons */

	LEDS_DRIVER_init(&ledsDriver, &GpioLedsAndRgbLeds, LEDS_RGBLEDS_DEVICE_ID,
			LED_CHANNEL, 0b0000);


	RGB_LEDS_DRIVER_init(&rgbLedsDriver, &GpioLedsAndRgbLeds, LEDS_RGBLEDS_DEVICE_ID,
			RGB_LED_CHANNEL, 0b000000);

	BUTTONS_DRIVER_init(&buttonsDriver, &GpioSwitchsAndButtons,
			BUTTONS_SWITCHES_DEVICE_ID, BUTTONS_CHANNEL);

	SWITCHES_DRIVER_init(&switchesDriver, &GpioSwitchsAndButtons,
			BUTTONS_SWITCHES_DEVICE_ID, SWITCHES_CHANNEL);

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
	while (!BUTTONS_DRIVER_poll_button4(&buttonsDriver)) {
		LEDS_DRIVER_set_leds_state(&ledsDriver, BUTTONS_DRIVER_poll(&buttonsDriver));
        RGB_LEDS_DRIVER_set_rgb_leds_state(&rgbLedsDriver, SWITCHES_DRIVER_poll(&switchesDriver)<<2 | SWITCHES_DRIVER_poll(&switchesDriver));
	}

	xil_printf("LED TEST Finished\n\r");
}

