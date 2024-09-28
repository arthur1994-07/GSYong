//bjju.InputManager
//

#pragma once

#include <map>
#include <list>
#include <boost/function.hpp>

#include "GLInputManager_KeySetting.h"

class GLGaeaClient;
class CKeySetting;
struct SACTIONTAR;

// ----------------------------------------------------------------------------

class GLControlBase;

struct SKEYCAPS
{
	typedef boost::function<void(GLControlBase*, const SKEYCAPS&)> FN_INPUTPROC;

	EMSHORTCUTS		emShortcuts;
	FN_INPUTPROC	fnProc;
	FN_INPUTPROC	fnNonEventProc;
	DWORD			dwKeyState;
	float			fElapsedTime;

	SKEYCAPS()
		: emShortcuts(EMSHORTCUTS_NULL)
		, dwKeyState(0)
		, fElapsedTime(0.0f)
	{
	}

	template<typename FN>
	SKEYCAPS( const FN& _fnProc )
		: emShortcuts(EMSHORTCUTS_NULL)
		, fnProc(_fnProc)
		, dwKeyState(0)
		, fElapsedTime(0.0f)
	{
	}

	template<typename FN>
	SKEYCAPS( const FN& _fnProc, const FN& _fnNonEventProc )
		: emShortcuts(EMSHORTCUTS_NULL)
		, fnProc(_fnProc)
		, fnNonEventProc(_fnNonEventProc)
		, dwKeyState(0)
		, fElapsedTime(0.0f)
	{
	}
};

class GLControlBase
{

public:
	typedef std::map<EMSHORTCUTS, SKEYCAPS>	MAP_INPUTCAPS;
	typedef MAP_INPUTCAPS::iterator			MAP_INPUTCAPS_ITER;

	typedef std::list<SKEYCAPS*>			LIST_PINPUTCAPS;
	typedef LIST_PINPUTCAPS::iterator		LIST_PINPUTCAPS_ITER;

	typedef std::map<SSHORTCUTS::KEY, bool>	MAP_USEDKEYS;
	typedef MAP_USEDKEYS::iterator			MAP_USEDKEYS_ITER;

public:
	GLControlBase( GLGaeaClient* pGaeaClient );
	virtual ~GLControlBase();

public:
	void Init( CKeySetting& cKeySetting );
	void FrameMove( float fTime, float fElapsedTime );
	const bool IsKeyboardBlock( float fTime, float fElapsedTime );  // 키보드 블록 상태인가.
	const bool IsMouseBlock( float fTime, float fElapsedTime );		// 마우스 블록 상태인가.
	const bool IsShortcutsBlock( float fTime, float fElapsedTime ); // 단축키 블록 상태인가.
	const bool IsCharacterBlock( float fTime, float fElapsedTime ); // 캐릭터 이동 블록 상태인가.
	const bool IsCameraBlock();

private:
	float m_fGfxUICheckTime;
	float m_fMaxGfxUICheckTime;
	bool m_bFixedCusor;

	void Input( float fTime, float fElapsedTime );
	void Update( float fTime, float fElapsedTime );

private:
	virtual void DoInit() = 0;
	virtual void DoInput( float fTime, float fElapsedTime ) = 0;
	virtual void DoInputCamera( float fTime, float fElapsedTime ) = 0;
	virtual void DoUpdate( float fTime, float fElapsedTime ) = 0;
	virtual const bool DoIsKeyboardBlock( float fTime, float fElapsedTime )  = 0;
	virtual const bool DoIsMouseBlock( float fTime, float fElapsedTime )	 = 0;
	virtual const bool DoIsShortcutsBlock( float fTime, float fElapsedTime ) = 0;
	virtual const bool DoIsCharacterBlock( float fTime, float fElapsedTime ) = 0;
	virtual void DoProc_SelectTarget(const SKEYCAPS& cKeyCaps) = 0;
	virtual void DoProc_CameraMove(const SKEYCAPS& cKeyCaps, const UINT nMoveDir) = 0;
	virtual void DoProc_CameraRotate(const SKEYCAPS& cKeyCaps, const UINT nMoveDir) = 0;
	virtual void DoProc_CameraMoveSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown) = 0;
	virtual void DoProc_CameraRotateSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown) = 0;
	virtual void DoProc_CameraInterpMoveTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown) = 0;
	virtual void DoProc_CameraInterpRotateTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown) = 0;
	virtual void DoProc_CameraInterpTargetSelectTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown) = 0;
	virtual void DoProc_CameraInterpClear(const SKEYCAPS&) = 0;
	virtual void DoProc_CameraPlay(const SKEYCAPS&) = 0;
	virtual void DoProc_CameraStop(const SKEYCAPS&) = 0;
	virtual void DoProc_CameraRewind(const SKEYCAPS&) = 0;

private:
	const bool CheckDuplicationShockcut( const SSHORTCUTS& sShortcuts ); // 중복키 체크
	void	   RegisterUsedKey( const SSHORTCUTS& sShortcuts );			 // 키사용 등록

protected:
	void DefaultCameraUpdate ( float fTime, float fElapsedTime );
	void DefaultCursorUpdate ( float fTime, float fElapsedTime, SACTIONTAR& sActorTar );
	void DefaultMovingUpdate ( float fTime, float fElapsedTime );  // 기본 이동키 체크
		void DefaultMovingUpdate_Self(const float fTime, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vTargetPt);		
		void DefaultMovingUpdate_Domination(const float fTime, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vTargetPt);
		void DefaultMovingUpdate_Random(void);	

	BOOL CharBlockCursorUpdate(void);

//! 단축키 프로시져
public:
	virtual void Proc_QuickSlot(const SKEYCAPS&, const UINT nIndex);
	virtual void Proc_SkillSlot(const SKEYCAPS&, const UINT nIndex);
	virtual void Proc_SkillTab(const SKEYCAPS&, const UINT nIndex);
	virtual void Proc_SkillTrayToggle(const SKEYCAPS&);
	virtual void Proc_ArmsSlotSwap(const SKEYCAPS&);

	virtual void Proc_UseChatMacro(const SKEYCAPS&, const UINT nIndex);

	virtual void Proc_Inventory(const SKEYCAPS&);
	virtual void Proc_Character(const SKEYCAPS&);
	virtual void Proc_Skill(const SKEYCAPS&);
	virtual void Proc_Party(const SKEYCAPS&);
	virtual void Proc_Quest(const SKEYCAPS&);
	virtual void Proc_Club(const SKEYCAPS&);
	virtual void Proc_Friend(const SKEYCAPS&);
	virtual void Proc_ItemBank(const SKEYCAPS&);
	virtual void Proc_MiniMap(const SKEYCAPS&);
	virtual void Proc_PointShop(const SKEYCAPS&);
	virtual void Proc_ChatMacro(const SKEYCAPS&);
	virtual void Proc_SNS(const SKEYCAPS&);
	virtual void Proc_Help(const SKEYCAPS&);
	virtual void Proc_QBox(const SKEYCAPS&);
	virtual void Proc_Pet(const SKEYCAPS&);
	virtual void Proc_Vehicle(const SKEYCAPS&);
	virtual void Proc_Product(const SKEYCAPS&);

	virtual void Proc_SchoolRecord(const SKEYCAPS&);
	virtual void Proc_BattleGround(const SKEYCAPS&);

	virtual void Proc_PickupItem(const SKEYCAPS&);
	virtual void Proc_ForcedPKMode(const SKEYCAPS&);
	virtual void Proc_RunMode(const SKEYCAPS&);
	virtual void Proc_PeaceMode(const SKEYCAPS&);
	virtual void Proc_VehicleActive(const SKEYCAPS&);
	virtual void Proc_VehicleBoost(const SKEYCAPS&);

	virtual void Proc_ActionCancel(const SKEYCAPS&);

	virtual void Proc_FlyCameraControl(const SKEYCAPS&);
	virtual void Proc_CameraMove(const SKEYCAPS& cKeyCaps, const UINT nMoveDir);
	virtual void Proc_CameraRotate(const SKEYCAPS& cKeyCaps, const UINT nMoveDir);
	virtual void Proc_CameraMoveSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown);
	virtual void Proc_CameraRotateSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown);
	virtual void Proc_SelectTarget(const SKEYCAPS& cKeyCaps);
	virtual void Proc_CameraInterpMoveTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown);
	virtual void Proc_CameraInterpRotateTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown);
	virtual void Proc_CameraInterpClear(const SKEYCAPS&);
	virtual void Proc_CameraInterpTargetSelectTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown);
	virtual void Proc_CameraPlay(const SKEYCAPS&);
	virtual void Proc_CameraStop(const SKEYCAPS&);
	virtual void Proc_CameraRewind(const SKEYCAPS&);

	virtual void Proc_Confirm( const SKEYCAPS& );
	virtual void Proc_InteractDropItem( const SKEYCAPS& );

	virtual void Proc_None ( const SKEYCAPS& ) { }


//! 단축키가 눌러지지 않았을때 프로시져
public:
	virtual void NonEventProc_ForcedPKMode(const SKEYCAPS&);

//! 외부 이벤트 처리
public:
	virtual void ExternEvent_SkillTrayLeftClickUp(const WORD wQUICKSKILLSLOT);
	virtual void ExternEvent_SkillTrayLeftClickUp( const SNATIVEID& sID, const WORD wQUICKSKILLSLOT );
	virtual void ExternEvent_SelectAround();

protected:
	CKeySetting*   m_pKeySetting;

protected:
	MAP_INPUTCAPS   m_mInputCaps;		// 입력 정보
	LIST_PINPUTCAPS m_SortedInputCaps;  // 정렬된 입력 정보
	MAP_USEDKEYS	m_mUsedKeys;		// 사용된 키

protected:
	POINT			m_ptHoldCursor;		// 커서 홀드용

protected:
	GLGaeaClient*  m_pGaeaClient;

	// UI에서 시작한 드래그인가?
	bool m_bIsDragEndFromUI;

public :
	// 홀드 아이템 버리기 / 사용
	void DropHoldItem();
};