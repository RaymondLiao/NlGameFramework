#include "NlGameEnvironment.h"
#include "NlRenderDevice.h"
#include "NlPhysicsDevice.h"
#include "InputManager.h"
#include "NlPxPicker.h"

//================================================================================
class NlGameTimer
{
	friend class NlGameEnvironment;
private:
	NlGameTimer():m_time(0) ,m_elapsedTime(0) {}
	~NlGameTimer() {}

	void init()
	{
		m_elapsedTime = 0;
		QueryPerformanceFrequency( &m_freq );
		m_time = GetRealTime();
	}

	void update()
	{
		QueryPerformanceFrequency( &m_freq );
		float newTime = GetRealTime();
		m_elapsedTime = newTime - m_time;
		m_time = newTime;
	}

	void Stop()
	{
		m_elapsedTime = 0;
	}

	float GetElapsedTime()
	{
		return m_elapsedTime;
	}

	float GetTime()
	{
		return GetRealTime();
	}

	float GetRealTime()
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter( &now );
		return (float)(now.QuadPart)/m_freq.QuadPart;
	}

private:
	float m_time;
	float m_elapsedTime;

	// high precision
	LARGE_INTEGER m_freq;
};
//================================================================================
// this class contains render information
class NlGameRenderKit
{
	friend class NlGameEnvironment;
private:
	NlGameRenderKit(): m_mainCamera(NULL), m_updateShader(false), m_drawForDebug(false) {}
	~NlGameRenderKit() {}

private:
	CCamera* m_mainCamera;

	bool m_updateShader;
	bool m_drawForDebug;
};

//=================================================================================
NlGameEnvironment::NlGameEnvironment( HWND hWnd, NlRenderDevice* renderDevice, NlPhysicsDevice* physicsDevice, int2 wndResolution ):
	m_hWnd(hWnd), m_renderDevice(renderDevice), m_physicsDevice(physicsDevice), m_wndResolution(wndResolution)
{
	assert( m_renderDevice && m_physicsDevice );

	m_isPausing = false;

	m_timer = new NlGameTimer();
	m_inputer = new SrInputManager();
	m_renderKit = new NlGameRenderKit();
	assert( m_timer && m_inputer && m_renderKit );

	m_picker = new NlPxPicker( m_hWnd, m_renderDevice, m_physicsDevice );
	assert( m_picker );
	m_isPicking = false;

	m_timer->init();
	m_inputer->Init( m_hWnd );

	// temporary
	m_lightDir = float3( 2, -2, 1 );
}
//---------------------------------------------------------------------------------
NlGameEnvironment::~NlGameEnvironment()
{
	if (m_timer)
	{
		delete m_timer;
		m_timer = NULL;
	}
	
	if (m_renderKit)
	{
		delete m_renderKit;
		m_renderKit = NULL;
	}	

	if (m_inputer)
	{
		m_inputer->Destroy();
		delete m_inputer;
		m_inputer = NULL;
	}
	
	if (m_picker)
	{
		delete m_picker;
		m_isPicking = NULL;
	}
	
}
//---------------------------------------------------------------------------------
void NlGameEnvironment::update()
{
	ProcessInput();

	if (m_isPausing == true)
	{
		m_timer->Stop();
		return;
	}

	// update timer
	m_timer->update();

	// update picker
	POINT pt;
	GetCursorPos( &pt );
	m_picker->update( pt );
	if (m_renderKit->m_drawForDebug)
		m_picker->Draw();

}
//---------------------------------------------------------------------------------
HWND& NlGameEnvironment::GetwindowHandle()
{
	return m_hWnd;
}
//---------------------------------------------------------------------------------
NlRenderDevice* NlGameEnvironment::GetRenderDevice()
{
	return m_renderDevice;
}
//---------------------------------------------------------------------------------
NlPhysicsDevice* NlGameEnvironment::GetPhysicsDevice()
{
	return m_physicsDevice;
}
//---------------------------------------------------------------------------------
const int2& NlGameEnvironment::GetWndResolution()
{
	return m_wndResolution;
}
//---------------------------------------------------------------------------------
float NlGameEnvironment::GetTime()
{
	return m_timer->GetTime();
}
//---------------------------------------------------------------------------------
float NlGameEnvironment::GetElapsedTime()
{
	return m_timer->GetElapsedTime();
}
//---------------------------------------------------------------------------------
SrInputManager* NlGameEnvironment::GetInputer()
{
	return m_inputer;
}
//---------------------------------------------------------------------------------
NlPxPicker* NlGameEnvironment::GetPicker()
{
	return m_picker;
}
//---------------------------------------------------------------------------------
void NlGameEnvironment::SetMainCamera( CCamera* camera )
{
	m_renderKit->m_mainCamera = camera;
}
//---------------------------------------------------------------------------------
CCamera* NlGameEnvironment::GetMainCamera()
{
	return m_renderKit->m_mainCamera;
}
//---------------------------------------------------------------------------------
void NlGameEnvironment::UpdateShaderToggle( bool open )
{
	m_renderKit->m_updateShader = open;
}
//---------------------------------------------------------------------------------
bool NlGameEnvironment::IfUpdateShader()
{
	return m_renderKit->m_updateShader;
}
//---------------------------------------------------------------------------------
void NlGameEnvironment::DebugDrawingToggle( bool open )
{
	m_renderKit->m_drawForDebug = open;
}
//---------------------------------------------------------------------------------
bool NlGameEnvironment::IfDrawForDebug()
{
	return m_renderKit->m_drawForDebug;
}
//---------------------------------------------------------------------------------
void NlGameEnvironment::ProcessInput()
{
	if (GetFocus() != m_hWnd)
		return;

	m_inputer->Update();
	
	// common events-------------------
	if (isKeyDown(VK_LBUTTON))
	{
		if (m_isPicking == false)
		{
			assert( m_picker );
			POINT pt;
			GetCursorPos( &pt );
			m_picker->PickActor( pt );

			m_isPicking = true;
		}
	}
	else
	{
		m_picker->LetGoActor();
		m_isPicking = false;
	}

}