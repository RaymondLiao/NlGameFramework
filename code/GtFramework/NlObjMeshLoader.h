#pragma once

/*
  @file   NlObjMeshLoader.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlObjMeshLoader_h_
#define _NlObjMeshLoader_h_

#include "nl_render_header.h"

typedef NlVertexPNT NlMeshVertex;
//====================================================
class NlObjMeshLoaderBase
{
public:
	NlObjMeshLoaderBase(void);
	virtual ~NlObjMeshLoaderBase(void);

	// hash-table's element
	struct CacheEntry
	{
		uint32 index;
		CacheEntry* pNext;

		CacheEntry()
		{
			index = 0;
			pNext = 0;
		}
	};

public:
	bool LoadObjGeometryFromFile( const char* fileName, const char* submeshName );
	bool LoadObjGeometryFromData( const char* pMeshData, const char* submeshName );

protected:
	WORD AddVertex( uint32 key, NlMeshVertex* pVertex );
	bool IsMtrlIDExist( DWORD mtrlID );
	void DeleteCache();
	void ClearData();

	virtual void CreateMeshInternal() = 0;

protected:
	bool m_bIsLoaded;

	std::vector<float3> _positions;
	std::vector<float2> _texcoords;
	std::vector<float3> _normals;

	std::vector<CacheEntry*>	m_vertexCache;	// hashtable cache for locating duplicate vertices
	std::vector<NlMeshVertex>	m_vertices;		// filled and copied to the vertex buffer
	std::vector<uint32>			m_indices;		// filled and copied to the index buffer
	std::vector<uint32>			m_attributes;	// filled and copied to the attribute buffer

	CacheEntry* nullCache;		// a hash-table stores the index of the existed vertex(key is the vPos index)
	std::string m_materialName;

};

//====================================================
class NlObjMeshLoader :
	public NlObjMeshLoaderBase
{
public:
	NlObjMeshLoader( NlRenderDevice* device );
	~NlObjMeshLoader(void);

	virtual void CreateMeshInternal();
	std::string GetMaterialName();

	NlRenderDevice* m_device;
	NlVertexBuffer* m_vb;
	NlIndexBuffer* m_ib;
	NlVertexDecl* m_vd;
};

#endif