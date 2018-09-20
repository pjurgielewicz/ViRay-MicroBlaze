#include "../include/handlers.h"

/*
 * BaseHandler
 */
BaseHandler::BaseHandler(unsigned char* ptr) : ptr(ptr)
{

}
void BaseHandler::SaveVector(const vec3& v, unsigned shift)
{
#ifdef UC_DIAGNOSTIC_VERBOSE
	xil_printf("Saving [%d, %d, %d] to %d\n\r", unsigned(v[0] * 1000), unsigned(v[1] * 1000), unsigned(v[2] * 1000), shift);
#endif
	memcpy(ptr + shift, v.data, sizeof(vec3));
}

void BaseHandler::SaveValue(const myType& s, unsigned shift)
{
#ifdef UC_DIAGNOSTIC_VERBOSE
	xil_printf("Saving %d to %d\n\r", unsigned(s * 1000), shift);
#endif
	memcpy(ptr + shift, &s, sizeof(myType));
}

/*
 * ObjectTransformHandler
 */
ObjectTransformHandler::ObjectTransformHandler(unsigned char* ptr) : BaseHandler(ptr)
{
	T = vec3(0.0);
	R = vec3(0.0, 1.0, 0.0);
	S = vec3(1.0);
	IS = vec3(1.0);
}

void ObjectTransformHandler::SetTranslation(const vec3& v, bool isImmediate)
{
	T = v;
	if (isImmediate) SaveVector(T, 0);
}

void ObjectTransformHandler::SetOrientation(const vec3& v, bool isImmediate)
{
	R = v;
	if (isImmediate) SaveVector(R, 12);
}

void ObjectTransformHandler::SetScale(const vec3& v, bool isImmediate)
{
	S = v;
	IS = vec3(myType(1.0) / v[0], myType(1.0) / v[1], myType(1.0) / v[2]);

	if (isImmediate)
	{
		SaveVector(S, 24);
		SaveVector(IS, 36);
	}
}

/////////////////////////////////////////////////////////////////////////////

void ObjectTransformHandler::DumpAll()
{
	SaveVector(T, 0);
	SaveVector(R, 12);
	SaveVector(S, 24);
	SaveVector(IS, 36);
}

/*
 *
 */
MaterialHandler::MaterialHandler(unsigned char* ptr) : BaseHandler(ptr)
{
	k 							= vec3(0.5f, 0.5f, 0.5f);
	exp 						= 10.0f;
	specularTerms 				= k[2] + exp;

	sigmaSqr 					= 0.0f;
	A 							= 1.0f;
	B							= 0.0f;

	ambientColor 				= vec3(0.1f);
	primaryDiffuseColor 		= vec3(0.8f, 0.0f, 0.0f);
	secondaryDiffuseColor 		= vec3(0.0f, 0.8f, 0.0f);
	specularColor 				= vec3(0.0f, 0.0f, 0.8f);

	resAmbientColor				= ambientColor * k[0];
	resPrimaryDiffuseColor  	= primaryDiffuseColor * k[1];
	resSecondaryDiffuseColor	= secondaryDiffuseColor * k[1];
	resSpecularColor			= specularColor * k[2];

	isFresnelReflection 		= false;
	isTorranceSparrowSpecular 	= false;
	isConductor					= false;

	float_union tmp;
	tmp.raw_bits				= 0;
	materialModifier			= tmp.fp_num;

	eta 						= 1.0f;
	absorptionCoeff 			= 1.0f;
	UpdateEtaAbsorption();

	texturePos 					= vec3(0.0f);
	textureScale 				= vec3(1.0f);
}

void MaterialHandler::SetK(const vec3& v, bool isImmediate)
{
	SetKAmbient(v[0], isImmediate);
	SetKDiffuse(v[1], isImmediate);
	SetKSpecular(v[2], isImmediate);
}

void MaterialHandler::SetKAmbient(const myType& s, bool isImmediate)
{
	k[0] = s;
	resAmbientColor = ambientColor * k[0];
	if (isImmediate)
	{
		SaveVector(resAmbientColor, 24);
	}
}

void MaterialHandler::SetKDiffuse(const myType& s, bool isImmediate)
{
	k[1] = s;
	resPrimaryDiffuseColor = primaryDiffuseColor * k[1];
	resSecondaryDiffuseColor = secondaryDiffuseColor * k[1];
	if (isImmediate)
	{
		SaveVector(resPrimaryDiffuseColor, 36);
		SaveVector(resSecondaryDiffuseColor, 48);
	}
}

void MaterialHandler::SetKSpecular(const myType& s, bool isImmediate)
{
	k[2] = s;
	if (k[2] >= myType(1.0)) k[2] = 0.99f; // Clamp

	resSpecularColor = specularColor * k[2];
	specularTerms = k[2] + exp;

	if (isImmediate)
	{
		SaveVector(resSpecularColor, 60);
		SaveValue(specularTerms, 8);
	}
}

void MaterialHandler::SetExp(const myType& s, bool isImmediate)
{
	exp = s;
	specularTerms = k[2] + exp;

	if (isImmediate)
	{
		SaveValue(specularTerms, 8);
	}
}

void MaterialHandler::SetSigmaSqr(const myType& s, bool isImmediate)
{
	sigmaSqr = s;
	A = myType(1.0) - myType(0.5) * sigmaSqr / (sigmaSqr + myType(0.33));
	B = myType(0.45) * sigmaSqr / (sigmaSqr + myType(0.09));
	if (isImmediate)
	{
		SaveValue(A, 0);
		SaveValue(B, 4);
	}
}

void MaterialHandler::SetAmbientColor(const vec3& v, bool isImmediate)
{
	ambientColor = v;
	resAmbientColor = ambientColor * k[0];
	if (isImmediate)
	{
		SaveVector(resAmbientColor, 24);
	}
}

void MaterialHandler::SetPrimaryDiffuseColor(const vec3& v, bool isImmediate)
{
	primaryDiffuseColor = v;
	resPrimaryDiffuseColor = primaryDiffuseColor * k[1];
	if (isImmediate)
	{
		SaveVector(resPrimaryDiffuseColor, 36);
	}
}

void MaterialHandler::SetSecondaryDiffuseColor(const vec3& v, bool isImmediate)
{
	secondaryDiffuseColor = v;
	resSecondaryDiffuseColor = secondaryDiffuseColor * k[1];
	if (isImmediate)
	{
		SaveVector(resSecondaryDiffuseColor, 48);
	}
}

void MaterialHandler::SetSpecularColor(const vec3& v, bool isImmediate)
{
	specularColor = v;
	resSpecularColor = specularColor * k[2];
	if (isImmediate)
	{
		SaveVector(resSpecularColor, 60);
	}
}

void MaterialHandler::SetMaterialModifier(const bool& isFresnel, const bool& isTorranceSparrow, const bool& isConductor, bool isImmediate)
{
	isFresnelReflection 		= isFresnel;
	isTorranceSparrowSpecular 	= isTorranceSparrow;
	this->isConductor 			= isConductor;

	float_union tmp;
	tmp.raw_bits = 	(isFresnelReflection ? 0x1 : 0) 		+
					(isTorranceSparrowSpecular ? 0x2 : 0) 	+
					(this->isConductor ? 0x4 : 0);
	materialModifier = tmp.fp_num;
	UpdateEtaAbsorption();
	if (isImmediate)
	{
		SaveValue(materialModifier, 12);
		SaveValue(invRelativeEtaSqrORExtendedAbsorptionCoeff, 20);
	}
}

void MaterialHandler::SetEta(const myType& s, bool isImmediate)
{
	eta = s;
	UpdateEtaAbsorption();
	if (isImmediate)
	{
		SaveValue(eta, 16);
		SaveValue(invRelativeEtaSqrORExtendedAbsorptionCoeff, 20);
	}
}

void MaterialHandler::SetAbsorptionCoeff(const myType& s, bool isImmediate)
{
	absorptionCoeff = s;
	UpdateEtaAbsorption();
	if (isImmediate)
	{
		SaveValue(invRelativeEtaSqrORExtendedAbsorptionCoeff, 20);
	}
}

void MaterialHandler::SetTexturePos(const vec3& v, bool isImmediate)
{
	texturePos = v;
	if (isImmediate)
	{
		SaveVector(texturePos, 72);
	}
}

void MaterialHandler::SetTextureScale(const vec3& v, bool isImmediate)
{
	textureScale = v;
	if (isImmediate)
	{
		SaveVector(textureScale, 84);
	}
}

/////////////////////////////////////////////////////////////////////////////

void MaterialHandler::DumpAll()
{
	SaveVector(vec3(A, B, specularTerms), 0);
	SaveVector(vec3(materialModifier, eta, invRelativeEtaSqrORExtendedAbsorptionCoeff), 12);
	SaveVector(resAmbientColor, 24);
	SaveVector(resPrimaryDiffuseColor, 36);
	SaveVector(resSecondaryDiffuseColor, 48);
	SaveVector(resSpecularColor, 60);
	SaveVector(texturePos, 72);
	SaveVector(textureScale, 84);
}

void MaterialHandler::UpdateEtaAbsorption()
{
	if (isConductor)
	{
		invRelativeEtaSqrORExtendedAbsorptionCoeff = eta * eta + absorptionCoeff * absorptionCoeff;
	}
	else
	{
		invRelativeEtaSqrORExtendedAbsorptionCoeff = myType(1.0) / (eta * eta);
	}
}

/*
 * ObjectHandler
 */
ObjectHandler::ObjectHandler(unsigned char* transformPtr, unsigned char* objTypePtr, unsigned char* materialPtr) : transformHandler(transformPtr), materialHandler(materialPtr), objTypePtr(objTypePtr)
{
	objType = -1;

	// Getting rid of temporary objects
	if (transformPtr != (unsigned char*)0xA0000000)
	{
		uniqueIdx = nextIdx++;
		if (uniqueIdx >= OBJ_NUM)
		{
			xil_printf("ERROR:\n\n\rWRONG OBJECT INDEX - %d EXCEEDS MAXIMUM %d\n\n\r", uniqueIdx + 1, OBJ_NUM);
		}
	}
}

void ObjectHandler::SetObjType(const unsigned& t)
{
	objType = t;
	memcpy(objTypePtr, &objType, sizeof(unsigned));
}

void ObjectHandler::DumpAll()
{
	xil_printf("Dumping object %d (obj type: %d)\n\r", uniqueIdx, objType);

	xil_printf("Transform data @ %x\n\r", unsigned(transformHandler.GetBaseAddress()));
	transformHandler.DumpAll();

	xil_printf("Material data @ %x\n\r", unsigned(materialHandler.GetBaseAddress()));
	materialHandler.DumpAll();

	xil_printf("Type data %d @ %x\n\r", objType, unsigned(objTypePtr));
	memcpy(objTypePtr, &objType, sizeof(unsigned));
}

/*
 * LightHandler
 */
LightHandler::LightHandler(unsigned char* ptr) : BaseHandler(ptr)
{
	position 	= vec3(0.0);
	direction 	= vec3(0.0, 0.0, -1.0);
	color 		= vec3(50.0);

	SetConeProperties(0.85, 0.7, false);

	// Getting rid of temporary objects
	if (ptr != (unsigned char*)0xA0000000)
	{
		uniqueIdx = nextIdx++;
		if (uniqueIdx >= LIGHTS_NUM)
		{
			xil_printf("ERROR:\n\n\rWRONG LIGHT INDEX - %d EXCEEDS MAXIMUM %d\n\n\r", uniqueIdx + 1, LIGHTS_NUM);
		}
	}
}

// setters' section
void LightHandler::SetPosition(const vec3& v, bool isImmediate)
{
	position = v;
	if (isImmediate)
	{
		SaveVector(position, 0);
	}
}

void LightHandler::SetDirection(const vec3& v, bool isImmediate)
{
	direction = v.Normalize();
	if (isImmediate)
	{
		SaveVector(direction, 12);
	}
}

void LightHandler::SetColor(const vec3& v, bool isImmediate)
{
	color = v;
	if (isImmediate)
	{
		SaveVector(color, 24);
	}
}

void LightHandler::SetConeProperties(const myType& inner, const myType& outer, bool isImmediate)
{
	innerAngle = inner;
	outerAngle = outer;
	myType outerMinusInner = innerAngle - outerAngle;
	outerMinusInnerInv = (outerMinusInner > CORE_BIAS) ? myType(1.0) / outerMinusInner : MAX_DISTANCE;
	if (isImmediate)
	{
		SaveVector(vec3(outerAngle, outerMinusInnerInv, 0.0), 36);
	}
}

/////////////////////////////////////////////////////////////////////////////

void LightHandler::DumpAll()
{
	xil_printf("Light data @ %x\n\r", unsigned(GetBaseAddress()));
	SaveVector(position, 0);
	SaveVector(direction, 12);
	SaveVector(color, 24);
	SaveVector(vec3(outerAngle, outerMinusInnerInv, 0.0f), 36);
}

/*
 * CameraHandler
 */

CameraHandler::CameraHandler(XViraymain* viray, unsigned char* ptr) :
		BaseHandler(ptr),
		viray(viray)
{
	zoom 		= 1.0f;
	nearPlane	= 1.0f;

	position 	= vec3(0.0);

	u			= vec3(1.0f, 0.0f, 0.0f);
	v 			= vec3(0.0f, 1.0f, 0.0f);
	w			= vec3(0.0f, 0.0f, 1.0f);

//	RebuildCameraFrame();

	movementSpeed = myType(100.0f);
	rotationSpeed = myType(0.1f);
}

// setters' section
void CameraHandler::SetZoom(const myType& s, bool isImmediate)
{
	zoom = s;
	if (isImmediate)
	{
		XViraymain_Set_cameraZoom(viray, *((u32*)(&zoom)));
	}
}

void CameraHandler::SetNearPlane(const myType& s, bool isImmediate)
{
	nearPlane = s;
	if (isImmediate)
	{
		XViraymain_Set_cameraNearPlane(viray, *((u32*)(&nearPlane)));
	}
}

void CameraHandler::SetPosition(const vec3& v, bool isImmediate)
{
	position = v;
	if (isImmediate)
	{
		SaveVector(position, 0);
	}
}

void CameraHandler::SetU(const vec3& v, bool isImmediate)
{
	this->u = v;
	if (isImmediate)
	{
		SaveVector(this->u, 12);
	}
}

void CameraHandler::SetV(const vec3& v, bool isImmediate)
{
	this->v = v;
	if (isImmediate)
	{
		SaveVector(this->v, 24);
	}
}

void CameraHandler::SetW(const vec3& v, bool isImmediate)
{
	this->w = v;
	if (isImmediate)
	{
		SaveVector(this->w, 36);
	}
}

void CameraHandler::SetSpeed(const myType& movement, const myType& rotation)
{
	movementSpeed = movement;
	rotationSpeed = rotation;
}

/////////////////////////////////////////////////////////////////////////////

void CameraHandler::DumpAll()
{
	XViraymain_Set_cameraZoom(viray, *((u32*)(&zoom)));
	XViraymain_Set_cameraNearPlane(viray, *((u32*)(&nearPlane)));

	SaveVector(position, 0);

	SaveVector(u, 12);
	SaveVector(v, 24);
	SaveVector(w, 36);
}

void CameraHandler::ApplyMovement(myType elapsedTime, unsigned gpioButtonsCode, bool isImmediate)
{
	// Clear opposite directions
	if (gpioButtonsCode & 0x08 && gpioButtonsCode & 0x02) gpioButtonsCode -= 0x0A;
	if (gpioButtonsCode & 0x10 && gpioButtonsCode & 0x04) gpioButtonsCode -= 0x14;
	if (gpioButtonsCode < 2) return; // 	NO MEANINGFUL INPUT

	if (gpioButtonsCode & 0x1) // Camera rotation
	{
		// Dropping modifier bit since it is not needed in the functions
		RotateCamera(elapsedTime, gpioButtonsCode >> 1);
#ifdef UC_DIAGNOSTIC_VERBOSE
		xil_printf("Camera rotated\n\r");
#endif
	}
	else
	{
		MoveCamera(elapsedTime, gpioButtonsCode >> 1);
#ifdef UC_DIAGNOSTIC_VERBOSE
		xil_printf("Camera moved\n\r");
#endif
	}

	if (isImmediate)
	{
		DumpAll();
	}
}

void CameraHandler::RebuildCameraFrame()
{
//	w = -lookAtDir;
	u = (v ^ w).Normalize();
	v = (w ^ u).Normalize();
}

void CameraHandler::MoveCamera(myType elapsedTime, unsigned gpioButtonsCodeReduced)
{
	myType delta = movementSpeed * elapsedTime;

	if ((gpioButtonsCodeReduced == 0x01 || gpioButtonsCodeReduced == 0x04) &&
		(gpioButtonsCodeReduced == 0x02 || gpioButtonsCodeReduced == 0x08)) delta *= myType(0.7071067);

	// LEFT-RIGHT
	if (gpioButtonsCodeReduced & 0x01)
	{
		position -= u * delta;
	}
	else if (gpioButtonsCodeReduced & 0x04)
	{
		position += u * delta;
	}
	// BACK-FORWARD
	if (gpioButtonsCodeReduced & 0x08)
	{
		position -= w * delta;
	}
	else if (gpioButtonsCodeReduced & 0x02)
	{
		position += w * delta;
	}
}

void CameraHandler::RotateCamera(myType elapsedTime, unsigned gpioButtonsCodeReduced)
{
	mat4 rot;
	myType delta = rotationSpeed * elapsedTime;

	if (gpioButtonsCodeReduced & 0x01 || gpioButtonsCodeReduced & 0x04) 	// GLOBAL Y-AXIS ROTATION
	{
		delta = (gpioButtonsCodeReduced & 0x01) ? delta : -delta;
		rot.RotationMatrix(delta, vec3(0.0, 1.0, 0.0));
	}
	else																	// LOCAL X-AXIS ROTATION
	{
		delta = (gpioButtonsCodeReduced & 0x02) ? -delta : delta;
		rot.RotationMatrix(delta, u);
	}

	u = (rot * u).Normalize();
	v = (rot * v).Normalize();
	w = (rot * w).Normalize();
}
