#ifndef FAISWORLD__H_
#define FAISWORLD__H_

#include "worldDescription.h"

#define TUNNEL_LENGTH_SCALE (myType(20.0f))
#define TUNNEL_RADIUS (myType(7.0f))
#define TUNNEL_INNER_RADIUS (TUNNEL_RADIUS * myType(0.75f))

class FaisWorld : public WorldDescription
{
public:
	FaisWorld(ObjectHandler* objs, LightHandler* lights,  XViraymain* viray = nullptr, XGpio* gpio = nullptr, I2C* i2c = nullptr, unsigned gpioButtonsChannel = 1);
	virtual ~FaisWorld();

	void Animate(myType elapsedTime, unsigned gpioButtonsCode);
	void PostAnimate(myType elapsedTime);

private:
	void SpawnLights();
	void SpawnObjects();
	void GenerateTextures();
	void SetupCamera();

	LightHandler& ambient;
	LightHandler& omni;

	ObjectHandler& cylinder;
	ObjectHandler& floor;
	ObjectHandler& ballCap;

	ObjectHandler& discCap;
	ObjectHandler& deflectorDisc;
	ObjectHandler& spinningBall;
	ObjectHandler& vectorCone;

	ObjectHandler& cone;

	//////////////////////////////////////

	myType spinningBallTime;
	bool isSpinningBallInverseMovement;
};

#endif
