#pragma once

#include "prerequisite.h"
#include "NlGameObject.h"

class Stick;

class Ground :
	public NlCombRenderble
{
public:
	Ground( float2 sides );
	~Ground(void);

	virtual bool init( NlGameEnvironment* gameEnv );
	virtual void Draw( NlGameEnvironment* gameEnv );
	virtual void destroy();

	int GetFallenStickCount() {return m_fallen.size();}
	// for double mode
	void SetTurn( bool playerATurn ) {m_playerATurn = playerATurn;}
	int GetFallenStickCountB() {return m_fallenB.size();}

private:
	bool LoadFromFile( NlRenderDevice* device,
		const char* objFileName, const char* mtrlFileName ) {}
	void AddSubRenderble( NlRenderble* sub ) {}

	virtual void OnCollisionEnter(  const NlCollider& another  );

private:
	float2 m_sides;

	std::vector<NlRenderble*> m_compos;
	std::vector<Stick*> m_fallen;

	// for double mode
	bool m_playerATurn;
	std::vector<Stick*> m_fallenB;

};