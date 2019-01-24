/*
  @file   NlWater.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlWater_h_
#define _NlWater_h_

#include "NlGameObject.h"

class NlTexture2D;

class NlWater :
	public NlRenderableRaw
{
public:
//		   ____________
//		  / 		  /
// length/			 /
//	    /___________/
//		  width
	NlWater( int fieldW, int fieldL );
	virtual ~NlWater();

	virtual bool init( NlGameEnvironment* gameEnv );
	virtual void Draw( NlGameEnvironment* gameEnv );
	virtual void destroy();

	void SetTextureResolution( int wid, int len );

	NlTexture2D* GetReflTexture() {return m_reflTex;}
	NlTexture2D* GetRefrTexture() {return m_refrTex;}

public:
	float  m_waveHei;		// wave height
	float  m_waveLen;		// wave length
	float  m_waveSpeed;
	float2 m_waveDir;		// wave movement's direction
	float2 m_waveoffset;	// unuse

private:
	int m_fieldWid;
	int m_fieldLen;

	NlVertexBuffer* m_tcVb;				// textcoord vb branch

	NlTexture2D* m_bumpTex;		// bump texture contains normal information
	NlTexture2D* m_reflTex;		// environment reflection
	NlTexture2D* m_refrTex;		// environment refraction

};

#endif