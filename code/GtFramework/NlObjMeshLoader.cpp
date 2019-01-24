#include "NlObjMeshLoader.h"
#include "GtFile.h"
#include <sstream>

//=================================================================================
NlObjMeshLoaderBase::NlObjMeshLoaderBase( void )
{
	nullCache = NULL;
}
//---------------------------------------------------------------------------------
NlObjMeshLoaderBase::~NlObjMeshLoaderBase( void )
{
	ClearData();
	DeleteCache();
}
//---------------------------------------------------------------------------------
bool NlObjMeshLoaderBase::LoadObjGeometryFromFile( const char* fileName, const char* submeshName )
{
	bool res = false;
	GtFile file( fileName );
	file.Open();
	if (file.IsOpen())
	{
		res = LoadObjGeometryFromData( file.Data(), submeshName );
	}
	file.Close();

	return res;
}
//---------------------------------------------------------------------------------
bool NlObjMeshLoaderBase::LoadObjGeometryFromData( const char* pMeshData, const char* submeshName )
{
	char strCommand[256] = {0};
	std::stringstream inFile( pMeshData );
	if (!inFile)
		return false;

	std::string currMtrlName;
	std::string currMeshName;

	int vertexIndexStart = 0;
	int normalIndexStart = 0;
	int texcoordIndexStart = 0;

	bool flushFace = false;

	for (; ;)
	{
		inFile >> strCommand;
		if (!inFile)
			break;

		if (0 == strcmp( strCommand, ("#") ))
		{
		}
		else if (0 == strcmp( strCommand, ("g") ))		// group, namely, object
		{
			inFile >> currMeshName;
			flushFace = true;
		}
		else if (0 == strcmp(strCommand, ("mtllib") ))	// matched material-file name
		{

		}
		else if (0 == strcmp( strCommand, ("usemtl") ))	// group's material
		{
			inFile >> currMtrlName;
		}
		else if (0 == strcmp( strCommand, ("v") ))		// vertex position
		{
			if (flushFace)		// finish getting mesh data
			{
				flushFace = false;

				if (currMeshName == submeshName)
				{
					// create mesh
					m_materialName = currMtrlName;
					CreateMeshInternal();
					return true;
				}
				else
				{
					ClearData();
					DeleteCache();
				}
			}

			// get v_pos info
			float x, y, z;
			inFile >> x >> y >> z;
			_positions.push_back( float3(x, y, z) ); 
		}
		else if (0 == strcmp( strCommand, ("vt") ))		// vertex texcoord
		{
			// get v_texcoord info
			float u, v;
			inFile >> u >> v;
			_texcoords.push_back( float2(u, 1-v) );
		}
		else if (0 == strcmp( strCommand, ("vn") ))		// vertex normal
		{
			// get v_normal info
			float x, y, z;
			inFile >> x >> y >> z;
			_normals.push_back( float3(x, y, z) );
		}
		else if (0 == strcmp( strCommand, ("f") ))		// triangle
		{
			// get triangle info
			int posIndex, tcdIndex, normIndex;
			NlMeshVertex vertex;
			uint32 vIndex[3];
			for (uint32 v = 0; v < 3; ++v)
			{
				memset( &vertex, 0, sizeof(NlMeshVertex) );

				// get pos index
				inFile >> posIndex;
				posIndex = abs( posIndex );
				vertex.pos = _positions[posIndex - 1];	// OBJ format uses 1-based arrays
				
				if ('/' == inFile.peek())
				{
					inFile.ignore();

					if ('/' != inFile.peek())
					{
						// get texcoord index
						inFile >> tcdIndex;
						tcdIndex = abs( tcdIndex );
						vertex.texcoord = _texcoords[tcdIndex - 1];
					}
					
					if ('/' == inFile.peek())
					{
						inFile.ignore();

						// get normal index
						inFile >> normIndex;
						normIndex = abs( normIndex );
						vertex.normal = _normals[normIndex - 1];
					}
				}

				// create a vertex and push it into vertex-list(for vb) if it hasn't been created
				vIndex[v] = AddVertex( posIndex, &vertex );
			}

			// push the vertex index into index-list(for ib)
			m_indices.push_back( vIndex[0] );
			m_indices.push_back( vIndex[1] );
			m_indices.push_back( vIndex[2] );
		}
		else
		{
			// unimplemented or unrecognized command
		}

		inFile.ignore( 1000, '\n' );	// swallow the whole line
	}

	// deal with the last group
	if (flushFace)
	{
		flushFace = false;

		if (currMeshName == submeshName)
		{
			m_materialName = currMtrlName;
			CreateMeshInternal();
			return true;
		}
		else
		{
			ClearData();
			DeleteCache();
		}
	}

	return false;
}
//---------------------------------------------------------------------------------
WORD NlObjMeshLoaderBase::AddVertex( uint32 key, NlMeshVertex* pVertex )
{
	// if this vertex doesn't already exist in the vertex-list, create a new entry
	// return the index of the vertex
	bool foundInList = false;
	WORD index = 0;

	// since it's very slow to check every element in the vertex-list, a hash-table stores
	// vertices indices according to the vertex position's index as reported by the OBJ file
	if ((uint32)m_vertexCache.size() > key)
	{
		CacheEntry *pEntry = m_vertexCache[key];
		while (pEntry != NULL)
		{
			NlMeshVertex *pCacheVertex = m_vertices.data() + pEntry->index;

			// if this vertex is identical to the vertex already in the vertex-list, simply
			// point the index buffer to the existing vertex
			if (0 == memcmp( pVertex, pCacheVertex, sizeof(NlMeshVertex) ))
			{
				foundInList = true;
				index = pEntry->index;
				break;
			}

			pEntry = pEntry->pNext;
		}
	}

	// vertex was not found in the list
	// create a new entry, both within the vertex-list and also within the hash-table cache
	if (!foundInList)
	{
		// add to the vertex-list------------------
		index = m_vertices.size();
		m_vertices.push_back( *pVertex );

		// add to the hash-table-------------------
		CacheEntry *pNewEntry = new CacheEntry;
		assert( pNewEntry );

		pNewEntry->index = index;
		pNewEntry->pNext = NULL;

		// grow the cache if needed
		if ((uint32)m_vertexCache.size() <= key)
		{
			m_vertexCache.resize( key+1, nullCache );
		}

		// add to the end of the hash-table
		CacheEntry *pCurEntry = m_vertexCache[key];
		if (pCurEntry == NULL)	// it's the head element
		{
			m_vertexCache[key] = pNewEntry;
		}
		else
		{
			// find the tail
			while(pCurEntry->pNext != NULL)
			{
				pCurEntry = pCurEntry->pNext;
			}

			pCurEntry->pNext = pNewEntry;
		}
	}
	
	return index;
}
//---------------------------------------------------------------------------------
bool NlObjMeshLoaderBase::IsMtrlIDExist( DWORD mtrlID )
{
	int num = m_attributes.size();
	for (int i = 0; i < num; ++i)
	{
		if (m_attributes[i] == mtrlID)
			return true;
	}

	return false;
}
//---------------------------------------------------------------------------------
void NlObjMeshLoaderBase::DeleteCache()
{
	// iterate through all the elements in the hash-table and clean them up
	for (uint32 i = 0; i < m_vertexCache.size(); ++i)
	{
		CacheEntry *pEntry = m_vertexCache[i];
		while(pEntry != NULL)
		{
			CacheEntry *pNext = pEntry->pNext;
			delete pEntry;
			pEntry = pNext;
		}
	}

	m_vertexCache.clear();
}
//---------------------------------------------------------------------------------
void NlObjMeshLoaderBase::ClearData()
{
	m_vertices.clear();
	m_indices.clear();
	m_attributes.clear();
}

//=================================================================================
NlObjMeshLoader::NlObjMeshLoader( NlRenderDevice* device )
{
	m_device = device;

	m_vb = NULL;
	m_ib = NULL;
	m_vd = NULL;
}

NlObjMeshLoader::~NlObjMeshLoader( void )
{

}

void NlObjMeshLoader::CreateMeshInternal()
{
	if (m_vertices.empty())
		return;

	m_vb = new NlVertexBuffer( m_device );
	assert( m_vb );
	m_vb->init( sizeof(NlMeshVertex), m_vertices.size() );

	m_ib = new NlIndexBuffer( m_device );
	assert( m_ib );
	m_ib->init( sizeof(uint32), m_indices.size() );

	m_vb->update( m_vertices.data(), m_vertices.size()*sizeof(NlMeshVertex) );
	m_ib->update( m_indices.data(), m_indices.size()*sizeof(uint32) );

	m_vd = new NlVertexDecl( m_device );
	assert( m_vd );
	VertexElemArray elements;
	{
		NlVertexElem elem( 0, eVEU_Position, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 0, eVEU_Normal, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 0, eVEU_Texcoord, 0 );
		elements.push_back( elem );
	}
	m_vd->init( elements );
}

std::string NlObjMeshLoader::GetMaterialName()
{
	return m_materialName;
}