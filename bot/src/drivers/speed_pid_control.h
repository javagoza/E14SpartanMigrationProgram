/************************************************************************/
/*                                                                      */
/*  speed_pid_control.h: Header file for the bot Speed PID controller   */
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
 */

/************************************************************************/
/*  Module Description:                                                 */
/*  This module contains the function definitions of the Speed PID      */
/*  controller to compute new duty cycles for the right and left motors */
/*  with goal of maintaining the same target speed for both motors      */
/*  Assumes that this function gets called at regular time intervals    */
/*                                                                      */
/*  Call SPEED_PID_CONTROLLER_init when creating a new controller       */
/*                                                                      */
/*  Call periodically at equal intervals                                */
/*      SPEED_PID_CONTROLLER_get_new_outputs                            */
/*  to get the new duty cycle computed values by the controller         */
/*                                                                      */
/*  Call SPEED_PID_CONTROLLER_set_duty_cycle to change base speed       */
/*                                                                      */
/*  Call SPEED_PID_CONTROLLER_reset_errors to reset errors              */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*     2022/07/03: (EAC) created                                        */
/*                                                                      */
/************************************************************************/

#ifndef SPEED_PID_CONTROLLER_H
#define SPEED_PID_CONTROLLER_H

typedef struct SpeedPIDController {
    double KProportional;
    double KIntegral;
    double KDerivative;
    int accumulated_error[2];
    int previous_error[2];
    double baseDutyCyclePct;
} SpeedPIDController;

/************ Function Prototypes ************/

void SPEED_PID_CONTROLLER_init(SpeedPIDController * controller,
        double KProportional, double KIntegral, double KDerivative,
        double baseDutyCyclePct);

void SPEED_PID_CONTROLLER_get_new_outputs(SpeedPIDController * controller,
        int speedTargetRpm, int speedsRpm[], double dutyCyclePct[]);

void SPEED_PID_CONTROLLER_reset_errors(SpeedPIDController * controller);

void SPEED_PID_CONTROLLER_set_duty_cycle(SpeedPIDController * controller,
        double baseDutyCyclePct);

#endif //SPEED_PID_CONTROLLER_H
