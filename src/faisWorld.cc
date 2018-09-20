#include "../include/faisWorld.h"


FaisWorld::FaisWorld(ObjectHandler* objs, LightHandler* lights, XViraymain* viray, XGpio* gpio, I2C* i2c, unsigned gpioButtonsChannel) :
	WorldDescription(viray, gpio, i2c, gpioButtonsChannel),
	cylinder(objs[0]),
	floor(objs[1]),
	ballCap(objs[2]),
	discCap(objs[3]),
	deflectorDisc(objs[4]),
	spinningBall(objs[5]),
	vectorCone(objs[6]),
	cone(objs[7]),
	ambient(lights[0]), omni(lights[1])
{
	SpawnLights();
	SpawnObjects();
	GenerateTextures();
	SetupCamera();

	xil_printf("Dumping...");
	DumpAll();

	spinningBallTime = 0.0f;
	isSpinningBallInverseMovement = false;
}

FaisWorld::~FaisWorld()
{
}

void FaisWorld::Animate(myType elapsedTime, unsigned gpioButtonsCode)
{
	/*
	 * REACT TO USER MOVEMENT
	 */
	CameraHandler& cam = GetCameraHandler();

	vec3 prevPos = cam.GetPosition();
	cam.ApplyMovement(elapsedTime, gpioButtonsCode, true);
	vec3 currPos = cam.GetPosition();

	// Z-limit
	if (currPos[2] > TUNNEL_LENGTH_SCALE * 0.98f)
	{
		currPos[2] = TUNNEL_LENGTH_SCALE * 0.98f;
	}
	else if (currPos[2] < -TUNNEL_LENGTH_SCALE + TUNNEL_RADIUS * 1.01f)
	{
		currPos[2] = -TUNNEL_LENGTH_SCALE + TUNNEL_RADIUS * 1.01f;
	}
	// Y-Limit
	if (currPos[1] < 1.01f - TUNNEL_RADIUS )
	{
		currPos[1] = 1.01f - TUNNEL_RADIUS;
	}
	// R-limit
	else if (currPos[0] * currPos[0] + currPos[1] * currPos[1] > 0.99f * TUNNEL_RADIUS * TUNNEL_RADIUS)
	{
		currPos = prevPos;
	}

	cam.SetPosition(currPos);

	/*
	 * SPINNING BALL
	 */
	if (!isSpinningBallInverseMovement)
	{
		spinningBallTime += elapsedTime;
		if (spinningBallTime > 10.0f)
		{
			spinningBallTime = 10.0f;
			isSpinningBallInverseMovement = true;
		}
	}
	else
	{
		spinningBallTime -= elapsedTime;
		if (spinningBallTime <= 0.0f)
		{
			spinningBallTime = 0.0f;
			isSpinningBallInverseMovement = false;
		}
	}

	const myType spinningBallOmega = 2.0f;
	const myType spinningBallZVelocity = 3.0f;

	vec3 spinningBallPosition = vec3(
			TUNNEL_INNER_RADIUS * std::sin(spinningBallOmega * spinningBallTime),
			TUNNEL_INNER_RADIUS * std::cos(spinningBallOmega * spinningBallTime),
			(TUNNEL_LENGTH_SCALE - spinningBallZVelocity * spinningBallTime)
	);
	spinningBall.GetTransformHandler().SetTranslation(spinningBallPosition, true);

	/*
	 * OMNI LIGHT
	 */
	vec3 lightPosition = -spinningBallPosition + vec3(0.0f, 0.0f, 0.35f * TUNNEL_LENGTH_SCALE);
//	if (lightPosition[2] < -0.65f * TUNNEL_LENGTH_SCALE) lightPosition[2] = -0.65f * TUNNEL_LENGTH_SCALE;
	omni.SetPosition(lightPosition, true);

	/*
	 * CYLINDER
	 */
	static myType cylinderTexturePos = 0.0f;
	cylinderTexturePos += 0.03f * elapsedTime;
	if (cylinderTexturePos > 1.0f) cylinderTexturePos -= 1.0f;
	cylinder.GetMaterialHandler().SetTexturePos(vec3(cylinderTexturePos, 0.0f, 0.0f), true);

	/*
	 * VECTOR
	 */
	const myType vectorPosAmplitude = TUNNEL_LENGTH_SCALE * 0.4f;
	const myType vectorPhaseVelocity = 0.5f;
	const myType vectorZ0 = TUNNEL_LENGTH_SCALE * 0.5f;

	static myType vectorPhase = 0.0f;


	vectorPhase += vectorPhaseVelocity * elapsedTime;
	if (vectorPhase > TWOPI) vectorPhase -= TWOPI;

	vec3 currVectorPos = vectorCone.GetTransformHandler().GetTranslation();
	currVectorPos[2] = vectorZ0 + vectorPosAmplitude * std::cos(vectorPhase);

	vec3 orientation((vectorPhase > PI) ? vec3(0.0f, 0.0f, 1.0f) : vec3(0.0f, 0.0f, -1.0f));

	myType vectorVelocity = std::fabs(-vectorPosAmplitude * std::sin(vectorPhase) * 0.2f) + 0.025f;

	vectorCone.GetTransformHandler().SetTranslation(currVectorPos, true);
	vectorCone.GetTransformHandler().SetScale(vec3(0.5f, 0.5f, vectorVelocity), true);
	vectorCone.GetTransformHandler().SetOrientation(orientation, true);

	/*
	 * DISC CAP
	 */
	const myType discCapReflectivityMax = 0.6f;
	const myType discCapReflectivityMin = 0.0f;
	const myType dicCapReflectivityChangeSpeed = 0.3f;
	static myType discCapInverseMovement = false;
	static myType discCapTime = 0.0f;

	if (!discCapInverseMovement)
	{
		discCapTime += elapsedTime * dicCapReflectivityChangeSpeed;
	}
	else discCapTime -= elapsedTime * dicCapReflectivityChangeSpeed;

	myType discCapReflectivity = discCapReflectivityMin + (discCapReflectivityMax - discCapReflectivityMin) * discCapTime;

	if (discCapReflectivity > discCapReflectivityMax)
	{
		discCapReflectivity = discCapReflectivityMax;
		discCapInverseMovement = true;
	}
	else if (discCapReflectivity < discCapReflectivityMin)
	{
		discCapReflectivity = discCapReflectivityMin;
		discCapInverseMovement = false;
	}
	discCap.GetMaterialHandler().SetKSpecular(discCapReflectivity, true);

	/*
	 * BALL CAP
	 */
//	static myType ballCapTexturePos = 0.0f;
//	ballCapTexturePos += 0.01f * elapsedTime;
//	if (ballCapTexturePos > 1.0f) ballCapTexturePos -= 1.0f;
//	ballCap.GetMaterialHandler().SetTexturePos(vec3(-ballCapTexturePos, 0.0f, 0.0f), true);
}

void FaisWorld::PostAnimate(myType elapsedTime)
{
	// Consume dummy cycles - data does not need to be computed all the time
//	for (unsigned i = 0; i < 5000; ++i);

	/*
	 * I2C communication consumes significant amount of cycles but it is still
	 * much below frame rendering time (~~50-60 ms)
	 */
	GetI2C().Refresh();
}

void FaisWorld::SpawnLights()
{
#define NEXT_HANDLER(x) (x = LightHandler(GetNextLightAddress()))

	NEXT_HANDLER(ambient);
	ambient.SetColor(vec3(0.25f));

	NEXT_HANDLER(omni);
	omni.SetPosition(vec3(3.0f, 0.0f, -7.0f));
	omni.SetDirection(vec3(0.0f, 0.0f, -1.0f));
	omni.SetColor(vec3(240.0f));
//	omni.SetConeProperties(0.85f, 0.7f);
//	omni.SetConeProperties(1.0f, 0.0f);
	omni.SetConeProperties(-1.0f, -1.0f);

#undef NEXT_HANDLER

	AssignLight(&ambient);
	AssignLight(&omni);
}

void FaisWorld::SpawnObjects()
{
#define NEXT_HANDLER(x) (x = ObjectHandler(GetNextTransformationAddress(), GetNextObjectTypeAddress(), GetNextMaterialAddress()))

	NEXT_HANDLER(cylinder);
	cylinder.SetObjType(ObjectType::CYLINDER);
	cylinder.GetTransformHandler().SetTranslation(vec3(0.0f, 0.0f, 0.0f));
	cylinder.GetTransformHandler().SetOrientation(vec3(0.0f, 0.0f, 1.0f));
	cylinder.GetTransformHandler().SetScale(vec3(TUNNEL_RADIUS, TUNNEL_RADIUS, TUNNEL_LENGTH_SCALE));
	cylinder.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.5f));
	cylinder.GetMaterialHandler().SetMaterialModifier(true, false, false);
	cylinder.GetMaterialHandler().SetEta(1.7f);
	cylinder.GetMaterialHandler().SetTextureScale(vec3(10.0f));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(floor);
	floor.SetObjType(ObjectType::PLANE);
	floor.GetTransformHandler().SetTranslation(vec3(0.0f, 1.0f - TUNNEL_RADIUS, 0.0f));
	floor.GetTransformHandler().SetOrientation(vec3(0.0f, 1.0f, 0.0f));
//	floor.GetTransformHandler().SetScale(vec3(1.0f, 1.0f, TUNNEL_LENGTH_SCALE));
	floor.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.9f, 0.9f, 0.0f));
	floor.GetMaterialHandler().SetSpecularColor(vec3(0.0f, 0.9f, 0.9f));
	floor.GetMaterialHandler().SetMaterialModifier(true, false, false);
	floor.GetMaterialHandler().SetEta(1.33f);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(ballCap);
	ballCap.SetObjType(ObjectType::SPHERE);
	ballCap.GetTransformHandler().SetTranslation(vec3(0.0f, 0.0f, -TUNNEL_LENGTH_SCALE * 1.0f));
	ballCap.GetTransformHandler().SetOrientation(vec3(0.0f, 1.0f, 0.0f));
	ballCap.GetTransformHandler().SetScale(vec3(2.0f * TUNNEL_RADIUS, 4.0f * TUNNEL_RADIUS, TUNNEL_RADIUS));
	ballCap.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.9f, 0.0f, 0.0f));
	ballCap.GetMaterialHandler().SetSpecularColor(vec3(0.5f, 0.5f, 0.1f));
//	ballCap.GetMaterialHandler().SetKSpecular(0.1f);
	ballCap.GetMaterialHandler().SetMaterialModifier(true, false, true);
//	ballCap.GetMaterialHandler().SetSigmaSqr(0.5f);
//	ballCap.GetMaterialHandler().SetEta(1.33f);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(discCap);
	discCap.SetObjType(ObjectType::DISK);
	discCap.GetTransformHandler().SetTranslation(vec3(0.0f, 0.0f, TUNNEL_LENGTH_SCALE));
	discCap.GetTransformHandler().SetOrientation(vec3(0.0f, 0.0f, 1.0f));
	discCap.GetTransformHandler().SetScale(vec3(TUNNEL_RADIUS));
	discCap.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.5f, 0.5f, 0.0f));
	discCap.GetMaterialHandler().SetKSpecular(0.1f);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(deflectorDisc);
	deflectorDisc.SetObjType(ObjectType::DISK);
	deflectorDisc.GetTransformHandler().SetTranslation(vec3(0.0f, 0.0f, 0.0f));
	deflectorDisc.GetTransformHandler().SetOrientation(vec3(0.0f, 0.0f, 1.0f));
	deflectorDisc.GetTransformHandler().SetScale(vec3(TUNNEL_RADIUS * 0.25f));
	deflectorDisc.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.5f, 0.5f, 0.5f));
	deflectorDisc.GetMaterialHandler().SetSpecularColor(vec3(0.8f, 0.2f, 0.7f));


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(spinningBall);
	spinningBall.SetObjType(ObjectType::SPHERE);
	spinningBall.GetTransformHandler().SetTranslation(vec3(0.0f, TUNNEL_INNER_RADIUS, 0.0f));
	spinningBall.GetTransformHandler().SetOrientation(vec3(0.0f, 0.0f, 1.0f));
	spinningBall.GetTransformHandler().SetScale(vec3(1.0f));
	spinningBall.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.5f, 0.7f, 0.0f));
	spinningBall.GetMaterialHandler().SetMaterialModifier(true, true, true);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	NEXT_HANDLER(vectorCone);
	vectorCone.SetObjType(ObjectType::CONE);
	vectorCone.GetTransformHandler().SetTranslation(vec3(0.0f, 0.0f, TUNNEL_LENGTH_SCALE * 0.5f));
	vectorCone.GetTransformHandler().SetOrientation(vec3(0.0f, 0.0f, 1.0f));
	vectorCone.GetTransformHandler().SetScale(vec3(0.5f, 0.5f, 1.0f));
	vectorCone.GetMaterialHandler().SetPrimaryDiffuseColor(vec3(0.5f, 0.7f, 0.0f));
	vectorCone.GetMaterialHandler().SetKSpecular(0.0f);
	vectorCone.GetMaterialHandler().SetSigmaSqr(0.5f);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//	NEXT_HANDLER(cone);
//	cone.SetObjType(ObjectType::CONE);
//	cone.GetTransformHandler().SetTranslation(vec3(0.0f, 2.633f, 0.577f));
//	cone.GetTransformHandler().SetScale(vec3(0.5f, 1.0f, 0.5f));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef NEXT_HANDLER

	AssignObject(&cylinder);
	AssignObject(&floor);
	AssignObject(&ballCap);
	AssignObject(&discCap);
	AssignObject(&deflectorDisc);
	AssignObject(&spinningBall);
	AssignObject(&vectorCone);
//	AssignObject(&cone);
}

void FaisWorld::GenerateTextures()
{
	unsigned addr, code;
	GetTextureHelper().SaveTexture(CTextureGenerator(128, 128, CTextureGenerator::CHECKERBOARD, 6, 6), ViRay::CMaterial::CYLINDRICAL, code, true, cylinder.GetObjIdx());
//	GetTextureHelper().SaveTexture(CTextureGenerator(192, 192, CTextureGenerator::MARBLE, 6, 4, 1784301, 5, 1.87), ViRay::CMaterial::SPHERICAL, code, true, ballCap.GetObjIdx());
}

void FaisWorld::SetupCamera()
{
	CameraHandler& c = GetCameraHandler();
	c.SetZoom(1.0f);
	c.SetNearPlane(0.5f);
	c.SetPosition(vec3(-4.0f, 0.0f, 7.0f));
	c.SetSpeed(5.0f, 2.0f);
}
