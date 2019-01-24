#include "NlBuffer.h"
#include "NlRenderDevice.h"

//=================================================================================
NlBuffer::NlBuffer( NlRenderDevice* device )
{
	assert( device );
	m_device = device->GetDevice();
	assert( m_device );
	m_elementCount = m_elementSize = 0;
}
//---------------------------------------------------------------------------------
NlBuffer::~NlBuffer(void)
{
}

// VB==============================================================================
void NlVertexBuffer::init( uint32 elemSize, uint32 elemCount )
{
	// clear up
	destroy();

	NlBuffer::init( elemSize, elemCount );

	m_device->CreateVertexBuffer( elemSize*elemCount,	// buffer size
		D3DUSAGE_WRITEONLY,
		NULL,
		D3DPOOL_MANAGED,
		&m_vb,
		NULL );
}
//---------------------------------------------------------------------------------
void NlVertexBuffer::update( void* pData, uint32 size )
{
	assert( pData );
	assert( m_vb );

	void *gpuData = NULL;
	if (D3D_OK == m_vb->Lock(0, size, &gpuData, NULL))
	{
		memcpy( gpuData, pData, size );
		m_vb->Unlock();
	}
}
//---------------------------------------------------------------------------------
void NlVertexBuffer::destroy()
{
	if (m_vb)
	{
		m_vb->Release();
		m_vb = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlVertexBuffer::Use()
{
	m_device->SetStreamSource( 0, m_vb, 0, m_elementSize );
}

void NlVertexBuffer::Use( uint32 streamNumcer )
{
	m_device->SetStreamSource( streamNumcer, m_vb, 0, m_elementSize );
}
// IB==============================================================================
void NlIndexBuffer::init( uint32 elemSize, uint32 elemCount )
{
	// clear up
	destroy();

	NlBuffer::init( elemSize, elemCount );

	m_device->CreateIndexBuffer( elemSize*elemCount,	// buffer size
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&m_ib,
		NULL );
}
//---------------------------------------------------------------------------------
void NlIndexBuffer::update( void* pData, uint32 size )
{
	assert( pData );
	assert( m_ib );

	void *gpuData = NULL;
	if (D3D_OK == m_ib->Lock(0, size, &gpuData, NULL))
	{
		memcpy( gpuData, pData, size );
		m_ib->Unlock();
	}
}
//---------------------------------------------------------------------------------
void NlIndexBuffer::destroy()
{
	if (m_ib)
	{
		m_ib->Release();
		m_ib = NULL;
	}
}
//---------------------------------------------------------------------------------
void NlIndexBuffer::Use()
{
	m_device->SetIndices( m_ib );
}