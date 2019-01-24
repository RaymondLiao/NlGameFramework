#include "NlJoint.h"

#include "nl_physics_header.h"

//=================================================================================
NlJointBase::NlJointBase( NlPhysicsDevice* device )
{
	assert( device );
	m_device = device;

	m_joint = NULL;
	m_major = m_connected = NULL;
}
//---------------------------------------------------------------------------------
NlJointBase::~NlJointBase()
{
}
//---------------------------------------------------------------------------------
bool NlJointBase::init( NlRigidBody* major, NlRigidBody* connected )
{
	// clear up
	destroy();

	if (major == NULL || connected == NULL)
		return false;

	m_major = major;
	m_connected = connected;

	return true;
}
//---------------------------------------------------------------------------------
void NlJointBase::destroy()
{
	// remember to release px actor
	if (m_joint)
	{
		m_device->ReleaseJoint( m_joint );
		m_joint = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlJointBase::SetBreakable( float force, float torque )
{
	if (m_joint == NULL)
		return;

	m_joint->setBreakable( force, torque );
}

//=================================================================================
NlFixedJoint::NlFixedJoint( NlPhysicsDevice* device ):
	NlJointBase( device )
{
}
//---------------------------------------------------------------------------------
NlFixedJoint::~NlFixedJoint()
{
}
//---------------------------------------------------------------------------------
bool NlFixedJoint::init( NlRigidBody* major, NlRigidBody* connected )
{
	if (!NlJointBase::init(major, connected))
		return false;
	
	NxFixedJointDesc fixedDesc;
	fixedDesc.actor[0] = major->m_pxActor;
	fixedDesc.actor[1] = connected->m_pxActor;

	m_joint = m_device->GetScene()->createJoint( fixedDesc );
	if (m_joint == NULL)
		return false;

	return true;
}

//=================================================================================
NlHingeJoint::NlHingeJoint( NlPhysicsDevice* device ):
	NlJointBase( device )
{
}
//---------------------------------------------------------------------------------
NlHingeJoint::~NlHingeJoint()
{
}
//---------------------------------------------------------------------------------
bool NlHingeJoint::init( NlRigidBody* major, NlRigidBody* connected, 
	float3 localAnchor /*= float3(0)*/, float3 localAxis /*= float3(1, 0, 0) */ )
{
	if (NlJointBase::init( major, connected ) == false)
		return false;

	NxVec3 globalAnchor = 
		major->m_pxActor->getGlobalPosition() + CovToPXVec( localAnchor );
	NxVec3 globalAxis;
	major->m_pxActor->getGlobalOrientation().multiply( CovToPXVec(localAxis), globalAxis );

	NxRevoluteJointDesc revDesc;
	revDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
	revDesc.actor[0] = major->m_pxActor;
	revDesc.actor[1] = connected->m_pxActor;
	revDesc.setGlobalAnchor( globalAnchor );
	revDesc.setGlobalAxis( globalAxis );

	m_joint = m_device->GetScene()->createJoint( revDesc );
	if (m_joint == NULL)
		return false;

	return true;
}
//---------------------------------------------------------------------------------
void NlHingeJoint::SetLimits( float minAngle, float maxAngle, float minBounce /*= 0*/, float maxBounce /*= 0 */ )
{
	if (m_joint == NULL)
		return;

	NxRevoluteJointDesc revDesc;
	m_joint->isRevoluteJoint()->saveToDesc( revDesc );
	{
		if (minAngle > maxAngle)
		{
			float temp = maxAngle;
			maxAngle = minAngle;
			minAngle = temp;
		}
		revDesc.flags |= NX_RJF_LIMIT_ENABLED;
		revDesc.limit.low.value = DEG2RAD( minAngle );
		revDesc.limit.high.value = DEG2RAD( maxAngle );
		revDesc.limit.low.restitution = minBounce;
		revDesc.limit.high.restitution = maxBounce;
	}
	m_joint->isRevoluteJoint()->loadFromDesc( revDesc );

}
//---------------------------------------------------------------------------------
void NlHingeJoint::SetSpring( float spring, float damper, float targetAngle )
{
	if (m_joint == NULL)
		return;

	NxRevoluteJointDesc revDesc;
	m_joint->isRevoluteJoint()->saveToDesc( revDesc );
	{
		revDesc.flags |= NX_RJF_SPRING_ENABLED;
		revDesc.spring.spring = spring;
		revDesc.spring.damper = damper;
		revDesc.spring.targetValue = DEG2RAD( targetAngle );
	}
	m_joint->isRevoluteJoint()->loadFromDesc( revDesc );

}
//---------------------------------------------------------------------------------
void NlHingeJoint::SetMotor( float targetVeloc, float force, bool freeSpin )
{
	if (m_joint == NULL)
		return;

	NxRevoluteJointDesc revDesc;
	m_joint->isRevoluteJoint()->saveToDesc( revDesc );
	{
		revDesc.flags |= NX_RJF_MOTOR_ENABLED;
		revDesc.motor.velTarget = targetVeloc;
		revDesc.motor.maxForce = force;
		revDesc.motor.freeSpin = freeSpin;
	}
	m_joint->isRevoluteJoint()->loadFromDesc( revDesc );

}

//=================================================================================
NlCylinderJoint::NlCylinderJoint( NlPhysicsDevice* device ):
NlJointBase( device )
{
}
//---------------------------------------------------------------------------------
NlCylinderJoint::~NlCylinderJoint()
{
}
//---------------------------------------------------------------------------------
bool NlCylinderJoint::init( NlRigidBody* major, NlRigidBody* connected, 
	float3 localAnchor /*= float3(0)*/, float3 localAxis /*= float3(1, 0, 0) */ )
{
	if (NlJointBase::init( major, connected ) == false)
		return false;

	NxVec3 globalAnchor = 
		major->m_pxActor->getGlobalPosition() + CovToPXVec( localAnchor );
	NxVec3 globalAxis;
	major->m_pxActor->getGlobalOrientation().multiply( CovToPXVec(localAxis), globalAxis );
	globalAxis.normalize();

	NxCylindricalJointDesc cylDesc;
	cylDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
	cylDesc.actor[0] = major->m_pxActor;
	cylDesc.actor[1] = connected->m_pxActor;
	cylDesc.setGlobalAnchor( globalAnchor );
	cylDesc.setGlobalAxis( globalAxis );

	m_joint = m_device->GetScene()->createJoint( cylDesc );
	if (m_joint == NULL)
		return false;

	// add default translational limits
	m_joint->setLimitPoint( globalAnchor );
	m_joint->addLimitPlane( -globalAxis, globalAnchor + 1.0f*globalAxis );
	m_joint->addLimitPlane( globalAxis, globalAnchor - 1.0f*globalAxis );
	return true;

}
//---------------------------------------------------------------------------------
void NlCylinderJoint::SetLimits( float upDist, float upBounce, float downDist, float downBounce )
{
	if (m_joint == NULL)
		return;

	NxVec3 globalAnchor = m_joint->getGlobalAnchor();
	NxVec3 globalAxis = m_joint->getGlobalAxis();
	m_joint->purgeLimitPlanes();
	m_joint->addLimitPlane( -globalAxis, globalAnchor + upDist*globalAxis, upBounce );
	m_joint->addLimitPlane( globalAxis, globalAnchor - downDist*globalAxis, downBounce );
}

//=================================================================================
NlSpringJoint::NlSpringJoint( NlPhysicsDevice* device ):
	NlJointBase( device )
{
}
//---------------------------------------------------------------------------------
NlSpringJoint::~NlSpringJoint()
{
}
//---------------------------------------------------------------------------------
bool NlSpringJoint::init( NlRigidBody* major, NlRigidBody* connected, float3 localAnchor /*= float3(0) */ )
{
	if (NlJointBase::init( major, connected ) == false)
		return false;

	float defDist = ( m_major->m_pxActor->getGlobalPose()*CovToPXVec(localAnchor) -
		connected->m_pxActor->getGlobalPosition() ).magnitude();

	NxDistanceJointDesc distanceDesc;
	distanceDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
	distanceDesc.flags = NX_DJF_SPRING_ENABLED | NX_DJF_MIN_DISTANCE_ENABLED | NX_DJF_MAX_DISTANCE_ENABLED;
	distanceDesc.actor[0] = major->m_pxActor;
	distanceDesc.actor[1] = connected->m_pxActor;
	distanceDesc.localAnchor[0] = CovToPXVec( localAnchor );
	distanceDesc.localAnchor[1] = NxVec3( 0, 0, 0 );
	distanceDesc.minDistance = defDist * 0.5f;
	distanceDesc.maxDistance = defDist * 1.5f;
	distanceDesc.spring.spring = 100.0f;
	distanceDesc.spring.damper = 0.5f;

	m_joint = m_device->GetScene()->createJoint( distanceDesc );
	if (m_joint == NULL)
		return false;

	return true;

}
//---------------------------------------------------------------------------------
void NlSpringJoint::SetDistance( float minDist, float maxDist )
{
	if (m_joint == NULL)
		return;

	NxDistanceJointDesc distanceDesc;
	m_joint->isDistanceJoint()->saveToDesc( distanceDesc );
	{
		if (minDist > maxDist)
		{
			float temp = maxDist;
			maxDist = minDist;
			minDist = temp;
		}
		distanceDesc.minDistance = minDist >= 0.0f ? minDist : 0.0f;
		distanceDesc.maxDistance = maxDist >= 0.0f ? maxDist : 0.0f;
	}
	m_joint->isDistanceJoint()->loadFromDesc( distanceDesc );

}
//---------------------------------------------------------------------------------
void NlSpringJoint::SetSpring( float spring, float damper )
{
	if (m_joint == NULL)
		return;

	NxDistanceJointDesc distanceDesc;
	m_joint->isDistanceJoint()->saveToDesc( distanceDesc );
	{
		distanceDesc.spring.spring = spring;
		distanceDesc.spring.damper = damper;
	}
	m_joint->isDistanceJoint()->loadFromDesc( distanceDesc );

}

//=================================================================================
NlCharacterJoint::NlCharacterJoint( NlPhysicsDevice* device ):
NlJointBase( device )
{
}
//---------------------------------------------------------------------------------
NlCharacterJoint::~NlCharacterJoint()
{
}
//---------------------------------------------------------------------------------
bool NlCharacterJoint::init( NlRigidBody* major, NlRigidBody* connected, 
	float3 localAnchor /*= float3(0)*/, float3 localAxis /*= float3(1, 0, 0)*/, float3 swingAxis /*= float3(0, 1, 0) */ )
{
	if (NlJointBase::init( major, connected ) == false)
		return false;

	NxVec3 globalAnchor = 
		major->m_pxActor->getGlobalPosition() + CovToPXVec( localAnchor );
	NxVec3 globalAxis;
	major->m_pxActor->getGlobalOrientation().multiply( CovToPXVec(localAxis), globalAxis );

	NxD6JointDesc d6Desc;
	d6Desc.twistMotion = NX_D6JOINT_MOTION_FREE;
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_FREE;
	d6Desc.swing2Motion = NX_D6JOINT_MOTION_FREE;
	d6Desc.xMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;

	d6Desc.jointFlags |= NX_JF_COLLISION_ENABLED;
	d6Desc.actor[0] = major->m_pxActor;
	d6Desc.actor[1] = connected->m_pxActor;
	d6Desc.setGlobalAnchor( globalAnchor );
	d6Desc.setGlobalAxis( globalAxis );

	// reconfigure d6_joint to spherical_joint
	//NxVec3 oldAxis[2], oldNormal[2], oldBinormal[2];

	//oldAxis[0] = d6Desc.localAxis[0];
	//oldAxis[1] = d6Desc.localAxis[1];
	//oldNormal[0] = d6Desc.localNormal[0];
	//oldNormal[1] = d6Desc.localNormal[1];
	//oldBinormal[0] = oldNormal[0].cross( oldAxis[0] );
	//oldBinormal[1] = oldNormal[1].cross( oldAxis[1] );

	//d6Desc.localAxis[0] = oldBinormal[0];
	//d6Desc.localAxis[1] = oldBinormal[1];

	// calculate global swing axis, namely, normal
	NxVec3 axis = d6Desc.localAxis[0];
	axis.normalize();
	NxVec3 proj = axis * (axis.dot(CovToPXVec(swingAxis)));
	NxVec3 normal( swingAxis.x-proj.x, swingAxis.y-proj.y, swingAxis.z-proj.z );
	normal.normalize();
	d6Desc.localAxis[0] = axis;
	d6Desc.localNormal[0] = normal;


	m_joint = m_device->GetScene()->createJoint( d6Desc );
	if (m_joint == NULL)
		return false;

	return true;
}
//---------------------------------------------------------------------------------
void NlCharacterJoint::SetLowTwistLimit( float limitAngle, float bounciness, float spring, float damper )
{
	if (m_joint == NULL)
		return;

	NxD6JointDesc d6Desc;
	m_joint->isD6Joint()->saveToDesc( d6Desc );
	{
		d6Desc.twistMotion = NX_D6JOINT_MOTION_LIMITED;

		d6Desc.twistLimit.low.value = DEG2RAD( limitAngle );
		d6Desc.twistLimit.low.restitution = bounciness;
		d6Desc.twistLimit.low.spring = spring;
		d6Desc.twistLimit.low.damping = damper;
	}
	m_joint->isD6Joint()->loadFromDesc( d6Desc );

}
//---------------------------------------------------------------------------------
void NlCharacterJoint::SetHighTwistLimit( float limitAngle, float bounciness, float spring, float damper )
{
	if (m_joint == NULL)
		return;

	NxD6JointDesc d6Desc;
	m_joint->isD6Joint()->saveToDesc( d6Desc );
	{
		d6Desc.twistMotion = NX_D6JOINT_MOTION_LIMITED;

		d6Desc.twistLimit.high.value = DEG2RAD( limitAngle );
		d6Desc.twistLimit.high.restitution = bounciness;
		d6Desc.twistLimit.high.spring = spring;
		d6Desc.twistLimit.high.damping = damper;
	}
	m_joint->isD6Joint()->loadFromDesc( d6Desc );

}
//---------------------------------------------------------------------------------
void NlCharacterJoint::SetNormSwingLimit( float limitAngle, float bounciness, float spring, float damper )
{
	if (m_joint == NULL)
		return;

	NxD6JointDesc d6Desc;
	m_joint->isD6Joint()->saveToDesc( d6Desc );
	{
		d6Desc.swing1Motion = NX_D6JOINT_MOTION_LIMITED;

		d6Desc.swing1Limit.value = DEG2RAD( limitAngle );
		d6Desc.swing1Limit.restitution = bounciness;
		d6Desc.swing1Limit.spring = spring;
		d6Desc.swing1Limit.damping = damper;
	}
	m_joint->isD6Joint()->loadFromDesc( d6Desc );

}
//---------------------------------------------------------------------------------
void NlCharacterJoint::SetBinormSwingLimit( float limitAngle, float bounciness, float spring, float damper )
{
	if (m_joint == NULL)
		return;

	NxD6JointDesc d6Desc;
	m_joint->isD6Joint()->saveToDesc( d6Desc );
	{
		d6Desc.swing2Motion = NX_D6JOINT_MOTION_LIMITED;

		d6Desc.swing2Limit.value = DEG2RAD( limitAngle );
		d6Desc.swing2Limit.restitution = bounciness;
		d6Desc.swing2Limit.spring = spring;
		d6Desc.swing2Limit.damping = damper;
	}
	m_joint->isD6Joint()->loadFromDesc( d6Desc );

}
