#pragma once

#include "./GLControlBase.h"

// ----------------------------------------------------------------------------
class GLCharacter;
struct GLSKILL;
struct GLREACTION;
struct SACTIONTAR;
struct STARGETID;

class GLControlTypeA : public GLControlBase
{

public:
	GLControlTypeA( GLGaeaClient* pGaeaClient );
	virtual ~GLControlTypeA();

//! 외부 이벤트 처리
public:
	virtual void ExternEvent_SelectAround();
	virtual void ExternEvent_SkillTrayLeftClickUp(const WORD wQUICKSKILLSLOT);
	virtual void ExternEvent_SkillTrayLeftClickUp( const SNATIVEID& sID, const WORD wQUICKSKILLSLOT );

private:
	virtual void DoInit() { /*Blank*/ }
	virtual void DoInput( float fTime, float fElapsedTime );

	virtual void DoInputCamera( float fTime, float fElapsedTime );
	virtual void DoUpdate( float fTime, float fElapsedTime );
	virtual const bool DoIsKeyboardBlock( float fTime, float fElapsedTime )  { return false; }
	virtual const bool DoIsMouseBlock( float fTime, float fElapsedTime )	 { return false; }
	virtual const bool DoIsShortcutsBlock( float fTime, float fElapsedTime ) { return false; }
	virtual const bool DoIsCharacterBlock( float fTime, float fElapsedTime ) { return false; }
	virtual void DoProc_CameraMove(const SKEYCAPS&, const UINT nMoveDir) {}
	virtual void DoProc_CameraRotate(const SKEYCAPS& cKeyCaps, const UINT nMoveDir) {}
	virtual void DoProc_CameraMoveSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown) {}
	virtual void DoProc_CameraRotateSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown) {}
	virtual void DoProc_SelectTarget(const SKEYCAPS& cKeyCaps){}
	virtual void DoProc_CameraInterpMoveTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown){}
	virtual void DoProc_CameraInterpRotateTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown){}
	virtual void DoProc_CameraInterpTargetSelectTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown){}
	virtual void DoProc_CameraInterpClear(const SKEYCAPS&){}
	virtual void DoProc_CameraPlay(const SKEYCAPS&){}
	virtual void DoProc_CameraStop(const SKEYCAPS&){}
	virtual void DoProc_CameraRewind(const SKEYCAPS&){}
	void DoSelectAround();
private:
	void DoInputRunSkill( GLCharacter* pCharacter, 
		GLSKILL* pRunSkill, 
		SACTIONTAR &sACTAR, 
		D3DXVECTOR3& vFromPt,
		D3DXVECTOR3& vTargetPt, 
		STARGETID &sTARID, 
		DWORD dwRunSkillKeyState, 
		GLREACTION &sReaction, 
		float fTime );

	void DoInputNotRunSkill( GLCharacter* pCharacter, 
		DWORD dwActionKeyState, 
		GLREACTION &sReaction, 
		SACTIONTAR &sACTAR, 
		STARGETID &sTARID, 
		bool bcontinue, 
		float fTime, 
		bool &bReaction, 
		bool &bSelect );

};