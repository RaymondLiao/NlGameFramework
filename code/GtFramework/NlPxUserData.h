/*
  @file   NlPxUserData.h
  
  @author Liaosheng

  @brief  PhysX user_data

  更改日志 history
  ver:1.0
 */

#ifndef _NlPxUserData_h_
#define _NlPxUserData_h_

#include "prerequisite.h"

class NlPhysicsDevice;

//====================================================
class NlPxUserData
{
public:
	static void AddUserDataToActors( NlPhysicsDevice* device );
	static void ReleaseUserDataFromActors( NlPhysicsDevice* device );

private:
	static void AddUserDataToShapes( NxActor* actor );
	static void ReleaseUserDataFromShapes( NxActor* actor );

private:
	static uint32 actorID;
	static uint32 shapeID;

};

//====================================================
enum eNlPxUserDataFlag
{
	eUD_IsHWActor		= (1 << 0),
	eUD_IsTRIGGER		= (1 << 1)
};

struct NlPxActorUserData
{
	uint32 id;
	uint32 flags;

	NlPxActorUserData()
	{
		id = 0;
		flags = 0;
	}
};

//====================================================
struct NlPxShapeUserData
{
	NlPxShapeUserData()
	{
		id = 0;
		flags = 0;
		shapeMesh = NULL;
	}
	~NlPxShapeUserData()
	{
		ClearEvents();
	}

	uint32 id;
	uint32 flags;
	void* shapeMesh;	// hold shape_mesh_description that used to save shape's mesh

	//-----------------------------------------
	struct ContactInfo
	{
		ContactInfo()
		{
			contactEvent = eventPairID = 0;
		}
		uint32 contactEvent;
		uint32 eventPairID;
	};
	struct TriggerEnfo
	{
		TriggerEnfo()
		{
			triggerEvent = eventPairID = 0;
		}
		uint32 triggerEvent;
		uint32 eventPairID;
	};
	std::vector<ContactInfo> contactEvents;
	std::vector<TriggerEnfo> triggerEvents;

	void ClearEvents();

};

#endif