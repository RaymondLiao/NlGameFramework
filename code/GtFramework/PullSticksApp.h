#pragma once

#include "NlGameInstanceBase.h"
#include "MainMenu.h"
#include "SingleModeScene.h"
#include "DoubleModeScene.h"

//====================================================
class PullSticksApp :
	public NlGameInstanceBase,
	public IInputEventListener
{
public:
	PullSticksApp(void);
	~PullSticksApp(void);

	virtual bool OnInit( NlGameEnvironment* gameEnv );
	virtual void OnUpdate( NlGameEnvironment* gameEnv );
	virtual void OnShutdown();

	virtual bool OnInputEvent( const SInputEvent &event );

private:
	NlGameEnvironment* m_gameEnv;	// used to switch scenes. However, we'd better don't store the point to gameEnv in general

	MainMenu		m_mainMenu;
	SingleModeScene	m_singleMode;
	DoubleModeScene m_doubleMode;

};