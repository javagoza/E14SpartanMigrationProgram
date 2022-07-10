/************************************************************************/
/*                                                                      */
/*  pwm_driver.h: PWM driver Header function definitionsfor the Bot.    */
/*  This driver drives a configurable PWM signal                        */
/*  This file is part of the Arty S7 Bot.                               */
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
/* This driver drives a configurable PWM signal                         */
/* Can adjust PWM signal period, duty cycle and enable                  */
/* or disable signals                                                   */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    2022/07/03: (EAC) created                                         */
/*                                                                      */
/************************************************************************/


#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include "xil_types.h"
#include "xtmrctr.h"

// State of the PWM Signal
enum PWMState{DISABLED = 0, ENABLED};

typedef struct PWMDriver {
    u16 deviceID;   // AXI device ID
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
