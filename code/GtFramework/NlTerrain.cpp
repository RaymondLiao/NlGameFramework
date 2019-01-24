#include "NlTerrain.h"
#include "NlGameEnvironment.h"
#include "GtFile.h"
#include "nl_render_header.h"

#define ROUND(a) (int)(a+0.5f)

//=================================================================================
NlTerrain::NlTerrain( int fieldW, int fieldL, float maxHeight, NlTransform& transform,
	bool autoTexture /*= false */, bool isStatic /*= true*/ ):
	NlRenderableRaw( transform, isStatic ),
	m_fieldWid(fieldW), m_fieldLen(fieldL), m_maxHeight(maxHeight), m_autoTexture(autoTexture)
{
	m_locallWid = fieldW;
	m_localLen = fieldL;

	m_vPos = NULL;
	m_vTcd = NULL;
	m_tcVb = NULL;

	m_heiMap = NULL;
	m_normMap = NULL;
	m_texture = NULL;
}
//---------------------------------------------------------------------------------
NlTerrain::~NlTerrain()
{
}
//---------------------------------------------------------------------------------
bool NlTerrain::init( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );
	NlRenderableRaw::init( gameEnv );

	HWND hWnd = gameEnv->GetwindowHandle();
	NlRenderDevice *renderDevice = gameEnv->GetRenderDevice();
	IDirect3DDevice9 *d3dDevice = renderDevice->GetDevice();
	assert( hWnd && d3dDevice );

	// create mesh------------------------------------------------------
	m_vPos = new float3[m_locallWid*m_localLen];
	m_vTcd = new float2[m_locallWid*m_localLen];
	assert( m_vPos );

	// create vb
	m_tcVb = new NlVertexBuffer( renderDevice );
	m_vb->init( sizeof(float3), m_locallWid*m_localLen );
	m_tcVb->init( sizeof(float2), m_locallWid*m_localLen );

	// create ib
	m_ib->init( sizeof(uint32), (m_locallWid-1)*(m_localLen-1)*2*3 );

	uint32 *iData = new uint32[(m_locallWid-1)*(m_localLen-1)*2*3];
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

	// create vd
	VertexElemArray elements; 
	{
		NlVertexElem elem( 0, eVEU_Position, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 1, eVEU_Texcoord, 0 );
		elements.push_back( elem );
	}
	m_vd->init( elements );

	// get the height information from height map
	m_heiMap = new uint8[m_fieldWid*m_fieldLen];
	assert( m_heiMap );
	GtFile heightMap( ASSETS_PATH+"terrain_heiMap.raw" );
	heightMap.Open();
	if (heightMap.IsOpen())
	{
		uint8 *data = (uint8*)heightMap.Data();
		
		for (int i = 0; i < m_fieldWid*m_fieldLen; ++i)
		{
			m_heiMap[i] = data[i] / 255.0f * m_maxHeight;
		}
		heightMap.Close();
	}

	// create normal texture
	float3 *posData  = new float3[m_fieldWid*m_fieldLen];
	uint8 *normXData = new uint8[m_fieldWid*m_fieldLen];
	uint8 *normYData = new uint8[m_fieldWid*m_fieldLen];
	uint8 *normZData = new uint8[m_fieldWid*m_fieldLen];
	assert( posData );
	assert( normXData );
	assert( normYData );
	assert( normZData );
	
	for (int y = 0; y < m_fieldLen; ++y)
	{
		for (int x = 0; x < m_fieldWid; ++x)
		{
			posData[y*m_fieldWid+x] = float3( x-m_fieldWid*0.5f, 
				m_heiMap[y*m_fieldWid+x],
				y-m_fieldLen*0.5 );

			normXData[y*m_fieldWid+x] = 0.0f;
			normYData[y*m_fieldWid+x] = 1.0f;
			normZData[y*m_fieldWid+x] = 0.0f;
		}
	}

	for (int i = m_fieldWid+1; i < m_fieldWid*m_fieldLen-m_fieldWid-1; ++i)
	{
		float3& left  = posData[i-1];
		float3& right = posData[i+1];
		float3& up    = posData[i-m_fieldWid];
		float3& down  = posData[i+m_fieldWid];

		float3 normal = (right - left) % (up - down);
		normal.normalize();

		normXData[i] = (1.0f+normal.x) * 0.5*255;
		normYData[i] = (1.0f+normal.y) * 0.5*255;
		normZData[i] = (1.0f+normal.z) * 0.5*255;
	}
	
	m_normMap = new NlTexture2D( renderDevice );
	assert( m_normMap );
	m_normMap->init( m_fieldWid, m_fieldLen, false );
	NlTexture2DInfo texInfo = m_normMap->GetInfo();
	uint32 *colorBuf = new uint32[texInfo.width*texInfo.pitch];
	assert( colorBuf );
	uint8 *address = (uint8*)colorBuf;

	for (int y = 0; y < texInfo.height; ++y)
	{
		uint32 *color = (uint32*)address;
		for (int x = 0; x < texInfo.width; ++x)
		{
			int i = y*m_fieldWid + x;
			color[x] = 0xff << 24 | normXData[i] << 16 | normYData[i] << 8 | normZData[i];
		}
		address += texInfo.pitch;
	}
	m_normMap->update( colorBuf );
	delete colorBuf;

	// clean up
	delete[] iData;
	delete[] posData;
	delete[] normXData;
	delete[] normYData;
	delete[] normZData;
	
	// initialize vb & tcVb
	for (int y = 0; y < m_localLen; ++y)
	{
		for (int x = 0; x < m_locallWid; ++x)
		{
			m_vPos[y*m_locallWid + x] = float3( x-m_locallWid*0.5f,
				m_heiMap[ROUND(y+m_fieldLen*0.5f-m_localLen*0.5f)*m_fieldWid + ROUND(x+m_fieldWid*0.5f-m_locallWid*0.5f)] / 255.0f * m_maxHeight,
				y-m_localLen*0.5f );

			m_vTcd[y*m_locallWid + x] = float2( (x+m_fieldWid*0.5f-m_locallWid*0.5f)/m_fieldWid, 
				(y+m_fieldLen*0.5f-m_localLen*0.5f)/m_fieldLen );
		}
	}

	m_vb->update( m_vPos, m_locallWid*m_localLen*sizeof(float3) );
	m_tcVb->update( m_vTcd, m_locallWid*m_localLen*sizeof(float2) );

	// load shader-----------------------------------
	std::string shaderFileName;
	m_shader = new NlDynamicShaderComb( renderDevice );
	assert( m_shader );
	if (m_autoTexture == true)
		shaderFileName = "terrainAutoTexShader.txt";
	else
		shaderFileName = "terrainShader.txt";
	m_shader->CompileFromFile( shaderFileName.c_str() );

	// create texture--------------------------------
	if (m_autoTexture == false)
	{
		//m_texture = new NlTexture2D( renderDevice );
		//m_texture->LoadFromFile( "terrain_tex.dds" );
		m_texture = NlTextureManager::GetInstance().Get2DTextureFromFile( renderDevice, "terrain_tex.dds" );
	}

	return true;
}
//---------------------------------------------------------------------------------
void NlTerrain::update( NlGameEnvironment* gameEnv )
{
	if (m_fieldWid!=m_locallWid || m_fieldLen!=m_localLen)
	{
		//
		// note: need player's relative coordinate
		//
		for (int y = 0; y < m_localLen; ++y)
		{
			for (int x = 0; x < m_locallWid; ++x)
			{
				m_vPos[y*m_locallWid + x] = float3( x-m_locallWid*0.5f,
					m_heiMap[ROUND(y+/*g_player._pos.z*/-m_localLen*0.5f)*m_fieldWid + ROUND(x+/*g_player._pos.x*/-m_locallWid*0.5f)] / 255.0f * m_maxHeight,
					y-m_localLen*0.5f );

				m_vTcd[y*m_locallWid + x] = float2( (x+/*g_player._pos.x*/-m_locallWid*0.5f)/m_fieldWid, 
					(y+/*g_player._pos.z*/-m_localLen*0.5f)/m_fieldLen );
			}
		}

		m_vb->update( m_vPos, m_locallWid*m_localLen*sizeof(float3) );
		m_tcVb->update( m_vTcd, m_locallWid*m_localLen*sizeof(float2) );
	}
	
}
//---------------------------------------------------------------------------------
void NlTerrain::Draw( NlGameEnvironment* gameEnv )
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

	d3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	// disable alpha-blending
	d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

	// draw------------------------------------------------------
	float3 camPos = camera->GetPosition();
	const float44& w_mat = GetModelMatrix();
	const float44& vp_mat = camera->GetViewProjectionMatrix();
	float44 wvp_mat= w_mat * vp_mat;
	float3 lightDir = gameEnv->m_lightDir;
	
	if (m_autoTexture == true)
	{
		// set shader constant
		m_shader->SetMatrix_vs( 0, wvp_mat );
		m_shader->SetMatrix_vs( 4, w_mat );
		m_shader->SetFloat4_vs( 8, float4(m_maxHeight, 0, 0, 0) );

		m_shader->SetMatrix_ps( 0, w_mat );
		m_shader->SetFloat4_ps( 4, float4(lightDir, 0) );
		m_shader->SetFloat4_ps( 5, float4(camPos, 0) );

		d3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		d3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
		d3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		d3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		d3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

		// set texture
		m_normMap->SetAddressMode( 0, D3DTADDRESS_CLAMP );
		m_normMap->SetFilter( 0, true );
		m_normMap->Use( 0 );
	}
	else
	{
		m_shader->SetMatrix_vs( 0, wvp_mat );
		m_shader->SetMatrix_vs( 4, w_mat );

		m_shader->SetMatrix_ps( 0, w_mat );
		m_shader->SetFloat4_ps( 4, float4(lightDir, 0) );
		m_shader->SetFloat4_ps( 5, float4(camPos, 0) );

		// set texture
		m_texture->SetAddressMode( 0, D3DTADDRESS_CLAMP );
		m_texture->SetFilter( 0, true );
		m_texture->Use( 0 );

		m_normMap->SetAddressMode( 1, D3DTADDRESS_CLAMP );
		m_normMap->SetFilter( 1, true );
		m_normMap->Use( 1 );

	}

	// draw terrain
	m_vb->Use( 0 );
	m_tcVb->Use( 1 );
	m_ib->Use();
	m_vd->Use();
	m_shader->Use();
	d3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_locallWid*m_localLen, 0, (m_locallWid-1)*(m_localLen-1)*2 );
}
//---------------------------------------------------------------------------------
void NlTerrain::destroy()
{
	NlRenderableRaw::destroy();

	if (m_vPos)
	{
		delete m_vPos;
		m_vPos = NULL;
	}
	if (m_vTcd)
	{
		delete m_vTcd;
		m_vTcd = NULL;
	}
	if (m_tcVb)
	{
		m_tcVb->destroy();
		delete m_tcVb;
		m_tcVb = NULL;
	}
	if (m_heiMap)
	{
		delete m_heiMap;
		m_heiMap = NULL;
	}
	if (m_normMap)
	{
		//m_normMap->destroy();
		//delete m_normMap;
		//m_normMap = NULL;
		NlTextureManager::GetInstance().Destroy2DTexture( m_normMap );
	}
	/*if (m_texture)
	{
		m_texture->destroy();
		delete m_texture;
		m_texture = NULL;
	}*/

}
//---------------------------------------------------------------------------------
void NlTerrain::SetLocalField( int fieldW, int fieldL )
{
	m_locallWid = fieldW;
	m_localLen = fieldL;
}