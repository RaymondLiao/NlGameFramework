/*
  @file   NlCollider.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlCollider_h_
#define _NlCollider_h_

#include "prerequisite.h"
#include "NlColliderEvent.h"

//====================================================
class NlCollider
{
	friend class NlRigidBody;
	friend class NlClidEventManager;

public:
	NlCollider( NlRigidBody* attachedBody );	// need a rigid body
	NlCollider( NlRigidBody* attachedBody, const float3& locPos, const Quat& locRot );
	virtual ~NlCollider(void);

	void SetEventDelegate( NlClidEventDelegate* delegation ) {m_eventDele = delegation;}
	const NlRigidBody& GetAttachedRigidBody() const {return *m_attachedRigBody;}
	float3 GetGlobalPos();

protected:
	void PostColliderEvent( NlCollider* another, ENlClidEvent msg );
	void Draw();

protected:
	NxShape* m_pxShape;
	NlRigidBody* m_attachedRigBody;
	NlClidEventDelegate* m_eventDele;

	bool m_initialized;

	bool m_isTrigger;
	uint32	m_id;	// a rigid body may have one more colliders so we need id to mark this one
	NxMat34 m_locPose;

};

//====================================================
class NlBoxCollider :
	public NlCollider
{
public:
	NlBoxCollider( NlRigidBody* attachedBody );
	NlBoxCollider( NlRigidBody* attachedBody, const float3& locPos, const Quat& locRot );
	~NlBoxCollider();

	bool init( float3 sides, 
		uint16 mtrlIndex = 0, bool isTrigger = false );

};

//====================================================
class NlSphereCollider :
	public NlCollider
{
public:
	NlSphereCollider( NlRigidBody* attachedBody );
	NlSphereCollider( NlRigidBody* attachedBody, const float3& locPos, const Quat& locRot );
	~NlSphereCollider();

	bool init( float radius, 
		uint16 mtrlIndex = 0, bool isTrigger = false );

};

//====================================================
class NlCapsuleCollider :
	public NlCollider
{
public:
	NlCapsuleCollider( NlRigidBody* attachedBody );
	NlCapsuleCollider( NlRigidBody* attachedBody, const float3& locPos, const Quat& locRot );
	~NlCapsuleCollider();

	bool init( float radius, float height, 
		uint16 mtrlIndex = 0, bool isTrigger = false );

};

//====================================================
typedef struct _NlMeshClidData
{
	_NlMeshClidData()
	{
		_vCount = 0;
		_iCount = 0;
	}
	_NlMeshClidData& operator= ( const _NlMeshClidData& another )
	{
		_vertices = another._vertices;
		_indices = another._indices;
		_vCount = another._vCount;
		_iCount = another._iCount;

		return *this;
	}

	~_NlMeshClidData()
	{
		_vertices.clear();
		_indices.clear();
	}

	void clear()
	{
		_vertices.clear();
		_indices.clear();
		_vCount = 0;
		_iCount = 0;
	}

	std::vector<float3> _vertices;
	std::vector<uint32> _indices;
	uint32 _vCount;
	uint32 _iCount;

} NlMeshClidData;

class NlMeshCollider :
	public NlCollider
{
public:
	NlMeshCollider( NlRigidBody* attachedBody );
	NlMeshCollider( NlRigidBody* attachedBody, const float3& locPos, const Quat& locRot );
	~NlMeshCollider();

	bool init( NlMeshClidData& data,
		uint16 mtrlIndex = 0, bool isTrigger = false );

// used for loading----------------------------------
public:
	static bool LoadMeshFromFile( NlMeshClidData& data, 
		const char* objFileName, const char* submeshName = NULL );
	//static void loadMeshFromFile( NlMeshClidData& data,
	//	const char* fileName );

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

private:
	static bool LoadMeshFromData( NlMeshClidData& data,
		const char* pMeshData, const char* submeshName = NULL );
	static WORD AddVertex( uint32 key, float3* pVertex );
	static void ClearUp();

	// load-in data
	static std::vector<float3> vPos;
	static NlMeshClidData loadedData;

	static std::vector<CacheEntry*> vertexCache;
	static CacheEntry* nullCache;
//----------------------------------------------------

private:
	NxTriangleMeshDesc* m_meshDesc;	// used to cook collide mesh

};

#endif