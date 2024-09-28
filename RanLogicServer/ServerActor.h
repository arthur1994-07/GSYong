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

    // ����: �����Ű���� �ϴ� ���� ũ��� ���� ����� ���� ũ�Ⱑ �ٸ� �� �־ reference�� ���� ����� ���� ũ�⸦ �����޵��� ��;
    virtual void            VAR_BODY_POINT( const EMCROW emActorCrow, const DWORD dwActorID, const DWORD dwVarTypeFlag, int& nVar_HP, int& nVar_MP, int& nVar_SP ) = 0;
	virtual const BOOL		RECEIVE_SKILLFACT( const SNATIVEID& skill_id, const WORD wlevel, DWORD &dwSELECT, const STARGETID& sID, const SSKILLFACT::STIME sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME), const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT, EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NORMAL, const float* const pLaunchTime = 0, const float* const pLaunchValue = 0) = 0;	
	virtual const BOOL		RECEIVE_SKILLFACTII( const SNATIVEID& skill_id, const WORD wlevel, DWORD &dwSELECT, const STARGETID& sID, const SSKILLFACT::STIME sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME), const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT, EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NORMAL, const float* const pLaunchTime = 0, const float* const pLaunchValue = 0) = 0;	

	virtual const BOOL		RECEIVE_SKILLCA( const SSKILLACTEX& sACTEX ) { return FALSE;	 }

	virtual BOOL			RESET_SKILLFACT( const SNATIVEID& skill_id, DWORD& dwOutSlot, EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NONE ) { return FALSE; }

    virtual const BOOL      REMOVE_SKILLFACT(const SNATIVEID& skill_id) { return FALSE; };
    virtual const BOOL      REMOVE_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON) { return FALSE; };

	// Note : �ش� Ÿ���� �����Ѵ�(����)
	virtual BOOL 			SetTargeting( const STARGETID& sTargetID ) { return FALSE; }

	virtual void 			RegisterLandEvent( const DWORD dwGUID ) {}
	virtual void 			RemoveLandEvent( const DWORD dwGUID )   {}

	// Target;
	virtual const STARGETID& GetTargetID() const { return g_InvalidDummyTarget; }		// Ÿ�� ���̵�;
	virtual void SetTargetID( const STARGETID& sTargetID ) { }							// Ÿ���� �����Ѵ�;
	virtual const bool IsNonTargetable() const { return false; }						// Ÿ�� �������� �Ǵ��Ѵ�;

public:
	/// �� ��ġ�� �����ϴ� ���� �����Ѵ�;
	// �⺻ ��ġ�� �������� �����鼭 �� ��ġ�� �̿��� �����Ѵ�;
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
	virtual HRESULT SendMsgViewAround(NET_MSG_GENERIC* pMsg) = 0; //! �ֺ� �ɸ��� �����Ҷ�	
	virtual void SendToClient(NET_MSG_GENERIC* pMsg) = 0; //! ������ �����Ҷ�	
	virtual void SendToClient(const DWORD dwClientSlot, NET_MSG_GENERIC* pMsg) = 0; //! �ٸ� Character ���� �����Ҷ�

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
	//! Ÿ���� �ش� ������ ���Ҵ���(����)
	const bool CalcBlocking(ServerActor* const pTargetActor, const int nDamage, const GLSKILL* const pSkill = NULL, const DWORD dwSkillLevel = 0);

	virtual void TurnAction(const EMACTIONTYPE toAction, const bool bMsg = true) = 0;

public:
	virtual void Reset(void) override;

	// Child Crow;
	virtual void RemoveAllChildCrow ();									// ��� �ڽ� Crow�� �����Ѵ�;
	virtual void KillAllChildCrow ();									// ��� �ڽ� Crow�� ���δ�;
	virtual void KillAllHireChildCrow ();								// ���� �ڽ� Crow�� ���δ�;

private:
	void OwnerAttack ();												// �θ� Crow�� ������ �� ȣ��ȴ�;
	void OwnerAttacked ( ServerActor* const pAttacker );				// �θ� Crow�� ���ݴ��� �� ȣ��ȴ�;

public:
	void SendToChildCrowAttack ();										// ��� �ڽ� Crow���� ���� ����� �˸���;
	void SendToChildCrowAttacked ( ServerActor* const pAttacker );		// ��� �ڽ� Crow���� ���ݴ��� ����� �˸���;

public:
	// Action;
	virtual STARGETID SearchTargetAction ( EMIMPACT_SIDE emTargetSide ) { return STARGETID(); }

	// �ܺο� �����Ű�� Callback;
	virtual void ApplyDelay ( float fDelayTime ) {}						// ������ ���ϵ��� �����̸� �����Ų��;
	virtual bool GetDelayState () { return false; }						// ������ �������� Ȯ���Ѵ�;
	virtual ServerActor* GetOwner () { return NULL; }					// ������ �ҷ��´�;
	virtual bool IsTarget () { return true; }							// Ÿ���� �ִ��� Ȯ��;
	virtual bool IsTargetExcludeOwner () { return false; }
	virtual bool IsValidOwnerDistance () { return true; }				// ���ΰ��� �Ÿ� ��ȿ�� �Ǵ�;
	virtual bool IsValidEventSkillDistance () { return true; }			// �̺�Ʈ ��ų�� ���� ���ΰ��� �Ÿ� ��ȿ�� �Ǵ�;
	virtual bool IsRunAway () { return false; }
	virtual const DWORD GetActionCount () { return 0; }					// �׼� Ƚ��;
	virtual void AutoSearchTarget () { }								// �ڵ� Ÿ�� �˻�;
	virtual void SetOwnerTarget () { }									// ������ �ִ� ��� ������ Ÿ���� Ÿ������;
	virtual void StartTargetAction ( DWORD dwCount = 0 ) { }			// Ÿ�� �׼� ����;
	virtual void DieAction () { }										// ����;
	virtual void ChangeSkillPage ( UINT iPage ) { }						// ��ų ������ ����;
	virtual void UseEventSkill ( UINT iIdx ) { }						// Ư�� �̺�Ʈ ��ų ���;
	virtual void RunAwayAction ( bool bSpeedUp = false ) { }			// ���� �׼�;
	virtual void NormalAction () { }									// �Ϲ� �׼�;
	virtual void TracingOwner( bool bSpeedUp = false ) { }				// ������ ����ٴѴ�;
	virtual void TracingTarget( bool bSpeedUp = false ) { }				// Ÿ���� ����ٴѴ�;
	virtual void SyncPositionTarget () { }								// Ÿ���� ��ġ�� ������ ��ġ�� ����ȭ�Ѵ�;

	// ��ų ȿ���� �����Ų��;
	// ���� �� �ݵ�� ��������� �Ѵ�;
	virtual void Update_ApplySkillEffect ( EMSPEC_ADDON emSpecAddon, bool bState ) { }

public:
	// �α׸� ��´�;
	virtual void LogInfo ( const char* szLog ) { sc::writeLogInfo( szLog ); }

	// AI ���¸� �����Ѵ�;
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

	// ���� �ɸ����������� ����ϰ� ������, �ñ������δ� �׷�ȭ CROW�� �̿��� �� �ֵ��� �ϸ� ����;
	GLPartyID m_PartyID;	

	// baseMapID�� �ƴ� instanceMapID�̴�, �δ��� �ƴѰ�� instanceMapID = baseMapID �̴�;
	MapID m_mapID;

public:	
	DWORD m_dwCeID;  // �� ID;

//	GLLandMan* m_pLandMan;
};


