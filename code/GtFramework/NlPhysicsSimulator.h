/*
  @file   NlPhysicsSimulator.h
  
  @author LiaoSheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlPhysicsSimulator_h_
#define _NlPhysicsSimulator_h_

#include "NlPhysicsDevice.h"

class NlGameEnvironment;
class NlPhysicsSimulator
{
public:
	NlPhysicsSimulator( float gravity = 9.8f, float paramScale = 1.0f );
	~NlPhysicsSimulator(void);

	bool Init();
	void Start();
	void Update( NlGameEnvironment* gameEnv );
	void ShutDown();

private:
	void StartPhysics( float elapsedTime );
	void GetPhysicsResults();

	void ClearGarbage();
	void ReleasePxMeshData( NxMeshData& data );

public:
	NlPhysicsDevice* m_device;
private:
	float m_paramScale;	// scale is meters per physX units
	float m_grvSize;	// gravity size
};

#endif