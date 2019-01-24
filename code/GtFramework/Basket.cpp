#include "Basket.h"
#include "NlGameObjPri.h"

#include "NlGameEnvironment.h"
#include "nl_render_header.h"
#include "nl_physics_header.h"

#include "Stick.h"

//=================================================================================
Basket::Basket( float3 sides )
{
	m_sides = sides;
	m_stickCout = 0;

}
//---------------------------------------------------------------------------------
Basket::~Basket( void )
{
}
//---------------------------------------------------------------------------------
bool Basket::init( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );

	NlRigidBodyDesc RBDesc;
	RBDesc._isStatic = true;
	NlRigidBody* pRigid = NULL;
	pRigid = new NlRigidBody( gameEnv->GetPhysicsDevice() );
	pRigid->init( RBDesc );
	SetRigidBody( pRigid, true );
	pRigid->AddBoxCollider( float3(m_sides)*0.8f, 0, true );

	uint16 pxMtrlIndex;
	NlPxMaterial* pxMtrl = NlPxMtrlManager::GetInstance().GetMaterialByName( "basket" );
	if (!pxMtrl)
	{
		NlPxMtrlParam mtrlParam;
		mtrlParam._bounciness = 0.0f;
		mtrlParam._staticFriction = 1.0f;
		mtrlParam._dynamicFriction = 1.0f;
		pxMtrlIndex = NlPxMtrlManager::GetInstance().
			CreateMaterial( gameEnv->GetPhysicsDevice(), mtrlParam, "basket" );
	}
	else
		pxMtrlIndex = pxMtrl->GetIndex();

	// base---------------------------------------------------
	NlCuboid* pCompo = NULL;
	pCompo = new NlCuboid( float3(1.0f*m_sides.x, 0.2f*m_sides.y, 1.0f*m_sides.z) );
	assert( pCompo );
	pCompo->init( gameEnv );
	pCompo->SetPos( float3(0, -0.4f*m_sides.y, 0) );
	pCompo->SetParent( this );

	pCompo->SetMaterial( NlMtrlManager::GetInstance().GetMtrlByName( 
		gameEnv->GetRenderDevice(), "basket_wood.mtl", "lightWood" ) );
	
	pRigid = new NlRigidBody( gameEnv->GetPhysicsDevice() );
	pRigid->init( RBDesc );
	pCompo->SetRigidBody( pRigid, true );
	pRigid->AddBoxCollider( float3(1.0f*m_sides.x, 0.2f*m_sides.y, 1.0f*m_sides.z), pxMtrlIndex );

	m_subRenderbles["base"] = pCompo;

	// four pillars------------------------------------------
	int count = 0;
	for (int y = -1; y <= 1; y += 2)
	{
		for (int x = -1; x <= 1; x += 2)
		{
			//pCompo = new NlCuboid( float3(0.1f*m_sides.x, 0.8f*m_sides.y, 0.1f*m_sides.z) );
			NlCylinder* pCompo = new NlCylinder( 0.05f*m_sides.x, 0.8f*m_sides.y );
			assert( pCompo );
			pCompo->init( gameEnv );
			pCompo->SetPos( float3(x*0.45f*m_sides.x, 0.1f*m_sides.y, y*0.45f*m_sides.z) );
			pCompo->SetParent( this );

			pCompo->SetMaterial( NlMtrlManager::GetInstance().GetMtrlByName( 
				gameEnv->GetRenderDevice(), "basket_wood.mtl", "lightWood" ) );

			pRigid = new NlRigidBody( gameEnv->GetPhysicsDevice() );
			pRigid->init( RBDesc );
			pCompo->SetRigidBody( pRigid, true );
			pRigid->AddBoxCollider( float3(0.1f*m_sides.x, 0.8f*m_sides.y, 0.1f*m_sides.z), pxMtrlIndex );

			char compoName[10];
			sprintf( compoName, "pillar_%d", count );	++count;
			m_subRenderbles[compoName] = pCompo;
		}
	}

	// four walls--------------------------------------------
	for (int i = -1; i <= 1; i +=2)
	{
		pCompo = new NlCuboid( float3(0.8f*m_sides.x, 0.8f*m_sides.y, 0.1f*m_sides.z) );
		assert( pCompo );
		pCompo->init( gameEnv );
		pCompo->SetPos( float3(0*m_sides.x, 0.1f*m_sides.y, i*0.45f*m_sides.z) );
		pCompo->SetParent( this );
		
		pCompo->SetMaterial( NlMtrlManager::GetInstance().GetMtrlByName( 
			gameEnv->GetRenderDevice(), "basket_glass.mtl", "blueGlass" ) );

		pRigid = new NlRigidBody( gameEnv->GetPhysicsDevice() );
		pRigid->init( RBDesc );
		pCompo->SetRigidBody( pRigid, true );
		pRigid->AddBoxCollider( float3(0.8f*m_sides.x, 0.8f*m_sides.y, 0.1f*m_sides.z), pxMtrlIndex );

		char compoName[10];
		sprintf( compoName, "wall_%d", (int)((i+2)*0.5f) );
		m_subRenderbles[compoName] = pCompo;
	}
	
	for (int i = -1; i <= 1; i +=2)
	{
		pCompo = new NlCuboid( float3(0.8f*m_sides.x, 0.8f*m_sides.y, 0.1f*m_sides.z) );
		assert( pCompo );
		pCompo->init( gameEnv );
		pCompo->SetPos( float3(i*0.45f*m_sides.x, 0.1f*m_sides.y, 0*m_sides.z) );
		pCompo->SetRot( Quat::CreateRotationY(DEG2RAD(90)) );
		pCompo->SetParent( this );
		
		pCompo->SetMaterial( NlMtrlManager::GetInstance().GetMtrlByName( 
			gameEnv->GetRenderDevice(), "basket_glass.mtl", "blueGlass" ) );

		pRigid = new NlRigidBody( gameEnv->GetPhysicsDevice() );
		pRigid->init( RBDesc );
		pCompo->SetRigidBody( pRigid, true );
		pRigid->AddBoxCollider( float3(0.8f*m_sides.x, 0.8f*m_sides.y, 0.1f*m_sides.z), pxMtrlIndex );

		char compoName[10];
		sprintf( compoName, "wall_%d", (int)((i+5)*0.5f) );
		m_subRenderbles[compoName] = pCompo;
	}

	name = "Basket";
	return true;
	
}
//---------------------------------------------------------------------------------
void Basket::OnTriggerEnter( const NlCollider& another )
{
	const NlGameObjectBase* anotherObj = &another.GetAttachedRigidBody().GetOwner();

	if (anotherObj->name == "Stick")
	{
		Stick* pStick = (Stick*)anotherObj;
		if (pStick->IsTouchedGround() == false)
			m_stickCout++;
	}

}