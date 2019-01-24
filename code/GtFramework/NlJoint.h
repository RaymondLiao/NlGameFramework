/*
  @file   NlJoint.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlJoint_h_
#define _NlJoint_h_

#include "prerequisite.h"

class NlPhysicsDevice;
class NlRigidBody;

//====================================================
// this is a abstruct class
class NlJointBase
{
public:
	NlJointBase( NlPhysicsDevice* device );
	~NlJointBase();

	bool init( NlRigidBody* major, NlRigidBody* connected );
	void destroy();

	// set force and torque that can break the joint
	void SetBreakable( float force, float torque );

protected:
	NlPhysicsDevice* m_device;
	NxJoint* m_joint;

	NlRigidBody* m_major;
	NlRigidBody* m_connected;

};

//====================================================
class NlFixedJoint :
	public NlJointBase
{
public:
	NlFixedJoint( NlPhysicsDevice* device );
	~NlFixedJoint();

	bool init( NlRigidBody* major, NlRigidBody* connected );

};

//====================================================
class NlHingeJoint :
	public NlJointBase
{
public:
	NlHingeJoint( NlPhysicsDevice* device );
	~NlHingeJoint();

	bool init( NlRigidBody* major, NlRigidBody* connected,
		float3 localAnchor = float3(0), float3 localAxis = float3(1, 0, 0) );

	void SetLimits( float minAngle, float maxAngle, float minBounce = 0, float maxBounce = 0 );	// angle in degree, bounciness in range 0~1
	void SetSpring( float spring, float damper, float targetAngle );	// angle in degree
	void SetMotor( float targetVeloc, float force, bool freeSpin );		// if spin freely, motor will not brake when reaching target velocity

};

//====================================================
class NlCylinderJoint :
	public NlJointBase
{
public:
	NlCylinderJoint( NlPhysicsDevice* device );
	~NlCylinderJoint();

	bool init( NlRigidBody* major, NlRigidBody* connected,
		float3 localAnchor = float3(0), float3 localAxis = float3(1, 0, 0) );

	void SetLimits( float upDist, float upBounce, float downDist, float downBounce );	// bounciness in range 0~1

};

//====================================================
class NlSpringJoint :
	public NlJointBase
{
public:
	NlSpringJoint( NlPhysicsDevice* device );
	~NlSpringJoint();

	bool init( NlRigidBody* major, NlRigidBody* connected, float3 localAnchor = float3(0) );
	void SetDistance( float minDist, float maxDist );
	void SetSpring( float spring, float damper );

};

//====================================================
class NlCharacterJoint :
	public NlJointBase
{
public:
	NlCharacterJoint( NlPhysicsDevice* device );
	~NlCharacterJoint();

	bool init( NlRigidBody* major, NlRigidBody* connected,
		float3 localAnchor = float3(0), float3 localAxis = float3(1, 0, 0), float3 swingAxis = float3(0, 1, 0) );

	// set limits for rotation around 3 axis, limit angle in degree; bounciness in range 0~1
	void SetLowTwistLimit( float limitAngle, float bounciness, float spring, float damper );
	void SetHighTwistLimit( float limitAngle, float bounciness, float spring, float damper );
	void SetNormSwingLimit( float limitAngle, float bounciness, float spring, float damper );
	void SetBinormSwingLimit( float limitAngle, float bounciness, float spring, float damper );

};

#endif