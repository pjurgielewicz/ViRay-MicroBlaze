#include "../include/myWorld.h"

// SINCE FPGA VIRAY DOES NOT HANDLE FREE ROTATIONS THERE IS NO NEED TO STORE THE ANGLE
// T -> R -> S -> INV_S
//myType objData[] = {    -1.0, 0.0, 1.0,   	-1.0, 0.0, 0.0,  	1.0, 1.0, 1.0,      1.0, 1.0, 1.0,
//					   	2.0, -1.0, -1.0,   	0.0, 0.0, 1.0,   	2.0, 1.0, 1.0,      1.0, 1.0, 1.0,
//					    0.0, -2.0, 0.0,    	0.0, 1.0, 0.0,  	1.0, 1.0, 1.0,      1.0, 1.0, 1.0,
//					   	0.0, 0.0, -4.0,   	0.0, 0.0, 1.0,   	5.0, 5.0, 1.0,      1.0, 1.0, 1.0,
//					  	-4.0, 0.0, 0.0,     1.0, 0.0, 0.0,   	1.0, 4.0, 8.0,      1.0, 1.0, 1.0,
//					   	4.0, 0.0, 0.0,     	-1.0, 0.0, 0.0,  	1.0, 4.0, 8.0,      1.0, 1.0, 1.0,
//					    2.0, -1.0, -1.0,   	0.0, 1.0, 0.0,   	1.0, 1.0, 1.0,      1.0, 1.0, 1.0,
//					   	0.0, 0.0, 8.0,    	0.0, 0.0, -1.0,   	5.0, 5.0, 1.0,      1.0, 1.0, 1.0,
//};
//
//unsigned objTypeData[] = {	ObjectType::SPHERE, ObjectType::CYLINDER, ObjectType::PLANE, ObjectType::DISK,
//								ObjectType::SQUARE, ObjectType::SQUARE, ObjectType::SPHERE, ObjectType::DISK,
//};
//
//myType lightData[] = {	0.0, 0.0, 0.0, 	0.0, -1.0, 0.0,    0.2, 0.2, 0.2,     0.0, 0.0, 0.0,
//							3.0, 6.0, 3.0, 	-0.5, -1.0, -0.5,  80.0, 80.0, 80.0,  0.7, 0.85, 0.0
//};
//
//myType materialData[] = {	1.0, 0.3, 10, 	3.0, 1.33, 0.5,     1.0, 1.0, 1.0, 	0.8, 0.2, 0.2,      0.3, 0.3, 0.2,      1.0, 1.0, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
//							1.0, 0.8, 75, 	0.0, 1.33, 0.0,     0.0, 1.0, 1.0, 	0.8, 0.5, 0.0,      0.0, 1.0, 0.0,      0.0, 1.0, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
//							1.0, 0.6, 100,  1.0, 1.33, 0.0,	    1.0, 1.0, 1.0, 	0.9, 0.9, 0.9,      0.1, 0.1, 0.1,      1.0, 1.0, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
//							1.0, 0.6, 100,  0.0, 1.33, 0.0,	    1.0, 0.0, 0.0, 	0.3, 0.3, 0.3,      1.0, 0.0, 0.0,      1.0, 0.0, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
//							1.0, 0.6, 100,  1.0, 1.33, 0.0,     0.0, 0.2, 0.0, 	0.0, 1.0, 1.0,      1.0, 0.0, 0.0,      0.0, 1.0, 1.0,  0.0, 0.0, 0.0,  2.0, 2.0, 1.0,
//							1.0, 0.6, 100,  1.0, 1.33, 0.0,     0.0, 1.0, 0.0, 	0.725, 0.478, 0.341, 0.514, 0.318, 0.212, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
//							1.0, 0.3, 75, 	0.0, 1.33, 0.0,     1.0, 1.0, 1.0, 	0.5, 0.5, 0.5,      0.0, 0.0, 0.0,      0.0, 0.0, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
//							1.0, 0.6, 100,  0.0, 1.33, 0.0,	    1.0, 0.0, 0.0, 	0.8, 0.4, 0.0,      1.0, 0.0, 0.0,      1.0, 0.0, 0.0,  0.0, 0.0, 0.0,  1.0, 1.0, 1.0,
//};
//
//myType cameraData[] = {	1.0,	0.0, 0.0, 7.0,
//								0.0, 0.0, -1.0,
//								0.0, 1.0, 0.0,
//};


MyWorld::MyWorld(XViraymain* viray, XGpio* gpio, unsigned gpioButtonsChannel) :
	WorldDescription(viray, gpio, gpioButtonsChannel)
{
	SpawnLights();
	SpawnObjects();
	GenerateTextures();
	SetupCamera();

	xil_printf("Dumping...");
	DumpAll();
}

MyWorld::~MyWorld()
{
}

void MyWorld::Animate(myType elapsedTime, unsigned gpioButtonsCode)
{
	/*
	 * Animate world accordingly
	 */
	CameraHandler& cam = GetCameraHandler();
	cam.ApplyMovement(elapsedTime, gpioButtonsCode, true);

#ifdef UC_DIAGNOSTIC_DEBUG
	if (gpioButtonsCode) xil_printf("Button code: %x", gpioButtonsCode);
#endif
}

void MyWorld::PostAnimate(myType elapsedTime)
{

}

void MyWorld::SpawnLights()
{
	ambient = LightHandler(GetNextLightAddress());
	ambient.SetColor(vec3(0.1, 0.1, 0.1));

	omni = LightHandler(GetNextLightAddress());
	omni.SetPosition(vec3(3.0, 6.0, 3.0));
	omni.SetDirection(vec3(-0.5, -1.0, -0.5));
	omni.SetColor(vec3(240.0));
	omni.SetConeProperties(0.85, 0.7);

	AssignLight(&ambient);
	AssignLight(&omni);
}

void MyWorld::SpawnObjects()
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
	sphere.GetMaterialHandler().SetExp(100);
	sphere.GetMaterialHandler().SetEta(1.33);
	sphere.GetMaterialHandler().SetMaterialModifier(true, true, true);
	sphere.GetMaterialHandler().SetAbsorptionCoeff(2.0);
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

void MyWorld::GenerateTextures()
{
	unsigned addr, code;
	GetTextureHelper().SaveTexture(CTextureGenerator(128, 128, CTextureGenerator::CHECKERBOARD, 6, 6), ViRay::CMaterial::PLANE_PLANAR, code, true, plane.GetObjIdx());
	GetTextureHelper().SaveTexture(CTextureGenerator(128, 128, CTextureGenerator::MARBLE, 6, 4, 1784301, 5, 1.87), ViRay::CMaterial::SPHERICAL, code, true, sphere.GetObjIdx());
}

void MyWorld::SetupCamera()
{
	CameraHandler& c = GetCameraHandler();
	c.SetZoom(1.0);
	c.SetPosition(vec3(2.0, 0.0, 7.0));
//	c.SetNearPlane(2.0);
}

