#include "nl_physics_header.h"
#include "NlGameObject.h"

//=================================================================================
NlRigidBody::NlRigidBody( NlPhysicsDevice* device )
{
	assert(device);
	m_device = device;
	m_pxActor = NULL;

	m_owner = NULL;
	m_isKinematic = m_isStatic = false;
	m_mass = 1.0f;
	m_collidersCount = 0;
}
//---------------------------------------------------------------------------------
NlRigidBody::~NlRigidBody(void)
{
}
//---------------------------------------------------------------------------------
bool NlRigidBody::init( const NlRigidBodyDesc& desc )
{
	// clear up
	destroy();

	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	
	m_mass = desc._mass;

	m_isKinematic = desc._isKinematic;
	m_isStatic = desc._isStatic;

	if (m_isStatic == false)
		actorDesc.body = &bodyDesc;
	else
		actorDesc.body = NULL;

	// set original global pose
	float44 poseMat = float44::CreateIdentity();
	poseMat.Set( float(1), desc._rot, desc._pos );
	SetupPXMat34( poseMat, actorDesc.globalPose );

	// placeholder shape
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = 1.0f;
	sphereDesc.materialIndex = 0;
	sphereDesc.localPose.t = NxVec3( 0.0f, 0.0f, 0.0f );
	sphereDesc.shapeFlags |= NX_SF_DISABLE_COLLISION;	// disable collision detection
	sphereDesc.shapeFlags |= NX_SF_DISABLE_RAYCASTING;	// disable raycast
	actorDesc.shapes.pushBack( &sphereDesc );
	
	// create pxActor
	actorDesc.density = 1.0f;
	if (actorDesc.isValid())
	{
		m_pxActor = m_device->GetScene()->createActor( actorDesc );
		assert( m_pxActor );
		m_pxActor->setMass( m_mass );

		if (!m_isStatic)
		{
			desc._isKinematic ? m_pxActor->raiseBodyFlag( NX_BF_KINEMATIC ) :
				m_pxActor->clearBodyFlag( NX_BF_KINEMATIC );

			if(!desc._useGravity)
				m_pxActor->raiseBodyFlag( NX_BF_DISABLE_GRAVITY );
		}
		return true;
	}
	else 
		return false;

}
//---------------------------------------------------------------------------------
void NlRigidBody::Draw()
{
	NlDebugRenderer::DrawCoordSystem( m_pxActor );

	std::vector<NlCollider*>::iterator it;
	for (it = m_colliders.begin(); it != m_colliders.end(); ++it)
	{
		if (*it)
			(*it)->Draw();
	}

}
//---------------------------------------------------------------------------------
void NlRigidBody::destroy()
{
	// clear collider array
	for (std::vector<NlCollider*>::iterator it = m_colliders.begin(); 
		it != m_colliders.end(); ++it)
	{
		if (*it)
		{
			m_device->GetClidEventManager()->RemoveListener( *it );
			delete (*it);
			(*it) = NULL;
		}
	}

	// clear joint array
	for (std::vector<NlJointBase*>::iterator it = m_joints.begin();
		it != m_joints.end(); ++it)
	{
		if (*it)
		{
			(*it)->destroy();
			delete (*it);
			(*it) = NULL;
		}
	}

	// remember to release px actor
	if (m_pxActor)
	{
		m_device->ReleaseActor( m_pxActor );
		//m_device->GetScene()->releaseActor( *m_pxActor );
		m_pxActor = NULL;
	}

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddBoxCollider( float3& sides, uint16 mtrlIndex /*= 0*/, bool isTrigger /*= false */ )
{
	NlBoxCollider *newCollider = new NlBoxCollider( this );
	assert( newCollider );

	if (newCollider->init( sides, mtrlIndex, isTrigger ) == false)
	{
		delete newCollider;
		return false;
	}

	newCollider->SetEventDelegate( this->m_owner );
	m_colliders.push_back( newCollider );
	m_device->GetClidEventManager()->AddListener( newCollider );

	return true;

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddBoxCollider( float3& sides, float3& locPos, Quat& locRot, uint16 mtrlIndex /*= 0*/, bool isTrigger /*= false */ )
{
	NlBoxCollider *newCollider = new NlBoxCollider( this, locPos, locRot );
	assert( newCollider );

	if (newCollider->init( sides, mtrlIndex, isTrigger ) == false)
	{
		delete newCollider;
		return false;
	}

	newCollider->SetEventDelegate( this->m_owner );
	m_colliders.push_back( newCollider );
	m_device->GetClidEventManager()->AddListener( newCollider );

	return true;

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddSphereCollider( float radius, uint16 mtrlIndex /*= 0*/, bool isTrigger /*= false */ )
{
	NlSphereCollider *newCollider = new NlSphereCollider( this );
	assert( newCollider );

	if (newCollider->init( radius, mtrlIndex, isTrigger ) == false)
	{
		delete newCollider;
		return false;
	}

	newCollider->SetEventDelegate( this->m_owner );
	m_colliders.push_back( newCollider );
	m_device->GetClidEventManager()->AddListener( newCollider );

	return true;

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddSphereCollider( float radius, float3& locPos, Quat& locRot, uint16 mtrlIndex /*= 0*/, bool isTrigger /*= false */ )
{
	NlSphereCollider *newCollider = new NlSphereCollider( this, locPos, locRot );
	assert( newCollider );

	if (newCollider->init( radius, mtrlIndex, isTrigger ) == false)
	{
		delete newCollider;
		return false;
	}

	newCollider->SetEventDelegate( this->m_owner );
	m_colliders.push_back( newCollider );
	m_device->GetClidEventManager()->AddListener( newCollider );

	return true;

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddCapsuleCollider( float radius, float height, uint16 mtrlIndex /*= 0*/, bool isTrigger /*= false */ )
{
	NlCapsuleCollider* newCollider = new NlCapsuleCollider( this );
	assert( newCollider );

	if (newCollider->init( radius, height, mtrlIndex, isTrigger ) == false)
	{
		delete newCollider;
		return false;
	}

	newCollider->SetEventDelegate( this->m_owner );
	m_colliders.push_back( newCollider );
	m_device->GetClidEventManager()->AddListener( newCollider );

	return true;

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddCapsuleCollider( float radius, float height, float3& locPos, Quat& locRot, uint16 mtrlIndex /*= 0*/, bool isTrigger /*= false */ )
{
	NlCapsuleCollider* newCollider = new NlCapsuleCollider( this, locPos, locRot );
	assert( newCollider );

	if (newCollider->init( radius, height, mtrlIndex, isTrigger ) == false)
	{
		delete newCollider;
		return false;
	}

	newCollider->SetEventDelegate( this->m_owner );
	m_colliders.push_back( newCollider );
	m_device->GetClidEventManager()->AddListener( newCollider );

	return true;

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddMeshCollider( NlMeshClidData& data, float3 scale /*= float3(1)*/, uint16 mtrlIndex /*= 0*/, bool isTrigger /*= false */ )
{
	NlMeshCollider* newCollider = new NlMeshCollider( this );
	assert( newCollider );

	// change mesh scale by changing vertices
	float44 scaleMat = float44::CreateIdentity();
	scaleMat.Set( scale, Quat::CreateIdentity(), float3(0) );
	std::vector<float3>::iterator it;
	for (it = data._vertices.begin(); it != data._vertices.end(); ++it)
	{
		*it = scaleMat.MultVector3( *it );
	}

	if (newCollider->init( data, mtrlIndex, isTrigger ) == false)
	{
		delete newCollider;
		return false;
	}

	newCollider->SetEventDelegate( this->m_owner );
	m_colliders.push_back( newCollider );
	m_device->GetClidEventManager()->AddListener( newCollider );

	return true;

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddMeshCollider( NlMeshClidData& data, float3& locPos, Quat& locRot, 
	float3 scale /*= float3(1)*/, uint16 mtrlIndex /*= 0*/, bool isTrigger /*= false */ )
{
	NlMeshCollider* newCollider = new NlMeshCollider( this, locPos, locRot );
	assert( newCollider );

	// change mesh scale by changing vertices
	float44 scaleMat = float44::CreateIdentity();
	scaleMat.Set( scale, Quat::CreateIdentity(), float3(0) );
	std::vector<float3>::iterator it;
	for (it = data._vertices.begin(); it != data._vertices.end(); ++it)
	{
		*it = scaleMat.MultVector3( *it );
	}

	if (newCollider->init( data, mtrlIndex, isTrigger ) == false)
	{
		delete newCollider;
		return false;
	}

	newCollider->SetEventDelegate( this->m_owner );
	m_colliders.push_back( newCollider );
	m_device->GetClidEventManager()->AddListener( newCollider );

	return true;

}
//---------------------------------------------------------------------------------
void NlRigidBody::GetGlobalPose( float3& pos, Quat& rot )
{
	assert( m_pxActor );
	NxVec3 _pos = m_pxActor->getGlobalPosition();
	NxQuat _rot = m_pxActor->getGlobalOrientation();

	pos = CovToFloat3( _pos );
	rot.x = _rot.x;
	rot.y = _rot.y;
	rot.z = _rot.z;
	rot.v = float3(rot.x, rot.y, rot.z);
	rot.w = _rot.w;

}
//---------------------------------------------------------------------------------
void NlRigidBody::SetGlobalPose( float3 pos, Quat rot )
{
	assert( m_pxActor );
	float44 worldMat = float44::CreateIdentity();
	worldMat.Set( float3(1), rot, pos );

	NxMat34 pose;
	SetupPXMat34( worldMat, pose );
	m_pxActor->setGlobalPose( pose );

}
//---------------------------------------------------------------------------------
void NlRigidBody::SetKinematic( bool isKinematic )
{
	if (m_pxActor == NULL)
	{
		MessageBox( NULL, "need initialize rigid body first", "error", MB_OK );
		return;
	}

	isKinematic ? m_pxActor->raiseBodyFlag( NX_BF_KINEMATIC ) :
		m_pxActor->clearBodyFlag( NX_BF_KINEMATIC );
	m_isKinematic = isKinematic;

}
//---------------------------------------------------------------------------------
bool NlRigidBody::SetCenterOfMass( float3& pos )
{
	if (m_pxActor != NULL)
	{
		m_pxActor->setCMassGlobalPosition( CovToPXVec(pos) );
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------------
void NlRigidBody::AddForce( const float3& forceDir, const float strength )
{
	assert( m_device && m_pxActor );
	NxVec3 forceVec = CovToPXVec( forceDir*strength*m_device->GetElapsedTime() );
	m_pxActor->addForce( forceVec );

}
//---------------------------------------------------------------------------------
void NlRigidBody::AddTorque( const float3& forceDir, const float strength )
{
	assert( m_device && m_pxActor );
	NxVec3 forceVec = CovToPXVec( forceDir*strength*m_device->GetElapsedTime() );
	m_pxActor->addTorque( forceVec );

}
//---------------------------------------------------------------------------------
void NlRigidBody::Move( const float3& velDir, float velocity )
{
	if (m_isKinematic == false)
		return;
	
	assert( m_device && m_pxActor );
	NxVec3 speed = CovToPXVec(velDir) * velocity;
	NxVec3 position = m_pxActor->getGlobalPosition() + speed*m_device->GetElapsedTime();

	m_pxActor->moveGlobalPosition( position );

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddFixedJoint( uint16& index, NlRigidBody* connected )
{
	NlFixedJoint* fixedJoint = new NlFixedJoint( m_device );
	if (fixedJoint->init( this, connected ) == false)
	{
		delete fixedJoint;
		return false;
	}

	m_joints.push_back( fixedJoint );
	index =  m_joints.size();

	return true;

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddHingeJoint( uint16& index, NlRigidBody* connected, 
	float3 localAnchor /*= float3(0)*/, float3 localAxis /*= float3(1, 0, 0) */ )
{
	NlHingeJoint* hingeJoint = new NlHingeJoint( m_device );
	if (hingeJoint->init( this, connected, localAnchor, localAxis ) == false)
	{
		delete hingeJoint;
		return false;
	}

	m_joints.push_back( hingeJoint );
	index =  m_joints.size();

	return true;

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddCylinderJoint( uint16& index, NlRigidBody* connected, 
	float3 localAnchor /*= float3(0)*/, float3 localAxis /*= float3(1, 0, 0) */ )
{
	NlCylinderJoint* cylJoint = new NlCylinderJoint( m_device );
	if (cylJoint->init( this, connected, localAnchor, localAxis ) == false)
	{
		delete cylJoint;
		return false;
	}

	m_joints.push_back( cylJoint );
	index = m_joints.size();

	return true;

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddSpringJoint( uint16& index, NlRigidBody* connected, float3 localAnchor /*= float3(0) */ )
{
	NlSpringJoint* springJoint = new NlSpringJoint( m_device );
	if (springJoint->init( this, connected, localAnchor ) == false)
	{
		delete springJoint;
		return false;
	}

	m_joints.push_back( springJoint );
	index = m_joints.size();

	return true;

}
//---------------------------------------------------------------------------------
bool NlRigidBody::AddCharacerJoint( uint16& index, NlRigidBody* connected, 
	float3 localAnchor /*= float3(0)*/, float3 localAxis /*= float3(1, 0, 0)*/, float3 swingAxis /*= float3(0, 1, 0) */ )
{
	NlCharacterJoint* characterJoint = new NlCharacterJoint( m_device );
	if (characterJoint->init( this, connected, localAnchor, localAxis, swingAxis ) == false)
	{
		delete characterJoint;
		return false;
	}

	m_joints.push_back( characterJoint );
	index = m_joints.size();

	return true;

}

//---------------------------------------------------------------------------------
NlJointBase* NlRigidBody::GetJointByIndex( uint16 index )
{
	if (index > m_joints.size())
		return NULL;

	return m_joints[index-1];

}