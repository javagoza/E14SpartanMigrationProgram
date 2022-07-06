/*
 *
 * bot_driver.h: Driving driver for the Bot.
 *
 * This driver provides functions for driving and stearing the bot
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */

/************ Include Files ************/

#include "bot_driver.h"
#include "motor_position.h"
#include "speed_pid_control.h"
#include "position_pid_control.h"
#include "pwm_driver.h"
#include "sleep.h"

/************ Macro Definitions ************/

#define SAMPLE_PER 25000 // 25 ms => 40 Hz sample frequency

#define FULL_TURN_ARCLENGTH       48.7
#define FULL_SWING_TURN_ARCLENGTH 97.4



/************ Function Definitions ************/
void enableMotors(BotDriver* driver);
void disableMotors(BotDriver* driver);
void resetErrors(BotDriver* driver);

/*
 * void BOT_DRIVER_init(BotDriver* driver, MotorPosition* motorPosition,
 *	PWMDriver* pwmRightMotor, PWMDriver* pwmLeftMotor,
 *		u32 pwmPeriod_ns)
 * ------------------------------------------------------------------------
 * Parameters:
 *      BotDriver* driver,
 *		MotorPosition* 			motorPosition, Motor Position driver
 *		PWMDriver*     			pwmRightMotor, PWM driver for the right motor
 *		PWMDriver*     			pwmLeftMotor, PWM driver for the left motor
 *		u32            			pwmPeriod_ns
 *		SpeedPIDController*		speedPIDController
 *		PositionPIDController*	positionPIDController
 *
 * Return:
 *       void
 *
 * Description:
 *       Initialize the Bot
 */
void BOT_DRIVER_init(BotDriver* driver, MotorPosition* motorPosition,
		PWMDriver* pwmRightMotor, PWMDriver* pwmLeftMotor, u32 pwmPeriod_ns,
		SpeedPIDController* speedPIDController,
		PositionPIDController* positionPIDController, HBridgeDriver* hbridge) {

	driver->motorPosition = motorPosition;
	driver->pwmLeftMotor = pwmLeftMotor;
	driver->pwmRightMotor = pwmRightMotor;
	driver->speedPIDController = speedPIDController;
	driver->positionPIDController = positionPIDController;
	driver->hbridge = hbridge;
	driver->pwmPeriod_ns = pwmPeriod_ns;
	driver->direction = IDLE;
	driver->previousDirection = IDLE;

	MOTOR_POSITION_clear_pos_counter(driver->motorPosition);
	MOTOR_POSITION_clear_speed_counters(driver->motorPosition);

	PWM_DRIVER_set_period_ns(driver->pwmRightMotor, driver->pwmPeriod_ns);
	PWM_DRIVER_disable(driver->pwmRightMotor);

	PWM_DRIVER_set_period_ns(driver->pwmLeftMotor, driver->pwmPeriod_ns);
	PWM_DRIVER_disable(driver->pwmLeftMotor);

	driver->previousPositionDifference = MOTOR_POSITION_get_position_difference(
			driver->motorPosition);
}

/*
 * void BOT_DRIVER_end(BotDriver* driver)
 * ------------------------------------------------------------------------
 * Parameters:
 *       BotDriver* driver to use with its actual state
 *
 * Return:
 *       void
 *
 * Description:
 *       Clean up the Bot
 */
void BOT_DRIVER_end(BotDriver* driver) {
	disableMotors(driver);
}

/*
 * void BOT_DRIVER_drive_forward(BotDriver* driver, double distance)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       distance: Distance in cm to drive the Bot
 *
 * Return:
 *       void
 *
 * Description:
 *       Drive forward by given distance (cm), then come to complete stop
 */
void BOT_DRIVER_drive_forward_cm(BotDriver* driver, double distanceCm) {
	BOT_DRIVER_drive_forward_continuous_cm(driver, distanceCm);
	BOT_DRIVER_delay_until_stop(driver);
}

/*
 * void BOT_DRIVER_drive_backward(BotDriver* driver, double distance)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       distance: Distance in cm to drive the Bot
 *
 * Return:
 *       void
 *
 * Description:
 *       Drive backward by given distance (cm), then come to complete stop
 */
void BOT_DRIVER_drive_backward_cm(BotDriver* driver, double distanceCm) {
	BOT_DRIVER_drive_backward_continuous_cm(driver, distanceCm);
	BOT_DRIVER_delay_until_stop(driver);
}

/*
 * void BOT_DRIVER_turn_left_degrees(BotDriver* driver, int degrees)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       degrees: Angle in degrees to turn the Bot
 *
 * Return:
 *       void
 *
 * Description:
 *       Turn left about the center by given angle from forward, then come to
 *       complete stop
 */
void BOT_DRIVER_turn_left_degrees(BotDriver* driver, int degrees) {
	BOT_DRIVER_turn_left_continuous_degrees(driver, degrees);
	BOT_DRIVER_delay_until_stop(driver);
}

/*
 * void BOT_DRIVER_turn_right(BotDriver* driver, int degrees)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       degrees: Angle in degrees to turn the Bot
 *
 * Return:
 *       void
 *
 * Description:
 *       Turn right about the center by given angle from forward, then come to
 *       complete stop
 */
void BOT_DRIVER_turn_right_degrees(BotDriver* driver, int degrees) {
	BOT_DRIVER_turn_right_continuous_degrees(driver, degrees);
	BOT_DRIVER_delay_until_stop(driver);
}

/*
 * BOT_DRIVER_drive_forward_continuous(BotDriver* driver, double distance)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       distance: Distance in cm to drive the Bot
 *
 * Return:
 *       void
 *
 * Description:
 *       Drive forward by given distance (cm), but do not come to complete stop
 *       Repeated calls to this function given short distances will allow
 *       processing during apparent continuous driving
 */
void BOT_DRIVER_drive_forward_continuous_cm(BotDriver* driver, double distanceCm) {
	BOT_DRIVER_set_direction_forward(driver);
	BOT_DRIVER_drive_cm(driver, distanceCm);
}

/*
 * void BOT_DRIVER_drive_backward_continuous(BotDriver* driver, double distance)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       distance: Distance in cm to drive the Bot
 *
 * Return:
 *       void
 *
 * Description:
 *       Drive backward by given distance (cm), but do not come to complete stop
 *       Repeated calls to this function given short distances will allow
 *       processing during apparent continuous driving
 */
void BOT_DRIVER_drive_backward_continuous_cm(BotDriver* driver, double distanceCm) {
	BOT_DRIVER_set_direction_backward(driver);
	BOT_DRIVER_drive_cm(driver, distanceCm);
}

/*
 * void BOT_DRIVER_turn_left_continuous(BotDriver* driver, int degrees)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       degrees: Angle in degrees to turn the Bot
 *
 * Return:
 *       void
 *
 * Description:
 *       Turn left about the center by given angle from forward, but do not come
 *       to complete stop
 *       Repeated calls to this function given small angles will allow
 *       processing during apparent continuous turning
 */
void BOT_DRIVER_turn_left_continuous_degrees(BotDriver* driver, int degrees) {
	double arclength = FULL_TURN_ARCLENGTH * (degrees / 360.0);
	BOT_DRIVER_set_direction_left(driver);
	BOT_DRIVER_turn(driver, arclength);
}

/*
 * void BOT_DRIVER_turn_right_continuous(BotDriver* driver, int degrees)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       degrees: Angle in degrees to turn the Bot
 *
 * Return:
 *       void
 *
 * Description:
 *       Turn right about the center by given angle from forward, but do not
 *       come to complete stop
 *       Repeated calls to this function given small angles will allow
 *       processing during apparent continuous turning
 */
void BOT_DRIVER_turn_right_continuous_degrees(BotDriver* driver, int degrees) {
	double arclength = FULL_TURN_ARCLENGTH * (degrees / 360.0);
	BOT_DRIVER_set_direction_right(driver);

	BOT_DRIVER_turn(driver, arclength);
}

/*
 * void BOT_DRIVER_swing_turn_left(BotDriver* driver, int degrees)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       degrees: Angle in degrees to turn the Bot
 *
 * Return:
 *       void
 *
 * Description:
 *       Turn left about left wheel by given angle from forward, then come to
 *       complete stop
 */
void BOT_DRIVER_swing_turn_left_degrees(BotDriver* driver, int degrees) {
	double arclength = FULL_SWING_TURN_ARCLENGTH * (degrees / 360.0);
	BOT_DRIVER_set_direction_left(driver);
	BOT_DRIVER_swing_turn(driver, arclength, LEFT);
	BOT_DRIVER_delay_until_stop(driver);
}

/*
 * void BOT_DRIVER_swing_turn_right(BotDriver* driver, int degrees)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       degrees: Angle in degrees to turn the Bot
 *
 * Return:
 *       void
 *
 * Description:
 *       Turn right about right wheel by given angle from forward, then come to
 *       complete stop
 */
void BOT_DRIVER_swing_turn_right_degrees(BotDriver* driver, int degrees) {
	double arclength = FULL_SWING_TURN_ARCLENGTH * (degrees / 360.0);
	BOT_DRIVER_set_direction_right(driver);
	BOT_DRIVER_swing_turn(driver, arclength, RIGHT);
	BOT_DRIVER_delay_until_stop(driver);
}


/*
 * void BOT_DRIVER_set_direction_forward(BotDriver* driver)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *
 * Return:
 *       void
 *
 * Description:
 *       Set motor directions to forward and reset position counters
 */
void BOT_DRIVER_set_direction_forward(BotDriver* driver) {
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

/*
 * void BOT_DRIVER_set_direction_backward(BotDriver* driver)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *
 * Return:
 *       void
 *
 * Description:
 *       Set motor directions to backward and reset position counters
 */
void BOT_DRIVER_set_direction_backward(BotDriver* driver) {
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

/*
 * void BOT_DRIVER_set_direction_left(BotDriver* driver)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *
 * Return:
 *       void
 *
 * Description:
 *       Set motor directions for left turn (left motor backward, right motor
 *       forward) and reset position counters
 */
void BOT_DRIVER_set_direction_left(BotDriver* driver) {
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

/*
 * void BOT_DRIVER_set_direction_right(BotDriver* driver)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *
 * Return:
 *       void
 *
 * Description:
 *       Set motor directions for right turn (left motor forward, right motor
 *       backward) and reset position counters
 */
void BOT_DRIVER_set_direction_right(BotDriver* driver) {
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


/*
 * void BOT_DRIVER_drive(BotDriver* driver, double distance)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       distance: Distance in cm to drive the Bot
 *
 * Return:
 *       void
 *
 * Description:
 *       Drive motors given distance using positional control (motors will have
 *       turned about the same amount at the end)
 */
void BOT_DRIVER_drive(BotDriver* driver, double distance) {
	int16_t dist_converted = (int16_t) (distance * 9.4); // TODO cm to sens edges

	int16_t pos_diff = MOTOR_POSITION_get_position_difference(
			driver->motorPosition);
	double duty_cycle[2];
	if (driver->direction == driver->previousDirection) {
		POSITION_PID_CONTROLLER_get_position_output(
				driver->positionPIDController,
				driver->previousPositionDifference, duty_cycle);
	} else {
		POSITION_PID_CONTROLLER_get_position_output(
				driver->positionPIDController, pos_diff, duty_cycle);
	}

	int16_t dist_traveled = MOTOR_POSITION_get_distance_traveled(
			driver->motorPosition);

	PWM_DRIVER_set_duty_pct(driver->pwmRightMotor, duty_cycle[0]);
	PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor, duty_cycle[1]);

	enableMotors(driver);
	while (dist_traveled < dist_converted) {
		usleep(SAMPLE_PER);
		pos_diff = MOTOR_POSITION_get_position_difference(
				driver->motorPosition);

		POSITION_PID_CONTROLLER_get_position_output(
				driver->positionPIDController, pos_diff, duty_cycle);

		PWM_DRIVER_set_duty_pct(driver->pwmRightMotor, duty_cycle[0]);
		PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor, duty_cycle[1]);

		dist_traveled = MOTOR_POSITION_get_distance_traveled(
				driver->motorPosition);
	}

	driver->previousPositionDifference = pos_diff;

	disableMotors(driver);

}

/*
 * BOT_DRIVER_turn(BotDriver* driver, double arclength)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       arclength: Length of arc in cm for wheels to follow during turn
 *
 * Return:
 *       void
 *
 * Description:
 *       Drive motors given arclength using speed control (motors will turn at
 *       same rate throughout the turn)
 */
void BOT_DRIVER_turn(BotDriver* driver, double arclength) {
	int16_t dist_converted = (int16_t) (arclength * 9.4); // cm to sens edges

	int motor_speed[2];
	MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);

	int16_t motor_pos[2];
	MOTOR_POSITION_get_positions(driver->motorPosition, motor_pos);

	double duty_cycle[2];
	int speedTarget = 50;

	SPEED_PID_CONTROLLER_get_speed_output(driver->speedPIDController, speedTarget,
			motor_speed, duty_cycle);

	enableMotors(driver);

	while (motor_pos[0] < dist_converted || motor_pos[1] < dist_converted) {
		int16_t dist_to_targ = dist_converted
				- (motor_pos[0] + motor_pos[1]) / 2;
		speedTarget = 50;
		if (dist_to_targ < 50) {
			speedTarget = 25 + 0.5 * dist_to_targ;
		}
		usleep(SAMPLE_PER);
		MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);

		SPEED_PID_CONTROLLER_get_speed_output(driver->speedPIDController,
				speedTarget, motor_speed, duty_cycle);

		if (motor_pos[0] >= dist_converted) {
			PWM_DRIVER_set_duty_pct(driver->pwmRightMotor, 0.0);
		} else {
			PWM_DRIVER_set_duty_pct(driver->pwmRightMotor, duty_cycle[0]);
		}
		if (motor_pos[1] >= dist_converted) {
			PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor, 0.0);
		} else {
			PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor, duty_cycle[0]);
		}
		MOTOR_POSITION_get_positions(driver->motorPosition, motor_pos);
	}

	disableMotors(driver);
}

/*
 * void BOT_DRIVER_swing_turn(BotDriver* driver, double arclength, int dir)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *       arclength: Length of arc in cm for wheels to follow during turn
 *       dir:       Direction of turn LEFT, RIGHT
 *
 * Return:
 *       void
 *
 * Description:
 *       Drive one motor (determined by dir parameter) the given arclength using
 *       speed control (motor will turn at constant rate throughout the turn)
 */
void BOT_DRIVER_swing_turn(BotDriver* driver, double arclength,
		TurnDirection dir) {
	int16_t dist_converted = (int16_t) (arclength * 9.4); // cm to sens edges

	int motor_speed[2];
	MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);

	int16_t motor_pos[2];
	MOTOR_POSITION_get_positions(driver->motorPosition, motor_pos);

	double duty_cycle[2];
	int speedTarget = 40;
	SPEED_PID_CONTROLLER_get_speed_output(driver->speedPIDController,
					speedTarget, motor_speed, duty_cycle);

	enableMotors(driver);
	while (motor_pos[0] < dist_converted && motor_pos[1] < dist_converted) {
		usleep(SAMPLE_PER);
		MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);

		SPEED_PID_CONTROLLER_get_speed_output(driver->speedPIDController,
						speedTarget, motor_speed, duty_cycle);

		if (dir == RIGHT) {
			PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor, duty_cycle[0]);
			PWM_DRIVER_set_duty_pct(driver->pwmRightMotor,  0.0);
		} else {
			PWM_DRIVER_set_duty_pct(driver->pwmLeftMotor, 0.0);
			PWM_DRIVER_set_duty_pct(driver->pwmRightMotor, duty_cycle[1]);
		}
		MOTOR_POSITION_get_positions(driver->motorPosition, motor_pos);
	}
	disableMotors(driver);
}

/*
 * void BOT_DRIVER_delay_until_stop(BotDriver* driver)
 * ------------------------------------------------------------------------
 * Parameters:
 *       driver: Bot Driver to use with its actual state
 *
 * Return:
 *       void
 *
 * Description:
 *       Wait until both motors have stopped turning
 */
void BOT_DRIVER_delay_until_stop(BotDriver* driver) {

	MOTOR_POSITION_clear_speed_counters(driver->motorPosition);

	int motor_speed[2];
	MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);
	while (motor_speed[0] + motor_speed[1]) {
		for (int i = 0; i < 3; i++) {
			while (motor_speed[0] + motor_speed[1]) {
				usleep(SAMPLE_PER);
				MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);
			}
			usleep(SAMPLE_PER);
		}
		MOTOR_POSITION_get_speeds(driver->motorPosition, motor_speed);
	}
}

void disableMotors(BotDriver* driver) {
	PWM_DRIVER_disable(driver->pwmLeftMotor);
	PWM_DRIVER_disable(driver->pwmRightMotor);
}


void enableMotors(BotDriver* driver) {
	PWM_DRIVER_enable(driver->pwmLeftMotor);
	PWM_DRIVER_enable(driver->pwmRightMotor);
}


void resetErrors(BotDriver* driver) {
	SPEED_PID_CONTROLLER_reset_errors(driver->speedPIDController);
	POSITION_PID_CONTROLLER_reset_errors(driver->positionPIDController);
}
