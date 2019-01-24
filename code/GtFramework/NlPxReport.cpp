#include "NlPxReport.h"
#include "NlPxUserData.h"

//=================================================================================
void NlPxContactReport::onContactNotify( NxContactPair& pair, NxU32 events )
{
	NxContactStreamIterator it( pair.stream );

	while (it.goNextPair())
	{
		// mark shape
		NlPxShapeUserData::ContactInfo info;
		info.contactEvent = events;

		NlPxShapeUserData *sud0 = NULL, *sud1 = NULL;
		NxShape* shape0 = it.getShape( 0 );
		NxShape* shape1 = it.getShape( 1 ); 

		sud0 = (NlPxShapeUserData*)shape0->userData;
		sud1 = (NlPxShapeUserData*)shape1->userData;

		if (sud0 && sud1)
		{
			info.eventPairID = sud1->id;
			sud0->contactEvents.push_back( info );

			info.eventPairID = sud0->id;
			sud1->contactEvents.push_back( info );
		}
	}

}

//=================================================================================
void NlPxTriggerReport::onTrigger( NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status )
{
	NlPxShapeUserData::TriggerEnfo info;
	info.triggerEvent = status;

	NlPxShapeUserData *sud0 = NULL, *sud1 = NULL;

	sud0 = (NlPxShapeUserData*)triggerShape.userData;
	sud1 = (NlPxShapeUserData*)otherShape.userData;

	if (sud0 && sud1)
	{
		info.eventPairID = sud1->id;
		sud0->triggerEvents.push_back( info );
		info.eventPairID = sud0->id;
		sud1->triggerEvents.push_back( info );
	}

}