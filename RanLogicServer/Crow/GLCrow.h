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
    GLCrow(GLGaeaServer* pGaeaServer);
    virtual ~GLCrow(void);

private:    	
	void	PreStrikeProc ( BOOL bSkill, BOOL bLowSP );

	//	���� ���μ���.
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

	//	��ų ���μ���.
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

	//	��ũ ���μ���.
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
	//   : IsNoAllPenalty, IsNoPenalty ���� ���� �������� �ϳ��־� �߰�;
	//     �Ѵ� ���� üũ�� �ؾߵǴµ� ���Լ� ��ο��� üũ�ع����� ��������;
	const BOOL IsNoAllPenalty( const unsigned int uiPenaltyType  ) const;
	
	const BOOL IsNoAllPenalty(void) const;

	// - bAlreadyProbabilty : �̹� Ȯ�� üũ�޴°�?;
	const BOOL IsNoPenalty( const unsigned int uiPenaltyType  ) const;

public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

// 	Actor				m_actorMove;
// 	D3DXVECTOR3			m_vDirect;
// 	D3DXMATRIX			m_matTrans;
// 	D3DXVECTOR3			m_vPosition;					//	���� ��ġ.

	DxSkinChar*	m_pSkinChar;		
	GLTransformSet*	m_pTransformSet;	

	//	Note : AABB
protected:
	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;
	int m_nSkillReActionError;

	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	//	Note : ���� �ڵ�.
	//
public:	
	//sc::SGLNODE<GLCrow*>*	m_pGlobNode;			//	�� ����Ʈ ���.
	LANDQUADNODE*		m_pQuadNode;			//	���� Ʈ�� ���.
	sc::SGLNODE<GLCrow*>*	m_pCellNode;			//	�� ����Ʈ ���.

	bool				m_bNpcRecall;			//	��ȯ�� ����
	DWORD				m_dwCallCharID;			//	��ȯ�� ĳ���� ID
	DWORD				m_dwCallUserID;			//	��ȯ�� User ID
	__time64_t			m_tGenTime;				//	��ȯ�� �ð�
	LONGLONG			m_lnCommission;			//	��ȯ�� ��Ȳ���� ���� ����

	//	Note : �ൿ.
protected:
	float				m_fAge;					//	��� �ð�.
	DWORD				m_dwCFlag;				//	��Ʈ�� �÷���.	
	D3DXVECTOR3			m_vGenPos;				//	���� ������ġ.
	D3DXVECTOR3			m_vOrgTarPos;			//	���� Ÿ���� ��ġ.
	
	float				m_fScale;				//  ĳ���� ������

	D3DXVECTOR3			m_vStartPos;			// �̵��޽��� ������ ������ġ

	DWORD				m_dwAttPosSlot;			//	���� ��ġ ����.
	STARGETID			m_TargetID;				//	��ǥ ����.

	EMREACTION			m_emReAction;			//	�������� ����.
	SCROWATTACK*		m_pAttackProp;			//	���� �Ӽ�.
	float				m_fattTIMER;			//	���� ������.
	HITLIST				m_sHITARRAY;

	WORD				m_wTARNUM;				//	Ÿ�� ����.
	STARID				m_sTARIDS[EMTARGET_NET];//	Ÿ�ٵ�.
	D3DXVECTOR3			m_vTARPOS;				//	Ÿ�� ��ġ.

	STARGETID			m_sAssault;				//	���� �ֱٿ� �ڽ��� ��������.	
	DAMAGELOG			m_cDamageLog;			///< ������ �α�(ĳ����)
	DAMAGELOG			m_cSummonDamageLog;		///< ������ �α�(��ȯ��)
	DAMAGELOGPARTY      m_cDamageLogParty;      ///< ������ �α�(��Ƽ)

	STARGETID			m_sHeal;					// ���� �ֱٿ� �ڽ��� ġ������.

protected:
	float				m_fIdleTime;			//	�޽� �ð�.
	float				m_fStayTimer;			//	�޽� ������.

	float				m_fTargetTimer;			//	Ÿ�� �˻� Ÿ�̸�.
    
    float               m_fMultipleAttack;	    // ���ݷ� ���
    float               m_fMultipleDefense;     // ���� ���
    float               m_fMultipleExp;         // ����ġ ���
	float               m_fMultipleDrop;        // ������ ����� ���
    DWORD               m_dwKillerDbNum;        // �������� DB Num

	//	Note : ������.
protected:
	MOBACTIONNODE*		m_pCurAct;
	GLMobSchedule*		m_pMobSchedule;

	// ���� �ൿ �ݰ� ����.
private:
	bool				m_bReturnPatrolPos;		// ��Ʈ�� ��ġ�� �̵��϶�!
	bool				m_bIsPatrolStop;		// ��ĳ�ٷ��� �ߴܵǾ��°�?
 	D3DXVECTOR3			m_vPatrolStopPos;		// ��Ʈ���� �ߴܵǾ��ٸ� ���� ��ġ

private:
    bool                m_bUseReward;           // ������ ����� ���� �ִ°�?;

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
		m_dwGlobalID = dwGaeaID; // GLCROWLOGIC���� ��� ���̶� �ϴ� ���ܵ�; // ���������δ� �����ؾ���;
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

	// ũ�ο�� ��Ƽ�� ������ ������ ��¿�� ����... ( ȣ���� �� ����! )
	// ũ�ο� �׷� ������ ��Ƽ ������ �̿��ϴ� ���� ����;
	// �ڵ尡 �л� ��; - /2013/01/25;
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

    // ���� ���δ�;
    void DoFalling();
    void DoFalling_Proc();

	//! ���� �׾����� ������ �߻���Ų�� (������, ���ӸӴ�, ?�ڽ� ���...)
	void GenerateReward();
	void GenerateReward_200502();
	void GenerateReward_20060402();
	void GenerateReward_20060417();
    void GenerateReward_20110722();
	//! ���� �׾����� ������ �߻���Ų�� (������, ���ӸӴ�, ?�ڽ� ���...)
    //! \param dwOWNER ������ ������
    void GenerateRewardItem_20060417(DWORD dwOWNER = GAEAID_NULL);
    void GenerateRewardItem_20110722(DWORD dwOWNER = GAEAID_NULL);

	const DWORD CalculateMaxPartyDamage(DWORD& dwRefPartyMasterGaeaID);

	//! �̺�Ʈ�� Ž�� ���� �ý��ۿ� ���� ����� ������ ������� ���Ѵ�.
	float CalculateItemGenRate_20060417( float fGenRate, PGLCHAR pChar, bool bParty );

    //! ���ʿ� ����� ������ ���� Callback
    float Callback_CalculateGenProb ( EMGENINFOTYPE emInfoType, float fDefaultProb, PGLCHAR pChar, bool bParty );

    //! �� �������� Ȯ���� ���� Callback
    float Callback_CalculateGenItemRate ( EMGENITEMTYPE emItemType, float fDefaultRate, WORD wGrade, PGLCHAR pChar );

    //! ���������� �߻��� �����ۿ� ���� Callback (�ݾ� Ȯ�� ��)
    void Callback_CalculateGenResult ( OUT SGENRESULT& sResult, GLChar* const pChar );

    //! �߻��� �����ۿ� ���� �α� Callback
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
	HRESULT UpdateDurDamage ( float fTime, float fElapsedTime );	//	���� ������ ó��( ������ �ش�. ) 
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
    //! �������, �о�� Ȯ�� ��� �� �Ÿ� Ȯ��
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

	//�߻��� ���� ������
	typedef std::vector<SGENRESULT> VEC_RESULT;
    typedef VEC_RESULT::iterator    VEC_RESULT_ITER;
    VEC_RESULT m_vecResult;

	void JackpotProcess( GLChar* pChar, JACKPOT_ITEM* Item );

    //! ��ȯ�� ����
    bool IsNpcRecall() const { return m_bNpcRecall; }

    //! ��ȯ�� �ð�
    __time64_t GetGenTime() const { return m_tGenTime; }
    DWORD GetGenTimeFromData() const { return m_pCrowData->GenTime(); }
    
    /**
        ������ ���� ������ ����� ���� �Ѵ�.
        HP, ���ݷ�, ����, ����ġ, ������ ����� ����� ���� �� �� �ִ�.
        �δ��� ���̵��� ������ ���� ����� ����.

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
        �׾��� �� �����ڰ� �������� �� �� �ִ�.
        �������� �� �ڵ��� ��Ƽ���̵� �ƴϵ� �������� ���� �� ����� �����ȴ�.
        �̰��� ���� �̺�Ʈ�� ����ó���� ��� �ȴ�.

     */
    const DWORD GetKillerID() const { return m_dwKillerDbNum; }

public:
	/// �� ��ġ�� �����ϴ� ���� �����Ѵ�;
	// �⺻ ��ġ�� �������� �����鼭 �� ��ġ�� �̿��� �����Ѵ�;
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
	// �⺻ ��ġ�� �߰��Ǵ� ��ġ, �̴� ���̳ʽ��� �� �� �ִ�;
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
