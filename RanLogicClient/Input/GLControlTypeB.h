#pragma once

#include "./GLControlBase.h"

// ----------------------------------------------------------------------------
class GLCharacter;
struct GLREACTION;

class GLControlTypeB : public GLControlBase
{

public:
	GLControlTypeB( GLGaeaClient* pGaeaClient );
	virtual ~GLControlTypeB();

//! 외부 이벤트 처리
public:
	virtual void ExternEvent_SelectAround();

private:
	virtual void DoInit();// { /*Blank*/ }
	virtual void DoInput( float fTime, float fElapsedTime );

	virtual void DoInputCamera( float fTime, float fElapsedTime );
	virtual void DoUpdate( float fTime, float fElapsedTime );
	virtual const bool DoIsKeyboardBlock( float fTime, float fElapsedTime )  { return false; }
	virtual const bool DoIsMouseBlock( float fTime, float fElapsedTime )	 { return false; }
	virtual const bool DoIsShortcutsBlock( float fTimex, float fElapsedTime ) { return false; }
	virtual const bool DoIsCharacterBlock( float fTime, float fElapsedTime ) { return false; }
	virtual void DoProc_CameraMove(const SKEYCAPS&, const UINT nMoveDir) {}
	virtual void DoProc_CameraRotate(const SKEYCAPS& cKeyCaps, const UINT nMoveDir) {}
	virtual void DoProc_CameraMoveSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown) {};
	virtual void DoProc_CameraRotateSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown) {};
	virtual void DoProc_SelectTarget(const SKEYCAPS& cKeyCaps){}
	virtual void DoProc_CameraInterpMoveTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown){}
	virtual void DoProc_CameraInterpRotateTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown){}
	virtual void DoProc_CameraInterpTargetSelectTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown){}
	virtual void DoProc_CameraInterpClear(const SKEYCAPS&){}
	virtual void DoProc_CameraPlay(const SKEYCAPS&){}
	virtual void DoProc_CameraStop(const SKEYCAPS&){}
	virtual void DoProc_CameraRewind(const SKEYCAPS&){}

public:
	virtual void Proc_ActionCancel(const SKEYCAPS&);
	virtual void Proc_SkillSlot(const SKEYCAPS&, const UINT nIndex);

private:
	void DoSelectAround();

	bool DoInputReaction( SACTIONTAR &sActionTar, 
		GLCharacter* pCharacter,
		DWORD dwActionKeyState,
		bool bCursorAction,
		GLREACTION &sReaction, 
		bool bcontinue, 
		float fTime );

};