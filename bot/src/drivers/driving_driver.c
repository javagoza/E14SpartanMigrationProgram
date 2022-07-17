/************************************************************************/
/*                                                                      */
/* driving_driver.c:  Driving driver for the Bot.                       */
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
/*  This file driver provides functions for driving and stearing        */
/*  the bot.                                                            */
/*                                                                      */
/*  Call BOT_DRIVER_init for initializing the driver                    */
/*                                                                      */
/*  DRIVING_DRIVER_drive_forward_cm :                                   */
/*    Drive forward by given distance (cm), then come to complete stop. */
/*                                                                      */
/*  DRIVING_DRIVER_drive_backward_cm                                    */
/*    Drive backward by given distance (cm), then come to complete      */
/*    stop.                                                             */
/*                                                                      */
/*  DRIVING_DRIVER_turn_left_degrees:                                   */
/*    Turn left about the center by given angle from forward,           */
/*    then come to complete stop.                                       */
/*                                                                      */
/*  DRIVING_DRIVER_turn_right_degrees:                                  */
/*      turn right about the center by given angle from forward,        */
/*      then come to complete stop.                                     */
/*                                                                      */
/*  DRIVING_DRIVER_drive_forward_continuous_cm                          */
/*      Drive forward by given distance (cm), but do not come to        */
/*      complete stop.Repeated calls to this function given short       */
/*      distances will allow processing during apparent continuous      */
/*      driving.                                                        */
/*                                                                      */
/*  DRIVING_DRIVER_drive_backward_continuous_cm                         */
/*      Drive backward by given distance (cm), but do not come to       */
/*      complete stop.Repeated calls to this function given short       */
/*      distances will allow processing during apparent continuous      */
/*      driving.                                                        */
/*                                                                      */
/*  DRIVING_DRIVER_turn_left_continuous_degrees                         */
/*      Turn left about the center by given angle from forward,         */
/*       then come to complete stop.                                    */
/*                                                                      */
/*  DRIVING_DRIVER_turn_right_continuous_degrees                        */
/*      Turn right about the center by given angle from forward,        */
/*       then come to complete stop.                                    */
/*                                                                      */
/*  DRIVING_DRIVER_swing_turn_left_degrees                              */
/*      Turn left about left wheel by given angle from forward,         */
/*      then come to complete stop.                                     */
/*                                                                      */
/*  DRIVING_DRIVER_swing_turn_right_degrees                             */
/*      Turn left about right wheel by given angle from forward,        */
/*      then come to complete stop.                                     */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*     2022/07/03: (EAC) created                                        */
/*                                                                      */
/************************************************************************/

/************ Include Files ************/

#include "driving_driver.h"

#include "motor_position.h"
#include "speed_pid_control.h"
#include "pwm_driver.h"
#include "sleep.h"
#include "distance_pid_control.h"

/************ Macro Definitions ************/

#define SAMPLE_PER 25000 // 25 ms => 40 Hz sample frequency

#define FULL_TURN_ARCLENGTH       3.141 * 15.0
#define FULL_SWING_TURN_ARCLENGTH 3.141 * 15.0 * 2

/************ Function Definitions ************/

// Low level API calls
void DRIVING_DRIVER_set_direction_forward(DrivingDriver* driver);
void DRIVING_DRIVER_set_direction_backward(DrivingDriver* driver);
void DRIVING_DRIVER_set_direction_left(DrivingDriver* driver);
void DRIVING_DRIVER_set_direction_right(DrivingDriver* driver);
void DRIVING_DRIVER_delay_until_stop(DrivingDriver* driver);
void DRIVING_DRIVER_drive_cm(DrivingDriver* driver, double distanceCm);
void DRIVING_DRIVER_turn(DrivingDriver* driver, double arclength);
void DRIVING_DRIVER_swing_turn(DrivingDriver* driver, double arclength,
        TurnDirection dir);

void enableMotors(DrivingDriver* driver);
void disableMotors(DrivingDriver* driver);
void resetErrors(DrivingDriver* driver);

void DRIVING_DRIVER_drive_light(DrivingDriver* driver, double distance_cm,
        u16 lightTarget);


/**
 * void DRIVING_DRIVER_init(DrivingDriver* driver, MotorPosition* motorPosition,
 *         PWMDriver* pwmRightMotor, PWMDriver* pwmLeftMotor, u32 pwmPeriod_ns,
 *         SpeedPIDController* speedPIDController,
 *         DistancePIDController* distancePIDController, HBridgeDriver* hbridge,
 *         SensorsConfiguration sensorsConfiguration, double distanceCmCorrection,
 *         double distanceArcCmCorrection, double baseDutyCycle)
 *
 * @details Configure and Initialize the Driving Driver
 *
 * @param driver                    Driving driver to use with its actual state
 * @param motorPosition             Motor Position driver
 * @param pwmRightMotor             PWM driver for the right motor
 * @param pwmLeftMotor              PWM driver for the left motor
 * @param pwmPeriod_ns              set the period for both PWM Drivers
 * @param speedPIDController        PID controller for speed control
 * @param distancePIDController     Distance controllr for position control
 * @param hbridge                   Hbridge driver
 * @param sensorsConfiguration      Proximity and Bumper Sensors in motors direct direction
 * @param distanceCmCorrection      cm to edge counts factor when driving straight
 * @param distanceArcCmCorrection   cm to edge counts factor when turning wheels
 * @param baseDutyCycle             base duty cycle for PID controllers, indirectly
 *                                  sets the mean rotational speed
 */
void DRIVING_DRIVER_init(DrivingDriver* driver, MotorPosition* motorPosition,
        PWMDriver* pwmRightMotor, PWMDriver* pwmLeftMotor, u32 pwmPeriod_ns,
        SpeedPIDController* speedPIDController,
        DistancePIDController* distancePIDController, HBridgeDriver* hbridge,
        SensorsConfiguration sensorsConfiguration, double distanceCmCorrection,
        double distanceArcCmCorrection, double baseDutyCycle) {

    driver->motorPosition = motorPosition;
    driver->pwmLeftMotor = pwmLeftMotor;
    driver->pwmRightMotor = pwmRightMotor;
    driver->speedPIDController = speedPIDController;
    driver->distancePIDController = distancePIDController;
    driver->hbridge = hbridge;
    driver->pwmPeriod_ns = pwmPeriod_ns;
    driver->direction = IDLE;
    driver->previousDirection = IDLE;
    driver->sensorsConfiguration = sensorsConfiguration;
    driver->distanceCmCorrection = distanceCmCorrection;
    driver->distanceArcCmCorrection = distanceArcCmCorrection;

    if (driver->sensorsConfiguration == FRONT_SENSORS) {
        HBRIDGE_DRIVER_set_direction(driver->hbridge, DIRECT_DIRECTION);
    } else {
        HBRIDGE_DRIVER_set_direction(driver->hbridge, REVERSE_DIRECTION);
    }

    MOTOR_POSITION_clear_pos_counter(driver->motorPosition);
    MOTOR_POSITION_clear_speed_counters(driver->motorPosition);

    PWM_DRIVER_set_period_ns(driver->pwmRightMotor, pwmPeriod_ns);
    PWM_DRIVER_disable(driver->pwmRightMotor);

    PWM_DRIVER_set_period_ns(driver->pwmLeftMotor, pwmPeriod_ns);
    PWM_DRIVER_disable(driver->pwmLeftMotor);

    DRIVING_DRIVER_set_speed(driver, baseDutyCycle);

    driver->previousPositionDifference = MOTOR_POSITION_get_position_difference(
            driver->motorPosition);
}

void DRIVING_DRIVER_set_light_pid_controller(DrivingDriver* driver, LightPIDController* lightPIDController, PmodCOLOR* colorSensor){
    driver->lightPIDController = lightPIDController;
    driver->colorSensor = colorSensor;
}

/**
 * DRIVING_DRIVER_set_speed(DrivingDriver * driver, double baseDutyCycle)
 *
 * @details Set new speed for the bot varying the baseDutyCycle of the PID controllers
 *
 * @param driver            Driving driver to use with its actual state
 * @param baseDutyCycle     new base duty cycle from 0.0 to 1.0
 */
void DRIVING_DRIVER_set_speed(DrivingDriver * driver, double baseDutyCycle) {
    SPEED_PID_CONTROLLER_set_duty_cycle(driver->speedPIDController,
            baseDutyCycle);
    DISTANCE_PID_CONTROLLER_set_duty_cycle(driver->distancePIDController,
            baseDutyCycle);
}

/**
 * void DRIVING_DRIVER_end(DrivingDriver* driver)
 *
 * @details Clean up the Bot. Call when driving driver is no longer needed
 *
 * @param driver         Driving driver to use with its actual state
 */
void DRIVING_DRIVER_end(DrivingDriver* driver) {
    disableMotors(driver);
    DRIVING_DRIVER_set_direction_forward(driver);

}

/**
 *  DRIVING_DRIVER_drive_forward_cm(DrivingDriver* driver, double distanceCm)
 *
 * @details Drive forward by given distance (cm), then come to complete stop
 *
 * @param driver            Driving driver to use with its actual state
 * @param distanceCm        Distance in cm to drive the Bot
 */
void DRIVING_DRIVER_drive_forward_cm(DrivingDriver* driver, double distanceCm) {
    DRIVING_DRIVER_drive_forward_continuous_cm(driver, distanceCm);
    DRIVING_DRIVER_delay_until_stop(driver);
}

/**
 * void DRIVING_DRIVER_drive_backward_cm(DrivingDriver* driver, double distanceCm)
 *
 * @details Drive backward by given distance (cm), then come to complete stop
 *
 * @param driver           Driving driver to use with its actual state
 * @param distanceCm       Distance in cm to drive the Bot
 */
void DRIVING_DRIVER_drive_backward_cm(DrivingDriver* driver, double distanceCm) {
    DRIVING_DRIVER_drive_backward_continuous_cm(driver, distanceCm);
    DRIVING_DRIVER_delay_until_stop(driver);
}

/**
 * void DRIVING_DRIVER_turn_left_degrees(DrivingDriver* driver, int degrees)
 *
 * @details Turn left about the center by given angle from forward, then come to
 *       complete stop
 *
 * @param driver            Driving driver to use with its actual state
 * @param degrees           Angle in degrees to left turn the Bot
 */
void DRIVING_DRIVER_turn_left_degrees(DrivingDriver* driver, int degrees) {
    DRIVING_DRIVER_turn_left_continuous_degrees(driver, degrees);
    DRIVING_DRIVER_delay_until_stop(driver);
}

/**
 * void DRIVING_DRIVER_turn_left_degrees(DrivingDriver* driver, int degrees)
 *
 * @details Turn right about the center by given angle from forward, then come to
 *       complete stop
 *
 * @param driver            Driving driver to use with its actual state
 * @param degrees           Angle in degrees to right turn the Bot
 */
void DRIVING_DRIVER_turn_right_degrees(DrivingDriver* driver, int degrees) {
    DRIVING_DRIVER_turn_right_continuous_degrees(driver, degrees);
    DRIVING_DRIVER_delay_until_stop(driver);
}

/**
 * void DRIVING_DRIVER_drive_forward_continuous_cm(DrivingDriver* driver,
 *         double distanceCm)
 *
 * @details Drive forward by given distance (cm), but do not come to complete stop
 *      Repeated calls to this function given short distances will allow
 *      processing during apparent continuous driving
 *
 * @param driver            Driving driver to use with its actual state
 * @param distanceCm        Distance in cm to drive the Bot
 */
void DRIVING_DRIVER_drive_forward_continuous_cm(DrivingDriver* driver,
        double distanceCm) {
    DRIVING_DRIVER_set_direction_forward(driver);
    DRIVING_DRIVER_drive_cm(driver, distanceCm);
}

/**
 * void DRIVING_DRIVER_drive_forward_continuous_cm(DrivingDriver* driver,
 *         double distanceCm)
 *
 * @details Drive backward by given distance (cm), but do not come to complete stop
 *      Repeated calls to this function given short distances will allow
 *      processing during apparent continuous driving
 *
 * @param driver            Driving driver to use with its actual state
 * @param distanceCm        Distance in cm to drive the Bot
 */
void DRIVING_DRIVER_drive_backward_continuous_cm(DrivingDriver* driver,
        double distanceCm) {
    DRIVING_DRIVER_set_direction_backward(driver);
    DRIVING_DRIVER_drive_cm(driver, distanceCm);
}

/**
 * void DRIVING_DRIVER_turn_left_continuous_degrees(DrivingDriver* driver, int degrees)
 *
 * @details Turn left about the center by given angle from forward, but do not come
 *      to complete stop.
 *      Repeated calls to this function given small angles will allow processing
 *      during apparent continuous turning.
 *
 * @param driver            Driving driver to use with its actual state
 * @param degrees           Angle in degrees to left turn the Bot
 */
void DRIVING_DRIVER_turn_left_continuous_degrees(DrivingDriver* driver,
        int degrees) {
    double arclength = FULL_TURN_ARCLENGTH * (degrees / 360.0);
    DRIVING_DRIVER_set_direction_left(driver);
    DRIVING_DRIVER_turn(driver, arclength);
}

/**
 * void DRIVING_DRIVER_turn_right_continuous_degrees(DrivingDriver* driver, int degrees)
 *
 * @details Turn right about the center by given angle from forward, but do not come
 *      to complete stop.
 *      Repeated calls to this function given small angles will allow processing
 *      during apparent continuous turning.
 *
 * @param driver            Driving driver to use with its actual state
 * @param degrees           Angle in degrees to right turn the Bot
 */
void DRIVING_DRIVER_turn_right_continuous_degrees(DrivingDriver* driver,
        int degrees) {
    double arclength = FULL_TURN_ARCLENGTH * (degrees / 360.0);
    DRIVING_DRIVER_set_direction_right(driver);

    DRIVING_DRIVER_turn(driver, arclength);
}

/**
 * void DRIVING_DRIVER_swing_turn_left_degrees(DrivingDriver* driver, int degrees)
 *
 * @details Turn left about left wheel by given angle from forward, then come to complete stop
 *
 * @param driver            Driving driver to use with its actual state
 * @param degrees           Angle in degrees to left turn the Bot
 */
void DRIVING_DRIVER_swing_turn_left_degrees(DrivingDriver* driver, int degrees) {
    double arclength = FULL_SWING_TURN_ARCLENGTH * (degrees / 360.0);
    DRIVING_DRIVER_set_direction_left(driver);
    DRIVING_DRIVER_swing_turn(driver, arclength, LEFT);
    DRIVING_DRIVER_delay_until_stop(driver);
}

/**
 * void DRIVING_DRIVER_swing_turn_right_degrees(DrivingDriver* driver, int degrees)
 *
 * @details Turn left about right wheel by given angle from forward, then come to complete stop
 *
 * @param driver            Driving driver to use with its actual state
 * @param degrees           Angle in degrees to right turn the Bot
 */
void DRIVING_DRIVER_swing_turn_right_degrees(DrivingDriver* driver, int degrees) {
    double arclength = FULL_SWING_TURN_ARCLENGTH * (degrees / 360.0);
    DRIVING_DRIVER_set_direction_right(driver);
    DRIVING_DRIVER_swing_turn(driver, arclength, RIGHT);
    DRIVING_DRIVER_delay_until_stop(driver);
}

/**
 * void DRIVING_DRIVER_set_direction_forward(DrivingDriver* driver)
 *
 * @details Set motor directions to forward and reset position counters
 *
 * @param driver           Driving driver to use with its actual state
 */
void DRIVING_DRIVER_set_direction_forward(DrivingDriver* driver) {
    driver->previousDirection = driver->direction;
    if (driver->direction != FORWARD) {
        disableMotors(driver);
        usleep(6);
        HBRIDGE_DRIVER_left_motor_forward(driver->hbridge);
        HBRIDGE_DRIVER_right_motor_forward(driver->hbridge);
        resetErrors(driver);
        driver->direction = FORWARD;
    }
    MOTOR_POSITION_clear_pos_counter(driver->motorPosition);
}

/**
 * void DRIVING_DRIVER_set_direction_backward(DrivingDriver* driver)
 *
 * @details Set motor directions to backward and reset position counters
 *
 * @param driver           Driving driver to use with its actual state
 */
void DRIVING_DRIVER_set_direction_backward(DrivingDriver* driver) {
    driver->previousDirection = driver->direction;
    if (driver->direction != BACKWARD) {
        disableMotors(driver);
        usleep(6);
        HBRIDGE_DRIVER_left_motor_backward(driver->hbridge);
        HBRIDGE_DRIVER_right_motor_backward(driver->hbridge);
        resetErrors(driver);
        driver->direction = BACKWARD;
    }
    MOTOR_POSITION_clear_pos_counter(driver->motorPosition);
}

/**
 * void DRIVING_DRIVER_set_direction_left(DrivingDriver* driver) *
 *
 * @details Set motor directions for left turn (left motor backward,
 *      right motor forward) and reset position counters
 *
 * @param driver            Driving driver to use with its actual state
 */
void DRIVING_DRIVER_set_direction_left(DrivingDriver* driver) {
    driver->previousDirection = driver->direction;
    disableMotors(driver);
    usleep(6);
    HBRIDGE_DRIVER_left_motor_backward(driver->hbridge);
    HBRIDGE_DRIVER_right_motor_forward(driver->hbridge);
    resetErrors(driver);
    driver->direction = TURN_LEFT;
    MOTOR_POSITION_clear_pos_counter(driver->motorPosition);
    MOTOR_POSITION_clear_speed_counters(driver->motorPosition);
}

/**
 * void DRIVING_DRIVER_set_direction_right(DrivingDriver* driver)
 *
 * @details Set motor directions for right turn (left motor forward, right motor
 *      backward) and reset position counters
 *
 * @param driver           Driving driver to use with its actual state
 */
void DRIVING_DRIVER_set_direction_right(DrivingDriver* driver) {
    driver->previousDirection = driver->direction;
    disableMotors(driver);
    usleep(6);
    HBRIDGE_DRIVER_left_motor_forward(driver->hbridge);
    HBRIDGE_DRIVER_right_motor_backward(driver->hbridge);
    resetErrors(driver);
    driver->direction = TURN_RIGHT;
    MOTOR_POSITION_clear_pos_counter(driver->motorPosition);
    MOTOR_POSITION_clear_speed_counters(driver->motorPosition);
}

/**
 * void DRIVING_DRIVER_drive_cm(DrivingDriver* driver, double distance_cm)
 *
 * @details Drive motors given distance using positional control (motors will have
 *       turned about the same amount at the end)
 *
 * @param driver            Driving driver to use with its actual state
 * @param distance_cm       Distance in cm to drive the Bot
 */
void DRIVING_DRIVER_drive_cm(DrivingDriver* driver, double distance_cm) {
    int16_t dist_converted = (int16_t) (distance_cm
            * driver->distanceCmCorrection); // TODO cm to sensed edges

    int16_t pos_diff = MOTOR_POSITION_get_position_difference(
            driver->motorPosition);
    double duty_cycle[2];
    if (driver->direction == driver->previousDirection) {
        DISTANCE_PID_CONTROLLER_get_new_outputs(driver->distancePIDController,
                driver->previousPositionDifference, duty_cycle);
    } else {
        DISTANCE_PID_CONTROLLER_get_new_outputs(driver->distancePIDController,
                pos_diff, duty_cycle);
    }

    int16_t dist_traveled = MOTOR_POSITION_get_distance_traveled(
            driver->motorPosition);

    disableMotors(driver);

    PWM_DRIVER_set_duty_pct(driver->pwmRightMotor, duty_cycle[RIGHT_MOTOR]);
    PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor, duty_cycle[LEFT_MOTOR]);

    enableMotors(driver);

    while (dist_traveled < dist_converted) {
        usleep(SAMPLE_PER);
        pos_diff = MOTOR_POSITION_get_position_difference(
                driver->motorPosition);

        DISTANCE_PID_CONTROLLER_get_new_outputs(driver->distancePIDController,
                pos_diff, duty_cycle);

        disableMotors(driver);

        PWM_DRIVER_set_duty_pct(driver->pwmRightMotor, duty_cycle[RIGHT_MOTOR]);
        PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor, duty_cycle[LEFT_MOTOR]);

        enableMotors(driver);

        dist_traveled = MOTOR_POSITION_get_distance_traveled(
                driver->motorPosition);

    }

    driver->previousPositionDifference = pos_diff;

    disableMotors(driver);

}

/**
 * DRIVING_DRIVER_turn(DrivingDriver* driver, double arclength)
 *
 * @details Drive motors given arclength using speed control (motors will turn at
 *       same rate throughout the turn)
 *
 * @param driver        Driving driver to use with its actual state
 * @param arclength     Length of arc in cm for wheels to follow during turn
 */
void DRIVING_DRIVER_turn(DrivingDriver* driver, double arclength) {
    int16_t dist_converted = (int16_t) (arclength
            * driver->distanceArcCmCorrection); // cm to sens edges

    int motor_speed[2];
    MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);

    int16_t motor_position[2];
    MOTOR_POSITION_get_positions(driver->motorPosition, motor_position);

    double duty_cycle[2];
    int speedTarget = 25;

    SPEED_PID_CONTROLLER_get_new_outputs(driver->speedPIDController,
            speedTarget, motor_speed, duty_cycle);

    enableMotors(driver);

    while (motor_position[RIGHT_MOTOR] < dist_converted
            || motor_position[LEFT_MOTOR] < dist_converted) {
//        int16_t dist_to_targ = dist_converted
//                - (motor_position[RIGHT_MOTOR] + motor_position[LEFT_MOTOR]) / 2;
//        speedTarget = 25;
//        if (dist_to_targ < 25) {
//            speedTarget = 12.5 + 0.5 * dist_to_targ;
//        }
        usleep(SAMPLE_PER);
        MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);

        SPEED_PID_CONTROLLER_get_new_outputs(driver->speedPIDController,
                speedTarget, motor_speed, duty_cycle);

        disableMotors(driver);

        if (motor_position[RIGHT_MOTOR] >= dist_converted) {
            PWM_DRIVER_set_duty_pct(driver->pwmRightMotor, 0.0);
        } else {
            PWM_DRIVER_set_duty_pct(driver->pwmRightMotor,
                    duty_cycle[RIGHT_MOTOR]);
        }
        if (motor_position[LEFT_MOTOR] >= dist_converted) {
            PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor, 0.0);
        } else {
            PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor,
                    duty_cycle[LEFT_MOTOR]);
        }

        enableMotors(driver);

        MOTOR_POSITION_get_positions(driver->motorPosition, motor_position);
    }

    disableMotors(driver);
}

/**
 * void BOT_DRIVER_swing_turn(BotDriver* driver, double arclength, int dir)
 *
 * @details Drive one motor (determined by dir parameter) the given arclength using
 *       speed control (motor will turn at constant rate throughout the turn)
 *
 * @param driver            Driving driver to use with its actual state
 * @param arclength         Length of arc in cm for wheels to follow during turn
 * @param dir               Direction of turn LEFT, RIGHT
 */
void DRIVING_DRIVER_swing_turn(DrivingDriver* driver, double arclength,
        TurnDirection dir) {
    int16_t dist_converted = (int16_t) (arclength
            * driver->distanceArcCmCorrection); // cm to sens edges

    int motor_speed[2];
    MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);

    int16_t motor_position[2];
    MOTOR_POSITION_get_positions(driver->motorPosition, motor_position);

    double duty_cycle[2];
    int speedTarget = 30;
    SPEED_PID_CONTROLLER_get_new_outputs(driver->speedPIDController,
            speedTarget, motor_speed, duty_cycle);

    if (dir == RIGHT) {
        PWM_DRIVER_enable(driver->pwmRightMotor);
        PWM_DRIVER_disable(driver->pwmLeftMotor);
    } else {
        PWM_DRIVER_disable(driver->pwmRightMotor);
        PWM_DRIVER_enable(driver->pwmLeftMotor);
    }

    while (motor_position[RIGHT_MOTOR] < dist_converted
            && motor_position[LEFT_MOTOR] < dist_converted) {
        usleep(SAMPLE_PER);
        MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);

        SPEED_PID_CONTROLLER_get_new_outputs(driver->speedPIDController,
                speedTarget, motor_speed, duty_cycle);

        disableMotors(driver);

        if (dir == RIGHT) {
            PWM_DRIVER_set_duty_pct(driver->pwmRightMotor,
                    duty_cycle[RIGHT_MOTOR]);
            PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor, 0.0);
        } else {
            PWM_DRIVER_set_duty_pct(driver->pwmRightMotor, 0.0);
            PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor,
                    duty_cycle[LEFT_MOTOR]);
        }

        if (dir == RIGHT) {
            PWM_DRIVER_enable(driver->pwmRightMotor);
            PWM_DRIVER_disable(driver->pwmLeftMotor);
        } else {
            PWM_DRIVER_disable(driver->pwmRightMotor);
            PWM_DRIVER_enable(driver->pwmLeftMotor);
        }

        MOTOR_POSITION_get_positions(driver->motorPosition, motor_position);
    }
    disableMotors(driver);
}

/**
 * void DRIVING_DRIVER_delay_until_stop(DrivingDriver* driver)
 *
 * @details     Wait until both motors have stopped turning
 *
 * @param driver        Driving driver to use with its actual state
 */
void DRIVING_DRIVER_delay_until_stop(DrivingDriver* driver) {

    MOTOR_POSITION_clear_speed_counters(driver->motorPosition);

    int motor_speed[2];
    MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);
    while (motor_speed[RIGHT_MOTOR] + motor_speed[LEFT_MOTOR]) {
        for (int i = 0; i < 3; i++) {
            while (motor_speed[RIGHT_MOTOR] + motor_speed[LEFT_MOTOR]) {
                usleep(SAMPLE_PER);
                MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);
            }
            usleep(SAMPLE_PER);
        }
        MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);
    }
}

/**
 * void disableMotors(DrivingDriver* driver)
 *
 * @details disables both motors disabling the PWM signals
 *
 * @param driver           Driving driver to use with its actual state
 */
void disableMotors(DrivingDriver* driver) {
    PWM_DRIVER_disable(driver->pwmLeftMotor);
    PWM_DRIVER_disable(driver->pwmRightMotor);
}

/**
 * void enableMotors(DrivingDriver* driver)
 *
 * @details enables both motors enabling the PWM signals
 *
 * @param driver           Driving driver to use with its actual state
 */
void enableMotors(DrivingDriver* driver) {
    PWM_DRIVER_enable(driver->pwmLeftMotor);
    PWM_DRIVER_enable(driver->pwmRightMotor);
}

/**
 * void resetErrors(DrivingDriver* driver)
 *
 * @details resets error for both PID controllers
 *
 * @param driver           Driving driver to use with its actual state
 */
void resetErrors(DrivingDriver* driver) {
    SPEED_PID_CONTROLLER_reset_errors(driver->speedPIDController);
    DISTANCE_PID_CONTROLLER_reset_errors(driver->distancePIDController);
}




void DRIVING_DRIVER_drive_forward_continuous_light(DrivingDriver* driver,
        double distanceCm, u16 lightTarget) {
    DRIVING_DRIVER_set_direction_forward(driver);
    DRIVING_DRIVER_drive_light(driver, distanceCm, lightTarget);
}

u16 DRIVING_DRIVER_light(COLOR_Data sample) {
    return sample.r;
}

/**
 * void DRIVING_DRIVER_drive_cm(DrivingDriver* driver, double distance_cm)
 *
 * @details Drive motors given distance using positional control (motors will have
 *       turned about the same amount at the end)
 *
 * @param driver            Driving driver to use with its actual state
 * @param distance_cm       Distance in cm to drive the Bot
 */
void DRIVING_DRIVER_drive_light(DrivingDriver* driver, double distance_cm,
        u16 lightTarget) {

    int16_t dist_converted = (int16_t) (distance_cm
            * driver->distanceCmCorrection); // TODO cm to sensed edges

    int16_t light_diff = DRIVING_DRIVER_light(COLOR_GetData(driver->colorSensor)) - lightTarget;
    double duty_cycle[2];

    LIGHT_PID_CONTROLLER_get_new_outputs(driver->lightPIDController, light_diff,
            duty_cycle);

    int16_t dist_traveled = MOTOR_POSITION_get_distance_traveled(
            driver->motorPosition);

    disableMotors(driver);

    PWM_DRIVER_set_duty_pct(driver->pwmRightMotor, duty_cycle[RIGHT_MOTOR]);
    PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor, duty_cycle[LEFT_MOTOR]);

    enableMotors(driver);

    while (dist_traveled < dist_converted) {
        usleep(SAMPLE_PER);

        light_diff = DRIVING_DRIVER_light(COLOR_GetData(driver->colorSensor)) - lightTarget;

        LIGHT_PID_CONTROLLER_get_new_outputs(driver->lightPIDController, light_diff,
                duty_cycle);

        disableMotors(driver);

        PWM_DRIVER_set_duty_pct(driver->pwmRightMotor, duty_cycle[RIGHT_MOTOR]);
        PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor, duty_cycle[LEFT_MOTOR]);

        enableMotors(driver);

        dist_traveled = MOTOR_POSITION_get_distance_traveled(
                driver->motorPosition);

    }

    driver->previousLightDifference = light_diff;

    disableMotors(driver);

}

