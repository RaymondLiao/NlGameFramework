#include "DoubleModeScene.h"
#include "Ground.h"
#include "Basket.h"
#include "Stick.h"

//================================================================================
DoubleModeScene::DoubleModeScene(void): NlGameScene()
{
	m_timeDisplayer = NULL;

	m_title = m_pauseIF = NULL;

	m_scoreGraphA = m_scoreGraphB = NULL;
	m_loseA = m_loseB = NULL;

	m_scoreIF = NULL;
	m_scoreSum = NULL;

	m_ground = NULL;
	m_basketA = m_basketB = NULL;
	m_basketClothA = m_basketClothB = NULL;

}
//---------------------------------------------------------------------------------
DoubleModeScene::~DoubleModeScene(void)
{
}
//---------------------------------------------------------------------------------
bool DoubleModeScene::OnInit( NlGameEnvironment* gameEnv )
{
	// process superclass
	if (NlGameScene::OnInit( gameEnv ) == false)
		return false;

	// configure camera
	m_camera->SetClearColor( D3DCOLOR_ARGB(0, 26, 30, 28) );
	m_camera->SetPosition( float3(0, 0, -10.0f) );
	CCamera::modCamMaxDist = 12.0f;
	CCamera::modCamMinDist = 5.0f;

	// set camera for picker
	gameEnv->GetPicker()->SetCamera( m_camera );

	// create all the objects and initialize them ==================================
	// add the ground
	m_ground = new Ground( float2(30, 30) );
	assert( m_ground );
	assert( m_ground->init( gameEnv ) );
	m_objs.push_back( m_ground );

	// add sticks according to current level
	float2 xzOffset_1 = float2( 0.0f, 0.0f );
	float2 xzOffset_2 = float2( -0.5f, -0.5f );
	float2 xzOffset_3 = float2( -1.0f, 0.0f );

	m_stickCount = 0;
	if (m_level == 1)
	{
		Stick* pStick = NULL;
		float3 stickSize( 0.3f,1.8f, 0.3f );
		const float offset = 0.5f;
		for (int x = -3; x < 3; ++x)
			for (int y = 0; y < 10; ++y)
			{
				int type = rand()%4;
				switch(type)
				{
				case 0:
					pStick = new Stick( eStick_wood, stickSize );
					break;
				case 1:
					pStick = new Stick( eStick_rubber, stickSize );
					break;
				case 2:
					pStick = new Stick( eStick_glass, stickSize );
					break;
				case 3:
					pStick = new Stick( eStick_china, stickSize );
					break;
				}

				assert( pStick );
				assert( pStick->init( gameEnv ) );

				if (y%2 == 0)
				{
					pStick->SetPos( float3( xzOffset_1.x, (y+offset)*stickSize.z+stickSize.z*0.5f, (x+offset)*stickSize.x ) );
					pStick->SetRot( -90, 0, 90 );
				}
				else
				{
					pStick->SetPos( float3( xzOffset_1.x+(x+offset)*stickSize.x, (y+offset)*stickSize.z+stickSize.z*0.5f, 0) );
					pStick->SetRot( -90, 0, 0 );
				}

				if (y == 0)
					pStick->SetBase();

				m_objs.push_back( pStick );		m_stickCount++;
			}

		m_baseStickCount = 6;
	}
	else if (m_level == 2)
	{
		Stick* pStick = NULL;
		Stick* pStick1 = NULL;
		Stick* pStick2 = NULL;
		Stick* pStick3 = NULL;
		Stick* pStick4 = NULL;
		float3 stickSize( 0.45f,0.45f, 1.5f );

		for (int z=0;z<=2;z++)
		{
			pStick = new Stick( eStick_wood, stickSize );
			pStick->init( gameEnv );
			pStick->SetPos( float3( xzOffset_2.x+0.75*z, 0.5, xzOffset_2.y+.75) );
			pStick->SetRot(0, 0, 0 );
			m_objs.push_back( pStick );		m_stickCount++;
			pStick->SetBase();

			pStick2 = new Stick( eStick_wood, stickSize );
			pStick2->init( gameEnv );
			pStick2->SetPos( float3(xzOffset_2.x+0.75, 1, xzOffset_2.y+0.75*z ) );
			pStick2->SetRot(0, 90, 0 );
			m_objs.push_back( pStick2 );	m_stickCount++;

			if (z==1)
			{
				uint16 jointIndex;
				pStick2->GetRigidBody()->AddCharacerJoint( jointIndex, pStick->GetRigidBody(),
					(float3(1)),float3(1,0,0),float3(0,0,1));
				NlCharacterJoint* joint = (NlCharacterJoint*)pStick2->GetRigidBody()->GetJointByIndex( jointIndex );
				assert( joint );
			}
		}

		for (int x=0;x<=2;x++)
		{
			for (int z=0;z<=2;z++)
			{
				int type = rand()%4;
				switch(type)
				{
				case 0:
					pStick = new Stick( eStick_wood, stickSize );
					break;
				case 1:
					pStick = new Stick( eStick_rubber, stickSize );
					break;
				case 2:
					pStick = new Stick( eStick_glass, stickSize );
					break;
				case 3:
					pStick = new Stick( eStick_china, stickSize );
					break;
				}
				if(x==z||x-z==2||z-x==2)
				{
					pStick->init( gameEnv );
					pStick->SetPos( float3(xzOffset_2.x+(x+0.5)*0.5, 2, xzOffset_2.y+(z+0.5)*0.5 ) );
					pStick->SetRot(90, 0, 0 );
					m_objs.push_back( pStick );		m_stickCount++;
				}
				else
				{
					delete pStick;
					pStick = NULL;
				}
			}
		}

		pStick1 = new Stick( eStick_wood, stickSize );
		pStick1->init( gameEnv );
		pStick1->SetPos( float3( xzOffset_2.x+0.75, 2.5, xzOffset_2.y+0.1) );
		pStick1->SetRot(0, 90, 0 );
		m_objs.push_back( pStick1 );	m_stickCount++;

		pStick2 = new Stick( eStick_wood, stickSize );
		pStick2->init( gameEnv );
		pStick2->SetPos( float3( xzOffset_2.x+0.75, 2.5, xzOffset_2.y+1.4) );
		pStick2->SetRot(0, 90, 0 );
		m_objs.push_back( pStick2 );	m_stickCount++;

		pStick3 = new Stick( eStick_rubber, stickSize );
		pStick3->init( gameEnv );
		pStick3->SetPos( float3( xzOffset_2.x+0.2, 3, xzOffset_2.y+0.75) );
		pStick3->SetRot(0, 0, 0 );
		m_objs.push_back( pStick3 );	m_stickCount++;

		pStick4 = new Stick( eStick_rubber, stickSize );
		pStick4->init( gameEnv );
		pStick4->SetPos( float3( xzOffset_2.x+1.2, 3, xzOffset_2.y+0.75) );
		pStick4->SetRot(0, 0, 0 );
		m_objs.push_back( pStick4 );	m_stickCount++;

		uint16 jointIndex;
		pStick2->GetRigidBody()->AddFixedJoint( jointIndex, pStick1->GetRigidBody());
		NlCharacterJoint* joint = (NlCharacterJoint*)pStick2->GetRigidBody()->GetJointByIndex( jointIndex );
		pStick4->GetRigidBody()->AddFixedJoint( jointIndex, pStick3->GetRigidBody());
		NlCharacterJoint* joint2 = (NlCharacterJoint*)pStick4->GetRigidBody()->GetJointByIndex( jointIndex );
		assert( joint );
		assert( joint2 );

		pStick1 = new Stick( eStick_wood, stickSize );
		pStick1->init( gameEnv );
		pStick1->SetPos( float3( xzOffset_2.x+0.75, 3.5, xzOffset_2.y+0.5) );
		pStick1->SetRot(0, 90, 0 );
		m_objs.push_back( pStick1 );	m_stickCount++;

		pStick2 = new Stick( eStick_wood, stickSize );
		pStick2->init( gameEnv );
		pStick2->SetPos( float3( xzOffset_2.x+0.75, 3.5, xzOffset_2.y+1.0) );
		pStick2->SetRot(0, 90, 0 );
		m_objs.push_back( pStick2 );	m_stickCount++;

		m_baseStickCount = 3;
	}
	else if (m_level == 3)
	{
		Stick* pStick = NULL;
		Stick* pStick1 = NULL;
		Stick* pStick2 = NULL;
		float3 stickSize( 0.5f,0.5f, 1.5f );

		pStick = new Stick( eStick_wood, stickSize );
		pStick->init( gameEnv );
		pStick->SetPos( float3(xzOffset_3.x, 0.5, xzOffset_3.y) );
		pStick->SetRot(0, 0, 0 );
		m_objs.push_back( pStick );		m_stickCount++;
		pStick->SetBase();

		pStick = new Stick( eStick_glass, stickSize );
		pStick->init( gameEnv );
		pStick->SetPos( float3(xzOffset_3.x+1, 0.5, xzOffset_3.y-1) );
		pStick->SetRot(0, 90, 0 );
		m_objs.push_back( pStick );		m_stickCount++;
		pStick->SetBase();

		pStick = new Stick( eStick_rubber, stickSize );
		pStick->init( gameEnv );
		pStick->SetPos( float3(xzOffset_3.x+2, 0.5, xzOffset_3.y) );
		pStick->SetRot(0, 0, 0 );
		m_objs.push_back( pStick );		m_stickCount++;
		pStick->SetBase();

		pStick = new Stick( eStick_china, stickSize );
		pStick->init( gameEnv );
		pStick->SetPos( float3( xzOffset_3.x+1, 0.5, xzOffset_3.y+1) );
		pStick->SetRot(0, 90, 0 );
		m_objs.push_back( pStick );		m_stickCount++;
		pStick->SetBase();

		for (int x=0;x<=1;x++)
		{
			for(int y=0;y<=1;y++)
			{
				pStick1 = new Stick( eStick_wood, stickSize );
				pStick1->init( gameEnv );
				pStick1->SetPos( float3( xzOffset_3.x+x*2, 1, xzOffset_3.y+y*2-1) );
				pStick1->SetRot(90, 0, 0 );
				m_objs.push_back( pStick1 );	m_stickCount++;
				pStick1->SetBase();

				pStick2 = new Stick( eStick_china, stickSize );
				pStick2->init( gameEnv );
				pStick2->SetPos( float3( xzOffset_3.x+x*2 ,2, xzOffset_3.y+y*2-1) );
				if (x+y==1)
				{
					pStick2->SetRot(0, 0, 0 );
				}
				else
				{
					pStick2->SetRot(0, 90, 0 );
				}

				m_objs.push_back( pStick2 );	m_stickCount++;

				uint16 jointIndex;
				pStick2->GetRigidBody()->AddFixedJoint( jointIndex, pStick1->GetRigidBody() );
				NlCylinderJoint* joint = (NlCylinderJoint*)pStick2->GetRigidBody()->GetJointByIndex( jointIndex );
				assert( joint );
			}
		}

		pStick = new Stick( eStick_rubber, stickSize );
		pStick->init( gameEnv );
		pStick->SetPos( float3(xzOffset_3.x+0.5, 1.5, xzOffset_3.y+1) );
		pStick->SetRot(90, 0, 0 );
		m_objs.push_back( pStick );		m_stickCount++;

		pStick = new Stick( eStick_rubber, stickSize );
		pStick->init( gameEnv );
		pStick->SetPos( float3(xzOffset_3.x+2, 1.5, xzOffset_3.y+0.5) );
		pStick->SetRot(90, 0, 0 );
		m_objs.push_back( pStick );		m_stickCount++;

		pStick = new Stick( eStick_rubber, stickSize );
		pStick->init( gameEnv );
		pStick->SetPos( float3(xzOffset_3.x+1.5, 1.5, xzOffset_3.y-1) );
		pStick->SetRot(90, 0, 0 );
		m_objs.push_back( pStick );		m_stickCount++;

		pStick = new Stick( eStick_rubber, stickSize );
		pStick->init( gameEnv );
		pStick->SetPos( float3(xzOffset_3.x, 1.5, xzOffset_3.y-0.5) );
		pStick->SetRot(90, 0, 0 );
		m_objs.push_back( pStick );		m_stickCount++;


		//////////////////////////////////////////////////////////////////////////
		pStick = new Stick( eStick_glass, stickSize );
		pStick->init( gameEnv );
		pStick->SetPos( float3(xzOffset_3.x+0.45, 2.5, xzOffset_3.y-0.55) );
		pStick->SetRot(0, 135, 0 );
		m_objs.push_back( pStick );		m_stickCount++;

		pStick = new Stick( eStick_glass, stickSize );
		pStick->init( gameEnv );
		pStick->SetPos( float3(xzOffset_3.x+1.55, 2.5, xzOffset_3.y+0.55) );
		pStick->SetRot(0, 135, 0 );
		m_objs.push_back( pStick );		m_stickCount++;

		pStick = new Stick( eStick_china, stickSize );
		pStick->init( gameEnv );
		pStick->SetPos( float3(xzOffset_3.x+0.45, 3, xzOffset_3.y-0.45) );
		pStick->SetRot(0, 45, 0 );	
		m_objs.push_back( pStick );		m_stickCount++;

		pStick = new Stick( eStick_china, stickSize );
		pStick->init( gameEnv );
		pStick->SetPos( float3(xzOffset_3.x+1.55, 3, xzOffset_3.y+0.55) );
		pStick->SetRot(0, 45, 0 );
		m_objs.push_back( pStick );		m_stickCount++;

		////////////////////////////////////////////////////////////////////////////
		pStick1 = new Stick( eStick_china, stickSize );
		pStick1->init( gameEnv );
		pStick1->SetPos( float3(xzOffset_3.x+2, 3, xzOffset_3.y+-1) );
		pStick1->SetRot(90, 0, 0 );
		m_objs.push_back( pStick1 );	m_stickCount++;

		pStick2 = new Stick( eStick_china, stickSize );
		pStick2->init( gameEnv );
		pStick2->SetPos( float3(xzOffset_3.x, 3, xzOffset_3.y+1) );
		pStick2->SetRot(90, 0, 0 );
		m_objs.push_back( pStick2 );	m_stickCount++;

		uint16 jointIndex;
		pStick2->GetRigidBody()->AddFixedJoint( jointIndex, pStick1->GetRigidBody());
		NlCharacterJoint* joint = (NlCharacterJoint*)pStick2->GetRigidBody()->GetJointByIndex( jointIndex );
		assert( joint );

		m_baseStickCount = 8;
	}

	// add the cloths
	NlClothDesc clothDesc;
	clothDesc.thickness = 0.3f;

	clothDesc.color = float4( 1.0f, 0.85f, 0.85f, 1.0f );
	clothDesc.globalPos = float3( -4.0f, 5, 1.5f );
	m_basketClothA = new NlCloth( gameEnv->GetRenderDevice(), gameEnv->GetPhysicsDevice() );
	assert( m_basketClothA );
	assert( m_basketClothA->init( clothDesc, 4, 4, 5 ) == true );

	clothDesc.color = float4( 0.85f, 1.0f, 0.85f, 1.0f );
	clothDesc.globalPos = float3( 4.0f, 5, 1.5f );
	m_basketClothB = new NlCloth( gameEnv->GetRenderDevice(), gameEnv->GetPhysicsDevice() );
	assert( m_basketClothB );
	assert( m_basketClothB->init( clothDesc, 4, 4, 5 ) == true );

	// add the baskets
	m_basketA = new Basket( float3(3.2f, 2.4f, 2.8f) );
	assert( m_basketA );
	assert( m_basketA->init( gameEnv ) );
	m_basketA->SetPos( float3(-5.5f, 1.2f, 0) );
	m_objs.push_back( m_basketA );

	m_basketB = new Basket( float3(3.2f, 2.4f, 2.8f) );
	assert( m_basketB );
	assert( m_basketB->init( gameEnv ) );
	m_basketB->SetPos( float3(5.5f, 1.2f, 0) );
	m_objs.push_back( m_basketB );

	// add UI =====================================================================
	NlTextDesc textDesc;
	textDesc.font = eFT_Arial;
	textDesc.style = eFT_Bold;
	textDesc.alignment = eFT_Center;

	// time information
	textDesc.size = 100;
	textDesc.position = float2( 0.51f, 0.8f );
	m_timeDisplayer = new NlGUIText( gameEnv->GetRenderDevice() );
	assert( m_timeDisplayer );
	assert( m_timeDisplayer->init(textDesc) );
	m_remainTime = 15.0f;

	// title
	m_title = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_title );
	assert( m_title->init("UI/DoubleMode.png") );

	// pause interface
	m_pauseIF = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_pauseIF );
	assert( m_pauseIF->init("UI/PauseTheme.png") );

	// score interface
	m_scoreIF = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_scoreIF );
	assert( m_scoreIF->init("UI/WinTheme.png") );

	textDesc.size = 100;
	textDesc.position = float2( 0.51f, 0.45f );
	m_scoreSum = new NlGUIText( gameEnv->GetRenderDevice() );
	assert( m_scoreSum );
	assert( m_scoreSum->init(textDesc) );

	// score information
	m_scoreGraphA = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_scoreGraphA );
	assert( m_scoreGraphA->init("UI/CharaANotPlaying.png") );

	m_scoreGraphB = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_scoreGraphB );
	assert( m_scoreGraphB->init("UI/CharaBNotPlaying.png") );

	textDesc.size = 70;
	textDesc.position = float2( 0.27f, 0.935f );
	m_loseA = new NlGUIText( gameEnv->GetRenderDevice() );
	assert( m_loseA );
	assert( m_loseA->init( textDesc ) );

	textDesc.position = float2( 0.74f, 0.935f );
	m_loseB = new NlGUIText( gameEnv->GetRenderDevice() );
	assert( m_loseB );
	assert( m_loseB->init( textDesc ) );

	// finish initializing
	m_state = Game_Playing;
	m_playerATurn = true;
	m_isPlayerALose = false;
	return true;

}
//---------------------------------------------------------------------------------
void DoubleModeScene::OnUpdate( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );
	bool pause = (m_state == Game_Pausing | m_state == Game_End);
	gameEnv->PauseToggle( pause );
	if (pause)
		return;

	// process superclass
	NlGameScene::OnUpdate( gameEnv );

	// update time
	m_remainTime -= gameEnv->GetElapsedTime();
	if (m_remainTime < 0)
	{
		m_remainTime = 15.0f;
		m_playerATurn = !m_playerATurn;
		m_ground->SetTurn( m_playerATurn );
	}

	// check ending
	if (m_ground->GetFallenStickCount() > 5)
	{
		m_isPlayerALose = true;
		m_state = Game_End;
	}
	else if (m_ground->GetFallenStickCountB() > 5)
	{
		m_isPlayerALose = false;
		m_state = Game_End;
	}
	else if (m_ground->GetFallenStickCount()+m_ground->GetFallenStickCountB()+
		m_basketA->GetCollecStickCount()+m_basketB->GetCollecStickCount() > m_stickCount-m_baseStickCount)
	{
		m_isPlayerALose = (m_basketA->GetCollecStickCount()<m_basketB->GetCollecStickCount()) ?
			true : false;
		m_state = Game_End;
	}

}
//---------------------------------------------------------------------------------
void DoubleModeScene::OnDraw( NlGameEnvironment* gameEnv )
{
	// process superclass
	NlGameScene::OnDraw( gameEnv );

	if (m_basketClothA)
		m_basketClothA->Draw( gameEnv );
	if (m_basketClothB)
		m_basketClothB->Draw( gameEnv );

	if (m_timeDisplayer)
	{
		char buf[20] = {0};
		sprintf( buf, "%2.0f",
			m_remainTime );
		m_timeDisplayer->Draw( buf, m_remainTime>10 ? 0xffffffff : 0xffff8080 );
	}

	if (m_title)
		m_title->Draw( float2(0.5f, 565/600.0f), float2(326/1024.0f, 70/600.0f) );

	if (m_state == Game_Pausing )
		m_pauseIF->Draw( float2(0.5f, 0.5f), float2(450/1024.f, 306/600.f) );

	if (m_scoreGraphA)
	{
		m_playerATurn ? m_scoreGraphA->Draw( "UI/CharaAPlaying.png", float2(0.265f, 565/600.0f), float2(168/1024.0f, 70/600.0f) )
			: m_scoreGraphA->Draw( float2(0.265f, 565/600.0f), float2(168/1024.0f, 70/600.0f) );
	}
	if (m_scoreGraphB)
	{
		!m_playerATurn ? m_scoreGraphB->Draw( "UI/CharaBPlaying.png", float2(0.735f, 565/600.0f), float2(168/1024.0f, 70/600.0f) )
			: m_scoreGraphB->Draw( float2(0.735f, 565/600.0f), float2(168/1024.0f, 70/600.0f) );
	}
	if (m_loseA)
	{
		char buf[20] = {0};
		sprintf( buf, "%d",
			m_ground->GetFallenStickCount() );
		m_loseA->Draw( buf, 0xff000000 );
	}
	if (m_loseB)
	{	
		char buf[20] = {0};
		sprintf( buf, "%d", 
			m_ground->GetFallenStickCountB() );
		m_loseB->Draw( buf, 0xff000000 );
	}

	if (m_state == Game_End)
	{
		if (m_scoreIF)
			m_scoreIF->Draw( float2(0.5f, 0.5f), float2(450/1024.f, 306/600.f) );
		if (m_scoreSum)
		{
			char buf[20] = {0};
			if (m_isPlayerALose == false)	// playerA win
				sprintf( buf, "PlayerA - %d", 
					m_basketA->GetCollecStickCount() );
			else
				sprintf( buf, "PlayerB - %d",
					m_basketB->GetCollecStickCount() );
				
			m_scoreSum->Draw( buf, 0xffff8080 );
		}
	}

}
//---------------------------------------------------------------------------------
void DoubleModeScene::OnGUI( int2 wndResolution )
{
	if (NlImmGUI::ButtonCustom( "UI/BackInGameButton.png", "UI/BackInGameButton.png", "UI/BackInGameButtonDown.png",
		float2(92, 34), float2(184, 70) ) && m_state != Game_Pausing)
	{
		m_state = Game_ToMainMenu;
	}

	if (NlImmGUI::ButtonCustom( "UI/ResetButton.png", "UI/ResetButton.png", "UI/ResetButtonDown.png",
		float2(wndResolution.x-92, 34), float2(184, 70) ) && m_state != Game_Pausing)
	{
		m_state = Game_Reset;
	}

}
//---------------------------------------------------------------------------------
void DoubleModeScene::OnDestroy()
{
	// process superclass
	NlGameScene::OnDestroy();

	m_ground = NULL;
	m_basketA = m_basketB = NULL;
	if (m_basketClothA)
	{
		m_basketClothA->destroy();
		delete m_basketClothA;
		m_basketClothA = NULL;
	}
	if (m_basketClothB)
	{
		m_basketClothB->destroy();
		delete m_basketClothB;
		m_basketClothB = NULL;
	}

	if (m_timeDisplayer)
	{
		m_timeDisplayer->destroy();
		delete m_timeDisplayer;
		m_timeDisplayer = NULL;
	}

	if (m_title)
	{
		m_title->destroy();
		delete m_title;
		m_title = NULL;
	}
	if (m_pauseIF)
	{
		m_pauseIF->destroy();
		delete m_pauseIF;
		m_pauseIF = NULL;
	}

	if (m_scoreGraphA)
	{
		m_scoreGraphA->destroy();
		delete m_scoreGraphA;
		m_scoreGraphA = NULL;
	}
	if (m_scoreGraphB)
	{
		m_scoreGraphB->destroy();
		delete m_scoreGraphB;
		m_scoreGraphB = NULL;
	}
	if (m_loseA)
	{
		m_loseA->destroy();
		delete m_loseA;
		m_loseA = NULL;
	}
	if (m_loseB)
	{
		m_loseB->destroy();
		delete m_loseB;
		m_loseB = NULL;
	}

	if (m_scoreIF)
	{
		m_scoreIF->destroy();
		delete m_scoreIF;
		m_scoreIF = NULL;
	}
	if (m_scoreSum)
	{
		m_scoreSum->destroy();
		delete m_scoreSum;
		m_scoreSum = NULL;
	}

}
//---------------------------------------------------------------------------------
bool DoubleModeScene::OnInputEvent( const SInputEvent &event )
{
	// process superclass
	NlGameScene::OnInputEvent( event );

	switch (event.keyId)
	{
	case eKI_LAlt:
		if (event.state == eIS_Pressed && m_state != Game_End)
			m_state == Game_Pausing ? m_state = Game_Playing : m_state = Game_Pausing;
		break;

	}
	// always return false
	return false;

}