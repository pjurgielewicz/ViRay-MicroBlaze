#ifndef MYWORLD__H_
#define MYWORLD__H_

#include "worldDescription.h"

class MyWorld : public WorldDescription
{
public:
	MyWorld(ObjectHandler* objs, LightHandler* lights,  XViraymain* viray = nullptr, XGpio* gpio = nullptr, I2C* i2c = nullptr, unsigned gpioButtonsChannel = 1);
	virtual ~MyWorld();

	void Animate(myType elapsedTime, unsigned gpioButtonsCode);
	void PostAnimate(myType elapsedTime);

private:
	void SpawnLights();
	void SpawnObjects();
	void GenerateTextures();
	void SetupCamera();

	LightHandler& ambient;
	LightHandler& omni;

//	ObjectHandler sphere;
	ObjectHandler& cylinder;
	ObjectHandler& floor;
	ObjectHandler& ceiling;

	ObjectHandler& redBall;
	ObjectHandler& greenBall;
	ObjectHandler& blueBall;
	ObjectHandler& topBall;

	ObjectHandler& cone;
};

#endif
