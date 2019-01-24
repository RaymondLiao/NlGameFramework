/*
  @file   NlTexture.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlTexture_h_
#define _NlTexture_h_

#include "prerequisite.h"

//====================================================
class NlRenderDevice;
class NlTextureBase
{
public:
	NlTextureBase( NlRenderDevice* device );
	virtual ~NlTextureBase(void);

	virtual bool LoadFromFile( const char* fileName ) = 0;
	virtual void destroy();

	void Use( uint8 slot );
	void SetFilter( uint8 slot, bool filter );
	void SetAddressMode( uint8 slot, uint32 mode );

protected:
	IDirect3DDevice9*      m_device;
	IDirect3DBaseTexture9* m_texture;
	//IDirect3DSurface9*     m_surf;		??
	
};

//====================================================
typedef struct _NlTexture2DInfo
{
	_NlTexture2DInfo():width(0), height(0), 
		forRTT(false), pitch(0) {}

	int width;
	int height;
	bool forRTT;

	int pitch;

} NlTexture2DInfo;

class NlTexture2D :
	public NlTextureBase
{
	friend class NlTextureManager;
public:
	NlTexture2D( NlRenderDevice* device );
	bool init( uint32 width, uint32 height, bool forRTT );

	const NlTexture2DInfo& GetInfo() {return m_info;}
	bool update( void* colors );
	bool Copy( NlTexture2D& other );	// need 

	bool StartTTS();
	void EndTTS();

private:
	~NlTexture2D() {}
	virtual bool LoadFromFile( const char* fileName );
	virtual void destroy() { NlTextureBase::destroy(); }

	IDirect3DSurface9* GetSurf();

	NlTexture2DInfo m_info;
};

//====================================================
class NlTextureCube :
	public NlTextureBase
{
public:
	NlTextureCube( NlRenderDevice* device ): NlTextureBase(device) {}
	~NlTextureCube() {}

	virtual bool LoadFromFile( const char* fileName );

};

// Singleton ==========================================
class NlTextureManager
{
	friend class NlGFramework;
public:
	static NlTextureManager& GetInstance();
	// if the 2d texture doesn't exist, create and return it; else return it
	NlTexture2D* Get2DTextureFromFile( NlRenderDevice* device,
		const char* fileName );
	// if the 2d texture is created manually, use this funtion to destroy it
	void Destroy2DTexture( NlTexture2D* texture );

private:
	NlTextureManager() {}
	NlTextureManager( const NlTextureManager& another ) {}
	NlTextureManager& operator= ( const NlTextureManager& another ) {}
	~NlTextureManager();

	static void Create();
	static void Destroy();

private:
	std::map<std::string, NlTexture2D*> m_2Ds;

	static NlTextureManager* instance;
	
};

#endif