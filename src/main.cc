/*
 * Empty C++ Application
 */

#include "../include/adv7511_setup.h"
#include "../include/vdma.h"

#define FRAME_ADDR ((unsigned*)0x90000000)
#define PLACEHOLDER_HEIGHT 1080
#define PLACEHOLDER_WIDTH 1920

int main()
{
	xil_printf("ASDFAS\n\r");

	I2C ic2Obj;
	if (ic2Obj.Init() != XST_SUCCESS) 		return XST_FAILURE;

	xil_printf("1");

	VDMA vdmaObj;
	if (vdmaObj.Init() != XST_SUCCESS) 		return XST_FAILURE;

	xil_printf("2");

	xil_printf("Starting VDMA...\n\r");
	if (vdmaObj.Start() != XST_SUCCESS)
	{
		xil_printf("Couldn't start VDMA!\n\r");
		return XST_FAILURE;
	}

	xil_printf("aasd\n\r");

	float hInv = 1.0f / PLACEHOLDER_HEIGHT;
	float wInv = 1.0f / PLACEHOLDER_WIDTH;

	for (;;)
	{
		unsigned* p = (unsigned*) FRAME_ADDR;

		unsigned G; // G
		unsigned R; // R
		unsigned B; // B
		unsigned color;

		u8 readReg;
		ic2Obj.EepromReadByte((AddressType)0x3D, &readReg, 1);
		xil_printf("VIC: %X = %X\n\r", 0x3D, readReg);

		for (unsigned j = 0; j < PLACEHOLDER_HEIGHT; ++j)
		{
			G = 0;//(255 * j) * hInv;
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

	return 0;
}
