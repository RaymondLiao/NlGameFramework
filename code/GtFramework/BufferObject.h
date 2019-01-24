//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	BufferObject.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/9/17
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _BufferObject_h_
#define _BufferObject_h_

#include "prerequisite.h"

class CIndexBuffer
{
public:
	CIndexBuffer( IDirect3DDevice9* device, int count, int dword = true ):m_device(device), m_count(count)
	{
		m_device->CreateIndexBuffer( m_count * sizeof(int32), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_indexBuffer, NULL );
	}

	~CIndexBuffer()
	{
		if (m_indexBuffer)
		{
			m_indexBuffer->Release();
		}
	}

	void Update(void* data, int length)
	{
		void* gpuData = NULL;
		if( D3D_OK == m_indexBuffer->Lock( 0, length, &gpuData, NULL ) )
		{
			memcpy( gpuData, data, length );
			m_indexBuffer->Unlock();
		}
		else
		{
			MessageBox(NULL, "ib create failed", "shader compile failed", MB_OK);
		}
	}

	void Use()
	{
		m_device->SetIndices( m_indexBuffer );
	}

	int Size()
	{
		return m_count;
	}

	IDirect3DDevice9* m_device;
	IDirect3DIndexBuffer9* m_indexBuffer;
	int m_count;
};

class CVertexBuffer
{
public:
	CVertexBuffer( IDirect3DDevice9* device, int bufferSize, int vertSize = 12 ):m_device(device), m_vertSize(vertSize)
	{
		m_device->CreateVertexBuffer( bufferSize, D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT, &m_vertexBuffer, NULL );
		m_count = bufferSize / vertSize;
	}

	~CVertexBuffer()
	{
		m_vertexBuffer->Release();
	}

	void Update(void* data, int length)
	{
		void* gpuData = NULL;
		if( D3D_OK == m_vertexBuffer->Lock( 0, length, &gpuData, NULL ) )
		{
			memcpy( gpuData, data, length );
			m_vertexBuffer->Unlock();
		}
		else
		{
			MessageBox(NULL, "vb create failed", "shader compile failed", MB_OK);
		}
	}

	void Use()
	{
		m_device->SetStreamSource( 0, m_vertexBuffer, 0, m_vertSize);
	}

	int Size()
	{
		return m_count;
	}

	IDirect3DDevice9* m_device;
	IDirect3DVertexBuffer9* m_vertexBuffer;
	int m_vertSize;
	int m_count;
};


class CVertexDecl
{
public:
	CVertexDecl( IDirect3DDevice9* device ):m_device(device), m_decl(NULL)
	{

	}

	~CVertexDecl()
	{
		if (m_decl)
		{
			m_decl->Release();
		}
	}

	void AddElement( D3DVERTEXELEMENT9 element )
	{
		m_elements.push_back( element );

		m_dirty = true;
	}

	void Use()
	{
		if ( m_dirty )
		{
			if ( m_decl )
			{
				m_decl->Release();
			}
			m_device->CreateVertexDeclaration( &(m_elements[0]), &m_decl );

			m_dirty = false;
		}

		m_device->SetVertexDeclaration( m_decl );
	}

	std::vector<D3DVERTEXELEMENT9> m_elements;
	IDirect3DVertexDeclaration9* m_decl;
	IDirect3DDevice9* m_device;
	bool m_dirty;
};

#endif