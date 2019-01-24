#include "NlTexture.h"
#include "nl_render_header.h"

//=================================================================================
NlTextureBase::NlTextureBase( NlRenderDevice* device )
{
	assert( device );
	m_device = device->GetDevice();
	m_texture = NULL;
}
//---------------------------------------------------------------------------------
NlTextureBase::~NlTextureBase(void)
{
}
//---------------------------------------------------------------------------------
void NlTextureBase::destroy()
{
	if (m_texture)
	{
		m_texture->Release();
		m_texture = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlTextureBase::Use( uint8 slot )
{
	m_device->SetTexture( slot, m_texture );
}
//---------------------------------------------------------------------------------
void NlTextureBase::SetFilter( uint8 slot, bool filter )
{
	if (filter)
	{
		m_device->SetSamplerState( slot, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		m_device->SetSamplerState( slot, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		m_device->SetSamplerState( slot, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	}
	else
	{
		m_device->SetSamplerState( slot, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		m_device->SetSamplerState( slot, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m_device->SetSamplerState( slot, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	}
}
//---------------------------------------------------------------------------------
void NlTextureBase::SetAddressMode( uint8 slot, uint32 mode )
{
	m_device->SetSamplerState( slot, D3DSAMP_ADDRESSU, mode );
	m_device->SetSamplerState( slot, D3DSAMP_ADDRESSV, mode );
}

//=================================================================================
NlTexture2D::NlTexture2D( NlRenderDevice* device ):
	NlTextureBase(device)
{
}
//---------------------------------------------------------------------------------
bool NlTexture2D::init( uint32 width, uint32 height, bool forRTT )
{
	m_info.width = width;
	m_info.height = height;
	m_info.forRTT = forRTT;

	uint32 usage = forRTT ? D3DUSAGE_RENDERTARGET : NULL;
	uint32 pool  = forRTT ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
	m_device->CreateTexture( width, height, 1, 
		usage,
		D3DFMT_A8R8G8B8,
		D3DPOOL(pool),
		(LPDIRECT3DTEXTURE9*)(&m_texture),
		NULL );
	assert( m_texture );

	IDirect3DTexture9* texture = (IDirect3DTexture9*)m_texture;
	D3DLOCKED_RECT rc;
	if (D3D_OK == texture->LockRect( 0, &rc, NULL, NULL ))
	{
		m_info.pitch = rc.Pitch;
		texture->UnlockRect( 0 );
	}
	else
		return false;

	return true;
}
//---------------------------------------------------------------------------------
bool NlTexture2D::update( void* colors )
{
	IDirect3DTexture9* texture = (IDirect3DTexture9*)m_texture;
	NlTexture2DInfo& info = m_info;

	D3DLOCKED_RECT rc;
	if (D3D_OK == texture->LockRect(0, &rc, NULL, NULL))
	{
		void* cData = rc.pBits;
		memcpy( cData, colors, info.height*m_info.pitch );
		texture->UnlockRect( 0 );
	}
	else
		return false;

	return true;
}
//---------------------------------------------------------------------------------
bool NlTexture2D::Copy( NlTexture2D& other )
{
	const NlTexture2DInfo &otherInfo = other.GetInfo();

	if (!m_info.forRTT || !otherInfo.forRTT)
		return false;
	if (m_info.width != otherInfo.width || m_info.height != otherInfo.height)
		return false;

	IDirect3DSurface9* sourceSurf = GetSurf();
	IDirect3DSurface9* destSurf = other.GetSurf();
	if (!sourceSurf || !destSurf)
		return false;

	m_device->StretchRect( sourceSurf, NULL, destSurf, NULL, D3DTEXF_POINT );

	return true;
}
//---------------------------------------------------------------------------------
bool NlTexture2D::LoadFromFile( const char* fileName )
{
	destroy();
	
	std::string path= getMediaPath() + ASSETS_PATH;
	path += fileName;
	D3DXCreateTextureFromFileA( m_device, path.c_str(), (LPDIRECT3DTEXTURE9*)&m_texture );
	if (!m_texture)
	{
		MessageBox( NULL, "load texture failed", "error", MB_OK );
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------------
bool NlTexture2D::StartTTS()
{
	if (m_info.forRTT == false)
		return false;

	IDirect3DSurface9* surf = GetSurf();
	assert( surf );

	m_device->SetRenderTarget( 0, surf );

	return true;
}
//---------------------------------------------------------------------------------
void NlTexture2D::EndTTS()
{
	IDirect3DSurface9* surfBackBuffer = NULL;
	m_device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &surfBackBuffer );

	m_device->SetRenderTarget( 0, surfBackBuffer );
}
//---------------------------------------------------------------------------------
IDirect3DSurface9* NlTexture2D::GetSurf()
{
	IDirect3DSurface9* surf = NULL;
	((IDirect3DTexture9*)m_texture)->GetSurfaceLevel( 0, &surf );
	return surf;
}

//=================================================================================
bool NlTextureCube::LoadFromFile( const char* fileName )
{
	destroy();
	
	std::string path= getMediaPath() + ASSETS_PATH;
	path += fileName;
	D3DXCreateCubeTextureFromFileA( m_device, path.c_str(), (LPDIRECT3DCUBETEXTURE9*)&m_texture );
	if (!m_texture)
	{
		MessageBox( NULL, "load texture failed", "error", MB_OK );
		return false;
	}

	return true;
}

//=================================================================================
NlTextureManager* NlTextureManager::instance = NULL;

NlTextureManager::~NlTextureManager()
{
	std::map<std::string, NlTexture2D*>::iterator it;
	for (it = m_2Ds.begin(); it != m_2Ds.end(); ++it)
	{
		if (it->second)
		{
			it->second->destroy();
			delete it->second;
			it->second = NULL;
		}
	}

}
//---------------------------------------------------------------------------------
NlTextureManager& NlTextureManager::GetInstance()
{
	assert(instance);
	return *instance;
}
//---------------------------------------------------------------------------------
NlTexture2D* NlTextureManager::Get2DTextureFromFile( NlRenderDevice* device, const char* fileName )
{
	assert( device );

	NlTexture2D* tex2D = m_2Ds[fileName];
	if (tex2D == NULL)
	{
		tex2D = new NlTexture2D( device );
		if (tex2D->LoadFromFile( fileName ) == false)
		{
			tex2D->destroy();
			delete tex2D;
			return NULL;
		}

		m_2Ds[fileName] = tex2D;
	}

	return tex2D;

}
//---------------------------------------------------------------------------------
void NlTextureManager::Destroy2DTexture( NlTexture2D* texture )
{
	if (texture == NULL)
		return;

	std::map<std::string, NlTexture2D*>::iterator it;
	for (it = m_2Ds.begin(); it != m_2Ds.end(); ++it)
	{
		if (it->second && it->second == texture)
		{
			it->second->destroy();
			delete it->second;
			it->second = NULL;
		}
	}
	
	if (it == m_2Ds.end())
	{
		texture->destroy();
		delete texture;
		texture = NULL;
	}

}
//---------------------------------------------------------------------------------
void NlTextureManager::Create()
{
	Destroy();
	instance = new NlTextureManager();
	assert( instance );

}
//---------------------------------------------------------------------------------
void NlTextureManager::Destroy()
{
	if (instance)
	{
		delete instance;
		instance = NULL;
	}

}