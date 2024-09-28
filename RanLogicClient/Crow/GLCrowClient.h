#ifndef GLCROWCLIENT_H_
#define GLCROWCLIENT_H_

#pragma once

#include "../../EngineLib/NaviMesh/Actor.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"

#include "../../RanLogic/Crow/GLCrowData.h"
#include "../../RanLogic/GLogicEx.h"
#include "../../RanLogic/NpcTalk/NpcTalkCondition.h"
#include "../ClientActor.h"


#ifndef GLLandManClient
	class GLLandManClient;
	class GLClientNode;
	typedef SQUADNODE<GLClientNode> CLIENTQUADNODE;
#endif //GLLandMan

struct SCompleteQuestState
{
	DWORD				dwQuestID;
	SNpcTalkCondition	sNPCComCondition;

	SCompleteQuestState()
		: dwQuestID(UINT_MAX)
	{
		sNPCComCondition.Reset();
	}
};

typedef std::vector<SCompleteQuestState> VEC_CQSTATE;
typedef VEC_CQSTATE::iterator		VEC_CQSTATE_ITER;

struct SCompeteNPCState
{
	BOOL		bQuestNPC;
	BOOL		bViewEffect;
	D3DXMATRIX	matEffect;
	VEC_CQSTATE vecCQState;

	void AddCQState( const DWORD dwQuestID, const SNpcTalkCondition& sNPCComCondition )
	{
		SCompleteQuestState sCQState;
		sCQState.dwQuestID		  = dwQuestID;
		sCQState.sNPCComCondition = sNPCComCondition;
		vecCQState.push_back(sCQState);
	}


	SCompeteNPCState()
		: bQuestNPC(FALSE)
		, bViewEffect(FALSE)
	{
	}

	void Init()
	{
		vecCQState.clear();
		bViewEffect = FALSE;
		bQuestNPC	= FALSE;
	}

};

class GLGaeaClient;
class GLTransformSet;
class SkinCharMulti;

class GLCrowClient : public GLCROWLOGIC, public ClientActor
{
public:
    GLCrowClient(GLGaeaClient* pGaeaClient);
    virtual ~GLCrowClient(void);

protected:

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
//	Actor				m_actorMove;
	DxSkinChar*	m_pSkinChar;
	SkinCharMulti* m_pSkinCharMulti;
	GLTransformSet*	m_pTransformSet;
	BOOL m_bVisible; // 은신등으로 인해 플레이어(나)에게 보여지는지에 대한 플래그, false일 경우 렌더링/타겟팅 되지 않음;
		
	float				m_fBillboardAlpha;

	float				m_fScale;

	//	Note : AABB
protected:
	D3DXVECTOR3		m_vMaxOrg;
	D3DXVECTOR3		m_vMinOrg;

public:
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
	float			m_fHeight;

	//	Note : 제어 코드.
	//
public:

	GLLandManClient*		m_pLandManClient;	//	맵 인스턴스 포인터.


	//	Note : 행동.
protected:
	float				m_fAge;					//	경과 시간.
//	EMACTIONTYPE		m_Action;				//	행동.	

	STARGETID			m_sTargetID;			//	목표 유닛.

	WORD				m_wTARNUM;				//	타겟 숫자.
	STARID				m_sTARIDS[EMTARGET_NET];//	타겟들.
	D3DXVECTOR3			m_vTARPOS;				//	타겟 위치.

	SCROWATTACK*		m_pAttackProp;			//	공격 속성.

	DWORD				m_dwANISUBSELECT;		//	서브 애니메이션

	//	퀘스트 완료 NPC 인지 체크
	SCompeteNPCState	m_sCQState;

protected:
	float				m_fIdleTime;			//	휴식 시간.
	float				m_fStayTimer;			//	휴식 딜래이.
	float				m_fAttackTimer;			//	공격 딜래이.
	float				m_fTargetTimer;			//	타갯 검색 타이머.
	float				m_fMoveDelay;			//	이동 딜래이.

	void UpdateActState(void);

private:
	std::tr1::shared_ptr<ActorWorldLight>		m_spWorldLight;
	std::tr1::shared_ptr<ActorNameCollision>	m_spNameCollision;

public:
	BOOL IsHaveVisibleBody();

	BOOL	IsVisibleDisplayName() const;
	
	virtual BOOL IsDominated() const			{ return m_sDOMINATION.emController == EMSPEC_DOMINATE_TYPE_CASTER; }
	virtual BOOL IsVisibleDetect(void) const	{ return m_bVisible; }

	void UpdateQuestNpcState();

private:
	const BOOL CalcVisibleDetect(const float fTime);
	const BOOL	CalcVisibleDetectAuto(const float fTime, const float fDistance, const D3DXVECTOR3& vDistance);	
	
public:
	virtual BOOL IsNonDamaged() const;

public:	
	void SetQuestCompleteNPC(const DWORD dwQuestID = UINT_MAX);
	void SetQuestCompleteNPC(const std::vector<DWORD>& vecQuestID);

public:
	HRESULT CreateCrow( GLLandManClient *pLandManClient, SDROP_CROW *pCrowDrop, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	float GetDirection();
	DxSkinChar* GetCharSkin()						{ return m_pSkinChar; }

public:
    virtual void FactEffectProcessing ( const EMFACTEFFPROCESSING emFactEffactCase );

public:
	virtual float GetMoveVelo();
	void TurnAction( EMACTIONTYPE toAction );

public:
    bool IsNpcActionable(
        const D3DXVECTOR3& vReqPos,
        const float fRadius,
        int nNpcAction=-1 )
    { 
        return ISNPC_INTERACTIONABLE(vReqPos, GetPosition(), fRadius, nNpcAction);
    }

public:
    virtual const DWORD GetCharID() const	override { return m_dwGaeaID; }
	virtual const SNATIVEID GetCrowID(void) const override { return m_sNativeID; }
    const STARGETID& GetTargetID() const { return m_sTargetID; }

public:
	virtual const EMCROW GetCrow(void) const override { return GETCROW(); }
	virtual void SetGaeaID(const DWORD dwGaeaID) override
	{
		m_dwGlobalID = dwGaeaID;
		m_dwGaeaID = dwGaeaID;
	}
	virtual const char* GetName() const		{ return m_pCrowData->GetName(); }
	virtual WORD GetLevel() const			{ return GETLEVEL(); }
	virtual GLDWDATA GetHp() const			{ return GLDWDATA(m_dwNowHP,GETMAXHP()); }
	virtual const D3DXVECTOR3 &GetMaxOrg()	{ return m_vMaxOrg; }
	virtual const D3DXVECTOR3 &GetMinOrg()	{ return m_vMinOrg; }

	virtual WORD GetBodyRadius();
	virtual float GetBodyHeight()			{ return m_fHeight; }
	virtual D3DXVECTOR3 GetPosBodyHeight()	{ return GetPosition() + D3DXVECTOR3(0.0f, m_fHeight + m_sAirborne.getHeight() + m_fInfluenceY, 0.0f); }

public:
	const BOOL IsNonTargetable(void) const;
    const bool IsHideNaming() const;
	// - 타겟을 선택하였을 때 활성화되는 툴팁을 보여줄지의 여부;
	const bool IsHideTooltip(void) const;

public:	
	virtual void ReceiveDamage(const int nDamage, DWORD dwDamageFlag, WORD wSchool, const STARGETID& sACTOR) override;
	virtual void ReceiveHeal(const int nHeal, DWORD dwHealFlag, WORD wSchool, const STARGETID& sACTOR) override;
	virtual void ReceiveBlocking(const STARGETID& sACTOR) override;
	virtual void ReceiveAVoid( const STARGETID& sACTOR ) override;
	virtual void ReceiveSwing() override;

	void RestoreActStateByInvisible(); // 상태 전환 및 스킬 관련 정리와 함께 정리 해야 함;

public:
	virtual void UPDATE_DATA_FINISH(const DWORD dwSkillfactIndex){ UPDATE_DATA_END(dwSkillfactIndex); }
protected:
	HRESULT UpdateAnimation( float fTime, float fElapsedTime );	
	virtual void UPDATE_DATA_END(const DWORD dwSkillFactIndex);	
	
	void AddSkillFact_SpecialAddon_Client(const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF, const bool bUseMobRate = false);

	const BOOL RECEIVE_SKILLFACT_AIRBORNE(const SKILL::SSPEC& sSPEC, const SKILL::CDATA_LVL& sSKILL_DATA);

	const BOOL RECEIVE_SKILLFACT_Basic ( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF );
	const BOOL RECEIVE_SKILLFACT_Addon( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF );
	const BOOL RECEIVE_SKILLFACT_Special( PGLSKILL pSkill, const WORD wlevel, const DWORD dwCount, SSKILLFACT& sSKILLEF ,const STARGETID& sID );
	const BOOL RECEIVE_SKILLFACT_BasicAction( PGLSKILL pSkill, const WORD wlevel, const SSKILLFACT::STIME sTime, SSKILLFACT& sSKILLEF );

	const bool RECEIVE_SKILLFACT_EntranceExit( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID );

	const BOOL RECEIVE_SKILLFACT( const GLMSG::SNETPC_SKILLHOLD_BRD* pMsg );
	const BOOL RECEIVE_SKILLFACT(
		const SNATIVEID& skill_id, 
		const WORD wlevel, 
		const DWORD dwSELECT,
        const STARGETID& sID,
		const SSKILLFACT::STIME sTime=SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME), 
		const DWORD dwCount = 0 );
	const BOOL RECEIVE_SKILLCA(
		const GLMSG::SNETPC_SKILLHOLD_BRD* pMsg );

protected:
	float m_fattTIMER;
	int m_nattSTEP;

protected:
	void StartAttackProc();
	BOOL AttackProc( float fElapsedTime );
	void AttackEffect( const SANIMSTRIKE &sStrikeEff );

protected:
	void StartSkillProc();
	void StartSkillDealyAction(const SCHARSKILL& sSkill, SCROWDATA* pCrowData, const SKILLTARGETDATA* pTargetData, bool bSubSkill );
	BOOL SkillProc( float fElapsedTime );

	void SKT_EFF_HOLDOUT(const STARGETID& sTarget, DWORD dwDamageFlag );

	void SK_EFF_TARG( const PGLSKILL pSkill, const SANIMSTRIKE &sStrikeEff, const STARGETID &sTarget );
	void SK_EFF_SELFZONE( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget=NULL );
	void SK_EFF_TARZONE( const STARGETID &sTarget, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF );
	void SkillEffect( const SANIMSTRIKE &sStrikeEff );
	void SkillEffectDealyAction(  const SANIMSTRIKE &sStrikeEff, SNATIVEID sSkillId, SKILLTARGETDATA *sSkillTargetData );

private:
	void SELECTANI( DxSkinChar* pRenderChar, const TCHAR* szAnim, DWORD dwFlags=NULL);
	void SELECTANI( DxSkinChar* pRenderChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0);
	void DEFAULTANI( DxSkinChar* pRenderChar );
	void TOSTARTTIME( DxSkinChar* pRenderChar );
	void TOENDTIME( DxSkinChar* pRenderChar );
	void SELECTSKILLANI (  DxSkinChar *pSkinChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE );
	BOOL IsANI_SELECTANI ( DxSkinChar *pSkinChar, const TCHAR* szAnim, DWORD dwFlags=NULL );
	BOOL IsANI_SELECTANI( DxSkinChar *pSkinChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0 );

public:
	BOOL PutPassiveEffect ( DxSkinChar *pSkinChar, const char* szEffFile, const D3DXVECTOR3 *pDir, float fTarDir=0.0f );
	void OutEffect ( DxSkinChar *pSkinChar, std::string strEffFile );
	//void CalculateAmbientColor( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, const D3DXVECTOR3& vPos );	// Ambient Color를 계산한다.
	void SetPositionSkin(  DxSkinChar *pSkinChar, const D3DXVECTOR3& vPos );
	HRESULT FrameMoveSkin( DxSkinChar *pSkinChar, 
		const float fTime, 
		const float fElapsedTime,
		BOOL bNoneOverFrame,
		const D3DXMATRIX &matRot, 
		const BOOL bContinue=TRUE, 
		BOOL const bFreeze=FALSE, 
		const BOOL bAttackMode=TRUE, 
		const BOOL bBoardRender = FALSE );

	BOOL		IsCollisionVolume();
	HRESULT 	FrameMove( const boost::shared_ptr<DxLandMan>& spLandMan, float fTime, float fElapsedTime );

	HRESULT 	RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bWorldSystemOLD );
	HRESULT		RenderShadow_SpotLight ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bUsedMaterialSystem, DWORD dwSlot );
	HRESULT 	Render( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );
	HRESULT 	RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	HRESULT 	RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );

private:
	void RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar* pRenderChar, BOOL bOpaqueState, BOOL bHALF_VISIBLE );

public:
	HRESULT 	RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT 	InvalidateDeviceObjects();

	void		MoveTo( const D3DXVECTOR3& vPos, const DWORD dwCell, const DWORD dwFlags );

	//////////////////////////////////////////////////////////////////////////
	//						AnimationMove
private:
	void SkinAniControl_AnimationMove();
	//						AnimationMove
	//////////////////////////////////////////////////////////////////////////

public:
    virtual void MsgProcess(NET_MSG_GENERIC* pMsg);

    void 		MsgCrowAttack(NET_MSG_GENERIC* pMsg);
    void 		MsgCrowSkill(NET_MSG_GENERIC* pMsg);
	void 		MsgCrowSkillDealyAction(NET_MSG_GENERIC* pMsg);
	void		MsgCrowEndSkill(NET_MSG_GENERIC* pMsg);
    void		MsgCrowUpdateState(NET_MSG_GENERIC* pMsg);
    void 		MsgCrowAttackBlocking(NET_MSG_GENERIC* pMsg);
    void 		MsgCrowAvoid(NET_MSG_GENERIC* pMsg);
    void 		MsgCrowDamage(NET_MSG_GENERIC* pMsg);
    void 		MsgActionBrd(NET_MSG_GENERIC* pMsg);	
    void 		MsgSkillFactBrd(NET_MSG_GENERIC* pMsg);
    void 		MsgSkillHoldBrd(NET_MSG_GENERIC* pMsg);
    void 		MsgSkillHoldRsBrd(NET_MSG_GENERIC* pMsg);
    void 		MsgStateBlowBrd(NET_MSG_GENERIC* pMsg);
    void 		MsgCureStateBlowBrd(NET_MSG_GENERIC* pMsg);
    void 		MsgPushPullBrd(NET_MSG_GENERIC* pMsg);
    void 		MsgCrowMoveTo(NET_MSG_GENERIC* pMsg);
    void 		MsgCrowAuthed(NET_MSG_GENERIC* pMsg);

	// 스킬 효과 적용;
	void MsgApplySkillEffect ( NET_MSG_GENERIC* pMsg );
};

typedef GLCrowClient* PGLCROWCLIENT;

typedef sc::CGLLIST<PGLCROWCLIENT>	GLCROWCLIENTLIST;
typedef sc::SGLNODE<PGLCROWCLIENT>	GLCROWCLIENTNODE;

#endif // GLCROWCLIENT_H_
