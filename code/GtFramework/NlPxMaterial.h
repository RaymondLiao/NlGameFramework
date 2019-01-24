/*
  @file   NlPhysicsMaterial.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlPhysicsMaterial_h_
#define _NlPhysicsMaterial_h_

#include "prerequisite.h"

enum ENlPxMtrlCombMode
{
	ePMComb_Average,
	ePMComb_MIN,
	ePMComb_MAX,
	ePMComb_MULTIPLU
};

typedef struct _NlPxMtrlParam
{
	NxReal _bounciness;					// range 0 ~ 1
	NxReal _dynamicFriction;			// range 0 ~ 1
	NxReal _staticFriction;				// range 0 ~ 1
	ENlPxMtrlCombMode _frictionCombine;
	ENlPxMtrlCombMode _BounceCombine;

	_NlPxMtrlParam()
	{
		_bounciness = _dynamicFriction = _staticFriction = 0.5;
		_frictionCombine = _BounceCombine = ePMComb_Average;
	}
	_NlPxMtrlParam( float bounciness, float dFriction, float sFriction,
		ENlPxMtrlCombMode frictionComb, ENlPxMtrlCombMode bounceComb ):
		_bounciness(bounciness), _dynamicFriction(dFriction), _staticFriction(sFriction),
		_frictionCombine(frictionComb), _BounceCombine(bounceComb) {}

} NlPxMtrlParam;

// internal physics types
//enum ENlPhysicsMtrlType
//{
//	ePMtrlT_Default,
//	ePMtrlT_Max
//};

//====================================================
class NlPhysicsDevice;
class NlPxMtrlManager;
class NlPxMaterial
{
	friend class NlPxMtrlManager;
private:
	NlPxMaterial( NlPhysicsDevice* device );
	NlPxMaterial( NlPxMaterial& another ) {}
	NlPxMaterial& operator= ( NlPxMaterial& another ) {}
	
	void init( NlPxMtrlParam& param );

public:
	uint16 GetIndex() {return m_index;}
	virtual ~NlPxMaterial(void);

private:
	NxScene* m_scene;
	NxMaterial* m_mtrl;

	std::string m_name;
	NxU16 m_index;

};
typedef std::map<std::string, NlPxMaterial*> NlPxMtrlMapper;

// Singleton ====================================================
class NlPxMtrlManager
{
	friend class NlGFramework;
public:
	static NlPxMtrlManager& GetInstance();
	uint16 CreateMaterial( NlPhysicsDevice* device, NlPxMtrlParam& param, const char* name );
	NlPxMaterial* GetMaterialByName( const char* name );
	void ClearUp();		// clear all the physics materials that exist

private:
	NlPxMtrlManager() {}
	NlPxMtrlManager( NlPxMtrlManager& another ) {}
	NlPxMtrlManager& operator= ( NlPxMtrlManager& another ) {}
	~NlPxMtrlManager();

	static void Create();
	static void Destroy();

private:
	NlPxMtrlMapper m_mtrls;
	static NxU16 curMaxIndex;

	static NlPxMtrlManager* instance;

};

#endif