#include "NlImmGUI.h"
#include "NlGameEnvironment.h"
#include "nl_render_header.h"
#include "InputManager.h"

NlImmGUI* NlImmGUI::instance = NULL;

//=================================================================================
NlImmGUI::NlImmGUI( NlRenderDevice* device )
{
	m_device = device;
	m_shaders = NULL;

	m_btnFont = NULL;
	m_btnNorm = NULL;
	m_btnHover = NULL;
	m_btnDown = NULL;

}
//---------------------------------------------------------------------------------
NlImmGUI::~NlImmGUI( void )
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

	if (m_btnFont)
	{
		m_btnFont->Release();
		m_btnFont = NULL;
	}

	/*if (m_btnNorm)
	{
		m_btnNorm->destroy();
		delete m_btnNorm;
		m_btnNorm = NULL;
	}

	if (m_btnHover)
	{
		m_btnHover->destroy();
		delete m_btnHover;
		m_btnHover = NULL;
	}

	if (m_btnDown)
	{
		m_btnDown->destroy();
		delete m_btnDown;
		m_btnDown = NULL;
	}*/

}
//---------------------------------------------------------------------------------
void NlImmGUI::Create( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );

	if (instance != NULL)
		return;

	NlRenderDevice* device = gameEnv->GetRenderDevice();
	instance = new NlImmGUI( device );
	assert( instance );

	instance->m_vd = new NlVertexDecl( device );
	assert( instance->m_vd );
	VertexElemArray elements;
	{
		NlVertexElem elem( 0, eVEU_Position, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 0, eVEU_Texcoord, 0 );
		elements.push_back( elem );
	}
	instance->m_vd->init( elements );

	instance->m_shaders = new NlDynamicShaderComb( device );
	assert( instance->m_shaders );
	assert( instance->m_shaders->CompileFromFile("ImmUI.shader") == true );

	// button---------------------------------------
	//instance->m_btnNorm = new NlTexture2D( device );
	//assert( instance->m_btnNorm ); 
	//assert( instance->m_btnNorm->LoadFromFile("defaultBtnN.png") == true );
	//instance->m_btnHover = new NlTexture2D( device );
	//assert( instance->m_btnHover );
	//assert( instance->m_btnHover->LoadFromFile("defaultBtnH.png") == true );
	//instance->m_btnDown = new NlTexture2D( device );
	//assert( instance->m_btnDown );
	//assert( instance->m_btnDown->LoadFromFile("defaultBtnD.png") == true );

	instance->m_btnNorm = NlTextureManager::GetInstance().Get2DTextureFromFile( device, "defaultBtnN.png" );
	assert( instance->m_btnNorm );
	instance->m_btnHover = NlTextureManager::GetInstance().Get2DTextureFromFile( device, "defaultBtnH.png" );
	assert( instance->m_btnHover );
	instance->m_btnDown = NlTextureManager::GetInstance().Get2DTextureFromFile( device, "defaultBtnD.png" );
	assert( instance->m_btnDown );

	D3DXFONT_DESC ftDesc;
	ZeroMemory( &ftDesc, sizeof(D3DXFONT_DESC) );
	ftDesc.Width = 42 * 0.25f;
	ftDesc.Height = 42 * 0.6f;
	ftDesc.Weight = 750;
	ftDesc.CharSet = DEFAULT_CHARSET;
	strcpy( ftDesc.FaceName, "Arial" );

	assert( D3D_OK == D3DXCreateFontIndirect( device->GetDevice(), &ftDesc, &instance->m_btnFont) );

	// add event listener
	gameEnv->GetInputer()->AddListener( instance );

}
//---------------------------------------------------------------------------------
void NlImmGUI::Destroy()
{
	if (instance)
	{
		delete instance;
		instance = NULL;
	}

}
//---------------------------------------------------------------------------------
bool NlImmGUI::Button( float2 pos, float2 size, const char* str/* = ""*/ )
{
	assert( instance );
	bool res = false;
	NlTexture2D* btnTex = instance->m_btnNorm;
	uint32 ftColor = 0xc8ffffff;

	RECT field;
	field.left = pos.x - size.x*0.5f;
	field.top = pos.y - size.y*0.5f;
	field.right = pos.x + size.x*0.5f;
	field.bottom = pos.y + size.y*0.5f;

	POINT pt;
	::GetCursorPos( &pt );
	::ScreenToClient( instance->m_device->GetWindowHandle(), &pt );

	if (instance->m_lBtnState == eIS_Pressed)
	{
		if (::PtInRect( &field, pt ))
		{
			btnTex = instance->m_btnDown;
			ftColor = 0x80ffffff;
			res = true;
		}

	}
	else if(::PtInRect( &field, pt))
	{
		btnTex = instance->m_btnHover;
		ftColor = 0xffffffff;
	}

	// draw button
	int2 wndSolution = instance->m_device->GetWndSolution();
	pos.x = pos.x / wndSolution.x * 2;
	pos.y = pos.y / wndSolution.y * 2;

	size.x = size.x / wndSolution.x * 2;
	size.y = size.y / wndSolution.y * 2;

	float buf[] = {
		-1+pos.x-size.x*0.5f,	1-pos.y+size.y*0.5f,	0.0001,	0,0,
		-1+pos.x+size.x*0.5f,	1-pos.y+size.y*0.5f,	0.0001,	1,0,
		-1+pos.x-size.x*0.5f,	1-pos.y-size.y*0.5f,	0.0001,	0,1,
		-1+pos.x+size.x*0.5f,	1-pos.y-size.y*0.5f,	0.0001,	1,1 };

	instance->m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	instance->m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	instance->m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	instance->m_vd->Use();
	instance->m_shaders->Use();
	btnTex->Use( 0 );
	instance->m_device->SetTexAddress( 0, eTEXAD_Wrap );
	instance->m_device->SetTexFilter( 0, eTEXFT_Linear );

	instance->m_device->GetDevice()->
		DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, buf, sizeof(float)*5 );
	instance->m_btnFont->DrawTextA( NULL, str, -1, &field,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE, ftColor );

	instance->m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

	return res;

}
//---------------------------------------------------------------------------------
bool NlImmGUI::ButtonCustom( const char* normTexFile, const char* hoverTexFile, const char* pressTexFile, 
	float2 pos, float2 size, const char* str /*= "" */ )
{
	assert( instance );
	NlTexture2D* normTex = NlTextureManager::GetInstance().Get2DTextureFromFile( instance->m_device, normTexFile );
	NlTexture2D* hoverTex = NlTextureManager::GetInstance().Get2DTextureFromFile( instance->m_device, hoverTexFile );
	NlTexture2D* pressTex = NlTextureManager::GetInstance().Get2DTextureFromFile( instance->m_device, pressTexFile );
	if (!normTex || !hoverTex || !pressTex)
		return false;

	bool res = false;
	NlTexture2D* btnTex = normTex;
	uint32 ftColor = 0xc8ffffff;

	RECT field;
	field.left = pos.x - size.x*0.5f;
	field.top = pos.y - size.y*0.5f;
	field.right = pos.x + size.x*0.5f;
	field.bottom = pos.y + size.y*0.5f;

	POINT pt;
	::GetCursorPos( &pt );
	::ScreenToClient( instance->m_device->GetWindowHandle(), &pt );

	if (instance->m_lBtnState == eIS_Pressed)
	{
		if (::PtInRect( &field, pt ))
		{
			btnTex = pressTex;
			ftColor = 0x80ffffff;
			res = true;
		}

	}
	else if(::PtInRect( &field, pt))
	{
		btnTex = hoverTex;
		ftColor = 0xffffffff;
	}

	// draw button
	int2 wndSolution = instance->m_device->GetWndSolution();
	pos.x = pos.x / wndSolution.x * 2;
	pos.y = pos.y / wndSolution.y * 2;

	size.x = size.x / wndSolution.x * 2;
	size.y = size.y / wndSolution.y * 2;

	float buf[] = {
		-1+pos.x-size.x*0.5f,	1-pos.y+size.y*0.5f,	0.0001,	0,0,
		-1+pos.x+size.x*0.5f,	1-pos.y+size.y*0.5f,	0.0001,	1,0,
		-1+pos.x-size.x*0.5f,	1-pos.y-size.y*0.5f,	0.0001,	0,1,
		-1+pos.x+size.x*0.5f,	1-pos.y-size.y*0.5f,	0.0001,	1,1 };

	instance->m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	instance->m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	instance->m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	instance->m_vd->Use();
	instance->m_shaders->Use();
	btnTex->Use( 0 );
	instance->m_device->SetTexAddress( 0, eTEXAD_Wrap );
	instance->m_device->SetTexFilter( 0, eTEXFT_Linear );

	instance->m_device->GetDevice()->
		DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, buf, sizeof(float)*5 );
	instance->m_btnFont->DrawTextA( NULL, str, -1, &field,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE, ftColor );

	instance->m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

	return res;

}
//---------------------------------------------------------------------------------
bool NlImmGUI::OnInputEvent( const SInputEvent &event )
{
	switch (event.keyId)
	{
	case eKI_Mouse1:
		m_lBtnState = event.state;
		break;
	}

	return false;

}