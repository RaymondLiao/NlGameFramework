#include "stableheader.h"
#include "NLRenderDevice.h"

//=================================================================================
NlRenderDevice::NlRenderDevice( void ):
	m_device(NULL), m_sdk_object(NULL), m_wndSolution(0, 0), m_surfBackBuffer(NULL)
{
}
//---------------------------------------------------------------------------------
NlRenderDevice::~NlRenderDevice( void )
{
}
//---------------------------------------------------------------------------------
bool NlRenderDevice::init( int width, int height, HWND hWnd, bool is_window )
{
	// clear up
	destroy();

	m_sdk_object = Direct3DCreate9( D3D_SDK_VERSION );

	D3DPRESENT_PARAMETERS para;
	memset( &para, 0, sizeof(para) );
	para.BackBufferWidth = width;
	para.BackBufferHeight = height;
	para.BackBufferCount = 1;
	para.BackBufferFormat = D3DFMT_X8R8G8B8;

	para.MultiSampleType = D3DMULTISAMPLE_NONE;
	para.MultiSampleQuality = 0;

	para.SwapEffect = D3DSWAPEFFECT_DISCARD;
	para.hDeviceWindow = hWnd;
	para.Windowed = is_window;

	para.EnableAutoDepthStencil = TRUE;
	para.AutoDepthStencilFormat = D3DFMT_D24S8;

	para.Flags = NULL;
	para.FullScreen_RefreshRateInHz = 0;
	para.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	m_sdk_object->CreateDevice( 0, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &para, &m_device );

	if (!m_device)
	{
		MessageBox( hWnd, "Render device can not create!", "Error", MB_OK);
		return false;
	}

	m_wndSolution = int2( width, height );
	m_hWnd = hWnd;

	// get backbuffer surface
	m_device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_surfBackBuffer );
	assert( m_surfBackBuffer );

	return true;

}
//---------------------------------------------------------------------------------
void NlRenderDevice::destroy()
{
	if (m_device)
		m_device->Release();
	if (m_sdk_object)
		m_sdk_object->Release();
}
//---------------------------------------------------------------------------------
bool NlRenderDevice::BeginFrame()
{
	return (D3D_OK == m_device->BeginScene());
}
//---------------------------------------------------------------------------------
void NlRenderDevice::EndFrame()
{
	m_device->EndScene();
}
//---------------------------------------------------------------------------------
void NlRenderDevice::Clear( uint32 color )
{
	m_device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0 );
}
void NlRenderDevice::Clear( uint32 flags, uint32 color, uint32 z, uint32 stencil )
{
	m_device->Clear( 0, NULL, flags, color, z, stencil );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::Present()
{
	m_device->Present( NULL,  NULL, NULL, NULL );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::SetRenderTarget( uint32 index, void* target )
{
	IDirect3DSurface9* surf = (IDirect3DSurface9*)target;
	if (surf == NULL)
		return;

	m_device->SetRenderTarget( index, surf );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::RenderToBackSurf()
{
	m_device->SetRenderTarget( 0, m_surfBackBuffer );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::SetRenderState( uint32 rs, uint32 value )
{
	m_device->SetRenderState( (D3DRENDERSTATETYPE)rs, value );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::DrawPrimitive( uint32 pttype, uint32 startVertex, uint32 primCount )
{
	m_device->DrawPrimitive( (D3DPRIMITIVETYPE)pttype, startVertex, primCount );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::DrawIndexedPrimitive( uint32 pttype, uint32 baseVertexIndex, uint32 minVertexIndex, 
	uint32 numVertices, uint32 startIndex, uint32 primCount )
{
	m_device->DrawIndexedPrimitive( (D3DPRIMITIVETYPE)pttype, baseVertexIndex, minVertexIndex, numVertices, startIndex, primCount );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::SetGlobalVSFloat( uint32 slot, const float& value )
{
	m_device->SetVertexShaderConstantF( slot, &value, 1 );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::SetGlobalVSFloat4( uint32 slot, const float4& value )
{
	m_device->SetVertexShaderConstantF( slot, &(value.x), 1 );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::SetGlobalVSMatrix( uint32 slot, const float44& value )
{
	m_device->SetVertexShaderConstantF( slot, value.m, 4 );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::SetGlobalPSFloat( uint32 slot, const float& value )
{
	m_device->SetPixelShaderConstantF( slot, &value, 1 );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::SetGlobalPSFloat4( uint32 slot, const float4& value )
{
	m_device->SetPixelShaderConstantF( slot, &(value.x), 1 );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::SetGlobalPSMatrix( uint32 slot, const float44& value )
{
	m_device->SetPixelShaderConstantF( slot, value.m, 4 );
}
//---------------------------------------------------------------------------------
void NlRenderDevice::SetTexFilter( uint32 slot, ENlTexFilterMode mode )
{
	if (m_currTFilMod[slot] != mode)
	{
		m_currTFilMod[slot] = mode;	// update filter mode
		switch( m_currTFilMod[slot] )
		{
		case eTEXFT_Point:
			m_device->SetSamplerState( slot, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			m_device->SetSamplerState( slot, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			m_device->SetSamplerState( slot, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
			break;

		case eTEXFT_Linear:
			m_device->SetSamplerState( slot, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_device->SetSamplerState( slot, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_device->SetSamplerState( slot, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
			break;
		}
	}
}
//---------------------------------------------------------------------------------
void NlRenderDevice::SetTexAddress( uint32 slot, ENlTexAddressMode mode )
{
	if (m_currTAddrMod[slot] != mode)
	{
		m_currTAddrMod[slot] = mode;
		switch( m_currTAddrMod[slot] )
		{
		case eTEXAD_Wrap:
			m_device->SetSamplerState( slot, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
			m_device->SetSamplerState( slot, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
			break;
			
		case eTEXAD_Clamp:
			m_device->SetSamplerState( slot, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			m_device->SetSamplerState( slot, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
			break;

		case eTEXAD_BoarderColor:
			m_device->SetSamplerState( slot, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER );
			m_device->SetSamplerState( slot, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER );
			m_device->SetSamplerState( slot, D3DSAMP_BORDERCOLOR, m_curTBodColor[slot] );
			break;
		}
	}
}