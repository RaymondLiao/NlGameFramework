#include "NlGFramework.h"
#include "NlGameEnvironment.h"
#include "NlGameInstanceBase.h"

#include "nl_render_header.h"
#include "nl_physics_header.h"

//=================================================================================
NlGFramework::NlGFramework( void )
{
}
//---------------------------------------------------------------------------------
NlGFramework::~NlGFramework( void )
{
}
//---------------------------------------------------------------------------------
bool NlGFramework::Init( const NlGFrameworkInitParam& createParam )
{
	HWND hWnd;
	NlRenderDevice*  pRenderDevice = NULL;
	NlPhysicsDevice* pPhyscisDevice = NULL;

	// init presenter
	NlPresenterParam pparam;
	pparam.hInst = createParam.hInst;
	pparam.windowName = createParam.gameName;
	pparam.width = createParam.width;
	pparam.height = createParam.height;
	pparam.bpp = 32;

	if (!m_presenter.Init( pparam ))
		return false;
	hWnd = m_presenter.m_hWnd;
	pRenderDevice = m_presenter.m_renderDevice;

	// create singleton-class instances
	NlTextureManager::Create();
	NlMtrlManager::Create();
	NlPxMtrlManager::Create();
	NlDebugRenderer::Create( pRenderDevice );

	// init px simulator
	if (!m_pxSimulator.Init())
		return false;
	pPhyscisDevice = m_pxSimulator.m_device;

	// create global game environment
	m_gameEnv = new NlGameEnvironment( hWnd, pRenderDevice, pPhyscisDevice, int2(pparam.width, pparam.height) );
	assert( m_gameEnv );
	NlImmGUI::Create( m_gameEnv );

	// init app
	m_app = createParam.app;
	if (m_app)
		m_app->OnInit( m_gameEnv );
	NlPxUserData::AddUserDataToActors( m_pxSimulator.m_device );	// set PhysX user-data

	// start physics simulation
	m_pxSimulator.Start();

	return true;

}
//---------------------------------------------------------------------------------
bool NlGFramework::Run()
{
	// main loop
	while(1)
	{
		if (m_presenter.ProcessMsg())
		{
			break;
		}

		Update();
	}

	return true;

}
//---------------------------------------------------------------------------------
bool NlGFramework::Shutdown()
{
	NlPxUserData::ReleaseUserDataFromActors( m_pxSimulator.m_device );	// clear PhysX user-data

	// destroy app
	if (m_app)
		m_app->OnShutdown();

	// destory game environment
	assert( m_gameEnv );
	delete m_gameEnv;
	m_gameEnv = NULL;

	NlTextureManager::Destroy();
	NlMtrlManager::Destroy();
	NlImmGUI::Destroy();
	NlDebugRenderer::Destroy();	// need destroy before presenter shutting down
	
	// destroy presenter & physics
	m_presenter.Shutdown();
	m_pxSimulator.ShutDown();

	NlPxMtrlManager::Destroy();	// need destroyed after simulator shutting down

	return true;

}
//---------------------------------------------------------------------------------
bool NlGFramework::Update()	// warn: order is crucial
{
	// clear back surface
	m_presenter.m_renderDevice->RenderToBackSurf();

	assert( m_gameEnv );
	if (m_gameEnv->GetMainCamera())
		m_presenter.m_renderDevice->Clear(  D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_gameEnv->GetMainCamera()->GetClearColor(), 1.0f, 0 );
	else
		m_presenter.m_renderDevice->Clear(  D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 );

	// update physics sim
	m_pxSimulator.Update( m_gameEnv );

	// update app
	if (m_app && m_app->CheckSceneReplacement( m_gameEnv ))
		m_app->OnUpdate( m_gameEnv );

	// update game environment, including input process
	m_gameEnv->update();

	// swap chain
	m_presenter.Swap();

	// start physics simulation
	m_pxSimulator.Start();

	return true;

}