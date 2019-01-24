#include "NlGameObject.h"
#include "NlGameEnvironment.h"

#include "nl_render_header.h"
#include "nl_physics_header.h"

#include "GtFile.h"
#include <sstream>

//=================================================================================
NlGameObjectBase::NlGameObjectBase( bool isStatic /*= false */ )
{
	m_isStatic = isStatic;
	m_parent = NULL;

	m_TRSMat = float44::CreateIdentity();
	m_otherTransMat = float44::CreateIdentity();
	m_modelMat = float44::CreateIdentity();

	m_rigidBody = NULL;
	m_autoDestroyRB = false;
}
//---------------------------------------------------------------------------------
NlGameObjectBase::NlGameObjectBase( NlTransform& transform, bool isStatic /*= false */  ):
	m_isStatic(isStatic), m_transform(transform)
{
	m_parent = NULL;

	m_TRSMat = float44::CreateIdentity();
	m_otherTransMat = float44::CreateIdentity();
	m_modelMat = float44::CreateIdentity();

	m_rigidBody = NULL;
	m_autoDestroyRB = false;
}
//---------------------------------------------------------------------------------
NlGameObjectBase::~NlGameObjectBase()
{
}
//---------------------------------------------------------------------------------
void NlGameObjectBase::update( NlGameEnvironment* gameEnv )
{
	// update pose
	if (m_rigidBody)
	{
		m_rigidBody->GetGlobalPose( m_transform._position, m_transform._rotation );
	}
}
//---------------------------------------------------------------------------------
void NlGameObjectBase::destroy()
{
	if (m_rigidBody && m_autoDestroyRB)
	{
		m_rigidBody->destroy();
		delete m_rigidBody;
		m_rigidBody = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlGameObjectBase::SetPos( const float3& pos )
{
	if (m_isStatic)
		return;

	m_transform._position = pos;

	// update rigid body's global pose
	if (m_rigidBody)
		m_rigidBody->SetGlobalPose( m_transform._position, m_transform._rotation );
}
//---------------------------------------------------------------------------------
void NlGameObjectBase::SetRot( const Quat& rot )
{
	if (m_isStatic)
		return;

	m_transform._rotation = rot;

	if (m_rigidBody)
		m_rigidBody->SetGlobalPose( m_transform._position, m_transform._rotation );
}
//---------------------------------------------------------------------------------
void NlGameObjectBase::SetRot( float xAngle, float yAngle, float zAngle )
{
	if (m_isStatic)
		return;

	Quat rot_x = Quat::CreateRotationX( DEG2RAD(xAngle) );
	Quat rot_y = Quat::CreateRotationY( DEG2RAD(yAngle) );
	Quat rot_z = Quat::CreateRotationZ( DEG2RAD(zAngle) );

	m_transform._rotation = rot_x * rot_y * rot_z;

	if (m_rigidBody)
		m_rigidBody->SetGlobalPose( m_transform._position, m_transform._rotation );
}
//---------------------------------------------------------------------------------
void NlGameObjectBase::SetScale( const float3& scale )
{
	m_transform._scale = scale;
}
//---------------------------------------------------------------------------------
void NlGameObjectBase::Mirror( const float3& n, float d )
{
	float44 mirror_mat( 
		-2*n.x*n.x+1, -2*n.y*n.x,   -2*n.z*n.x,   0,
		-2*n.x*n.y,   -2*n.y*n.y+1, -2*n.z*n.y,   0,
		-2*n.x*n.z,   -2*n.y*n.z,   -2*n.z*n.z+1, 0,
		-2*n.x*d,     -2*n.y*d,     -2*n.z*d,     1 );

	m_otherTransMat *= mirror_mat;
}
//---------------------------------------------------------------------------------
const float3& NlGameObjectBase::GetPos() const
{
	return m_transform._position;
}
//---------------------------------------------------------------------------------
const Quat& NlGameObjectBase::GetRot() const
{
	return m_transform._rotation;
}
//---------------------------------------------------------------------------------
const float3& NlGameObjectBase::GetScale() const
{
	return m_transform._scale;
}
//---------------------------------------------------------------------------------
const float44& NlGameObjectBase::GetModelMatrix()
{
	m_TRSMat = float44::CreateIdentity();
	m_TRSMat.Set( m_transform._scale, m_transform._rotation, m_transform._position );
	m_modelMat = m_otherTransMat*m_TRSMat;
	return m_modelMat;
}
//---------------------------------------------------------------------------------
void NlGameObjectBase::SetParent( NlGameObjectBase* parent )
{
	if (parent == this)
		return;

	m_parent = parent;
}
//---------------------------------------------------------------------------------
void NlGameObjectBase::SetRigidBody( NlRigidBody* rigidbody, bool autoDestroy )
{
	if (rigidbody == NULL)
		return;

	if (m_rigidBody && m_autoDestroyRB)
	{
		m_rigidBody->destroy();
		delete m_rigidBody;
		m_rigidBody = NULL;
	}
	m_autoDestroyRB = autoDestroy;

	NlTransform bodyTransform = m_transform;
	if (m_parent != NULL)
	{
		bodyTransform._position += m_parent->GetPos();
		bodyTransform._rotation *= m_parent->GetRot();
	}
	m_rigidBody = rigidbody;
	m_rigidBody->SetGlobalPose( bodyTransform._position, bodyTransform._rotation );
	m_rigidBody->SetOwner( this );

}

//=================================================================================
NlRenderableRaw::NlRenderableRaw( bool isStatic /*= false */ ):
	NlGameObjectBase( isStatic )
{
	m_vb = NULL;
	m_ib = NULL;
	m_vd = NULL;

	m_shader = NULL;

}
//---------------------------------------------------------------------------------
NlRenderableRaw::NlRenderableRaw( NlTransform& transform, bool isStatic /*= false */ ):
	NlGameObjectBase( transform, isStatic )
{
	m_vb = NULL;
	m_ib = NULL;
	m_vd = NULL;

	m_shader = NULL;
}
//---------------------------------------------------------------------------------
NlRenderableRaw::~NlRenderableRaw()
{
}
//---------------------------------------------------------------------------------
bool NlRenderableRaw::init( NlGameEnvironment* gameEnv )
{
	// clear up
	destroy();

	assert( gameEnv );
	NlRenderDevice* renderDevice = gameEnv->GetRenderDevice();

	// create vb & ib & vd
	m_vb = new NlVertexBuffer( renderDevice );
	assert( m_vb );
	m_ib = new NlIndexBuffer( renderDevice );
	assert( m_ib );
	m_vd = new NlVertexDecl( renderDevice );
	assert( m_vd );

	return true;
}
//---------------------------------------------------------------------------------
void NlRenderableRaw::destroy()
{
	NlGameObjectBase::destroy();

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
	if (m_shader)
	{
		m_shader->destroy();
		delete m_shader;
		m_shader = NULL;
	}
}

//=================================================================================
NlRenderble::NlRenderble( bool isStatic /*= false */ ):
	NlGameObjectBase( isStatic )
{
	m_mesh = NULL;
	m_material = NULL;
}
//---------------------------------------------------------------------------------
NlRenderble::NlRenderble( NlTransform& transform, bool isStatic /*= false */ ):
	NlGameObjectBase( transform, isStatic )
{
	m_mesh = NULL;
	m_material = NULL;
}
//---------------------------------------------------------------------------------
NlRenderble::~NlRenderble()
{
}
//---------------------------------------------------------------------------------
void NlRenderble::Draw( NlGameEnvironment* gameEnv )
{
	if (!m_mesh || !m_material)
		return;

	assert( gameEnv );
	NlDefShaderParam* shaderParams = m_material->GetShaderParamsToUpdate();
	assert( shaderParams );

	CCamera* camera = gameEnv->GetMainCamera();
	if (!camera)
		return;

	const float44& world_mat = 
		(m_parent == NULL || m_rigidBody != NULL) ? GetModelMatrix() : GetModelMatrix()*m_parent->GetModelMatrix();

	if (m_parent != NULL && m_rigidBody != NULL)
	{
		float44 parentScaleMat = float44::CreateIdentity();
		parentScaleMat.Set( m_parent->GetScale(), Quat::CreateIdentity(), float3(0) );
		
		shaderParams->wvp_mat = parentScaleMat * world_mat * camera->GetViewProjectionMatrix();
		shaderParams->world_mat = parentScaleMat * world_mat;
	}
	else
	{
		shaderParams->wvp_mat = world_mat * camera->GetViewProjectionMatrix();
		shaderParams->world_mat = world_mat;
	}
	
	shaderParams->lightDir = gameEnv->m_lightDir;
	shaderParams->camPos = camera->GetPosition();
	
	m_material->Use();
	m_mesh->draw();

	if( m_rigidBody && gameEnv->IfDrawForDebug() )
		m_rigidBody->Draw();
}
//---------------------------------------------------------------------------------
bool NlRenderble::LoadFromFile( NlRenderDevice* device, const char* objFileName, const char* mtrlFileName, const char* submeshName )
{
	destroy();

	assert( device );
	m_mesh = NlMeshManager::GetInstance().GetMeshByName( device, objFileName, submeshName );
	m_material = NlMtrlManager::GetInstance().GetMtrlByName( device, mtrlFileName, m_mesh->m_mtrlName.c_str() );

	if (m_mesh && m_material)
		return true;
	else
		return false;
}

//=================================================================================
NlCombRenderble::NlCombRenderble( bool isStatic /*= false */ ):
	NlGameObjectBase( isStatic )
{
}
//---------------------------------------------------------------------------------
NlCombRenderble::NlCombRenderble( NlTransform& transform, bool isStatic /*= false */ ):
	NlGameObjectBase( transform, isStatic )
{
}
//---------------------------------------------------------------------------------
NlCombRenderble::~NlCombRenderble()
{
}
//---------------------------------------------------------------------------------
bool NlCombRenderble::LoadFromFile( NlRenderDevice* device,
	const char* objFileName, const char* mtrlFileName )
{
	destroy();

	GtFile file( ASSETS_PATH+objFileName );
	file.Open();
	if (file.IsOpen())
	{
		char strCommand[256] = {0};
		std::stringstream infile( file.Data() );
		if (!infile)
		{
			MessageBox( NULL, "load combined-renderble failed", "error", MB_OK );
			return false;
		}

		NlMtrlLoadingParam param;

		for (; ;)
		{
			infile >> strCommand;
			if (!infile)
				break;

			if (0 == strcmp( strCommand, ("g") ))
			{
				std::string submeshName;
				infile >> submeshName;

				NlRenderble* subRender = new NlRenderble( device );
				//subRender->onInit();
				if (subRender->LoadFromFile( device, objFileName, mtrlFileName, submeshName.c_str() ))
				{
					subRender->name = submeshName;
					subRender->SetParent( this );
					m_subRenderbles[subRender->name] = subRender;
				}
				else
				{
					subRender->destroy();
					delete subRender;
					subRender = NULL;
					MessageBox( NULL, "load combined-renderble failed", "error", MB_OK );
					return false;
				}
			}
			infile.ignore( 1000, '\n' );
		}

		file.Close();
	}
	else
	{
		MessageBox( NULL, "load combined-renderble failed", "error", MB_OK );
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------------
void NlCombRenderble::AddSubRenderble( NlRenderble* sub )
{
	assert( sub );
	m_subRenderbles[sub->name] = sub;
}
//---------------------------------------------------------------------------------
void NlCombRenderble::update( NlGameEnvironment* gameEnv )
{
	std::map<std::string, NlRenderble*>::iterator it;
	for (it = m_subRenderbles.begin(); it != m_subRenderbles.end(); ++it)
	{
		if (it->second)
			it->second->update( gameEnv );
	}

}
//---------------------------------------------------------------------------------
void NlCombRenderble::Draw( NlGameEnvironment* gameEnv )
{
	assert( gameEnv );

	std::map<std::string, NlRenderble*>::iterator it;
	for (it = m_subRenderbles.begin(); it != m_subRenderbles.end(); ++it)
	{
		if (it->second)
			it->second->Draw( gameEnv );
	}

	if (m_rigidBody && gameEnv->IfDrawForDebug())
		m_rigidBody->Draw();

}
//---------------------------------------------------------------------------------
void NlCombRenderble::destroy()
{
	NlGameObjectBase::destroy();

	std::map<std::string, NlRenderble*>::iterator it;
	for (it = m_subRenderbles.begin(); it != m_subRenderbles.end(); ++it)
	{
		if (it->second)
		{
			(it->second)->destroy();
			delete (it->second);
			(it->second) = NULL;
		}
	}

	m_subRenderbles.clear();

}
//---------------------------------------------------------------------------------
void NlCombRenderble::SetPos( const float3& pos )
{
	NlGameObjectBase::SetPos( pos );

	std::map<std::string, NlRenderble*>::iterator it;
	for (it = m_subRenderbles.begin(); it != m_subRenderbles.end(); ++it)
	{
		if (it->second && it->second->GetRigidBody())
		{
			// update sub renderables' rigid body's global pose
			it->second->GetRigidBody()->SetGlobalPose( m_transform._position+it->second->GetPos(), 
				m_transform._rotation*it->second->GetRot() );
		}
	}

}
//---------------------------------------------------------------------------------
// note: there is a transform-order problem so don't call this function
void NlCombRenderble::SetRot( const Quat& rot )
{
	NlGameObjectBase::SetRot( rot );

	std::map<std::string, NlRenderble*>::iterator it;
	for (it = m_subRenderbles.begin(); it != m_subRenderbles.end(); ++it)
	{
		if (it->second && it->second->GetRigidBody())
		{
			// update sub renderables' rigid body's global pose
			it->second->GetRigidBody()->SetGlobalPose( m_transform._position+it->second->GetPos(), 
				m_transform._rotation*it->second->GetRot() );
		}
	}

}