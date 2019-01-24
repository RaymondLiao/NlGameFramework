#pragma once
/*
  @file   NlSkyBox.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlSkyBox_h_
#define _NlSkyBox_h_

#include "NlGameObject.h"

class NlTextureCube;
class NlSkyBox :
	public NlRenderableRaw
{
public:
	NlSkyBox();
	virtual ~NlSkyBox();

	virtual bool init( NlGameEnvironment* gameEnv );
	virtual void Draw( NlGameEnvironment* gameEnv );
	virtual void destroy();

	NlTextureCube* GetEnvTexture();

private:
	NlTextureCube* m_envTexture;	// environment texture
};

#endif