//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	TextureObject.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/10/7
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _TextureObject_h_
#define _TextureObject_h_

#include "prerequisite.h"

class CTexture
{
public:
	CTexture(IDirect3DDevice9* device):m_device(device), m_texture(NULL)
	{

	}

	~CTexture()
	{

	}

	void LoadFromFile( const char* filename)
	{
		std::string path = getMediaPath();
		path += filename;
		D3DXCreateTextureFromFileA( m_device, path.c_str(), &m_texture );
	}

	void Use(uint8 slot)
	{
		m_device->SetTexture( slot, m_texture );
	}

	void SetFilter(uint8 slot, bool filter)
	{
		if (filter)
		{
			m_device->SetSamplerState( slot, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			m_device->SetSamplerState( slot, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			m_device->SetSamplerState( slot, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		}
		else
		{
			m_device->SetSamplerState( slot, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			m_device->SetSamplerState( slot, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			m_device->SetSamplerState( slot, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		}

	}

private:
	IDirect3DTexture9* m_texture;
	IDirect3DDevice9* m_device;
};

#endif