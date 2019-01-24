#include "NlGUITexture.h"
#include "nl_render_header.h"

//=================================================================================
NlGUITexture::NlGUITexture( NlRenderDevice* device )
{
	assert( device );
	m_device = device;

	m_vd = NULL;
	m_shaders = NULL;
	m_texture = NULL;

	m_hasFrameAni = false;

}
//---------------------------------------------------------------------------------
NlGUITexture::~NlGUITexture( void )
{
}
//---------------------------------------------------------------------------------
void NlGUITexture::destroy()
{
	if (m_vd)
	{
		m_vd->destroy();
		delete m_vd;
		m_vd = NULL;
	}

	if (m_shaders)
	{
		m_shaders->destroy();
		delete m_shaders;
		m_shaders = NULL;
	}

}
//---------------------------------------------------------------------------------
bool NlGUITexture::init( const char* texFileName )
{
	m_texture = NlTextureManager::GetInstance().Get2DTextureFromFile( m_device, texFileName );
	if (m_texture == NULL)
		return false;

	m_vd = new NlVertexDecl( m_device );
	assert( m_vd );
	VertexElemArray elements;
	{
		NlVertexElem elem( 0, eVEU_Position, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 0, eVEU_Texcoord, 0 );
		elements.push_back( elem );
	}
	m_vd->init( elements );

	m_shaders = new NlDynamicShaderComb( m_device );
	assert( m_shaders );
	assert( m_shaders->CompileFromFile("ImmUI.shader") == true );

	return true;

}
//---------------------------------------------------------------------------------
void NlGUITexture::Draw( float2 pos, float2 size )
{
	if (m_texture == NULL || m_vd == NULL || m_shaders == NULL)
		return;

	pos.y = 1.0 - pos.y;
	float buf[] = {
		-1+pos.x*2-size.x,	1-pos.y*2+size.y,	0.0001,	0,0,
		-1+pos.x*2+size.x,	1-pos.y*2+size.y,	0.0001,	1,0,
		-1+pos.x*2-size.x,	1-pos.y*2-size.y,	0.0001,	0,1,
		-1+pos.x*2+size.x,	1-pos.y*2-size.y,	0.0001,	1,1 };

	m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	m_vd->Use();
	m_shaders->Use();
	m_texture->Use( 0 );
	m_device->SetTexAddress( 0, eTEXAD_Wrap );
	m_device->SetTexFilter( 0, eTEXFT_Linear );

	m_device->GetDevice()->
		DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, buf, sizeof(float)*5 );

	m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

}
//---------------------------------------------------------------------------------
void NlGUITexture::Draw( const char* texFileName, float2 pos, float2 size )
{
	if (m_texture == NULL || m_vd == NULL || m_shaders == NULL)
		return;

	pos.y = 1.0 - pos.y;
	float buf[] = {
		-1+pos.x*2-size.x,	1-pos.y*2+size.y,	0.0001,	0,0,
		-1+pos.x*2+size.x,	1-pos.y*2+size.y,	0.0001,	1,0,
		-1+pos.x*2-size.x,	1-pos.y*2-size.y,	0.0001,	0,1,
		-1+pos.x*2+size.x,	1-pos.y*2-size.y,	0.0001,	1,1 };

	m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	m_vd->Use();
	m_shaders->Use();
	
	m_device->SetTexAddress( 0, eTEXAD_Wrap );
	m_device->SetTexFilter( 0, eTEXFT_Linear );
	NlTexture2D* another = NlTextureManager::GetInstance().Get2DTextureFromFile( m_device, texFileName );
	(another != NULL) ? another->Use( 0 ) : m_texture->Use( 0 );

	m_device->GetDevice()->
		DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, buf, sizeof(float)*5 );

	m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

}
//---------------------------------------------------------------------------------
bool NlGUITexture::initFrameAni( std::vector<std::string> texFileNames, float intervalTime )
{

	std::vector<std::string>::iterator it;
	for (it = texFileNames.begin(); it != texFileNames.end(); ++it)
	{
		NlTexture2D* frame = NlTextureManager::GetInstance().Get2DTextureFromFile( m_device, (*it).c_str() );
		if (frame == NULL)
		{
			m_frames.clear();
			return false;
		}

		m_frames.push_back( frame );
	}

	m_vd = new NlVertexDecl( m_device );
	assert( m_vd );
	VertexElemArray elements;
	{
		NlVertexElem elem( 0, eVEU_Position, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 0, eVEU_Texcoord, 0 );
		elements.push_back( elem );
	}
	m_vd->init( elements );

	m_shaders = new NlDynamicShaderComb( m_device );
	assert( m_shaders );
	assert( m_shaders->CompileFromFile("ImmUI.shader") == true );

	m_hasFrameAni = true;
	m_curFrame = 0;
	m_intervalTime = intervalTime;
	m_acumuTime = 0;
	return true;

}
//---------------------------------------------------------------------------------
void NlGUITexture::PlayFrameAni( float elapsedTime, float2 pos, float2 size, bool loop /*= false */ )
{
	if (m_hasFrameAni == false)
		return;

	m_acumuTime += elapsedTime;
	if (m_acumuTime > m_intervalTime)
	{
		m_acumuTime = 0;

		(m_curFrame+1 > m_frames.size()-1) ? 
			((loop == false) ? 1 : m_curFrame=0) : m_curFrame++;
	}

	NlTexture2D* curFrame = m_frames[m_curFrame];

	if (curFrame == NULL)
		return;
	assert( m_vd && m_shaders );

	pos.y = 1.0 - pos.y;
	float buf[] = {
		-1+pos.x*2-size.x,	1-pos.y*2+size.y,	0.0001,	0,0,
		-1+pos.x*2+size.x,	1-pos.y*2+size.y,	0.0001,	1,0,
		-1+pos.x*2-size.x,	1-pos.y*2-size.y,	0.0001,	0,1,
		-1+pos.x*2+size.x,	1-pos.y*2-size.y,	0.0001,	1,1 };

	m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	m_vd->Use();
	m_shaders->Use();

	m_device->SetTexAddress( 0, eTEXAD_Wrap );
	m_device->SetTexFilter( 0, eTEXFT_Linear );
	curFrame->Use( 0 );

	m_device->GetDevice()->
		DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, buf, sizeof(float)*5 );

	m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

}