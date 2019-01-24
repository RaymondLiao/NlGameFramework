#include "nl_render_header.h"
#include "NlObjMeshLoader.h"

//=================================================================================
NlMesh::NlMesh( NlRenderDevice* device )
{
	assert( device );
	m_device = device;

	m_vb = NULL;
	m_ib = NULL;
	m_vd = NULL;
}
//---------------------------------------------------------------------------------
NlMesh::~NlMesh( void )
{
}
//---------------------------------------------------------------------------------
bool NlMesh::LoadFromFile( const char* objFileName, const char* submeshName )
{
	destroy();
	m_name = std::string( submeshName );

	NlObjMeshLoader loader( m_device );
	if (loader.LoadObjGeometryFromFile( (ASSETS_PATH+objFileName).c_str(), submeshName ))
	{
		m_vb = loader.m_vb;
		m_ib = loader.m_ib;
		m_vd = loader.m_vd;

		m_mtrlName = loader.GetMaterialName();
	}
	else
	{
		MessageBox( NULL, "load mesh failed", "error", MB_OK );
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------------
void NlMesh::destroy()
{
	if (m_vb)
	{
		m_vb->destroy();
		delete m_vb;
		m_vb = NULL;
	}
	if (m_ib)
	{
		m_ib->destroy();
		delete m_ib;
		m_ib = NULL;
	}
	if (m_vd)
	{
		m_vd->destroy();
		delete m_vd;
		m_vd = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlMesh::Use()
{
	if (m_vb)
		m_vb->Use();
	if (m_ib)
		m_ib->Use();
	if (m_vd)
		m_vd->Use();
}
//---------------------------------------------------------------------------------
void NlMesh::draw()
{
	Use();

	if (m_vb && m_ib)
		m_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_vb->m_elementCount, 0, m_ib->m_elementCount/3 );
}

//=================================================================================
NlMeshManager::~NlMeshManager()
{
	NlMeshArray::iterator it;
	for (it = m_meshes.begin(); it != m_meshes.end(); ++it)
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
NlMeshManager& NlMeshManager::GetInstance()
{
	static NlMeshManager instance;
	return instance;
}
//---------------------------------------------------------------------------------
NlMesh* NlMeshManager::GetMeshByName( NlRenderDevice* device,
	const char* objFileName, const char* submeshName )
{
	assert( device );

	NlMeshArray::iterator it;
	for (it = m_meshes.begin(); it != m_meshes.end(); ++it)
	{
		if (!(*it))
			continue;

		if ((*it)->m_name == submeshName)
			return (*it);
	}

	NlMesh *newMesh = new NlMesh( device );
	assert( newMesh );
	if (newMesh->LoadFromFile( objFileName, submeshName ) == true)
		m_meshes.push_back( newMesh );
	else
	{
		newMesh->destroy();
		delete newMesh;
		newMesh = NULL;
	}

	return newMesh;
}