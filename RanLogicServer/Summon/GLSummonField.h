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
		EMRETURN_SETREACTION_FAILED,				// �Ϲ� ����
		EMRETURN_SETREACTION_FAILED_RANGE,			// �����Ÿ� ����
		EMRETURN_SETREACTION_FAILED_SKILL,			// ��ų ������ ��. ����
		EMRETURN_SETREACTION_FAILED_COLLISION,		// ������ �浹�ؼ� ����
		EMRETURN_SETREACTION_SUCCESS,				// ����
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
	// Note : �������� ����ü
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
	//	������ ť
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
	//sc::SGLNODE<GLSummonField*>* m_pGlobNode;	 //	�� ����Ʈ ���.
	LANDQUADNODE*			 m_pQuadNode;		 //	���� Ʈ�� ���.
	sc::SGLNODE<GLSummonField*>* m_pCellNode;	 // �� ����Ʈ ���.

public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	DxSkinChar*			m_pSkinChar;

	D3DXVECTOR3			m_vBackupPosition;

public:
	D3DXVECTOR3			m_vTarPos;
	STARGETID			m_TargetID;

	SCROWATTACK			m_sAttackProp;			// ���� �Ӽ�.

	D3DXVECTOR3			m_vStartPos;			// �̵��޽��� ������ ������ġ

protected:
	GLARoundSlot		m_ARoundSlot;			// �ǰ��ݽÿ� �ڽ��� �ֺ��� ������� �ִ� ��ġ�� �ִ��� ����.

	float			    m_fIdleTime;
	float			    m_fGestureStartTime;    //  ������ ���۽ð�.

	DWORD               m_dwAtypeAfterTracing;  //  ���� �� ����� AType 
	WORD                m_wRangeTracing;        //  ������ ����� Range 

	WORD				m_wAniSub;
	float				m_fattTIMER;			//	���� ������.
	DWORD               m_dwReactCount;         //  ���׼� Ƚ��.
	HITLIST				m_sHITARRAY;

	STARGETID			m_sAssault;				//	���� �ֱٿ� �ڽ��� ��������.
	STARGETID			m_sHeal;					// ���� �ֱٿ� �ڽ��� ġ������.

	WORD				m_wTARNUM;				//	Ÿ�� ����.
	STARID				m_sTARIDS[EMTARGET_NET];//	Ÿ�ٵ�.
	D3DXVECTOR3			m_vTARPOS;				//	Ÿ�� ��ġ.

	bool				m_bRunAway;				// ���� ���;
	
	bool				m_bUnlimit;				// ������ ���;
	bool				m_bSpeedUp;				// ���ǵ�� ���;

protected:
	SSCHEDULEDACTION	m_ScheduledAction;		// ����� ����

private:
	DWORD				m_dwEventSkillIdx;
	bool				m_bEventSkill;
	bool				m_bDelay;
	float				m_fDelayTime;

	int					m_nSkillFramCount;

	bool				m_bHire;				// ��� ��ȯ���ΰ�?;

public:
	inline const bool IsEventSkill () const { return m_bEventSkill; }
	inline void SetEnableEventSkill ( bool bEventSkill ) { m_bEventSkill = bEventSkill; }
	inline const bool IsHireSummon () const { return m_bHire; }
	inline void SetHireSummon ( bool bHire ) { m_bHire = bHire; }

public:
	const DWORD GetAIType () const;
	void SetAIType ( DWORD dwType );

	// Target�� �ڵ��˻��Ͽ� ��ȯ�Ѵ�;
	STARGETID AutoEventSkillSearchTarget ( UINT iIdx );

	const D3DXVECTOR3& GetTarPos() { return m_vTARPOS; }

public:
	virtual void ApplyDelay ( float fDelayTime );
	virtual bool GetDelayState ();
	virtual ServerActor* GetOwner ();						// ������ �ҷ��´�;
	virtual bool IsTarget ();								// Ÿ���� �ִ��� Ȯ��;
	virtual bool IsTargetExcludeOwner ();
	virtual bool IsValidOwnerDistance ();					// ���ΰ��� �Ÿ� ��ȿ�� �Ǵ�;
	virtual bool IsValidEventSkillDistance ();				// �̺�Ʈ ��ų�� ���� ���ΰ��� �Ÿ� ��ȿ�� �Ǵ�;
	virtual bool IsRunAway ();
	virtual const DWORD GetActionCount () const;			// �׼� Ƚ��;
	virtual void AutoSearchTarget ();						// �ڵ� Ÿ�� �˻�;
	virtual void SetOwnerTarget ();							// ������ �ִ� ��� ������ Ÿ���� Ÿ������;
	virtual void StartTargetAction ( DWORD dwCount = 0 );	// Ÿ�� �׼� ����;
	virtual void DieAction ();								// ����;
	virtual void ChangeSkillPage ( UINT iPage );			// ��ų ������ ����;
	virtual void UseEventSkill ( UINT iIdx );				// Ư�� �̺�Ʈ ��ų ���;
	virtual void RunAwayAction ( bool bSpeedUp = false );	// ���� �׼�;
	virtual void NormalAction ();							// �Ϲ� �׼�;
	virtual void TracingOwner ( bool bSpeedUp = false );	// ������ ����ٴѴ�;
	virtual void TracingTarget ( bool bSpeedUp = false );	// Ÿ���� ����ٴѴ�;
	virtual void SyncPositionTarget ();						// Ÿ���� ��ġ�� ��ġ�� ����ȭ�Ѵ�;

	// ��ų ȿ���� �����Ų��;
	// ���� �� �ݵ�� ��������� �Ѵ�;
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

	//  ��� ���μ���
	void    IdleProcess     ( float fElapsedTime );

	//	���� ���μ���.
	BOOL	ShockProcess    ( float fElapsedTime );
	BOOL	AttackProcess   ( float fElapsedTime );

	// ȸ�� ���μ���
	void AvoidProc(const SNATIVEID& SkillID = NATIVEID_NULL(), const WORD wSkillLevel = 0);

	// ��� ���μ���
	void BlockProc(const STARGETID& sTargetID, const SNATIVEID& SkillID = NATIVEID_NULL(), const WORD wSkillLevel = 0);

	// ������ ���μ���
	void	DamageProc      ( const int nDAMAGE, DWORD dwDamageFlag );
	DWORD	ToDamage        ( const STARGETID &sTargetID, const int nDamage, const DWORD dwDamageFlag );


public:
	// ��ų ���μ���
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
	HRESULT	   UpdateDurDamage       ( float fTime, float fElapsedTime );	//	���� ������ ó��( ������ �ش�. ) 
	const BOOL UpdateScheduledAction (); // ����� �׼����� ó��

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
	// ��ȯ���� ��Ƽ�� ������ ������ ��¿�� ����... ( ȣ���� �� ����! )
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

	// ��ȯ�ں��� ��ȯ���� �Ÿ��� �� ª���� üũ�Ѵ�.
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
	/// �� ��ġ�� �����ϴ� ���� �����Ѵ�;
	// �⺻ ��ġ�� �������� �����鼭 �� ��ġ�� �̿��� �����Ѵ�;
	// Summon ������ ������ �ʴ´�;
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

	//	�ڱ����� ��ų üũ ( �ڱ� ��ġ�� ���� )
	BOOL SKT_SELFZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//  ���鿡�� ��ų üũ ( �ڱ� ��ġ�� ���� )
	BOOL SKT_SELFZONEENEMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//  ��ο��� ��ų üũ ( �ڱ� ��ġ�� ���� )
	BOOL SKT_SELFZONANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	�ڱ����� ��ų üũ ( ��� ��ġ�� ���� )�̰� �ڽ� ����
	BOOL SKT_TARSPECOUR_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	���鿡�� ��ų üũ ( ��� ��ġ�� ���� )�̰� �ڽ� ����
	BOOL SKT_TARSPECENEMY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	��ο��� ��ų üũ ( ��� ��ġ�� ���� )�̰� �ڽ� ����
	BOOL SKT_TARSPECANYBODY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	�ڱ����� ��ų üũ ( ��� ��ġ�� ���� )�̰� ��ǥ ����
	BOOL SKT_TARSPECOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	���鿡�� ��ų üũ ( ��� ��ġ�� ���� )�̰� ��ǥ ����
	BOOL SKT_TARSPECENEMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	��ο��� ��ų üũ ( ��� ��ġ�� ���� )�̰� ��ǥ ����
	BOOL SKT_TARSPECANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	'��ǥ'�� ���� ��ä�� ���.
	BOOL SKT_TARSPECFANWIZE ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	// �����Ը� �ڱ� ��ġ���� ��� ��ġ����. ( ȭ�� ���� )
	BOOL SKT_TARSELFTOSPEC ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	�ڱ����� ��ų üũ ( ��ġ�� ���� )
	BOOL SKT_TARZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	���鿡�� ��ų üũ ( ��ġ�� ���� )
	BOOL SKT_TARZONEENEMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	��ο��� ��ų üũ ( ��ġ�� ���� )
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
    //! �������, �о�� Ȯ�� ��� �� �Ÿ� Ȯ��
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
