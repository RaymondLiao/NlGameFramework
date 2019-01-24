/*
  @file   NlVertexDecl.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlVertexDecl_h_
#define _NlVertexDecl_h_

#include "prerequisite.h"

//====================================================
typedef struct _NlVertexPNT
{
	float3 pos;
	float3 normal;
	float2 texcoord;

	_NlVertexPNT():pos(0), normal(0), texcoord(0, 0) {}

	_NlVertexPNT( const float3& p, const float3& n, const float2& t ):
		pos(p), normal(n), texcoord(t) {}

} NlVertexPNT;
#define NlFVF_PNT ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX0 )
typedef std::vector<NlVertexPNT> NlVertPNTArray;

//----------------------------------------------------
typedef struct _NlVertexPC
{
	float3 pos;
	float3 color;

	_NlVertexPC():pos(0), color(0) {}
	_NlVertexPC( const float3& p, const float3& col ):
		pos(p), color(col)	{}

} NlVertexPC;
#define NlFVF_PC ( D3DFVF_XYZ | D3DFVF_TEX0 )
typedef std::vector<NlVertexPC> NlVertPCArray;

//====================================================
typedef enum _ENlVertexElemUsage
{
	eVEU_Position,
	eVEU_Normal,
	eVEU_Texcoord,
	eVEU_Tangent,
	eVEU_Color,

	eVEU_count
} ENlVertexElemUsage;

typedef struct _NlVertexElem
{
	_NlVertexElem( uint32 _stream, ENlVertexElemUsage _usage, uint8 _usageIndex ):
		stream(_stream), usage(_usage), usageIndex(_usageIndex) {}

	uint32 stream;
	ENlVertexElemUsage usage;
	uint8  usageIndex;

} NlVertexElem;
typedef std::vector<NlVertexElem> VertexElemArray;

//====================================================
class NlRenderDevice;
class NlVertexDecl
{
public:
	NlVertexDecl( NlRenderDevice* device );
	~NlVertexDecl(void);

	void init();	// used for pos_norm_textcoord
	void init( VertexElemArray& vertexLayout );
	void destroy();
	void Use();

private:
	IDirect3DDevice9* m_device;
	IDirect3DVertexDeclaration9* m_vd;

};

#endif
