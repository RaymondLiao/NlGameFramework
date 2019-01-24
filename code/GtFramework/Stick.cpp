#include "Stick.h"

#include "NlGameEnvironment.h"
#include "nl_render_header.h"
#include "nl_physics_header.h"

//=================================================================================
Stick::Stick( EStickType type, float3 sides ):
	m_type(type), NlCuboid( sides )
{
	m_isBase = false;
	m_touchGround = false;
}
//---------------------------------------------------------------------------------
Stick::~Stick()
{
}
//---------------------------------------------------------------------------------
bool Stick::init( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );

	// remember to initialize father class--------------------
	if (NlCuboid::init( gameEnv ) == false)
		return false;

	// set rigid body-----------------------------------------
	NlRigidBodyDesc RBDesc;
	//RBDesc._isStatic = true;

	switch(m_type)
	{
	case eStick_wood:
		RBDesc._mass = 0.5f * m_sides.x*m_sides.y*m_sides.z;
		break;
		
	case eStick_rubber:
		RBDesc._mass = 0.93f * m_sides.x*m_sides.y*m_sides.z;
		break;

	case eStick_glass:
		RBDesc._mass = 2.5f * m_sides.x*m_sides.y*m_sides.z;
		break;

	case eStick_china:
		RBDesc._mass = 2.3f * m_sides.x*m_sides.y*m_sides.z;
		break;

	default:
		return false;
	}

	NlRigidBody *rigid = new NlRigidBody( gameEnv->GetPhysicsDevice() );
	assert( rigid );
	rigid->init( RBDesc );
	SetRigidBody( rigid, true );

	// get physcis material's index-------------------------
	NlPxMaterial* pxMtrl = NULL;
	uint16 mtrlIndex;

	switch (m_type)
	{
	case eStick_wood:
		pxMtrl = NlPxMtrlManager::GetInstance().GetMaterialByName( "wood" );
		if (!pxMtrl)	// "wood" material hasn't been created
		{
			NlPxMtrlParam mtrlParam;
			mtrlParam._bounciness = 0.4f;
			mtrlParam._staticFriction = 0.3f;
			mtrlParam._dynamicFriction = 0.25f;
			mtrlIndex = NlPxMtrlManager::GetInstance().
				CreateMaterial( gameEnv->GetPhysicsDevice(), mtrlParam, "wood" );
		}
		else
			mtrlIndex = pxMtrl->GetIndex();
			
		break;

	case eStick_rubber:
		pxMtrl = NlPxMtrlManager::GetInstance().GetMaterialByName( "rubber" );
		if (!pxMtrl)	// "rubber" material hasn't been created
		{
			NlPxMtrlParam mtrlParam;
			mtrlParam._bounciness = 0.8f;
			mtrlParam._staticFriction = 0.7f;
			mtrlParam._dynamicFriction = 0.65f;
			mtrlIndex = NlPxMtrlManager::GetInstance().
				CreateMaterial( gameEnv->GetPhysicsDevice(), mtrlParam, "rubber" );
		}
		else
			mtrlIndex = pxMtrl->GetIndex();

		break;

	case eStick_glass:
		pxMtrl = NlPxMtrlManager::GetInstance().GetMaterialByName( "glass" );
		if (!pxMtrl)	// "glass" material hasn't been created
		{
			NlPxMtrlParam mtrlParam;
			mtrlParam._bounciness = 0.1f;
			mtrlParam._staticFriction = 0.9f;
			mtrlParam._dynamicFriction = 0.4f;
			mtrlParam._frictionCombine = ePMComb_MIN;
			mtrlIndex = NlPxMtrlManager::GetInstance().
				CreateMaterial( gameEnv->GetPhysicsDevice(), mtrlParam, "glass" );
		}
		else
			mtrlIndex = pxMtrl->GetIndex();

		break;

	case eStick_china:
		pxMtrl = NlPxMtrlManager::GetInstance().GetMaterialByName( "china" );
		if (!pxMtrl)	// "china" material hasn't been created
		{
			NlPxMtrlParam mtrlParam;
			mtrlParam._bounciness = 0.2f;
			mtrlParam._staticFriction = 0.6f;
			mtrlParam._dynamicFriction = 0.55f;
			mtrlIndex = NlPxMtrlManager::GetInstance().
				CreateMaterial( gameEnv->GetPhysicsDevice(), mtrlParam, "china" );
		}
		else
			mtrlIndex = pxMtrl->GetIndex();

		break;
	}
	
	// add a box collider------------------------------------
	assert( m_rigidBody );
	m_rigidBody->AddBoxCollider( m_sides, mtrlIndex );

	// set render-material
	switch (m_type)
	{
	case eStick_wood:
		m_material = NlMtrlManager::GetInstance().GetMtrlByName( gameEnv->GetRenderDevice(), "stick_wood.mtl", "wood" );
		break;

	case eStick_rubber:
		m_material = NlMtrlManager::GetInstance().GetMtrlByName( gameEnv->GetRenderDevice(), "stick_rubber.mtl", "rubber" );
		break;

	case eStick_glass:
		m_material = NlMtrlManager::GetInstance().GetMtrlByName( gameEnv->GetRenderDevice(), "stick_glass.mtl", "glass" );
		break;

	case eStick_china:
		m_material = NlMtrlManager::GetInstance().GetMtrlByName( gameEnv->GetRenderDevice(), "stick_china.mtl", "china" );
		break;
	}
	
	name = "Stick";
	return true;

}
//---------------------------------------------------------------------------------
void Stick::OnCollisionEnter( const NlCollider& another )
{
	const NlGameObjectBase& anotherObj = another.GetAttachedRigidBody().GetOwner();

	if (anotherObj.name == "Ground")
		m_touchGround = true;

}