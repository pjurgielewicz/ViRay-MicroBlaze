#ifndef MYWORLD__H_
#define MYWORLD__H_

#include "worldDescription.h"

class MyWorld : public WorldDescription
{
public:
	MyWorld(XViraymain* viray, XGpio* gpio, unsigned gpioButtonsChannel);
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
