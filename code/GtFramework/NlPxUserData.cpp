#include "NlPxUserData.h"
#include "NlPhysicsDevice.h"

uint32 NlPxUserData::actorID = 0;
uint32 NlPxUserData::shapeID = 0;
//=================================================================================
void NlPxUserData::AddUserDataToActors( NlPhysicsDevice* device )
{
	assert( device );
	NxScene* scene = device->GetScene();
	assert( scene );

	actorID = shapeID = 0;

	NxActor** actors = scene->getActors();
	uint32 actorCount = scene->getNbActors();

	while (actorCount--)
	{
		NxActor* actor = actors[actorCount];
		actor->userData = new NlPxActorUserData;
		((NlPxActorUserData*)(actor->userData))->id = actorID++;

		AddUserDataToShapes( actor );
	}
}
//---------------------------------------------------------------------------------
void NlPxUserData::AddUserDataToShapes( NxActor* actor )
{
	NxShape *const* shapes = actor->getShapes();
	uint32 shapeCount = actor->getNbShapes();

	while (shapeCount--)
	{
		NxShape* shape = shapes[shapeCount];
		shape->userData = new NlPxShapeUserData;

		NlPxShapeUserData* sud = (NlPxShapeUserData*)(shape->userData);
		sud->id = shapeID++;
		if (shape->getType() == NX_SHAPE_CONVEX)
		{
			sud->shapeMesh = new NxConvexMeshDesc;
			shape->isConvexMesh()->getConvexMesh().saveToDesc( *(NxConvexMeshDesc*)sud->shapeMesh );
		}
		if (shape->getType() == NX_SHAPE_MESH)
		{
			sud->shapeMesh = new NxTriangleMeshDesc;
			shape->isTriangleMesh()->getTriangleMesh().saveToDesc( *(NxTriangleMeshDesc*)sud->shapeMesh );
		}
	}
}
//---------------------------------------------------------------------------------
void NlPxUserData::ReleaseUserDataFromActors( NlPhysicsDevice* device )
{
	assert( device );
	NxScene* scene = device->GetScene();
	assert( scene );

	NxActor** actors = scene->getActors();
	uint32 actorCount = scene->getNbActors();

	while (actorCount--)
	{
		NxActor* actor = actors[actorCount];
		if (actor->userData)
		{
			delete actor->userData;
			actor->userData = NULL;
		}

		ReleaseUserDataFromShapes( actor );
	}
}
//---------------------------------------------------------------------------------
void NlPxUserData::ReleaseUserDataFromShapes( NxActor* actor )
{
	NxShape *const* shapes = actor->getShapes();
	uint32 shapeCount = actor->getNbShapes();
	while (shapeCount--)
	{
		NxShape* shape = shapes[shapeCount];
		if (shape->userData)
		{
			NlPxShapeUserData* sud = (NlPxShapeUserData*)(shape->userData);	// there may be some problems
			if (sud)
			{
				if (sud->shapeMesh)
				{
					delete sud->shapeMesh;
					sud->shapeMesh = 0;
				}
				delete (NlPxShapeUserData*)(shape->userData);
			}
			else
				delete shape->userData;
		}
		
		shape->userData = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlPxShapeUserData::ClearEvents()
{
	contactEvents.clear();
	triggerEvents.clear();
}