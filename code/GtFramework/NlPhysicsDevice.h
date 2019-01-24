/*
  @file   NlPhysicsDevice.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlPhysicsDevice_h_
#define _NlPhysicsDevice_h_
#include "prerequisite.h"

class NlClidEventManager;
class NlPhysicsDevice
{
	friend class NlPhysicsSimulator;

public:
	NlPhysicsDevice(void);
	~NlPhysicsDevice(void);

	NxPhysicsSDK* GetSDK() {return m_sdk_obj;}
	NxScene* GetScene() {return m_scene;}
	NlClidEventManager* GetClidEventManager() {return m_clidEventManager;}
	float GetElapsedTime() {return m_elapsedTime;}

	void ReleaseActor( NxActor* actor ) {m_actorGarbage.push_back( actor );}
	void ReleaseJoint( NxJoint* joint ) {m_jointGarbage.push_back( joint );}
	void ReleaseCloth( NxCloth* cloth ) {m_clothGarbage.push_back( cloth );}
	void ReleaseClothMesh( NxClothMesh* mesh ) {m_clothMeshGarbage.push_back( mesh ); }

	void ResetDevice();

private:
	bool init( float gravity, float paramScale );
	void update( float elapsedTime );
	void destroy();

private:
	NxPhysicsSDK* m_sdk_obj;
	NxScene* m_scene;
	
	NlClidEventManager* m_clidEventManager;

	float m_elapsedTime;

	float m_paramScale;
	float m_gravity;
	bool m_runHardware;
	
	std::vector<NxActor*> m_actorGarbage;			// actors need to be released
	std::vector<NxJoint*> m_jointGarbage;			// joints need to be released
	std::vector<NxCloth*> m_clothGarbage;			// cloths need to be released
	std::vector<NxClothMesh*> m_clothMeshGarbage;	// cloth meshes need to be released
	bool m_reset;

};

#endif