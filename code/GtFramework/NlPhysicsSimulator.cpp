#include "stableheader.h"
#include "NlPhysicsSimulator.h"
#include "NlGameEnvironment.h"
#include "NlDebugRenderer.h"
#include "NlPxPicker.h"

// lead in PhysX
#pragma comment(lib, "physx_2.8_sdk/lib/win32/Physxloader.lib")
#pragma comment(lib, "physx_2.8_sdk/lib/win32/physxcooking.lib")
#pragma comment(lib, "physx_2.8_sdk/lib/win32/nxcharacter.lib")

//=================================================================================
NlPhysicsSimulator::NlPhysicsSimulator( float gravitySize /*= 9.8f*/, float paramScale /*= 1.0f */ ):
	m_grvSize(gravitySize), m_paramScale(paramScale)
{
	m_device = NULL;
}
//---------------------------------------------------------------------------------
NlPhysicsSimulator::~NlPhysicsSimulator(void)
{
}
//---------------------------------------------------------------------------------
bool NlPhysicsSimulator::Init()
{
	// clear up
	ShutDown();

	m_device = new NlPhysicsDevice();
	assert( m_device );

	if (m_device->init( m_grvSize, m_paramScale ))
	{
		return true;
	}
	else
	{
		m_device->destroy();
		delete m_device;
		m_device = NULL;
		return false;
	}
}
//---------------------------------------------------------------------------------
void NlPhysicsSimulator::Start()
{
	// start the first frame of the simulation
	StartPhysics( m_device->m_elapsedTime );
}
//---------------------------------------------------------------------------------
void NlPhysicsSimulator::Update( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );
	if (m_device)
	{
		GetPhysicsResults();

		// clear tasks
		ClearGarbage();

		// check if reset device
		if (m_device->m_reset == true)
		{
			assert( m_device->init( m_grvSize, m_paramScale ) == true );
			m_device->m_reset = false;
		}

		// update device
		m_device->update( gameEnv->GetElapsedTime() );

		// debug render
		if (gameEnv->IfDrawForDebug())
		{
			const NxDebugRenderable* pData = m_device->m_scene->getDebugRenderable();
			if (pData)
				NlDebugRenderer::RenderData( *pData );
		}
	}

}
//---------------------------------------------------------------------------------
void NlPhysicsSimulator::ShutDown()
{
	if (m_device)
	{
		GetPhysicsResults();	// make sure to fetchResults() before shutting down
		ClearGarbage();
		m_device->destroy();
		delete m_device;
		m_device = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlPhysicsSimulator::StartPhysics( float elapsedTime )
{
	// start collision and dynamics for delta time since the last frame
	m_device->m_scene->simulate( elapsedTime );
	m_device->m_scene->flushStream();
}
//---------------------------------------------------------------------------------
void NlPhysicsSimulator::GetPhysicsResults()
{
	// get results from m_scene->simulate( elapsedTime )
	while (!m_device->m_scene->fetchResults( NX_RIGID_BODY_FINISHED, false ));
}
//---------------------------------------------------------------------------------
void NlPhysicsSimulator::ClearGarbage()
{
	// clear actors
	{
		std::vector<NxActor*>::iterator it;
		for (it = m_device->m_actorGarbage.begin(); it != m_device->m_actorGarbage.end(); ++it)
		{
			if (*it)
				m_device->m_scene->releaseActor( *(*it) );
		}

		m_device->m_actorGarbage.clear();
	}
	
	// clear joints
	{
		std::vector<NxJoint*>::iterator it;
		for (it = m_device->m_jointGarbage.begin(); it != m_device->m_jointGarbage.end(); ++it)
		{
			if (*it)
				m_device->m_scene->releaseJoint( *(*it) );
		}

		m_device->m_jointGarbage.clear();
	}

	// clear cloths
	{
		std::vector<NxCloth*>::iterator it;
		for (it = m_device->m_clothGarbage.begin(); it != m_device->m_clothGarbage.end(); ++it)
		{
			if (*it)
			{
				NxMeshData& meshData = (*it)->getMeshData();
				m_device->m_scene->releaseCloth( *(*it) );
				ReleasePxMeshData( meshData );
			}
		}
		m_device->m_clothGarbage.clear();
	}
	// clear cloth meshes
	{
		std::vector<NxClothMesh*>::iterator it;
		for (it = m_device->m_clothMeshGarbage.begin(); it != m_device->m_clothMeshGarbage.end(); ++it)
		{
			if (*it)
				m_device->m_sdk_obj->releaseClothMesh( *(*it) );
		}
		m_device->m_clothMeshGarbage.clear();
	}
	
}
//---------------------------------------------------------------------------------
void NlPhysicsSimulator::ReleasePxMeshData( NxMeshData& data )
{
	NxVec3* pVec;
	NxU32*  pUint;

	pVec  = (NxVec3*)data.verticesPosBegin;		if (pVec)	free( pVec );
	pVec  = (NxVec3*)data.verticesNormalBegin;	if (pVec)	free( pVec );
	pUint = (NxU32*)data.numVerticesPtr;		if (pUint)  free( pUint );

	pUint = (NxU32*)data.indicesBegin;			if (pUint)  free( pUint );
	pUint = (NxU32*)data.numIndicesPtr;			if (pUint)  free( pUint );

	pUint = (NxU32*)data.parentIndicesBegin;	if (pUint)  free( pUint );
	pUint = (NxU32*)data.numParentIndicesPtr;	if (pUint)  free( pUint );

	data.verticesPosBegin		= NULL;
	data.verticesNormalBegin	= NULL;
	data.numVerticesPtr			= NULL;
	data.indicesBegin			= NULL;
	data.numIndicesPtr			= NULL;
	data.parentIndicesBegin		= NULL;
	data.numParentIndicesPtr	= NULL;

}