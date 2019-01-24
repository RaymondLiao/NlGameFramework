/*
  @file   NlGFramework.h
  
  @author Liaosheng

  @brief  Game Framework

  更改日志 history
  ver:1.0
 */

#ifndef _NlGFramework_h_
#define _NlGFramework_h_

#include "prerequisite.h"
#include "NlPresenter.h"
#include "NlPhysicsSimulator.h"

class NlGameEnvironment;
class NlGameInstanceBase;

struct NlGFrameworkInitParam
{
	HINSTANCE hInst;
	int width;
	int height;
	const char* gameName;

	NlGameInstanceBase* app;

	NlGFrameworkInitParam()
	{
		memset( this, 0, sizeof(NlGFrameworkInitParam) );
	}
};

//====================================================
class NlGFramework
{
public:
	NlGFramework(void);
	~NlGFramework(void);

	bool Init( const NlGFrameworkInitParam& createParam );
	bool Run();
	bool Shutdown();

private:
	bool Update();

private:
	NlPresenter			m_presenter;	// contains render-device
	NlPhysicsSimulator  m_pxSimulator;	// contains physics-device
	NlGameEnvironment*	m_gameEnv;		// contains game information

	NlGameInstanceBase* m_app;
};

#endif