#ifndef WORLDDESCRIPTION__H_
#define WORLDDESCRIPTION__H_

#include "xviraymain.h"
#include "xgpio.h"

#include "../include/adv7511_setup.h"
#include "handlers.h"
#include "AXITimerHelper.h"
#include "TextureGenerator.h"

#define OBJ_TRANSFORMATION_ARRAY_ADDR 	((unsigned char*)XPAR_MIG_0_BASEADDR + (0x01000000))
#ifndef SIMPLE_OBJECT_TRANSFORM_ENABLE
#define OBJ_TYPE_ADDR 					(OBJ_TRANSFORMATION_ARRAY_ADDR + sizeof(mat4) * OBJ_NUM * 2)
#else
#define OBJ_TYPE_ADDR 					(OBJ_TRANSFORMATION_ARRAY_ADDR + sizeof(vec3) * OBJ_NUM * 4)
#endif

#define LIGHT_ARRAY_ADDR 				(OBJ_TYPE_ADDR + sizeof(int) * OBJ_NUM)
#define MATERIAL_ARRAY_ADDR				(LIGHT_ARRAY_ADDR + sizeof(vec3) * LIGHTS_NUM * 4)
#define CAMERA_ARRAY_ADDR				(MATERIAL_ARRAY_ADDR + sizeof(vec3) * OBJ_NUM * 8)

#define TEXTURE_DATA_ADDR				(CAMERA_ARRAY_ADDR + sizeof(vec3) * 4)
#define TEXTURE_DESCRIPTION_ADDR		(TEXTURE_DATA_ADDR + sizeof(float_union) * TEXT_PAGE_SIZE)
#define TEXTURE_BASE_ADDR_ADDR			(TEXTURE_DESCRIPTION_ADDR + sizeof(unsigned) * OBJ_NUM)

#define OUT_COLOR_ADDR 					((unsigned char*)XPAR_MIG_0_BASEADDR + (0x10000000))

class I2C;

class WorldDescription
{
public:
	WorldDescription(XViraymain* viray, XGpio* gpio, I2C* i2c, unsigned gpioButtonsChannel);
	~WorldDescription();

	virtual void Animate(myType elapsedTime, unsigned gpioButtonCode) = 0;
	virtual void PostAnimate(myType elapsedTime) = 0;						// Can be used to put extra dummy cycles before next execution
	void RunViRay(bool enableAutoRestart = false);
protected:
	void AssignLight(LightHandler* h);
	void AssignObject(ObjectHandler* h);

	void DumpAll();

	unsigned char* GetNextLightAddress() const 			{return LIGHT_ARRAY_ADDR + LightStructureSize * LightHandler::nextIdx;}
	unsigned char* GetNextTransformationAddress() const {return OBJ_TRANSFORMATION_ARRAY_ADDR + TransformationStructureSize * ObjectHandler::nextIdx;}
	unsigned char* GetNextObjectTypeAddress() const		{return OBJ_TYPE_ADDR + ObjectTypeSize * ObjectHandler::nextIdx;}
	unsigned char* GetNextMaterialAddress() const 		{return MATERIAL_ARRAY_ADDR + MaterialStructureSize * ObjectHandler::nextIdx;}

	CTextureHelper& GetTextureHelper() 	{return textureHelper;}
	CameraHandler& GetCameraHandler() 	{return camera;}
	CAXITimerHelper& GetTimer()			{return timer;}
	I2C& GetI2C()						{return *i2c;}

private:
	unsigned GetGPIOButtonCode() const					{return XGpio_DiscreteRead(gpio, gpioButtonsChannel);}

	LightHandler* 	lights[LIGHTS_NUM];
	ObjectHandler* 	objects[OBJ_NUM];
	CameraHandler   camera;

	CTextureHelper  textureHelper;
	CAXITimerHelper timer;

	XViraymain* 	viray;
	XGpio*			gpio;
	I2C*			i2c;
	const unsigned  gpioButtonsChannel;

	const unsigned LightStructureSize 			= 48;
	const unsigned TransformationStructureSize 	= 48;
	const unsigned ObjectTypeSize				= 4;
	const unsigned MaterialStructureSize 		= 96;
};

#endif
