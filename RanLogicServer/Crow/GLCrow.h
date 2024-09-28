#ifndef GLCROW_H_
#define GLCROW_H_

#pragma once

#include <deque>
#include "../../EngineLib/NaviMesh/Actor.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"

#include "../../RanLogic/Crow/GLMobSchedule.h"
#include "../../RanLogic/GLogicEx.h"
#include "../ServerActor.h"

#ifndef GLLandMan
	class GLLandMan;
	class GLLandNode;
	typedef SQUADNODE<GLLandNode> LANDQUADNODE;
#endif //GLLandMan

#ifndef GLChar
	class GLChar;
	typedef GLChar* PGLCHAR;
#endif //GLChar

class GLGaeaServer;
class GLTransformSet;
struct SSKILLACTEX;
struct JACKPOT_ITEM;

typedef std::vector<STARGETID>	VEC_SK_TAR;
typedef VEC_SK_TAR::iterator	VEC_SK_ITER;

enum SKILL::EMACTION_TYPE;

class GLCrow : public GLCROWLOGIC, public ServerActor
{
public:
	enum EMREACTION
	{
		EMREACT_NONE	= 0,
		EMREACT_ATTACK	= 1,
		EMREACT_SKILL	= 2,
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
    GLCrow(GLGaeaServer* pGaeaServer);
    virtual ~GLCrow(void);

private:    	
	void	PreStrikeProc ( BOOL bSkill, BOOL bLowSP );

	//	공격 프로세스.
	void	PreAttackProc();
	BOOL	AttackProcess(float fElapsedTime);
	void	AvoidProc(const SNATIVEID& SkillID = NATIVEID_NULL(), const WORD wSkillLevel = 0);
	void	DamageProc(const int nDAMAGE, DWORD dwDamageFlag);
	void	BlockProc(const STARGETID& sTargetID, const SNATIVEID& SkillID = NATIVEID_NULL(), const WORD wSkillLevel = 0);
public:
	virtual void DamageReflectionProc(int nDAMAGE, const STARGETID& sACTOR) override;
	virtual void DamageCurse(int nDAMAGE, const STARGETID& sACTOR, EMCROW emCrow = CROW_NULL ) override;
	virtual void DefenseSkill(const SNATIVEID& sNativeID, WORD wLevel, const STARGETID& sACTOR) override { }

	virtual const bool IsImmuneToSkill ( BYTE iIndex ) const;

	//	스킬 프로세스.
	BOOL PreSkillProc();
	BOOL SkillProcess(float fElapsedTime);
	void StartSkillProc();
	void SkillProc(const BOOL bLowSP=FALSE);
	void SkillProc( BOOL bLowSP, SNATIVEID skill_id, WORD wSKILL_LVL, WORD wTARNUM, STARID *sTARIDS  );
	void ApplySkill( BOOL bLowSP, SNATIVEID skill_id, WORD wSKILL_LVL, WORD wTARNUM, STARID *sTARIDS );
	void ApplySkillSspecial(PGLSKILL pSkill, SSKILLACTEX &sSKILLACTEX, ServerActor* const pTargetActor, WORD wSKILL_LVL);
	void ApplySkillSspecialCA(PGLSKILL pSkill, SSKILLACTEX &sSKILLACTEX, SSKILLACTEX &sCASKILLACTEX, ServerActor* const pTargetActor, WORD wSKILL_LVL);
	void DelayActionSkillStart( STARGETID sCasterID, SNATIVEID sSkillID, WORD wLevel );
	void InvokeActionSkillStart( STARGETID sTargetID, SNATIVEID sSkillID, WORD wLevel  );

	void	SelfSkillStart( SNATIVEID sSkillID, WORD wLevel );
	BOOL   IsSkillLoopEnd();
	BOOL   IsSkillEnd( SNATIVEID sSkill );

	//	쇼크 프로세스.
	BOOL ShockProcess(float fElapsedTime);

	//
	//mjeon.CaptureTheField
	//
	void Reset4CTF();

	inline BOOL			IsCheckedActFlags( DWORD dwFlags )
	{
		if( !m_pCrowData )
			return FALSE;

		return m_pCrowData->IsCheckedActFlags( dwFlags );
	} //IsCheckedActFlags
	/*
	inline BOOL			IsNoAllPenalty()
	{
		if( !m_pCrowData )
			return FALSE;

		if ( m_pCrowData->m_bNoAllPenalty == TRUE )
		{
			if ( m_pCrowData->m_wResist_Probability >= sc::Random::getInstance().getPercent() )
				return TRUE;
		}
		return FALSE;
	} //IsNoAllPenalty
	*/
	//  [9/16/2013 gbgim];
	// - bProbabilityCheck
	//   : IsNoAllPenalty, IsNoPenalty 같이 사용시 문제점이 하나있어 추가;
	//     둘다 랜덤 체크를 해야되는데 두함수 모두에서 체크해버리면 문제가됨;
	const BOOL IsNoAllPenalty( const unsigned int uiPenaltyType  ) const;
	
	const BOOL IsNoAllPenalty(void) const;

	// - bAlreadyProbabilty : 이미 확률 체크햇는가?;
	const BOOL IsNoPenalty( const unsigned int uiPenaltyType  ) const;

public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

// 	Actor				m_actorMove;
// 	D3DXVECTOR3			m_vDirect;
// 	D3DXMATRIX			m_matTrans;
// 	D3DXVECTOR3			m_vPosition;					//	현제 위치.

	DxSkinChar*	m_pSkinChar;		
	GLTransformSet*	m_pTransformSet;	

	//	Note : AABB
protected:
	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;
	int m_nSkillReActionError;

	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	//	Note : 제어 코드.
	//
public:	
	//sc::SGLNODE<GLCrow*>*	m_pGlobNode;			//	맵 리스트 노드.
	LANDQUADNODE*		m_pQuadNode;			//	쿼드 트리 노드.
	sc::SGLNODE<GLCrow*>*	m_pCellNode;			//	셀 리스트 노드.

	bool				m_bNpcRecall;			//	소환된 여부
	DWORD				m_dwCallCharID;			//	소환한 캐릭터 ID
	DWORD				m_dwCallUserID;			//	소환한 User ID
	__time64_t			m_tGenTime;				//	소환된 시간
	LONGLONG			m_lnCommission;			//	소환된 상황에서 생긴 세금

	//	Note : 행동.
protected:
	float				m_fAge;					//	경과 시간.
	DWORD				m_dwCFlag;				//	콘트롤 플래그.	
	D3DXVECTOR3			m_vGenPos;				//	최초 생성위치.
	D3DXVECTOR3			m_vOrgTarPos;			//	최초 타겟팅 위치.
	
	float				m_fScale;				//  캐릭터 스케일

	D3DXVECTOR3			m_vStartPos;			// 이동메시지 보낼때 시작위치

	DWORD				m_dwAttPosSlot;			//	공격 위치 슬롯.
	STARGETID			m_TargetID;				//	목표 유닛.

	EMREACTION			m_emReAction;			//	진행중인 행위.
	SCROWATTACK*		m_pAttackProp;			//	공격 속성.
	float				m_fattTIMER;			//	공격 프래임.
	HITLIST				m_sHITARRAY;

	WORD				m_wTARNUM;				//	타겟 숫자.
	STARID				m_sTARIDS[EMTARGET_NET];//	타겟들.
	D3DXVECTOR3			m_vTARPOS;				//	타겟 위치.

	STARGETID			m_sAssault;				//	가장 최근에 자신을 공격한자.	
	DAMAGELOG			m_cDamageLog;			///< 데미지 로그(캐릭터)
	DAMAGELOG			m_cSummonDamageLog;		///< 데미지 로그(소환수)
	DAMAGELOGPARTY      m_cDamageLogParty;      ///< 데미지 로그(파티)

	STARGETID			m_sHeal;					// 가장 최근에 자신을 치유한자.

protected:
	float				m_fIdleTime;			//	휴식 시간.
	float				m_fStayTimer;			//	휴식 딜래이.

	float				m_fTargetTimer;			//	타갯 검색 타이머.
    
    float               m_fMultipleAttack;	    // 공격력 배수
    float               m_fMultipleDefense;     // 방어력 배수
    float               m_fMultipleExp;         // 경험치 배수
	float               m_fMultipleDrop;        // 아이템 드랍율 배수
    DWORD               m_dwKillerDbNum;        // 죽인자의 DB Num

	//	Note : 스케쥴.
protected:
	MOBACTIONNODE*		m_pCurAct;
	GLMobSchedule*		m_pMobSchedule;

	// 몬스터 행동 반경 제한.
private:
	bool				m_bReturnPatrolPos;		// 패트롤 위치로 이동하라!
	bool				m_bIsPatrolStop;		// 스캐줄러가 중단되었는가?
 	D3DXVECTOR3			m_vPatrolStopPos;		// 패트롤이 중단되었다면 과거 위치

private:
    bool                m_bUseReward;           // 보상을 드랍한 적이 있는가?;

public:
	GLMobSchedule* GetMobSchedule ()			{ return m_pMobSchedule; }
    __forceinline void SetCurrentMobActionNode( MOBACTIONNODE* _pCurAct ) { m_pCurAct = _pCurAct; };

public:
	INT IsLowerHP ();
	
	BOOL IsRunning () const						{ return isAction(GLAT_MOVE) && IsActState(EM_ACT_RUN); };
	
protected:
//	BOOL IsActState ( DWORD dwState ) const		{ return m_dwActState&dwState; }
//	void SetActState ( DWORD dwState )				{ m_dwActState |= dwState; }
//	void ReSetActState ( DWORD dwState )			{ m_dwActState &= ~dwState; }

public:
	BOOL IsFLAG ( EMCONTROL emFlag )			{ return m_dwCFlag&emFlag; }

protected:
	void SetFLAG ( EMCONTROL emFlag )			{ m_dwCFlag |= emFlag; }
	void ReSetFLAG ( EMCONTROL emFlag )			{ m_dwCFlag &= ~emFlag; }

public:
	EMCROWACT_UP GetActPattern ();

protected:
	bool FindLongTracePos ( const D3DXVECTOR3 &vCur, const D3DXVECTOR3 &vTar, const float fMin, const float fMax, D3DXVECTOR3 &vTracePos );
	bool DoShortTracing ( const WORD wTARRANGE, const float fElapsedTime );
	bool DoLongTracing ( const WORD wTARRANGE, const float fElapsedTime );

protected:
	void RunNextAct ();
	void RunSchedule ( float fTime, float fElapsedTime );

	DWORD FindOptAType ( const STARGETID &sTargetID, bool branvar );

	void ReAction ( const STARGETID &sTargetID, float fElapsedTime=0.0f );
	void ResetAction ();
	void ResetTarget ();

	bool AttackReAction ( float fTime, float fElapsedTime );
	bool SkillReAction ( float fTime, float fElapsedTime );

public:
	void NewTarget ( const STARGETID &sTargetID, BOOL bCheckMember = FALSE );
	BOOL SetTargeting ( const STARGETID& sTargetID );	

	virtual void SetGaeaID(const DWORD dwGaeaID) override
	{
		m_dwGlobalID = dwGaeaID; // GLCROWLOGIC에서 사용 중이라 일단 남겨둠; // 최종적으로는 제거해야함;
		m_dwGaeaID = dwGaeaID;
	}	
	virtual const STARGETID& GetTargetID ()	const			{ return m_TargetID; }
	EMATT_RGTYPE GetAttackRangeType ()			
    { 
        if (!m_pAttackProp)
            return EMATT_SHORT;
        return m_pAttackProp->emAttRgType;
    }

	BOOL IsGroupMember()						
    { 
        return (!m_pMobSchedule || m_pMobSchedule->m_strGroupName == "" ) ? FALSE : TRUE; 
    }

	BOOL IsGroupLeader()						
    {
        return (!m_pMobSchedule || !m_pMobSchedule->m_bLeaderMonster ) ? FALSE : TRUE;
    }
	std::string GetGroupName()					
    { 
        return (!m_pMobSchedule ) ? "" : m_pMobSchedule->m_strGroupName;
    }
	BOOL IsGroupTarget( const STARGETID& sTargetID );

	void GroupAttackProcess( const STARGETID& sTargetID );
	BOOL CheckGroupRoyalty();
	BOOL CheckChangeGroupTarget();

public:
    bool IsNpcInteractionAble(const D3DXVECTOR3& vReqPos, const float fRadius, int nNpcAction=-1) const
    { 
        return ISNPC_INTERACTIONABLE(vReqPos, GetPosition(), fRadius, nNpcAction);
    }
	
	virtual int GETSKILLFACTSIZE() const override { return CROW_SKILLFACT_SIZE; }

protected:
	DWORD SELECT_SKILLSLOT ( SNATIVEID skill_id, EMSKILLFACT_TYPE emSkillFactType );

public:
    virtual const DWORD GetCharID() const	override { return m_dwGaeaID; }
	virtual const EMCROW GetCrow(void) const override { return GETCROW(); }	

	ServerActor* GetTarget(const STARGETID& sTargetID);
	ServerActor* GetTarget(const GLLandMan* pLandMan, const STARGETID& sTargetID);

	// 크로우는 파티를 가질수 없지만 어쩔수 없이... ( 호출할 일 없음! )
	// 크로우 그룹 로직은 파티 로직을 이용하는 편이 좋음;
	// 코드가 분산 됨; - /2013/01/25;
	virtual DWORD GETPARTYID () const				{ return 0; }
	
	virtual BOOL IsCtrlBlockBody () const;
	virtual BOOL IsActionLimit( DWORD dwLimit ) const { return m_dwActionLimit&dwLimit; } 
	virtual BOOL IsNonDamaged() const;

	virtual bool IsCheckedSkillFlagSpecial ( DWORD dwFlag )	const;
	
	virtual WORD GetBodyRadius () const				{ return GETBODYRADIUS(); }

	virtual DWORD GETHOLDBLOW () const				{ return m_dwHOLDBLOW; }

	virtual WORD GetLevel () const					{ return GETLEVEL(); }
	
	virtual DWORD GetNowHP () const					{ return GETHP(); }
	virtual DWORD GetMaxHP () const;
    virtual float GetMaxHpRate() const				{ return GETMAXHPRATE(); }
    virtual void  SetMaxHpRate( float fRate )       {  SETMAXHPRATE( fRate ); };

	virtual WORD GetNowMP () const					{ return GETMP(); };
	virtual WORD GetMaxMP () const					{ return GETMAXMP(); }

	virtual WORD GetNowSP () const					{ return GETSP(); }
	virtual WORD GetMaxSP () const					{ return GETMAXSP(); }

	virtual int				GetBasicOffenceHigh() const			{ return GETFORCE_HIGH(); }
	virtual int				GetBasicOffenceLow() const			{ return GETFORCE_LOW(); }
	virtual int				GetBasicDefence() const				{ return m_nDefence; }
	virtual const SRESIST&	GetBasicRegist() const				{ return m_sSUMRESIST; }
	virtual int				GetBasicHitRate() const				{ return GETHIT(); }
	virtual int				GetBasicAvoid() const				{ return GETAVOID(); }
	virtual float			GetBasicHPRestorationRate() const	{ return GETHPRESTORATIONRATE(); }
	virtual float			GetBasicMPRestorationRate() const	{ return GETMPRESTORATIONRATE(); }
	virtual float			GetBasicSPRestorationRate() const	{ return GETSPRESTORATIONRATE(); }

	virtual int				GetOffenceHigh () const				{ return GETFORCE_HIGH() + m_nOffence; }
	virtual int				GetOffenceLow () const				{ return GETFORCE_LOW() + m_nOffence; }
	virtual int				GetDefense () const					{ return static_cast< int >( ( GETDEFENSE() + m_nDefence ) * GetMultipleDefense() ); }
	virtual const SRESIST&	GETRESIST () const
	{
		SRESIST sRegist( m_nFireRegist, m_nIceRegist, m_nElectricRegist, m_nPoisonRegist, m_nSpiritRegist );
		return m_sSUMRESIST + sRegist;
	}
	virtual int				GetHitRate () const					{ return GETHIT() + m_nAccuracyRate; }
	virtual int				GetAvoid () const					{ return GETAVOID() + m_nAvoidanceRate; }
	virtual float			GetHPRestorationRate() const		{ return GETHPRESTORATIONRATE() + GetHPRestorationRatePlus(); }
	virtual float			GetMPRestorationRate() const		{ return GETMPRESTORATIONRATE() + GetMPRestorationRatePlus(); }
	virtual float			GetSPRestorationRate() const		{ return GETSPRESTORATIONRATE() + GetSPRestorationRatePlus(); }



	virtual EMBRIGHT GetBright () const				{ return GETBRIGHT(); }


	virtual DWORD GetBonusExp () const;

	virtual DWORD		GETCLIENTID ();
	virtual WORD		GetSchool () const;
	virtual SCONFTING*	GETCONFTING ();

	virtual SSKILLFACT* GETSKILLFACT() override { return m_sSKILLFACT; }
	virtual DAMAGE_SPEC* GETDAMAGESPEC() { return &m_sDamageSpec; }

	virtual const EMSPEC_DOMINATE_TYPE GetController(void) const { return m_sDOMINATION.emController; }
	virtual void	SetController(const EMSPEC_DOMINATE_TYPE emDOMINATION_TYPE){m_sDOMINATION.emController = emDOMINATION_TYPE; }
	virtual void	SetPosition(const D3DXVECTOR3& vPosition);

	virtual const DWORD ReceiveDamage(const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag ) override;
    const DWORD ReceiveDamage_Procedure(const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag );
    const DWORD ReceiveDamage_Procedure_For_CTF(const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag );

	virtual const DWORD	ReceiveHeal(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag) override;
    const DWORD	ReceiveHeal_Procedure(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag);

	virtual void SendPushPull( const D3DXVECTOR3 &vMovePos, const float fPUSH_SPEED, const DWORD dwAniType, const bool bReActor = true );

	virtual bool STATEBLOW ( const SSTATEBLOW &sStateBlow );
	virtual void CURE_STATEBLOW ( DWORD dwCUREFLAG );
	virtual void SKILL_EFFECT_REMOVE ( DWORD dwRemoveFlag );
	virtual void DO_STUN_ACTION ();
    virtual void VAR_BODY_POINT( 
        const EMCROW emActorCrow, 
        const DWORD dwActorID, 
        const DWORD dwVarTypeFlag,
        int& nVar_HP, 
        int& nVar_MP, 
        int& nVar_SP ) override;

	virtual void UPDATE_DATA_END(const DWORD dwSkillfactIndex, BOOL bTimeDel = FALSE);
	virtual void UPDATE_DATA_Additional(const float fElapsedTime);
		void UPDATE_DATA_Additional_Stigma(SSKILLFACT& sSKEFF, const DWORD dwStigmaIndex, const DWORD dwSkillfactIndex);
		void UPDATE_DATA_Additional_Domination(SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex);
		void UPDATE_DATA_Additional_Provoke(SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex);

	const bool RECEIVE_SKILLFACT_Domination(const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel);
    const bool RECEIVE_SKILLFACT_Taunt( const STARGETID& sID );
	const bool RECEIVE_SKILLFACT_Stigma(const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel);
	const bool RECEIVE_SKILLFACT_Special( PGLSKILL pSkill, const SNATIVEID& sSkillID, const WORD wlevel, const DWORD dwCount, SSKILLFACT& sSKILLEF, const STARGETID sID );
	const bool RECEIVE_SKILLFACT_Addon( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF );
	const bool RECEIVE_SKILLFACT_Basic( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF );
	const bool RECEIVE_SKILLFACT_EntranceExit( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID );
	const bool RECEIVE_SKILLFACT_Provoke( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID );
	
	virtual const BOOL RECEIVE_SKILLFACT(
        const SNATIVEID& skill_id,
        const WORD wlevel,
        DWORD &dwSELECT,
        const STARGETID& sID,
        const SSKILLFACT::STIME sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME),
        const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT,
        EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NORMAL,
		const float* const pLaunchTime = 0, const float* const pLaunchValue = 0) override;

	virtual const BOOL RECEIVE_SKILLFACTII(
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

	virtual const DAMAGE_SPEC&	GetDamageSpec() const		{ return m_sDamageSpec; }
	virtual DAMAGE_SPEC*		GetDamageSpecPtr()			{ return &m_sDamageSpec; }
	
	virtual DEFENSE_SKILL GetDefenseSkill() const	{ return m_sDefenseSkill; }
	virtual SINCREASEEFF* const GetIncreaseEff() { return &m_sIncreaseEff; }
	virtual SIGNORED_DAMAGE* const GetIgnoredDamage(void) override { return &m_sIgnoredDamage; }

	WORD    GetBonusCP() const						{ return m_pCrowData->m_wBonusCP; }

protected:
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

	BOOL SelectSkillTarget ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const SNATIVEID &sCheckSkill, const WORD wLevel );
	
	void SelectFromNearRange( IN TARDISTMAP &vecTAR_TEMP,
		OUT WORD &nTarget, 
		IN const WORD wTARNUM, 
		IN const WORD wMaxTarget, 
		OUT STARID *sTARIDS  );

public:
	bool	 		MoveTo( const D3DXVECTOR3* pTarPos, bool bJump, DWORD dwFlags );
	HRESULT AttackTo(STARGETID& sTARID, float fTime);
	HRESULT ESCAPE ( STARGETID &sTARID, const DWORD dwDamage );

protected:
	void 			ToRun();
	void 			ToWalk();
	virtual float GetMoveVelo();

	bool 			PreTurnActionAttack();
	bool 			PreTurnActionSkill();

	bool 			TurnActionIdle( const bool bMsg );
	bool 			TurnActionMove( const bool bMsg, EMACTIONTYPE emOldAction );
	bool 			TurnActionAttack( const bool bMsg );
	bool 			TurnActionSkill();
	bool 			TurnActionAuthed();
	bool 			TurnActionFalling( const bool bMsg );

	virtual void TurnAction(const EMACTIONTYPE toAction, const bool bMsg = true) override;

public:
	float GetAge ()						{ return m_fAge; }
	
	bool			CheckStateToGo()	{ return true; }

	//
	//mjeon.CaptureTheField
	//
	void BeAuthedBy(WORD wSchool);
	WORD IsAuthedBy();

	const D3DXVECTOR3& GetTarPos() { return m_vTARPOS; }

protected:
	DWORD ToDamage ( const STARGETID &sTargetID, const int nDAMAGE, const DWORD dwDamageFlag );

    // 몹을 죽인다;
    void DoFalling();
    void DoFalling_Proc();

	//! 몹이 죽었을때 보상을 발생시킨다 (아이템, 게임머니, ?박스 등등...)
	void GenerateReward();
	void GenerateReward_200502();
	void GenerateReward_20060402();
	void GenerateReward_20060417();
    void GenerateReward_20110722();
	//! 몹이 죽었을때 보상을 발생시킨다 (아이템, 게임머니, ?박스 등등...)
    //! \param dwOWNER 아이템 소유자
    void GenerateRewardItem_20060417(DWORD dwOWNER = GAEAID_NULL);
    void GenerateRewardItem_20110722(DWORD dwOWNER = GAEAID_NULL);

	const DWORD CalculateMaxPartyDamage(DWORD& dwRefPartyMasterGaeaID);

	//! 이벤트나 탐닉 방지 시스템에 의해 변경된 아이템 드랍율을 구한다.
	float CalculateItemGenRate_20060417( float fGenRate, PGLCHAR pChar, bool bParty );

    //! 최초에 드롭할 증가에 대한 Callback
    float Callback_CalculateGenProb ( EMGENINFOTYPE emInfoType, float fDefaultProb, PGLCHAR pChar, bool bParty );

    //! 각 아이템의 확률에 대한 Callback
    float Callback_CalculateGenItemRate ( EMGENITEMTYPE emItemType, float fDefaultRate, WORD wGrade, PGLCHAR pChar );

    //! 최종적으로 발생할 아이템에 대한 Callback (금액 확대 등)
    void Callback_CalculateGenResult ( OUT SGENRESULT& sResult, GLChar* const pChar );

    //! 발생한 아이템에 대한 로그 Callback
    //void Callback_GenItemLog ( EMGENITEMTYPE emItemType, float fRate, SGENRESULT& sResult, PGLCHAR pChar, bool bParty, SNATIVEID sCrowID );

protected:

	STARGETID  FindEnemyChar ();
	BOOL	   IsViewInActor( const STARGETID& sID );

	const EMREACTIONABLE IsReactionable ( ServerActor* _pTarget, const EMIMPACT_SIDE _emTargetSide );

	const EMREACTIONABLE IsReactionableFaction ( ServerActor* _pTarget, const EMIMPACT_SIDE _emTargetSide );
	const EMREACTIONABLE IsReactionableNonFaction ( ServerActor* _pTarget, const EMIMPACT_SIDE _emTargetSide );

public:
	VEC_SK_TAR DetectTarget ( const D3DXVECTOR3 &vDetectPosA, const D3DXVECTOR3 &vDetectPosB, const DWORD dwMaxFind );
	VEC_SK_TAR DetectTarget ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind );

public:
	void ReqNetMsg_Drop(GLMSG::SNETDROP_CROW& Msg);

public:
    typedef std::vector<DWORD>		VECPC;
    typedef VECPC::iterator			VECPC_ITER;
    typedef VECPC::const_iterator   VECPC_CITER;

protected:
	DWORD	m_dwPC_AMOUNT;
	DWORD	m_dwSUMMON_AMOUNT;
	DWORD	m_dwMOB_AMOUNT;

	VECPC	m_vecPC_VIEW;
	VECPC	m_vecSUMMON_VIEW;
	VECPC	m_vecMob_VIEW;

	void INSERT_PC_VIEW( DWORD dwGAEAID );
	void INSERT_SUMMON_VIEW ( DWORD dwGAEAID );
	void INSERT_MOB_VIEW ( DWORD dwGAEAID );

public:
    __forceinline VECPC_CITER getViewPcBegin() const { return m_vecPC_VIEW.begin(); };
    __forceinline VECPC_CITER getViewPcEnd() const { return m_vecPC_VIEW.end(); };
    __forceinline const DWORD getViewPcAmount() const { return m_dwPC_AMOUNT; };

    __forceinline VECPC_CITER getViewMobBegin() const { return m_vecMob_VIEW.begin(); };
    __forceinline VECPC_CITER getViewMobEnd() const { return m_vecMob_VIEW.end(); };
    __forceinline const DWORD getViewMobAmount() const { return m_dwMOB_AMOUNT; };

    __forceinline VECPC_CITER getViewSummonBegin() const { return m_vecSUMMON_VIEW.begin(); };
    __forceinline VECPC_CITER getViewSummonEnd() const { return m_vecSUMMON_VIEW.end(); };
    __forceinline const DWORD getViewSummonAmount() const { return m_dwSUMMON_AMOUNT; };

public:
	void			UpdateViewAroundPc( GLLandNode *pLandNode, BOUDRECT& bound );
	void			UpdateViewAroundSummon( GLLandNode *pLandNode, BOUDRECT& bound );
	void			UpdateViewAroundMob( GLLandNode *pLandNode, BOUDRECT& bound );
	HRESULT			UpdateViewAround();
	virtual HRESULT	SendMsgViewAround(NET_MSG_GENERIC* nmg);
	virtual void SendToClient(NET_MSG_GENERIC* pMsg);	
	virtual void SendToClient(const DWORD dwClientSlot, NET_MSG_GENERIC* pMsg);

public:
	HRESULT UpateAnimation ( float fTime, float fElapsedTime );
	HRESULT UpdateDurDamage ( float fTime, float fElapsedTime );	//	지속 데미지 처리( 데미지 준다. ) 
	void	UpdateSkillEff( float fTime, float fElapsedTime );

protected:
	void UpdateActState(void);

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT FrameMoveAction( float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );	

public:
	void RESET_DATA ();
	HRESULT CreateMob ( GLMobSchedule *pMobSchedule, GLLandMan* pLandMan, SNATIVEID sCROWID, LPDIRECT3DDEVICEQ pd3dDevice=NULL, 
						 LPD3DXVECTOR3 pPos=NULL, bool bFirstCreateForInstMap = FALSE, FLOAT fScale = 1.0f );
	/*HRESULT CreateSummon (
		LPDIRECT3DDEVICEQ pd3dDevice,
		GLLandMan* pLandMan,
		PGLSUMMON pSummonData,
		ServerActor* pOwner,
		D3DXVECTOR3& vPosition,
		const STARGETID& sTarget );*/

    const bool CheckHit(const STARGETID &cTargetID);
    //! 끌어오기, 밀어내기 확률 계산 및 거리 확인
    DWORD CALCDAMAGE(
        int& rResultDAMAGE, 
		const DWORD dwGaeaID, 
		const STARGETID &cTargetID, 
		const GLLandMan* pLandMan,
        const GLSKILL* pSkill=NULL, 
		DWORD dwskill_lev=0, 
		DWORD dwWeatherFlag=NULL, 
		DWORD dwDivCount=0);
        
    bool IsBasicTalk(int nAction) const;
    std::tr1::shared_ptr<SNpcTalk> GetTalk(DWORD dwTalkID) const;

public:
	void Generate(GLChar* pChar, bool bParty = false);
	void GenerateItem(GLChar* pChar, const SGENINFO::VEC_SPGENITEM& vecGenItem, OUT SGENRESULT& sResult, bool bParty = false);

	//발생한 보상 아이템
	typedef std::vector<SGENRESULT> VEC_RESULT;
    typedef VEC_RESULT::iterator    VEC_RESULT_ITER;
    VEC_RESULT m_vecResult;

	void JackpotProcess( GLChar* pChar, JACKPOT_ITEM* Item );

    //! 소환된 여부
    bool IsNpcRecall() const { return m_bNpcRecall; }

    //! 소환된 시간
    __time64_t GetGenTime() const { return m_tGenTime; }
    DWORD GetGenTimeFromData() const { return m_pCrowData->GenTime(); }
    
    /**
        몬스터의 세부 사항의 배수를 설정 한다.
        HP, 공격력, 방어력, 경험치, 아이템 드랍율 배수를 설정 할 수 있다.
        인던의 난이도를 설정을 위해 만들어 졌다.

     */
    virtual void SetMultipleHP(float fMultiple);
    virtual void SetMultipleAttack(float fMultiple);
	virtual void SetMultipleDefense(float fMultiple);
	virtual void SetMultipleExp(float fMultiple);
	virtual void SetMultipleDrop(float fMultiple);

	virtual const float GetMultipleHP() const;
    virtual const float GetMultipleAttack() const;
    virtual const float GetMultipleDefense() const;
    virtual const float GetMultipleExp() const;
	virtual const float GetMultipleDrop() const;

    /**
        죽었을 때 죽인자가 누구인지 알 수 있다.
        데미지를 준 자들이 파티원이든 아니든 데미지를 많이 준 사람이 선정된다.
        이것은 현재 이벤트의 보상처리시 사용 된다.

     */
    const DWORD GetKillerID() const { return m_dwKillerDbNum; }

public:
	/// 각 수치를 제어하는 값을 설정한다;
	// 기본 수치는 변경하지 않으면서 이 수치를 이용해 변경한다;
	virtual void			SetOffence ( int _nOffence );
	virtual void			SetDefence ( int _nDefence );
	virtual void			SetFireRegist ( int _nFireRegist );
	virtual void			SetIceRegist ( int _nIceRegist );
	virtual void			SetElectricRegist ( int _nElectricRegist );
	virtual void			SetPoisonRegist ( int _nPoisonRegist );
	virtual void			SetSpiritRegist ( int _nSpiritRegist );
	virtual void			SetAccuracyRate ( int _nAccuracyRate );
	virtual void			SetAvoidanceRate ( int _nAvoidanceRate );
	virtual void			SetHPRestorationRate ( float _fHPRestorationRate );
	virtual void			SetMPRestorationRate ( float _fMPRestorationRate );
	virtual void			SetSPRestorationRate ( float _fSPRestorationRate );

private:
	// 기본 수치에 추가되는 수치, 이는 마이너스도 올 수 있다;
	int m_nOffence;
	int m_nDefence;
	int m_nFireRegist;
	int m_nIceRegist;
	int m_nElectricRegist;
	int m_nPoisonRegist;
	int m_nSpiritRegist;
	int m_nAccuracyRate;
	int m_nAvoidanceRate;

	//////////////////////////////////////////////////////////////////////////
	//						AnimationMove
private:
	void SkinAniControl_AnimationMove();
	//						AnimationMove
	//////////////////////////////////////////////////////////////////////////
};

typedef GLCrow* PGLCROW;

typedef sc::CGLLIST<PGLCROW>	GLCROWLIST;
typedef sc::SGLNODE<PGLCROW>	GLCROWNODE;

#endif // GLCROW_H_
