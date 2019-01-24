#pragma once

#include "NlGameSceneBase.h"

//====================================================
class Ground;
class Stick;
class Basket;

class SingleModeScene :
	public NlGameScene
{
public:
	enum State
	{
		Game_Playing,
		Game_Pausing,
		Game_End,
		Game_Reset,
		Game_ToMainMenu
	} m_state;

public:
	SingleModeScene(void);
	~SingleModeScene(void);

	void SetLevel( int level ) {m_level = level;}
	virtual bool OnInit( NlGameEnvironment* gameEnv );
	virtual void OnUpdate( NlGameEnvironment* gameEnv );
	virtual void OnDraw( NlGameEnvironment* gameEnv );
	virtual void OnGUI( int2 wndResolution );
	virtual void OnDestroy();

	virtual bool OnInputEvent( const SInputEvent &event );

private:
	int m_level;
	int m_stickCount;

	Ground* m_ground;
	Basket* m_basket;
	NlCloth* m_basketCloth;

	// time
	float m_remainTime;
	NlGUIText* m_timeDisplayer;

	//
	NlGUITexture* m_title, *m_pauseIF;

	// score information
	NlGUITexture *m_scoreGraph, *m_loseGraph;
	NlGUIText *m_score, *m_lose;

	// score interface
	NlGUITexture* m_scoreIF;
	NlGUIText* m_scoreSum;

};