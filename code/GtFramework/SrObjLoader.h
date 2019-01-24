/**
  @file SrObjLoader.h
  
  @author Kaiming

  更改日志 history
  ver:1.0
   
 */

#ifndef SrObjLoader_h__
#define SrObjLoader_h__

#include "MeshObject.h"
#include "GtFile.h"

class GtObjLoader
{

	struct CacheEntry
	{
		uint32 index;
		CacheEntry* pNext;
	};
public:
	GtObjLoader( IDirect3DDevice9* device );
	~GtObjLoader(void);

public:
	bool LoadOBJGeometryFromString( const char* strMeshData, CMeshs& primitives );
	bool LoadOBJGeometryFromFile( const char* filename, CMeshs& primitives );

private:
	WORD AddVertex( uint32 hash, SPt332* pVertex );
	bool IsMatIDExist( DWORD matID );
	void DeleteCache();
	void ClearData();
	void CreateMeshInternal(CMeshs& primitives );

	bool	m_bIsLoaded;

	std::vector<float3> Positions;
	std::vector<float2> TexCoords;
	std::vector<float3> Normals;

	std::vector<CacheEntry*> m_VertexCache;		// Hashtable cache for locating duplicate vertices
	std::vector<SPt332> m_Vertices;     // Filled and copied to the vertex buffer
	std::vector<uint32> m_Indices;				// Filled and copied to the index buffer
	std::vector<uint32> m_Attributes;			// Filled and copied to the attribute buffer

	CacheEntry* nullCache;

	IDirect3DDevice9* m_device;
};

#endif


