/*
  @file   NlGameInstanceBase.h
  
  @author Liaosheng

  @brief  Game App Base

  更改日志 history
  ver:1.0
 */

#ifndef _NlGameInstanceBase_h_
#define _NlGameInstanceBase_h_

#include "prerequisite.h"

class NlGameSceneBase;
class NlGameEnvironment;
//====================================================
class NlGameInstanceBase
{
public:
	NlGameInstanceBase();
	virtual ~NlGameInstanceBase();

	// do all the initialization tasks, called by NlGFramework::Init()
	virtual bool OnInit( NlGameEnvironment* gameEnv ) = 0;
	
	// do all the logic and drawing tasks, called every frame by NlGFramework::Update()
	virtual void OnUpdate( NlGameEnvironment* gameEnv ) = 0;

	// do all the clear tasks, called by NlGFramework::Shutdown()
	virtual void OnShutdown() = 0;

	// if the next scene has been assigned, replace scene
	bool CheckSceneReplacement( NlGameEnvironment* gameEnv );

protected:
	// initialize current game-scene
	bool LoadScene( NlGameEnvironment* gameEnv );

	// switch game scene, will destroy the old one
	virtual void SwitchGameScene( NlGameEnvironment* gameEnv, NlGameSceneBase* newScene );

protected:
	NlGameSceneBase* m_curScene;
	NlGameSceneBase* m_nextScene;

};

#endif