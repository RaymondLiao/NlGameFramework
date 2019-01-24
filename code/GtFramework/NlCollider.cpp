#include "nl_physics_header.h"
#include "nl_render_header.h"

#include "NxCooking.h"
#include "Stream.h"

#include "GtFile.h"
#include <sstream>

const float3 COLLIDER_COLOR( 0.7f, 1.0f, 0.5f );

//=================================================================================
NlCollider::NlCollider( NlRigidBody* attachedBody )
{ 
	assert( attachedBody );
	m_pxShape = NULL;
	m_attachedRigBody = attachedBody;
	m_eventDele = NULL;

	m_initialized = false;

	m_isTrigger = false;
	m_id = 0;
}

NlCollider::NlCollider( NlRigidBody* attachedBody, const float3& locPos, const Quat& locRot )
{
	assert( attachedBody );
	m_attachedRigBody = attachedBody;
	m_eventDele = NULL;

	float44 localMat = float44::CreateIdentity();
	localMat.Set( float3(1), locRot, locPos );
	SetupPXMat34( localMat, m_locPose );

	m_initialized = false;

	m_isTrigger = false;
	m_id = 0;
}
//---------------------------------------------------------------------------------
NlCollider::~NlCollider( void )
{
}
//---------------------------------------------------------------------------------
float3 NlCollider::GetGlobalPos()
{
	assert( m_pxShape == NULL );
	return CovToFloat3( m_pxShape->getGlobalPosition() );
		
}
//---------------------------------------------------------------------------------
void NlCollider::PostColliderEvent( NlCollider* another, ENlClidEvent msg )
{
	if (m_eventDele == NULL)
		return;

	assert( another );
	switch (msg)
	{
	case eCLID_ColEnter:
		m_eventDele->OnCollisionEnter( *another );
		break;
		
	case eCLID_ColExit:
		m_eventDele->OnCollisionExit( *another );
		break;

	case eCLID_ColStay:
		m_eventDele->OnCollisionStay( *another );
		break;

	case eCLID_TriEnter:
		m_eventDele->OnTriggerEnter( *another );
		break;

	case eCLID_TriExit:
		m_eventDele->OnTriggerExit( *another );
		break;

	case eCLID_TriStay:
		m_eventDele->OnTriggerStay( *another );
		break;
	}
}
//---------------------------------------------------------------------------------
void NlCollider::Draw()
{
	assert( m_attachedRigBody );
	NxActor* pxActor = m_attachedRigBody->m_pxActor;
	assert( pxActor );

	NlDebugRenderer::DrawWireShape( pxActor->getShapes()[m_id], COLLIDER_COLOR );
	//NlDebugRenderer::DrawWireShape( pxActor->getShapes()[0], float3(1.0f, 1.0f, 1.0f) );
}
//=================================================================================
NlBoxCollider::NlBoxCollider( NlRigidBody* attachedBody ) : NlCollider( attachedBody )
{
}

NlBoxCollider::NlBoxCollider( NlRigidBody* attachedBody, const float3& locPos, const Quat& locRot ):
	NlCollider( attachedBody, locPos, locRot )
{
}
//---------------------------------------------------------------------------------
NlBoxCollider::~NlBoxCollider()
{
}
//---------------------------------------------------------------------------------
bool NlBoxCollider::init( float3 sides, uint16 mtrlIndex /*= 0*/, bool isTrigger /*= false */ )
{
	// avoid initialize repeatedly
	if (m_initialized)
		return true;
	m_initialized = true;

	assert( m_attachedRigBody );

	NxActor* pxActor = m_attachedRigBody->m_pxActor;
	assert( pxActor );

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3( sides.x*0.5f, sides.y*0.5f, sides.z*0.5f );
	boxDesc.materialIndex = mtrlIndex;
	boxDesc.localPose = m_locPose;
	if (isTrigger)
	{
		boxDesc.shapeFlags |= NX_TRIGGER_ENABLE;
		m_isTrigger = true;
	}

	// add a new collider to rigid body
	if ((m_pxShape = pxActor->createShape( boxDesc )) == NULL)
		return false;
	m_attachedRigBody->m_collidersCount++;
	m_id = m_attachedRigBody->m_collidersCount;

	return true;
}

//=================================================================================
NlSphereCollider::NlSphereCollider( NlRigidBody* attachedBody ): NlCollider( attachedBody )
{
}

NlSphereCollider::NlSphereCollider( NlRigidBody* attachedBody, const float3& locPos, const Quat& locRot ):
	NlCollider( attachedBody, locPos, locRot )
{
}
//---------------------------------------------------------------------------------
NlSphereCollider::~NlSphereCollider()
{
}
//---------------------------------------------------------------------------------
bool NlSphereCollider::init( float radius, uint16 mtrlIndex /*= 0*/, bool isTrigger /*= false */ )
{
	// avoid initialize repeatedly
	if (m_initialized)
		return true;
	m_initialized = true;

	assert( m_attachedRigBody );

	NxActor* pxActor = m_attachedRigBody->m_pxActor;
	assert( pxActor );

	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius;
	sphereDesc.materialIndex = mtrlIndex;
	sphereDesc.localPose = m_locPose;
	if (isTrigger)
	{
		sphereDesc.shapeFlags |= NX_TRIGGER_ENABLE;
		m_isTrigger = true;
	}

	// add a new collider to rigid body
	if ((m_pxShape = pxActor->createShape( sphereDesc )) == NULL)
		return false;
	m_attachedRigBody->m_collidersCount++;
	m_id = m_attachedRigBody->m_collidersCount;

	return true;
}

//=================================================================================
NlCapsuleCollider::NlCapsuleCollider( NlRigidBody* attachedBody ): NlCollider( attachedBody )
{
}

NlCapsuleCollider::NlCapsuleCollider( NlRigidBody* attachedBody, const float3& locPos, const Quat& locRot ):
	NlCollider( attachedBody, locPos, locRot )
{
}
//---------------------------------------------------------------------------------
NlCapsuleCollider::~NlCapsuleCollider()
{
}
//---------------------------------------------------------------------------------
bool NlCapsuleCollider::init( float radius, float height, uint16 mtrlIndex /*= 0*/, bool isTrigger /*= false */ )
{
	// avoid initialize repeatedly
	if (m_initialized)
		return true;
	m_initialized = true;

	assert( m_attachedRigBody );

	NxActor* pxActor = m_attachedRigBody->m_pxActor;
	assert( pxActor );

	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.radius = radius;
	capsuleDesc.height = height;
	capsuleDesc.materialIndex = mtrlIndex;
	capsuleDesc.localPose = m_locPose;
	if (isTrigger)
	{
		capsuleDesc.shapeFlags |= NX_TRIGGER_ENABLE;
		m_isTrigger = true;
	}

	// add a new collider to rigid body
	if ((m_pxShape = pxActor->createShape( capsuleDesc )) == NULL)
		return false;
	m_attachedRigBody->m_collidersCount++;
	m_id = m_attachedRigBody->m_collidersCount;

	return true;
}

//=================================================================================
std::vector<float3> NlMeshCollider::vPos;
NlMeshClidData NlMeshCollider::loadedData;
std::vector<NlMeshCollider::CacheEntry*> NlMeshCollider::vertexCache;
NlMeshCollider::CacheEntry* NlMeshCollider::nullCache;
//---------------------------------------------------------------------------------
NlMeshCollider::NlMeshCollider( NlRigidBody* attachedBody ): NlCollider( attachedBody )
{
	m_meshDesc = NULL;
}

NlMeshCollider::NlMeshCollider( NlRigidBody* attachedBody, const float3& locPos, const Quat& locRot ):
	NlCollider( attachedBody, locPos, locRot )
{
	m_meshDesc = NULL;
}
//---------------------------------------------------------------------------------
NlMeshCollider::~NlMeshCollider()
{
	assert( m_attachedRigBody );
	if (m_meshDesc)
	{
		delete m_meshDesc;
		m_meshDesc = NULL;
	}
}
//---------------------------------------------------------------------------------
bool NlMeshCollider::init( NlMeshClidData& data, uint16 mtrlIndex /*= 0*/, bool isTrigger /*= false */ )
{
	// avoid initialize repeatedly
	if (m_initialized)
		return true;
	m_initialized = true;

	assert( m_attachedRigBody );

	NxActor* pxActor = m_attachedRigBody->m_pxActor;
	assert( pxActor );

	if (!m_meshDesc)
	{
		m_meshDesc = new NxTriangleMeshDesc;
		assert( m_meshDesc );
	}

	// get mesh data
	float3* vData = new float3[data._vCount];
	memcpy( vData, data._vertices.data(), data._vCount*sizeof(float3) );
	uint32* iData = new uint32[data._iCount];
	memcpy( iData, data._indices.data(), data._iCount*sizeof(uint32) );

	m_meshDesc->flags = 0;
	m_meshDesc->numVertices = data._vCount;
	m_meshDesc->pointStrideBytes = sizeof( float3 );
	m_meshDesc->points = vData;
	m_meshDesc->numTriangles = data._iCount / 3;
	m_meshDesc->triangleStrideBytes = 3 * sizeof(uint32);
	m_meshDesc->triangles = iData;
	
	// the pxActor has one shape, a triangle mesh
	NxInitCooking();
		MemoryWriteBuffer buf;

		bool status = NxCookTriangleMesh( *m_meshDesc, buf );
		NxTriangleMesh* pMesh;
		if (status)
		{
			NxPhysicsSDK* sdk_obj = m_attachedRigBody->m_device->GetSDK();
			assert( sdk_obj );
			pMesh = sdk_obj->createTriangleMesh( MemoryReadBuffer(buf.data) );
		}
		else
		{
			MessageBox( NULL, "Cook tri-mesh failed", "error", MB_OK );

			//// placeholder shape
			//NxSphereShapeDesc sphereDesc;
			//sphereDesc.radius = 1.0f;
			//sphereDesc.materialIndex = 0;
			//sphereDesc.localPose.t = NxVec3( 0.0f, 0.0f, 0.0f );
			//sphereDesc.shapeFlags |= NX_SF_DISABLE_COLLISION;
			//pxActor->createShape( sphereDesc );

			return false;
		}
	NxCloseCooking();
	// clear up
	delete[] vData;
	delete[] iData;

	// create triangle mesh above code segment
	NxTriangleMeshShapeDesc tmsDesc;
	tmsDesc.meshData = pMesh;
	tmsDesc.userData = m_meshDesc;
	tmsDesc.localPose = m_locPose;
	tmsDesc.meshPagingMode = NX_MESH_PAGING_AUTO;
	if (isTrigger)
	{
		tmsDesc.shapeFlags |= NX_TRIGGER_ENABLE;
		m_isTrigger = true;
	}

	if (!tmsDesc.isValid() || !pMesh)
	{
		MessageBox( NULL, "create mesh collider failed", "error", MB_OK );
		return false;
	}

	// add a new collider to rigid body
	if((m_pxShape = m_attachedRigBody->m_pxActor->createShape( tmsDesc )) == NULL)
		return false;
	m_attachedRigBody->m_collidersCount++;
	m_id = m_attachedRigBody->m_collidersCount;

	// save mesh in userData for drawing
	pMesh->saveToDesc( *m_meshDesc );

	return true;
}

//---------------------------------------------------------------------------------
// used for loading****************************************************************
bool NlMeshCollider::LoadMeshFromFile( NlMeshClidData& data, const char* objFileName, const char* submeshName /*= NULL */ )
{
	bool res = false;

	std::string path = ASSETS_PATH + objFileName;
	GtFile file( path.c_str() );
	file.Open();
	if (file.IsOpen())
	{
		res = LoadMeshFromData( data, file.Data(), submeshName );
	}
	file.Close();

	return res;
}
//-------------------------------------------
bool NlMeshCollider::LoadMeshFromData( NlMeshClidData& data, const char* pMeshData, const char* submeshName /*= NULL */ )
{
	data.clear();
	bool loadAll = (submeshName == NULL);

	char strCommand[256] = {0};
	std::stringstream inFile( pMeshData );
	if (!inFile)
		return false;

	std::string currMeshName;
	bool flushFace = false;

	for (; ;)
	{
		inFile >> strCommand;
		if (!inFile)
			break;

		if (0 == strcmp( strCommand, ("g") ))
		{
			inFile >> currMeshName;
			flushFace = true;
		}
		else if (0 == strcmp( strCommand, ("v") ))
		{
			if (!loadAll && flushFace)
			{
				flushFace = false;

				if (currMeshName == submeshName)
				{
					// finish loading
					data = loadedData;
					ClearUp();
					return true;
				}
				else
				{
					data.clear();
					ClearUp();
				}
			}

			// get v_pos info
			float x, y, z;
			inFile >> x >> y >> z;
			vPos.push_back( float3(x, y, z) ); 
		}
		else if (0 == strcmp( strCommand, ("f") ))
		{
			int posIndex, tcdIndex, normIndex;
			float3 vertex;
			uint32 indices[3];

			for (uint32 v = 0; v < 3; ++v)
			{
				vertex = float3( 0 );

				// get pos index
				inFile >> posIndex;
				posIndex = abs( posIndex );
				vertex = vPos[posIndex - 1];	// OBJ format uses 1-based arrays

				// skip other indices
				if ('/' == inFile.peek())
				{
					inFile.ignore();
					if ('/' != inFile.peek())
					{
						inFile >> tcdIndex;
					}
					if ('/' == inFile.peek())
					{
						inFile.ignore();
						inFile >> normIndex;
					}
				}

				indices[v] = AddVertex( posIndex, &vertex );
			}

			loadedData._indices.push_back( indices[0] );
			loadedData._indices.push_back( indices[1] );
			loadedData._indices.push_back( indices[2] );
			loadedData._iCount += 3;
		}

		// swallow the whole line
		inFile.ignore( 1000, '\n' );
	}

	// deal with the last group
	if (!loadAll && flushFace)
	{
		flushFace = false;

		if (currMeshName == submeshName)
		{
			// finish loading
			data = loadedData;
			ClearUp();
			return true;
		}
		else
		{
			data.clear();
			ClearUp();
		}
	}

	// load all mesh in the file?
	if (loadAll)
	{
		data = loadedData;
		ClearUp();
		return true;
	}

	ClearUp();
	return false;
}
//-------------------------------------------
WORD NlMeshCollider::AddVertex( uint32 key, float3* pVertex )
{
	bool foundInList = false;
	WORD index = 0;

	if ((uint32)vertexCache.size() > key)
	{
		CacheEntry *pEntry = vertexCache[key];
		while (pEntry != NULL)
		{
			float3 *pCacheVertex = loadedData._vertices.data() + pEntry->index;

			if (0 == memcmp( pVertex, pCacheVertex, sizeof(float3) ))
			{
				foundInList = true;
				index = pEntry->index;
				break;
			}

			pEntry = pEntry->pNext;
		}
	}

	if (!foundInList)
	{
		// add a new vertex
		index = loadedData._vertices.size();
		loadedData._vertices.push_back( *pVertex );
		loadedData._vCount++;

		// add to the hash-table
		CacheEntry *pNewEntry = new CacheEntry;
		assert( pNewEntry );

		pNewEntry->index = index;
		pNewEntry->pNext = NULL;

		// grow the cache if needed
		if ((uint32)vertexCache.size() <= key)
		{
			vertexCache.resize( key+1, nullCache );
		}

		// add to the end of the hash-table
		CacheEntry *pCurEntry = vertexCache[key];
		if (pCurEntry == NULL)	// it's the head element
		{
			vertexCache[key] = pNewEntry;
		}
		else
		{
			// find the tail
			while(pCurEntry->pNext != NULL)
			{
				pCurEntry = pCurEntry->pNext;
			}

			pCurEntry->pNext = pNewEntry;
		}
	}

	return index;
}
//-------------------------------------------
void NlMeshCollider::ClearUp()
{
	loadedData.clear();

	for (uint32 i = 0; i < vertexCache.size(); ++i)
	{
		CacheEntry *pEntry = vertexCache[i];
		while (pEntry != NULL)
		{
			CacheEntry *pNext = pEntry->pNext;
			delete pEntry;
			pEntry = pNext;
		}
	}
	vertexCache.clear();
}