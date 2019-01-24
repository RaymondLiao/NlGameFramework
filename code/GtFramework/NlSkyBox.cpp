#include "NlSkyBox.h"
#include "NlGameEnvironment.h"
#include "nl_render_header.h"

//=================================================================================
NlSkyBox::NlSkyBox():
	NlRenderableRaw( false )
{
	m_envTexture = NULL;
}
//---------------------------------------------------------------------------------
NlSkyBox::~NlSkyBox()
{
}
//---------------------------------------------------------------------------------
bool NlSkyBox::init( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );
	NlRenderableRaw::init( gameEnv );

	NlRenderDevice *renderDevice = gameEnv->GetRenderDevice();
	IDirect3DDevice9 *d3dDevice = renderDevice->GetDevice();
	assert( d3dDevice );

	// create vb-----------------------------------
	float3 vertices[8];
	vertices[0] = float3( -0.5, -0.5, -0.5 );
	vertices[1] = float3(  0.5, -0.5, -0.5 );
	vertices[2] = float3( -0.5,	-0.5,  0.5 );
	vertices[3] = float3(  0.5,	-0.5,  0.5 );

	vertices[4] = float3( -0.5,	 0.5, -0.5 );
	vertices[5] = float3(  0.5,	 0.5, -0.5 );
	vertices[6] = float3( -0.5,  0.5,  0.5 );
	vertices[7] = float3(  0.5,	 0.5,  0.5 );

	NlVertexPNT* vData = new NlVertexPNT[36];
	assert( vData );
	// bottom
	vData[0] =	NlVertexPNT( vertices[0], float3( 0, -1,  0), float2(0, 0) );
	vData[1] =	NlVertexPNT( vertices[1], float3( 0, -1,  0), float2(1, 0) );
	vData[2] =	NlVertexPNT( vertices[2], float3( 0, -1,  0), float2(0, 1) );

	vData[3] =	NlVertexPNT( vertices[1], float3( 0, -1,  0), float2(1, 0) );
	vData[4] =	NlVertexPNT( vertices[3], float3( 0, -1,  0), float2(1, 1) );
	vData[5] =	NlVertexPNT( vertices[2], float3( 0, -1,  0), float2(0, 1) );
	// top
	vData[6] =	NlVertexPNT( vertices[4], float3( 0,  1,  0), float2(0, 1) );
	vData[7] =	NlVertexPNT( vertices[6], float3( 0,  1,  0), float2(0, 0) );
	vData[8] =	NlVertexPNT( vertices[5], float3( 0,  1,  0), float2(1, 1) );

	vData[9] =	NlVertexPNT( vertices[5], float3( 0,  1,  0), float2(1, 1) );
	vData[10] = NlVertexPNT( vertices[6], float3( 0,  1,  0), float2(0, 0) );
	vData[11] = NlVertexPNT( vertices[7], float3( 0,  1,  0), float2(1, 0) );
	// left
	vData[12] =	NlVertexPNT( vertices[0], float3(-1,  0,  0), float2(1, 1) );
	vData[13] =	NlVertexPNT( vertices[2], float3(-1,  0,  0), float2(0, 1) );
	vData[14] =	NlVertexPNT( vertices[4], float3(-1,  0,  0), float2(1, 0) );

	vData[15] =	NlVertexPNT( vertices[2], float3(-1,  0,  0), float2(0, 1) );
	vData[16] = NlVertexPNT( vertices[6], float3(-1,  0,  0), float2(0, 0) );
	vData[17] = NlVertexPNT( vertices[4], float3(-1,  0,  0), float2(1, 0) );
	// right
	vData[18] =	NlVertexPNT( vertices[1], float3( 1,  0,  0), float2(0, 1) );
	vData[19] =	NlVertexPNT( vertices[5], float3( 1,  0,  0), float2(0, 0) );
	vData[20] =	NlVertexPNT( vertices[3], float3( 1,  0,  0), float2(1, 1) );

	vData[21] =	NlVertexPNT( vertices[3], float3( 1,  0,  0), float2(1, 1) );
	vData[22] = NlVertexPNT( vertices[5], float3( 1,  0,  0), float2(0, 0) );
	vData[23] = NlVertexPNT( vertices[7], float3( 1,  0,  0), float2(1, 0) );
	// back
	vData[24] =	NlVertexPNT( vertices[2], float3( 0,  1,  0), float2(1, 1) );
	vData[25] =	NlVertexPNT( vertices[3], float3( 0,  1,  0), float2(0, 1) );
	vData[26] =	NlVertexPNT( vertices[6], float3( 0,  1,  0), float2(1, 0) );

	vData[27] =	NlVertexPNT( vertices[3], float3( 0,  1,  0), float2(0, 1) );
	vData[28] = NlVertexPNT( vertices[7], float3( 0,  1,  0), float2(0, 0) );
	vData[29] = NlVertexPNT( vertices[6], float3( 0,  1,  0), float2(1, 0) );
	// front
	vData[30] =	NlVertexPNT( vertices[0], float3( 0,  1,  0), float2(0, 1) );
	vData[31] =	NlVertexPNT( vertices[4], float3( 0,  1,  0), float2(0, 0) );
	vData[32] =	NlVertexPNT( vertices[1], float3( 0,  1,  0), float2(1, 1) );

	vData[33] =	NlVertexPNT( vertices[1], float3( 0, -1,  0), float2(1, 1) );
	vData[34] = NlVertexPNT( vertices[4], float3( 0, -1,  0), float2(0, 0) );
	vData[35] = NlVertexPNT( vertices[5], float3( 0, -1,  0), float2(1, 0) );

	m_vb->init( sizeof(NlVertexPNT), 36 );
	m_vb->update( vData, 36*sizeof(NlVertexPNT) );
	delete[] vData;

	// load shader-----------------------------------
	m_shader = new NlDynamicShaderComb( renderDevice );
	assert( m_shader );
	m_shader->CompileFromFile( "SkyBoxShader.txt" );

	// create environment texture--------------------
	std::string tfilePath = getMediaPath();
	m_envTexture = new NlTextureCube( renderDevice );
	assert( m_envTexture );
	m_envTexture->LoadFromFile( "env_tex.dds" );

	return true;
}
//---------------------------------------------------------------------------------
void NlSkyBox::Draw( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );

	IDirect3DDevice9*d3dDevice = gameEnv->GetRenderDevice()->GetDevice();
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
	const float44& world_mat = 
		(m_parent == NULL) ? GetModelMatrix() : GetModelMatrix()*m_parent->GetModelMatrix();
	const float44& vp_mat = camera->GetViewProjectionMatrix();
	float44 wvp_mat= world_mat * vp_mat;

	// set shader constant
	m_shader->SetMatrix_vs( 0, wvp_mat );
	m_shader->SetMatrix_vs( 4, world_mat );

	m_shader->SetFloat4_ps( 0, float4(camPos, 1) );

	// set texture
	m_envTexture->SetAddressMode( 0, D3DTADDRESS_CLAMP );
	m_envTexture->SetFilter( 0, false );
	m_envTexture->Use( 0 );

	// draw skyBox
	m_vb->Use();
	d3dDevice->SetFVF( NlFVF_PNT );
	m_shader->Use();
	d3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 12 );
}
//---------------------------------------------------------------------------------
void NlSkyBox::destroy()
{
	NlRenderableRaw::destroy();

	if (m_envTexture)
	{
		m_envTexture->destroy();
		delete m_envTexture;
		m_envTexture = NULL;
	}
}
//---------------------------------------------------------------------------------
NlTextureCube* NlSkyBox::GetEnvTexture()
{
	return m_envTexture;
}