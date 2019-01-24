//////////////////////////////////////////////////////////////////////////
//
// Name:   	main.cpp
// Desc:	NlGFrammework for 3D game design
// 	
// 
// Author:	LiaoSheng
// Date:	2013/12/20
// 
//////////////////////////////////////////////////////////////////////////

#include "stableheader.h"

#include "NlGFramework.h"
#include "PullSticksApp.h"

#include <time.h>

#define WIN_WIDTH 1024
#define WIN_HEIGHT 600

// Main function===========================================================
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	char*    lpCmdLine,
	int       nCmdShow)
{
#if (defined(DEBUG) || defined(_DEBUG))
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// set random seed
	srand( time(0) );

	// create your game instance
	PullSticksApp game;	

	// create game framework
	NlGFramework framework;
	NlGFrameworkInitParam param;
	param.hInst = hInstance;
	param.width = WIN_WIDTH;
	param.height = WIN_HEIGHT;
	param.gameName = "Pull the Sticks";
	// give the game instance to framework
	param.app = &game;

	// initialize the game
	if (framework.Init( param ) == false)
	{
		MessageBox( NULL, "Init framework failed", "error", MB_OK );
		framework.Shutdown();
		exit( 0 );
	}

	// start to run the game
	framework.Run();

	// exit the game
	framework.Shutdown();

	return 0;
}
