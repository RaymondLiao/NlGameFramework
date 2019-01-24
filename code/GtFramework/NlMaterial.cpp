#include "nl_render_header.h"
#include "GtFile.h"
#include <sstream>

//=================================================================================
NlMaterial::NlMaterial( NlRenderDevice* device )
{
	assert( device );
	m_device = device;

	m_shaderParam = NULL;
	m_shaders = NULL;

	for (int i = 0; i < eMTSLOT_Max; ++i)
		m_textures[i] = NULL;
}
//---------------------------------------------------------------------------------
NlMaterial::~NlMaterial( void )
{
}
//---------------------------------------------------------------------------------
bool NlMaterial::GetLoadParamFromFile( NlMtrlLoadingParam& param, const char* mtrlFileName, const char* mtrlName )
{
	GtFile file( (ASSETS_PATH+mtrlFileName).c_str() );
	file.Open();
	if (file.IsOpen())
	{
		char strCommand[256] = {0};
		std::stringstream inFile( file.Data() );
		if (!inFile)
		{
			MessageBox( NULL, "load material failed", "error", MB_OK );
			return false;
		}

		for (; ;)
		{
			inFile >> strCommand;
			if (!inFile)
				break;

			if (0 == strcmp( strCommand, ("#") ))
			{
			}
			else if (0 == strcmp( strCommand, ("newmtl") ))
			{
				if ( param.mtrlName == mtrlName )
				{
					if (param.Glossness != 0.0f)
						param.mtrlType = (param.Trans == false) ? eMTType_Specular : eMTType_TranSpec;

					return true;
				}
				inFile >> param.mtrlName;
			}
			else if (0 == strcmp( strCommand, ("Ka") ))
			{
				inFile >> param.Ka.r >> param.Ka.g >> param.Ka.b;
			}
			else if (0 == strcmp( strCommand, ("Kd") ))
			{
				inFile >> param.Kd.r >> param.Kd.g >> param.Kd.b;
			}
			else if (0 == strcmp( strCommand, ("Ks") ))
			{
				inFile >> param.Ks.r >> param.Ks.g >> param.Ks.b;
			}
			else if (0 == strcmp( strCommand, ("Ns") ))
			{
				inFile >> param.Glossness;
				param.Glossness = Clamp( param.Glossness, 0.0f, 255.0f );
			}
			else if (0 == strcmp( strCommand, ("Trans") ))
			{
				param.Trans = true;
			}
			else if (0 == strcmp( strCommand, ("map_Kd") ))
			{
				inFile >> param.Kd_map;
			}
			else if (0 == strcmp( strCommand, ("map_Kb") ))
			{
				inFile >> param.Kb_map;
			}
			else if (0 == strcmp( strCommand, ("map_Ks") ))
			{
				inFile >> param.Ks_map;
			}
			else if (0 == strcmp( strCommand, ("map_Kr") ))
			{
				inFile >> param.Kr_map;
			}
			else
			{
				// unimplemented or unrecognized command
			}

			inFile.ignore( 1000, '\n' );	// swallow the whole line
		}
		file.Close();

		// deal with the last material
		if (param.mtrlName == mtrlName)
		{
			if (param.Glossness != 0.0f)
				param.mtrlType = (param.Trans == false) ? eMTType_Specular : eMTType_TranSpec;

			return true;
		}
	}

	MessageBox( NULL, "load material failed", "error", MB_OK );
	return false;

}
//---------------------------------------------------------------------------------
void NlMaterial::destroy()
{
	if (m_shaderParam)
	{
		delete m_shaderParam;
		m_shaderParam = NULL;
	}

	if (m_shaders)
	{
		m_shaders->destroy();
		delete m_shaders;
		m_shaders = NULL;
	}
	
	/*for (int i = 0; i < eMTSLOT_Max; ++i)
	{
		if (m_textures[i])
		{
			m_textures[i]->destroy();
			delete m_textures[i];
			m_textures[i] = NULL;
		}
	}*/

}
//---------------------------------------------------------------------------------
bool NlMaterial::init( NlMtrlLoadingParam& param )
{
	destroy();
	m_name = param.mtrlName;

	m_shaderParam = new NlDefShaderParam;
	assert( m_shaderParam );

	m_shaders = new NlDynamicShaderComb( m_device );
	assert( m_shaders );

	return true;
}

//=================================================================================
NlDiffuseMtrl::NlDiffuseMtrl( NlRenderDevice* device ):
	NlMaterial( device )
{
	m_Ka = m_Kd = float3( 0 );
}
//---------------------------------------------------------------------------------
NlDiffuseMtrl::~NlDiffuseMtrl()
{
}

//---------------------------------------------------------------------------------
bool NlDiffuseMtrl::init( NlMtrlLoadingParam& param )
{
	if (param.mtrlType != eMTType_Diffuse)
		return false;

	NlMaterial::init( param );

	m_Ka = param.Ka.xyz;
	m_Kd = param.Kd.xyz;

	if (!param.Kd_map.empty())
	{
		//m_textures[eMTSLOT_Diffuse] = new NlTexture2D( m_device );
		//assert( m_textures[eMTSLOT_Diffuse] );
		//m_textures[eMTSLOT_Diffuse]->LoadFromFile( param.Kd_map.c_str() );
		m_textures[eMTSLOT_Diffuse] = NlTextureManager::GetInstance().Get2DTextureFromFile( m_device, param.Kd_map.c_str() );
	}

	assert( m_shaders->CompileFromFile( "Diffuse.shader" ) == true );

	return true;
}
//---------------------------------------------------------------------------------
void NlDiffuseMtrl::Use()
{ 
	assert( m_shaders );
	m_shaders->Use();

	// counter clockwise cull mode
	m_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	// disable alpha-blending
	m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

	assert( m_shaderParam );
	m_device->SetGlobalVSMatrix( 0, m_shaderParam->wvp_mat );
	m_device->SetGlobalVSMatrix( 4, m_shaderParam->world_mat );

	m_device->SetGlobalPSFloat4( 0, float4(m_shaderParam->lightDir, 0) );
	m_device->SetGlobalPSFloat4( 1, float4(m_shaderParam->camPos, 0) );
	m_device->SetGlobalPSFloat4( 2, float4(m_Ka, 0) );
	m_device->SetGlobalPSFloat4( 3, float4(m_Kd, 0) );

	if (m_textures[eMTSLOT_Diffuse])
	{
		m_device->SetGlobalPSFloat( 4, 1 );
		m_textures[eMTSLOT_Diffuse]->Use( 0 );	// slot0 for diffuse_map
	}
	else
		m_device->SetGlobalPSFloat( 4, 0 );

}

//=================================================================================
NlSpecularMtrl::NlSpecularMtrl( NlRenderDevice* device ):
	NlDiffuseMtrl( device )
{
	m_Ks = float3( 0 );
	m_glossness = 0;
}
//---------------------------------------------------------------------------------
NlSpecularMtrl::~NlSpecularMtrl()
{
}
//---------------------------------------------------------------------------------
bool NlSpecularMtrl::init( NlMtrlLoadingParam& param )
{
	if (param.mtrlType != eMTType_Specular)
		return false;

	NlMaterial::init( param );

	m_Ka = param.Ka.xyz;
	m_Kd = param.Kd.xyz;
	m_Ks = param.Ks.xyz;
	m_glossness = param.Glossness;

	if (!param.Kd_map.empty())
	{
		//m_textures[eMTSLOT_Diffuse] = new NlTexture2D( m_device );
		//assert( m_textures[eMTSLOT_Diffuse] );
		//m_textures[eMTSLOT_Diffuse]->LoadFromFile( param.Kd_map.c_str() );
		m_textures[eMTSLOT_Diffuse] = NlTextureManager::GetInstance().Get2DTextureFromFile( m_device, param.Kd_map.c_str() );
	}
	if (!param.Ks_map.empty())
	{
		/*m_textures[eMTSLOT_Specular] = new NlTexture2D( m_device );
		assert( m_textures[eMTSLOT_Specular] );
		m_textures[eMTSLOT_Specular]->LoadFromFile( param.Ks_map.c_str() );*/
		m_textures[eMTSLOT_Specular] = NlTextureManager::GetInstance().Get2DTextureFromFile( m_device, param.Ks_map.c_str() );
	}

	assert( m_shaders->CompileFromFile( "Specular.shader" ) == true );

	return true;

}

//---------------------------------------------------------------------------------
void NlSpecularMtrl::Use()
{
	NlDiffuseMtrl::Use();

	m_device->SetGlobalPSFloat4( 5, float4(m_Ks, 0) );
	m_device->SetGlobalPSFloat( 6, m_glossness );

	if (m_textures[eMTSLOT_Specular])
	{
		m_device->SetGlobalPSFloat( 7, 1 );
		m_textures[eMTSLOT_Specular]->Use( 1 );	// slot1 for specular_map
	}
	else
		m_device->SetGlobalPSFloat( 7, 0 );

}

//=================================================================================
NlTranSpecMtrl::NlTranSpecMtrl( NlRenderDevice* device ):
	NlSpecularMtrl( device )
{
}
//---------------------------------------------------------------------------------
NlTranSpecMtrl::~NlTranSpecMtrl()
{
}
//---------------------------------------------------------------------------------
bool NlTranSpecMtrl::init( NlMtrlLoadingParam& param )
{
	if (param.mtrlType != eMTType_TranSpec)
		return false;

	NlMaterial::init( param );

	m_Ka = param.Ka.xyz;
	m_Kd = param.Kd.xyz;
	m_Ks = param.Ks.xyz;
	m_glossness = param.Glossness;

	if (!param.Kd_map.empty())
	{
		//m_textures[eMTSLOT_Diffuse] = new NlTexture2D( m_device );
		//assert( m_textures[eMTSLOT_Diffuse] );
		//m_textures[eMTSLOT_Diffuse]->LoadFromFile( param.Kd_map.c_str() );
		m_textures[eMTSLOT_Diffuse] = NlTextureManager::GetInstance().Get2DTextureFromFile( m_device, param.Kd_map.c_str() );
	}
	if (!param.Ks_map.empty())
	{
		//m_textures[eMTSLOT_Specular] = new NlTexture2D( m_device );
		//assert( m_textures[eMTSLOT_Specular] );
		//m_textures[eMTSLOT_Specular]->LoadFromFile( param.Ks_map.c_str() );
		m_textures[eMTSLOT_Specular] = NlTextureManager::GetInstance().Get2DTextureFromFile( m_device, param.Ks_map.c_str() );
	}

	assert( m_shaders->CompileFromFile( "TranSpec.shader" ) == true );

	return true;
}
//---------------------------------------------------------------------------------
void NlTranSpecMtrl::Use()
{
	NlSpecularMtrl::Use();

	// enable alpha-blending
	m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

}

//=================================================================================
NlMtrlManager* NlMtrlManager::instance = NULL;

NlMtrlManager::~NlMtrlManager()
{
	NlMaterialArray::iterator it;
	for (it = m_mtrls.begin(); it != m_mtrls.end(); ++it)
	{
		if ((*it))
		{
			(*it)->destroy();
			delete (*it);
			(*it) = NULL;
		}
	}
}
//---------------------------------------------------------------------------------
void NlMtrlManager::Create()
{
	Destroy();
	instance = new NlMtrlManager();
	assert( instance );

}
//---------------------------------------------------------------------------------
void NlMtrlManager::Destroy()
{
	if (instance)
	{
		delete instance;
		instance = NULL;
	}
}
//---------------------------------------------------------------------------------
NlMtrlManager& NlMtrlManager::GetInstance()
{
	assert( instance );
	return *instance;
}

//---------------------------------------------------------------------------------
NlMaterial* NlMtrlManager::GetMtrlByName( NlRenderDevice* device, 
	const char* mtrlFileName, const char* mtrlName )
{
	assert( device );

	NlMaterialArray::iterator it;
	for (it = m_mtrls.begin(); it != m_mtrls.end(); ++it)
	{
		if (!(*it))
			continue;

		if ((*it)->m_name == mtrlName)
			return (*it);
	}
	
	NlMtrlLoadingParam param;
	if (NlMaterial::GetLoadParamFromFile( param, mtrlFileName, mtrlName ) == false)
		return NULL;

	NlMaterial* newMtrl = NULL;
	switch( param.mtrlType )
	{
	case eMTType_Diffuse:
		{
			newMtrl = new NlDiffuseMtrl( device );
			assert( newMtrl );
			if (newMtrl->init(param) == false)
			{
				newMtrl->destroy();
				delete newMtrl;
				return NULL;
			}
		}break;

	case eMTType_Specular:
		{
			newMtrl = new NlSpecularMtrl( device );
			assert( newMtrl );
			if (newMtrl->init(param) == false)
			{
				newMtrl->destroy();
				delete newMtrl;
				return NULL;
			}
		}break;

	case eMTType_TranSpec:
		{
			newMtrl = new NlTranSpecMtrl( device );
			assert( newMtrl );
			if (newMtrl->init(param) == false)
			{
				newMtrl->destroy();
				delete newMtrl;
				return NULL;
			}
		}break;

	}
	
	m_mtrls.push_back( newMtrl );
	return newMtrl;

}