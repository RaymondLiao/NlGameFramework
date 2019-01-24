#pragma once

#include "NlGameObjPri.h"

enum EStickType
{
	eStick_wood,
	eStick_rubber,
	eStick_glass,
	eStick_china
};

class NlGameEnvironment;
//====================================================
class Stick:
	public NlCuboid
{
public:
	Stick( EStickType type, float3 sides );
	~Stick();

	virtual bool init( NlGameEnvironment* gameEnv );
	bool IsTouchedGround() {return m_touchGround;}

	void SetBase()	{m_isBase = true;}
	bool CheckBase() {return m_isBase;}

protected:
	virtual void OnCollisionEnter( const NlCollider& another );

private:
	EStickType m_type;

	bool m_isBase;
	bool m_touchGround;

};