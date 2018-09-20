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


MyWorld::MyWorld(ObjectHandler* objs, LightHandler* lights, XViraymain* viray, XGpio* gpio, I2C* i2c, unsigned gpioButtonsChannel) :
	WorldDescription(viray, gpio, i2c, gpioButtonsChannel),
	cylinder(objs[0]),
	floor(objs[1]),
	redBall(objs[2]), greenBall(objs[3]), blueBall(objs[4]), topBall(objs[5]),
	ceiling(objs[6]),
	cone(objs[7]),
	ambient(lights[0]), omni(lights[1])
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
	 * REACT TO USER MOVEMENT
	 */
	CameraHandler& cam = GetCameraHandler();
	cam.ApplyMovement(elapsedTime, gpioButtonsCode, true);

#ifdef UC_DIAGNOSTIC_DEBUG
	if (gpioButtonsCode) xil_printf("Button code: %x", gpioButtonsCode);
#endif

	/*
	 * ANIMATE OMNI LIGHT
	 */
//	mat4 precessionMat;
//	vec3 omniDir = omni.GetDirection();
//	vec3 xzOmniDir = vec3(omniDir[0], 0.0f, omniDir[2]);
//	precessionMat.RotationMatrix(200 * elapsedTime, vec3(0.0f, 1.0f, 0.0f));
//	vec3 newXZOmniDir = precessionMat * xzOmniDir;
//	vec3 newOmniDir(newXZOmniDir[0], omniDir[1], newXZOmniDir[2]);
//
//	omni.SetDirection(newOmniDir, true);

	static myType totalTime = 0.0f;

	totalTime += 0.3f * elapsedTime;
	if (totalTime > TWOPI) totalTime -= TWOPI;

	mat4 positionRotation;
	vec3 omniPos = omni.GetPosition();
	positionRotation.RotationMatrix(0.3f * elapsedTime, vec3(0.0f, 1.0f, 0.0f));
	vec3 newOmniPosition = positionRotation * omniPos;
	omniPos[1] = 4.0f + 5.0f * std::sin(totalTime);

	omni.SetPosition(newOmniPosition, true);
	omni.SetDirection(-newOmniPosition, true);

	/*
	 * SCALE WALLS
	 */
	vec3 cylScale = cylinder.GetTransformHandler().GetScale();
	cylScale[2] = 17.5f + 2.5f * std::sin(totalTime);
	cylinder.GetTransformHandler().SetScale(cylScale, true);

	/*
	 * SHIFT TEXTURE
	 */
	const float textureShiftSpeed = 0.01f;

	vec3 texturePos = floor.GetMaterialHandler().GetTexturePosition();
	texturePos += vec3(textureShiftSpeed * elapsedTime);
	for (unsigned i = 0; i < 3; ++i)
	{
		if (texturePos[i] > 1.0f) texturePos -= 1.0f;
	}
	floor.GetMaterialHandler().SetTexturePos(texturePos, true);
}

void MyWorld::PostAnimate(myType elapsedTime)
{
	// Consume dummy cycles - data does not need to be computed all the time
//	for (unsigned i = 0; i < 5000; ++i);

	/*
	 * I2C communication consumes significant amount of cycles but it is still
	 * much below frame rendering time (~~50-60 ms)
	 */
	GetI2C().Refresh();
}

void MyWorld::SpawnLights()
{
#define NEXT_HANDLER(x) (x = LightHandler(GetNextLightAddress()))

	NEXT_HANDLER(ambient);
	ambient.SetColor(vec3(0.1f, 0.1f, 0.1f));

	NEXT_HANDLER(omni);
	omni.SetPosition(vec3(4.0f));
	omni.SetDirection(vec3(-1.0f));
	omni.SetColor(vec3(240.0f));
//	omni.SetConeProperties(0.85f, 0.7f);
//	omni.SetConeProperties(1.0f, 0.0f);
	omni.SetConeProperties(-1.0f, -1.0f);

#undef NEXT_HANDLER

	AssignLight(&ambient);
	AssignLight(&omni);
}

void MyWorld::SpawnObjects()
{
#define NEXT_HANDLER(x) (x = ObjectHandler(GetNextTransformationAddress(), GetNextObjectTypeAddress(), GetNextMaterialAddress()))
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	NEXT_HANDLER(sphere);
//	sphere.SetObjType(ObjectType::SPHERE);
//	sphere.GetTransformHandler().SetTranslation(vec3(-1.0, 0.0, 1.0));
//	sphere.GetTransformHandler().SetOrientation(vec3(0.0, 1.0, 0.0));
//	sphere.GetTransformHandler().SetScale(vec3(1.0, 1.0, 1.0));
//
//	sphere.GetMaterialHandler().SetAmbientColor(vec3(1.0, 1.0, 1.0));
//	sphere.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.8, 0.2, 0.2));
//	sphere.GetMaterialHandler().SetSecondaryDiffuseColor(vec3(0.3, 0.3, 0.2));
//	sphere.GetMaterialHandler().SetSpecularColor(vec3(1.0, 1.0, 0.0));
//
//	sphere.GetMaterialHandler().SetK(vec3(1.0, 1.0, 0.3));
//	sphere.GetMaterialHandler().SetExp(100);
//	sphere.GetMaterialHandler().SetEta(1.33);
//	sphere.GetMaterialHandler().SetMaterialModifier(true, false, false);
//	sphere.GetMaterialHandler().SetAbsorptionCoeff(2.0);
//	sphere.GetMaterialHandler().SetSigmaSqr(0.5);
//
//	sphere.GetMaterialHandler().SetTexturePos(vec3(0.0));
//	sphere.GetMaterialHandler().SetTextureScale(vec3(1.0));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	myType roomSizeXZ = 20.0f;
//	myType roomSizeY  = 5.0f;

	NEXT_HANDLER(cylinder);
	cylinder.SetObjType(ObjectType::CYLINDER);
	cylinder.GetTransformHandler().SetTranslation(vec3(0.0f, 2.5f, 0.0f));
	cylinder.GetTransformHandler().SetOrientation(vec3(0.0f, 1.0f, 0.0f));
	cylinder.GetTransformHandler().SetScale(vec3(roomSizeXZ, 4.5f, roomSizeXZ));

	cylinder.GetMaterialHandler().SetAmbientColor(vec3(1.0f));
	cylinder.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.5f, 1.0f, 0.0f));
	cylinder.GetMaterialHandler().SetSecondaryDiffuseColor(vec3(0.0f));
	cylinder.GetMaterialHandler().SetSpecularColor(vec3(1.0f));
	cylinder.GetMaterialHandler().SetK(vec3(0.1f, 0.7f, 0.3f));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(floor);
	floor.SetObjType(ObjectType::DISK);
	floor.GetTransformHandler().SetTranslation(vec3(0.0f, -2.0f, 0.0f));
	floor.GetTransformHandler().SetOrientation(vec3(0.0f, 1.0f, 0.0f));
	floor.GetTransformHandler().SetScale(vec3(roomSizeXZ));

	floor.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.9f));
	floor.GetMaterialHandler().SetSecondaryDiffuseColor(vec3(0.1f));
	floor.GetMaterialHandler().SetTextureScale(vec3(6.0f));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(redBall);
	redBall.SetObjType(ObjectType::SPHERE);
	redBall.GetTransformHandler().SetTranslation(vec3(-1.0f, 0.0f, 0.0f));

	redBall.GetMaterialHandler().SetAmbientColor(vec3(1.0f));
	redBall.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(1.0f, 0.6f, 0.6f));
	redBall.GetMaterialHandler().SetSecondaryDiffuseColor(vec3(1.0f, 0.6f, 0.6f));
	redBall.GetMaterialHandler().SetSpecularColor(vec3(1.0f));

	redBall.GetMaterialHandler().SetKSpecular(0.0f);
	redBall.GetMaterialHandler().SetSigmaSqr(1.0f);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(greenBall);
	greenBall.SetObjType(ObjectType::SPHERE);
	greenBall.GetTransformHandler().SetTranslation(vec3(1.0f, 0.0f, 0.0f));

	greenBall.GetMaterialHandler().SetAmbientColor(vec3(1.0f));
	greenBall.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.6f, 1.0f, 0.6f));
	greenBall.GetMaterialHandler().SetSecondaryDiffuseColor(vec3(0.6f, 1.0f, 0.6f));
	greenBall.GetMaterialHandler().SetSpecularColor(vec3(1.0f));

	greenBall.GetMaterialHandler().SetKSpecular(0.2f);
	greenBall.GetMaterialHandler().SetSigmaSqr(0.0f);
	greenBall.GetMaterialHandler().SetMaterialModifier(true, false, false);
	greenBall.GetMaterialHandler().SetEta(1.5f);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(blueBall);
	blueBall.SetObjType(ObjectType::SPHERE);
	blueBall.GetTransformHandler().SetTranslation(vec3(0.0f, 0.0f, 1.73f));

	blueBall.GetMaterialHandler().SetAmbientColor(vec3(1.0f));
	blueBall.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.2f, 0.2f, 1.0f));
	blueBall.GetMaterialHandler().SetSecondaryDiffuseColor(vec3(1.0f, 1.0f, 1.0f));
	blueBall.GetMaterialHandler().SetSpecularColor(vec3(0.5f));
	blueBall.GetMaterialHandler().SetKSpecular(0.3f);
	blueBall.GetMaterialHandler().SetExp(100.0f);
//	blueBall.GetMaterialHandler().SetSigmaSqr(100.0f);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(topBall);
	topBall.SetObjType(ObjectType::SPHERE);
	topBall.GetTransformHandler().SetTranslation(vec3(0.0f, 1.633f, 0.577f));

	topBall.GetMaterialHandler().SetAmbientColor(vec3(1.0f));
	topBall.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.851f, 0.564f, 0.345f));	// copper
	topBall.GetMaterialHandler().SetSecondaryDiffuseColor(vec3(0.6f, 0.6f, 0.6f));
	topBall.GetMaterialHandler().SetSpecularColor(vec3(1.0f));
	topBall.GetMaterialHandler().SetMaterialModifier(true, true, true);
	topBall.GetMaterialHandler().SetAbsorptionCoeff(0.5f);
	topBall.GetMaterialHandler().SetEta(1.33f);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(ceiling);
	ceiling.SetObjType(ObjectType::SPHERE);
	ceiling.GetTransformHandler().SetTranslation(vec3(0.0f, -11.5f, 0.0f));
	ceiling.GetTransformHandler().SetOrientation(vec3(0.0f, 1.0f, 0.0f));
	ceiling.GetTransformHandler().SetScale(vec3(roomSizeXZ * 1.25f));

	ceiling.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.9f));
	ceiling.GetMaterialHandler().SetSecondaryDiffuseColor(vec3(0.1f));
	ceiling.GetMaterialHandler().SetSpecularColor(vec3(0.0f, 0.8f, 0.8f));
	ceiling.GetMaterialHandler().SetK(vec3(0.1f, 0.7f, 0.3f));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(cone);
	cone.SetObjType(ObjectType::CONE);
	cone.GetTransformHandler().SetTranslation(vec3(0.0f, 2.633f, 0.577f));
	cone.GetTransformHandler().SetScale(vec3(0.5f, 1.0f, 0.5f));


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef NEXT_HANDLER

//	AssignObject(&sphere);
	AssignObject(&cylinder);
	AssignObject(&floor);

	AssignObject(&redBall);
	AssignObject(&greenBall);
	AssignObject(&blueBall);
	AssignObject(&topBall);

	AssignObject(&ceiling);
	AssignObject(&cone);
}

void MyWorld::GenerateTextures()
{
	unsigned addr, code;
	GetTextureHelper().SaveTexture(CTextureGenerator(128, 128, CTextureGenerator::CHECKERBOARD, 6, 6), ViRay::CMaterial::PLANAR, code, true, floor.GetObjIdx());
	GetTextureHelper().SaveTexture(CTextureGenerator(128, 128, CTextureGenerator::MARBLE, 6, 4, 1784301, 5, 1.87), ViRay::CMaterial::SPHERICAL, code, true, blueBall.GetObjIdx());
}

void MyWorld::SetupCamera()
{
	CameraHandler& c = GetCameraHandler();
	c.SetZoom(1.0);
	c.SetPosition(vec3(2.0, 0.0, 7.0));
	c.SetSpeed(5, 2);
//	c.SetNearPlane(2.0);
}

