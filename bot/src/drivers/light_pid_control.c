/************************************************************************/
/*                                                                      */
/* light_pid_control.c PID controller for controlling motor speed       */
/* with goal minimizing ligth difference from target                    */
/************************************************************************/
/*  This file is part of the Arty S7 Bot Library                        */
/*  Parts of this Library are based in Arvin Tang's ArtyBot Library     */
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
/*  This module implements a PID controller to compute new duty cycles  */
/*  for the right and left motors with goal of minimizing the light     */
/*  difference from target to 0                                         */
/*  Assumes that this function gets called at regular time intervals    */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*     2022/07/16: (EAC) created                                         */
/*                                                                      */
/************************************************************************/

#include "light_pid_control.h"

/**
 * void LIGHT_PID_CONTROLLER_init(LightPIDController * controller,
 *         double KProportional, double KIntegral, double KDerivative,
 *         double baseDutyCyclePct)
 *
 * Uses a PID controller to compute new duty cycles for motor1 and motor2
 * with goal of minimizing light difference from target. Assumes that this
 * function gets called at regular time intervals.
 *
 * @param controller            Distance PID controller configuration and state
 * @param KProportional         PID proportional term
 * @param KIntegral             PID Integral term
 * @param KDerivative           PID Derivative term
 * @param baseDutyCyclePct      Base duty cycle percentage
 */
void LIGHT_PID_CONTROLLER_init(LightPIDController * controller,
        double KProportional, double KIntegral, double KDerivative,
        double baseDutyCyclePct) {

    controller->KProportional = KProportional;
    controller->KIntegral = KIntegral;
    controller->KDerivative = KDerivative;
    controller->baseDutyCyclePct = baseDutyCyclePct;

}

/**
 * void LIGHT_PID_CONTROLLER_set_duty_cycle(LightPIDController * controller,
 *       double baseDutyCyclePct)
 *
 * Set the new base duty cycle
 *
 * @param controller        Distance PID controller configuration and state
 * @param baseDutyCyclePct  Base duty cycle percentage 0.4 typical 0.75 for fast speed
 *                          If using PMODColor do not exceed .2 for fast
 */
void LIGHT_PID_CONTROLLER_set_duty_cycle(LightPIDController * controller,
        double baseDutyCyclePct) {
    controller->baseDutyCyclePct = baseDutyCyclePct;
}

/**
 * void LIGHT_PID_CONTROLLER_get_new_outputs(
 *      LightPIDController * controller, int pos_diff, double dutyCyclePct[])
 *
 *  Uses a PID controller to compute new duty cycles for the right motor and the left motor
 *  with goal of minimizing light difference to 0 and store them in duty_cycle.
 *  Assumes that this function gets called at regular time intervals
 *
 * @param controller        Distance PID controller configuration and state
 * @param pos_diff
 * @param dutyCyclePct      returns base duty cycle values (from 0.0 to 1.0)
 *                          for right and left motors
 */
void LIGHT_PID_CONTROLLER_get_new_outputs(
        LightPIDController * controller, int pos_diff, double dutyCyclePct[]) {

    controller->accumulated_error += pos_diff;


    double correction = controller->KProportional * pos_diff
            + controller->KIntegral * controller->accumulated_error
            + controller->KDerivative * (pos_diff - controller->previous_error);


    if (correction > 0.0) {
        dutyCyclePct[0] = controller->baseDutyCyclePct + correction; //
        dutyCyclePct[1] = controller->baseDutyCyclePct ;
    } else  {
        dutyCyclePct[0] = controller->baseDutyCyclePct;
        dutyCyclePct[1] = controller->baseDutyCyclePct - correction;
    }
    controller->previous_error = pos_diff;

    // Bound duty cycles between 0 and 1
    if (dutyCyclePct[0] < 0.0) {
        dutyCyclePct[0] = 0.0;
    } else if (dutyCyclePct[0] > 1.0) {
        dutyCyclePct[0] = 1.0;
    }

    if (dutyCyclePct[1] < 0.0) {
        dutyCyclePct[1] = 0.0;
    } else if (dutyCyclePct[1] > 1.0) {
        dutyCyclePct[1] = 1.0;
    }

}

/**
 * void LIGHT_PID_CONTROLLER_reset_errors(LightPIDController * controller)
 *
 * Reset accumulated errors and previous errors to 0
 *
 * @param controller    Distance PID controller configuration and state
 */
void LIGHT_PID_CONTROLLER_reset_errors(LightPIDController * controller) {
    controller->accumulated_error = 0;
    controller->previous_error = 0;
}
