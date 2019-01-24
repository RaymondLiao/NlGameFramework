#include "PullSticksApp.h"

#include "NlGameEnvironment.h"

#include "nl_render_header.h"
#include "nl_physics_header.h"

//================================================================================
PullSticksApp::PullSticksApp(void):
	NlGameInstanceBase()
{
	m_gameEnv = NULL;
}
//---------------------------------------------------------------------------------
PullSticksApp::~PullSticksApp(void)
{
}
//---------------------------------------------------------------------------------
bool PullSticksApp::OnInit( NlGameEnvironment* gameEnv )
{
	if (gameEnv == NULL)
		return false;

	// store point to game environment for switch scenes
	m_gameEnv = gameEnv;
	// start to listen to input-events
	m_gameEnv->GetInputer()->AddListener( this );

	m_curScene = &m_mainMenu;

	return LoadScene( gameEnv );
	
}
//---------------------------------------------------------------------------------
void PullSticksApp::OnUpdate( NlGameEnvironment* gameEnv )
{
	if (gameEnv == NULL)
		return;

	assert( m_curScene );
	m_curScene->OnUpdate( gameEnv );

	// draw the scene----------------------------------------
	NlRenderDevice* renderDevice = gameEnv->GetRenderDevice();
	assert(renderDevice);
	if (true == renderDevice->BeginFrame())
	{
		m_curScene->OnDraw( gameEnv );
		m_curScene->OnGUI( gameEnv->GetWndResolution() );

		renderDevice->EndFrame();
	}

	// check scene info-------------------------------------
	if (m_curScene == &m_mainMenu)
	{
		switch( m_mainMenu.m_state )
		{
		case MainMenu::Menu_ToGame:
			if (m_mainMenu.m_loadSingle == true)
			{
				m_singleMode.SetLevel( m_mainMenu.m_loadLevel );
				SwitchGameScene( gameEnv, &m_singleMode );
			}
			else
			{
				m_doubleMode.SetLevel( m_mainMenu.m_loadLevel );
				SwitchGameScene( gameEnv, &m_doubleMode );
			}
			break;
		}
	}
	else if (m_curScene == &m_singleMode)
	{
		switch( m_singleMode.m_state )
		{
		case SingleModeScene::Game_ToMainMenu:
			SwitchGameScene( gameEnv, &m_mainMenu );
			break;

		case SingleModeScene::Game_Reset:
			SwitchGameScene( gameEnv, &m_singleMode );
			break;
		}
	}
	else if (m_curScene == &m_doubleMode)
	{
		switch( m_doubleMode.m_state )
		{
		case DoubleModeScene::Game_ToMainMenu:
			SwitchGameScene( gameEnv, &m_mainMenu );
			break;

		case DoubleModeScene::Game_Reset:
			SwitchGameScene( gameEnv, &m_doubleMode );
			break;
		}
	}

}
//---------------------------------------------------------------------------------
void PullSticksApp::OnShutdown()
{
	// stop listening
	assert( m_gameEnv );
	m_gameEnv->GetInputer()->RemoveListener( this );

	assert( m_curScene );
	m_curScene->OnDestroy();

}
//---------------------------------------------------------------------------------
bool PullSticksApp::OnInputEvent( const SInputEvent &event )
{
	/*switch( event.keyId )
	{
	 press f2 to switch scenes
	case eKI_F2:
		if (event.state == eIS_Pressed)
		{
			assert( m_gameEnv );
			if (m_curScene == &m_mainMenu)
				SwitchGameScene( m_gameEnv, &m_singleMode );
			else 
				SwitchGameScene( m_gameEnv, &m_mainMenu );
		}
		break;
	}*/

	// always return false
	return false;

}
