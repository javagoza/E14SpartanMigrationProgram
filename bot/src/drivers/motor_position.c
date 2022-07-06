
/**
 *
 * motor_positon.c: Motor Position Driver for the Bot.
 *
 * This driver gets data for the magnetic sensor tachometers
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */


/***************************** Include Files *******************************/
#include "motor_position.h"

/************************** Function Definitions ***************************/

void MOTOR_POSITION_init(MotorPosition* motorPosition, u32 baseAddr,
      u32 clkFreqHz, u32 edgesPerRev, u32 gearboxRatio) {
   motorPosition->baseAddr = baseAddr;
   motorPosition->clkFreqHz = clkFreqHz;
   motorPosition->edgesPerRev = edgesPerRev;
   motorPosition->gearboxRatio = gearboxRatio;
}

void MOTOR_POSITION_get_speeds(MotorPosition* motorPosition, int motor_speed[]) {
   int m1[2];
   int m2[2];
   MOTOR_POSITION_get_edge_counts(motorPosition, m1, m2);
   double conversionFactor = 60.0 * motorPosition->clkFreqHz
                                  / motorPosition->edgesPerRev
                                  / motorPosition->gearboxRatio;
   motor_speed[0] = conversionFactor * m1[0] / m1[1];
   motor_speed[1] = conversionFactor * m2[0] / m2[1];
   MOTOR_POSITION_clear_speed_counters(motorPosition);
}

int16_t MOTOR_POSITION_get_distance_traveled(MotorPosition* motorPosition) {
   int16_t motor_pos[2];
   MOTOR_POSITION_get_positions(motorPosition, motor_pos);
   return (motor_pos[0] + motor_pos[1]) / 2;
}

void MOTOR_POSITION_get_edge_counts(MotorPosition* motorPosition, int m1[],
      int m2[]) {
   u32 m1Pos_addr = motorPosition->baseAddr + MOTOR_POSITION_M1_POS_OFFSET;
   u32 m2Pos_addr = motorPosition->baseAddr + MOTOR_POSITION_M2_POS_OFFSET;
   u32 clk_addr   = motorPosition->baseAddr + MOTOR_POSITION_CLK_OFFSET;
   m1[0] = (int) (Xil_In32(m1Pos_addr) >> 16);
   m1[1] = (int)  Xil_In32(clk_addr);
   m2[0] = (int) (Xil_In32(m2Pos_addr) >> 16);
   m2[1] = (int)  Xil_In32(clk_addr);
}

void MOTOR_POSITION_clear_speed_counters(MotorPosition* motorPosition) {
   Xil_Out8(motorPosition->baseAddr + MOTOR_POSITION_CLEAR_OFFSET, 0x1);
   Xil_Out8(motorPosition->baseAddr + MOTOR_POSITION_CLEAR_OFFSET, 0x0);
}

void MOTOR_POSITION_get_positions(MotorPosition* motorPosition,
      int16_t motor_pos[]) {
   u32 m1Pos_addr = motorPosition->baseAddr + MOTOR_POSITION_M1_POS_OFFSET;
   u32 m2Pos_addr = motorPosition->baseAddr + MOTOR_POSITION_M2_POS_OFFSET;
   motor_pos[0] = Xil_In16(m1Pos_addr);
   motor_pos[1] = Xil_In16(m2Pos_addr);
}

int16_t MOTOR_POSITION_get_position_difference(MotorPosition* motorPosition) {
   u32 pos_diff_addr = motorPosition->baseAddr + MOTOR_POSITION_POS_DIFF_OFFSET;
   return (int16_t) Xil_In16(pos_diff_addr);
}

void MOTOR_POSITION_clear_pos_counter(MotorPosition* motorPosition) {
   Xil_Out8(motorPosition->baseAddr + MOTOR_POSITION_CLEAR_OFFSET, 0x2);
   Xil_Out8(motorPosition->baseAddr + MOTOR_POSITION_CLEAR_OFFSET, 0x0);
}
