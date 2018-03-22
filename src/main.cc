/*
 * Empty C++ Application
 */

#include "../include/adv7511_setup.h"
#include "../include/vdma.h"

#include "xparameters.h"
#include "xil_cache.h"
#include "mb_interface.h"

#include "xviraymain.h"
#include "xviraymain_hw.h"
#include "xgpio.h"

#include "TextureGenerator.h"
#include "mat4.h"
#include "vec3.h"

#include "../include/myWorld.h"

/*
 * REQUIRED RESETS
 */
unsigned ObjectHandler::nextIdx = 0;
unsigned LightHandler::nextIdx 	= 0;

bool loopedExecution = true;

//#define DEBUG_PATTERN

#ifdef DEBUG_PATTERN

#define FRAME_ADDR 			OUT_COLOR_ADDR
#define PLACEHOLDER_HEIGHT 	HEIGHT
#define PLACEHOLDER_WIDTH 	WIDTH

#endif

// GLOBAL OBJECTS

XGpio_Config* gpioConf;
XGpio gpio;

XViraymain_Config *virayConfig;
XViraymain viray;

I2C ic2Obj;
VDMA vdmaObj;

int main()
{
	/*
	 * GPIO INIT
	 */
	gpioConf = XGpio_LookupConfig(XPAR_AXI_GPIO_0_DEVICE_ID);

	if (!gpioConf){
		xil_printf("GPIO: %u not found!\n\r", XPAR_AXI_GPIO_0_DEVICE_ID);
		return XST_FAILURE;
	}
	if (XGpio_CfgInitialize(&gpio, gpioConf, gpioConf->BaseAddress) != XST_SUCCESS)
	{
		xil_printf("GPIO: %u not initialized properly!\n\r", XPAR_AXI_GPIO_0_DEVICE_ID);
		return XST_FAILURE;
	}

	/*
	 * VIRAY CORE INIT
	 */
	virayConfig = XViraymain_LookupConfig(XPAR_XVIRAYMAIN_0_DEVICE_ID);
	if (virayConfig)
	{
		XViraymain_CfgInitialize(&viray, virayConfig);
	}
	else
	{
		xil_printf("Error while initializing access to the VIRAY device!\n\r");
		return XST_FAILURE;
	}

	/*
	 * ========================================================================
	 * 					VIRAY WORLD SETUP (DATA BINDINGS ALSO)
	 * ========================================================================
	 */
#ifndef DEBUG_PATTERN
	MyWorld myWorld(&viray, &gpio, 1);
#endif
	/*
	 * ========================================================================
	 * 							PIXEL DATA TRANSFER STUFF
	 * ========================================================================
	 */

	/*
	 * ADV7511 setup over I2C bus
	 */
	if (ic2Obj.Init() != XST_SUCCESS) 		return XST_FAILURE;

	/*
	 * Start VDMA engine
	 */
	if (vdmaObj.Init() != XST_SUCCESS) 		return XST_FAILURE;

	xil_printf("Starting VDMA...\n\r");
	if (vdmaObj.Start() != XST_SUCCESS)
	{
		xil_printf("Couldn't start VDMA!\n\r");
		return XST_FAILURE;
	}

	/*
	 * ========================================================================
	 * 							START RENDERING
	 * ========================================================================
	 */

#ifdef DEBUG_PATTERN
	/*
	 * Test pattern-generator
	 */

	float hInv = 1.0f / PLACEHOLDER_HEIGHT;
	float wInv = 1.0f / PLACEHOLDER_WIDTH;

	for (;;)
	{
		unsigned* p = (unsigned*) FRAME_ADDR;

		unsigned R, G, B;
		unsigned color;

		u8 readReg;
		ic2Obj.EepromReadByte((AddressType)0x3D, &readReg, 1);
		xil_printf("VIC: %X = %X\n\r", 0x3D, readReg);

		for (unsigned j = 0; j < PLACEHOLDER_HEIGHT; ++j)
		{
			G = (255 * j) * hInv;
			B = (255 * j) * hInv;

			for (unsigned i = 0; i < PLACEHOLDER_WIDTH; ++i)
			{
				R = (255 * i) * wInv;

				if (!(i & 0x1))
				{
					color = (G << 8) + B;
//					color = (B << 8) + G;
				}
				else
				{
					color = (G << 8) + R;
//					color = (R << 8) + G;
				}

				p[PLACEHOLDER_WIDTH * j + i] = color;
			}
		}
	}
#else
	/*
	 * Run real engine
	 */

	myWorld.RunViRay(loopedExecution);
#endif

	return 0;
}
