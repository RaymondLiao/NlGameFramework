/*
  @file   NlPxPicker.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlPxPicker_h_
#define _NlPxPicker_h_

#include "prerequisite.h"

class NlRenderDevice;
class NlPhysicsDevice;
class CCamera;
class NlPxPicker
{
public:
	friend class NlGameEnvironment;

	NlPxPicker( HWND hWnd, NlRenderDevice* RD, NlPhysicsDevice* PD );
	~NlPxPicker(void);

	void SetCamera( CCamera* camera );	// need camera

private:
	void update( POINT& pt );
	bool PickActor( POINT& pt );
	void LetGoActor();	// stop picking
	void Draw();	// draw for debugging

	// transform a point from world-coord to screen-coord
	// used to record hit-point depth in view-coord
	void ViewProject( NxVec3& v, int& winX, int& winY, float& depth );
	// transform a point from screen-coord to world-coord
	// depth is the z-distance of the point to viewpoint
	void ViewUnProject( int winX, int winY, float depth, NxVec3& v );

	NxActor* CreateHitSphere( const NxVec3& pos );

	HWND			 m_hWnd;
	NlRenderDevice*  m_RD;
	NlPhysicsDevice* m_PD;
	CCamera* m_camera;

	NxJoint*		 m_hitJoint;
	NxActor*		 m_hitSphere;		// PhysX actor with a sphere shape
	NxReal			 m_hitDepth;

	NxActor*		 m_selected;		// PhysX actor that is picked
	
};

#endif