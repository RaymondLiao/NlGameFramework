/*
  @file   NlGUIText.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlGUIText_h_
#define _NlGUIText_h_

#include "prerequisite.h"

//typedef struct _NlColor
//{
//	uint8 r, g, b, a;
//
//	_NlColor()
//	{
//		r = g = b = a = 255;
//	}
//
//	uint32 ToHexadecimal()
//	{
//		
//	}
//
//} NlColor;

enum ENlFont
{
	eFT_Arial,
	eFT_TimesNewRoman
};

enum ENlFontStyle
{
	eFT_Normal,
	eFT_Bold,
	eFT_Italic,
	eFT_BoldAndItalic
};

enum ENlFtAlignment
{
	/*eFT_UpperLeft,
	eFT_UpperCenter,
	eFT_UpperRight,
	eFT_MiddleLeft,
	eFT_MiddleCenter,
	eFT_MiddleRight,
	eFT_LowerLeft,
	eFT_LowerCenter,
	eFT_LowerRight*/
	eFT_Left,
	eFT_Center,
	eFT_Right
};

typedef struct _NlTextDesc
{
	ENlFont			font;
	ENlFontStyle	style;
	uint32			size;
	ENlFtAlignment	alignment;
	float2			position;

	_NlTextDesc()
	{
		font		= eFT_Arial;
		style		= eFT_Normal;
		size		= 0;
		alignment	= eFT_Left;
		position	= float2(0, 0);		// range 0~1
	}

} NlTextDesc;

//====================================================
class NlRenderDevice;
class NlGUIText
{
public:
	NlGUIText( NlRenderDevice* device );
	~NlGUIText(void);

	bool init( NlTextDesc desc );
	void destroy();
	void Draw( const char* content, uint32 color = 0xffffffff );

private:
	IDirect3DDevice9* m_device;
	ID3DXFont* m_font;

	int2 m_pos;
	ENlFtAlignment m_alignment;

};

#endif