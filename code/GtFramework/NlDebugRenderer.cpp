#include "NlDebugRenderer.h"
#include "nlPxMathlib.h"
#include "nl_render_header.h"

//=================================================================================
typedef struct _NlPxDebugPoint :
	public NlVertexPC
{
	_NlPxDebugPoint():NlVertexPC() {}

	_NlPxDebugPoint( float x, float y, float z, float r, float g, float b )
	{
		pos.x = x; pos.y = y; pos.z = z;
		color.r = r; color.g = g; color.b = b;
	}

} NlPxDebugPoint;
typedef std::vector<NlPxDebugPoint> NlPxDebugPointArr;

//=================================================================================
NlDebugRenderer* NlDebugRenderer::instance = NULL;
NlPxDebugPointArr gDebugPointArr;

NlDebugRenderer::NlDebugRenderer( NlRenderDevice* device ):
	m_device( device )
{
	m_camera = NULL;
}
//---------------------------------------------------------------------------------
NlDebugRenderer::~NlDebugRenderer(void)
{
	if (m_vd)
	{
		m_vd->destroy();
		delete m_vd;
		m_vd = NULL;
	}
	if (m_shaders)
	{
		m_shaders->destroy();
		delete m_shaders;
		m_shaders = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlDebugRenderer::Create( NlRenderDevice* device )
{
	if (instance != NULL)
		return;

	assert( device );
	instance = new NlDebugRenderer( device );
	assert( instance );
	instance->init();
}

//NlDebugRenderer* NlDebugRenderer::GetInstance()
//{
//	if (m_instance == NULL)
//	{
//		MessageBox( NULL, "Need to create Debug Render first", "error", MB_OK );
//		return NULL;
//	}
//
//	return m_instance;
//}

//---------------------------------------------------------------------------------
void NlDebugRenderer::init()
{
	// create vertex declaration
	m_vd = new NlVertexDecl( m_device );
	assert( m_vd );

	VertexElemArray elements;
	{
		NlVertexElem elem( 0, eVEU_Position, 0 );
		elements.push_back( elem );
	}
	{
		NlVertexElem elem( 0, eVEU_Color, 0 );
		elements.push_back( elem );
	}
	m_vd->init( elements );

	// create debug shader
	m_shaders = new NlDynamicShaderComb( m_device );
	assert( m_shaders );

	m_shaders->CompileFromFile( "PxDebug.shader" );
}
//---------------------------------------------------------------------------------
void NlDebugRenderer::Destroy()
{
	if (instance)
	{
		delete instance;
		instance = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlDebugRenderer::SetCamera( CCamera* camera )
{
	if (instance == NULL)
	{
		MessageBox( NULL, "Need to create Debug Render first", "error", MB_OK );
		return;
	}

	instance->m_camera = camera;
}
//---------------------------------------------------------------------------------
void NlDebugRenderer::RenderData( const NxDebugRenderable& data )
{
	// draw points----------------------
	uint32 nbPoints = data.getNbPoints();
	if (nbPoints)
	{
		const NxDebugPoint* points = data.getPoints();
		assert( points );

		gDebugPointArr.clear();
		while( nbPoints-- )
		{
			NlPxDebugPoint p( points->p.x, points->p.y, points->p.z,
				(float)((points->color >> 16) & 0xff) / 255.0f,
				(float)((points->color >> 8)  & 0xff) / 255.0f,
				(float)( points->color        & 0xff) / 255.0f );

			gDebugPointArr.push_back( p );
			points++;
		}

		RenderBuffer( (float*)&gDebugPointArr.data()[0], D3DPT_POINTLIST, data.getNbPoints() );
	}

	// draw lines----------------------
	uint32 nbLines = data.getNbLines();
	if (nbLines)
	{
		const NxDebugLine* lines = data.getLines();
		assert( lines );

		gDebugPointArr.clear();
		while( nbLines-- )
		{
			NlPxDebugPoint p0( lines->p0.x, lines->p0.y, lines->p0.z,
				(float)((lines->color >> 16) & 0xff) / 255.0f,
				(float)((lines->color >> 8)  & 0xff) / 255.0f,
				(float)( lines->color        & 0xff) / 255.0f );
			NlPxDebugPoint p1( lines->p1.x, lines->p1.y, lines->p1.z,
				(float)((lines->color >> 16) & 0xff) / 255.0f,
				(float)((lines->color >> 8)  & 0xff) / 255.0f,
				(float)( lines->color        & 0xff) / 255.0f );

			gDebugPointArr.push_back( p0 );
			gDebugPointArr.push_back( p1 );
			lines++;
		}

		RenderBuffer( (float*)&gDebugPointArr.data()[0], D3DPT_LINELIST, data.getNbLines() );
	}

	// draw triangles----------------------
	uint32 nbTris = data.getNbTriangles();
	if (nbTris)
	{
		const NxDebugTriangle* triangles = data.getTriangles();
		assert( triangles );

		gDebugPointArr.clear();
		while( nbTris-- )
		{
			NlPxDebugPoint p0( triangles->p0.x, triangles->p0.y, triangles->p0.z,
				(float)((triangles->color >> 16) & 0xff) / 255.0f,
				(float)((triangles->color >> 8)  & 0xff) / 255.0f,
				(float)( triangles->color        & 0xff) / 255.0f );
			NlPxDebugPoint p1( triangles->p1.x, triangles->p1.y, triangles->p1.z,
				(float)((triangles->color >> 16) & 0xff) / 255.0f,
				(float)((triangles->color >> 8)  & 0xff) / 255.0f,
				(float)( triangles->color        & 0xff) / 255.0f );
			NlPxDebugPoint p2( triangles->p2.x, triangles->p2.y, triangles->p2.z,
				(float)((triangles->color >> 16) & 0xff) / 255.0f,
				(float)((triangles->color >> 8)  & 0xff) / 255.0f,
				(float)( triangles->color        & 0xff) / 255.0f );
			
			gDebugPointArr.push_back( p0 );
			gDebugPointArr.push_back( p1 );
			gDebugPointArr.push_back( p2 );
		}

		RenderBuffer( (float*)&gDebugPointArr.data()[0], D3DPT_TRIANGLELIST, data.getNbTriangles() );
	}

}

void NlDebugRenderer::RenderBuffer( float* buf, uint32 primType, int primCount )
{
	if (!instance || instance->m_camera == NULL)
		return;

	float44 wvp_mat = instance->m_camera->GetViewProjectionMatrix();
	instance->m_shaders->SetMatrix_vs( 0, wvp_mat );
	instance->m_shaders->SetMatrix_vs( 4, float44::CreateIdentity() );

	instance->m_vd->Use();
	instance->m_shaders->Use();

	IDirect3DDevice9* renderDevice = instance->m_device->GetDevice();
	assert( renderDevice );

	renderDevice->DrawPrimitiveUP( (D3DPRIMITIVETYPE)primType, primCount, 
		buf, sizeof(NlPxDebugPoint) );

}
//---------------------------------------------------------------------------------
void NlDebugRenderer::DrawLine( const float3& p0, const float3& p1, const float3& color )
{
	DrawLine( CovToPXVec(p0), CovToPXVec(p1), color );
}

void NlDebugRenderer::DrawLine( const NxVec3& p0, const NxVec3& p1, const float3& color )
{
	
	if (!instance || instance->m_camera == NULL)
		return;

	float44 wvp_mat = instance->m_camera->GetViewProjectionMatrix();
	instance->m_shaders->SetMatrix_vs( 0, wvp_mat );
	instance->m_shaders->SetMatrix_vs( 4, float44::CreateIdentity() );

	instance->m_vd->Use();
	instance->m_shaders->Use();

	IDirect3DDevice9* renderDevice = instance->m_device->GetDevice();
	assert( renderDevice );

	float buf[] = { p0.x, p0.y, p0.z, color.x, color.y, color.z, 
		p1.x, p1.y, p1.z, color.x, color.y, color.z };

	renderDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, buf, sizeof(float3)*2 );
}

void NlDebugRenderer::DrawLine( const float3& locP0, const float3& locP1, const float44& worldMat, const float3& color )
{
	DrawLine( CovToPXVec(locP0), CovToPXVec(locP1), worldMat, color );
}

void NlDebugRenderer::DrawLine( const NxVec3& locP0, const NxVec3& locP1, const float44& worldMat, const float3& color )
{
	if (!instance || instance->m_camera == NULL)
		return;

	float44 wvp_mat = worldMat * instance->m_camera->GetViewProjectionMatrix();
	instance->m_shaders->SetMatrix_vs( 0, wvp_mat );
	instance->m_shaders->SetMatrix_vs( 4, worldMat );

	instance->m_vd->Use();
	instance->m_shaders->Use();

	IDirect3DDevice9* renderDevice = instance->m_device->GetDevice();
	assert( renderDevice );

	float buf[] = { locP0.x, locP0.y, locP0.z, color.x, color.y, color.z,
		locP1.x, locP1.y, locP1.z, color.x, color.y, color.z };

	renderDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, buf, sizeof(float3)*2 );
}

//---------------------------------------------------------------------------------
void NlDebugRenderer::DrawCircle( const uint32 segmentCount, const float44& worldMat, const float3& color,
	const float radius, const bool semicircle /* = false */ )
{
	float step = SR_PI*2/float(segmentCount);
	uint32 segs = segmentCount;
	if (semicircle)
	{
		segs *= 0.5f;
	}

	for (uint32 i = 0; i < segs; ++i)
	{
		uint32 j = i + 1;
		if (j == segmentCount)
			j = 0;	// start point
		
		float angle0 = uint32(i) * step;
		float angle1 = uint32(j) * step;

		float3 p0(radius*sinf(angle0), radius*cosf(angle0), 0.0f);
		float3 p1(radius*sinf(angle1), radius*cosf(angle1), 0.0f);
		p0 = worldMat.MultVector3( p0 );
		p1 = worldMat.MultVector3( p1 );

		DrawLine( p0, p1, color );
	}
}

void NlDebugRenderer::DrawCircle( const uint32 segmentCount, const NxMat34& worldMat, const float3& color, 
	const float radius, const bool semicircle /*= false */ )
{
	float44 _worldMat;
	SetupDXMat44( worldMat, _worldMat );
	DrawCircle( segmentCount, _worldMat, color, radius, semicircle );
}
//---------------------------------------------------------------------------------
void NlDebugRenderer::DrawForce( NxActor* actor, NxVec3& forceVec, const float3& color )
{
	assert( actor );

	NxReal strength = forceVec.magnitude();
	if (strength < 0.1f)
		return;

	forceVec = forceVec * 3 / strength;
	NxVec3 pos = actor->getCMassGlobalPosition();
	DrawArrow( pos, pos+forceVec, color );
}
//---------------------------------------------------------------------------------
void NlDebugRenderer::DrawArrow( const float3& posA, const float3& posB, const float3& color )
{
	NxVec3 _posA( posA.x, posA.y, posA.z );
	NxVec3 _posB( posB.x, posB.y, posB.z );
	DrawArrow( _posA, _posB, color );
}

void NlDebugRenderer::DrawArrow( const NxVec3& posA, const NxVec3& posB, const float3& color )
{
	if (!instance || instance->m_camera == NULL)
		return;

	NxVec3 vec = posB - posA;
	NxVec3 t0, t1, t2;
	NxNormalToTangents( vec, t1, t2 );

	t0 = posB - posA;
	t0.normalize();

	NxVec3 lobe1 = posB - t0*0.15f + t1*0.15f;
	NxVec3 lobe2 = posB - t0*0.15f - t1*0.15f;
	NxVec3 lobe3 = posB - t0*0.15f + t2*0.15f;
	NxVec3 lobe4 = posB - t0*0.15f - t2*0.15f;

	float buf[] = { posA.x, posA.y, posA.z, color.x, color.y, color.z,
		posB.x, posB.y, posB.z, color.x, color.y, color.z,
		posB.x, posB.y, posB.z, color.x, color.y, color.z,
		lobe1.x, lobe1.y, lobe1.z, color.x, color.y, color.z,
		posB.x, posB.y, posB.z, color.x, color.y, color.z,
		lobe2.x, lobe2.y, lobe2.z, color.x, color.y, color.z,
		posB.x, posB.y, posB.z, color.x, color.y, color.z,
		lobe3.x, lobe3.y, lobe3.z, color.x, color.y, color.z,
		posB.x, posB.y, posB.z, color.x, color.y, color.z,
		lobe4.x, lobe4.y, lobe4.z, color.x, color.y, color.z };

	float44 wvp_mat = instance->m_camera->GetViewProjectionMatrix();
	instance->m_shaders->SetMatrix_vs( 0, wvp_mat );
	instance->m_shaders->SetMatrix_vs( 4, float44::CreateIdentity() );

	instance->m_vd->Use();
	instance->m_shaders->Use();

	IDirect3DDevice9* renderDevice = instance->m_device->GetDevice();
	assert( renderDevice );

	renderDevice->DrawPrimitiveUP( D3DPT_LINELIST, 5, buf, sizeof(float3)*2 );
}
//---------------------------------------------------------------------------------
void NlDebugRenderer::DrawCoordSystem( const float3& pos, const Quat& rot )
{
	const float len = 1.0f;
	float44 world_mat = float44::CreateIdentity();
	world_mat.Set( float3(1), rot, pos );

	float3 origin( 0 );
	float3 axis_x( len, 0, 0 );
	float3 axis_y( 0, len, 0 );
	float3 axis_z( 0, 0, len );

	origin = world_mat.MultVector3( origin );
	axis_x = world_mat.MultVector3( axis_x );
	axis_y = world_mat.MultVector3( axis_y );
	axis_z = world_mat.MultVector3( axis_z );

	DrawArrow( origin, axis_x, float3(1.0f, 0.0f, 0.0f) );	// red
	DrawArrow( origin, axis_y, float3(0.0f, 1.0f, 0.0f) );	// green
	DrawArrow( origin, axis_z, float3(0.0f, 0.0f, 1.0f) );	// blue
}

void NlDebugRenderer::DrawCoordSystem( const NxMat34& pose )
{
	const NxReal len = 1.0f;
	NxVec3 origin( 0, 0, 0 );
	NxVec3 axis_x( len, 0, 0 );
	NxVec3 axis_y( 0, len, 0 );
	NxVec3 axis_z( 0, 0, len );

	pose.multiply( origin, origin );
	pose.multiply( axis_x, axis_x );
	pose.multiply( axis_y, axis_y );
	pose.multiply( axis_z, axis_z );

	DrawArrow( origin, axis_x, float3(1.0f, 0.0f, 0.0f) );
	DrawArrow( origin, axis_y, float3(0.0f, 1.0f, 0.0f) );
	DrawArrow( origin, axis_z, float3(0.0f, 0.0f, 1.0f) );
}

void NlDebugRenderer::DrawCoordSystem( NxActor* actor )
{
	assert( actor );
	DrawCoordSystem( actor->getGlobalPose() );
}

//---------------------------------------------------------------------------------
void NlDebugRenderer::DrawWireShape( NxShape* shape, const float3& color )
{
	assert( shape );
	switch(shape->getType())
	{
	case NX_SHAPE_PLANE:
		break;
	case NX_SHAPE_BOX:
		DrawWireBox( shape, color );
		break;
	case NX_SHAPE_SPHERE:
		DrawWireSphere( shape, color );
		break;
	case NX_SHAPE_CAPSULE:
		DrawWireCapsule( shape, color );
		break;
	case NX_SHAPE_CONVEX:
		break;
	case NX_SHAPE_MESH:
		DrawWireMesh( shape, color );
		break;
	}
}
//---------------------------------------------------------------------------------
void NlDebugRenderer::DrawWireBox( NxShape* box, const float3& color )
{
	NxBox obb;
	box->isBox()->getWorldOBB( obb );

	// compute obb vertices
	NxVec3 pp[8];
	NxComputeBoxPoints( obb, pp );

	// draw all lines
	const NxU32* indices = NxGetBoxEdges();
	for (uint32 i = 0; i < 12; ++i)
	{
		uint32 vRef0 = *indices++;
		uint32 vRef1 = *indices++;
		DrawLine( pp[vRef0], pp[vRef1], color );
	}
}
//---------------------------------------------------------------------------------
void NlDebugRenderer::DrawWireSphere( NxShape* sphere, const float3& color )
{
	float radius = sphere->isSphere()->getRadius();

	NxMat34 pose = sphere->getGlobalPose();
	float44 worldMat;

	NxVec3 col0;	pose.M.getColumn( 0, col0 );
	NxVec3 col1;	pose.M.getColumn( 1, col1 );
	NxVec3 col2;	pose.M.getColumn( 2, col2 );
	SetupDXMat44( pose, worldMat );
	DrawCircle( 20, worldMat, color, radius );

	pose.M.setColumn( 0, col1 );
	pose.M.setColumn( 1, col2 );
	pose.M.setColumn( 2, col0 );
	SetupDXMat44( pose, worldMat );
	DrawCircle( 20, worldMat, color, radius );

	pose.M.setColumn( 0, col2 );
	pose.M.setColumn( 1, col0 );
	pose.M.setColumn( 2, col1 );
	SetupDXMat44( pose, worldMat );
	DrawCircle( 20, worldMat, color, radius );
}
//---------------------------------------------------------------------------------
void NlDebugRenderer::DrawWireCapsule( NxShape* capsule, const float3& color )
{
	float radius = capsule->isCapsule()->getRadius();
	float height = capsule->isCapsule()->getHeight();

	NxMat34 pose = capsule->getGlobalPose();
	float44 worldMat;

	NxSegment lineSg;
	pose.M.getColumn( 1, lineSg.p1 );
	lineSg.p1 *= 0.5 * height;
	lineSg.p0 = -lineSg.p1;
	lineSg.p0 += pose.t;
	lineSg.p1 += pose.t;

	NxVec3 col0;	pose.M.getColumn( 0, col0 );
	NxVec3 col1;	pose.M.getColumn( 1, col1 );
	NxVec3 col2;	pose.M.getColumn( 2, col2 );
	DrawLine( lineSg.p0 + col0*radius, lineSg.p1 + col0*radius, color );
	DrawLine( lineSg.p0 - col0*radius, lineSg.p1 - col0*radius, color );
	DrawLine( lineSg.p0 + col2*radius, lineSg.p1 + col2*radius, color );
	DrawLine( lineSg.p0 - col2*radius, lineSg.p1 - col2*radius, color );

	// draw half, flipped circles
	pose.M.setColumn( 0, -col1 );
	pose.M.setColumn( 1, -col0 );
	pose.M.setColumn( 2, col2 );
	pose.t = lineSg.p0;
	SetupDXMat44( pose, worldMat );
	DrawCircle( 20, worldMat, color, radius, true );

	pose.M.setColumn( 0, col1 );
	pose.M.setColumn( 1, -col0 );
	pose.M.setColumn( 2, col2 );
	pose.t = lineSg.p1;
	SetupDXMat44( pose, worldMat );
	DrawCircle( 20, worldMat, color, radius, true );

	pose.M.setColumn( 0, -col1 );
	pose.M.setColumn( 1, col2 );
	pose.M.setColumn( 2, col0 );
	pose.t = lineSg.p0;
	SetupDXMat44( pose, worldMat );
	DrawCircle( 20, worldMat, color, radius, true );

	pose.M.setColumn( 0, col1 );
	pose.M.setColumn( 1, col2 );
	pose.M.setColumn( 2, col0 );
	pose.t = lineSg.p1;
	SetupDXMat44( pose, worldMat );
	DrawCircle( 20, worldMat, color, radius, true );

	// draw full, flipped circles
	pose.M.setColumn( 0, col2 );
	pose.M.setColumn( 1, col0 );
	pose.M.setColumn( 2, col1 );
	pose.t = lineSg.p0;
	SetupDXMat44( pose, worldMat );
	DrawCircle( 20, worldMat, color, radius );
	pose.t = lineSg.p1;
	SetupDXMat44( pose, worldMat );
	DrawCircle( 20, worldMat, color, radius );
}
//---------------------------------------------------------------------------------
typedef NxVec3 MeshPoint;
typedef struct _MeshTri { NxU32 p0; NxU32 p1; NxU32 p2; } MeshTri;

void NlDebugRenderer::DrawWireMesh( NxShape* mesh, const float3& color )
{
	if (mesh->userData == NULL)
		return;

	NxMat34 pose = mesh->getGlobalPose();
	float44 worldMat;
	SetupDXMat44( pose, worldMat );

	void* ptr = mesh->userData;
	NxTriangleMeshDesc meshDesc = *((NxTriangleMeshDesc*)(ptr));

	NxU32 vCount = meshDesc.numVertices;
	NxU32 triCount = meshDesc.numTriangles;

	MeshPoint* points = (MeshPoint*)meshDesc.points;
	MeshTri* triangles = (MeshTri*)meshDesc.triangles;

	while(triCount--)
	{
		DrawLine( points[triangles->p0], points[triangles->p1], worldMat, color );
		DrawLine( points[triangles->p1], points[triangles->p2], worldMat, color );
		DrawLine( points[triangles->p2], points[triangles->p0], worldMat, color );

		triangles++;
	}
}
