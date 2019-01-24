/*
  @file   NlDebugRenderer.h
  
  @author Liaosheng

  @brief  render_debug for PhysX

  更改日志 history
  ver:1.0
 */

#ifndef _NlDebugRenderer_h_
#define _NlDebugRenderer_h_

#include "prerequisite.h"

class NlRenderDevice;
class NlVertexDecl;
class NlDynamicShaderComb;

class CCamera;
class NxDebugRenderable;

// Singleton ====================================================
class NlDebugRenderer
{
	friend class NlGFramework;
public:
	static void SetCamera( CCamera* camera );

	static void RenderData( const NxDebugRenderable& data );

	static void DrawLine( const float3& p0, const float3& p1, const float3& color );
	static void DrawLine( const NxVec3& p0, const NxVec3& p1, const float3& color );
	static void DrawLine( const float3& locP0, const float3& locP1, const float44& worldMat, const float3& color );
	static void DrawLine( const NxVec3& locP0, const NxVec3& locP1, const float44& worldMat, const float3& color );

	static void DrawCircle( const uint32 segmentCount, const float44& worldMat, const float3& color,
		const float radius, const bool semicircle = false );
	static void DrawCircle( const uint32 segmentCount, const NxMat34& worldMat, const float3& color,
		const float radius, const bool semicircle = false );

	static void DrawArrow( const float3& posA, const float3& posB, const float3& color );
	static void DrawArrow( const NxVec3& posA, const NxVec3& posB, const float3& color );

	static void DrawForce( NxActor* actor, NxVec3& forceVec, const float3& color );

	static void DrawCoordSystem( const float3& pos, const Quat& rot );
	static void DrawCoordSystem( const NxMat34& pose );
	static void DrawCoordSystem( NxActor* actor );

	static void DrawWireShape( NxShape* shape, const float3& color );
private:
	static void DrawWireBox( NxShape* box, const float3& color );
	static void DrawWireSphere( NxShape* sphere, const float3& color );
	static void DrawWireCapsule( NxShape* capsule, const float3& color );
	static void DrawWireMesh( NxShape* mesh, const float3& color );

private:
	static void Create( NlRenderDevice* device );
	static void Destroy();

	void init();
	static void RenderBuffer( float* buf, uint32 primType, int primCount );

	NlDebugRenderer( NlRenderDevice* device );
	NlDebugRenderer( const NlDebugRenderer& another );
	NlDebugRenderer& operator= ( const NlDebugRenderer& another );
	~NlDebugRenderer();

private:
	NlRenderDevice* m_device;
	CCamera* m_camera;

	NlVertexDecl* m_vd;
	NlDynamicShaderComb* m_shaders;

	static NlDebugRenderer* instance;

};

#endif