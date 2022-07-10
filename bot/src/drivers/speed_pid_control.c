/************************************************************************/
/*                                                                      */
/*  speed_pid_control.c: PID controller for controlling the rotational  */
/*  speed of the two motors with goal of maintaining both speeds equals */
/*  at a target speed                                                   */
/*                                                                      */
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

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/************************************************************************/
/*  Module Description:                                                 */
/*  This module contains the function definitions of the speed PID      */
/*  controller to compute new duty cycles for the right and left motors */
/*  with goal of maintaining motor rotational speeds at speed target    */
/*  Assumes that this function gets called at regular time intervals    */
/*                                                                      */
/*  Call SPEED_PID_CONTROLLER_init when creating a new controller       */
/*                                                                      */
/*  Call periodically at equal intervals                                */
/*      SPEED_PID_CONTROLLER_get_new_outputs                            */
/*  to get the new dutycycle computed values by the controller          */
/*                                                                      */
/*  Call SPEED_PID_CONTROLLER_set_duty_cycle to change base speed    */
/*                                                                      */
/*  Call SPEED_PID_CONTROLLER_reset_errors to reset errors           */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*     2022/07/03: (EAC) created                                        */
/*                                                                      */
/************************************************************************/

#include "speed_pid_control.h"



/**
 * SPEED_PID_CONTROLLER_init(SpeedPIDController * controller,
 *  double KProportional, double KIntegral, double KDerivative, double baseDutyCyclePct)
 *
 * Uses a PID controller to compute new duty cycles for left and right motors
 * with goal of maintaining rotational motor speeds at speed target and store them in
 * dutyCyclePct. Assumes that this function gets called at regular time intervals
 *
 * @param controller            Speed PID controller configuration and state
 * @param KProportional         PID proportional term
 * @param KIntegral             PID Integral term
 * @param KDerivative           PID Derivative term
 * @param baseDutyCyclePct      Base duty cycle percentage
 */
void SPEED_PID_CONTROLLER_init(SpeedPIDController * controller,
        double KProportional, double KIntegral, double KDerivative,
        double baseDutyCyclePct) {

    controller->KProportional = KProportional;
    controller->KIntegral = KIntegral;
    controller->KDerivative = KDerivative;
    controller->baseDutyCyclePct = baseDutyCyclePct;

}


/**
 * void SPEED_PID_CONTROLLER_set_duty_cycle(DistancePIDController * controller,
 *       double baseDutyCyclePct)
 *
 * Set the new base duty cycle
 *
 * @param controller        Distance PID controller configuration and state
 * @param baseDutyCyclePct  Base duty cycle percentage 0.4 typical 0.75 for fast speed
 */
void SPEED_PID_CONTROLLER_set_duty_cycle(SpeedPIDController * controller,
        double baseDutyCyclePct) {
    controller->baseDutyCyclePct = baseDutyCyclePct;
}


/**
 * void SPEED_PID_CONTROLLER_get_new_outputs(
 *      SpeedPIDController * controller, int pos_diff, double dutyCyclePct[])
 *
 *  Uses a PID controller to compute new duty cycles for the right motor and the left motor
 *  with goal of minimizing position difference to 0 and store them in duty_cycle.
 *  Assumes that this function gets called at regular time intervals
 *
 * @param controller        Speed PID controller configuration and state
 * @param speedTargetRpm    Speed set point (desired speed) in RPM
 * @param speedsRpm         Array with current speeds of right motor, left motor, respectively
 * @param dutyCyclePct      Array for storing new percentage duty cycles for right motor
 *                          and left motor respectively. Values from 0.0 to 1.0
 */
void SPEED_PID_CONTROLLER_get_new_outputs(SpeedPIDController * controller,
        int speedTargetRpm, int speedsRpm[], double dutyCyclePct[]) {

    int err_m1 = speedTargetRpm - speedsRpm[0]; // Current error
    int err_m2 = speedTargetRpm - speedsRpm[1];

    controller->accumulated_error[0] += err_m1; // Accumulated error
    controller->accumulated_error[1] += err_m2;

    dutyCyclePct[0] = controller->baseDutyCyclePct;
    dutyCyclePct[1] = controller->baseDutyCyclePct;

    dutyCyclePct[0] += controller->KProportional * err_m1
            + controller->KIntegral * controller->accumulated_error[0]
            + controller->KDerivative
                    * (err_m1 - controller->previous_error[0]);

    dutyCyclePct[1] += controller->KProportional * err_m2
            + controller->KIntegral * controller->accumulated_error[1]
            + controller->KDerivative
                    * (err_m2 - controller->previous_error[1]);

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
 * void SPEED_PID_CONTROLLER_reset_errors(SpeedPIDController * controller)
 *
 * Reset accumulated errors and previous errors to 0
 *
 * @param controller    Speed PID controller configuration and state
 */
void SPEED_PID_CONTROLLER_reset_errors(SpeedPIDController * controller) {
    controller->previous_error[0] = 0;
    controller->previous_error[1] = 0;

    controller->accumulated_error[0] = 0;
    controller->accumulated_error[1] = 0;

}
