#include "SrObjLoader.h"
#include <sstream>


GtObjLoader::GtObjLoader(IDirect3DDevice9* device):m_device(device)
{
	nullCache = NULL;
}


GtObjLoader::~GtObjLoader(void)
{
}


//////////////////////////////////////////////////////////////////////////
bool GtObjLoader::IsMatIDExist( DWORD matID )
{
	int num = m_Attributes.size();
	for(int i=0; i < num; i++)
	{
		if (m_Attributes[i] == matID)
		{
			return true;
		}
	}

	return false;
}
//--------------------------------------------------------------------------------------
WORD GtObjLoader::AddVertex( uint32 hash, SPt332* pVertex )
{
	// If this vertex doesn't already exist in the Vertices list, create a new entry.
	// Add the index of the vertex to the Indices list.
	bool bFoundInList = false;
	WORD index = 0;

	// Since it's very slow to check every element in the vertex list, a hashtable stores
	// vertex indices according to the vertex position's index as reported by the OBJ file
	if( ( uint32 )m_VertexCache.size() > hash )
	{
		CacheEntry* pEntry = m_VertexCache[hash];
		while( pEntry != NULL )
		{
			SPt332* pCacheVertex = m_Vertices.data() + pEntry->index;

			// If this vertex is identical to the vertex already in the list, simply
			// point the index buffer to the existing vertex
			if( 0 == memcmp( pVertex, pCacheVertex, sizeof( SPt332 ) ) )
			{
				bFoundInList = true;
				index = pEntry->index;
				break;
			}

			pEntry = pEntry->pNext;
		}
	}

	// Vertex was not found in the list. Create a new entry, both within the Vertices list
	// and also within the hashtable cache
	if( !bFoundInList )
	{
		// Add to the Vertices list
		index = m_Vertices.size();
		m_Vertices.push_back( *pVertex );

		// Add this to the hashtable
		CacheEntry* pNewEntry = new CacheEntry;
		if( pNewEntry == NULL )
			return 0;

		pNewEntry->index = index;
		pNewEntry->pNext = NULL;

		// Grow the cache if needed
		if ( ( uint32 )m_VertexCache.size() <= hash )
		{
			m_VertexCache.resize( hash + 1, nullCache );
		}
// 		while( ( uint32 )m_VertexCache.size() <= hash )
// 		{
// 			m_VertexCache.push_back( NULL );
// 		}

		// Add to the end of the linked list
		CacheEntry* pCurEntry = m_VertexCache[hash];
		if( pCurEntry == NULL )
		{
			// This is the head element
			m_VertexCache[hash] = pNewEntry;
		}
		else
		{
			// Find the tail
			while( pCurEntry->pNext != NULL )
			{
				pCurEntry = pCurEntry->pNext;
			}

			pCurEntry->pNext = pNewEntry;
		}
	}

	return index;
}


//--------------------------------------------------------------------------------------
void GtObjLoader::DeleteCache()
{
	// Iterate through all the elements in the cache and subsequent linked lists
	for( uint32 i = 0; i < m_VertexCache.size(); i++ )
	{
		CacheEntry* pEntry = m_VertexCache[i];
		while( pEntry != NULL )
		{
			CacheEntry* pNext = pEntry->pNext;
			if (pEntry)
			{
				delete pEntry;
			}
			pEntry = pNext;
		}
	}

	m_VertexCache.clear();
}


bool GtObjLoader::LoadOBJGeometryFromFile( const char* filename, CMeshs& primitives )
{
	bool ret = false;
	GtFile file( filename );
	file.Open();
	if (file.IsOpen())
	{
		ret = LoadOBJGeometryFromString( file.Data(), primitives );
	}
	file.Close();

	return ret;
}

//--------------------------------------------------------------------------------------
bool GtObjLoader::LoadOBJGeometryFromString( const char* pMeshData, CMeshs& primitives )
{
	// Find the file
	// 找文件

	// File input
	char strCommand[256] = {0};
	std::stringstream InFile( pMeshData );
	if( !InFile )
		return S_FALSE;

	std::string currMtlName;

	int vertexIndexStart = 0;
	int normalIndexStart = 0;
	int texcoordIndexStart = 0;

	bool flushFace = false;

	for(; ; )
	{
		InFile >> strCommand;
		if( !InFile )
			break;

		if( 0 == strcmp( strCommand, ("#") ) )
		{
			// Comment
		}
		else if( 0 == strcmp( strCommand, ("g") ) )
		{
			// 物体
			flushFace = true;
		}
		else if( 0 == strcmp( strCommand, ("usemtl") ) )
		{
			// 物体材质
			InFile >> currMtlName;
		}
		else if( 0 == strcmp( strCommand, ("mtllib") ) )
		{
			// 物体材质
		}
		else if( 0 == strcmp( strCommand, ("v") ) )
		{
			if (flushFace)
			{
				flushFace = false;

				// CreateMesh
				CreateMeshInternal(primitives);			
			}

			// Vertex Position
			float x, y, z;
			InFile >> x >> y >> z;

			Positions.push_back( float3( x, y, z ) );

			// for3DsMax, 加入点列，随后方便走面的时候，输入normal和tc
		}
		else if( 0 == strcmp( strCommand, ("vt") ) )
		{
			// Vertex TexCoord
			float u, v;
			InFile >> u >> v;
			// for3DsMax, 加入点列，随后方便走面的时候，输入normal和tc
			TexCoords.push_back( float2( u, 1-v ) );
		}
		else if( 0 == strcmp( strCommand, ("vn"))  )
		{
			// Vertex Normal
			float x, y, z;
			InFile >> x >> y >> z;
			Normals.push_back( float3( x, y, z) );
		}
		else if( 0 == strcmp( strCommand, ("f") ) )
		{
			// Face
			int iPosition, iTexCoord, iNormal;
			SPt332 vertex;
			uint16	dwIndex[3];
			for( uint32 iFace = 0; iFace < 3; iFace++ )
			{
				memset( &vertex, 0, sizeof( SPt332 ) );

				// OBJ format uses 1-based arrays
				InFile >> iPosition;
				iPosition = abs(iPosition);
				vertex.pos = Positions[ iPosition - 1 ];

				if( '/' == InFile.peek() )
				{
					InFile.ignore();

					if( '/' != InFile.peek() )
					{
						// Optional texture coordinate
						InFile >> iTexCoord;
						iTexCoord = abs(iTexCoord);
						vertex.texcoord = TexCoords[ iTexCoord - 1];

					}

					if( '/' == InFile.peek() )
					{
						InFile.ignore();

						// Optional vertex normal
						InFile >> iNormal;
						iNormal = abs(iNormal);
						vertex.normal = Normals[ iNormal - 1 ];
					}
				}

				dwIndex[iFace] = AddVertex( iPosition, &vertex );			
			}
			// gkEngine, Add Indice
			m_Indices.push_back( dwIndex[0] );
			m_Indices.push_back( dwIndex[1] );
			m_Indices.push_back( dwIndex[2] );
		}
		else
		{
			// Unimplemented or unrecognized command
		}

		InFile.ignore( 1000, '\n' );
	}


	if (flushFace)
	{
		flushFace = false;

		// CreateMesh
		CreateMeshInternal(primitives);		
	}

	return S_OK;
}

void GtObjLoader::ClearData()
{
	m_Vertices.clear();     // Filled and copied to the vertex buffer
	m_Indices.clear();       // Filled and copied to the index buffer
	m_Attributes.clear();    // Filled and copied to the attribute buffer
}


void GtObjLoader::CreateMeshInternal(CMeshs& primitives )
{
	// Cleanup
	DeleteCache();
	m_bIsLoaded = true;

	if (m_Vertices.empty())
	{
		return;
	}

	CMesh* newMesh = new CMesh( m_device );
	newMesh->m_vertexBuffer = new CVertexBuffer( m_device, sizeof(SPt332) * m_Vertices.size() , sizeof(SPt332) );
	newMesh->m_indexBuffer = new CIndexBuffer( m_device, m_Indices.size() );

	newMesh->m_vertexBuffer->Update( m_Vertices.data(), sizeof(SPt332) * m_Vertices.size() );
	newMesh->m_indexBuffer->Update( m_Indices.data(), sizeof(uint32) * m_Indices.size() );

	newMesh->m_vertexDecl = new CVertexDecl( m_device );

	D3DVERTEXELEMENT9 element;
	element.Stream = 0;
	element.Offset = 0;
	element.Type = D3DDECLTYPE_FLOAT3;
	element.Method = D3DDECLMETHOD_DEFAULT;
	element.Usage = D3DDECLUSAGE_POSITION;
	element.UsageIndex = 0;
	newMesh->m_vertexDecl->AddElement( element );

	element.Stream = 0;
	element.Offset = 12;
	element.Type = D3DDECLTYPE_FLOAT3;
	element.Method = D3DDECLMETHOD_DEFAULT;
	element.Usage = D3DDECLUSAGE_TEXCOORD;
	element.UsageIndex = 0;
	newMesh->m_vertexDecl->AddElement( element );

	element.Stream = 0;
	element.Offset = 24;
	element.Type = D3DDECLTYPE_FLOAT2;
	element.Method = D3DDECLMETHOD_DEFAULT;
	element.Usage = D3DDECLUSAGE_TEXCOORD;
	element.UsageIndex = 1;
	newMesh->m_vertexDecl->AddElement( element );

	element.Stream = 0xff;
	element.Offset = 0;
	element.Type = D3DDECLTYPE_UNUSED;
	element.Method = 0;
	element.Usage = 0;
	element.UsageIndex = 0;
	newMesh->m_vertexDecl->AddElement( element );

	primitives.push_back(newMesh);

	ClearData();

}

