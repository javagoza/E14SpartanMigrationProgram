/************************************************************************/
/*                                                                      */
/* driving_driver.h: Heder file for the driving driver for the Bot.                       */
/* This file driver provides functions for driving and stearing the bot */
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
/*  This file driver define the functions for driving and stearing      */
/*  the bot.                                                            */
/*                                                                      */
/*  Call BOT_DRIVER_init for initializing the driver                    */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*     2022/07/03: (EAC) created                                        */
/*                                                                      */
/************************************************************************/


#ifndef BOT_DRIVER_H
#define BOT_DRIVER_H

#include "distance_pid_control.h"
#include "motor_position.h"
#include "pwm_driver.h"
#include "speed_pid_control.h"
#include "hbridge_driver.h"

typedef enum MoveDirection {
    IDLE, FORWARD, BACKWARD, TURN_LEFT, TURN_RIGHT
} MoveDirection;

typedef enum TurnDirection {
    LEFT = 0, RIGHT =1
} TurnDirection;

typedef enum SensorsConfiguration {
    FRONT_SENSORS, REAR_SENSORS
} SensorsConfiguration;

typedef struct DrivingDriver {
    MotorPosition*         motorPosition;
    PWMDriver*             pwmRightMotor;
    PWMDriver*             pwmLeftMotor;
    u32                    pwmPeriod_ns;
    MoveDirection          direction;
    MoveDirection          previousDirection;
    int16_t                previousPositionDifference;
    SpeedPIDController*    speedPIDController;
    DistancePIDController* distancePIDController;
    HBridgeDriver*         hbridge;
    SensorsConfiguration   sensorsConfiguration;
    double                   distanceCmCorrection;
    double                    distanceArcCmCorrection;
} DrivingDriver;

/************ Function Prototypes ************/

// High level API calls

void DRIVING_DRIVER_init(DrivingDriver* driver, MotorPosition* motorPosition,
        PWMDriver* pwmRightMotor, PWMDriver* pwmLeftMotor, u32 pwmPeriod_ns,
        SpeedPIDController* speedPIDController,
        DistancePIDController* distancePIDController, HBridgeDriver* hbridge,
        SensorsConfiguration sensorsConfiguration, double distanceCmCorrection,
        double distanceArcCmCorrection, double baseDutyCycle);

void DRIVING_DRIVER_end(DrivingDriver* driver);

void DRIVING_DRIVER_drive_forward_cm(DrivingDriver* driver, double distanceCm);

void DRIVING_DRIVER_drive_backward_cm(DrivingDriver* driver, double distanceCm);

void DRIVING_DRIVER_turn_left_degrees(DrivingDriver* driver, int degrees);

void DRIVING_DRIVER_turn_right_degrees(DrivingDriver* driver, int degrees);

void DRIVING_DRIVER_drive_forward_continuous_cm(DrivingDriver* driver, double distanceCm);

void DRIVING_DRIVER_drive_backward_continuous_cm(DrivingDriver* driver, double distanceCm);

void DRIVING_DRIVER_turn_left_continuous_degrees(DrivingDriver* driver, int degrees);

void DRIVING_DRIVER_turn_right_continuous_degrees(DrivingDriver* driver, int degrees);

void DRIVING_DRIVER_swing_turn_left_degrees(DrivingDriver* driver, int degrees);

void DRIVING_DRIVER_swing_turn_right_degrees(DrivingDriver* driver, int degrees);

void DRIVING_DRIVER_set_speed(DrivingDriver * driver, double baseDutyCyclePct);

#endif // BOT_DRIVER_H
