#include "../include/worldDescription.h"


WorldDescription::WorldDescription(XViraymain* viray, XGpio* gpio, I2C* i2c, unsigned gpioButtonsChannel) :
	camera(viray, CAMERA_ARRAY_ADDR),
	textureHelper((float_union*)TEXTURE_DATA_ADDR, (unsigned*)TEXTURE_DESCRIPTION_ADDR, (unsigned*)TEXTURE_BASE_ADDR_ADDR),
	timer(XPAR_AXI_TIMER_0_DEVICE_ID, XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ),
	viray(viray),
	gpio(gpio),
	i2c(i2c),
	gpioButtonsChannel(gpioButtonsChannel)
{
//	xil_printf("Initial clean-up\n\r");
	for (unsigned i = 0; i < LIGHTS_NUM; ++i)
	{
		lights[i] = nullptr;
	}
	for (unsigned i = 0; i < OBJ_NUM; ++i)
	{
		objects[i] = nullptr;
		*(((int*)OBJ_TYPE_ADDR) + i) = ObjectType::INVALID;
	}

	XViraymain_Set_objTransformationArrayIn(viray, (u32)OBJ_TRANSFORMATION_ARRAY_ADDR);
//	xil_printf("Transformation: %x\n\r", (u32)OBJ_TRANSFORMATION_ARRAY_ADDR);

	XViraymain_Set_objTypeIn(viray, (u32)OBJ_TYPE_ADDR);
//	xil_printf("TYPE ARRAY: %x\n\r", (u32)OBJ_TYPE_ADDR);

	XViraymain_Set_materialArrayIn(viray, (u32)MATERIAL_ARRAY_ADDR);
//	xil_printf("MATERIAL ARRAY: %x\n\r", (u32)MATERIAL_ARRAY_ADDR);

	XViraymain_Set_lightArrayIn(viray, (u32)LIGHT_ARRAY_ADDR);
//	xil_printf("LIGHT ARRAY: %x\n\r", (u32)LIGHT_ARRAY_ADDR);

	XViraymain_Set_textureDataIn(viray, (u32)textureHelper.GetTextureDataAddress());
	XViraymain_Set_textureDescriptionIn(viray, (u32)textureHelper.GetTextureDescriptionDataAddress());
	XViraymain_Set_textureBaseAddressIn(viray, (u32)textureHelper.GetTextureBaseAddressAddress());
//	xil_printf("TEXTURE DATA: %x %x %x\n\r", (u32)textureHelper.GetTextureDataAddress(), (u32)textureHelper.GetTextureDescriptionDataAddress(), (u32)textureHelper.GetTextureBaseAddressAddress());

	XViraymain_Set_cameraArrayIn(viray, (u32)camera.GetBaseAddress());
//	xil_printf("CAMERA DATA: %x\n\r", (u32)camera.GetBaseAddress());

	XViraymain_Set_outColor(viray, (u32)OUT_COLOR_ADDR);
//	xil_printf("COLOR ARRAY: %x\n\r", (u32)OUT_COLOR_ADDR);

	// RESTART CLEANUP
	XViraymain_DisableAutoRestart(viray);
}

WorldDescription::~WorldDescription()
{
}

void WorldDescription::RunViRay(bool enableAutoRestart)
{
	// Single frame rendering
	myType elapsedTime(0.0);

	if (!enableAutoRestart)
	{
		timer.StartTimer();
		XViraymain_Start(viray);
		while(!XViraymain_IsIdle(viray));
		timer.StopTimer();

		elapsedTime = timer.GetElapsedSeconds();
		xil_printf("RENDER TIME: %d us\n\r", int(elapsedTime * 1e6));
	}
	// Looped rendering
	else
	{
		XViraymain_EnableAutoRestart(viray);
		XViraymain_Start(viray);

		while(true)
		{
			// ANIMATE
			timer.StartTimer();
			//....
			Animate(elapsedTime, GetGPIOButtonCode());

			timer.StopTimer();
			myType animationTime = timer.GetElapsedSeconds();

			// POST-ANIMATE
			timer.StartTimer();
			//....
			PostAnimate(animationTime);

			timer.StopTimer();
			myType postAnimationTime = timer.GetElapsedSeconds();

			// TOTAL ELAPSED TIME
			elapsedTime = animationTime + postAnimationTime;

			// update takes 100s of us which is << 60 ms
//			xil_printf("Last update duration: %d [us]\n\r", unsigned(elapsedTime * 1e6));

//			u8 readReg;
//			i2c->EepromReadByte((AddressType)0x3D, &readReg, 1);
//			xil_printf("VIC: %X = %X\n\r", 0x3D, readReg);
		}
	}
}

void WorldDescription::AssignLight(LightHandler* h)
{
	if (h->GetObjIdx() < LIGHTS_NUM)
	{
		lights[h->GetObjIdx()] = h;
	}
	else
	{
		xil_printf("ERROR:\n\n\r LIGHT #%d CAN NOT BE SAVED", h->GetObjIdx() + 1);
	}
}

void WorldDescription::AssignObject(ObjectHandler* h)
{
	if (h->GetObjIdx() < OBJ_NUM)
	{
		objects[h->GetObjIdx()] = h;
	}
	else
	{
		xil_printf("ERROR:\n\n\r OBJECT #%d CAN NOT BE SAVED", h->GetObjIdx() + 1);
	}
}

void WorldDescription::DumpAll()
{
	unsigned maxLightIdx = (LightHandler::nextIdx > LIGHTS_NUM ? LIGHTS_NUM : LightHandler::nextIdx);
	xil_printf("Dumping #%d lights:\n\r", maxLightIdx);
	for (unsigned i = 0; i < maxLightIdx; ++i)
	{
		xil_printf("##%d\n\r", i);
		lights[i]->DumpAll();
	}

	unsigned maxObjectIdx = (ObjectHandler::nextIdx > OBJ_NUM ? OBJ_NUM : ObjectHandler::nextIdx);
	xil_printf("Dumping #%d objects:\n\r", maxObjectIdx);
	for (unsigned i = 0; i < maxObjectIdx; ++i)
	{
		xil_printf("##%d\n\r", i);
		objects[i]->DumpAll();
	}

	xil_printf("Dumping camera:\n\r");
	camera.DumpAll();
}
