#ifndef _GLOGIC_SERVER_GLSUMMON_FIELD_H_
#define _GLOGIC_SERVER_GLSUMMON_FIELD_H_

#pragma once

#include "../../SigmaCore/Container/GLList.h"

#include "../../Enginelib/NaviMesh/actor.h"

#include "../../RanLogic/Summon/GLSummon.h"
#include "../../RanLogic/GLogicEx.h"

#include "../AroundSlot/GLARoundSlot.h"
#include "../ServerActor.h"

//#define _USE_SUMMON_COMPONET_

class GLChar;
class GLGaeaServer;
struct SSKILLACTEX;

namespace AISystem
{
	class CAICompField;
}

class GLSummonField : public GLSUMMON, public ServerActor
{
public:

	enum EMRETURN_SETREACTION
	{
		EMRETURN_SETREACTION_FAILED,				// 일반 실패
		EMRETURN_SETREACTION_FAILED_RANGE,			// 사정거리 실패
		EMRETURN_SETREACTION_FAILED_SKILL,			// 스킬 딜레이 등. 실패
		EMRETURN_SETREACTION_FAILED_COLLISION,		// 지형과 충돌해서 실패
		EMRETURN_SETREACTION_SUCCESS,				// 성공
	};

	enum EMREACTION
	{
		EMREACT_NONE	= 0,
		EMREACT_ATTACK	= 1,
		EMREACT_SKILL	= 2,
	};

	enum EMSCHEDULETYPE
	{
		EMSCHEDULETYPE_NONE = 0,
		EMSCHEDULETYPE_DIE  = 1,
	};

public:
	// Note : 예약행위 구조체
	struct SSCHEDULEDACTION
	{
		EMSCHEDULETYPE emAction;
		D3DXVECTOR3	   vTarPos;
		STARGETID	   sTargetID;

		SSCHEDULEDACTION() 
			: emAction(EMSCHEDULETYPE_NONE)
			, vTarPos(D3DXVECTOR3(0,0,0))
		{
			sTargetID.RESET();
		}

		void Assign ( const SSCHEDULEDACTION& rValue )
		{
			emAction  = rValue.emAction;
			vTarPos   = rValue.vTarPos;
			sTargetID = rValue.sTargetID;
		}

		void Reset ()
		{
			emAction  = EMSCHEDULETYPE_NONE;
			vTarPos   = D3DXVECTOR3(0,0,0);
			sTargetID.RESET();
		}
	};

private:
	//	데미지 큐
	struct SSTRIKE
	{
		WORD wDivKey;
		bool bHit;

		SSTRIKE () : wDivKey(0), bHit(false) { }
		SSTRIKE ( WORD _wDivKey, bool _bHit ) : wDivKey(_wDivKey), bHit(_bHit) { }
	};
	typedef std::deque<SSTRIKE>	HITLIST;
	typedef HITLIST::iterator	HITLIST_ITER;

public:
    GLSummonField(GLGaeaServer* pServer);
    virtual ~GLSummonField();
   
public:	
	//sc::SGLNODE<GLSummonField*>* m_pGlobNode;	 //	맵 리스트 노드.
	LANDQUADNODE*			 m_pQuadNode;		 //	쿼드 트리 노드.
	sc::SGLNODE<GLSummonField*>* m_pCellNode;	 // 셀 리스트 노드.

public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	DxSkinChar*			m_pSkinChar;

	D3DXVECTOR3			m_vBackupPosition;

public:
	D3DXVECTOR3			m_vTarPos;
	STARGETID			m_TargetID;

	SCROWATTACK			m_sAttackProp;			// 공격 속성.

	D3DXVECTOR3			m_vStartPos;			// 이동메시지 보낼때 시작위치

protected:
	GLARoundSlot		m_ARoundSlot;			// 피공격시에 자신의 주변에 몰려들수 있는 위치가 있는지 점검.

	float			    m_fIdleTime;
	float			    m_fGestureStartTime;    //  제스쳐 시작시간.

	DWORD               m_dwAtypeAfterTracing;  //  추적 후 사용할 AType 
	WORD                m_wRangeTracing;        //  추적에 사용할 Range 

	WORD				m_wAniSub;
	float				m_fattTIMER;			//	공격 프래임.
	DWORD               m_dwReactCount;         //  리액션 횟수.
	HITLIST				m_sHITARRAY;

	STARGETID			m_sAssault;				//	가장 최근에 자신을 공격한자.
	STARGETID			m_sHeal;					// 가장 최근에 자신을 치유한자.

	WORD				m_wTARNUM;				//	타겟 숫자.
	STARID				m_sTARIDS[EMTARGET_NET];//	타겟들.
	D3DXVECTOR3			m_vTARPOS;				//	타겟 위치.

	bool				m_bRunAway;				// 도망 모드;
	
	bool				m_bUnlimit;				// 무제한 모드;
	bool				m_bSpeedUp;				// 스피드업 모드;

protected:
	SSCHEDULEDACTION	m_ScheduledAction;		// 예약된 행위

private:
	DWORD				m_dwEventSkillIdx;
	bool				m_bEventSkill;
	bool				m_bDelay;
	float				m_fDelayTime;

	int					m_nSkillFramCount;

	bool				m_bHire;				// 고용 소환수인가?;

public:
	inline const bool IsEventSkill () const { return m_bEventSkill; }
	inline void SetEnableEventSkill ( bool bEventSkill ) { m_bEventSkill = bEventSkill; }
	inline const bool IsHireSummon () const { return m_bHire; }
	inline void SetHireSummon ( bool bHire ) { m_bHire = bHire; }

public:
	const DWORD GetAIType () const;
	void SetAIType ( DWORD dwType );

	// Target을 자동검색하여 반환한다;
	STARGETID AutoEventSkillSearchTarget ( UINT iIdx );

	const D3DXVECTOR3& GetTarPos() { return m_vTARPOS; }

public:
	virtual void ApplyDelay ( float fDelayTime );
	virtual bool GetDelayState ();
	virtual ServerActor* GetOwner ();						// 주인을 불러온다;
	virtual bool IsTarget ();								// 타겟이 있는지 확인;
	virtual bool IsTargetExcludeOwner ();
	virtual bool IsValidOwnerDistance ();					// 주인과의 거리 유효성 판단;
	virtual bool IsValidEventSkillDistance ();				// 이벤트 스킬을 위한 주인과의 거리 유효성 판단;
	virtual bool IsRunAway ();
	virtual const DWORD GetActionCount () const;			// 액션 횟수;
	virtual void AutoSearchTarget ();						// 자동 타겟 검색;
	virtual void SetOwnerTarget ();							// 주인이 있는 경우 주인의 타겟을 타겟팅함;
	virtual void StartTargetAction ( DWORD dwCount = 0 );	// 타겟 액션 시작;
	virtual void DieAction ();								// 죽음;
	virtual void ChangeSkillPage ( UINT iPage );			// 스킬 페이지 변경;
	virtual void UseEventSkill ( UINT iIdx );				// 특정 이벤트 스킬 사용;
	virtual void RunAwayAction ( bool bSpeedUp = false );	// 도망 액션;
	virtual void NormalAction ();							// 일반 액션;
	virtual void TracingOwner ( bool bSpeedUp = false );	// 주인을 따라다닌다;
	virtual void TracingTarget ( bool bSpeedUp = false );	// 타겟을 따라다닌다;
	virtual void SyncPositionTarget ();						// 타겟의 위치와 위치를 동기화한다;

	// 스킬 효과를 적용시킨다;
	// 적용 후 반드시 해제해줘야 한다;
	virtual void Update_ApplySkillEffect ( EMSPEC_ADDON emSpecAddon, bool bState );

public:
	void	ViewSkillTargetEffect ( SNATIVEID sSkillID );

private:

	BOOL	IsOtherWay( PGLSKILL pSkill, WORD wLv );
	void	StartStrikeProc ( BOOL bSkill, BOOL bLowSP );
	void	StartAttackProc ();
	void    StartSkillProc  ();

	void	SelfSkillStart( SNATIVEID sSkillID, WORD wLevel );
	BOOL    IsSkillLoopEnd();
	BOOL   IsSkillEnd( SNATIVEID sSkill );

	//  대기 프로세스
	void    IdleProcess     ( float fElapsedTime );

	//	공격 프로세스.
	BOOL	ShockProcess    ( float fElapsedTime );
	BOOL	AttackProcess   ( float fElapsedTime );

	// 회피 프로세스
	void AvoidProc(const SNATIVEID& SkillID = NATIVEID_NULL(), const WORD wSkillLevel = 0);

	// 방어 프로세스
	void BlockProc(const STARGETID& sTargetID, const SNATIVEID& SkillID = NATIVEID_NULL(), const WORD wSkillLevel = 0);

	// 데미지 프로세스
	void	DamageProc      ( const int nDAMAGE, DWORD dwDamageFlag );
	DWORD	ToDamage        ( const STARGETID &sTargetID, const int nDamage, const DWORD dwDamageFlag );


public:
	// 스킬 프로세스
	BOOL	SkillProcess    ( float fElapsedTime  );
	void    SkillProc       ( BOOL bLowSP = FALSE );
	void	SkillProc ( BOOL bLowSP, SNATIVEID skill_id, WORD wSKILL_LVL, WORD wTARNUM, STARID *sTARIDS );
	void	ApplySkill( BOOL bLowSP, SNATIVEID skill_id, WORD wSKILL_LVL, WORD wTARNUM, STARID *sTARIDS );
	void	ApplySkillSpecial(PGLSKILL pSkill, SSKILLACTEX& sSKILLACTEX, ServerActor* const pTargetActor,STARGETID sTARID, WORD wSkillLevel);
	void	ApplySkillSpecialCA(PGLSKILL pSkill, SSKILLACTEX& sSKILLACTEX, SSKILLACTEX& sCASKILLACTEX, ServerActor* const pTargetActor,STARGETID sTARID, WORD wSkillLevel);
	
	EMATT_RGTYPE GetAttackRangeType ()			{ if ( !m_sAttackProp.bUsed ) return EMATT_SHORT; return m_sAttackProp.emAttRgType; }

	
protected:
	typedef std::vector<DWORD>		VECGUID;
	typedef VECGUID::iterator		VECGUID_ITER;

	DWORD	m_dwPC_AMOUNT;
	DWORD	m_dwCROW_AMOUNT;
	DWORD	m_dwSUMMON_AMOUNT;

	VECGUID	m_vecPC_VIEW;
	VECGUID	m_vecCROW_VIEW;
	VECGUID	m_vecSUMMON_VIEW;

	void INSERT_PC_VIEW ( DWORD dwGUID );
	void INSERT_CROW_VIEW ( DWORD dwGUID );
	void INSERT_SUMMON_VIEW ( DWORD dwGUID );

public:

	typedef std::vector<STARGETID>	VEC_SK_TAR;
	typedef VEC_SK_TAR::iterator	VEC_SK_ITER;

	VEC_SK_TAR DetectTarget ( const D3DXVECTOR3 &vDetectPosA, const D3DXVECTOR3 &vDetectPosB, const DWORD dwMaxFind, const BOOL bEnemy=TRUE );
	VEC_SK_TAR DetectTarget ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind, const BOOL bEnemy=TRUE );

public:
	HRESULT UpdateViewAround ();
	virtual HRESULT SendMsgViewAround ( NET_MSG_GENERIC* pMsg );	
	virtual void SendToClient(NET_MSG_GENERIC* pMsg);	
	virtual void SendToClient(const DWORD dwClientSlot, NET_MSG_GENERIC* pMsg);

public:
	void	   RunSchedule ( float fTime, float fElapsedTime );

	HRESULT	   UpdateTurnAction      ( float fElapsedTime );
	HRESULT	   UpdateAnimation       ( float fTime, float fElapsedTime );
	HRESULT	   UpdateDurDamage       ( float fTime, float fElapsedTime );	//	지속 데미지 처리( 데미지 준다. ) 
	const BOOL UpdateScheduledAction (); // 예약된 액션행위 처리

	void		UpdateLandEffect();
	void		UpdateMapEffect();
	void		UpdateSkillEff( float fTime, float fElapsedTime );

	HRESULT Create(
        LPDIRECT3DDEVICEQ pd3dDevice,
        GLLandMan* pLandMan,
        PGLSUMMON pSummonData,
		ServerActor* pOwner,
        D3DXVECTOR3& vPosition,
        const STARGETID& sTarget);

	virtual const EMSPEC_DOMINATE_TYPE GetController () const { return m_sDOMINATION.emController; }
	virtual void SetController ( const EMSPEC_DOMINATE_TYPE emDOMINATION_TYPE ) { m_sDOMINATION.emController = emDOMINATION_TYPE; }

	HRESULT SetPosition(GLLandMan* pLandMan);

	HRESULT FrameMove( float fTime, float fElapsedTime );

	virtual void TurnAction(const EMACTIONTYPE toAction, const bool bMsg = true) override;

	void  TurnScheduledAction ( SSCHEDULEDACTION& sSchedule );
	void  SetScheduledAction  ( SSCHEDULEDACTION& sSchedule );

	virtual DWORD GETCLIENTID();
	virtual SSKILLFACT* GETSKILLFACT() override { return m_sSKILLFACT; }

	void	RESET_DATA ();

	void	AccountSkill ( SNATIVEID sSkillID );

	// Move
	void       Move                      ( const D3DXVECTOR3& vPos , bool bMsg=true, bool bSpeedUp=false );
	void       Move_TargetTracing        ( const STARGETID& sTarget, const WORD wReactionRange, bool IsOwnerTracing=false, bool bSpeedUp=false );
	const BOOL GetRandomTargetPos        ( D3DXVECTOR3& vOutPos, const ServerActor* pTarget, const float fDistanceMin, const float fDistanceMax, bool IsOwnerTracing=false );    

	// Move Speed
	virtual float GetMoveVelo();

	// Reaction
	EMRETURN_SETREACTION    SetReactionTarget( STARGETID sTargetID, bool bEventSkill = false );
	bool                    GetReactionTarget( STARGETID sTargetID );
	void                    DetectReactionableTarget( STARGETID& sOutTargetID ); 
	DWORD                   FindOptReactionType ( const STARGETID &sTargetID, bool branvar );
    const EMREACTIONABLE    IsReActionable ( ServerActor* pTARGET, const EMIMPACT_SIDE _emTargetSide );

	// Action;
	bool IsTargetable (
		ServerActor* pTarget,
		const EMIMPACT_SIDE& emTargetSide,
		float& fTargetLength,
		bool bForceAttack = true );
	virtual STARGETID SearchTargetAction ( EMIMPACT_SIDE emTargetSide );

private:
    //const EMREACTIONABLE    IsReActionableFaction ( ServerActor* pTARGET, const EMIMPACT_SIDE _emTargetSide );
    //const EMREACTIONABLE    IsReActionableNonFaction ( ServerActor* pTARGET, const EMIMPACT_SIDE _emTargetSide );
	BOOL	   IsViewInActor( const STARGETID& sID );

public:
	// SkillFact
	void                    DisableSkillFact();
	
	// Order
	void				    Order_Die();
	
	BOOL IsRunning () const						{ return isAction(GLAT_MOVE) && IsActState(EM_ACT_RUN); };
	
	DWORD SELECT_SKILLSLOT ( SNATIVEID skill_id, EMSKILLFACT_TYPE emSkillFactType );

	virtual void SetGaeaID ( const DWORD dwGaeaID ) override
	{
		m_dwGlobalID = dwGaeaID;
		m_dwGaeaID = dwGaeaID;
	}
	
    virtual const DWORD GetCharID() const	override { return m_dwGaeaID; }

	virtual GLDWDATA GetHp () const			{ return GLDWDATA(GETHP(),GETMAXHP()); }
	// 소환수는 파티를 가질수 없지만 어쩔수 없이... ( 호출할 일 없음! )
	virtual DWORD GETPARTYID () const				{ return 0; }
	
	virtual BOOL  IsCtrlBlockBody () const;

	virtual bool IsCheckedSkillFlagSpecial ( DWORD dwFlag )	const;

	virtual BOOL IsActionLimit( DWORD dwLimit ) const { return m_dwActionLimit&dwLimit; }
	
	virtual const STARGETID&   GetTarget   () const	    { return m_TargetID; }

	virtual DWORD    GETHOLDBLOW ()   const				{ return m_dwHOLDBLOW; }
	virtual WORD     GetBodyRadius () const			    { return GETBODYRADIUS(); }

	virtual WORD     GetLevel () const					{ return GETLEVEL(); }	

	virtual DWORD    GetNowHP () const					{ return GETHP(); }
	virtual DWORD    GetMaxHP () const					{ return GETMAXHP(); }
    virtual float    GetMaxHpRate() const				{ return GETMAXHPRATE(); }
    virtual void     SetMaxHpRate( float fRate )        {  SETMAXHPRATE( fRate ); }

	virtual WORD     GetNowMP () const					{ return GETMP(); };
	virtual WORD     GetMaxMP () const					{ return GETMAXMP(); }

	virtual WORD     GetNowSP () const					{ return GETSP(); }
	virtual WORD     GetMaxSP () const					{ return GETMAXSP(); }

	virtual int				GetBasicOffenceHigh() const			{ return 0; }
	virtual int				GetBasicOffenceLow() const			{ return 0; }
	virtual int				GetBasicDefence() const				{ return GETDEFENSE(); }
	virtual const SRESIST&	GetBasicRegist() const				{ return m_pSummonCrowData->m_sResist; }
	virtual int				GetBasicHitRate() const				{ return 0; }
	virtual int				GetBasicAvoid() const				{ return GETAVOID(); }
	virtual float			GetBasicHPRestorationRate() const	{ return 0; }
	virtual float			GetBasicMPRestorationRate() const	{ return 0; }
	virtual float			GetBasicSPRestorationRate() const	{ return 0; }

	virtual int				GetOffenceHigh () const				{ return 0; }
	virtual int				GetOffenceLow () const				{ return 0; }
	virtual int				GetDefense () const					{ return GETDEFENSE(); }
	virtual const SRESIST&	GETRESIST   () const				{ return m_pSummonCrowData->m_sResist; }
	virtual int				GetHitRate () const					{ return 0; }
	virtual int				GetAvoid () const					{ return GETAVOID(); }
	virtual float			GetHPRestorationRate() const		{ return 0; }
	virtual float			GetMPRestorationRate() const		{ return 0; }
	virtual float			GetSPRestorationRate() const		{ return 0; }

	virtual const float		GetMultipleHP() const				{ return 1.f; }
	virtual const float		GetMultipleAttack() const			{ return 1.f; }
	virtual const float		GetMultipleDefense() const			{ return 1.f; }
	virtual const float		GetMultipleExp() const				{ return 1.f; }
	virtual const float		GetMultipleDrop() const				{ return 1.f; }


	virtual DWORD    GetBonusExp () const				{ return DWORD(m_pSummonCrowData->m_wBonusExp*GLCONST_CHAR::GET_EXP_SCALE()); };

	virtual EMBRIGHT GetBright () const				    { return GETBRIGHT(); }

	virtual WORD       GetSchool() const;
	virtual SCONFTING* GETCONFTING();
	
	bool			MoveTo ( const D3DXVECTOR3* pTarPos, bool bJump, DWORD dwFlags );
	ServerActor*	GetTarget(const STARGETID& sTargetID);
	bool			CheckStateToGo()													{ return true; }
	
    virtual const DWORD ReceiveDamage ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag ) override;
    const DWORD ReceiveDamage_Procedure( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag );

	virtual const DWORD ReceiveHeal(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag) override;
    const DWORD ReceiveHeal_Procedure(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag);

	virtual void  SendPushPull( const D3DXVECTOR3 &vMovePos, const float fPUSH_SPEED, const DWORD dwAniType, const bool bReActor = true );

	virtual void DO_STUN_ACTION ();
	virtual bool  STATEBLOW ( const SSTATEBLOW &sStateBlow );
	virtual void  CURE_STATEBLOW ( DWORD dwCUREFLAG );

    virtual void VAR_BODY_POINT( 
        const EMCROW emActorCrow,
        const DWORD dwActorID,
        const DWORD dwVarTypeFlag,
        int& nVar_HP,
        int& nVar_MP, 
        int& nVar_SP ) override;

	virtual void UPDATE_DATA_END ( const DWORD dwSkillfactIndex, BOOL bTimeDel = FALSE );
	virtual void UPDATE_DATA_Additional ( const float fElapsedTime );
	void UPDATE_DATA_Additional_Stigma ( SSKILLFACT& sSKEFF, const DWORD dwStigmaIndex, const DWORD dwSkillfactIndex );
	void UPDATE_DATA_Additional_Domination ( SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex );
	void UPDATE_DATA_Additional_Provoke(SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex);

	const bool RECEIVE_SKILLFACT_Domination ( const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel );
	const bool RECEIVE_SKILLFACT_Stigma ( const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel );
	const bool RECEIVE_SKILLFACT_Provoke( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID );
	

	virtual const BOOL  RECEIVE_SKILLFACT(
        const SNATIVEID& skill_id,
        const WORD wlevel,
        DWORD &dwSELECT,
        const STARGETID& sID,
        const SSKILLFACT::STIME sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME),
        const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT,
        EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NORMAL,
		const float* const pLaunchTime = 0, const float* const pLaunchValue = 0) override;

	virtual const BOOL  RECEIVE_SKILLFACTII(
		const SNATIVEID& skill_id,
		const WORD wlevel,
		DWORD &dwSELECT,
		const STARGETID& sID,
		const SSKILLFACT::STIME sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME),
		const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT,
		EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NORMAL,
		const float* const pLaunchTime = 0, const float* const pLaunchValue = 0) override;

	virtual const BOOL RECEIVE_SKILLCA(
		const SSKILLACTEX &sACTEX ) override;

    virtual const BOOL REMOVE_SKILLFACT(const SNATIVEID& skill_id) override;
    virtual const BOOL REMOVE_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON) override;

	virtual float GETSTATE_DAMAGE () const			{ return m_fSTATE_DAMAGE; }
	virtual int GETSKILLFACTSIZE() const override { return CROW_SKILLFACT_SIZE; }

	virtual const DAMAGE_SPEC&	GetDamageSpec() const		{ return m_sDamageSpec; }
	virtual DAMAGE_SPEC*		GetDamageSpecPtr()			{ return &m_sDamageSpec; }
	virtual DEFENSE_SKILL GetDefenseSkill() const	{ return m_sDefenseSkill; }

	virtual void DamageReflectionProc(int nDAMAGE, const STARGETID& sACTOR) override;
	virtual void DamageCurse(int nDAMAGE, const STARGETID& sACTOR, EMCROW emCrow) override;
	virtual void DefenseSkill(const SNATIVEID& sNativeID, WORD wLevel, const STARGETID& sACTOR) override { }

	const bool IsImmuneToSkill ( BYTE iIndex ) const;

	virtual void SKILL_EFFECT_REMOVE ( DWORD dwRemoveFlag );

	GLARoundSlot& GetARoundSlot ()			{ return m_ARoundSlot; }
	BOOL IsFreeARSlot ()					{ return m_ARoundSlot.GetAmount()!=GLARoundSlot::SLOT_SIZE; }

	// 소환자보다 소환수가 거리가 더 짧은지 체크한다.
	BOOL IsOwnerShortDistance( D3DXVECTOR3 vCheckPos );

	virtual GLARoundSlot* GETSLOT ()				{ return &m_ARoundSlot; }

	virtual SINCREASEEFF* const GetIncreaseEff() { return &m_sIncreaseEff; }
	virtual SIGNORED_DAMAGE* const GetIgnoredDamage(void) override { return &m_sIgnoredDamage; }
	void DelayActionSkillStart( STARGETID sCasterID, SNATIVEID sSkillID, WORD wLEVEL );
	void InvokeActionSkillStart( STARGETID sTargetID, SNATIVEID sSkillID, WORD wLevel  );

	virtual void SetTargetID( const STARGETID& sTargetID ) { m_TargetID = sTargetID; m_bRunAway = false; }

	virtual const STARGETID& GetTargetID ()	const			{ return m_TargetID; }
	virtual const bool IsNonTargetable() const;

	bool IsSummonNpcInteractionAble(const D3DXVECTOR3& vReqPos, const float fRadius, int nNpcAction=-1) const
	{ 
		return ISSummonNPC_INTERACTIONABLE(vReqPos, GetPosition(), fRadius, nNpcAction);
	}
	
public:
	/// 각 수치를 제어하는 값을 설정한다;
	// 기본 수치는 변경하지 않으면서 이 수치를 이용해 변경한다;
	// Summon 에서는 사용되지 않는다;
	virtual void			SetOffence ( int _nOffence ) { }
	virtual void			SetDefence ( int _nDefence ) { }
	virtual void			SetFireRegist ( int _nFireRegist ) { }
	virtual void			SetIceRegist ( int _nIceRegist ) { }
	virtual void			SetElectricRegist ( int _nElectricRegist ) { }
	virtual void			SetPoisonRegist ( int _nPoisonRegist ) { }
	virtual void			SetSpiritRegist ( int _nSpiritRegist ) { }
	virtual void			SetAccuracyRate ( int _nAccuracyRate ) { }
	virtual void			SetAvoidanceRate ( int _nAvoidanceRate ) { }
	virtual void			SetHPRestorationRate ( float _fHPRestorationRate ) { }
	virtual void			SetMPRestorationRate ( float _fMPRestorationRate ) { }
	virtual void			SetSPRestorationRate ( float _fSPRestorationRate ) { }

	virtual void			SetMultipleHP ( float fMultiple ) { }
	virtual void			SetMultipleAttack ( float fMultiple ) { }
	virtual void			SetMultipleDefense ( float fMultiple ) { }
	virtual void			SetMultipleExp ( float fMultiple ) { }
	virtual void			SetMultipleDrop ( float fMultiple ) { }

protected:
	BOOL SelfZoneInCrow( STARGETID &sTargetID, const PGLSKILL pSkill, const WORD wLEVEL );

	//	자기편에게 스킬 체크 ( 자기 위치에 영향 )
	BOOL SKT_SELFZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//  적들에게 스킬 체크 ( 자기 위치에 영향 )
	BOOL SKT_SELFZONEENEMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//  모두에게 스킬 체크 ( 자기 위치에 영향 )
	BOOL SKT_SELFZONANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	자기편에게 스킬 체크 ( 대상 위치에 영향 )이고 자신 주위
	BOOL SKT_TARSPECOUR_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	적들에게 스킬 체크 ( 대상 위치에 영향 )이고 자신 주위
	BOOL SKT_TARSPECENEMY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	모두에게 스킬 체크 ( 대상 위치에 영향 )이고 자신 주위
	BOOL SKT_TARSPECANYBODY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	자기편에게 스킬 체크 ( 대상 위치에 영향 )이고 목표 주위
	BOOL SKT_TARSPECOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	적들에게 스킬 체크 ( 대상 위치에 영향 )이고 목표 주위
	BOOL SKT_TARSPECENEMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	모두에게 스킬 체크 ( 대상 위치에 영향 )이고 목표 주위
	BOOL SKT_TARSPECANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	'목표'를 향한 부채꼴 모양.
	BOOL SKT_TARSPECFANWIZE ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	// 적에게만 자기 위치에서 대상 위치까지. ( 화살 관통 )
	BOOL SKT_TARSELFTOSPEC ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	자기편에게 스킬 체크 ( 위치에 영향 )
	BOOL SKT_TARZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	적들에게 스킬 체크 ( 위치에 영향 )
	BOOL SKT_TARZONEENEMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	모두에게 스킬 체크 ( 위치에 영향 )
	BOOL SKT_TARZONEANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	BOOL UpdateSkillTarget ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const SNATIVEID &sCheckSkill, const WORD wLevel );

	void SelectFromNearRange( IN TARDISTMAP &vecTAR_TEMP,
							  OUT WORD &nTarget, 
							  IN const WORD wTARNUM, 
							  IN const WORD wMaxTarget, 
							  OUT STARID *sTARIDS  );
public:
	void ReqNetMsg_Drop(GLMSG::SNET_SUMMON_DROP_SUMMON& Msg);
	void MsgProcess(NET_MSG_GENERIC* nmg);
    const bool CheckHit(const STARGETID &cTargetID);
    //! 끌어오기, 밀어내기 확률 계산 및 거리 확인
    BOOL CHECKPULLIN(const STARGETID &cTargetID, const GLLandMan* pLandMan, const D3DXVECTOR3& vPos, float fRate, float& fDist);
    BOOL CHECKKNOCKBACK(const STARGETID &cTargetID, const GLLandMan* pLandMan, const float fRate);
    DWORD CALCDAMAGE(
        int& rResultDAMAGE, 
		const DWORD dwGaeaID, 
		const STARGETID &cTargetID, 
		const GLLandMan* pLandMan,
        const GLSKILL* pSkill=NULL, 
		DWORD dwskill_lev=0, 
		DWORD dwWeatherFlag=NULL, 
		DWORD dwDivCount=0);
};

typedef GLSummonField* PGLSUMMONFIELD;

typedef sc::CGLLIST<PGLSUMMONFIELD>	GLSUMMONLIST;
typedef sc::SGLNODE<PGLSUMMONFIELD>	GLSUMMONNODE;

#endif // _GLOGIC_SERVER_GLSUMMON_FIELD_H_
