/*
  @file   NlGameObjPri.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlGameObjPri_h_
#define _NlGameObjPri_h_

#include "NlGameObject.h"

//====================================================
class NlCuboid :
	public NlRenderble
{
public:
	NlCuboid( float3 sides, bool isStatic = false );
	NlCuboid( float3 sides, NlTransform& transform, bool isStatic = false );
	virtual ~NlCuboid();

	virtual bool init( NlGameEnvironment* gameEnv );

protected:
	bool LoadFromFile( NlRenderDevice* device,
		const char* objFileName, const char* mtrlFileName, const char* submeshName );
	void SetScale( const float3& scale ) { NlRenderble::SetScale(scale); }

protected:
	float3 m_sides;
};

//====================================================
class NlSphere :
	public NlRenderble
{
public:
	NlSphere( float radius, bool isStatic = false );
	NlSphere( float radius, NlTransform& transform, bool isStatic = false );
	virtual ~NlSphere();

	virtual bool init( NlGameEnvironment* gameEnv );

protected:
	bool LoadFromFile( NlRenderDevice* device,
		const char* objFileName, const char* mtrlFileName, const char* submeshName );
	void SetScale( const float3& scale ) { NlRenderble::SetScale(scale); }

protected:
	float m_radius;
};

//====================================================
class NlCylinder :
	public NlRenderble
{
public:
	NlCylinder( float radius, float height, bool isStatic = false );
	NlCylinder( float radius, float height, NlTransform& transform, bool isStatic = false );
	virtual ~NlCylinder();

	virtual bool init( NlGameEnvironment* gameEnv );

protected:
	bool LoadFromFile( NlRenderDevice* device,
		const char* objFileName, const char* mtrlFileName, const char* submeshName );
	void SetScale( const float3& scale ) { NlRenderble::SetScale(scale); }

protected:
	float m_radius, m_heigth;
};

//====================================================
class NlPlane :
	public NlRenderble
{
public:
	NlPlane( float width, float length, bool isStatic = false );
	NlPlane( float width, float length, NlTransform& transform, bool isStatic = false );
	~NlPlane();

	virtual bool init( NlGameEnvironment* gameEnv );

protected:
	bool LoadFromFile( NlRenderDevice* device,
		const char* objFileName, const char* mtrlFileName, const char* submeshName );
	void SetScale( const float3& scale ) { NlRenderble::SetScale(scale); }

protected:
	float m_width, m_length;

};

#endif