#include "SingleModeScene.h"
#include "Ground.h"
#include "Basket.h"
#include "Stick.h"

//================================================================================
SingleModeScene::SingleModeScene(void): NlGameScene()
{
	m_ground = NULL;
	m_basket = NULL;
	m_basketCloth = NULL;

	m_timeDisplayer = NULL;

	m_title = m_pauseIF = NULL;

	m_scoreGraph = m_loseGraph = NULL;
	m_score = m_lose = NULL;

	m_scoreIF = NULL;
	m_scoreSum = NULL;

}
//---------------------------------------------------------------------------------
SingleModeScene::~SingleModeScene(void)
{
}
//---------------------------------------------------------------------------------
bool SingleModeScene::OnInit( NlGameEnvironment* gameEnv )
{
	// process superclass
	if (NlGameScene::OnInit( gameEnv ) == false)
		return false;

	// configure camera
	m_camera->SetClearColor( D3DCOLOR_ARGB(0, 26, 30, 28) );
	m_camera->SetPosition( float3(0, 0, -10.0f) );
	CCamera::modCamMaxDist = 10.0f;
	CCamera::modCamMinDist = 5.0f;

	// set camera for picker
	gameEnv->GetPicker()->SetCamera( m_camera );

	// create all the objects and initialize them ==================================
	// add the ground
	m_ground = new Ground( float2(15, 15) );
	assert( m_ground );
	assert( m_ground->init( gameEnv ) );
	m_objs.push_back( m_ground );

	// add sticks according to current level
	float2 xzOffset_1 = float2( 2.5f, 0.0f );
	float2 xzOffset_2 = float2( 2.0f, -0.5f );
	float2 xzOffset_3 = xzOffset_1;

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

	}

	// add the cloth
	NlClothDesc clothDesc;
	clothDesc.color = float4( 1.0f, 0.85f, 0.85f, 1.0f );
	clothDesc.globalPos = float3( -1.5f, 5, 1.5f );
	clothDesc.thickness = 0.3f;
	m_basketCloth = new NlCloth( gameEnv->GetRenderDevice(), gameEnv->GetPhysicsDevice() );
	assert( m_basketCloth );
	assert( m_basketCloth->init( clothDesc, 4, 4, 5 ) == true );

	// add the basket
	m_basket = new Basket( float3(2.8f, 2.4f, 2.8f) );
	assert( m_basket );
	assert( m_basket->init( gameEnv ) );
	m_basket->SetPos( float3(-2.5f, 1.2f, 0) );
	m_objs.push_back( m_basket );

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
	m_remainTime = 150.0f;

	// title
	m_title = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_title );
	assert( m_title->init("UI/SingleMode.png") );

	// pause interface
	m_pauseIF = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_pauseIF );
	assert( m_pauseIF->init("UI/PauseTheme.png") );

	// score interface
	m_scoreIF = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_scoreIF );
	assert( m_scoreIF->init("UI/WinTheme.png") );
	
	textDesc.size = 150;
	textDesc.position = float2( 0.51f, 0.45f );
	m_scoreSum = new NlGUIText( gameEnv->GetRenderDevice() );
	assert( m_scoreSum );
	assert( m_scoreSum->init(textDesc) );

	// score information
	m_scoreGraph = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_scoreGraph );
	assert( m_scoreGraph->init("UI/Score.png") );

	m_loseGraph = new NlGUITexture( gameEnv->GetRenderDevice() );
	assert( m_loseGraph );
	assert( m_loseGraph->init("UI/Falls.png") );

	textDesc.size = 70;
	textDesc.position = float2( 0.29f, 0.93f );
	m_score = new NlGUIText( gameEnv->GetRenderDevice() );
	assert( m_score );
	assert( m_score->init( textDesc ) );

	textDesc.position = float2( 0.77f, 0.93f );
	m_lose = new NlGUIText( gameEnv->GetRenderDevice() );
	assert( m_lose );
	assert( m_lose->init( textDesc ) );

	// finish initializing
	m_state = Game_Playing;
	return true;

}
//---------------------------------------------------------------------------------
void SingleModeScene::OnUpdate( NlGameEnvironment* gameEnv )
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

	// check ending
	if (m_ground->GetFallenStickCount() > 5 || 
		m_basket->GetCollecStickCount()+m_ground->GetFallenStickCount() >= m_stickCount ||
		m_remainTime < 0)
		m_state = Game_End;
	//else if (m_level == 2)
	//{
	//	if (m_ground->GetFallenStickCount() >= 8 || 
	//		m_basket->GetCollecStickCount()+m_ground->GetFallenStickCount() >= m_stickCount ||
	//		m_remainTime < 0)
	//		m_state = Game_End;
	//}
	//else if (m_level == 3)
	//{
	//	if (m_ground->GetFallenStickCount() >= 5 || 
	//		m_basket->GetCollecStickCount()+m_ground->GetFallenStickCount() >= m_stickCount ||
	//		m_remainTime < 0)
	//		m_state = Game_End;
	//}

}
//---------------------------------------------------------------------------------
void SingleModeScene::OnDraw( NlGameEnvironment* gameEnv )
{
	// process superclass
	NlGameScene::OnDraw( gameEnv );

	if (m_basketCloth)
		m_basketCloth->Draw( gameEnv );

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
	
	if (m_scoreGraph)
		m_scoreGraph->Draw( float2(0.265f, 565/600.0f), float2(168/1024.0f, 70/600.0f) );
	if (m_loseGraph)
		m_loseGraph->Draw( float2(0.735f, 565/600.0f), float2(168/1024.0f, 70/600.0f) );
	if (m_score)
	{
		char buf[20] = {0};
		sprintf( buf, "%d",
			m_basket->GetCollecStickCount() );
		m_score->Draw( buf, 0xffff8080 );
	}
	if (m_lose)
	{	
		char buf[20] = {0};
		sprintf( buf, "%d", 
			m_ground->GetFallenStickCount() );
		m_lose->Draw( buf, 0xff000000 );
	}

	if (m_state == Game_End)
	{
		if (m_scoreIF)
			m_scoreIF->Draw( float2(0.5f, 0.5f), float2(450/1024.f, 306/600.f) );
		if (m_scoreSum)
		{
			char buf[20] = {0};
			sprintf( buf, "%d", 
				m_basket->GetCollecStickCount() );

			m_scoreSum->Draw( buf, 0xffff8080 );
		}
	}
	
}
//---------------------------------------------------------------------------------
void SingleModeScene::OnGUI( int2 wndResolution )
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
void SingleModeScene::OnDestroy()
{
	// process superclass
	NlGameScene::OnDestroy();

	m_ground = NULL;
	m_basket = NULL;
	if (m_basketCloth)
	{
		m_basketCloth->destroy();
		delete m_basketCloth;
		m_basketCloth = NULL;
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

	if (m_scoreGraph)
	{
		m_scoreGraph->destroy();
		delete m_scoreGraph;
		m_scoreGraph = NULL;
	}
	if (m_loseGraph)
	{
		m_loseGraph->destroy();
		delete m_loseGraph;
		m_loseGraph = NULL;
	}
	if (m_score)
	{
		m_score->destroy();
		delete m_score;
		m_score = NULL;
	}
	if (m_lose)
	{
		m_lose->destroy();
		delete m_lose;
		m_lose = NULL;
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
bool SingleModeScene::OnInputEvent( const SInputEvent &event )
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