//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	MeshObject.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/10/7
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _MeshObject_h_
#define _MeshObject_h_

#include "BufferObject.h"

struct SPt332
{
	float3 pos;
	float3 normal;
	float2 texcoord;

	SPt332()
	{

	}

	SPt332( float3 p, float3 n, float2 t )
	{
		pos = p;
		normal = n;
		texcoord = t;
	}
};

class CMesh
{
public:
	CMesh( IDirect3DDevice9* device )
	{
		m_vertexBuffer = NULL;
		m_vertexDecl = NULL;
		m_indexBuffer = NULL;
		m_device = device;
	}
	virtual ~CMesh()
	{
		if (m_vertexBuffer)
		{
			delete m_vertexBuffer;
		}

		if (m_indexBuffer)
		{
			delete m_indexBuffer;
		}

		if (m_vertexDecl)
		{
			delete m_vertexDecl;
		}

	}

	void Draw()
	{
		if (m_vertexBuffer && m_vertexDecl)
		{
			m_vertexBuffer->Use();
			m_vertexDecl->Use();

			if (m_indexBuffer)
			{
				m_indexBuffer->Use();

				m_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_vertexBuffer->Size(), 0, m_indexBuffer->Size() / 3 );
			}
			else
			{
				m_device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_vertexBuffer->Size() / 3);
			}
		}
	}

	CVertexBuffer* m_vertexBuffer;
	CIndexBuffer* m_indexBuffer;
	CVertexDecl* m_vertexDecl;
	IDirect3DDevice9* m_device;
};

typedef std::vector<CMesh*> CMeshs;

#endif