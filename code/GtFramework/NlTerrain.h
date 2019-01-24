/*
  @file   NlTerrain.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlTerrain_h_
#define _NlTerrain_h_

#include "NlGameObject.h"

class NlTexture2D;

class NlTerrain :
	public NlRenderableRaw
{
public:
//		     ____________
//		 |  / 		    /
// height| /		   /length
//	     |/___________/
//		  width
	NlTerrain( int fieldW, int fieldL, float maxHeight, NlTransform& transform, 
		bool autoTexture = false, bool isStatic = true );
	virtual ~NlTerrain();

	virtual bool init( NlGameEnvironment* gameEnv );
	virtual void update( NlGameEnvironment* gameEnv );
	virtual void Draw( NlGameEnvironment* gameEnv );
	virtual void destroy();

	void SetLocalField( int fieldW, int fieldL );

private:
	void SetParent( NlGameObjectBase* parent );

private:
	int m_fieldWid;
	int m_fieldLen;
	int m_maxHeight;

	int m_locallWid;
	int m_localLen;

	float3* m_vPos;						// contains vertexes positions' information
	float2* m_vTcd;						// contains vertexes texcoords' information
	NlVertexBuffer* m_tcVb;				// textcoord vb branch
	
	uint8*	m_heiMap;					// height map
	NlTexture2D* m_normMap;		// normal map
	NlTexture2D* m_texture;
	bool m_autoTexture;					// generate texture automatically

};

#endif