#include "Ground.h"
#include "NlGameObjPri.h"

#include "NlGameEnvironment.h"
#include "nl_render_header.h"
#include "nl_physics_header.h"

#include "Stick.h"

Ground::Ground( float2 sides )
{
	m_sides = sides;
	SetScale( float3(1, 1, sides.y) );

}

Ground::~Ground( void )
{
}

void Ground::destroy()
{
	NlCombRenderble::destroy();

	std::vector<NlRenderble*>::iterator it;
	for (it = m_compos.begin(); it != m_compos.end(); ++it)
	{
		if (*it)
		{
			(*it)->destroy();
			delete (*it);
			(*it) = NULL;
		}
	}
	m_compos.clear();
	m_fallen.clear();

}

bool Ground::init( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );

	NlRigidBodyDesc RBDesc;
	RBDesc._isStatic = true;
	NlRigidBody* pRigid = NULL;
	pRigid = new NlRigidBody( gameEnv->GetPhysicsDevice() );
	pRigid->init( RBDesc );
	SetRigidBody( pRigid, true );
	pRigid->AddBoxCollider( float3(m_sides.x, 0.1f, m_sides.y), 0 );

	uint16 pxMtrlIndex;
	NlPxMaterial* pxMtrl = NlPxMtrlManager::GetInstance().GetMaterialByName( "ground" );
	if (!pxMtrl)
	{
		NlPxMtrlParam mtrlParam;
		mtrlParam._bounciness = 0.1f;
		mtrlParam._staticFriction = 0.75f;
		mtrlParam._dynamicFriction = 0.75f;
		pxMtrlIndex = NlPxMtrlManager::GetInstance().
			CreateMaterial( gameEnv->GetPhysicsDevice(), mtrlParam, "ground" );
	}
	else
		pxMtrlIndex = pxMtrl->GetIndex();

	NlCuboid* pCompo = NULL;
	for (int i = -m_sides.x*0.5f; i <= m_sides.x*0.5f; ++i)
	{
		pCompo = new NlCuboid( float3(0.95f, 0.1f, 1.0f) );
		assert( pCompo );
		pCompo->init( gameEnv );
		pCompo->SetPos( float3(i, 0, 0 ) );
		pCompo->SetParent( this );

		pCompo->SetMaterial( NlMtrlManager::GetInstance().GetMtrlByName( 
			gameEnv->GetRenderDevice(), "ground.mtl", "ground" ) );

		m_compos.push_back( pCompo );
	}

	name = "Ground";
	m_playerATurn = true;
	return true;

}

void Ground::Draw( NlGameEnvironment* gameEnv )
{
	NlCombRenderble::Draw( gameEnv );

	std::vector<NlRenderble*>::iterator it;
	for (it = m_compos.begin(); it != m_compos.end(); ++it)
	{
		if (*it)
			(*it)->Draw( gameEnv );
	}

}

void Ground::OnCollisionEnter( const NlCollider& another )
{
	const NlGameObjectBase* anotherObj = &another.GetAttachedRigidBody().GetOwner();

	if (anotherObj->name == "Stick")
	{
		Stick* pStick = (Stick*)anotherObj;
		if (pStick->CheckBase() == true)
			return;

		std::vector<Stick*>::iterator it;
		if (m_playerATurn == true)
		{
			for (it = m_fallen.begin(); it != m_fallen.end(); ++it)
				if ((*it) == pStick)
					return;

			m_fallen.push_back( pStick );
		}
		else
		{
			for (it = m_fallenB.begin(); it != m_fallenB.end(); ++it)
				if ((*it) == pStick)
					return;

			m_fallenB.push_back( pStick );
		}
		
	}

}