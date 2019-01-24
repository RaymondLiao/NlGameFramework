#include "NlPxPicker.h"
#include "NlGameEnvironment.h"

#include "NlRenderDevice.h"
#include "NlPhysicsDevice.h"

#include "CameraObject.h"
#include "nlPxMathlib.h"
#include "NlDebugRenderer.h"

//=================================================================================
NlPxPicker::NlPxPicker( HWND hWnd, NlRenderDevice* RD, NlPhysicsDevice* PD )
{
	assert( RD );
	assert( PD );

	m_hWnd = hWnd;
	m_RD = RD;
	m_PD = PD;

	m_camera = NULL;
	m_hitJoint = NULL;
	m_hitSphere = NULL;
	m_selected = NULL;

}
//---------------------------------------------------------------------------------
NlPxPicker::~NlPxPicker( void )
{
}
//---------------------------------------------------------------------------------
void NlPxPicker::SetCamera( CCamera* camera )
{
	m_camera = camera;
}
//---------------------------------------------------------------------------------
bool NlPxPicker::PickActor( POINT& pt )
{
	if (m_camera == NULL)
		return false;
	LetGoActor();

	ScreenToClient( m_hWnd, &pt );

	// cast a ray
	NxRay ray;
	ViewUnProject( pt.x, pt.y, 0, ray.orig );
	ViewUnProject( pt.x, pt.y, 1, ray.dir );
	ray.dir -= ray.orig;	ray.dir.normalize();

	NxRaycastHit hit;
	NxShape* closestShape = m_PD->GetScene()->raycastClosestShape( ray, NX_ALL_SHAPES, hit );
	if (!closestShape || !closestShape->getActor().isDynamic())
		return false;

	int hitX, hitY;
	ViewProject( hit.worldImpact, hitX, hitY, m_hitDepth );
   	m_hitSphere = CreateHitSphere( hit.worldImpact );
	if (m_hitSphere == false)
		return false;

	m_hitSphere->raiseBodyFlag( NX_BF_KINEMATIC );
	m_hitSphere->raiseActorFlag( NX_AF_DISABLE_COLLISION );

	// create a fixed-joint to connect selected one with hit-sphere
	NxFixedJointDesc desc;
	m_selected = &closestShape->getActor();
	desc.actor[0] = m_hitSphere;
	desc.actor[1] = m_selected;
	m_hitSphere->getGlobalPose().multiplyByInverseRT( hit.worldImpact, desc.localAnchor[0] );
	m_selected->getGlobalPose().multiplyByInverseRT( hit.worldImpact, desc.localAnchor[1] );
	if (desc.isValid() == false)
		return false;

	NxJoint* fixedJoint = m_PD->GetScene()->createJoint( desc );
	if(!fixedJoint)
	{
		m_PD->ReleaseActor( m_hitSphere );
		return false;
	}

	m_hitJoint = (NxFixedJoint*)fixedJoint->is( NX_JOINT_FIXED );
	return true;
	
}
//---------------------------------------------------------------------------------
void NlPxPicker::update( POINT& pt )
{
	if (!m_hitSphere)
		return;

	ScreenToClient( m_hWnd, &pt );

	NxVec3 pos;
	ViewUnProject( pt.x, pt.y, m_hitDepth, pos );
	m_hitSphere->setGlobalPosition( pos );

}
//---------------------------------------------------------------------------------
void NlPxPicker::Draw()
{
	if (!m_hitSphere)
		return;

	// draw the hit point--------------
	NlDebugRenderer::DrawWireShape( m_hitSphere->getShapes()[0], float3(1.0f, 1.0f, 0.0f) );
	// draw the hit joint--------------
	NxActor* p1, *p2;
	m_hitJoint->getActors( &p1, &p2 );
	NlDebugRenderer::DrawArrow( p1->getCMassGlobalPosition(), p2->getCMassGlobalPosition(), float3(1.0f, 1.0f, 0.0f) );

	/*NxVec3 jointAnchor = m_hitJoint->getGlobalAnchor();
	NxVec3 jointAxis   = m_hitJoint->getGlobalAxis();
	NlDebugRenderer::DrawArrow( jointAnchor, jointAnchor+0.8f*jointAxis, float3(1.0f, 1.0f, 0.0f) );*/
	
}
//---------------------------------------------------------------------------------
void NlPxPicker::LetGoActor()
{
	if (m_hitJoint)
	{
		m_PD->ReleaseJoint( m_hitJoint );
		m_hitJoint = NULL;
	}
	if (m_hitSphere)
	{
		m_PD->ReleaseActor( m_hitSphere );
		m_hitSphere = NULL;
	}

}
//---------------------------------------------------------------------------------
void NlPxPicker::ViewProject( NxVec3& v, int& winX, int& winY, float& depth )
{
	assert( m_camera );

	const float44& viewMat = m_camera->GetViewMatrix();
	const float44& vpMat = m_camera->GetViewProjectionMatrix();
	float44 viewportMat;
	float44 vpvMat;

	D3DVIEWPORT9 vp;
	m_RD->GetDevice()->GetViewport( &vp );

	viewportMat = float44( vp.Width*0.5f,	   0,				    0,				 0,
						   0,				   vp.Height*0.5f,      0,				 0,
						   0,				   0,				    vp.MaxZ-vp.MinZ, 0,
						   vp.X+vp.Width*0.5f, vp.Y+vp.Height*0.5f, vp.MinZ,		 1 );
	vpvMat = vpMat*viewportMat;

	float3 worldPos = CovToFloat3( v );
	float3 screenPos = viewMat.MultVector3( worldPos );

	// record depth
	depth = screenPos.z;

	screenPos = vpvMat.MultVector3( worldPos );
	winX = screenPos.x;	winY = vp.Height - screenPos.y;

}
//---------------------------------------------------------------------------------
void NlPxPicker::ViewUnProject( int winX, int winY, float depth, NxVec3& v )
{
	assert( m_camera );

	// coordinate in view-coord system
	float vcX = 0.0f;
	float vcY = 0.0f;

	const float44& projMat = m_camera->GetProjectMatrix();
	float44 viewMat = m_camera->GetViewMatrix();
	float44 viewMatInv = viewMat.GetInverse();

	D3DVIEWPORT9 vp;
	m_RD->GetDevice()->GetViewport( &vp );

	vcX = ((( 2.0f*winX) / vp.Width)  - 1.0f) / projMat.m00;
	vcY = (((-2.0f*winY) / vp.Height) + 1.0f) / projMat.m11;

	float3 _v( vcX*depth, vcY*depth, depth );
	_v = viewMatInv.MultVector3( _v );

	v = CovToPXVec( _v );

}
//---------------------------------------------------------------------------------
NxActor* NlPxPicker::CreateHitSphere( const NxVec3& pos )
{
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = 0.05f;
	sphereDesc.localPose.t = NxVec3( 0, 0.05f, 0 );

	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	actorDesc.shapes.pushBack( &sphereDesc );
	actorDesc.globalPose.t = pos;
	actorDesc.density = 1.0f;
	actorDesc.body = &bodyDesc;

	return m_PD->GetScene()->createActor( actorDesc );

}