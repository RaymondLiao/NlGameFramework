#include "NlGameSceneBase.h"

//================================================================================
NlGameScene::NlGameScene(): m_inputer(NULL), m_camera(NULL)
{
	m_reset = false;
}
//---------------------------------------------------------------------------------
NlGameScene::~NlGameScene()
{
}
//---------------------------------------------------------------------------------
bool NlGameScene::OnInit( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );
	m_drawForDebug = gameEnv->IfDrawForDebug();

	// store inputer and start to listen to input-events
	m_inputer = gameEnv->GetInputer();
	assert( m_inputer );
	m_inputer->AddListener( this );

	// create camera
	int2 winSolution = gameEnv->GetWndResolution();
	m_camera = new CCamera( DEG2RAD(45), winSolution.x / (float)winSolution.y, 0.1f, 2000.0f );
	assert( m_camera );
	gameEnv->SetMainCamera( m_camera );	// remember to set camera to the game environment
	NlDebugRenderer::SetCamera( m_camera );	// debug render also needs a camera

	return true;
}
//---------------------------------------------------------------------------------
void NlGameScene::OnUpdate( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );
	gameEnv->DebugDrawingToggle( m_drawForDebug );

	// update camera
	if (m_camera)
		UpdateCamControl_ModelCam( gameEnv->GetwindowHandle(), *m_camera, float3(0,1.5,0), 0.005 );

	// update all common objects
	std::vector<NlGameObjectBase*>::iterator it;
	for (it = m_objs.begin(); it != m_objs.end(); ++it)
	{
		if (*it)
			(*it)->update( gameEnv );
	}

}
//---------------------------------------------------------------------------------
void NlGameScene::OnDraw( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );

	// draw all common renderbles in the scene
	std::vector<NlGameObjectBase*>::iterator it;
	for (it = m_objs.begin(); it != m_objs.end(); ++it)
	{
		if (*it)
		{
			if (dynamic_cast<NlRenderble*>(*it) != NULL)
				((NlRenderble*)(*it))->Draw( gameEnv );

			if (dynamic_cast<NlCombRenderble*>(*it) != NULL)
				((NlCombRenderble*)(*it))->Draw( gameEnv );

			if (dynamic_cast<NlRenderableRaw*>(*it) != NULL)
				((NlRenderableRaw*)(*it))->Draw(gameEnv);
		}
	}

	// PhysX debug render
	/*NxScene* pxScene = gameEnv->getPhysicsDevice()->GetScene();
	uint32 actorCount = pxScene->getNbActors();
	NxActor** actors = pxScene->getActors();
	while (actorCount--)
	{
		NlDebugRenderer::DrawCoordSystem( actors[actorCount] );

		uint32 shapeCount = actors[actorCount]->getNbShapes();
		NxShape *const* shapes = actors[actorCount]->getShapes();

		while (shapeCount--)
		{
			NlDebugRenderer::DrawWireShape( shapes[shapeCount], float3(0, 1, 0) );
		}
		
	}*/

}
//---------------------------------------------------------------------------------
void NlGameScene::OnDestroy()
{
	// stop listening
	assert( m_inputer );
	m_inputer->RemoveListener( this );

	// destroy camera
	if (m_camera)
	{
		delete m_camera;
		m_camera = NULL;
	}

	// destroy all common objects
	std::vector<NlGameObjectBase*>::iterator it;
	for (it = m_objs.begin(); it != m_objs.end(); ++it)
	{
		if ((*it) != NULL)
		{
			(*it)->destroy();
			delete (*it);
			(*it) = NULL;
		}
	}

}
//---------------------------------------------------------------------------------
bool NlGameScene::OnInputEvent( const SInputEvent &event )
{
	// process common input event
	switch( event.keyId)
	{
		// draw for debugging toggle
	case eKI_F1:
		if (event.state == eIS_Pressed)
			m_drawForDebug = !m_drawForDebug;
		break;
	}

	// always return false
	return false;

}
