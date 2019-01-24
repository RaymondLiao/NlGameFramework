#pragma once

#include "prerequisite.h"
#include "NlGameObject.h"

class Basket :
	public NlCombRenderble
{
public:
	Basket( float3 sides );
	~Basket(void);

	virtual bool init( NlGameEnvironment* gameEnv );
	int	GetCollecStickCount() {return m_stickCout;}

private:
	bool LoadFromFile( NlRenderDevice* device,
		const char* objFileName, const char* mtrlFileName ) {}
	void AddSubRenderble( NlRenderble* sub ) {}

	virtual void OnTriggerEnter( const NlCollider& another );

private:
	float3 m_sides;
	int m_stickCout;

};