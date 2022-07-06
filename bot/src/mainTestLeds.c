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
#include "sleep.h"

#define LEDS_RGBLEDS_DEVICE_ID      XPAR_AXI_GPIO_0_DEVICE_ID
#define BUTTONS_SWITCHES_DEVICE_ID  XPAR_AXI_GPIO_1_DEVICE_ID
#define HBRIDGE_CONTROL_DEVICE_ID   XPAR_AXI_GPIO_HBRIDGE_DEVICE_ID

XGpio GpioLedsAndRgbLeds ;   /* Dual GPIO Driver for LEDs and RGB LEDS*/
XGpio GpioSwitchsAndButtons; /* Dual GPIO Driver for Switchs and Push Buttons */
XGpio GpioHbridgeControl;    /* Single GPIO Driver for HBridge Control */

// Get device IDs from xparameters.h

#define RGB_LED_CHANNEL 1
#define LED_CHANNEL 2

#define BUTTONS_CHANNEL 1
#define BTN_MASK 0b1111
#define LED_MASK 0b1111

LedsDriver ledsDriver;
RgbLedsDriver rgbLedsDriver;

void init_leds(LedsDriver* driver);
void init_rgb_leds(RgbLedsDriver* driver);
void test_leds();


int main_test_leds()
{
    init_platform();
    test_leds();
    cleanup_platform();
    return 0;
}



void test_leds() {
	unsigned long semiperiod = 250000;
    xil_printf("LED TEST Started\n\r");
    init_leds(&ledsDriver);
    init_rgb_leds(&rgbLedsDriver);

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

void init_leds(LedsDriver* driver) {

	XGpio* gpioLedsAndRgbLeds = &GpioLedsAndRgbLeds;
    LEDS_DRIVER_init(driver,
    		gpioLedsAndRgbLeds,
			LEDS_RGBLEDS_DEVICE_ID,
			LED_CHANNEL,
			0b0000);

}

void init_rgb_leds(RgbLedsDriver* driver) {

	XGpio* gpioLedsAndRgbLeds = &GpioLedsAndRgbLeds;
    RGB_LEDS_DRIVER_init(driver,
    		gpioLedsAndRgbLeds,
			LEDS_RGBLEDS_DEVICE_ID,
			RGB_LED_CHANNEL,
			0b000000);

}
