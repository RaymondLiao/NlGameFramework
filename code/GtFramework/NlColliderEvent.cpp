#include "nl_physics_header.h"
#include <algorithm>

//=================================================================================
NlClidEventManager::NlClidEventManager( NlPhysicsDevice* device )
{
	assert( device );
	m_device = device;
}
//---------------------------------------------------------------------------------
NlClidEventManager::~NlClidEventManager()
{

}
//---------------------------------------------------------------------------------
void NlClidEventManager::AddListener( NlCollider* col )
{
	assert( col );

	std::vector<NlCollider*>::iterator it;
	for (it = m_colliders.begin(); it != m_colliders.end(); ++it)
	{
		if (*it == col)
			return;
	}
	m_colliders.push_back( col );

	if (col->m_isTrigger == true)
		return;

	// set contact pairs----------------------------------
	NlRigidBody* rigidBody0 = col->m_attachedRigBody;
	NxActor* pActor0 = rigidBody0->m_pxActor;

	for (it = m_colliders.begin(); it != m_colliders.end(); ++it)
	{
		if ((*it)->m_isTrigger == true)
			continue;

		NlRigidBody* rigidBody1 = (*it)->m_attachedRigBody;
		if(rigidBody0 != rigidBody1)
		{
			NxActor* pActor1 = rigidBody1->m_pxActor;
			m_device->GetScene()->setActorPairFlags( *pActor0, *pActor1,
				NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH );

			rigidBody0->m_nbContactListener++;
			rigidBody1->m_nbContactListener++;
		}
	}

}
//---------------------------------------------------------------------------------
void NlClidEventManager::RemoveListener( NlCollider* col )
{
	assert( col );

	std::vector<NlCollider*>::iterator it;
	for (it = m_colliders.begin(); it != m_colliders.end(); )
	{
		if (*it == col)
		{
			it = m_colliders.erase( it );

			if (col->m_isTrigger == true)
				return;

			// clear contact pairs------------------------------
			NlRigidBody* rigidBody0 = col->m_attachedRigBody;
			NxActor* pActor0 =rigidBody0->m_pxActor;

			bool clearFlag = (--(rigidBody0->m_nbContactListener) == 0);

			for (it = m_colliders.begin(); it != m_colliders.end(); ++it)
			{
				if ((*it)->m_isTrigger == true)
					continue;

				NlRigidBody* rigidBody1 = (*it)->m_attachedRigBody;
				if(rigidBody0 != rigidBody1)
				{
					NxActor* pActor1 = (*it)->m_attachedRigBody->m_pxActor;
					rigidBody1->m_nbContactListener--;

					if (clearFlag == true)
						m_device->GetScene()->setActorPairFlags( *pActor0, *pActor1,
						NX_IGNORE_PAIR );
				}
			}

			return;
		}
		else
			++it;
	}
}

//---------------------------------------------------------------------------------
void NlClidEventManager::update()
{
	// check contact and trigger events
	std::vector<NlCollider*>::iterator it;
	for (it = m_colliders.begin(); it != m_colliders.end();)
	{
		if (*it)
		{
			NxShape* shape = (*it)->m_pxShape;
			assert( shape );
			NlPxShapeUserData* sud = (NlPxShapeUserData*)shape->userData;

			if (sud->contactEvents.size() != 0)
			{
				for (int i = 0; i < sud->contactEvents.size(); ++i)
				{
					if (sud->contactEvents[i].contactEvent & NX_NOTIFY_ON_START_TOUCH)
					{
						EventInfo info;
						info.collider = (*it);
						info.type = eCLID_ColEnter;
						info.pairID = sud->contactEvents[i].eventPairID;

						m_contactInfos.push_back( info );
					}
					if (sud->contactEvents[i].contactEvent & NX_NOTIFY_ON_END_TOUCH)
					{
						EventInfo info;
						info.collider = (*it);
						info.type = eCLID_ColExit;
						info.pairID = sud->contactEvents[i].eventPairID;

						m_contactInfos.push_back( info );
					}
					if (sud->contactEvents[i].contactEvent & NX_NOTIFY_ON_TOUCH)
					{
						EventInfo info;
						info.collider = (*it);
						info.type = eCLID_ColStay;
						info.pairID = sud->contactEvents[i].eventPairID;

						m_contactInfos.push_back( info );
					}
				}
			}

			if ((*it)->m_isTrigger == true)
			{
				if (sud->triggerEvents.size() != 0)
				{
					for (int i = 0; i < sud->triggerEvents.size(); ++i)
					{
						if (sud->triggerEvents[i].triggerEvent & NX_TRIGGER_ON_ENTER)
						{
							EventInfo info;
							info.collider = (*it);
							info.type = eCLID_TriEnter;
							info.pairID = sud->triggerEvents[i].eventPairID;

							m_triggerInfos.push_back( info );
						}
						if (sud->triggerEvents[i].triggerEvent & NX_TRIGGER_ON_LEAVE)
						{
							EventInfo info;
							info.collider = (*it);
							info.type = eCLID_TriExit;
							info.pairID = sud->triggerEvents[i].eventPairID;

							m_triggerInfos.push_back( info );
						}
						if (sud->triggerEvents[i].triggerEvent & NX_TRIGGER_ON_STAY)
						{
							EventInfo info;
							info.collider = (*it);
							info.type = eCLID_TriStay;
							info.pairID = sud->triggerEvents[i].eventPairID;

							m_triggerInfos.push_back( info );
						}
					}
				}
			}

			// clear events
			sud->ClearEvents();
			++it;
		}
		else
		{
			it = m_colliders.erase( it );
		}
	}

	if (m_contactInfos.size() == 0 && m_triggerInfos.size() == 0)
		return;

	// post message
	{
		sort( m_contactInfos.begin(), m_contactInfos.end(), SortEventInfo );
		std::vector<EventInfo>::iterator it;
		std::vector<NlCollider*>::iterator innerIt = m_colliders.end()-1;

		for (it = m_contactInfos.begin(); it != m_contactInfos.end(); ++it)
		{
			while ( (*it).pairID != 
				((NlPxShapeUserData*)(*innerIt)->m_pxShape->userData)->id )
				innerIt--;

			switch ((*it).type)
			{
			case eCLID_ColEnter:
				{
					(*it).collider->PostColliderEvent( (*innerIt), eCLID_ColEnter );
				}break;

			case eCLID_ColExit:
				{
					(*it).collider->PostColliderEvent( (*innerIt), eCLID_ColExit );
				}break;

			case eCLID_ColStay:
				{
					(*it).collider->PostColliderEvent( (*innerIt), eCLID_ColStay );
				}break;
			}
		}

	}

	{
		sort( m_triggerInfos.begin(), m_triggerInfos.end(), SortEventInfo );
		std::vector<EventInfo>::iterator it;
		std::vector<NlCollider*>::iterator innerIt = m_colliders.end()-1;

		for (it = m_triggerInfos.begin(); it != m_triggerInfos.end(); ++it)
		{
			while ( (*it).pairID != 
				((NlPxShapeUserData*)(*innerIt)->m_pxShape->userData)->id )
				innerIt--;

			switch ((*it).type)
			{
			case eCLID_TriEnter:
				{
					(*it).collider->PostColliderEvent( (*innerIt), eCLID_TriEnter );
				}break;

			case eCLID_TriExit:
				{
					(*it).collider->PostColliderEvent( (*innerIt), eCLID_TriExit );
				}break;

			case eCLID_TriStay:
				{
					(*it).collider->PostColliderEvent( (*innerIt), eCLID_TriStay );
				}break;
			}
		}

	}

	// clear event informations
	m_contactInfos.clear();
	m_triggerInfos.clear();

}