#pragma once
/*
  @file   NlMaterial.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlMaterial_h_
#define _NlMaterial_h_

#include "prerequisite.h"

//====================================================
enum ENlMtrlTexSlot
{
	eMTSLOT_Diffuse,
	eMTSLOT_Specular,

	eMTSLOT_Max
};

enum ENlMtrlType
{
	eMTType_Diffuse,
	eMTType_Specular,
	eMTType_TranSpec,

};

struct NlMtrlLoadingParam
{
	std::string mtrlName;
	ENlMtrlType mtrlType;

	float4		Ka;
	float4		Kd;
	float4		Ks;
	float		Glossness;
	bool		Trans;

	std::string Kd_map;		// diffuse_map file name
	std::string Kb_map;		// bump_map file name
	std::string Ks_map;		// specular_map file name
	std::string Kr_map;		// reflection_map file name

	NlMtrlLoadingParam()
	{
		mtrlType = eMTType_Diffuse;

		Ka = float4( 1.0f, 1.0f, 1.0f, 1.0f );
		Kd = float4( 1.0f, 1.0f, 1.0f, 1.0f );
		Ks = float4( 0.0f, 0.0f, 0.0f, 0.0f );
		Glossness = 0.0f;
		Trans = false;
	}
};

typedef struct _NlDefShaderParam
{
	_NlDefShaderParam()
	{
		wvp_mat = float44::CreateIdentity();
		world_mat = float44::CreateIdentity();

		lightDir = 0;
		camPos = 0;
	}
	// for vs
	float44 wvp_mat;
	float44 world_mat;

	// for ps
	float3 lightDir;
	float3 camPos;
} NlDefShaderParam;

//====================================================
class NlRenderDevice;
class NlDynamicShaderComb;
class NlTexture2D;
class CCamera;

class NlMaterial
{
public:
	NlMaterial( NlRenderDevice* device );
	virtual ~NlMaterial(void);

	static bool GetLoadParamFromFile( NlMtrlLoadingParam& param, const char* mtrlFileName, const char* mtrlName );
	virtual bool init( NlMtrlLoadingParam& param );
	void destroy();

	NlDefShaderParam* GetShaderParamsToUpdate() {return m_shaderParam;}
	virtual void Use() = 0;

	NlDynamicShaderComb* GetShaders() {return m_shaders;}

public:
	std::string m_name;

protected:
	NlRenderDevice* m_device;
	NlDefShaderParam* m_shaderParam;
	NlDynamicShaderComb* m_shaders;

	NlTexture2D* m_textures[eMTSLOT_Max];
	
};
typedef std::vector<NlMaterial*> NlMaterialArray;

//====================================================
class NlDiffuseMtrl :
	public NlMaterial
{
public:
	NlDiffuseMtrl( NlRenderDevice* device );
	virtual ~NlDiffuseMtrl();

	virtual bool init( NlMtrlLoadingParam& param );
	virtual void Use();

protected:
	float3 m_Ka, m_Kd;

};

//====================================================
class NlSpecularMtrl :
	public NlDiffuseMtrl
{
public:
	NlSpecularMtrl( NlRenderDevice* device );
	virtual ~NlSpecularMtrl();

	virtual bool init( NlMtrlLoadingParam& param );
	virtual void Use();

protected:
	float3 m_Ks;
	float  m_glossness;

};

//====================================================
class NlTranSpecMtrl :
	public NlSpecularMtrl
{
public:
	NlTranSpecMtrl( NlRenderDevice* device );
	virtual ~NlTranSpecMtrl();

	virtual bool init( NlMtrlLoadingParam& param );
	virtual void Use();

};

// Singleton ====================================================
class NlMtrlManager
{
	friend class NlGFramework;
public:
	static NlMtrlManager& GetInstance();
	// if the material doesn't exist, create and return it; else, return it directly
	NlMaterial* GetMtrlByName( NlRenderDevice* device, 
		const char* mtrlFileName, const char* mtrlName );

private:
	NlMtrlManager() {}
	NlMtrlManager( const NlMtrlManager& another ) {}
	NlMtrlManager& operator= ( const NlMtrlManager& another ) {}
	~NlMtrlManager();

	static void Create();
	static void Destroy();

private:
	NlMaterialArray m_mtrls;

	static NlMtrlManager* instance;

};

#endif