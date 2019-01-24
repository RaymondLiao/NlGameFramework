#include "NlGUIText.h"
#include "NlGameEnvironment.h"
#include "NlRenderDevice.h"

//=================================================================================
NlGUIText::NlGUIText( NlRenderDevice* device )
{
	assert( device );
	m_device = device->GetDevice();
	m_pos = device->GetWndSolution();

	m_font = NULL;

}
//---------------------------------------------------------------------------------
NlGUIText::~NlGUIText( void )
{
}
//---------------------------------------------------------------------------------
bool NlGUIText::init( NlTextDesc desc )
{
	destroy();

	D3DXFONT_DESC ftDesc;
	ZeroMemory( &ftDesc, sizeof(D3DXFONT_DESC) );
	ftDesc.Width = desc.size*0.25f;
	ftDesc.Height = desc.size * 0.6f;
	if (desc.style == eFT_Bold || desc.style == eFT_BoldAndItalic)
		ftDesc.Weight = 1000;
	else
		ftDesc.Weight = 500;
	ftDesc.MipLevels = D3DX_DEFAULT;
	if (desc.style == eFT_Italic || desc.style == eFT_BoldAndItalic)
		ftDesc.Italic = true;
	else
		ftDesc.Italic = false;
	ftDesc.CharSet = DEFAULT_CHARSET;
	ftDesc.OutputPrecision = 0;
	ftDesc.Quality = 0;
	ftDesc.PitchAndFamily = 0;
	switch( desc.font )
	{
	case eFT_Arial:
		strcpy( ftDesc.FaceName, "Arial" );
		break;

	case eFT_TimesNewRoman:
		strcpy( ftDesc.FaceName, "Times New Roman" );
		break;
	}

	m_pos.x = desc.position.x * m_pos.x;
	m_pos.y = (1-desc.position.y) * m_pos.y;
	m_alignment = desc.alignment;
	return (D3D_OK == D3DXCreateFontIndirect( m_device, &ftDesc, &m_font ));

}
//---------------------------------------------------------------------------------
void NlGUIText::destroy()
{
	if (m_font)
	{
		m_font->Release();
		m_font = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlGUIText::Draw( const char* content, uint32 color /*= 0xffffffff */ )
{
	if (m_font == NULL)
		return;

	std::string _content = content;
	
	// split the string
	std::string::size_type pos;
	std::vector<std::string> subStrings;
	int contSize = _content.size();
	int maxLineWid = 0;
	int lineCount = 1;

	_content += '\n';
	for (int i = 0; i < contSize; ++i)
	{
		pos = _content.find( '\n', i );
		if (!(pos > contSize))
		{
			std::string str = _content.substr( i, pos-i+1 );
			if (maxLineWid < str.size())
				maxLineWid = str.size();

			subStrings.push_back( str );
			i = pos + 1;
		}
	}
	if (maxLineWid == 0)	// no '\n'
		maxLineWid = _content.size();
	lineCount = subStrings.size()==0 ? 1 : subStrings.size();

	D3DXFONT_DESC ftDesc;
	m_font->GetDesc( &ftDesc );
	uint32 alignment;
	switch( m_alignment )
	{
	case eFT_Left:
		alignment = DT_LEFT;
		break;

	case eFT_Center:
		alignment = DT_CENTER;
		break;

	case eFT_Right:
		alignment = DT_RIGHT;
		break;
	}

	RECT field;
	field.left = m_pos.x - ftDesc.Width*1.3f*maxLineWid * 0.5f;
	field.bottom = m_pos.y + ftDesc.Height*1.3f*lineCount * 0.5f;
	field.right = m_pos.x + ftDesc.Width*maxLineWid * 0.5f;
	field.top = m_pos.y - ftDesc.Height*lineCount * 0.5f;
	
	m_font->DrawTextA( NULL, content, -1, &field, alignment, color );

}