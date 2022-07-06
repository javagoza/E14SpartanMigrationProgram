#include "speed_pid_control.h"

/*
 *
 * void SPEED_PID_CONTROLLER_init(SpeedPIDController * controller,
 *		double KProportional, double KIntegral, double KDerivative,
 *		double baseDutyCycle)
 * ------------------------------------------------------------------------
 * Parameters:
 *       controller:        Speed PID controller configuration and state
 *       KProportional:     PID proportional term
 *       KIntegral:         PID Integral term
 *       KDerivative:       PID Derivative term
 *       baseDutyCyclePct:     Base duty cycle percetage
 *
 * Return:
 *       void
 *
 * Description:
 *       Uses a PID controller to compute new duty cycles for motor1 and motor2
 *       with goal of maintaining motor speeds at speed target and store them in
 *       dutyCyclePct. Assumes that this function gets called at regular time
 *       intervals
 */
void SPEED_PID_CONTROLLER_init(SpeedPIDController * controller,
		double KProportional, double KIntegral, double KDerivative,
		double baseDutyCyclePct) {

	controller->KProportional = KProportional;
	controller->KIntegral = KIntegral;
	controller->KDerivative = KDerivative;
	controller->baseDutyCyclePct = baseDutyCyclePct;

}


/*
 * void SPEED_PID_CONTROLLER_get_speed_output(int speed_sp, int speed[], double duty_cycle[])
 * ------------------------------------------------------------------------
 * Parameters:
 *       controller:         Speed PID controller configuration and state
 *       speedTargetRpm:     Speed set point (desired speed) in RPM
 *       speedsRpm:          Array with current speeds of motor1, motor2, respectively
 *       dutyCyclePct:       Array for storing new percentage duty cycles for motor1, motor2,
 *                           respectively. Values from 0 to 1
 *
 * Return:
 *       void
 *
 * Description:
 *       Uses a PID controller to compute new duty cycles for motor1 and motor2
 *       with goal of maintaining motor speeds at speed target and store them in
 *       dutyCyclePct. Assumes that this function gets called at regular time
 *       intervals
 */
void SPEED_PID_CONTROLLER_get_speed_output(SpeedPIDController * controller,
		int speedTargetRpm,
		int speedsRpm[],
		double dutyCyclePct[]) {

	int err_m1 = speedTargetRpm - speedsRpm[0]; // Current error
	int err_m2 = speedTargetRpm - speedsRpm[1];

	controller->accumulated_error[0] += err_m1; // Accumulated error
	controller->accumulated_error[1] += err_m2;

	dutyCyclePct[0] = controller->baseDutyCyclePct;
	dutyCyclePct[1] = controller->baseDutyCyclePct;

	dutyCyclePct[0] += controller->KProportional * err_m1
			+ controller->KIntegral * controller->accumulated_error[0]
			+ controller->KDerivative * (err_m1 - controller->previous_error[0]);

	dutyCyclePct[1] += controller->KProportional * err_m2
			+ controller->KIntegral * controller->accumulated_error[1]
			+ controller->KDerivative * (err_m2 - controller->previous_error[1]);

	// Bound duty cycles between 0 and 1
	if (dutyCyclePct[0] < 0) {
		dutyCyclePct[0] = 0.0;
	} else if (dutyCyclePct[0] > 1) {
		dutyCyclePct[0] = 1.0;
	}

	if (dutyCyclePct[1] < 0) {
		dutyCyclePct[1] = 0.0;
	} else if (dutyCyclePct[1] > 1) {
		dutyCyclePct[1] = 1.0;
	}

}


/*
 * void SPEED_PID_CONTROLLER_reset_errors(SpeedPIDController * controller)
 * ------------------------------------------------------------------------
 * Parameters:
 *       controller:         Speed PID controller configuration and state
 *
 * Return:
 *       void
 *
 * Description:
 *       Reset accumulated errors and previous errors to 0
 */
void SPEED_PID_CONTROLLER_reset_errors(SpeedPIDController * controller) {
    controller->previous_error[0] = 0;
    controller->previous_error[1] = 0;

    controller->accumulated_error[0] =0;
    controller->accumulated_error[1] = 0;

}
