/*
  @file   NlGameObject.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlGameObject_h_
#define _NlGameObject_h_

#include "prerequisite.h"
#include "NlColliderEvent.h"

typedef struct _NlTransform
{
	_NlTransform()
	{
		_position = float3( 0 );
		_rotation = Quat::CreateIdentity();
		_scale = float3( 1 );
	}

	_NlTransform( float3 pos, Quat rot, float3 scale ):
		_position(pos), _rotation(rot), _scale(scale) {}

	_NlTransform( const _NlTransform& another ):
		_position(another._position), _rotation(another._rotation), _scale(another._scale) {}

	_NlTransform& operator= ( const _NlTransform& another )
	{
		_position = another._position;
		_rotation = another._rotation;
		_scale    = another._scale;

		return *this;
	}

	float3 _position;
	Quat   _rotation;
	float3 _scale;

} NlTransform;

//====================================================
class NlGameEnvironment;
class NlRigidBody;
class NlCollider;

class NlGameObjectBase :
	public NlClidEventDelegate
{
public:
	NlGameObjectBase( bool isStatic = false );
	NlGameObjectBase( NlTransform& transform, bool isStatic = false );
	virtual ~NlGameObjectBase();

	virtual bool init( NlGameEnvironment* gameEnv ) { return true; }
	virtual void update( NlGameEnvironment* gameEnv );
	virtual void destroy();

	void SetPos( const float3& pos );
	void SetRot( const Quat& rot );
	void SetRot(  float xAngle, float yAngle, float zAngle );	// angle in degree
	void SetScale( const float3& scale );
	void Mirror( const float3& n, float d );	// enable to static object

	const float3& GetPos() const;
	const Quat& GetRot() const;
	const float3& GetScale() const;
	const float44& GetModelMatrix();

	void SetParent( NlGameObjectBase* parent );

	void SetRigidBody( NlRigidBody* rigidbody, bool autoDestroy );
	NlRigidBody* GetRigidBody() {return m_rigidBody;}

public:
	std::string name;

protected:
	bool m_isStatic;
	NlGameObjectBase* m_parent;

	NlTransform m_transform;

	float44 m_TRSMat;		// translation, rotation, scaling 
	float44 m_otherTransMat;
	float44 m_modelMat;

// physics components
	NlRigidBody* m_rigidBody;
private:
	bool m_autoDestroyRB;	// whether destroy rigid body automatically

};

//====================================================
class NlRenderDevice;
class NlVertexBuffer;
class NlIndexBuffer;
class NlVertexDecl;

class NlDynamicShaderComb;
class CCamera;

class NlRenderableRaw :
	public NlGameObjectBase
{
public:
	NlRenderableRaw( bool isStatic = false );
	NlRenderableRaw( NlTransform& transform,  bool isStatic = false );
	virtual ~NlRenderableRaw();

	virtual bool init( NlGameEnvironment* gameEnv );
	virtual void Draw( NlGameEnvironment* gameEnv ) = 0;
	virtual void destroy();

protected:
	NlVertexBuffer*              m_vb;
	NlIndexBuffer*               m_ib;
	NlVertexDecl*				 m_vd;
	NlDynamicShaderComb*         m_shader;

	// enable physics components temporarily
private:
	void SetRigidBody();
	NlRigidBody* GetRigidBody();

};

//====================================================
class NlMesh;
class NlMaterial;

class NlRenderble :
	public NlGameObjectBase
{
public:
	NlRenderble( bool isStatic = false );
	NlRenderble( NlTransform& transform, bool isStatic = false );
	virtual ~NlRenderble();

	bool LoadFromFile( NlRenderDevice* device,
		const char* objFileName, const char* mtrlFileName, const char* submeshName );
	// warn: will not destroy old instance
	void SetMesh( NlMesh* mesh ) { m_mesh = mesh; }
	void SetMaterial( NlMaterial* mtrl ) { m_material = mtrl; }

	// use default material
	virtual void Draw( NlGameEnvironment* gameEnv );
	virtual void destroy() {NlGameObjectBase::destroy();}

protected:
	NlMesh* m_mesh;
	NlMaterial *m_material;

};

//====================================================
class NlCombRenderble :
	public NlGameObjectBase
{
public:
	NlCombRenderble( bool isStatic = false );
	NlCombRenderble( NlTransform& transform, bool isStatic = false );
	virtual ~NlCombRenderble();

	bool LoadFromFile( NlRenderDevice* device,
		const char* objFileName, const char* mtrlFileName );

	// note: the component added will be deleted when NlCombRenderble::onDestroy is calleds
	void AddSubRenderble( NlRenderble* sub );

	virtual void update( NlGameEnvironment* gameEnv );
	virtual void Draw( NlGameEnvironment* gameEnv );
	virtual void destroy();

	void SetPos( const float3& pos );
	void SetRot( const Quat& rot );

protected:
	std::map<std::string, NlRenderble*> m_subRenderbles;

};

#endif