#include "CameraObject.h"

//--------------------------------------------------------------------------------------------------
void UpdateCamControl_FreeCam( HWND hwnd, CCamera& cam, float sensitive /*= 0.005*/, float camSpeed /*= 0.05 */ )
{
	Quat rotation = cam.GetRotation();
	float3 camPos = cam.GetPosition();

	POINT pt;
	::GetCursorPos(&pt);

	static float2 prev_pos;

	if (isKeyDown(VK_LBUTTON) && ::GetFocus() == hwnd)
	{
		float2 curr_pos = float2(pt.x, pt.y);
		float2 delta = curr_pos - prev_pos;
		prev_pos = curr_pos;

		Quat rot_x = Quat::CreateRotationX( delta.y * sensitive );
		Quat rot_y = Quat::CreateRotationY( delta.x * sensitive );

		rotation = rotation * rot_x;
		rotation = rot_y * rotation;
	}
	else
	{
		prev_pos = float2(pt.x, pt.y);
	}

	float3 frontDir = rotation.GetColumn2();
	float3 rightDir = rotation.GetColumn0();

	//////////////////////////////////////////////////////////////////////////
	// 处理相机移动
	if (::GetFocus() == hwnd)
	{
		if( isKeyDown('A') )
		{
			camPos -= rightDir * camSpeed;
		}
		if( isKeyDown('D') )
		{
			camPos += rightDir * camSpeed;
		}
		if( isKeyDown('W') )
		{
			camPos += frontDir * camSpeed;
		}
		if( isKeyDown('S') )
		{
			camPos -= frontDir * camSpeed;
		}
	}



	cam.SetPosition( camPos );
	cam.SetRotation( rotation );
}

//--------------------------------------------------------------------------------------------------
float CCamera::modCamMinDist = 500.0f;
float CCamera::modCamMaxDist = 750.0f;
void UpdateCamControl_ModelCam( HWND hwnd, CCamera& cam, float3 focuspoint /*= float3(0,1.5,0)*/, float sensitive /*= 0.005*/, float camSpeed /*= 0.05 */ )
{
	Quat rotation = cam.GetRotation();
	float3 camPos = cam.GetPosition();

	POINT pt;
	::GetCursorPos(&pt);

	static float2 prev_pos;

	if (::GetFocus() == hwnd && isKeyDown(VK_SHIFT))
	{
		float2 curr_pos = float2(pt.x, pt.y);
		float2 delta = curr_pos - prev_pos;
		prev_pos = curr_pos;

		Quat rot_x = Quat::CreateRotationX( delta.y * sensitive );
		Quat rot_y = Quat::CreateRotationY( delta.x * sensitive );

		rotation = rotation * rot_x;
		rotation = rot_y * rotation;
	}
	else
	{
		prev_pos = float2(pt.x, pt.y);
	}

	float3 frontDir = rotation.GetColumn2();
	float3 rightDir = rotation.GetColumn0();

	//static float cam_dist = 650.0f;
	static float cam_dist = (CCamera::modCamMinDist + CCamera::modCamMaxDist) * 0.5f;
	if (::GetFocus() == hwnd)
	{
		if( isKeyDown('W') )
		{
			cam_dist -= camSpeed;
		}
		if( isKeyDown('S') )
		{
			cam_dist += camSpeed;
		}
	}

	//cam_dist = max(cam_dist, 500.0f);
	//cam_dist = min(cam_dist, 700.0f);
	cam_dist = max(cam_dist, CCamera::modCamMinDist);
	cam_dist = min(cam_dist, CCamera::modCamMaxDist);

	camPos = focuspoint - frontDir * cam_dist;


	cam.SetPosition( camPos );
	cam.SetRotation( rotation );
}