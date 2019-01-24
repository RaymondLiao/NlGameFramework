#pragma once

#include "NlGameSceneBase.h"

//====================================================
class MainMenu :
	public NlGameScene
{
public:
	enum State
	{
		Menu_MainIF,
		Menu_SelModeIF,
		Menu_SelLevelIF,
		Menu_ToGame,
		Menu_HelpIF,
		Menu_AboutIF
	} m_state;

public:
	MainMenu(void);
	~MainMenu(void);

	virtual bool OnInit( NlGameEnvironment* gameEnv );
	virtual void OnUpdate( NlGameEnvironment* gameEnv );
	virtual void OnDraw( NlGameEnvironment* gameEnv );
	virtual void OnGUI( int2 wndResolution );
	virtual void OnDestroy();

	virtual bool OnInputEvent( const SInputEvent &event );

public:
	// game-loading information
	bool m_loadSingle;
	int	 m_loadLevel;

private:
	// background textures
	NlGUITexture* m_mainMenuBK;
	NlGUITexture* m_selModeBK;
	NlGUITexture* m_selLevelBK;
	NlGUITexture* m_helpBK;
	NlGUITexture* m_aboutBK;

};