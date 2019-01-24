/*
  @file   NlGameSceneBase.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlGameSceneBase_h_
#define _NlGameSceneBase_h_

#include "stableheader.h"

#include "nl_render_header.h"
#include "nl_physics_header.h"

#include "NlGameEnvironment.h"
#include "NlGameObject.h"
#include "NlGameObjPri.h"

//====================================================
class NlGameSceneBase
{
public:
	virtual ~NlGameSceneBase() {}
	
	virtual bool OnInit( NlGameEnvironment* gameEnv ) = 0;
	virtual void OnUpdate( NlGameEnvironment* gameEnv ) = 0;
	virtual void OnDraw( NlGameEnvironment* gameEnv ) = 0;
	virtual void OnGUI( int2 wndResolution ) {};
	virtual void OnDestroy() = 0;

};

//====================================================
class NlGameScene :
	public NlGameSceneBase,
	public IInputEventListener
{
public:
	NlGameScene();
	virtual ~NlGameScene();

	virtual bool OnInit( NlGameEnvironment* gameEnv );
	virtual void OnUpdate( NlGameEnvironment* gameEnv );
	virtual void OnDraw( NlGameEnvironment* gameEnv );
	virtual void OnDestroy();

	virtual bool OnInputEvent( const SInputEvent &event );
	bool IfGetReset() {return m_reset;}

protected:
	SrInputManager* m_inputer;
	CCamera*	m_camera;

	bool m_reset;			// if reset?
	bool m_drawForDebug;	// if draw for debugging?

	std::vector<NlGameObjectBase*> m_objs;

};

#endif