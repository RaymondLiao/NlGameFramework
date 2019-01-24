#include "NlGameInstanceBase.h"
#include "NlGameSceneBase.h"
#include "NlGameEnvironment.h"

//================================================================================
NlGameInstanceBase::NlGameInstanceBase(): m_curScene(NULL), m_nextScene(NULL)
{
}
//---------------------------------------------------------------------------------
NlGameInstanceBase::~NlGameInstanceBase()
{
}
//---------------------------------------------------------------------------------
bool NlGameInstanceBase::LoadScene( NlGameEnvironment* gameEnv )
{
	assert ( m_curScene );

	return m_curScene->OnInit( gameEnv );
}
//---------------------------------------------------------------------------------
void NlGameInstanceBase::SwitchGameScene( NlGameEnvironment* gameEnv, NlGameSceneBase* newScene )
{
	assert( gameEnv );
	assert( newScene );

	//if (newScene == m_curScene)
	//	return;
	m_nextScene = newScene;

	gameEnv->GetPhysicsDevice()->ResetDevice();	// mark reset task

	NlPxUserData::ReleaseUserDataFromActors( gameEnv->GetPhysicsDevice() );	// clear PhysX user-data
	m_curScene->OnDestroy();

}
//---------------------------------------------------------------------------------
bool NlGameInstanceBase::CheckSceneReplacement( NlGameEnvironment* gameEnv )
{
	if (m_nextScene == NULL)
		return true;

	assert( gameEnv );

	m_curScene = m_nextScene;
	if (m_curScene->OnInit( gameEnv ) == false)
		return false;

	NlPxUserData::AddUserDataToActors( gameEnv->GetPhysicsDevice() );	// reset PhysX user-data

	m_nextScene =NULL;
	return true;

}