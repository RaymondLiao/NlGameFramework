/*
  @file   NlRenderDevice.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlRenderDevice_h_
#define _NlRenderDevice_h_

#include "prerequisite.h"
//====================================================
enum ENlTexFilterMode
{
	eTEXFT_Point,
	eTEXFT_Linear
};

enum ENlTexAddressMode
{
	eTEXAD_Wrap,
	eTEXAD_Clamp,
	eTEXAD_BoarderColor
};

//====================================================
class NlRenderDevice
{
public:
	NlRenderDevice(void);
	~NlRenderDevice(void);

	bool init( int width, int height, HWND hWnd, bool is_window );
	void destroy();

	bool BeginFrame();
	void EndFrame();

	void Clear( uint32 color );
	void Clear( uint32 flags, uint32 color, uint32 z, uint32 stencil );

	void Present();

	IDirect3DDevice9 *GetDevice() {return m_device;}
	HWND GetWindowHandle() {return m_hWnd;}
	int2 GetWndSolution() {return m_wndSolution;}

	void SetRenderTarget( uint32 index, void* target );
	void RenderToBackSurf();
	void SetRenderState( uint32 rs, uint32 value );

	void DrawPrimitive( uint32 pttype, uint32 startVertex, uint32 primCount );
	void DrawIndexedPrimitive( uint32 pttype, uint32 baseVertexIndex, uint32 minVertexIndex, 
		uint32 numVertices, uint32 startIndex, uint32 primCount );

	// set global shader-constants---------------------------------------
	void SetGlobalVSFloat( uint32 slot, const float& value );
	void SetGlobalVSFloat4( uint32 slot, const float4& value );
	void SetGlobalVSMatrix( uint32 slot, const float44& value );
	
	void SetGlobalPSFloat( uint32 slot, const float& value );
	void SetGlobalPSFloat4( uint32 slot, const float4& value );
	void SetGlobalPSMatrix( uint32 slot, const float44& value );

	// set filter attribute-----------------------------------------------
	void SetTexFilter( uint32 slot, ENlTexFilterMode mode );
	void SetTexAddress( uint32 slot, ENlTexAddressMode mode );
	void SetTexBorderColor( uint32 slot, uint32 color );

private:
	struct IDirect3DDevice9* m_device;
	struct IDirect3D9* m_sdk_object;

	HWND m_hWnd;
	int2 m_wndSolution;

	ENlTexFilterMode m_currTFilMod[32];		// filter mode
	ENlTexAddressMode m_currTAddrMod[32];	// address mode
	uint32 m_curTBodColor[32];				// border color

	IDirect3DSurface9* m_surfBackBuffer;
};	

#endif