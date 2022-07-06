#include "POSITION_PID_control.h"

/*
 *
 * void POSITION_PID_CONTROLLER_init(PositionPIDController * controller,
 *		double KProportional, double KIntegral, double KDerivative,
 *		double baseDutyCycle)
 * ------------------------------------------------------------------------
 * Parameters:
 *       controller:        Speed PID controller configuration and state
 *       KProportional:     PID proportional term
 *       KIntegral:         PID Integral term
 *       KDerivative:       PID Derivative term
 *       baseDutyCyclePct:  Base duty cycle percetage
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
void POSITION_PID_CONTROLLER_init(PositionPIDController * controller,
		double KProportional, double KIntegral, double KDerivative,
		double baseDutyCyclePct) {

	controller->KProportional = KProportional;
	controller->KIntegral = KIntegral;
	controller->KDerivative = KDerivative;
	controller->baseDutyCyclePct = baseDutyCyclePct;

}


/*
 * void POSITION_PID_CONTROLLER_get_speed_output(int speed_sp, int speed[], double duty_cycle[])
 * ------------------------------------------------------------------------
 * Parameters:
 *       controller:         Speed PID controller configuration and state
 *       pos_diff  :         Current position difference between motor1 and motor2
 *       dutyCyclePct:       Array for storing new percentage duty cycles for motor1, motor2,
 *                           respectively. Values from 0 to 1
 *
 * Return:
 *       void
 *
 * Description:
 *       Uses a PID controller to compute new duty cycles for motor1 and motor2
 *       with goal of minimizing position difference to 0 and store them in
 *       duty_cycle. Assumes that this function gets called at regular time
 *       intervals
 */
void POSITION_PID_CONTROLLER_get_position_output(PositionPIDController * controller,
		int pos_diff,
		double dutyCyclePct[]) {

	   controller->accumulated_error += pos_diff;

	   double correction =   controller->KProportional *  pos_diff
	                       + controller->KIntegral *  controller->accumulated_error
	                       + controller->KDerivative * (pos_diff - controller->previous_error);

	   dutyCyclePct[0] = controller->baseDutyCyclePct;
	   dutyCyclePct[1] = controller->baseDutyCyclePct;
	   if (correction < 0) {
		   dutyCyclePct[0] -= correction; // Motor1 lagging, speed up
	   } else {
		   dutyCyclePct[1] += correction; // Motor2 lagging, speed up
	   }

	   controller->previous_error = pos_diff;

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
 * void POSITION_PID_CONTROLLER_reset_errors(PositionPIDController * controller)
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
void POSITION_PID_CONTROLLER_reset_errors(PositionPIDController * controller) {
    controller->accumulated_error = 0;
    controller->previous_error = 0;
}
