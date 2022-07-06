#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xil_types.h"

#include "sleep.h"
#include "xtmrctr.h"

#include "drivers/leds_driver.h"
#include "drivers/pwm_driver.h"
#include "drivers/hbridge_driver.h"

#define LEDS_RGBLEDS_DEVICE_ID      XPAR_AXI_GPIO_0_DEVICE_ID
#define RGB_LED_CHANNEL 1
#define LED_CHANNEL 2

#define HBRIDGE_DEVICE_ID      XPAR_AXI_GPIO_HBRIDGE_DEVICE_ID
#define HBRIDGE_CHANNEL 1

#define RIGHTMOTORTMRCTR_DEVICE_ID        XPAR_TMRCTR_0_DEVICE_ID
#define LEFTMOTORTMRCTR_DEVICE_ID         XPAR_TMRCTR_1_DEVICE_ID
#define PWM_PERIOD              10000000     /* PWM period in (10 ms) */
#define TMRCTR_0                0            /* Timer 0 ID */
#define TMRCTR_1                1            /* Timer 1 ID */
#define CYCLE_PER_DUTYCYCLE     10           /* Clock cycles per duty cycle */
#define MAX_DUTYCYCLE           100          /* Max duty cycle */
#define DUTYCYCLE_DIVISOR       MAX_DUTYCYCLE/* Duty cycle Divisor */

#define PERCENTAGE_INCREMENTS   10

void initleds();

int mainTestPwm() {
	HBridgeDriver hbridgeDriver;
	PWMDriver pwmDriverRightMotor;
	PWMDriver pwmDriverLeftMotor;
	LedsDriver ledsDriver;

	XTmrCtr timerPwmRightMotor;
	XTmrCtr timerPwmLeftMotor;
	XGpio GpioHbridge;
	XGpio GpioLeds; /* Dual GPIO Driver for LEDs and RGB LEDS*/

	xil_printf("PWM TEST Started\n\r");

	init_platform();

	LEDS_DRIVER_init(&ledsDriver, &GpioLeds, LEDS_RGBLEDS_DEVICE_ID, LED_CHANNEL,
			0b0000);

	HBRIDGE_DRIVER_init(&hbridgeDriver, &GpioHbridge, HBRIDGE_DEVICE_ID,
	HBRIDGE_CHANNEL, 0b0000);

	HBRIDGE_DRIVER_left_motor_backward(&hbridgeDriver);

	HBRIDGE_DRIVER_right_motor_backward(&hbridgeDriver);

	PWM_DRIVER_init(&pwmDriverRightMotor,
	RIGHTMOTORTMRCTR_DEVICE_ID, &timerPwmRightMotor,
	PWM_PERIOD, 0, DISABLED);

	PWM_DRIVER_init(&pwmDriverLeftMotor,
	LEFTMOTORTMRCTR_DEVICE_ID, &timerPwmLeftMotor,
	PWM_PERIOD, 0, DISABLED);

	PWM_DRIVER_set_duty_pct(&pwmDriverLeftMotor, 0.20);
	PWM_DRIVER_enable(&pwmDriverLeftMotor);

	PWM_DRIVER_set_duty_pct(&pwmDriverRightMotor, 0.15);
	PWM_DRIVER_enable(&pwmDriverRightMotor);

	unsigned long semiperiod = 1;
	int i = 0;
	while (i < 3) {
		LEDS_DRIVER_set_led1_toggle(&ledsDriver);
		sleep(semiperiod);
		++i;
	}

	PWM_DRIVER_disable(&pwmDriverRightMotor);
	PWM_DRIVER_disable(&pwmDriverLeftMotor);
	xil_printf("PWM TEST finished\n\r");

	cleanup_platform();
	return 0;
}


