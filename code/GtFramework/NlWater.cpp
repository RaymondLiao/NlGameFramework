#include "NlWater.h"
#include "NlSkyBox.h"
#include "NlGameEnvironment.h"
#include "nl_render_header.h"

//=================================================================================
typedef struct _WaterMeshVex
{
	float3 position;
	float3 normal;
	float3 tangent;
	float  waveHeiScale;

	_WaterMeshVex() {}
	_WaterMeshVex( float3 p, float3 n, float3 tan, float hScale ):
	position(p), normal(n), tangent(tan), waveHeiScale(hScale)
	{
	}

} WaterMeshVex;

//=================================================================================
NlWater::NlWater( int fieldW, int fieldL ): 
	NlRenderableRaw( false ),
	m_fieldWid(fieldW), m_fieldLen(fieldL)
{
	m_tcVb = NULL;

	m_bumpTex = NULL;
	m_reflTex = NULL;
	m_refrTex = NULL;

	m_waveHei = 15.0f;
	m_waveLen = 0.5f;
	m_waveSpeed = 0.3f;
	m_waveDir = float2( 1.0f, 0.0f );
}
//---------------------------------------------------------------------------------
NlWater::~NlWater()
{
}
//---------------------------------------------------------------------------------
bool NlWater::init( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );
	NlRenderableRaw::init( gameEnv );

	HWND hWnd = gameEnv->GetwindowHandle();
	NlRenderDevice *renderDevice = gameEnv->GetRenderDevice();
	IDirect3DDevice9 *d3dDevice = renderDevice->GetDevice();
	assert( hWnd && d3dDevice );

	// create mesh------------------------------------------------------

	// create vb
	m_tcVb = new NlVertexBuffer( renderDevice );
	assert( m_tcVb );
	m_vb->init( sizeof(WaterMeshVex), m_fieldWid*m_fieldLen );
	m_tcVb->init( sizeof(float2), m_fieldWid*m_fieldLen );

	WaterMeshVex *vData = new WaterMeshVex[m_fieldWid*m_fieldLen];
	float2 *tcData = new float2[m_fieldWid*m_fieldLen];
	assert( vData && tcData );

	for (int y = 0; y < m_fieldLen; ++y)
	{
		for (int x = 0; x < m_fieldWid; ++x)
		{
			float3 pos = float3( x-m_fieldWid*0.5f, 0, y-m_fieldLen*0.5f );
			float3 normal = float3( 0, 1, 0 );
			float2 texCoord = float2( x/(float)m_fieldWid, y/(float)m_fieldLen );
			float3 tangent = float3( 0, 0, 1 );
			float  waveHeiScale = 0;

			vData[y*m_fieldWid+x] = WaterMeshVex( pos, normal, tangent, waveHeiScale );
			tcData[y*m_fieldWid+x] = texCoord;
		}
	}

	m_vb->update( vData, m_fieldWid*m_fieldLen*sizeof(WaterMeshVex) );
	m_tcVb->update( tcData, m_fieldWid*m_fieldLen*sizeof(float2) );

	delete[] vData;
	delete[] tcData;

	// create ib
	m_ib->init( sizeof(uint32), (m_fieldWid-1)*(m_fieldLen-1)*2*3 );

	uint32 *iData = new uint32[(m_fieldWid-1)*(m_fieldLen-1)*3*2];
	assert( iData );

	int iCount = 0;
	int rowCount = 0;
	for (int y = 0; y < (m_fieldLen-1); ++y)
	{
		for (int x = 0; x < (m_fieldWid-1); ++x)
		{
			iData[iCount++] = rowCount;
			iData[iCount++] = rowCount + 1;
			iData[iCount++] = rowCount + m_fieldLen;

			iData[iCount++] = rowCount + m_fieldLen;
			iData[iCount++] = rowCount + 1;
			iData[iCount++] = rowCount + m_fieldLen + 1;

			++rowCount;
		}

		++rowCount;
	}

	m_ib->update( iData, (m_fieldWid-1)*(m_fieldLen-1)*2*3*sizeof(uint32) );

	delete[] iData;

	// create vd
	VertexElemArray elements;
	{
		NlVertexElem elem( 0, eVEU_Position, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 0, eVEU_Normal, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 0, eVEU_Tangent, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 0, eVEU_Color, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 1, eVEU_Texcoord, 0);
		elements.push_back( elem );
	}
	
	m_vd->init( elements );

	// load shader-----------------------------------
	m_shader = new NlDynamicShaderComb( renderDevice );
	assert( m_shader );
	m_shader->CompileFromFile( "WaterEffectShader.txt" );

	// create textures---------------------------
	//m_bumpTex = new NlTexture2D( renderDevice );
	//assert( m_bumpTex );
	//m_bumpTex->LoadFromFile( "wavebump_tex.dds" );
	m_bumpTex = NlTextureManager::GetInstance().Get2DTextureFromFile( renderDevice, "wavebump_tex.dds" );
	
	int2 winResolution = gameEnv->GetWndResolution();

	m_reflTex = new NlTexture2D( renderDevice );
	m_refrTex = new NlTexture2D( renderDevice );
	assert( m_reflTex && m_refrTex );

	m_reflTex->init( winResolution.x, winResolution.y, true );
	m_refrTex->init( winResolution.x, winResolution.y, true );

	return true;
}
//---------------------------------------------------------------------------------
void NlWater::Draw( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );

	IDirect3DDevice9 *d3dDevice = gameEnv->GetRenderDevice()->GetDevice();
	assert( d3dDevice );

	CCamera *camera = gameEnv->GetMainCamera();
	bool updateShader = gameEnv->IfUpdateShader();

	if (camera == NULL || m_shader == NULL)
		return;

	if (updateShader)
		m_shader->update();

	d3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	// disable alpha-blending
	d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

	// draw------------------------------------------------------
	float time = gameEnv->GetTime();

	float3 camPos = camera->GetPosition();
	const float44& world_mat = 
		(m_parent == NULL) ? GetModelMatrix() : GetModelMatrix()*m_parent->GetModelMatrix();
	const float44& v_mat = camera->GetViewMatrix();
	const float44& vp_mat = camera->GetViewProjectionMatrix();
	float44 wvp_mat= world_mat * vp_mat;
	int2 wndResolution = gameEnv->GetWndResolution();

	// set shader constant
	m_shader->SetFloat4_vs( 0, float4(m_waveHei, 0, 0, 0) );					// wave height
	m_shader->SetFloat4_vs( 1, float4(m_waveLen, 0, 0, 0) );					// wave length
	m_shader->SetFloat4_vs( 2, float4(m_waveSpeed, 0, 0, 0) );					// wave speed
	m_shader->SetFloat4_vs( 3, float4(m_waveDir.x, m_waveDir.y, 0, 0) );		// wave forward direction
	m_shader->SetFloat4_vs( 4, float4(m_waveoffset.x, m_waveoffset.y, 0, 0) );
	m_shader->SetFloat4_vs( 5, float4(time, 0, 0, 0) );							// time
	m_shader->SetFloat4_vs( 7, float4(camPos, 0) );
	m_shader->SetMatrix_vs( 11, world_mat );
	m_shader->SetMatrix_vs( 15, vp_mat );
	m_shader->SetFloat4_vs( 6,													// distortion
		float4(m_waveDir.x*m_waveSpeed*0.1f, m_waveDir.y*m_waveSpeed*0.1f, 
		m_waveDir.x*m_waveSpeed*0.1f, m_waveDir.y*m_waveSpeed*0.1f) );

	m_shader->SetFloat4_ps( 0, float4(0.0f, 0.6f, 0.9f, 1.0f) );				// water's diffuse color
	m_shader->SetFloat4_ps( 1, float4(1.0f, 1.0f, 1.0f, 1.0f) );				// water's specular color
	m_shader->SetFloat4_ps( 2, float4(wndResolution.x, wndResolution.y, 0, 0) );
	m_shader->SetMatrix_ps( 3, world_mat );
	m_shader->SetMatrix_ps( 7, v_mat );

	// set texture
	m_bumpTex->SetAddressMode( 0, D3DTADDRESS_WRAP );
	m_bumpTex->SetAddressMode( 1, D3DTADDRESS_WRAP );
	m_reflTex->SetAddressMode( 3, D3DTADDRESS_WRAP );
	m_refrTex->SetAddressMode( 4, D3DTADDRESS_WRAP );

	m_bumpTex->SetFilter( 0, true );
	m_bumpTex->SetFilter( 1, true );
	m_reflTex->SetFilter( 3, true );
	m_refrTex->SetFilter( 4, true );

	m_bumpTex->Use( 0 );
	m_bumpTex->Use( 1 );
	m_reflTex->Use( 3 );
	m_refrTex->Use( 4 );

	NlSkyBox *skyBox = camera->GetSkyBox();
	if (skyBox)
	{
		NlTextureCube* envTexture= skyBox->GetEnvTexture();
		envTexture->SetAddressMode( 2, D3DTADDRESS_WRAP );
		envTexture->SetFilter( 2, true );
		envTexture->Use( 2 ); 
	}
	
	// draw water
	m_vb->Use( 0 );
	m_tcVb->Use( 1 );
	m_ib->Use();
	m_vd->Use();
	m_shader->Use();
	d3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_fieldWid*m_fieldLen, 0, (m_fieldWid-1)*(m_fieldLen-1)*2 );
	d3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

}
//---------------------------------------------------------------------------------
void NlWater::destroy()
{
	NlRenderableRaw::destroy();

	if (m_tcVb)
	{
		m_tcVb->destroy();
		delete m_tcVb;
		m_tcVb = NULL;
	}
	/*if (m_bumpTex)
	{
		m_bumpTex->destroy();
		delete m_bumpTex;
		m_bumpTex = NULL;
	}
	if (m_reflTex)
	{
		m_reflTex->destroy();
		delete m_reflTex;
		m_reflTex = NULL;
	}
	if (m_refrTex)
	{
		m_refrTex->destroy();
		delete m_refrTex;
		m_refrTex = NULL;
	}*/
	NlTextureManager::GetInstance().Destroy2DTexture( m_reflTex );
	NlTextureManager::GetInstance().Destroy2DTexture( m_refrTex );

}
//---------------------------------------------------------------------------------
void NlWater::SetTextureResolution( int wid, int len )
{
	float2 *tcData = new float2[m_fieldWid*m_fieldLen];
	assert( tcData );

	for (int y = 0; y < m_fieldLen; ++y)
	{
		for (int x = 0; x < m_fieldWid; ++x)
		{
			tcData[y*m_fieldWid+x] = float2( x/(float)wid, y/(float)len );
		}
	}

	m_tcVb->update( tcData, m_fieldWid*m_fieldLen*sizeof(float2) );

}