//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	ShaderObject.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/9/17
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ShaderObject_h_
#define _ShaderObject_h_

#include "prerequisite.h"
#include "GtFile.h"

class CShader
{
public:
	CShader( IDirect3DDevice9* device, const char* shader, const char* vs_main, const char* ps_main ):m_device(device), m_vertexShader(NULL), m_pixelShader(NULL)
	{
		// 编译shader
		CreateShader( shader, vs_main, ps_main );
	}

	CShader( IDirect3DDevice9* device, const char* filename ):m_device(device), m_vertexShader(NULL), m_pixelShader(NULL)
	{
		GtFile file( filename );
		file.Open();

		if (file.IsOpen())
		{
			CreateShader( file.Data(), "vs_main", "ps_main" );
		}
		else
		{
			MessageBox(NULL, "shader file not find.", "shader compile failed", MB_OK);
		}
		
		file.Close();
	}

	~CShader()
	{
		m_vertexShader->Release();
		m_pixelShader->Release();
	}

	void CreateShader( const char* shader, const char* vs_main, const char* ps_main )
	{
		// 编译shader
		LPD3DXBUFFER vShader = NULL;
		LPD3DXBUFFER pShader = NULL;
		LPD3DXBUFFER vShaderError = NULL;
		if( D3D_OK != D3DXCompileShader( shader, strlen(shader), NULL, NULL, vs_main, "vs_3_0", NULL, &vShader, &vShaderError, NULL ) )
		{
			MessageBox(NULL, (char*)vShaderError->GetBufferPointer(), "shader compile failed", MB_OK);
		}
		if( D3D_OK != D3DXCompileShader( shader, strlen(shader), NULL, NULL, ps_main, "ps_3_0", NULL, &pShader, &vShaderError, NULL ) )
		{
			MessageBox(NULL, (char*)vShaderError->GetBufferPointer(), "shader compile failed", MB_OK);
		}

		// 编译成功了，创建shader
		if (vShader)
		{
			m_device->CreateVertexShader( (DWORD*)vShader->GetBufferPointer(), &m_vertexShader );
		}
		if (pShader)
		{
			m_device->CreatePixelShader( (DWORD*)pShader->GetBufferPointer(), &m_pixelShader );
		}
	}

	void Use()
	{
		m_device->SetVertexShader( m_vertexShader );
		m_device->SetPixelShader( m_pixelShader );
	}

	void VS_SetMatrix( int slot, const float44& matrix)
	{
		m_device->SetVertexShaderConstantF( slot, matrix.m, 4);
	}

	void VS_SetFloat4( int slot, const float4& vector )
	{
		m_device->SetVertexShaderConstantF( slot, &(vector.x), 1);
	}

	void PS_SetMatrix( int slot, const float44& matrix)
	{
		m_device->SetPixelShaderConstantF( slot, matrix.m, 4);
	}

	void PS_SetFloat4( int slot, const float4& vector )
	{
		m_device->SetPixelShaderConstantF( slot, &(vector.x), 1);
	}


	IDirect3DDevice9* m_device;
	IDirect3DVertexShader9* m_vertexShader;
	IDirect3DPixelShader9* m_pixelShader;
};

#endif