//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	CameraObject.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/10/7
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _CameraObject_h_
#define _CameraObject_h_

#include "prerequisite.h"

class NlSkyBox;
class CCamera
{
public:
	CCamera( float vFov, float aspect, float nearPlane, float farPlane )
	{
		m_fov = vFov;
		m_aspect = aspect;
		m_zn = nearPlane;
		m_zf = farPlane;
		m_projMatrix.PerspectiveFovLH( m_fov, m_aspect, m_zn, m_zf );

		m_cacheDirty = true;

		m_position = float3(0,0,0);
		m_rotation = Quat::CreateIdentity();

		m_clearColor = D3DCOLOR_ARGB(0, 0, 0, 0);
		m_skyBox = NULL;
	}

	~CCamera()
	{

	}

	Quat GetRotation()
	{
		return m_rotation;
	}

	float3 GetPosition()
	{
		return m_position;
	}

	void SetPosition(const float3& pos)
	{
		m_position = pos;
		m_cacheDirty = true;
	}

	void SetRotation(const Quat& rot)
	{
		m_rotation = rot;
		m_cacheDirty = true;
	}

	void SetNearClip( float zn)
	{
		m_zn = zn;
		m_projMatrix.PerspectiveFovLH( m_fov, m_aspect, m_zn, m_zf );
		m_cacheDirty = true;
	}

	void SetFarClip( float zn)
	{
		m_zf = zn;
		m_projMatrix.PerspectiveFovLH( m_fov, m_aspect, m_zn, m_zf );
		m_cacheDirty = true;
	}

	void SetAspect( float aspect)
	{
		m_aspect = aspect;
		m_projMatrix.PerspectiveFovLH( m_fov, m_aspect, m_zn, m_zf );
		m_cacheDirty = true;
	}

	void SetVFov( float fov )
	{
		m_fov = fov;
		m_projMatrix.PerspectiveFovLH( m_fov, m_aspect, m_zn, m_zf );
		m_cacheDirty = true;
	}

	const float44& GetViewMatrix()
	{
		return m_viewMatrix;
	}
	const float44& GetProjectMatrix()
	{
		return m_projMatrix;
	}

	const float44& GetViewProjectionMatrix()
	{
		if (m_cacheDirty)
		{
			m_cacheDirty = false;

			float3 frontDir = m_rotation.GetColumn2();
			frontDir.normalize();

			float3 upDir = float3(0,1,0);
			float3 rightDir = upDir % frontDir;
			rightDir.normalize();

			upDir = frontDir % rightDir;
			upDir.normalize();

			m_viewMatrix.row0 = float4(rightDir,0);
			m_viewMatrix.row1 = float4(upDir,0);
			m_viewMatrix.row2 = float4(frontDir,0);
			m_viewMatrix.row3 = float4(m_position, 1.0);
			m_viewMatrix.Inverse();

			m_cacheMatrix = m_viewMatrix * m_projMatrix;
		}

		return m_cacheMatrix;
	}

	void SetClearColor( uint32 clearColor ) {m_clearColor = clearColor;}
	uint32 GetClearColor() {return m_clearColor;}
	void SetSkyBox( NlSkyBox* skyBox ) { m_skyBox = skyBox; }
	NlSkyBox* GetSkyBox() { return m_skyBox; }

public:
	static float modCamMinDist;
	static float modCamMaxDist;

private:
	float3 m_position;
	Quat m_rotation;

	float44 m_viewMatrix;
	float44 m_projMatrix;
	float44 m_cacheMatrix;

	bool m_cacheDirty;

	float m_fov, m_aspect, m_zn, m_zf;

	uint32 m_clearColor;
	NlSkyBox* m_skyBox;

};

void UpdateCamControl_FreeCam( HWND hwnd, CCamera& cam, float sensitive = 0.005, float camSpeed = 0.05 );
void UpdateCamControl_ModelCam(  HWND hwnd, CCamera& cam, float3 focuspoint = float3(0,1.5,0), float sensitive = 0.005, float camSpeed = 0.05 );

#endif