/*
  @file   NlCloth.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlCloth_h_
#define _NlCloth_h_

#include "prerequisite.h"

typedef struct _NlClothDesc
{
	float4 color;	// rgba

	float  thickness;			// range 0-infinite
	float  bendingStiffness;	// range 0-1

	bool   lacerable;
	float  tearFactor;			// valid when cloth is lacerable
	bool   influRigidBody;

	float3 globalPos;
	Quat   globalRot;

	_NlClothDesc()
	{
		color = float4(0.784, 0.784, 0.882, 1);
		thickness = 0.2f;
		bendingStiffness = 0.0f;

		lacerable = false;
		tearFactor = 2.0f;
		influRigidBody= false;

		globalPos = float3( 0 );
		globalRot = Quat::CreateIdentity();
	}

} NlClothDesc;

class NlRenderDevice;
class NlPhysicsDevice;

class NlVertexBuffer;
class NlIndexBuffer;
class NlVertexDecl;
class NlDynamicShaderComb;
class NlTexture2D;

class NlGameEnvironment;

class NlCloth
{
public:
	NlCloth( NlRenderDevice* rdDevice, NlPhysicsDevice* pxDevice );
	~NlCloth(void);

	// create a regular cloth
	// length: range 1~inf;	width: range 1~inf	quality: range 1~10
	bool init( NlClothDesc desc,
		float length, float width,	float quality, const char* texFileName = NULL );

	void Draw( NlGameEnvironment* gameEnv );
	void destroy();
	NxCloth* GetPhysicsCloth() {return m_cloth;}

private:
	void GenerateRegularMeshDesc( NxClothMeshDesc& meshDesc, 
		NxReal len, NxReal wid, NxReal dif, bool genTexCoord = false );
	//void GenerateTearLines( NxClothMeshDesc& meshDesc, NxU32 len, NxU32 wid );
	bool CreateCloth( NxScene* scene, NxClothDesc& clothDesc, NxClothMeshDesc& meshDesc );
	bool CookClothMesh( NxClothMeshDesc& meshDesc );
	void ReleaseMeshDescBuffers( const NxClothMeshDesc& meshDesc );

	void AllocateReceiveBuffers( int vertexCount, int triangleCount );
	void UpdateTextureCoordinates();

private:
	NlRenderDevice* m_rdDevice;
	NlPhysicsDevice* m_pxDevice;

	NxCloth* m_cloth;
	NxClothMesh* m_clothMesh;
	NxMeshData m_receiveBuffers;

	NlVertexBuffer* m_posVb;
	NlVertexBuffer* m_normVb;
	NlVertexBuffer* m_tcdVb;
	NlIndexBuffer*  m_ib;
	NlVertexDecl*   m_vd;

	NlDynamicShaderComb* m_shaders;
	float4	m_color;
	NlTexture2D*	m_texture;

	float2* m_texCoords;
	uint32 m_texCoordCount;

};

#endif