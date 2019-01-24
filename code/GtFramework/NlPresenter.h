/*
  @file   NlPresenter.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlPresenter_h_
#define _NlPresenter_h_

#include "prerequisite.h"

struct NlPresenterParam
{
	HINSTANCE hInst;
	const char* windowName;

	int width;
	int height;
	int bpp;
};

class NlRenderDevice;
class NlPresenter
{
public:
	NlPresenter(void);
	~NlPresenter(void);

	bool Init(const NlPresenterParam& initParam);
	bool Swap();
	bool Shutdown();
	bool ProcessMsg();

public:
	NlRenderDevice* m_renderDevice;
	HWND m_hWnd;
};

#endif