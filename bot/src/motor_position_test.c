
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "sleep.h"
#include "PmodOLED.h"
#include "xil_cache.h"
#include "xparameters.h"

#include "drivers/motor_position.h"

PmodOLED myOled;
// To change between PmodOLED and OnBoardOLED is to change Orientation
const u8 orientation = 0x0; // Set up for Normal PmodOLED(false) vs normal
// Onboard OLED(true)
const u8 invert = 0x0; // true = whitebackground/black letters
// false = black background /white letters

MotorPosition motorPosition;

#define MOTORPOSITION_BASEADDR    XPAR_MOTORPOSITION_0_S00_AXI_BASEADDR

// Register offsets for MotorPosition's AXI GPIO registers
#define MOTORPOSITION_EDGES_PER_REVOLUTION 3
#define MOTORPOSITION_GEAR_BOX_RATIO       120

void OLED_putFloatVariable(PmodOLED *InstancePtr, int xch, int ych, char * head,
		float value, char * tail);

void OLED_putIntVariable(PmodOLED *InstancePtr, int value);

int main_motorposition() {

	xil_printf("OLED Init Started\n\r");
	OLED_Begin(&myOled, XPAR_PMODOLED_0_AXI_LITE_GPIO_BASEADDR,
	XPAR_PMODOLED_0_AXI_LITE_SPI_BASEADDR, orientation, invert);
	xil_printf("OLED Init Done\n\r");

	MOTOR_POSITION_init(&motorPosition, MOTORPOSITION_BASEADDR,
	XPAR_CPU_CORE_CLOCK_FREQ_HZ, MOTORPOSITION_EDGES_PER_REVOLUTION,
			MOTORPOSITION_GEAR_BOX_RATIO);
	init_platform();

	while (1) {
		OLED_ClearBuffer(&myOled);
		OLED_SetCursor(&myOled, 0, 0);
		OLED_PutString(&myOled, "PmodOLED");
		OLED_SetCursor(&myOled, 0, 1);
		OLED_putIntVariable(&myOled,32789);
		sleep(1);

	}

	cleanup_platform();
	return 0;
}

/*
 * Displays on the OLED a float variable with a header and a tail
 * ie T: 12.67 C
 * OLED_putFloatVariable(&myOled, 1, 1, "T: ", temp_degc, " C");
 */
void OLED_putFloatVariable(PmodOLED *InstancePtr, int xch, int ych, char * head,
		float value, char * tail) {
	OLED_SetCursor(InstancePtr, xch, ych);
	OLED_PutString(InstancePtr, head);
	OLED_PutChar(InstancePtr, ((int) value / 10) % 10 + 48);
	OLED_PutChar(InstancePtr, ((int) value) % 10 + 48);
	OLED_PutString(InstancePtr, ".");
	OLED_PutChar(InstancePtr, ((int) (value * 10)) % 10 + 48);
	OLED_PutChar(InstancePtr, ((int) (value * 100)) % 10 + 48);
	OLED_PutString(InstancePtr, tail);
}

void OLED_putIntVariable(PmodOLED *InstancePtr, int value) {
	OLED_PutChar(InstancePtr, ((int) value / 100000) % 10 + 48);
	OLED_PutChar(InstancePtr, ((int) value / 10000) % 10 + 48);
	OLED_PutChar(InstancePtr, ((int) value / 1000) % 10 + 48);
	OLED_PutChar(InstancePtr, ((int) value / 100) % 10 + 48);
	OLED_PutChar(InstancePtr, ((int) value / 10) % 10 + 48);
	OLED_PutChar(InstancePtr, ((int) value) % 10 + 48);
}
