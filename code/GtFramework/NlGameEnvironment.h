/*
  @file   NlGameEnvironment.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlGameEnvironment_h_
#define _NlGameEnvironment_h_

#include "prerequisite.h"

class NlRenderDevice;
class NlPhysicsDevice;
class NlGameRenderKit;
class CCamera;

class NlGameTimer;
class SrInputManager;
class NlPxPicker;

//====================================================
class NlGameEnvironment
{
	friend class NlGFramework;

public:
	HWND&				GetwindowHandle();
	const int2&			GetWndResolution();

	NlRenderDevice*		GetRenderDevice();
	NlPhysicsDevice*	GetPhysicsDevice();

	void				PauseToggle( bool open ) {m_isPausing = open;}

	float				GetTime();
	float				GetElapsedTime();
	SrInputManager*		GetInputer();
	NlPxPicker*			GetPicker();

	void				SetMainCamera( CCamera* camera );
	CCamera*			GetMainCamera();

	void				UpdateShaderToggle( bool open );
	void				DebugDrawingToggle( bool open );
	bool				IfUpdateShader();
	bool				IfDrawForDebug();

private:
	NlGameEnvironment( HWND hWnd, NlRenderDevice* renderDevice, NlPhysicsDevice* physicsDevice, int2 wndResolution );
	~NlGameEnvironment();

	void update();
	void ProcessInput();

private:
	HWND m_hWnd;
	NlRenderDevice*		m_renderDevice;
	NlPhysicsDevice*	m_physicsDevice;

	bool				m_isPausing;

    NlGameTimer*		m_timer;
	SrInputManager*		m_inputer;
	NlGameRenderKit*	m_renderKit;

	NlPxPicker*			m_picker;
	bool				m_isPicking;

	int2 m_wndResolution;

	// temporary
public:
	float3 m_lightDir;

};

#endif