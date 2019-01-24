/*
  @file   NlDynamicShaderComb.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlDynamicShaderComb_h_
#define _NlDynamicShaderComb_h_

class NlRenderDevice;
class NlDynamicShaderComb
{
public:
	NlDynamicShaderComb( NlRenderDevice* renderDevice  );
	~NlDynamicShaderComb(void);

	bool CompileFromFile( const std::string& fileName );
	void update();
	void destroy();

	void SetMatrix_vs( int slot, const float44& matrix );
	void SetFloat4_vs( int slot, const float4& vector );
	void SetMatrix_ps( int slot, const float44& matrix );
	void SetFloat4_ps( int slot, const float4& vector );

	void Use();

	//IDirect3DVertexShader9* getVertexShader() {return m_vs;}
	//IDirect3DPixelShader9*  getPixelShader()  {return m_ps;}
private:
	std::string m_fileName;
	NlRenderDevice *m_renderDevice;
	
	IDirect3DVertexShader9 *m_vs;
	IDirect3DPixelShader9 *m_ps;

	HANDLE m_hShaderFile;
	FILETIME m_lastUpdateTime;
};

#endif