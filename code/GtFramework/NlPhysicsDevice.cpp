#include "nl_physics_header.h"
#include "NlPhysicsSimulator.h"
#include "NlPxReport.h"

NlPxContactReport g_contactReport;
NlPxTriggerReport g_triggerReport;
//=================================================================================
NlPhysicsDevice::NlPhysicsDevice(void)
{
	m_sdk_obj = NULL;
	m_scene = NULL;

	m_clidEventManager = NULL;
	m_elapsedTime = 0;

	m_gravity = 9.8f;
	m_paramScale = 1.0f;
	m_runHardware = true;

	m_reset =false;
}
//---------------------------------------------------------------------------------
NlPhysicsDevice::~NlPhysicsDevice(void)
{
}
//---------------------------------------------------------------------------------
bool NlPhysicsDevice::init( float gravity, float paramScale )
{
	// clear up
	destroy();

	m_gravity = gravity;
	m_paramScale = paramScale;

	// create the PhysX SDK
	NxPhysicsSDKDesc sdkDesc;
	sdkDesc.flags &= ~NX_SDKF_NO_HARDWARE;

	m_sdk_obj = NxCreatePhysicsSDK( NX_PHYSICS_SDK_VERSION, 0, 0, sdkDesc );
	if (!m_sdk_obj)
	{
		MessageBox( NULL, "create PhysX SDK failed", "error", MB_OK );
		return false;
	}

	// set the debug visualization parameters
	m_sdk_obj->setParameter( NX_VISUALIZATION_SCALE, 1 );
	//m_sdk_obj->setParameter( NX_VISUALIZE_COLLISION_SHAPES, 1 );
	//m_sdk_obj->setParameter( NX_VISUALIZE_ACTOR_AXES, 1 );
	m_sdk_obj->setParameter( NX_VISUALIZE_JOINT_LOCAL_AXES, 1 );
	m_sdk_obj->setParameter( NX_VISUALIZE_JOINT_WORLD_AXES, 1 );
	m_sdk_obj->setParameter( NX_VISUALIZE_JOINT_LIMITS, 1 );
	m_sdk_obj->setParameter( NX_VISUALIZE_CLOTH_MESH, 1 );
	m_sdk_obj->setParameter( NX_VISUALIZE_CLOTH_SLEEP, 1 );

	// set the PhysX dependent parameters
	m_sdk_obj->setParameter( NX_SKIN_WIDTH, 0.01f*(1/m_paramScale) );
	m_sdk_obj->setParameter( NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 0.15f*0.15f*(1/m_paramScale)*(1/m_paramScale) );
	m_sdk_obj->setParameter( NX_BOUNCE_THRESHOLD, -2*(1/m_paramScale) );		// relative velocity
	m_sdk_obj->setParameter( NX_VISUALIZATION_SCALE, 0.5f*(1/m_paramScale) );

	// create the scene
	NxSceneDesc sceneDesc;
	sceneDesc.simType = m_runHardware ? NX_SIMULATION_HW : NX_SIMULATION_SW;	// simulate on hardware
	sceneDesc.gravity = NxVec3( 0, -m_gravity, 0 );
	sceneDesc.userContactReport = &g_contactReport;
	sceneDesc.userTriggerReport = &g_triggerReport;

	m_scene = m_sdk_obj->createScene( sceneDesc );
	if (!m_scene)
	{
		if (m_runHardware == true)
			MessageBox( NULL, "PhysX cannot simulate on hardware", "error", MB_OK );
		m_runHardware = false;

		sceneDesc.simType = NX_SIMULATION_SW;
		m_scene = m_sdk_obj->createScene( sceneDesc );
		if (!m_scene)
		{
			MessageBox( NULL, "creat PhysX scene failed", "error", MB_OK );
			return false;
		}
	}

	// create default material
	NlPxMtrlParam defaultPxMtrlParam;
	NlPxMtrlManager::GetInstance().CreateMaterial( this, defaultPxMtrlParam, "default" );

	// add a collider_event manager
	m_clidEventManager = new NlClidEventManager( this );
	assert( m_clidEventManager );

	return true;

}
//---------------------------------------------------------------------------------
void NlPhysicsDevice::update( float elapsedTime )
{
	m_elapsedTime = elapsedTime;

	assert( m_clidEventManager );
	m_clidEventManager->update();

}
//---------------------------------------------------------------------------------
void NlPhysicsDevice::destroy()
{
	if (m_scene)
	{
		m_sdk_obj->releaseScene( *m_scene );
		m_scene = NULL;
	}
	if (m_sdk_obj)
	{
		m_sdk_obj->release();
		m_sdk_obj = NULL;
	}

	NlPxMtrlManager::GetInstance().ClearUp();

	if (m_clidEventManager)
	{
		delete m_clidEventManager;
		m_clidEventManager = NULL;
	}

}
//---------------------------------------------------------------------------------
void NlPhysicsDevice::ResetDevice()
{
	m_reset = true;

	m_scene->setUserContactReport( 0 );
	m_scene->setUserTriggerReport( 0 );

}