/*
  @file   NlImmGUI.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlImmGUI_h_
#define _NlImmGUI_h_

#include "prerequisite.h"

class NlGameEnvironment;
class NlRenderDevice;
class NlVertexDecl;
class NlDynamicShaderComb;
class NlTexture2D;

// Singleton ====================================================
class NlImmGUI :
	public IInputEventListener
{
	friend class NlGFramework;
public:
	//-------------------------------------------
	static bool Button( float2 pos, float2 size, const char* str = "" );
	// use custom button texture, needs three texture's file name
	static bool ButtonCustom( const char* normTexFile, const char* hoverTexFile, const char* pressTexFile,
		float2 pos, float2 size, const char* str = "" );

private:
	NlImmGUI( NlRenderDevice* device );
	NlImmGUI( const NlImmGUI& another ) {}
	NlImmGUI& operator= ( const NlImmGUI& another ) {}
	~NlImmGUI(void);

	static void Create( NlGameEnvironment* gameEnv );
	static void Destroy();

	virtual bool OnInputEvent( const SInputEvent &event );

private:
	static NlImmGUI* instance;

	NlRenderDevice*		 m_device;
	NlVertexDecl*		 m_vd;
	NlDynamicShaderComb* m_shaders;

	// button
	ID3DXFont*			 m_btnFont;
	NlTexture2D*		 m_btnNorm;
	NlTexture2D*		 m_btnHover;
	NlTexture2D*		 m_btnDown;
	EInputState			 m_lBtnState;

};

#endif