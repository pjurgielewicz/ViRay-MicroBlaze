/*
 * Empty C++ Application
 */

#include "xparameters.h"
#include "xil_cache.h"
#include "mb_interface.h"

#include "xviraymain.h"
#include "xviraymain_hw.h"

#include "TextureGenerator.h"
#include "AXITimerHelper.h"
#include "mat4.h"
#include "vec3.h"

#include "../include/handlers.h"

#define OBJ_TRANSFORMATION_ARRAY_ADDR 	((unsigned char*)XPAR_MIG_0_BASEADDR + (0x01000000))
#define OBJ_TRANSFORMATION_ARRAY2_ADDR 	(OBJ_TRANSFORMATION_ARRAY_ADDR 	+ sizeof(vec3) * OBJ_NUM * 4)
#ifndef SIMPLE_OBJECT_TRANSFORM_ENABLE
#define OBJ_TYPE_ADDR 					(OBJ_TRANSFORMATION_ARRAY_ADDR 	+ sizeof(mat4) * OBJ_NUM * 2)
#else
#define OBJ_TYPE_ADDR 					(OBJ_TRANSFORMATION_ARRAY2_ADDR + sizeof(vec3) * OBJ_NUM * 4)
#endif
#define LIGHT_ARRAY_ADDR 				(OBJ_TYPE_ADDR + sizeof(int) * OBJ_NUM)
#define MATERIAL_ARRAY_ADDR				(LIGHT_ARRAY_ADDR + sizeof(vec3) * LIGHTS_NUM * 4)
#define CAMERA_ARRAY_ADDR				(MATERIAL_ARRAY_ADDR + sizeof(vec3) * OBJ_NUM * 8)

#define TEXTURE_DATA_ADDR				(CAMERA_ARRAY_ADDR + sizeof(vec3) * 3)
#define TEXTURE_DESCRIPTION_ADDR		(TEXTURE_DATA_ADDR + sizeof(float_union) * TEXT_PAGE_SIZE)
#define TEXTURE_BASE_ADDR_ADDR			(TEXTURE_DESCRIPTION_ADDR + sizeof(unsigned) * OBJ_NUM)

#define OUT_COLOR_ADDR 					((unsigned char*)XPAR_MIG_0_BASEADDR + (0x10000000))

/*
 * ------------------------------------------------------------------------------------------------
 */

// SINCE FPGA VIRAY DOES NOT HANDLE FREE ROTATIONS THERE IS NO NEED TO STORE THE ANGLE
// T -> R -> S -> INV_S
myType objData[] = {    -1.0, 0.0, 1.0,   	-1.0, 0.0, 0.0,  	1.0, 1.0, 1.0,      1.0, 1.0, 1.0,
					   	2.0, -1.0, -1.0,   	0.0, 0.0, 1.0,   	2.0, 1.0, 1.0,      1.0, 1.0, 1.0,
					    0.0, -2.0, 0.0,    	0.0, 1.0, 0.0,  	1.0, 1.0, 1.0,      1.0, 1.0, 1.0,
					   	0.0, 0.0, -4.0,   	0.0, 0.0, 1.0,   	5.0, 5.0, 1.0,      1.0, 1.0, 1.0,
					  	-4.0, 0.0, 0.0,     1.0, 0.0, 0.0,   	1.0, 4.0, 8.0,      1.0, 1.0, 1.0,
					   	4.0, 0.0, 0.0,     	-1.0, 0.0, 0.0,  	1.0, 4.0, 8.0,      1.0, 1.0, 1.0,
					    2.0, -1.0, -1.0,   	0.0, 1.0, 0.0,   	1.0, 1.0, 1.0,      1.0, 1.0, 1.0,
					   	0.0, 0.0, 8.0,    	0.0, 0.0, -1.0,   	5.0, 5.0, 1.0,      1.0, 1.0, 1.0,
};

unsigned objTypeData[] = {	ObjectType::SPHERE, ObjectType::CYLINDER, ObjectType::PLANE, ObjectType::DISK,
							ObjectType::SQUARE, ObjectType::SQUARE, ObjectType::SPHERE, ObjectType::DISK,
};

myType lightData[] = {	0.0, 0.0, 0.0, 	0.0, -1.0, 0.0,    0.2, 0.2, 0.2,     0.0, 0.0, 0.0,
						3.0, 6.0, 3.0, 	-0.5, -1.0, -0.5,  80.0, 80.0, 80.0,  0.7, 0.85, 0.0
};

myType materialData[] = {	1.0, 0.3, 10, 	3.0, 1.33, 0.5,     1.0, 1.0, 1.0, 	0.8, 0.2, 0.2,      0.3, 0.3, 0.2,      1.0, 1.0, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
							1.0, 0.8, 75, 	0.0, 1.33, 0.0,     0.0, 1.0, 1.0, 	0.8, 0.5, 0.0,      0.0, 1.0, 0.0,      0.0, 1.0, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
							1.0, 0.6, 100,  1.0, 1.33, 0.0,	    1.0, 1.0, 1.0, 	0.9, 0.9, 0.9,      0.1, 0.1, 0.1,      1.0, 1.0, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
							1.0, 0.6, 100,  0.0, 1.33, 0.0,	    1.0, 0.0, 0.0, 	0.3, 0.3, 0.3,      1.0, 0.0, 0.0,      1.0, 0.0, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
							1.0, 0.6, 100,  1.0, 1.33, 0.0,     0.0, 0.2, 0.0, 	0.0, 1.0, 1.0,      1.0, 0.0, 0.0,      0.0, 1.0, 1.0,  0.0, 0.0, 0.0,  2.0, 2.0, 1.0,
							1.0, 0.6, 100,  1.0, 1.33, 0.0,     0.0, 1.0, 0.0, 	0.725, 0.478, 0.341, 0.514, 0.318, 0.212, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
							1.0, 0.3, 75, 	0.0, 1.33, 0.0,     1.0, 1.0, 1.0, 	0.5, 0.5, 0.5,      0.0, 0.0, 0.0,      0.0, 0.0, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
							1.0, 0.6, 100,  0.0, 1.33, 0.0,	    1.0, 0.0, 0.0, 	0.8, 0.4, 0.0,      1.0, 0.0, 0.0,      1.0, 0.0, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
};

myType cameraData[] = {	1.0,	0.0, 0.0, 7.0,
								0.0, 0.0, -1.0,
								0.0, 1.0, 0.0,
};


/*
 * NEEDED RESETS
 */
unsigned ObjectHandler::nextIdx = 0;
unsigned LightHandler::nextIdx 	= 0;

class WorldDescription
{
public:
	WorldDescription(XViraymain* viray) :
		viray(viray),
		textureHelper((float_union*)TEXTURE_DATA_ADDR, (unsigned*)TEXTURE_DESCRIPTION_ADDR, (unsigned*)TEXTURE_BASE_ADDR_ADDR),
		camera(viray, CAMERA_ARRAY_ADDR)
	{
		xil_printf("Initial clean-up\n\r");
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
		XViraymain_Set_objTransformationArrayCopyIn(viray, (u32)OBJ_TRANSFORMATION_ARRAY_ADDR);
		xil_printf("Transformation: %x\n\r", (u32)OBJ_TRANSFORMATION_ARRAY_ADDR);

		XViraymain_Set_objTypeIn(viray, (u32)OBJ_TYPE_ADDR);
		xil_printf("TYPE ARRAY: %x\n\r", (u32)OBJ_TYPE_ADDR);

		XViraymain_Set_materialArrayIn(viray, (u32)MATERIAL_ARRAY_ADDR);
		xil_printf("MATERIAL ARRAY: %x\n\r", (u32)MATERIAL_ARRAY_ADDR);

		XViraymain_Set_lightArrayIn(viray, (u32)LIGHT_ARRAY_ADDR);
		xil_printf("LIGHT ARRAY: %x\n\r", (u32)LIGHT_ARRAY_ADDR);

		XViraymain_Set_textureDataIn(viray, (u32)textureHelper.GetTextureDataAddress());
		XViraymain_Set_textureDescriptionIn(viray, (u32)textureHelper.GetTextureDescriptionDataAddress());
		XViraymain_Set_textureBaseAddressIn(viray, (u32)textureHelper.GetTextureBaseAddressAddress());
		xil_printf("TEXTURE DATA: %x %x %x\n\r", (u32)textureHelper.GetTextureDataAddress(), (u32)textureHelper.GetTextureDescriptionDataAddress(), (u32)textureHelper.GetTextureBaseAddressAddress());

		XViraymain_Set_cameraArrayIn(viray, (u32)CAMERA_ARRAY_ADDR/*camera.GetBaseAddress()*/);
		xil_printf("CAMERA DATA: %x\n\r", (u32)CAMERA_ARRAY_ADDR/*camera.GetBaseAddress()*/);

		XViraymain_Set_outColor(viray, (u32)OUT_COLOR_ADDR);
		xil_printf("COLOR ARRAY: %x\n\r", (u32)OUT_COLOR_ADDR);
	}

	~WorldDescription()
	{
	}

	virtual void Animate() = 0;
protected:
	void AssignLight(LightHandler* h)
	{
		lights[h->GetObjIdx()] = h;
	}

	void AssignObject(ObjectHandler* h)
	{
		objects[h->GetObjIdx()] = h;
	}

	void DumpAll()
	{
		xil_printf("Dumping lights:\n\r");
		for (unsigned i = 0; i < LightHandler::nextIdx; ++i)
		{
			xil_printf("##%d\n\r", i);
			lights[i]->DumpAll();
		}

		xil_printf("Dumping objects:\n\r");
		for (unsigned i = 0; i < ObjectHandler::nextIdx; ++i)
		{
			xil_printf("##%d\n\r", i);
			objects[i]->DumpAll();
		}

		xil_printf("Dumping camera:\n\r");
		camera.DumpAll();
	}

	unsigned char* GetNextLightAddress() const 			{return LIGHT_ARRAY_ADDR + LightStructureSize * LightHandler::nextIdx;}
	unsigned char* GetNextTransformationAddress() const {return OBJ_TRANSFORMATION_ARRAY_ADDR + TransformationStructureSize * ObjectHandler::nextIdx;}
	unsigned char* GetNextObjectTypeAddress() const		{return OBJ_TYPE_ADDR + ObjectTypeSize * ObjectHandler::nextIdx;}
	unsigned char* GetNextMaterialAddress() const 		{return MATERIAL_ARRAY_ADDR + MaterialStructureSize * ObjectHandler::nextIdx;}

	CTextureHelper& GetTextureHelper() {return textureHelper;}
	CameraHandler& GetCameraHandler() {return camera;}

	const unsigned LightStructureSize 			= 48;
	const unsigned TransformationStructureSize 	= 48;
	const unsigned ObjectTypeSize				= 4;
	const unsigned MaterialStructureSize 		= 96;

private:
	LightHandler* 	lights[LIGHTS_NUM];
	ObjectHandler* 	objects[OBJ_NUM];
	CameraHandler   camera;

	XViraymain* 	viray;
	CTextureHelper  textureHelper;
};

class MyWorld : public WorldDescription
{
public:
	MyWorld(XViraymain* viray) : WorldDescription(viray)
	{
		SpawnLights();
		SpawnObjects();
		GenerateTextures();
		SetupCamera();

		xil_printf("Dumping...");
		DumpAll();
	}
	virtual ~MyWorld()
	{
	}
	void Animate()
	{

	}
private:
	void SpawnLights()
	{
		ambient = LightHandler(GetNextLightAddress());
		ambient.SetColor(vec3(0.1, 0.1, 0.5), false);

		omni = LightHandler(GetNextLightAddress());
		omni.SetPosition(vec3(3.0, 6.0, 3.0), false);
		omni.SetDirection(vec3(-0.5, -1.0, -0.5), false);
		omni.SetColor(vec3(80.0), false);
		omni.SetConeProperties(0.85, 0.7, false);

		AssignLight(&ambient);
		AssignLight(&omni);
	}

	void SpawnObjects()
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		sphere = ObjectHandler(GetNextTransformationAddress(), GetNextObjectTypeAddress(), GetNextMaterialAddress());
		sphere.SetObjType(ObjectType::SPHERE);
		sphere.GetTransformHandler().SetTranslation(vec3(-1.0, 0.0, 1.0));
		sphere.GetTransformHandler().SetOrientation(vec3(0.0, 1.0, 0.0));
		sphere.GetTransformHandler().SetScale(vec3(1.0, 1.0, 1.0));

		sphere.GetMaterialHandler().SetAmbientColor(vec3(1.0, 1.0, 1.0));
		sphere.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.8, 0.2, 0.2));
		sphere.GetMaterialHandler().SetSecondaryDiffuseColor(vec3(0.3, 0.3, 0.2));
		sphere.GetMaterialHandler().SetSpecularColor(vec3(1.0, 1.0, 0.0));

		sphere.GetMaterialHandler().SetK(vec3(1.0, 1.0, 0.3));
		sphere.GetMaterialHandler().SetExp(10);
		sphere.GetMaterialHandler().SetEta(1.33);
		sphere.GetMaterialHandler().SetMaterialModifier(true, false);
		sphere.GetMaterialHandler().SetSigmaSqr(0.5);

		sphere.GetMaterialHandler().SetTexturePos(vec3(0.0));
		sphere.GetMaterialHandler().SetTextureScale(vec3(1.0));

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		cylinder = ObjectHandler(GetNextTransformationAddress(), GetNextObjectTypeAddress(), GetNextMaterialAddress());
		cylinder.SetObjType(ObjectType::CYLINDER);
		cylinder.GetTransformHandler().SetTranslation(vec3(2.0, -1.0, -1.0));
		cylinder.GetTransformHandler().SetOrientation(vec3(0.0, 0.0, 1.0));
		cylinder.GetTransformHandler().SetScale(vec3(2.0, 1.0, 1.0));

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		plane = ObjectHandler(GetNextTransformationAddress(), GetNextObjectTypeAddress(), GetNextMaterialAddress());
		plane.SetObjType(ObjectType::PLANE);
		plane.GetTransformHandler().SetTranslation(vec3(0.0, -2.0, 0.0));
		plane.GetTransformHandler().SetOrientation(vec3(0.0, 1.0, 0.0));
		plane.GetTransformHandler().SetScale(vec3(1.0, 1.0, 1.0));

		plane.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.9));
		plane.GetMaterialHandler().SetSecondaryDiffuseColor(vec3(0.1));

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		AssignObject(&sphere);
		AssignObject(&cylinder);
		AssignObject(&plane);
	}

	void GenerateTextures()
	{
		unsigned addr, code;
		GetTextureHelper().SaveTexture(CTextureGenerator(128, 128, CTextureGenerator::CHECKERBOARD, 6, 6), ViRay::CMaterial::PLANE_PLANAR, code, true, plane.GetObjIdx());
		GetTextureHelper().SaveTexture(CTextureGenerator(128, 128, CTextureGenerator::MARBLE, 6, 4, 1784301, 5, 1.87), ViRay::CMaterial::SPHERICAL, code, true, sphere.GetObjIdx());
	}

	void SetupCamera()
	{
		CameraHandler& c = GetCameraHandler();
		c.SetZoom(1.0);
		c.SetPosition(vec3(0.0, 0.0, 7.0));
		c.SetLookAtDir(vec3(0.0, 0.0, -1.0));
		c.SetUp(vec3(0.0, 1.0, 0.0));
	}

	LightHandler ambient;
	LightHandler omni;

	ObjectHandler sphere;
	ObjectHandler cylinder;
	ObjectHandler plane;
};

int main()
{
	XViraymain_Config *virayConfig;
	XViraymain viray;

	virayConfig = XViraymain_LookupConfig(XPAR_XVIRAYMAIN_0_DEVICE_ID);
	if (virayConfig)
	{
		XViraymain_CfgInitialize(&viray, virayConfig);
	}
	else
	{
		xil_printf("Error while initializing access to the device!\n\r");
		return XST_FAILURE;
	}

	/*
	 * TODO: SETUP BUTTONS, I2C, VDMA...
	 */

	/////////////////////////////////////////////////////////// OBJS SETUP
//	for (unsigned i = 0; i < OBJ_NUM; ++i)
//	{
//		for (unsigned j = 0; j < 3; ++j)
//		{
//			objData[i * 12 + 9 + j] = myType(1.0) / objData[i * 12 + 6 + j];
//		}
//	}

	xil_printf("Test... %d & %d\n\r", LightHandler::nextIdx, ObjectHandler::nextIdx);
	MyWorld myWorld(&viray);
	xil_printf("Test_out");

//	memcpy(OBJ_TRANSFORMATION_ARRAY_ADDR, objData, sizeof(myType) * 3 * 4 * OBJ_NUM);
//	memcpy(OBJ_TRANSFORMATION_ARRAY2_ADDR, objData, sizeof(myType) * 3 * 4 * OBJ_NUM);
	// Dummy for the moment...
//	memcpy(OBJ_TRANSFORMATION_ARRAY2_ADDR, OBJ_TRANSFORMATION_ARRAY_ADDR, sizeof(myType) * 3 * 4 * OBJ_NUM);


//	XViraymain_Set_objTransformationArrayIn(&viray, (u32)OBJ_TRANSFORMATION_ARRAY_ADDR);
//	XViraymain_Set_objTransformationArrayCopyIn(&viray, (u32)OBJ_TRANSFORMATION_ARRAY2_ADDR);
//	xil_printf("Transformation: %x\n\r", (u32)OBJ_TRANSFORMATION_ARRAY_ADDR);

//	memcpy(OBJ_TYPE_ADDR, objTypeData, sizeof(unsigned) * OBJ_NUM);
//	XViraymain_Set_objTypeIn(&viray, (u32)OBJ_TYPE_ADDR);
//	xil_printf("TYPE: %x\n\r", (u32)OBJ_TYPE_ADDR);

	///////////////////////////////////////////////////////// LIGHTS SETUP
//	for (unsigned i = 1; i < LIGHTS_NUM; ++i) // 0 - ambient light
//	{
//		myType outer = lightData[i * 12 + 10];
//		myType inner = lightData[i * 12 + 9];
//		myType outerMinusInner = outer - inner;
//		myType outerMinusInnerInv = (outerMinusInner > CORE_BIAS) ? myType(1.0) / (outerMinusInner) : MAX_DISTANCE;
//
//		lightData[i * 12 + 10] = outerMinusInnerInv;
//	}

//	memcpy(LIGHT_ARRAY_ADDR, lightData, sizeof(myType) * 12 * LIGHTS_NUM);
//	XViraymain_Set_lightArrayIn(&viray, (u32)LIGHT_ARRAY_ADDR);
//	xil_printf("LIGHT ARRAY: %x\n\r", (u32)LIGHT_ARRAY_ADDR);

	///////////////////////////////////////////////////////// MATERIALS SETUP
//	for (unsigned i = 0; i < OBJ_NUM; ++i)
//	{
//		myType kd = materialData[i * 24 + 0];
//		myType ks = materialData[i * 24 + 1];
//
//		// Oren-Nayar diffuse model coeffs
//		myType sigmaSqr = materialData[i * 24 + 5];
//		myType A = myType(1.0) - myType(0.5) * sigmaSqr / (sigmaSqr + myType(0.33));
//        myType B = myType(0.45) * sigmaSqr / (sigmaSqr + myType(0.09));
//
//        // SAVE INFO EG. WHETHER TO USE TORRANCE-SPARROW SPECULAR REFLECTION OR NOT
//		float_union materialInfo;
//		materialInfo.raw_bits = unsigned(materialData[i * 24 + 3]);
//
//		// FRESNEL INV FACTOR
//		myType eta = materialData[i * 24 + 4];
//		myType invEtaSqr = myType(1.0) / (eta * eta);
//
//		for (unsigned j = 0; j < 3; ++j)
//		{
//			materialData[i * 24 + 9  + j] *= kd;	// primary diffuse
//			materialData[i * 24 + 12 + j] *= kd;	// secondary diffuse
//			materialData[i * 24 + 15 + j] *= ks;	// specular
//		}
//
//		// ALTERING DATA IN THE STRUCTURE
//		materialData[i * 24 + 0] = A;
//		materialData[i * 24 + 1] = B;
//		materialData[i * 24 + 2] = materialData[i * 24 + 2] + ks;
//		materialData[i * 24 + 3] = materialInfo.fp_num;
//		materialData[i * 24 + 5] = invEtaSqr;
//
//		xil_printf("sigma: %d\tA: %d\tB: %d\n\r", unsigned(sigmaSqr * 1000), unsigned(A * 1000), unsigned(B * 1000));
//	}
//
//	memcpy(MATERIAL_ARRAY_ADDR, materialData, sizeof(myType) * 24 * OBJ_NUM);
//	XViraymain_Set_materialArrayIn(&viray, (u32)MATERIAL_ARRAY_ADDR);
//	xil_printf("MATERIAL: %x\n\r", (u32)MATERIAL_ARRAY_ADDR);

	///////////////////////////////////////////////////////// CAMERA
//	myType zoom = cameraData[0];
//	XViraymain_Set_cameraZoom(&viray, *((u32*)(&zoom)));

//	memcpy(CAMERA_ARRAY_ADDR, &(cameraData[1]), sizeof(myType) * 9);
//	XViraymain_Set_cameraArrayIn(&viray, (u32)CAMERA_ARRAY_ADDR);
//	xil_printf("CAMERA DATA: %x\n\r", (u32)CAMERA_ARRAY_ADDR);

//	///////////////////////////////////////////////////////// TEXTURE SETUP
//	// DIRECTLY ASSIGN ADDRESSES TO THE EXTERNAL MEMORY
//	float_union* textureData 			= (float_union*)TEXTURE_DATA_ADDR;
//	unsigned* textureDescriptionData 	= (unsigned*)TEXTURE_DESCRIPTION_ADDR;
//	unsigned* textureBaseAddr 			= (unsigned*)TEXTURE_BASE_ADDR_ADDR;
//
//	for (unsigned i = 0; i < OBJ_NUM; ++i)
//	{
//		textureDescriptionData[i] 	= (128 << 10) + 128; // REQUIRED DUMMY
//		textureBaseAddr[i]			= 0;
//	}
//	CTextureHelper textureHelper(textureData, textureDescriptionData, textureBaseAddr);
//	unsigned addr, code;
//	textureHelper.SaveTexture(CTextureGenerator(128, 128, CTextureGenerator::CHECKERBOARD, 6, 6), ViRay::CMaterial::PLANE_PLANAR, code, true, 2);
//	textureHelper.SaveTexture(CTextureGenerator(128, 128, CTextureGenerator::MARBLE, 6, 4, 1784301, 5, 1.87), ViRay::CMaterial::SPHERICAL, code, true, 0);
//
//	// NO NEED FOR MEMCPY BECAUSE TEXTURE DATA RESIDES DIRECTLY IN EXTERNAL MEMORY
//	XViraymain_Set_textureDataIn(&viray, (u32)textureData);
//	XViraymain_Set_textureDescriptionIn(&viray, (u32)textureDescriptionData);
//	XViraymain_Set_textureBaseAddressIn(&viray, (u32)textureBaseAddr);
//	xil_printf("TEXTURE DATA: %x\n\r", (u32)TEXTURE_DATA_ADDR);

	/////////////////////////////////////////////////////////// OUT_COLOR
//	XViraymain_Set_outColor(&viray, (u32)OUT_COLOR_ADDR);

	///////////////////////////////////////////////////////////
	xil_printf("Starting frame calculation...\n\r");
	CAXITimerHelper timer(XPAR_AXI_TIMER_0_DEVICE_ID, XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ);
	timer.StartTimer();

	XViraymain_Start(&viray);
	while(!XViraymain_IsIdle(&viray));

	timer.StopTimer();
	myType k = timer.GetElapsedSeconds();
	xil_printf("RENDER TIME: %d ms\n\r", int(k*1e3));

	return 0;
}
