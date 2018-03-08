#include "../include/handlers.h"

/*
 * BaseHandler
 */
BaseHandler::BaseHandler(unsigned char* ptr) : ptr(ptr)
{

}
void BaseHandler::SaveVector(const vec3& v, unsigned shift)
{
	memcpy(ptr + shift, v.data, sizeof(vec3));
}

void BaseHandler::SaveValue(const myType& s, unsigned shift)
{
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
	k 			= vec3(0.5);
	exp 		= 10.0;
	specularTerms = k[2] + exp;

	sigmaSqr 	= 0.0;
	A 			= 1.0;
	B			= 0.0;

	ambientColor 			= vec3(0.1);
	primaryDiffuseColor 	= vec3(0.8, 0.0, 0.0);
	secondaryDiffuseColor 	= vec3(0.0, 0.8, 0.0);
	specularColor 			= vec3(0.0, 0.0, 0.8);

	resAmbientColor			= ambientColor * k[0];
	resPrimaryDiffuseColor  = primaryDiffuseColor * k[1];
	resSecondaryDiffuseColor= secondaryDiffuseColor * k[1];
	resSpecularColor		= specularColor * k[2];

	isFresnelReflection 		= false;
	isTorranceSparrowSpecular 	= false;
	float_union tmp;
	tmp.raw_bits				= 0;
	materialModifier			= tmp.fp_num;

	eta 		= 1.0;
	invEtaSqr 	= 1.0;

	texturePos 		= vec3(0.0);
	textureScale 	= vec3(1.0);
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

void MaterialHandler::SetMaterialModifier(const bool& isFresnel, const bool& isTorranceSparrow, bool isImmediate)
{
	isFresnelReflection = isFresnel;
	isTorranceSparrowSpecular = isTorranceSparrow;

	float_union tmp;
	tmp.raw_bits = (isFresnelReflection ? 1 : 0) + (isTorranceSparrowSpecular ? 2 : 0);
	materialModifier = tmp.fp_num;
	if (isImmediate)
	{
		SaveValue(materialModifier, 12);
	}
}

void MaterialHandler::SetEta(const myType& s, bool isImmediate)
{
	eta = s;
	invEtaSqr = myType(1.0) / (eta * eta);
	if (isImmediate)
	{
		SaveValue(eta, 16);
		SaveValue(invEtaSqr, 20);
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
	SaveVector(vec3(materialModifier, eta, invEtaSqr), 12);
	SaveVector(resAmbientColor, 24);
	SaveVector(resPrimaryDiffuseColor, 36);
	SaveVector(resSecondaryDiffuseColor, 48);
	SaveVector(resSpecularColor, 60);
	SaveVector(texturePos, 72);
	SaveVector(textureScale, 84);
}

/*
 * ObjectHandler
 */
ObjectHandler::ObjectHandler(unsigned char* transformPtr, unsigned char* objTypePtr, unsigned char* materialPtr) : transformHandler(transformPtr), materialHandler(materialPtr), objTypePtr(objTypePtr)
{
	objType = -1;
}

void ObjectHandler::SetObjType(const unsigned& t)
{
	objType = t;
	memcpy(objTypePtr, &objType, sizeof(unsigned));
}

void ObjectHandler::DumpAll()
{
	transformHandler.DumpAll();
	materialHandler.DumpAll();
	memcpy(objTypePtr, &objType, sizeof(unsigned));
}

/*
 * LightHandler
 */
LightHandler::LightHandler(unsigned char* ptr) : BaseHandler(ptr)
{
	position = vec3(0.0);
	direction = vec3(0.0, 0.0, -1.0);
	color = vec3(50.0);

	SetConeProperties(0.85, 0.7, false);
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
	SaveVector(position, 0);
	SaveVector(direction, 12);
	SaveVector(color, 24);
	SaveVector(vec3(outerAngle, outerMinusInnerInv, 0.0), 36);
}
