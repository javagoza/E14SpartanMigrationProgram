/**
 *
 * position_pid_control.h: Position PID controller for the Bot.
 *
 *
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */


#ifndef POSITION_PID_CONTROLLER_H
#define POSITION_PID_CONTROLLER_H

typedef struct PositionPIDController {
	double KProportional;
	double KIntegral;
	double KDerivative;
	int accumulated_error;
	int previous_error;
	double baseDutyCyclePct;
} PositionPIDController;

/************ Function Prototypes ************/

void POSITION_PID_CONTROLLER_init(PositionPIDController * controller,
		double KProportional, double KIntegral, double KDerivative,
		double baseDutyCyclePct);

void POSITION_PID_CONTROLLER_get_position_output(PositionPIDController * controller ,int pos_diff, double duty_cycle[]);

void POSITION_PID_CONTROLLER_reset_errors(PositionPIDController * controller);

#endif //POSITION_PID_CONTROLLER_H
