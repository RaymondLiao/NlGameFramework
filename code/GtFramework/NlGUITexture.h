/*
  @file   NlGUITexture.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlGUITexture_h_
#define _NlGUITexture_h_

#include "prerequisite.h"

class NlRenderDevice;
class NlVertexDecl;
class NlDynamicShaderComb;
class NlTexture2D;

class NlGUITexture
{
public:
	NlGUITexture( NlRenderDevice* device );
	~NlGUITexture(void);
	
	void destroy();

	bool init( const char* texFileName );
	// use default texture
	void Draw( float2 pos, float2 size );	// both range 0~1
	// use another texture; if the texture file dosen't exist, use default texture
	void Draw( const char* texFileName, float2 pos, float2 size );

	bool initFrameAni( std::vector<std::string> texFileNames, float intervalTime );
	void PlayFrameAni( float elapsedTime, float2 pos, float2 size, bool loop = false );

private:
	NlRenderDevice*	     m_device;
	NlVertexDecl*		 m_vd;
	NlDynamicShaderComb* m_shaders;

	NlTexture2D*		 m_texture;

	// frame Animation
	bool m_hasFrameAni;
	std::vector<NlTexture2D*> m_frames;
	int m_curFrame;
	float m_intervalTime;
	float m_acumuTime;

};

#endif