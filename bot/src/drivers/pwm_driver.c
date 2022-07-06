/*
 *
 * pwm_driver.c: PWM driver for the ArtyBot.
 *
 * This driver drives a configurable PWM signal
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */


#include "pwm_driver.h"

/**
 * Change the PWM duty cycle.
 * Automatically adjusts the hightime with the new duty cycle
 * for the set period
 *
 */
void PWM_DRIVER_set_duty_pct(PWMDriver* pwmDriver, double dutyCyclePct) {
	u32 highTime;

	if (dutyCyclePct >= 1.0) {
		dutyCyclePct = 0.99;
	} else if (dutyCyclePct < 0.0) {
		dutyCyclePct = 0.0;
	}
	pwmDriver->dutyCyclePct = dutyCyclePct;
	/* Disable PWM for reconfiguration if percentage == 0 disable pwm */
	PWM_DRIVER_disable(pwmDriver);
	/* Configure PWM */
	if (pwmDriver->dutyCyclePct > 0.0) {
		highTime = pwmDriver->period_ns * pwmDriver->dutyCyclePct;
		XTmrCtr_PwmConfigure(pwmDriver->timer, pwmDriver->period_ns, highTime);
		/* Enable PWM after reconfiguration */
		if (pwmDriver->state == ENABLED) {
			PWM_DRIVER_enable(pwmDriver);
		}
	}
}

/**
 * Change the PWM signal period.
 * Automatically adjusts the hightime with the actual duty cycle
 */
void PWM_DRIVER_set_period(PWMDriver* pwmDriver, u32 period_ns){
	u32 highTime;
	pwmDriver->period_ns = period_ns;
	/* Disable PWM for reconfiguration if percentage == 0 disable pwm */
	PWM_DRIVER_disable(pwmDriver);
	/* Configure PWM */
	if (pwmDriver->dutyCyclePct > 0.0) {
		highTime = pwmDriver->period_ns * pwmDriver->dutyCyclePct;
		XTmrCtr_PwmConfigure(pwmDriver->timer, pwmDriver->period_ns, highTime);
		/* Enable PWM after reconfiguration */
		if (pwmDriver->state == ENABLED) {
			PWM_DRIVER_enable(pwmDriver);
		}
	}
}

/**
 * Enable the PWM signal
 */
void PWM_DRIVER_enable(PWMDriver* pwmDriver) {
	pwmDriver->state = ENABLED;
	XTmrCtr_PwmEnable(pwmDriver->timer);
}

/**
 * Disable the PWM signal
 */
void PWM_DRIVER_disable(PWMDriver* pwmDriver) {
	XTmrCtr_PwmDisable(pwmDriver->timer);
	pwmDriver->state = DISABLED;
}

/**
 * returns the actual Duty Cycle Percentage 0 to 100
 */
double PWM_DRIVER_get_duty_pct(PWMDriver* pwmDriver){
	return pwmDriver->dutyCyclePct;
}

/**
 * returns the actual Perido in nanoseconds
 */
u32 PWM_DRIVER_get_period_ns(PWMDriver* pwmDriver){
	return pwmDriver->period_ns;
}




long PWM_DRIVER_init(
		PWMDriver* pwmDriver,
		u16 deviceID,
		XTmrCtr* timer,
		u32 period_ns,
		double dutyCyclePct,
		enum PWMState state) {

	int status;
	pwmDriver->deviceID = deviceID;
	pwmDriver->timer = timer;
	pwmDriver->period_ns = period_ns;

	pwmDriver->state = state;

	pwmDriver->dutyCyclePct = dutyCyclePct;

	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	status = XTmrCtr_Initialize(pwmDriver->timer, pwmDriver->deviceID);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	PWM_DRIVER_set_duty_pct(pwmDriver, dutyCyclePct);
    return 0;
}
