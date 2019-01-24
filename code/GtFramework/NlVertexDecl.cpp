#include "nl_render_header.h"

//=================================================================================
NlVertexDecl::NlVertexDecl( NlRenderDevice* device ): m_device(device->GetDevice())
{
	assert( m_device );
	m_vd = NULL;
}
//---------------------------------------------------------------------------------
NlVertexDecl::~NlVertexDecl(void)
{
	if (m_vd)
	{
		m_vd->Release();
		m_vd = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlVertexDecl::init()
{
	// clear up
	destroy();

	std::vector<D3DVERTEXELEMENT9> elements;

	{
		D3DVERTEXELEMENT9 elem = { 0, 0,
			D3DDECLTYPE_FLOAT3,
			D3DDECLMETHOD_DEFAULT,
			D3DDECLUSAGE_POSITION,
			0 };
		elements.push_back( elem );
	}
	{
		D3DVERTEXELEMENT9 elem = { 0, 12,
			D3DDECLTYPE_FLOAT3,
			D3DDECLMETHOD_DEFAULT,
			D3DDECLUSAGE_NORMAL,
			0 };
		elements.push_back( elem );
	}
	{
		D3DVERTEXELEMENT9 elem = { 0, 24,
			D3DDECLTYPE_FLOAT2,
			D3DDECLMETHOD_DEFAULT,
			D3DDECLUSAGE_TEXCOORD,
			0 };
		elements.push_back( elem );
	}
	{
		D3DVERTEXELEMENT9 elem = D3DDECL_END();
		elements.push_back( elem );
	}

	m_device->CreateVertexDeclaration( elements.data(), &m_vd );
}
//---------------------------------------------------------------------------------
void NlVertexDecl::init( VertexElemArray& vertexLayout )
{
	// clear up
	destroy();

	std::vector<D3DVERTEXELEMENT9> elements;
	uint32 startPos[MAX_STREAM_COUT];
	memset( startPos, 0, MAX_STREAM_COUT*sizeof(uint32) );

	std::vector<NlVertexElem>::iterator it = vertexLayout.begin();
	for ( ; it != vertexLayout.end(); ++it)
	{
		uint32 stream = it->stream;
		ENlVertexElemUsage usage = it->usage;
		uint8 usageIndex = it->usageIndex;

		if (usage == eVEU_Position)
		{
			D3DVERTEXELEMENT9 elem = { stream, startPos[stream],
				D3DDECLTYPE_FLOAT3,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_POSITION,
				usageIndex };
			elements.push_back( elem );

			startPos[stream] += 12;
		}
		if (usage == eVEU_Normal)
		{
			D3DVERTEXELEMENT9 elem = { stream, startPos[stream],
				D3DDECLTYPE_FLOAT3,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_NORMAL,
				usageIndex };
			elements.push_back( elem );

			startPos[stream] += 12;
		}
		if (usage == eVEU_Texcoord)
		{
			D3DVERTEXELEMENT9 elem = { stream, startPos[stream],
				D3DDECLTYPE_FLOAT2,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_TEXCOORD,
				usageIndex };
			elements.push_back( elem );

			startPos[stream] += 8;
		}
		if (usage == eVEU_Tangent)
		{
			D3DVERTEXELEMENT9 elem = { stream, startPos[stream],
				D3DDECLTYPE_FLOAT3,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_TANGENT,
				usageIndex };
			elements.push_back( elem );

			startPos[stream] += 12;
		}
		if (usage == eVEU_Color)
		{
			D3DVERTEXELEMENT9 elem = { stream, startPos[stream],
				D3DDECLTYPE_FLOAT3,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_COLOR,
				usageIndex };
			elements.push_back( elem );

			startPos[stream] += 12;
		}
	}
	{
		D3DVERTEXELEMENT9 elem = D3DDECL_END();
		elements.push_back( elem );
	}

	m_device->CreateVertexDeclaration( elements.data(), &m_vd );
	
}
//---------------------------------------------------------------------------------
void NlVertexDecl::destroy()
{
	if (m_vd)
	{
		m_vd->Release();
		m_vd = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlVertexDecl::Use()
{
	m_device->SetVertexDeclaration( m_vd );
}