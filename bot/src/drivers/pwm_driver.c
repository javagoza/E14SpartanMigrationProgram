/************************************************************************/
/*                                                                      */
/*  pwm_driver.c: PWM driver for the ArtyBot.                           */
/*      This driver drives a configurable PWM signal                    */
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


#include "pwm_driver.h"

/**
 * long PWM_DRIVER_init(PWMDriver* pwmDriver,u16 deviceID,XTmrCtr* timer,
 *      u32 period_ns,double dutyCyclePct,enum PWMState state)
 *
 * @details initialize the driver
 *
 * @param pwmDriver           PWM Driver with actual state reference
 * @param deviceID            AXI Device ID
 * @param timer               AXI Timer Counter reference
 * @param period_ns           PWM period in nanoseconds
 * @param dutyCyclePct        Actual duty cycle
 * @param state               Actual PWMState
 */
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


/**
 * void PWM_DRIVER_set_duty_pct(PWMDriver* pwmDriver, double dutyCyclePct)
 *
 * @details  Change the PWM duty cycle.
 *      Automatically adjusts the hightime with the new duty cycle
 *      for the set period
 *
 * @param pwmDriver      PWM Driver with actual state
 * @param dutyCyclePct   new duty cycle 0.0 to 1.0
 */
void PWM_DRIVER_set_duty_pct(PWMDriver* pwmDriver, double dutyCyclePct) {
    u32 highTime;

    if (dutyCyclePct >= 1.0) {
        dutyCyclePct = 0.99;
    } else if (dutyCyclePct < 0.0) {
        dutyCyclePct = 0.0;
    }
    pwmDriver->dutyCyclePct = dutyCyclePct;
    /* Disable PWM for reconfiguration */
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
 * PWM_DRIVER_set_period_ns(PWMDriver* pwmDriver, u32 period_ns)
 *
 * @details Change the PWM signal period.
 *      Automatically adjusts the hightime with the actual duty cycle
 *
 * @param pwmDriver      PWM Driver with actual state
 * @param period_ns      new pwm signal period in nanoseconds
 */
void PWM_DRIVER_set_period_ns(PWMDriver* pwmDriver, u32 period_ns){
    u32 highTime;
    pwmDriver->period_ns = period_ns;
    /* Disable PWM for reconfiguration */
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
 * void PWM_DRIVER_enable(PWMDriver* pwmDriver)
 *
 * @details Enable the PWM signal
 *
 * @param pwmDriver      PWM Driver with actual state
 */
void PWM_DRIVER_enable(PWMDriver* pwmDriver) {
    pwmDriver->state = ENABLED;
    XTmrCtr_PwmEnable(pwmDriver->timer);
}


/**
 * void PWM_DRIVER_disable(PWMDriver* pwmDriver)
 *
 *
 * @details Disable the PWM signal
 *
 * @param pwmDriver
 */
void PWM_DRIVER_disable(PWMDriver* pwmDriver) {
    XTmrCtr_PwmDisable(pwmDriver->timer);
    pwmDriver->state = DISABLED;
}


/**
 * double PWM_DRIVER_get_duty_pct(PWMDriver* pwmDriver)
 *
 *
 * @details returns the actual Duty Cycle Percentage 0.0 to 1.0
 *
 * @param pwmDriver
 * @return  the actual Duty Cycle Percentage 0.0 to 1.0
 */
double PWM_DRIVER_get_duty_pct(PWMDriver* pwmDriver){
    return pwmDriver->dutyCyclePct;
}

/**
 * returns the actual Period in nanoseconds
 */
u32 PWM_DRIVER_get_period_ns(PWMDriver* pwmDriver){
    return pwmDriver->period_ns;
}




