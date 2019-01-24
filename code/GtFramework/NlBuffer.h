/*
  @file   NlBuffer.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlBuffer_h_
#define _NlBuffer_h_

#include "prerequisite.h"

class NlRenderDevice;
class NlBuffer
{
public:
	NlBuffer( NlRenderDevice* device );
	virtual ~NlBuffer(void);

	virtual void init( uint32 elemSize, uint32 elemCount )
	{
		m_elementSize = elemSize;
		m_elementCount = elemCount;
	}
	virtual void update( void* pData, uint32 size ) = 0;
	virtual void destroy() = 0;
	virtual void Use() = 0;

public:
	uint32 m_elementCount;
protected:
	IDirect3DDevice9* m_device;
	uint32 m_elementSize;

};

// VB====================================================
class NlVertexBuffer :
	public NlBuffer
{
public:
	NlVertexBuffer( NlRenderDevice* device ): NlBuffer(device), m_vb(NULL) {}
	~NlVertexBuffer(void) {}

	virtual void init( uint32 elemSize, uint32 elemCount );
	virtual void update( void* pData, uint32 size );
	virtual void destroy();
	virtual void Use();
	        void Use( uint32 streamNumcer );	// for multi-stream

private:
	IDirect3DVertexBuffer9* m_vb;

};

// IB====================================================
class NlIndexBuffer :
	public NlBuffer
{
public:
	NlIndexBuffer( NlRenderDevice* device ): NlBuffer(device), m_ib(NULL) {}
	~NlIndexBuffer(void) {}

	virtual void init( uint32 elemSize, uint32 elemCount );
	virtual void update( void* pData, uint32 size );
	virtual void destroy();
	virtual void Use();

private:
	IDirect3DIndexBuffer9* m_ib;
};

#endif