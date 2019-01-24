//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2013
// 
//
// Name:   	main.cpp
// Desc:		hello world 3d ��������
// 	
// 
// Author:	YiKaiming
// Date:		2013/8/11
// 
//////////////////////////////////////////////////////////////////////////

// Ԥ����ͷ�ļ�
#include "stableheader.h"
// presenter
#include "NlPresenter.h"
#include "BufferObject.h"
#include "ShaderObject.h"
#include "mathlib/math_def.h"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	char*    lpCmdLine,
	int       nCmdShow)
{
#if (defined(DEBUG) || defined(_DEBUG))
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	NlPresenterParam param;
	param.hInst = hInstance;
	param.windowName = "Hello World 3D";
	param.width = 512;
	param.height = 512;
	param.bpp = 32;

	NlPresenter presenter;
	presenter.Init( param );


	//////////////////////////////////////////////////////////////////////////
	// 4. ��Ⱦѭ��
	while(1)
	{
		if (presenter.ProcessMsg())
		{
			break;
		}
		presenter.Swap();
	}

	//////////////////////////////////////////////////////////////////////////
	// 6.presenter����
	presenter.Shutdown();

	return 0;
}
