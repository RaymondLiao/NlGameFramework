#include "stableheader.h"
#include "NlPresenter.h"
#include "nl_render_header.h"

// lead in d3d
#include "d3d9.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dxsdk/lib/x86/d3dx9.lib")

//=================================================================================
LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//=================================================================================
NlPresenter::NlPresenter(void)
{
	m_hWnd = 0;
	m_renderDevice = NULL;
}
//---------------------------------------------------------------------------------
NlPresenter::~NlPresenter(void)
{
}
//---------------------------------------------------------------------------------
bool NlPresenter::Init( const NlPresenterParam& initParam )
{
	// create window-----------------------------------------------
	// register window
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= DefaultWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= initParam.hInst;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= "GtFramework Window Class";

	wcex.hIcon			= NULL;
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);

	// create window
	m_hWnd = CreateWindow("GtFramework Window Class", initParam.windowName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, initParam.width, initParam.height, NULL, NULL, initParam.hInst, NULL);

	// adjust size
	RECT realRect;
	GetClientRect(m_hWnd, &realRect);

	int width = realRect.right - realRect.left;
	int height = realRect.bottom - realRect.top;
	width = initParam.width * 2 - width;
	height = initParam.height * 2 - height;

	MoveWindow(m_hWnd, GetSystemMetrics(SM_CXSCREEN)/2 - width/2, GetSystemMetrics(SM_CYSCREEN)/2 - height/2, width, height, FALSE);

	if (!m_hWnd)
	{
		return false;
	}

	// show window
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	// create render device---------------------------------------------
	m_renderDevice = new NlRenderDevice();
	assert( m_renderDevice );
	if (false == m_renderDevice->init(initParam.width, initParam.height, m_hWnd, true))
		return false;

	return true;
}
//---------------------------------------------------------------------------------
bool NlPresenter::Swap()
{
	m_renderDevice->Present();
	return true;
}
//---------------------------------------------------------------------------------
bool NlPresenter::Shutdown()
{
	if (m_renderDevice)
	{
		m_renderDevice->destroy();
		delete m_renderDevice;
	}

	return true;
}
//---------------------------------------------------------------------------------
bool NlPresenter::ProcessMsg()
{
	MSG msg;
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (msg.message != WM_QUIT)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			return true;
		}
	}

	return false;
}