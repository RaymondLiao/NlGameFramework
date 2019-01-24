#include "stableheader.h"
#include "nl_render_header.h"
#include "dxsdk/Include/D3Dcompiler.h"

#include <string.h>
#include <assert.h>

#pragma comment( lib, "dxsdk/lib/x86/d3dCompiler.lib" )

//=================================================================================
NlDynamicShaderComb::NlDynamicShaderComb( NlRenderDevice* renderDevice )
{
	assert( renderDevice );
	m_renderDevice = renderDevice;

	m_vs = NULL;
	m_ps = NULL;

	m_hShaderFile = NULL;
}
//---------------------------------------------------------------------------------
NlDynamicShaderComb::~NlDynamicShaderComb( void )
{
}
//---------------------------------------------------------------------------------
bool NlDynamicShaderComb::CompileFromFile( const std::string& fileName )
{
	m_fileName = fileName;
	// read file's data-----------------------------------------
	DWORD fDataSize = 0;
	char *fData = NULL;

	std::string realpath = getMediaPath() + ASSETS_PATH + m_fileName;
	m_hShaderFile = CreateFile( realpath.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if (m_hShaderFile)
	{
		// get the time informations
		GetFileTime( m_hShaderFile, NULL, NULL, &m_lastUpdateTime );

		// get file size
		fDataSize = GetFileSize( m_hShaderFile, NULL );
		if ( fDataSize == -1)
		{
			CloseHandle( m_hShaderFile );
			MessageBox( NULL, "compile shader failed", "error", MB_OK );
			return false;
		}

		// alloc memory
		fData = new char[fDataSize + 1];
		memset( fData, 0, fDataSize+1 );
		if (!fData)
		{
			CloseHandle( m_hShaderFile );
			MessageBox( NULL, "compile shader failed", "error", MB_OK );
			return false;
		}

		// copy data in file
		DWORD readSize;
		bool readSuccess = ReadFile( m_hShaderFile, fData, fDataSize, &readSize, NULL );

		if (!readSuccess || fDataSize != readSize)
		{
			delete[] fData;
			MessageBox( NULL, "compile shader failed", "error", MB_OK );
			return false;
		}
		else
		{
			fData[fDataSize] = '\0';
		}
	}
	else
	{
		MessageBox( NULL, "compile shader failed", "error", MB_OK );
		return false;
	}

	// creat shader-----------------------------------------
	IDirect3DDevice9 *d3dDevice = m_renderDevice->GetDevice();
	ID3DBlob *bincode = NULL;
	ID3DBlob *errormsg = NULL;

	if (D3D_OK == D3DCompile( fData, fDataSize, NULL,
		NULL, NULL,
		"vs_main", "vs_3_0",
		NULL, NULL,
		&bincode, &errormsg ))
	{
		d3dDevice->CreateVertexShader( (DWORD*)bincode->GetBufferPointer(), &m_vs );
	}
	else
	{
		MessageBox( NULL, (char*)errormsg->GetBufferPointer(), "CompileFail", MB_OK );
		return false;
	}

	if (D3D_OK == D3DCompile( fData, fDataSize, NULL,
		NULL, NULL,
		"ps_main", "ps_3_0",
		NULL, NULL,
		&bincode, &errormsg ))
	{
		d3dDevice->CreatePixelShader( (DWORD*)bincode->GetBufferPointer(), &m_ps );
	}
	else
	{
		MessageBox( NULL, (char*)errormsg->GetBufferPointer(), "CompileFail", MB_OK );
		return false;
	}

	delete[] fData;
	return true;

}
//---------------------------------------------------------------------------------
void NlDynamicShaderComb::update()
{
	if (!m_hShaderFile)
		return;

	// check whether the shader file is changed
	FILETIME updateTime;
	GetFileTime( m_hShaderFile, NULL, NULL, &updateTime );
	if (CompareFileTime(&updateTime, &m_lastUpdateTime) != 1)
		return;
	m_lastUpdateTime = updateTime;

	// get new file's data--------------------------------------
	DWORD fDataSize = 0;
	char *fData = NULL;

	fDataSize = GetFileSize( m_hShaderFile, NULL );
	if ( fDataSize == -1)
	{
		MessageBox( NULL, "update shader failed", "error", MB_OK );
		return;
	}

	fData = new char[fDataSize + 1];
	memset( fData, 0, fDataSize+1 );
	if (!fData)
	{
		fDataSize = 0;
		MessageBox( NULL, "update shader failed", "error", MB_OK );
		return;
	}

	DWORD readSize;
	SetFilePointer( m_hShaderFile, 0, NULL, FILE_BEGIN );
	bool readSuccess = ReadFile( m_hShaderFile, fData, fDataSize, &readSize, NULL );

	if (!readSuccess || fDataSize != readSize)
	{
		delete[] fData;
		MessageBox( NULL, "update shader failed", "error", MB_OK );
		return;
	}
	else
	{
		fData[fDataSize] = '\0';
	}

	// recreate shader----------------------------------------
	IDirect3DDevice9 *d3dDevice = m_renderDevice->GetDevice();
	ID3DBlob *bincode = NULL;
	ID3DBlob *errormsg = NULL;

	if (D3D_OK == D3DCompile( fData, fDataSize, NULL,
		NULL, NULL,
		"vs_main", "vs_3_0",
		NULL, NULL,
		&bincode, &errormsg ))
	{
		// update vs
		m_vs->Release();
		d3dDevice->CreateVertexShader( (DWORD*)bincode->GetBufferPointer(), &m_vs );
	}
	else
	{
		MessageBox( NULL, (char*)errormsg->GetBufferPointer(), "CompileFail", MB_OK );
	}

	if (D3D_OK == D3DCompile( fData, fDataSize, NULL,
		NULL, NULL,
		"ps_main", "ps_3_0",
		NULL, NULL,
		&bincode, &errormsg ))
	{
		// update ps
		m_ps->Release();
		d3dDevice->CreatePixelShader( (DWORD*)bincode->GetBufferPointer(), &m_ps );
	}
	else
	{
		MessageBox( NULL, (char*)errormsg->GetBufferPointer(), "CompileFail", MB_OK );
	}

	delete[] fData;
}
//---------------------------------------------------------------------------------
void NlDynamicShaderComb::destroy()
{
	if (m_hShaderFile)
		CloseHandle( m_hShaderFile );

	if (m_vs != NULL)
		m_vs->Release();
	if (m_ps != NULL)
		m_ps->Release();
}
//---------------------------------------------------------------------------------
void NlDynamicShaderComb::Use()
{
	IDirect3DDevice9 *d3dDevice = m_renderDevice->GetDevice();
	d3dDevice->SetVertexShader( m_vs );
	d3dDevice->SetPixelShader( m_ps );
}
//---------------------------------------------------------------------------------
void NlDynamicShaderComb::SetMatrix_vs( int slot, const float44& matrix )
{
	IDirect3DDevice9 *d3dDevice = m_renderDevice->GetDevice();
	d3dDevice->SetVertexShaderConstantF( slot, matrix.m, 4 );
}
//---------------------------------------------------------------------------------
void NlDynamicShaderComb::SetFloat4_vs( int slot, const float4& vector )
{
	IDirect3DDevice9 *d3dDevice = m_renderDevice->GetDevice();
	d3dDevice->SetVertexShaderConstantF( slot, &(vector.x), 1 );
}
//---------------------------------------------------------------------------------
void NlDynamicShaderComb::SetMatrix_ps( int slot, const float44& matrix )
{
	IDirect3DDevice9 *d3dDevice = m_renderDevice->GetDevice();
	d3dDevice->SetPixelShaderConstantF( slot, matrix.m, 4 );
}
//---------------------------------------------------------------------------------
void NlDynamicShaderComb::SetFloat4_ps( int slot, const float4& vector )
{
	IDirect3DDevice9 *d3dDevice = m_renderDevice->GetDevice();
	d3dDevice->SetPixelShaderConstantF( slot, &(vector.x), 1 );
}