/************************************************************************/
/*                                                                      */
/*  distance_pid_control.h: Header file for the bot Distance            */
/*  PID controller                                                      */
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
/*  This module contains the function definitions of the distance PID   */
/*  controller to compute new duty cycles for the right and left motors */
/*  with goal of minimizing the traveled distance by means              */
/*  of minimizing the position difference between the two motors to 0   */
/*  Assumes that this function gets called at regular time intervals    */
/*                                                                      */
/*  Call LIGHT_PID_CONTROLLER_init when creating a new controller    */
/*                                                                      */
/*  Call periodically at equal intervals                                */
/*      LIGHT_PID_CONTROLLER_get_new_outputs                     */
/*  to get the new dutycycle computed values by the controller          */
/*                                                                      */
/*  Call LIGHT_PID_CONTROLLER_set_duty_cycle to change base speed    */
/*                                                                      */
/*  Call LIGHT_PID_CONTROLLER_reset_errors to reset errors           */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*     2022/07/03: (EAC) created                                        */
/*                                                                      */
/************************************************************************/



#ifndef LIGHT_PID_CONTROLLER_H
#define LIGHT_PID_CONTROLLER_H

typedef struct LightPIDController {
    double KProportional;
    double KIntegral;
    double KDerivative;
    int accumulated_error;
    int previous_error;
    double baseDutyCyclePct;
} LightPIDController;

/************ Function Prototypes ************/

void LIGHT_PID_CONTROLLER_init(LightPIDController * controller,
        double KProportional, double KIntegral, double KDerivative,
        double baseDutyCyclePct);

void LIGHT_PID_CONTROLLER_get_new_outputs(
        LightPIDController * controller, int pos_diff, double duty_cycle[]);

void LIGHT_PID_CONTROLLER_reset_errors(LightPIDController * controller);

void LIGHT_PID_CONTROLLER_set_duty_cycle(LightPIDController * controller,
        double baseDutyCyclePct);

#endif //LIGHT_PID_CONTROLLER_H
