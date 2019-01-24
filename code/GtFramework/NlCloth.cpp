#include "NlCloth.h"
#include "NlGameEnvironment.h"

#include "nl_render_header.h"
#include "nl_physics_header.h"

#include "NxCooking.h"
#include "Stream.h"

#define TEAR_MEMORY_FACTOR 2	// extended memory for tearing
//=================================================================================
NlCloth::NlCloth( NlRenderDevice* rdDevice, NlPhysicsDevice* pxDevice )
{
	assert( rdDevice );
	assert( pxDevice );

	m_rdDevice = rdDevice;
	m_pxDevice = pxDevice;

	m_cloth = NULL;
	m_clothMesh = NULL;

	m_posVb = NULL;
	m_normVb = NULL;
	m_tcdVb = NULL;

	m_ib = NULL;
	m_vd = NULL;
	m_shaders = NULL;

	m_texCoords = NULL;
	m_texCoordCount = 0;
	m_texture = NULL;

}
//---------------------------------------------------------------------------------
NlCloth::~NlCloth( void )
{
}
//---------------------------------------------------------------------------------
void NlCloth::destroy()
{
	if (m_cloth)
	{
		m_pxDevice->ReleaseCloth( m_cloth );
		m_cloth = NULL;
	}
	if (m_clothMesh)
	{
		m_pxDevice->ReleaseClothMesh( m_clothMesh );
		m_clothMesh = NULL;
	}

	if (m_posVb)
	{
		m_posVb->destroy();
		delete m_posVb;
		m_posVb = NULL;
	}
	if (m_normVb)
	{
		m_normVb->destroy();
		delete m_normVb;
		m_normVb = NULL;
	}
	if (m_tcdVb)
	{
		m_tcdVb->destroy();
		delete m_tcdVb;
		m_tcdVb = NULL;
	}
	if (m_ib)
	{
		m_ib->destroy();
		delete m_ib;
		m_ib = NULL;
	}
	if (m_vd)
	{
		m_vd->destroy();
		delete m_vd;
		m_vd = NULL;
	}

	if (m_shaders)
	{
		m_shaders->destroy();
		delete m_shaders;
		m_shaders = NULL;
	}

	//if (m_texture)
	//{
	//	m_texture->destroy();
	//	delete m_texture;
	//	m_texture = NULL;
	//}
	if (m_texCoords)
	{
		free( m_texCoords );
		m_texCoords = NULL;
	}

}
//---------------------------------------------------------------------------------
bool NlCloth::init( NlClothDesc desc, 
	float length, float width, float quality, const char* texFileName /*= NULL */ )
{
	destroy();

	// rectify
	length = length > 1 ? length : 1;
	width = width > 1 ? width : 1;
	quality = quality >=1 ? quality : 1;
	quality = quality <= 5 ? quality : 10;

	m_color = desc.color;
	// check and create cloth texture------------------------
	bool genTexCoord = false;
	if (texFileName != NULL)
	{
		//m_texture = new NlTexture2D( m_rdDevice );
		//if (m_texture->LoadFromFile( texFileName ) == false)
		//{
		//	m_texture->destroy();
		//	delete m_texture;
		//	m_texture = NULL;
		//}		
		m_texture = NlTextureManager::GetInstance().Get2DTextureFromFile( m_rdDevice, texFileName );
		if (m_texture != NULL)
		{
			m_tcdVb = new NlVertexBuffer( m_rdDevice );
			assert( m_tcdVb );
			genTexCoord = true;
		}
	}

	// create cloth mesh-------------------------------------
	NxClothDesc clothDesc;
	float44 worldMat = float44::CreateIdentity();
	worldMat.Set( float(1), desc.globalRot, desc.globalPos );
	NxMat34 pose;
	SetupPXMat34( worldMat, pose );
	clothDesc.globalPose = pose;

	clothDesc.thickness = (desc.thickness > 0) ? desc.thickness : 0;
	clothDesc.bendingStiffness = desc.bendingStiffness;
	if (desc.lacerable == true)
	{
		clothDesc.flags |= NX_CLF_TEARABLE;
		clothDesc.tearFactor = desc.tearFactor;
	}
	if (desc.influRigidBody == true)
		clothDesc.flags |= NX_CLF_COLLISION_TWOWAY;

	clothDesc.flags |= NX_CLF_BENDING;
	clothDesc.flags |= NX_CLF_VISUALIZATION;

	NxClothMeshDesc meshDesc;
	GenerateRegularMeshDesc( meshDesc, length, width, 1/quality, genTexCoord );

	return CreateCloth( m_pxDevice->GetScene(), clothDesc, meshDesc );

}
//---------------------------------------------------------------------------------
void NlCloth::Draw( NlGameEnvironment* gameEnv )
{
	if (!m_posVb || !m_normVb || !m_ib || !m_vd || !m_shaders)
		return;
	assert( gameEnv );
	CCamera* camera = gameEnv->GetMainCamera();
	if (camera == NULL)
		return;

	//// don't cull facets
	//m_rdDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//// disable alpha-blending
	//m_rdDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

	// counter clockwise cull mode
	m_rdDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	// enable alpha-blending
	//m_rdDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_rdDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
	m_rdDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_rdDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// update and use vb & ib & vd-------------------------------
	m_vd->Use();

	NxU32 vCount = *m_receiveBuffers.numVerticesPtr;

	NxVec3* pVPos = (NxVec3*)m_receiveBuffers.verticesPosBegin;
	m_posVb->update( pVPos, vCount*sizeof(NxVec3) );
	m_posVb->Use( 0 );	// stream 0

	NxVec3* pVNorm = (NxVec3*)m_receiveBuffers.verticesNormalBegin;
	m_normVb->update( pVNorm, vCount*sizeof(NxVec3) );
	m_normVb->Use( 1 );	// stream 1 

	if (m_texCoords != NULL)
	{
		UpdateTextureCoordinates();
		assert( m_tcdVb );
		m_tcdVb->update( m_texCoords, m_texCoordCount*sizeof(float2) );
		m_tcdVb->Use( 2 );	//stream 2

		m_rdDevice->SetGlobalPSFloat( 4, 1 );	// has texture
		m_texture->Use( 0 );
	}
	else
		m_rdDevice->SetGlobalPSFloat( 4, 0 );


	NxU32 iCount = *m_receiveBuffers.numIndicesPtr;
	NxU32* pVIdx = (NxU32*)m_receiveBuffers.indicesBegin;
	m_ib->update( pVIdx, iCount*sizeof(NxU32) );
	m_ib->Use();

	// set shader constants-----------------------------------------
	m_rdDevice->SetGlobalVSMatrix( 0, camera->GetViewProjectionMatrix() );	// wvp matrix
	m_rdDevice->SetGlobalVSMatrix( 4, float44::CreateIdentity() );	// world matrix
	m_rdDevice->SetGlobalPSFloat4( 0, float4(gameEnv->m_lightDir, 1) );
	m_rdDevice->SetGlobalPSFloat4( 1, float4(camera->GetPosition(), 1) );

	m_rdDevice->SetGlobalPSFloat4( 2, m_color );	// ka
	m_rdDevice->SetGlobalPSFloat4( 3, m_color );	// kd

	// draw----------------------------------------------------------
	m_rdDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vCount, 0, iCount/3 );

}

//---------------------------------------------------------------------------------
void NlCloth::GenerateRegularMeshDesc( NxClothMeshDesc& meshDesc, 
	NxReal len, NxReal wid, NxReal dif, bool genTexCoord /*= false */ )
{
	int numX = (int)(len/dif) + 1;
	int numY = (int)(wid/dif) + 1;
	if (numX % 2 != 0)
		numX += 1;
	if (numY % 2 != 0)
		numY += 1;

	meshDesc.numVertices			= (numX+1) * (numY+1);
	meshDesc.numTriangles			= numX * numY * 2;
	meshDesc.pointStrideBytes		= sizeof( NxVec3 );
	meshDesc.triangleStrideBytes	= 3 * sizeof( NxU32 );
	meshDesc.vertexMassStrideBytes	= sizeof( NxReal );
	meshDesc.vertexFlagStrideBytes	= sizeof( NxU32 );
	meshDesc.points					= (NxVec3*)malloc( sizeof(NxVec3)*meshDesc.numVertices );	assert( meshDesc.points );
	meshDesc.triangles				= (NxU32*)malloc( sizeof(NxU32)*meshDesc.numTriangles*3 );	assert( meshDesc.triangles );
	meshDesc.vertexMasses			= 0;
	meshDesc.vertexFlags			= 0;
	meshDesc.flags					= 0;

	// generate vertex positions
	{
		NxVec3* pBuf = (NxVec3*)meshDesc.points;
		for (int y = -numY*0.5f; y <= numY*0.5f; ++y)
			for (int x = -numX*0.5f; x <= numX*0.5f; ++x)
			{
				pBuf->set( -x*dif, 0.0f, -y*dif );
				++pBuf;
			}
	}

	// generate texcoord
	if (genTexCoord)
	{
		m_texCoords = (float2*)malloc( sizeof(float2)*TEAR_MEMORY_FACTOR*meshDesc.numVertices );
		float2* pBuf = m_texCoords;
		float dx = 1.0f / numX;
		float dy = 1.0f / numY;

		for (int y = 0; y <= numY; ++y)
			for (int x = 0; x <= numX; ++x)
			{
				(*pBuf).x = x * dx;
				(*pBuf).y = y * dy;
				++pBuf;
			}

			m_texCoordCount = meshDesc.numVertices;
	}
	else
		m_texCoordCount = 0;

	// generate vertex indices
	{
		NxU32* pBuf = (NxU32*)meshDesc.triangles;
		for (int y = 0; y < numY; ++y)
			for (int x = 0; x < numX; ++x)
			{
				NxU32 id0 = y * (numX+1) + x;
				NxU32 id1 = id0 + 1;
				NxU32 id2 = id0 + (numX+1);
				NxU32 id3 = id2 + 1;
				if ((x+y)%2)
				{
					*pBuf++ = id0; *pBuf++ = id2; *pBuf++ = id1;
					*pBuf++ = id1; *pBuf++ = id2; *pBuf++ = id3;
				}
				else
				{
					*pBuf++ = id0; *pBuf++ = id2; *pBuf++ = id3;
					*pBuf++ = id0; *pBuf++ = id3; *pBuf++ = id1;
				}
			}
	}

	// generate tear lines if necessary
	//if (lacerable == true)
	//	GenerateTearLines( meshDesc, numX+1, numY+1 );

}

//---------------------------------------------------------------------------------
//void NlCloth::GenerateTearLines( NxClothMeshDesc& meshDesc, NxU32 len, NxU32 wid )
//{
//	// allocate flag buffer
//	if (meshDesc.vertexFlags == 0)
//		meshDesc.vertexFlags = malloc( sizeof(NxU32)*meshDesc.numVertices );
//
//	// create tear lines
//	NxU32* pBuf = (NxU32*)meshDesc.vertexFlags;
//	for (NxU32 y = 0; y < wid; ++y)
//		for (NxU32 x = 0; x < len; ++x)
//		{
//			if (((x + y) % 16 == 0 || (y - x + 16) % 16 == 0))
//				pBuf[y * len + x] = NX_CLOTH_VERTEX_TEARABLE;
//			else
//				pBuf[y * len + x] = 0;
//		}
//
//}
//---------------------------------------------------------------------------------
bool NlCloth::CreateCloth( NxScene* scene, NxClothDesc& clothDesc, NxClothMeshDesc& meshDesc )
{
	// if need tearing we must tell the cooker
	// it will generate some space for particles that will be generate during tearing
	if (clothDesc.flags & NX_CLF_TEARABLE)
		meshDesc.flags |= NX_CLOTH_MESH_TEARABLE;

	// create cloth------------------------------------------------
	assert( NxInitCooking() == true ); 
		if (CookClothMesh( meshDesc ) == false)
			return false;
	NxCloseCooking();

	ReleaseMeshDescBuffers( meshDesc );
	AllocateReceiveBuffers( meshDesc.numVertices, meshDesc.numTriangles );

	clothDesc.clothMesh = m_clothMesh;
	clothDesc.meshData = m_receiveBuffers;
	m_cloth = m_pxDevice->GetScene()->createCloth( clothDesc );

	// create and initialize render components---------------------
	// create and initialize vb
	m_posVb = new NlVertexBuffer( m_rdDevice );		assert( m_posVb );
	m_posVb->init( sizeof(NxVec3), m_receiveBuffers.maxVertices );

	m_normVb = new NlVertexBuffer( m_rdDevice );	assert( m_normVb );
	m_normVb->init( sizeof(NxVec3), m_receiveBuffers.maxVertices );

	if (m_tcdVb != NULL)
		m_tcdVb->init( sizeof(NxVec3), m_receiveBuffers.maxVertices );

	// create and initialize ib
	m_ib = new NlIndexBuffer( m_rdDevice );			assert( m_ib );
	m_ib->init( sizeof(NxU32), m_receiveBuffers.maxIndices );

	// create and initialize vd, use multiple streams
	m_vd = new NlVertexDecl( m_rdDevice );			assert( m_vd );
	VertexElemArray elements;
	{
		NlVertexElem elem( 0, eVEU_Position, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 1, eVEU_Normal, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 2, eVEU_Texcoord, 0 );
		elements.push_back( elem );
	}
	m_vd->init( elements );

	// create and compile shaders
	m_shaders = new NlDynamicShaderComb( m_rdDevice );	assert( m_shaders );
	assert( m_shaders->CompileFromFile( "PlasticCloth.shader" ) == true );

	return true;

}
//---------------------------------------------------------------------------------
bool NlCloth::CookClothMesh( NxClothMeshDesc& meshDesc )
{
	// cool the mesh on the fly through a memory stream
	MemoryWriteBuffer wb;
	if (NxCookClothMesh(meshDesc, wb) == false)
		return false;

	MemoryReadBuffer rb(wb.data);
	m_clothMesh = m_pxDevice->GetSDK()->createClothMesh( rb );
	
	return (m_clothMesh != NULL);

}
//---------------------------------------------------------------------------------
void NlCloth::ReleaseMeshDescBuffers( const NxClothMeshDesc& meshDesc )
{
	NxVec3* pV = (NxVec3*)meshDesc.points;
	NxU32* pTri = (NxU32*)meshDesc.triangles;
	NxReal* pMass = (NxReal*)meshDesc.vertexMasses;
	NxU32* pFlag = (NxU32*)meshDesc.vertexFlags;

	free( pV );
	free( pTri );
	free( pMass );
	free( pFlag );

}
//---------------------------------------------------------------------------------
void NlCloth::AllocateReceiveBuffers( int vertexCount, int triangleCount )
{
	// setup the buffers through which the SDK returns the dynamic cloth data
	// we need to reserve more memory for vertices than initial mesh takes
	// because tearing creates new vertices
	// the SDK only tears cloth when there is room in these buffers

	// note: the cloth physics mesh data will be release by physics simulator
	NxU32 maxVCount = TEAR_MEMORY_FACTOR * vertexCount;
	m_receiveBuffers.verticesPosBegin = (NxVec3*)malloc( sizeof(NxVec3)*maxVCount );	assert( m_receiveBuffers.verticesPosBegin );
	m_receiveBuffers.verticesNormalBegin = (NxVec3*)malloc( sizeof(NxVec3)*maxVCount );	assert( m_receiveBuffers.verticesNormalBegin );
	m_receiveBuffers.verticesPosByteStride = sizeof( NxVec3 );
	m_receiveBuffers.verticesNormalByteStride = sizeof( NxVec3 );
	m_receiveBuffers.maxVertices = maxVCount;
	m_receiveBuffers.numVerticesPtr = (NxU32*)malloc( sizeof(NxU32) );					assert( m_receiveBuffers.numVerticesPtr );

	// the number of triangles is constant, even if the cloth is torn
	NxU32 maxICount = 3 * triangleCount;
	m_receiveBuffers.indicesBegin = (NxU32*)malloc( sizeof(NxU32)*maxICount );			assert( m_receiveBuffers.indicesBegin );
	m_receiveBuffers.indicesByteStride = sizeof( NxU32 );
	m_receiveBuffers.maxIndices = maxICount;
	m_receiveBuffers.numIndicesPtr = (NxU32*)malloc( sizeof(NxU32) );					assert( m_receiveBuffers.numIndicesPtr );

	// the parent index information would be needed if we used textured cloth
	NxU32 maxPICount = maxVCount;
	m_receiveBuffers.parentIndicesBegin = (NxU32*)malloc( sizeof(NxU32)*maxPICount );	assert( m_receiveBuffers.parentIndicesBegin );
	m_receiveBuffers.parentIndicesByteStride = sizeof( NxU32 );
	m_receiveBuffers.maxParentIndices = maxPICount;
	m_receiveBuffers.numParentIndicesPtr = (NxU32*)malloc( sizeof(NxU32) );				assert( m_receiveBuffers.numParentIndicesPtr );

	// initialize the buffers in case of drawing the mesh before the SDK as filled in the correct values
	*m_receiveBuffers.numVerticesPtr = 0;
	*m_receiveBuffers.numIndicesPtr = 0;
	
}
//---------------------------------------------------------------------------------
void NlCloth::UpdateTextureCoordinates()
{
	// set texcoord for tearing particle
	NxU32 vCount = *m_receiveBuffers.numVerticesPtr;
	NxU32* parent = (NxU32*)m_receiveBuffers.parentIndicesBegin + m_texCoordCount;

	for (NxU32 i = m_texCoordCount; i < vCount; ++i, ++parent)
	{
		m_texCoords[i].x = m_texCoords[(*parent)].x;
		m_texCoords[i].y = m_texCoords[(*parent)].y;
	}

	m_texCoordCount = vCount;

}