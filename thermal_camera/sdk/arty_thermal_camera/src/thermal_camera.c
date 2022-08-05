/**
 * Example of using the Digilent display drivers for Digilent Arty S7, with animation
 */

#include <stdio.h>
#include "xil_types.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "zybo_vga/display_ctrl.h"
#include "mlx90640_api.h"
#include "platform.h"

#include "xiic.h"
#include "xintc.h"
#include "xil_exception.h"
#include "sleep.h"

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define IIC_DEVICE_ID		XPAR_IIC_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define IIC_INTR_ID	XPAR_INTC_0_IIC_0_VEC_ID

// Frame size (based on 1440x900 resolution, 32 bits per pixel)
#define MAX_FRAME (800*600)
#define FRAME_STRIDE (800*4)


DisplayCtrl dispCtrl; // Display driver struct
u32 frameBuf[DISPLAY_NUM_FRAMES][MAX_FRAME]; // Frame buffers for video data
void *pFrames[DISPLAY_NUM_FRAMES]; // Array of pointers to the frame buffers

XIic IicInstance; /* The instance of the IIC device */

XIntc InterruptController; /* The instance of the Interrupt controller */

#define IIC_SLAVE_ADDR		0x33
#define IIC_SCLK_RATE		100000

volatile u8 TransmitComplete;
volatile u8 ReceiveComplete;

#define WIDTH 32
#define HEIGHT 24

#define TEST_BUFFER_SIZE	512
#define TA_SHIFT 8

/************************** Function Prototypes ******************************/

int IicRepeatedStartExample();

static int SetupInterruptSystem(XIic *IicInstPtr);
static void ReceiveHandler(XIic *InstancePtr);
static void SendHandler(XIic *InstancePtr);

static void StatusHandler(XIic *InstancePtr, int Event);

void VGA_Fill_Color(uint16_t color);
int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress,
		uint16_t nMemAddressRead, uint16_t *data);
int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data);
void VGA_Fill_Display(float *mlx90640Frame);
void VGA_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
long map(long x, long in_min, long in_max, long out_min, long out_max);

u8 SendBuffer[TEST_BUFFER_SIZE];    //I2C TX
u8 RecvBuffer[TEST_BUFFER_SIZE];    //I2C RX

u16 frame[WIDTH][HEIGHT];

const u32 camColors[] = { 0x480F, 0x400F, 0x400F, 0x400F, 0x4010, 0x3810,
		0x3810, 0x3810, 0x3810, 0x3010, 0x3010, 0x3010, 0x2810, 0x2810, 0x2810,
		0x2810, 0x2010, 0x2010, 0x2010, 0x1810, 0x1810, 0x1811, 0x1811, 0x1011,
		0x1011, 0x1011, 0x0811, 0x0811, 0x0811, 0x0011, 0x0011, 0x0011, 0x0011,
		0x0011, 0x0031, 0x0031, 0x0051, 0x0072, 0x0072, 0x0092, 0x00B2, 0x00B2,
		0x00D2, 0x00F2, 0x00F2, 0x0112, 0x0132, 0x0152, 0x0152, 0x0172, 0x0192,
		0x0192, 0x01B2, 0x01D2, 0x01F3, 0x01F3, 0x0213, 0x0233, 0x0253, 0x0253,
		0x0273, 0x0293, 0x02B3, 0x02D3, 0x02D3, 0x02F3, 0x0313, 0x0333, 0x0333,
		0x0353, 0x0373, 0x0394, 0x03B4, 0x03D4, 0x03D4, 0x03F4, 0x0414, 0x0434,
		0x0454, 0x0474, 0x0474, 0x0494, 0x04B4, 0x04D4, 0x04F4, 0x0514, 0x0534,
		0x0534, 0x0554, 0x0554, 0x0574, 0x0574, 0x0573, 0x0573, 0x0573, 0x0572,
		0x0572, 0x0572, 0x0571, 0x0591, 0x0591, 0x0590, 0x0590, 0x058F, 0x058F,
		0x058F, 0x058E, 0x05AE, 0x05AE, 0x05AD, 0x05AD, 0x05AD, 0x05AC, 0x05AC,
		0x05AB, 0x05CB, 0x05CB, 0x05CA, 0x05CA, 0x05CA, 0x05C9, 0x05C9, 0x05C8,
		0x05E8, 0x05E8, 0x05E7, 0x05E7, 0x05E6, 0x05E6, 0x05E6, 0x05E5, 0x05E5,
		0x0604, 0x0604, 0x0604, 0x0603, 0x0603, 0x0602, 0x0602, 0x0601, 0x0621,
		0x0621, 0x0620, 0x0620, 0x0620, 0x0620, 0x0E20, 0x0E20, 0x0E40, 0x1640,
		0x1640, 0x1E40, 0x1E40, 0x2640, 0x2640, 0x2E40, 0x2E60, 0x3660, 0x3660,
		0x3E60, 0x3E60, 0x3E60, 0x4660, 0x4660, 0x4E60, 0x4E80, 0x5680, 0x5680,
		0x5E80, 0x5E80, 0x6680, 0x6680, 0x6E80, 0x6EA0, 0x76A0, 0x76A0, 0x7EA0,
		0x7EA0, 0x86A0, 0x86A0, 0x8EA0, 0x8EC0, 0x96C0, 0x96C0, 0x9EC0, 0x9EC0,
		0xA6C0, 0xAEC0, 0xAEC0, 0xB6E0, 0xB6E0, 0xBEE0, 0xBEE0, 0xC6E0, 0xC6E0,
		0xCEE0, 0xCEE0, 0xD6E0, 0xD700, 0xDF00, 0xDEE0, 0xDEC0, 0xDEA0, 0xDE80,
		0xDE80, 0xE660, 0xE640, 0xE620, 0xE600, 0xE5E0, 0xE5C0, 0xE5A0, 0xE580,
		0xE560, 0xE540, 0xE520, 0xE500, 0xE4E0, 0xE4C0, 0xE4A0, 0xE480, 0xE460,
		0xEC40, 0xEC20, 0xEC00, 0xEBE0, 0xEBC0, 0xEBA0, 0xEB80, 0xEB60, 0xEB40,
		0xEB20, 0xEB00, 0xEAE0, 0xEAC0, 0xEAA0, 0xEA80, 0xEA60, 0xEA40, 0xF220,
		0xF200, 0xF1E0, 0xF1C0, 0xF1A0, 0xF180, 0xF160, 0xF140, 0xF100, 0xF0E0,
		0xF0C0, 0xF0A0, 0xF080, 0xF060, 0xF040, 0xF020, 0xF800, };

int main(void) {
	init_platform();

	int Status;

	XIic_Config *ConfigPtr; /* Pointer to configuration data */

	init_platform();

	ConfigPtr = XIic_LookupConfig(XPAR_IIC_0_DEVICE_ID);
	if (ConfigPtr == NULL) {
		return XST_FAILURE;
	}
	// print("XIic_LookupConfig\n\r");

	Status = XIic_CfgInitialize(&IicInstance, ConfigPtr,
			ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// print("XIic_CfgInitialize\n\r");

	/*
	 * Setup the Interrupt System.
	 */
	Status = SetupInterruptSystem(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// print("SetupInterruptSystem\n\r");

	/*
	 * Set the Transmit, Receive and Status handlers.
	 */
	XIic_SetSendHandler(&IicInstance, &IicInstance, (XIic_Handler) SendHandler);
	// print("XIic_SetSendHandler\n\r");

	XIic_SetStatusHandler(&IicInstance, &IicInstance,
			(XIic_StatusHandler) StatusHandler);
	// print("XIic_SetStatusHandler\n\r");

	XIic_SetRecvHandler(&IicInstance, &IicInstance,
			(XIic_Handler) ReceiveHandler);

	// print("XIic_SetRecvHandler\n\r");

	/*
	 * Set the Address of the Slave.
	 */
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE,
	IIC_SLAVE_ADDR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	// print("XIic_SetAddress\n\r");

	/*
	 * Start the IIC device.
	 */
	Status = XIic_Start(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// print("XIic_Start\n\r");

	static u16 mlx90640Frame[834];
	static uint16_t eeMLX90640[832];
	paramsMLX90640 mlx90640;

	float Ta;
	float emissivity = 0.95;
	static float mlx90640To[768];

	MLX90640_DumpEE(0x33, eeMLX90640);
	// print("MLX90640_DumpEE\n\r");
	MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
	// print("MLX90640_ExtractParameters\n\r");

	xil_printf("Successfully started vga example\r\n");

	// Initialise an array of pointers to the 2 frame buffers
	int i;
	for (i = 0; i < DISPLAY_NUM_FRAMES; i++)
		pFrames[i] = frameBuf[i];

	// Initialise the display controller
	DisplayInitialize(&dispCtrl, XPAR_AXIVDMA_0_DEVICE_ID, XPAR_VTC_0_DEVICE_ID,
	XPAR_VGA_AXI_DYNCLK_0_BASEADDR, pFrames, FRAME_STRIDE);

	// Start with the first frame buffer (of two)
	DisplayChangeFrame(&dispCtrl, 0);

	// Set the display resolution
	DisplaySetMode(&dispCtrl, &VMODE_800x600);

	// Enable video output
	DisplayStart(&dispCtrl);

	// Get parameters from display controller struct
	int x, y;
	u32 stride = dispCtrl.stride / 4;
	u32 width = dispCtrl.vMode.width;
	u32 height = dispCtrl.vMode.height;

	u32 *frame;

	u32 buff = dispCtrl.curFrame;

	while (1) {

		MLX90640_GetFrameData(0x33, mlx90640Frame);
		// print("MLX90640_GetFrameData\n\r");
		Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT;
		// print("MLX90640_GetTa\n\r");
		MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, Ta,
				mlx90640To);

		float maxTemp = 0.0;
		float minTemp = 9999.99;
		for (int i = 0; i < 768; ++i) {
			if (mlx90640To[i] < minTemp) {
				minTemp = mlx90640To[i];
			}
			if (mlx90640To[i] > maxTemp) {
				maxTemp = mlx90640To[i];
			}
		}
		xil_printf("MAX Temp: %d.%d, MIN Temp %d.%d\n\r",(int)maxTemp, ((int)(maxTemp*100))%100,(int)minTemp, ((int)(minTemp*100))%100);

		// Switch the frame we're modifying to be back buffer (1 to 0, or 0 to 1)
		buff = !buff;
		frame = dispCtrl.framePtr[buff];

		// Clear the frame to white
		// memset(frame, 0xFF, MAX_FRAME * 4);

		float temp;
		u8 mapped;
		u32 colour;
		int scale = 24;
		int frameWidth = WIDTH * scale;
		int frameHeight = HEIGHT * scale;

		int yo = (height - frameHeight) / 2 ;
		int xo = (width - frameWidth) /2;

		for (y = 0; y < frameHeight; y++)
			for (x = 0; x < frameWidth; x++) {

				temp = (mlx90640To[x / scale + y / scale * 32]);
				mapped = map((u16) temp, minTemp, maxTemp, 0, 255);
				colour = (((camColors[mapped] >> 12) & 0x0F)
						<< (BIT_DISPLAY_RED + 4))
						| (((camColors[mapped] >> 7) & 0x0F)
								<< (BIT_DISPLAY_GREEN + 4))
						| ((camColors[mapped] >> 1) & 0x0F)
								<< (BIT_DISPLAY_BLUE + 4);

				frame[(y+yo) * stride + x + xo] = colour;

			}

		// Flush everything out to DDR
		Xil_DCacheFlush()
		;

		// Switch active frame to the back buffer
		DisplayChangeFrame(&dispCtrl, buff);

		// Wait for the frame to switch (after active frame is drawn) before continuing
		DisplayWaitForSync(&dispCtrl);
	}

	/*
	 * Stop the IIC device.
	 */
	Status = XIic_Stop(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	cleanup_platform();

	return 0;
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress,
		uint16_t nMemAddressRead, uint16_t *data) {

	// print("MLX90640_I2CRead\n\r");
	int Status;
	//int BusBusy;

	/*
	 * Set the defaults.
	 */
	ReceiveComplete = 1;

	/*
	 * Set the Repeated Start option.
	 */
	IicInstance.Options = XII_REPEATED_START_OPTION;

	int cnt = 0;
	int i = 0;
	u8 cmd[2] = { 0, 0 };
	u8 i2cData[1664] = { 0 };
	uint16_t *p;

	p = data;
	cmd[0] = startAddress >> 8;
	cmd[1] = startAddress & 0x00FF;

	Status = XIic_MasterSend(&IicInstance, cmd, 2);
	if (Status != XST_SUCCESS) {

		return XST_FAILURE;
	}
	// print("XIic_MasterSend\n\r");
	usleep(1000);

	/*
	 * This is for verification that Bus is not released and still Busy.
	 */
	// BusBusy = XIic_IsIicBusy(&IicInstance);

	ReceiveComplete = 1;
	IicInstance.Options = 0x0;

	/*
	 * Receive the Data.
	 */
	Status = XIic_MasterRecv(&IicInstance, i2cData, 2 * nMemAddressRead);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	usleep(1000);
	// print("XIic_MasterRecv\n\r");

	while (XIic_IsIicBusy(&IicInstance) == TRUE) {

	}

	for (cnt = 0; cnt < nMemAddressRead; cnt++) {
		i = cnt << 1;
		*p++ = (uint16_t) i2cData[i] * 256 + (uint16_t) i2cData[i + 1];
	}

	return 0;
}

int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data) {

	int Status;
	//int BusBusy;

	/*
	 * Set the defaults.
	 */
	TransmitComplete = 1;

	/*
	 * Set the Repeated Start option.
	 */
	IicInstance.Options = XII_REPEATED_START_OPTION;

	u8 cmd[4] = { 0, 0, 0, 0 };
	static uint16_t dataCheck;

	cmd[0] = writeAddress >> 8;
	cmd[1] = writeAddress & 0x00FF;
	cmd[2] = data >> 8;
	cmd[3] = data & 0x00FF;

	/*
	 * Send the data.
	 */
	Status = XIic_MasterSend(&IicInstance, cmd, 4);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	usleep(1000);
	//print("XIic_MasterSend\n\r");

	/*
	 * Wait till data is transmitted.
	 */
//	while (TransmitComplete) {
//
//	}
	/*
	 * This is for verification that Bus is not released and still Busy.
	 */
	//BusBusy = XIic_IsIicBusy(&IicInstance);

	TransmitComplete = 1;
	IicInstance.Options = 0x0;

	/*
	 * Wait till data is transmitted.
	 */
//	while ((TransmitComplete) || (XIic_IsIicBusy(&IicInstance) == TRUE)) {
//
//	}
	MLX90640_I2CRead(slaveAddr, writeAddress, 1, &dataCheck);

	if (dataCheck != data) {
		return -2;
	}

	return 0;
}

/*****************************************************************************/
/**
 * This function setups the interrupt system so interrupts can occur for the
 * IIC. The function is application-specific since the actual system may or
 * may not have an interrupt controller. The IIC device could be directly
 * connected to a processor without an interrupt controller. The user should
 * modify this function to fit the application.
 *
 * @param	IicInstPtr contains a pointer to the instance of the IIC  which
 *		is going to be connected to the interrupt controller.
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 * @note		None.
 *
 ******************************************************************************/
static int SetupInterruptSystem(XIic *IicInstPtr) {
	int Status;

	if (InterruptController.IsStarted == XIL_COMPONENT_IS_STARTED) {
		return XST_SUCCESS;
	}

	/*
	 * Initialize the interrupt controller driver so that it's ready to use.
	 */
	Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 *  the specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(&InterruptController, IIC_INTR_ID,
			(XInterruptHandler) XIic_InterruptHandler, IicInstPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the interrupt controller so interrupts are enabled for all
	 * devices that cause interrupts.
	 */
	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Enable the interrupts for the IIC device.
	 */
	XIntc_Enable(&InterruptController, IIC_INTR_ID);

	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XIntc_InterruptHandler,
			&InterruptController);

	/*
	 * Enable non-critical exceptions.
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This Send handler is called asynchronously from an interrupt context and
 * indicates that data in the specified buffer has been sent.
 *
 * @param	InstancePtr is a pointer to the IIC driver instance for which
 * 		the handler is being called for.
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/
static void SendHandler(XIic *InstancePtr) {
	TransmitComplete = 0;
}

/*****************************************************************************/
/**
 * This Status handler is called asynchronously from an interrupt
 * context and indicates the events that have occurred.
 *
 * @param	InstancePtr is a pointer to the IIC driver instance for which
 *		the handler is being called for.
 * @param	Event indicates the condition that has occurred.
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/
static void StatusHandler(XIic *InstancePtr, int Event) {

}

/*****************************************************************************/
/**
 * This Receive handler is called asynchronously from an interrupt context and
 * indicates that data in the specified buffer has been Received.
 *
 * @param	InstancePtr is a pointer to the IIC driver instance for which
 * 		the handler is being called for.
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/
static void ReceiveHandler(XIic *InstancePtr) {
	ReceiveComplete = 0;
}

