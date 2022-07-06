/**
 *
 * speed_pid_control.h: Speed PID controller for the Bot.
 *
 *
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */


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

void SPEED_PID_CONTROLLER_get_speed_output(SpeedPIDController * controller,
		int speedTargetRpm,
		int speedsRpm[],
		double dutyCyclePct[]);

void SPEED_PID_CONTROLLER_reset_errors(SpeedPIDController * controller);

#endif //SPEED_PID_CONTROLLER_H
