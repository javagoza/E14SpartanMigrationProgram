/*
 *
 * pwm_driver.h: PWM driver for the Bot.
 *
 * This driver drives a configurable PWM signal
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */

#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include "xil_types.h"
#include "xtmrctr.h"

enum PWMState{DISABLED = 0, ENABLED};

typedef struct PWMDriver {
	u16 deviceID;
	XTmrCtr* timer; // pointer to the XTmrCtr instance
	u32 period_ns; // period is the period of pwm signal in nano seconds.
	double dutyCyclePct; // Duty Cycle percentage 0 to 100
	enum PWMState state; // Disabled, Enabled
} PWMDriver;

long PWM_DRIVER_init(
		PWMDriver* pwmDriver,
		u16 deviceID,
		XTmrCtr* timer,
		u32 period_ns,
		double dutyCyclePct,
		enum PWMState state);

void PWM_DRIVER_set_duty_pct(PWMDriver* pwmDriver, double dutyCyclePct);
void PWM_DRIVER_set_period_ns(PWMDriver* pwmDriver, u32 period_ns);
u32 PWM_DRIVER_get_period_ns(PWMDriver* pwmDriver);
double PWM_DRIVER_get_duty_pct(PWMDriver* pwmDriver);
void PWM_DRIVER_enable(PWMDriver* pwmDriver);
void PWM_DRIVER_disable(PWMDriver* pwmDriver);

#endif // PWM_DRIVER_H
