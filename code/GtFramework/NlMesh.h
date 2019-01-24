/*
  @file   NlMesh.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlMesh_h_
#define _NlMesh_h_

#include "prerequisite.h"

//====================================================
class NlRenderDevice;
class NlVertexBuffer;
class NlIndexBuffer;
class NlVertexDecl;

class NlMesh
{
public:
	NlMesh( NlRenderDevice* device );
	~NlMesh(void);

	bool LoadFromFile( const char* objFileName, const char* submeshName );
	void destroy();
	void draw();

private:
	void Use();

public:
	std::string m_name;
	std::string m_mtrlName;

private:
	NlRenderDevice* m_device;
	NlVertexBuffer* m_vb;
	NlIndexBuffer*  m_ib;
	NlVertexDecl*	m_vd;

};

typedef std::vector<NlMesh*> NlMeshArray;

// Singleton ====================================================
class NlMeshManager
{
public:
	~NlMeshManager();

	static NlMeshManager& GetInstance();

	// if the mesh doesn't exist, create it; else, return it directly
	NlMesh* GetMeshByName( NlRenderDevice* device,
		 const char* objFileName, const char* submeshName );

private:
	NlMeshManager() {}
	NlMeshManager( const NlMeshManager& another ) {}
	NlMeshManager& operator= ( const NlMeshManager& another ) {}

private:
	NlMeshArray m_meshes;

};

#endif