/******************************************************************************/
/*                                                                            */
/* main.c -- Environmental Monitor. Temperature / Humidity                    */
/*  PmodOLED IP + PmodHYGRO                                                   */
/*                                                                            */
/******************************************************************************/
/* Author: Enrique Albertos                                                   */
/* 2022 Public Domain                                                         */
/******************************************************************************/
/* File Description:                                                          */
/*                                                                            */
/* This project initializes and uses the PmodOLED and the PmodHYGRO to        */
/* display temperature and humidity values.                          */
/* Temperature and humidity data are captured from the I2C connection and     */
/* displayed over the serial connection and on the OLED display once per      */
/* second.                                                                    */
/* This application configures UART 16550 to Baud rate 9600. PS7 UART (Zynq)  */
/* is not initialized by this application, since bootrom/bsp configures it to */
/* Baud rate 115200.                                                          */
/*                                                                            */
/*    UART TYPE   BAUD RATE                                                   */
/*    uartns550   9600                                                        */
/*    uartlite    Configurable only in HW design                              */
/*    ps7_uart    115200 (configured by bootrom/bsp)                          */
/*                                                                            */
/******************************************************************************/
/* Revision History:                                                          */
/*                                                                            */
/*    11/06/2022 Enrique Albertos:   Created w/ Vivado 2021.1                 */
/*                                                                            */
/******************************************************************************/
/* Baud Rates:                                                                */
/*                                                                            */
/*    Microblaze: 9600 or what was specified in UARTlite core                 */
/*    Zynq: 115200                                                            */
/*                                                                            */
/******************************************************************************/

/* ------------------------------------------------------------ */
/*                  Include File Definitions                    */
/* ------------------------------------------------------------ */

#include "PmodOLED.h"
#include "PmodHYGRO.h"
#include "sleep.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xil_types.h"

#ifdef __MICROBLAZE__
#define TIMER_FREQ_HZ XPAR_CPU_M_AXI_DP_FREQ_HZ
#else
#define TIMER_FREQ_HZ 100000000
#endif

/* ------------------------------------------------------------ */
/*                  Global Variables                            */
/* ------------------------------------------------------------ */

PmodOLED myOled;
PmodHYGRO myHygro;

/* ------------------------------------------------------------ */
/*                  Forward Declarations                        */
/* ------------------------------------------------------------ */

void DemoInitialize();
void DemoRun();
void DemoCleanup();
void EnableCaches();
void DisableCaches();
void OLED_putFloatVariable(PmodOLED *InstancePtr, int xch, int ych, char * head,
		float value, char * tail);

// To change between PmodOLED and OnBoardOLED is to change Orientation
const u8 orientation = 0x0; // Set up for Normal PmodOLED(false) vs normal
// Onboard OLED(true)
const u8 invert = 0x0; // true = whitebackground/black letters
// false = black background /white letters

#define BTN_MASK 0b1111
#define LED_MASK 0b1111
#define BTN1_MASK 0b0001
#define BTN2_MASK 0b0010
#define BTN3_MASK 0b0100
#define BTN4_MASK 0b1000
#define LED1_MASK 0b0001
#define LED2_MASK 0b0010
#define LED3_MASK 0b0100
#define LED4_MASK 0b1000
#define CELSIUS_LED LED1_MASK
#define FAHRENHEIT_LED LED1_MASK

// Get device IDs from xparameters.h
#define BTN_ID XPAR_AXI_GPIO_0_DEVICE_ID
#define LED_ID XPAR_AXI_GPIO_0_DEVICE_ID
#define BTN_CHANNEL 2
#define LED_CHANNEL 1

XGpio_Config *cfg_ptr;
XGpio led_device, btn_device;



/* ------------------------------------------------------------ */
/*                  Function Definitions                        */
/* ------------------------------------------------------------ */

int main() {
	DemoInitialize();
	DemoRun();
	DemoCleanup();

	return 0;
}

void DemoInitialize() {
	EnableCaches();

	xil_printf("HYGRO Init Started\n\r");
	HYGRO_begin(&myHygro,
	XPAR_PMODHYGRO_0_AXI_LITE_IIC_BASEADDR, 0x40, // Chip address of PmodHYGRO IIC
			XPAR_PMODHYGRO_0_AXI_LITE_TMR_BASEADDR,
			XPAR_PMODHYGRO_0_DEVICE_ID,
			TIMER_FREQ_HZ // Clock frequency of AXI bus, used to convert timer data
			);
	xil_printf("HYGRO Init Done\n\r");

	xil_printf("OLED Init Started\n\r");
	OLED_Begin(&myOled, XPAR_PMODOLED_0_AXI_LITE_GPIO_BASEADDR,
	XPAR_PMODOLED_0_AXI_LITE_SPI_BASEADDR, orientation, invert);
	xil_printf("OLED Init Done\n\r");

	// Initialize LED Device
	cfg_ptr = XGpio_LookupConfig(LED_ID);
	XGpio_CfgInitialize(&led_device, cfg_ptr, cfg_ptr->BaseAddress);

	// Initialize Button Device
	cfg_ptr = XGpio_LookupConfig(BTN_ID);
	XGpio_CfgInitialize(&btn_device, cfg_ptr, cfg_ptr->BaseAddress);

	// Set Button Tristate
	XGpio_SetDataDirection(&btn_device, BTN_CHANNEL, BTN_MASK);

	// Set Led Tristate
	XGpio_SetDataDirection(&led_device, LED_CHANNEL, 0);

}

/* ------------------------------------------------------------ */
/*** DemoRun()
 **
 **   Parameters:
 **      none
 **
 **   Return Value:
 **      none
 **
 **   Errors:
 **      If the demo is shut down without properly exiting, does not reinitialize
 **      properly.
 **
 **   Description:
 **      Displays Temperature each 1 second.
 **      Toggles temp units with button 1 pressed
 **      Exists when button 4 is pressed.
 **      Requires UART connection to terminal program on PC.
 */
void DemoRun() {
	long timeCounterMillis;
	u32 buttonsState;
	int button1State;
	int lastbutton1State;
	float temp_degc, hum_perrh, temp_degf;
	u8 bDegreesCelsius = 1;

	xil_printf("UART, I2C and SPI opened for Environment Monitor\n\r");
	XGpio_DiscreteWrite(&led_device, LED_CHANNEL, LED1_MASK);
	while (1) {
        // read buttons
		buttonsState = XGpio_DiscreteRead(&btn_device, BTN_CHANNEL);

        //if button 4 is pressed exit
        if(buttonsState & BTN4_MASK ) {
        	xil_printf("exiting\r\n");
        	// exit
        	break;
        }

        // check button for toggling temperature units
        button1State = buttonsState & BTN1_MASK;
		//if button 1 low to high edge
		if((button1State != lastbutton1State) && button1State && timeCounterMillis > 100) {
        	xil_printf("toggle units\r\n");
        	// toggle temp units mode
        	bDegreesCelsius = !bDegreesCelsius;
        	// change led indicator
        	if (bDegreesCelsius ) {
        		XGpio_DiscreteWrite(&led_device, LED_CHANNEL, LED1_MASK);
        	} else {
        		XGpio_DiscreteWrite(&led_device, LED_CHANNEL, LED2_MASK);
        	}
        	// force environmental data update
        	timeCounterMillis = 1001;
        }
		lastbutton1State = button1State;

        if( timeCounterMillis > 1000) {
        	timeCounterMillis = 0;
			temp_degc = HYGRO_getTemperature(&myHygro);
			temp_degf = HYGRO_tempC2F(temp_degc);
			hum_perrh = HYGRO_getHumidity(&myHygro);
			xil_printf("Temperature: %d.%02d deg F  Humidity: %d.%02d RH\n\r",
					(int) temp_degf, ((int) (temp_degf * 100)) % 100,
					(int) hum_perrh, ((int) (hum_perrh * 100)) % 100);

			// Turn automatic updating off
			OLED_SetCharUpdate(&myOled, 0);

			if(bDegreesCelsius) {
				// Display Temperature Value in Degrees Celsius
				OLED_putFloatVariable(&myOled, 1, 1, "T: ", temp_degc, " C");
			} else {
				// Display Temperature Value in Degrees Fahrenheit
				OLED_putFloatVariable(&myOled, 1, 1, "T: ", temp_degf, " F");
			}
			// Display Relative Humidity Percentage
			OLED_putFloatVariable(&myOled, 1, 3, "H: ", hum_perrh, " %RH");
			// Update display
			OLED_Update(&myOled);
        }

		usleep(1000);
		timeCounterMillis++;
	}
	xil_printf("Exiting PmodOLED Demo\n\r");
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

void DemoCleanup() {
	OLED_End(&myOled);
	DisableCaches();
}

void EnableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
	Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
	Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
	Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
	Xil_ICacheDisable();
#endif
#endif
}

