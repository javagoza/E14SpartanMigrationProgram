#ifndef BOT_DRIVER_H
#define BOT_DRIVER_H

#include "motor_position.h"
#include "pwm_driver.h"
#include "speed_pid_control.h"
#include "position_pid_control.h"
#include "hbridge_driver.h"

typedef enum MoveDirection {
	IDLE, FORWARD, BACKWARD, TURN_LEFT, TURN_RIGHT
} MoveDirection;

typedef enum TurnDirection {
	LEFT = 0, RIGHT =1
} TurnDirection;

typedef struct BotDriver {
	MotorPosition*         motorPosition;
	PWMDriver*             pwmRightMotor;
	PWMDriver*             pwmLeftMotor;
	u32                    pwmPeriod_ns;
	MoveDirection          direction;
	MoveDirection          previousDirection;
	int16_t                previousPositionDifference;
	SpeedPIDController*    speedPIDController;
	PositionPIDController* positionPIDController;
	HBridgeDriver*         hbridge;
} BotDriver;

/************ Function Prototypes ************/

// High level API calls

void BOT_DRIVER_init(
		BotDriver* driver,
		MotorPosition* motorPosition,
		PWMDriver* pwmRightMotor,
		PWMDriver* pwmLeftMotor,
		u32 pwmPeriod_ns,
		SpeedPIDController* speedPIDController,
		PositionPIDController* positionPIDController,
		HBridgeDriver* hbridge);

void BOT_DRIVER_end(BotDriver* driver);

void BOT_DRIVER_drive_forward_cm(BotDriver* driver, double distanceCm);

void BOT_DRIVER_drive_backward_cm(BotDriver* driver, double distanceCm);

void BOT_DRIVER_turn_left_degrees(BotDriver* driver, int degrees);

void BOT_DRIVER_turn_right_degrees(BotDriver* driver, int degrees);

void BOT_DRIVER_drive_forward_continuous_cm(BotDriver* driver, double distanceCm);

void BOT_DRIVER_drive_backward_continuous_cm(BotDriver* driver, double distanceCm);

void BOT_DRIVER_turn_left_continuous_degrees(BotDriver* driver, int degrees);

void BOT_DRIVER_turn_right_continuous_degrees(BotDriver* driver, int degrees);

void BOT_DRIVER_swing_turn_left_degrees(BotDriver* driver, int degrees);

void BOT_DRIVER_swing_turn_right_degrees(BotDriver* driver, int degrees);

// Low level API calls

void BOT_DRIVER_set_direction_forward(BotDriver* driver);

void BOT_DRIVER_set_direction_backward(BotDriver* driver);

void BOT_DRIVER_set_direction_left(BotDriver* driver);

void BOT_DRIVER_set_direction_right(BotDriver* driver);

void BOT_DRIVER_delay_until_stop(BotDriver* driver);

void BOT_DRIVER_drive_cm(BotDriver* driver, double distanceCm);

void BOT_DRIVER_turn(BotDriver* driver, double arclength);

void BOT_DRIVER_swing_turn(BotDriver* driver, double arclength,
		TurnDirection dir);



#endif // BOT_DRIVER_H
