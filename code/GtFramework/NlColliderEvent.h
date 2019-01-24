/*
  @file   NlColliderEvent.h
  
  @author Liaosheng

  @brief  collider events processor

  更改日志 history
  ver:1.0
 */

#ifndef _NlColliderEvent_h_
#define _NlColliderEvent_h_

#include "prerequisite.h"

//====================================================
// collider_message delegation
class NlPhysicsDevice;
class NlCollider;

class NlClidEventDelegate
{
public:
	// message sent to collider
	virtual void OnCollisionEnter( const NlCollider& another ) {}
	virtual void OnCollisionExit( const NlCollider& another )  {}
	virtual void OnCollisionStay( const NlCollider& another )  {}

	// message sent to trigger
	virtual void OnTriggerEnter( const NlCollider& another )   {}
	virtual void OnTriggerExit( const NlCollider& another )    {}
	virtual void OnTriggerStay( const NlCollider& another )	   {}
};

//====================================================
enum ENlClidEvent
{
	eCLID_Nothing,
	eCLID_ColEnter,
	eCLID_ColExit,
	eCLID_ColStay,
	eCLID_TriEnter,
	eCLID_TriExit,
	eCLID_TriStay
};

class NlClidEventManager	// hold and update by NlPhysicsDevice
{
public:
	NlClidEventManager( NlPhysicsDevice* device );
	~NlClidEventManager();

	void AddListener( NlCollider* col );
	void RemoveListener( NlCollider* col );

	void update();

private:
	NlPhysicsDevice* m_device;
	std::vector<NlCollider*> m_colliders;

	// collider event information-------------
	struct EventInfo
	{
		EventInfo()
		{
			collider = NULL;
			type = eCLID_Nothing;
			pairID = 0;
		}

		NlCollider* collider;
		ENlClidEvent type;
		uint32	pairID;
	};
	std::vector<EventInfo> m_contactInfos;
	std::vector<EventInfo> m_triggerInfos;

	static bool SortEventInfo( const EventInfo& info1, const EventInfo& info2 )
	{
		return info1.pairID < info2.pairID;
	}

};

#endif