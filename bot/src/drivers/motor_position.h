/**
 *
 * motor_positon.h: Motor Position Driver for the Bot.
 *
 * This driver gets data for the magnetic sensor tachometers
 *
 * Author: Enrique Albertos
 * Date: 2022/07/03
 *
 */

#ifndef MOTOR_POSITION_H
#define MOTOR_POSITION_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xil_io.h"
#include "xstatus.h"

#define MOTOR_POSITION_S00_AXI_SLV_REG0_OFFSET 0
#define MOTOR_POSITION_S00_AXI_SLV_REG1_OFFSET 4
#define MOTOR_POSITION_S00_AXI_SLV_REG2_OFFSET 8
#define MOTOR_POSITION_S00_AXI_SLV_REG3_OFFSET 12
#define MOTOR_POSITION_S00_AXI_SLV_REG4_OFFSET 16

#define MOTOR_POSITION_CLEAR_OFFSET     MOTOR_POSITION_S00_AXI_SLV_REG0_OFFSET
#define MOTOR_POSITION_M1_POS_OFFSET    MOTOR_POSITION_S00_AXI_SLV_REG1_OFFSET
#define MOTOR_POSITION_M2_POS_OFFSET    MOTOR_POSITION_S00_AXI_SLV_REG2_OFFSET
#define MOTOR_POSITION_POS_DIFF_OFFSET  MOTOR_POSITION_S00_AXI_SLV_REG3_OFFSET
#define MOTOR_POSITION_CLK_OFFSET       MOTOR_POSITION_S00_AXI_SLV_REG4_OFFSET


typedef struct MotorPosition {
   u32 baseAddr;
   u32 clkFreqHz;
   u32 edgesPerRev;
   u32 gearboxRatio;
} MotorPosition;


/**************************** Type Definitions *****************************/
/**
 *
 * Write a value to a MOTOR_POSITION register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the MOTOR_POSITIONdevice.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void MOTOR_POSITION_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define MOTOR_POSITION_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a MOTOR_POSITION register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the MOTOR_POSITION device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	u32 MOTOR_POSITION_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define MOTOR_POSITION_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

/************************** Function Prototypes ****************************/
/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the MOTOR_POSITION instance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-test code passed
 *    - XST_FAILURE   if any self-test code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self test may fail if data memory and device are not on the same bus.
 *
 */
XStatus MOTOR_POSITION_Reg_SelfTest(void * baseaddr_p);

void MOTOR_POSITION_init(MotorPosition* motorPosition, u32 baseAddr,
      u32 clkFreqHz, u32 edgesPerRev, u32 gearboxRatio);

void MOTOR_POSITION_get_speeds(MotorPosition* motorPosition, int motor_speed[]);

int16_t MOTOR_POSITION_get_distance_traveled(MotorPosition* motorPosition);

void MOTOR_POSITION_get_edge_counts(MotorPosition* motorPosition, int m1[],
      int m2[]);

void MOTOR_POSITION_clear_speed_counters(MotorPosition* motorPosition) ;

void MOTOR_POSITION_get_positions(MotorPosition* motorPosition,
      int16_t motor_pos[]);

int16_t MOTOR_POSITION_get_position_difference(MotorPosition* motorPosition);

void MOTOR_POSITION_clear_pos_counter(MotorPosition* motorPosition);

#endif // MOTOR_POSITION_H
