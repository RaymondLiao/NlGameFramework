/*
  @file   NlRigidBody.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlRigidBody_h_
#define _NlRigidBody_h_
#include "prerequisite.h"
#include "NlCollider.h"

//====================================================
class NlPhysicsDevice;
typedef struct _NlRigidBodyDesc
{
	_NlRigidBodyDesc()
	{
		_mass = 1.0f;

		_isStatic = false;
		_isKinematic = false;
		_useGravity = true;
		
		_pos = float3(0);
		_rot = Quat::CreateIdentity();
	}
	
	float		_mass;

	bool		_isStatic;
	bool		_isKinematic;
	bool		_useGravity;

	// original global pose
	float3		_pos;
	Quat		_rot;

} NlRigidBodyDesc;

//====================================================
class NlGameObjectBase;
class NlJointBase;

class NlRigidBody
{
	friend class NlCollider;
	friend class NlBoxCollider;
	friend class NlSphereCollider;
	friend class NlCapsuleCollider;
	friend class NlMeshCollider;

	friend class NlFixedJoint;
	friend class NlHingeJoint;
	friend class NlSpringJoint;
	friend class NlCylinderJoint;
	friend class NlCharacterJoint;

	friend void NlClidEventManager::AddListener( NlCollider* col );
	friend void NlClidEventManager::RemoveListener(  NlCollider* col );

public:
	NlRigidBody( NlPhysicsDevice* device );
	~NlRigidBody(void);

	bool init( const NlRigidBodyDesc& desc );
	void Draw();	// colliders not attach through "Add"-function will not be drawn
	void destroy();

	void SetOwner( NlGameObjectBase* obj ) {m_owner = obj;}
	const NlGameObjectBase& GetOwner() const {return *m_owner;}
	
	bool AddBoxCollider( float3& sides, uint16 mtrlIndex = 0, bool isTrigger = false );
	bool AddBoxCollider( float3& sides, float3& locPos, Quat& locRot,
		uint16 mtrlIndex = 0, bool isTrigger = false );
	bool AddSphereCollider( float radius, uint16 mtrlIndex = 0, bool isTrigger = false );
	bool AddSphereCollider( float radius, float3& locPos, Quat& locRot,
		uint16 mtrlIndex = 0, bool isTrigger = false );
	bool AddCapsuleCollider( float radius, float height, uint16 mtrlIndex = 0, bool isTrigger = false );
	bool AddCapsuleCollider( float radius, float height, float3& locPos, Quat& locRot,
		uint16 mtrlIndex = 0, bool isTrigger = false );
	bool AddMeshCollider( NlMeshClidData& data, float3 scale = float3(1), uint16 mtrlIndex = 0, bool isTrigger = false );
	bool AddMeshCollider( NlMeshClidData& data, float3& locPos, Quat& locRot,
		float3 scale = float3(1), uint16 mtrlIndex = 0, bool isTrigger = false );

	// add a new joint on this rigid body. you can get the joint by its index later
	bool AddFixedJoint( uint16& index, NlRigidBody* connected );
	bool AddHingeJoint( uint16& index, NlRigidBody* connected, float3 localAnchor = float3(0), float3 localAxis = float3(1, 0, 0) );
	bool AddCylinderJoint( uint16& index, NlRigidBody* connected, float3 localAnchor = float3(0), float3 localAxis = float3(1, 0, 0) );
	bool AddSpringJoint( uint16& index, NlRigidBody* connected, float3 localAnchor = float3(0) );
	bool AddCharacerJoint( uint16& index, NlRigidBody* connected,
		float3 localAnchor = float3(0), float3 localAxis = float3(1, 0, 0), float3 swingAxis = float3(0, 1, 0) );
	NlJointBase* GetJointByIndex( uint16 index );

	void SetGlobalPose( float3 pos, Quat rot );
	void GetGlobalPose( float3& pos, Quat& rot );
	
	void SetKinematic( bool isKinematic );
	bool IsKinematic() {return m_isKinematic;}

	bool SetCenterOfMass( float3& pos );	// global position
	float GetMass() {return m_mass;}

	void AddForce( const float3& forceDir, const float strength );
	void AddTorque( const float3& forceDir, const float strength );
	void Move( const float3& velDir, float velocity );	// only valid when this is a kinematic one

private:
	NlPhysicsDevice* m_device;
	NxActor* m_pxActor;

	NlGameObjectBase*	m_owner;
	bool m_isKinematic;
	bool m_isStatic;

	float  m_mass;
	
	std::vector<NlCollider*> m_colliders;	// colliders(attaching through "Add"-function) on this rigid body
	uint32 m_collidersCount;				// number of all colliders on this rigid body, so may not equal to m_colliders.size()
	std::vector<NlJointBase*> m_joints;

	uint32 m_nbContactListener;	// if the value=0, pxActor will ignore Physics contact events

};

#endif