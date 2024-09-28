#pragma once

#include "../RanLogic/Actor/GLActor.h"
#include "../RanLogic/Character/GLCharData.h"
#include "../RanLogic/s_NetGlobal.h"

struct NET_MSG_GENERIC;
struct SSTATEBLOW;
struct GLSKILL;
struct SSKILLACTEX;

extern STARGETID g_InvalidDummyTarget;

class GLGaeaServer;
class GLARoundSlot;
class GLLandMan;
class GLLandManager;
class CSkillCAInterface;
class ServerActor : public GLActor
{
public:
	/// Getter;
	// School;
	virtual WORD			GetSchool() 								const 		{ return 0; }
	
	virtual BOOL 			IsCtrlBlockBody()							const		= 0;
	virtual BOOL 			IsActionLimit( DWORD dwLimit )				const		= 0;
	virtual BOOL 			IsRunning()									const		= 0;
	virtual bool 			IsSafeTime()								const 		{ return FALSE; }
	virtual bool 			IsBasiceSafeTime()							const 		{ return false; }	
	virtual bool 			IsCDMSafeTime()								const 		{ return false; }
	virtual bool 			IsSafeZone()								const 		{ return false; }
	virtual BOOL 			IsNonDamaged()								const 		{ return FALSE; }

	virtual BOOL			IsVisibleDetect ( ServerActor* pAttacker )	const;

	virtual bool			IsCheckedSkillFlagSpecial ( DWORD dwFlag )	const		{ return false; }

	virtual DWORD 			GETHOLDBLOW()					const = 0;
	
	virtual void			SetDirection(D3DXVECTOR3& vDirection){;}

	virtual WORD			GetBodyRadius()					const = 0;

	virtual WORD			GetLevel() 						const = 0;
	virtual const WORD GetGradeAttack(void) const { return 0; }

	virtual DWORD			GetNowHP() 						const = 0;
	virtual DWORD			GetMaxHP() 						const = 0;
	virtual float			GetMaxHpRate() 					const = 0;
	virtual void 			SetMaxHpRate( float fRate )           = 0;

	virtual WORD			GetNowMP() 						const = 0;
	virtual WORD			GetMaxMP() 						const = 0;

	virtual WORD			GetNowSP() 						const = 0;
	virtual WORD			GetMaxSP() 						const = 0;

	virtual float			GetMoveVelo()							{ return 0.f; }

	virtual int				GetBasicOffenceHigh()			const = 0;
	virtual int				GetBasicOffenceLow()			const = 0;
	virtual int				GetBasicDefence()				const = 0;
	virtual const SRESIST&	GetBasicRegist()				const = 0;
	virtual int				GetBasicHitRate()				const = 0;
	virtual int				GetBasicAvoid()  				const = 0;
	virtual float			GetBasicHPRestorationRate()		const = 0;
	virtual float			GetBasicMPRestorationRate()		const = 0;
	virtual float			GetBasicSPRestorationRate()		const = 0;

	virtual int				GetOffenceHigh()				const = 0;
	virtual int				GetOffenceLow()					const = 0;
	virtual int				GetDefense()					const = 0;
	virtual const SRESIST&	GETRESIST()						const = 0;
	virtual int				GetHitRate()					const = 0;
	virtual int				GetAvoid()  					const = 0;
	virtual float			GetHPRestorationRate()			const = 0;
	virtual float			GetMPRestorationRate()			const = 0;
	virtual float			GetSPRestorationRate()			const = 0;

	virtual const float		GetMultipleHP()					const = 0;
	virtual const float		GetMultipleAttack()				const = 0;
	virtual const float		GetMultipleDefense()			const = 0;
	virtual const float		GetMultipleExp()				const = 0;
	virtual const float		GetMultipleDrop()				const = 0;

	virtual int				GetBodyDefense() 				const	{ return 0; }
	virtual int				GetItemDefense() 				const	{ return 0; }

	virtual float			GetIgnoreBossDamageR()			const	{ return 0.f; }
	virtual float			GetDecRDamage_EmergencyState()	const	{ return 0; }
	virtual int				GetBossFixedDamage()			const	{ return 0; }
	virtual float			GetIncR_ExpAddEffect()			const	{ return 0.f; }
	virtual float			GetIncR_DropGenMoney()			const	{ return 0.f; }
	virtual float			GetDecR_DamageMeleeSkill()		const	{ return 0.f; }
	virtual float			GetDecR_DamageRangeSkill()		const	{ return 0.f; }
	virtual float			GetDecR_DamageMagicSkill()		const	{ return 0.f; }

	virtual const DAMAGE_SPEC&	GetDamageSpec()             const = 0;
	virtual DAMAGE_SPEC*		GetDamageSpecPtr()				  = 0;
	virtual DEFENSE_SKILL		GetDefenseSkill()			const = 0;
	virtual SINCREASEEFF* const GetIncreaseEff() { return NULL; }
	virtual SIGNORED_DAMAGE* const GetIgnoredDamage(void) { return NULL; }

	virtual DWORD			GetBonusExp()					const = 0;

	virtual EMBRIGHT		GetBright() 					const = 0;

	virtual GLARoundSlot*	GETSLOT()								{ return NULL; }
	virtual SCONFTING*		GETCONFTING()							{ return NULL; }
	virtual DWORD			GETCLIENTID()							{ return 0; }
	virtual SSKILLFACT*		GETSKILLFACT()							{ return NULL; }
	virtual int				GETSKILLFACTSIZE()				const	{ return 0; }
	virtual DAMAGE_SPEC*	GETDAMAGESPEC()							{ return NULL; }

	virtual const EMSPEC_DOMINATE_TYPE GetController(void) const { return EMSPEC_DOMINATE_TYPE_SELF; }
	virtual void			SetController(const EMSPEC_DOMINATE_TYPE emDOMINATION_TYPE){;}

	virtual const DWORD		ReceiveDamage(const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag) = 0;
	virtual const DWORD		ReceiveHeal(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag) = 0;
	virtual void			SendPushPull( const D3DXVECTOR3 &vMovePos, const float fPUSH_SPEED, const DWORD dwAniType, const bool bReActor = true ) = 0;

	virtual bool	 		MoveTo( const D3DXVECTOR3* pTarPos, bool bJump, DWORD dwFlags ) = 0;
	virtual void			ReactDamage( ServerActor* pAttacker, DWORD dwDamageFlags )		{}
	virtual void 			DamageReflectionProc(int nDAMAGE, const STARGETID& sACTOR) = 0;
	virtual void 			DamageCurse(int nDAMAGE, const STARGETID& sACTOR, EMCROW emCrow) = 0;
	virtual void 			DefenseSkill(const SNATIVEID& sNativeID, WORD wLevel, const STARGETID& sACTOR) = 0;

	virtual const bool		IsImmuneToSkill ( BYTE iIndex ) const { return false; }

	virtual bool			CheckStateToGo() = 0;
	virtual ServerActor*	GetTarget( const STARGETID& sTargetID ) = 0;

	virtual bool 			STATEBLOW( const SSTATEBLOW &sStateBlow ) = 0;
	virtual float			GETSTATE_DAMAGE() const = 0;

	virtual void 			CURE_STATEBLOW( DWORD dwCUREFLAG ) = 0;
	virtual void 			SKILL_EFFECT_REMOVE( DWORD dwRemoveFlag ) = 0;
	virtual void 			DO_STUN_ACTION() = 0;

    // 주의: 변경시키고자 하는 값의 크기와 실제 변경된 값의 크기가 다를 수 있어서 reference로 실제 변경된 값의 크기를 돌려받도록 함;
    virtual void            VAR_BODY_POINT( const EMCROW emActorCrow, const DWORD dwActorID, const DWORD dwVarTypeFlag, int& nVar_HP, int& nVar_MP, int& nVar_SP ) = 0;
	virtual const BOOL		RECEIVE_SKILLFACT( const SNATIVEID& skill_id, const WORD wlevel, DWORD &dwSELECT, const STARGETID& sID, const SSKILLFACT::STIME sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME), const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT, EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NORMAL, const float* const pLaunchTime = 0, const float* const pLaunchValue = 0) = 0;	
	virtual const BOOL		RECEIVE_SKILLFACTII( const SNATIVEID& skill_id, const WORD wlevel, DWORD &dwSELECT, const STARGETID& sID, const SSKILLFACT::STIME sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME), const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT, EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NORMAL, const float* const pLaunchTime = 0, const float* const pLaunchValue = 0) = 0;	

	virtual const BOOL		RECEIVE_SKILLCA( const SSKILLACTEX& sACTEX ) { return FALSE;	 }

	virtual BOOL			RESET_SKILLFACT( const SNATIVEID& skill_id, DWORD& dwOutSlot, EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NONE ) { return FALSE; }

    virtual const BOOL      REMOVE_SKILLFACT(const SNATIVEID& skill_id) { return FALSE; };
    virtual const BOOL      REMOVE_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON) { return FALSE; };

	// Note : 해당 타겟을 설정한다(병진)
	virtual BOOL 			SetTargeting( const STARGETID& sTargetID ) { return FALSE; }

	virtual void 			RegisterLandEvent( const DWORD dwGUID ) {}
	virtual void 			RemoveLandEvent( const DWORD dwGUID )   {}

	// Target;
	virtual const STARGETID& GetTargetID() const { return g_InvalidDummyTarget; }		// 타겟 아이디;
	virtual void SetTargetID( const STARGETID& sTargetID ) { }							// 타겟을 지정한다;
	virtual const bool IsNonTargetable() const { return false; }						// 타겟 가능한지 판단한다;

public:
	/// 각 수치를 제어하는 값을 설정한다;
	// 기본 수치는 변경하지 않으면서 이 수치를 이용해 변경한다;
	virtual void			SetOffence ( int _nOffence ) = 0;
	virtual void			SetDefence ( int _nDefence ) = 0;
	virtual void			SetFireRegist ( int _nFireRegist ) = 0;
	virtual void			SetIceRegist ( int _nIceRegist ) = 0;
	virtual void			SetElectricRegist ( int _nElectricRegist ) = 0;
	virtual void			SetPoisonRegist ( int _nPoisonRegist ) = 0;
	virtual void			SetSpiritRegist ( int _nSpiritRegist ) = 0;
	virtual void			SetAccuracyRate ( int _nAccuracyRate ) = 0;
	virtual void			SetAvoidanceRate ( int _nAvoidanceRate ) = 0;
	virtual void			SetHPRestorationRate ( float _fHPRestorationRate ) = 0;
	virtual void			SetMPRestorationRate ( float _fMPRestorationRate ) = 0;
	virtual void			SetSPRestorationRate ( float _fSPRestorationRate ) = 0;

	virtual void			SetMultipleHP ( float fMultiple ) = 0;
	virtual void			SetMultipleAttack ( float fMultiple ) = 0;
	virtual void			SetMultipleDefense ( float fMultiple ) = 0;
	virtual void			SetMultipleExp ( float fMultiple ) = 0;
	virtual void			SetMultipleDrop ( float fMultiple ) = 0;

public: // GLGaeaServer;	
	virtual HRESULT SendMsgViewAround(NET_MSG_GENERIC* pMsg) = 0; //! 주변 케릭에 전송할때	
	virtual void SendToClient(NET_MSG_GENERIC* pMsg) = 0; //! 나에게 전송할때	
	virtual void SendToClient(const DWORD dwClientSlot, NET_MSG_GENERIC* pMsg) = 0; //! 다른 Character 에게 전송할때

public: // Land;
	void SetCurrentMap(const MapID& _mapID) { m_mapID = _mapID; }
	const MapID& GetCurrentMap() const { return m_mapID; }	
	GLLandMan* const GetLandMan(void);
	GLLandManager* const GetParentLandManager(void);

public: // Party;
	const bool isValidParty(void) const;
	const bool isValidExpedition(void) const;
	const GLPartyID& GetPartyID(void) const;

	virtual void SetPartyID(const GLPartyID& PartyID);
	virtual void ResetPartyID(void);

public:
	GLGaeaServer* const GetGaeaServer() {return m_pGaeaServer;};

public: // Battle;
	//! 타겟이 해당 공격을 막았는지(병진)
	const bool CalcBlocking(ServerActor* const pTargetActor, const int nDamage, const GLSKILL* const pSkill = NULL, const DWORD dwSkillLevel = 0);

	virtual void TurnAction(const EMACTIONTYPE toAction, const bool bMsg = true) = 0;

public:
	virtual void Reset(void) override;

	// Child Crow;
	virtual void RemoveAllChildCrow ();									// 모든 자식 Crow를 삭제한다;
	virtual void KillAllChildCrow ();									// 모든 자식 Crow를 죽인다;
	virtual void KillAllHireChildCrow ();								// 고용된 자식 Crow를 죽인다;

private:
	void OwnerAttack ();												// 부모 Crow가 공격할 때 호출된다;
	void OwnerAttacked ( ServerActor* const pAttacker );				// 부모 Crow가 공격당할 때 호출된다;

public:
	void SendToChildCrowAttack ();										// 모든 자식 Crow에게 공격 사실을 알린다;
	void SendToChildCrowAttacked ( ServerActor* const pAttacker );		// 모든 자식 Crow에게 공격당한 사실을 알린다;

public:
	// Action;
	virtual STARGETID SearchTargetAction ( EMIMPACT_SIDE emTargetSide ) { return STARGETID(); }

	// 외부에 노출시키는 Callback;
	virtual void ApplyDelay ( float fDelayTime ) {}						// 공격을 못하도록 딜레이를 적용시킨다;
	virtual bool GetDelayState () { return false; }						// 딜레이 상태인지 확인한다;
	virtual ServerActor* GetOwner () { return NULL; }					// 주인을 불러온다;
	virtual bool IsTarget () { return true; }							// 타겟이 있는지 확인;
	virtual bool IsTargetExcludeOwner () { return false; }
	virtual bool IsValidOwnerDistance () { return true; }				// 주인과의 거리 유효성 판단;
	virtual bool IsValidEventSkillDistance () { return true; }			// 이벤트 스킬을 위한 주인과의 거리 유효성 판단;
	virtual bool IsRunAway () { return false; }
	virtual const DWORD GetActionCount () { return 0; }					// 액션 횟수;
	virtual void AutoSearchTarget () { }								// 자동 타겟 검색;
	virtual void SetOwnerTarget () { }									// 주인이 있는 경우 주인의 타겟을 타겟팅함;
	virtual void StartTargetAction ( DWORD dwCount = 0 ) { }			// 타겟 액션 시작;
	virtual void DieAction () { }										// 죽음;
	virtual void ChangeSkillPage ( UINT iPage ) { }						// 스킬 페이지 변경;
	virtual void UseEventSkill ( UINT iIdx ) { }						// 특정 이벤트 스킬 사용;
	virtual void RunAwayAction ( bool bSpeedUp = false ) { }			// 도망 액션;
	virtual void NormalAction () { }									// 일반 액션;
	virtual void TracingOwner( bool bSpeedUp = false ) { }				// 주인을 따라다닌다;
	virtual void TracingTarget( bool bSpeedUp = false ) { }				// 타겟을 따라다닌다;
	virtual void SyncPositionTarget () { }								// 타겟의 위치와 액터의 위치를 동기화한다;

	// 스킬 효과를 적용시킨다;
	// 적용 후 반드시 해제해줘야 한다;
	virtual void Update_ApplySkillEffect ( EMSPEC_ADDON emSpecAddon, bool bState ) { }

public:
	// 로그를 찍는다;
	virtual void LogInfo ( const char* szLog ) { sc::writeLogInfo( szLog ); }

	// AI 상태를 변경한다;
	virtual void ChangeAIState ( const char* szChangeName );

public:
	const DWORD GetChildSummonTypeCount ( SUMMON_TYPE emSummonType ) const;
	const DWORD GetChildSummonAITypeCount ( DWORD dwAIType ) const;
	const DWORD GetChildSummonIDCount ( SNATIVEID sSummonCrowID ) const;
	const Faction::GLActorID RemoveFirstChildSummon ( SUMMON_TYPE emSummonType, DWORD dwAIType, SNATIVEID sCrowID = SNATIVEID( false ) );

public:
	ServerActor(GLGaeaServer* const pGaeaServer, const EMCROW emCrow);
	virtual ~ServerActor(void);

protected:
	GLGaeaServer* const m_pGaeaServer;

	// 현재 케릭터측에서만 사용하고 있지만, 궁극적으로는 그룹화 CROW도 이용할 수 있도록 하면 좋음;
	GLPartyID m_PartyID;	

	// baseMapID가 아닌 instanceMapID이다, 인던이 아닌경우 instanceMapID = baseMapID 이다;
	MapID m_mapID;

public:	
	DWORD m_dwCeID;  // 셀 ID;

//	GLLandMan* m_pLandMan;
};


