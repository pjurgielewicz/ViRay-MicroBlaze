#ifndef MYWORLD__H_
#define MYWORLD__H_

#include "worldDescription.h"

class MyWorld : public WorldDescription
{
public:
	MyWorld(XViraymain* viray = nullptr, XGpio* gpio = nullptr, unsigned gpioButtonsChannel = 1);
	virtual ~MyWorld();

	void Animate(myType elapsedTime, unsigned gpioButtonsCode);
	void PostAnimate(myType elapsedTime);

private:
	void SpawnLights();
	void SpawnObjects();
	void GenerateTextures();
	void SetupCamera();

	LightHandler ambient;
	LightHandler omni;

	ObjectHandler sphere;
	ObjectHandler cylinder;
	ObjectHandler plane;
};

#endif
