#include "NlGameObjPri.h"
#include "NlGameEnvironment.h"
#include "nl_render_header.h"

//=================================================================================
NlCuboid::NlCuboid( float3 sides, bool isStatic /*= false */ ): 
	m_sides(sides), NlRenderble( isStatic )
{
	SetScale( sides );
}
//---------------------------------------------------------------------------------
NlCuboid::NlCuboid( float3 sides, NlTransform& transform, bool isStatic /*= false */ ):
	m_sides(sides), NlRenderble( transform, isStatic )
{
	SetScale( sides );
}
//---------------------------------------------------------------------------------
NlCuboid::~NlCuboid()
{
}
//---------------------------------------------------------------------------------
bool NlCuboid::init( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );
	assert( LoadFromFile( gameEnv->GetRenderDevice(), "Cube.obj", "Default.mtl", "Box" ) );

	name = "Cuboid";
	return true;
}
//---------------------------------------------------------------------------------
bool NlCuboid::LoadFromFile( NlRenderDevice* device, 
	const char* objFileName, const char* mtrlFileName, const char* submeshName )
{
	return NlRenderble::LoadFromFile( device, objFileName, mtrlFileName, submeshName );
}

//=================================================================================
NlSphere::NlSphere( float radius, bool isStatic /*= false */ ):
	m_radius(radius), NlRenderble( isStatic )
{
	SetScale( float3(2*m_radius) );
}
//---------------------------------------------------------------------------------
NlSphere::NlSphere( float radius, NlTransform& transform, bool isStatic /*= false */ ):
	m_radius(radius), NlRenderble( transform, isStatic )
{
	SetScale( float3(2*m_radius) );
}
//---------------------------------------------------------------------------------
NlSphere::~NlSphere()
{
}
//---------------------------------------------------------------------------------
bool NlSphere::init( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );
	assert( LoadFromFile( gameEnv->GetRenderDevice(), "Sphere.obj", "Default.mtl", "GeoSphere" ) );

	name = "Sphere";
	return true;
}
//---------------------------------------------------------------------------------
bool NlSphere::LoadFromFile( NlRenderDevice* device,
	const char* objFileName, const char* mtrlFileName, const char* submeshName )
{
	return NlRenderble::LoadFromFile( device, objFileName, mtrlFileName, submeshName );
}

//=================================================================================
NlCylinder::NlCylinder( float radius, float height, bool isStatic /*= false */ ):
	m_radius(radius), m_heigth(height), NlRenderble( isStatic )
{
	SetScale( float3(2*m_radius, m_heigth, 2*m_radius) );
}
//---------------------------------------------------------------------------------
NlCylinder::NlCylinder( float radius, float height, NlTransform& transform, bool isStatic /*= false */ ):
	m_radius(radius), m_heigth(height), NlRenderble( transform, isStatic )
{
	SetScale( float3(2*m_radius, m_heigth, 2*m_radius) );
}
//---------------------------------------------------------------------------------
NlCylinder::~NlCylinder()
{
}
//---------------------------------------------------------------------------------
bool NlCylinder::init( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );
	assert( LoadFromFile( gameEnv->GetRenderDevice(), "Cylinder.obj", "Default.mtl", "Cylinder" ) );

	name = "Cylinder";
	return true;
}
//---------------------------------------------------------------------------------
bool NlCylinder::LoadFromFile( NlRenderDevice* device, const char* objFileName, const char* mtrlFileName, const char* submeshName )
{
	return NlRenderble::LoadFromFile( device, objFileName, mtrlFileName, submeshName );
}

//=================================================================================
NlPlane::NlPlane( float width, float length, bool isStatic /*= false */ ):
	m_width(width), m_length(length), NlRenderble( isStatic )
{
	SetScale( float3(m_width, 1, m_length) );
}
//---------------------------------------------------------------------------------
NlPlane::NlPlane( float width, float length, NlTransform& transform, bool isStatic /*= false */ ):
	m_width(width), m_length(length), NlRenderble( transform, isStatic )
{
	SetScale( float3(m_width, 1, m_length) );
}
//---------------------------------------------------------------------------------
NlPlane::~NlPlane()
{
}
//---------------------------------------------------------------------------------
bool NlPlane::init( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );
	assert( LoadFromFile( gameEnv->GetRenderDevice(), "Plane.obj", "Default.mtl", "Plane" ) );

	name = "Plane";
	return true;
}
//---------------------------------------------------------------------------------
bool NlPlane::LoadFromFile( NlRenderDevice* device, const char* objFileName, const char* mtrlFileName, const char* submeshName )
{
	return NlRenderble::LoadFromFile( device, objFileName, mtrlFileName, submeshName );
}