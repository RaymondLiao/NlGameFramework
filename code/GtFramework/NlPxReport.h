/*
  @file   NlPxReport.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlPxReport_h_
#define _NlPxReport_h_

#include "NxPhysics.h"

// contact-report====================================================
struct NlPxContactPoint
{
	NlPxContactPoint( const NxVec3& _p ): p(_p) {}
	NxVec3 p;
};
typedef NxArray<NlPxContactPoint> NlPxContactPointArr;
extern NlPxContactPointArr g_contactPointArr;

class NlPxContactReport :
	public NxUserContactReport
{
public:
	virtual void onContactNotify( NxContactPair& pair, NxU32 events );

};

// trigger-report====================================================
class NlPxTriggerReport :
	public NxUserTriggerReport
{
public:
	virtual void onTrigger( NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status );

};

#endif