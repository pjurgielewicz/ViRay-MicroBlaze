/*
 * Empty C++ Application
 */

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

bool loopedExecution = false;

int main()
{
	XGpio_Config* gpioConf;
	XGpio gpio;

	XViraymain_Config *virayConfig;
	XViraymain viray;

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
	 * TODO: SETUP BUTTONS, I2C, VDMA...
	 */

	/*
	 * Setup the world and make all necessary memory bindings
	 */
	MyWorld myWorld(&viray, &gpio, 1);

	///////////////////////////////////////////////////////////

	/*
	 * RUN THE ENGINE
	 */
	myWorld.RunViRay(loopedExecution);

	return 0;
}
