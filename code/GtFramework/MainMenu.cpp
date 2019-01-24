#include "MainMenu.h"

#include "NlSkyBox.h"
#include "NlTerrain.h"
#include "NlWater.h"

//================================================================================
MainMenu::MainMenu(void):
	NlGameScene()
{
	m_mainMenuBK = NULL;
	m_selModeBK = NULL;
	m_selLevelBK = NULL;
	m_helpBK = NULL;
	m_aboutBK = NULL;

	m_loadSingle = NULL;
	m_loadLevel = 1;

}
//---------------------------------------------------------------------------------
MainMenu::~MainMenu(void)
{
}
//---------------------------------------------------------------------------------
bool MainMenu::OnInit( NlGameEnvironment* gameEnv )
{
	// process superclass
	if (NlGameScene::OnInit( gameEnv ) == false)
		return false;

	// background
	m_mainMenuBK = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_mainMenuBK );
	assert( m_mainMenuBK->init( "UI/Maintheme.jpg" ) );

	m_selModeBK = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_selModeBK );
	assert( m_selModeBK->init( "UI/Modetheme.jpg") );

	m_selLevelBK = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_selLevelBK );
	assert( m_selLevelBK->init( "UI/Leveltheme.jpg") );

	m_helpBK = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_helpBK );
	assert( m_helpBK->init( "UI/Helptheme.jpg") );

	m_aboutBK = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_aboutBK );
	assert( m_aboutBK->init( "UI/Abouttheme.jpg") );

	// finish initialization
	m_state = Menu_MainIF;
	return true;

}
//---------------------------------------------------------------------------------
void MainMenu::OnUpdate( NlGameEnvironment* gameEnv )
{
	// process superclass
	NlGameScene::OnUpdate( gameEnv );

}
//---------------------------------------------------------------------------------
void MainMenu::OnDraw( NlGameEnvironment* gameEnv )
{
	// process superclass
	NlGameScene::OnDraw( gameEnv );

	// draw background
	switch (m_state)
	{
	case Menu_MainIF:
		if (m_mainMenuBK)
			m_mainMenuBK->Draw( float2(0.5f, 0.5f), float2(1, 1) );
		break;

	case Menu_SelModeIF:
		if (m_selModeBK)
			m_selModeBK->Draw( float2(0.5f, 0.5f), float2(1, 1) );
		break;

	case Menu_SelLevelIF:
		if (m_selLevelBK)
			m_selLevelBK->Draw( float2(0.5f, 0.5f), float2(1, 1) );
		break;

	case Menu_HelpIF:
		if (m_helpBK)
			m_helpBK->Draw( float2(0.5f, 0.5f), float2(1, 1) );
		break;

	case Menu_AboutIF:
		if (m_aboutBK)
			m_aboutBK->Draw( float2(0.5f, 0.5f), float2(1, 1) );

	}


}
//---------------------------------------------------------------------------------
void MainMenu::OnGUI( int2 wndResolution )
{
	switch (m_state)
	{
	case Menu_MainIF:
		if (NlImmGUI::ButtonCustom( "UI/BeginButton.png", "UI/BeginButtonHold.png", "UI/BeginButtonDown.png",
			float2(wndResolution.x-250, wndResolution.y-330), float2(323, 67) ))
		{
			m_state = Menu_SelModeIF;
		}
		if (NlImmGUI::ButtonCustom( "UI/HelpButton.png", "UI/HelpButtonHold.png", "UI/HelpButtonDown.png",
			float2(wndResolution.x-250, wndResolution.y-250), float2(323, 67) ))
		{
			m_state = Menu_HelpIF;
		}
		if (NlImmGUI::ButtonCustom( "UI/AboutButton.png", "UI/AboutButtonHold.png", "UI/AboutButtonDown.png",
			float2(wndResolution.x-250, wndResolution.y-160), float2(323, 67) ))
		{
			m_state = Menu_AboutIF;
		}
		break;

	case Menu_SelModeIF:
		if (NlImmGUI::ButtonCustom( "UI/SingleModeButton.png", "UI/SingleModeButton.png", "UI/SingleModeButtonDown.png",
			float2(wndResolution.x*0.5f-220, wndResolution.y*0.55f), float2(249, 224) ))
		{
			m_loadSingle = true;
			m_state = Menu_SelLevelIF;
		}
		if (NlImmGUI::ButtonCustom( "UI/DoubleModeButton.png", "UI/DoubleModeButton.png", "UI/DoubleModeButtonDown.png",
			float2(wndResolution.x*0.5f+220, wndResolution.y*0.55f), float2(249, 224) ))
		{
			m_loadSingle = false;
			m_state = Menu_SelLevelIF;
		}
		//if (NlImmGUI::ButtonCustom( "UI/Back.png", "UI/BackHold.png", "UI/BackDown.png",
		//	float2(wndResolution.x*0.5f, wndResolution.y*0.9f), float2(323, 67) ))
		//{
		//	m_loadSingle = true;
		//	m_state = Menu_MainIF;
		//}
		break;

	case Menu_SelLevelIF:
		if (NlImmGUI::ButtonCustom( "UI/LevelButton1.png", "UI/LevelButton1.png", "UI/LevelButton1Down.png",
			float2(wndResolution.x*0.2f, wndResolution.y*0.55f), float2(187, 187) ))
		{
			m_loadLevel = 1;
			m_state = Menu_ToGame;
		}
		if (NlImmGUI::ButtonCustom( "UI/LevelButton2.png", "UI/LevelButton2.png", "UI/LevelButton2Down.png",
			float2(wndResolution.x*0.5f, wndResolution.y*0.55f), float2(187, 187) ))
		{
			m_loadLevel = 2;
			m_state = Menu_ToGame;
		}
		if (NlImmGUI::ButtonCustom( "UI/LevelButton3.png", "UI/LevelButton3.png", "UI/LevelButton3Down.png",
			float2(wndResolution.x*0.8f, wndResolution.y*0.55f), float2(187, 187) ))
		{
			m_loadLevel = 3;
			m_state = Menu_ToGame;
		}
		if (NlImmGUI::ButtonCustom( "UI/Back.png", "UI/BackHold.png", "UI/BackDown.png",
			float2(wndResolution.x*0.5f, wndResolution.y*0.85f), float2(323, 67) ))
		{
			m_loadSingle = true;
			m_state = Menu_MainIF;
		}

		break;

	case Menu_HelpIF:
		if (NlImmGUI::ButtonCustom( "UI/Back.png", "UI/BackHold.png", "UI/BackDown.png",
			float2(wndResolution.x-220, wndResolution.y-50), float2(323, 67) ))
		{
			m_state = Menu_MainIF;
		}
		break;

	case Menu_AboutIF:
		if (NlImmGUI::ButtonCustom( "UI/Back.png", "UI/BackHold.png", "UI/BackDown.png",
			float2(220, wndResolution.y-80), float2(323, 67) ))
		{
			m_state = Menu_MainIF;
		}
		break;

	}
	

}
//---------------------------------------------------------------------------------
void MainMenu::OnDestroy()
{
	// process superclass
	NlGameScene::OnDestroy();

	// destroy background texture
	if (m_mainMenuBK)
	{
		m_mainMenuBK->destroy();
		delete m_mainMenuBK;
		m_mainMenuBK = NULL;
	}
	if (m_selModeBK)
	{
		m_selModeBK->destroy();
		delete m_selModeBK;
		m_selModeBK = NULL;
	}
	if (m_selLevelBK)
	{
		m_selLevelBK->destroy();
		delete m_selLevelBK;
		m_selLevelBK = NULL;
	}
	if (m_helpBK)
	{
		m_helpBK->destroy();
		delete m_helpBK;
		m_helpBK = NULL;
	}
	if (m_aboutBK)
	{
		m_aboutBK->destroy();
		delete m_aboutBK;
		m_aboutBK = NULL;
	}

}
//---------------------------------------------------------------------------------
bool MainMenu::OnInputEvent( const SInputEvent &event )
{
	// process superclass
	NlGameScene::OnInputEvent( event );

	// always return false
	return false;

}