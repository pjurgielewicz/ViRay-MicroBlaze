#ifndef HANDLERS__H_
#define HANDLERS__H_


#include <string.h>
#include "../Utils/vec3.h"

class BaseHandler
{
public:
	BaseHandler(unsigned char* ptr);
	void SaveVector(const vec3& v, unsigned shift);
	void SaveValue(const myType& s, unsigned shift);

private:
	unsigned char* ptr;
};

class ObjectTransformHandler : public BaseHandler
{
public:
	ObjectTransformHandler(unsigned char* ptr);

	// getters' section
	const vec3& GetTranslation() const {return T;}
	const vec3& GetOrientation() const {return R;}
	const vec3& GetScale() const {return S;}

	// setters' section
	void SetTranslation(const vec3& v, bool isImmediate = true);
	void SetOrientation(const vec3& v, bool isImmediate = true);
	void SetScale(const vec3& v, bool isImmediate = true);

	/////////////////////////////////////////////////////////////////////////////

	void DumpAll();
private:
	vec3 T, R, S, IS;
};

class MaterialHandler : public BaseHandler
{
public:
	MaterialHandler(unsigned char* ptr);

	// getters' section
	const vec3& GetK() const {return k;}
	const myType& GetExp() const {return exp;}
	const myType& GetSigmaSqr() const {return sigmaSqr;}

	const vec3& GetAmbientColor() const {return ambientColor;}
	const vec3& GetPrimaryDiffuseColor() const {return primaryDiffuseColor;}
	const vec3& GetSecondaryDiffuseColor() const {return secondaryDiffuseColor;}
	const vec3& GetSpecularColor() const {return specularColor;}

	const bool& IsFresnelReflection() const {return isFresnelReflection;}
	const bool& IsTorranceSparrowSpecular() const {return isTorranceSparrowSpecular;}

	const myType& GetEta() const {return eta;}

	const vec3& GetTexturePosition() const {return texturePos;}
	const vec3& GetTextureScale() const {return textureScale;}

	// setters' section
	void SetK(const vec3& v, bool isImmediate = true);
	void SetKAmbient(const myType& s, bool isImmediate = true);
	void SetKDiffuse(const myType& s, bool isImmediate = true);
	void SetKSpecular(const myType& s, bool isImmediate = true);
	void SetExp(const myType& s, bool isImmediate = true);
	void SetSigmaSqr(const myType& s, bool isImmediate = true);
	void SetAmbientColor(const vec3& v, bool isImmediate = true);
	void SetPrimaryDiffuseColor(const vec3& v, bool isImmediate = true);
	void SetSecondaryDiffuseColor(const vec3& v, bool isImmediate = true);
	void SetSpecularColor(const vec3& v, bool isImmediate = true);
	void SetMaterialModifier(const bool& isFresnel, const bool& isTorranceSparrow, bool isImmediate = true);
	void SetEta(const myType& s, bool isImmediate = true);
	void SetTexturePos(const vec3& v, bool isImmediate = true);
	void SetTextureScale(const vec3& v, bool isImmediate = true);

	/////////////////////////////////////////////////////////////////////////////

	void DumpAll();

private:
	vec3 k;
	myType exp, specularTerms;
	myType sigmaSqr, A, B;

	vec3 ambientColor, resAmbientColor;
	vec3 primaryDiffuseColor, secondaryDiffuseColor, resPrimaryDiffuseColor, resSecondaryDiffuseColor;
	vec3 specularColor, resSpecularColor;

	bool isFresnelReflection, isTorranceSparrowSpecular;
	myType materialModifier;
	myType eta, invEtaSqr;

	vec3 texturePos, textureScale;
};

class ObjectHandler
{
public:
	ObjectHandler(unsigned char* transformPtr, unsigned char* objTypePtr, unsigned char* materialPtr);

	const unsigned& GetObjType() const {return objType;}
	void SetObjType(const unsigned& t);
	void DumpAll();


	ObjectTransformHandler& GetTransformHandler() {return transformHandler;}
	MaterialHandler& GetMaterialHandler() {return materialHandler;}

private:
	ObjectTransformHandler 	transformHandler;
	MaterialHandler			materialHandler;

	unsigned 				objType;
	unsigned char*			objTypePtr;
};

class LightHandler : public BaseHandler
{
public:
	LightHandler(unsigned char* ptr);

	// getters' section
	const vec3& GetPosition() const {return position;}
	const vec3& GetDirection() const {return direction;}
	const vec3& GetColor() const {return color;}
	const myType& GetInnerAngleCos() const {return innerAngle;}
	const myType& GetOuterAngleCos() const {return outerAngle;}

	// setters' section
	void SetPosition(const vec3& v, bool isImmediate = true);
	void SetDirection(const vec3& v, bool isImmediate = true);
	void SetColor(const vec3& v, bool isImmediate = true);
	void SetConeProperties(const myType& inner, const myType& outer, bool isImmediate = true);

	/////////////////////////////////////////////////////////////////////////////

	void DumpAll();

private:
	vec3 position;
	vec3 direction;
	vec3 color;
	myType innerAngle, outerAngle, outerMinusInnerInv;
};

#endif
