#ifndef GLMETERIAL_H_
#define GLMETERIAL_H_

#pragma once

#include <deque>
#include "../../RanLogic/Crow/GLMobSchedule.h"
#include "../../RanLogic/Crow/GLCrowData.h"
#include "../../EngineLib/NaviMesh/Actor.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
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

namespace GLMSG
{
    struct SNETDROP_MATERIAL;    
}

class GLGaeaServer;

class GLMaterial : public ServerActor
{
public:
    GLMaterial(GLGaeaServer* pGaeaServer);
    virtual ~GLMaterial(void);

public:
	PCROWDATA		m_pCrowData;					//	Crow 기본 정보.
	SNATIVEID			m_sNativeID;					//	Crow NativeID.

public:
	
	LPDIRECT3DDEVICEQ	m_pd3dDevice;	
	DxSkinChar*			m_pSkinChar;

// 	Actor				m_actorMove;
// 	D3DXVECTOR3			m_vDirect;
// 	D3DXMATRIX			m_matTrans;
// 	D3DXVECTOR3			m_vPosition;					//	현제 위치.

	//	Note : AABB
protected:
	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;

public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	//	Note : 제어 코드.
	//
public:	
	LANDQUADNODE*			m_pQuadNode;			//	쿼드 트리 노드.
	sc::SGLNODE<GLMaterial*>*		m_pCellNode;			//	셀 리스트 노드.

	DWORD	m_dwGatherCharID;		//	채집중인 캐릭터
	float		m_fGatherTime;			//	채집중 남은시간

	DAMAGE_SPEC			m_sDamageSpec;		//	데미지 흡수 및 반사
	//	Note : 행동.
protected:
	float						m_fAge;					//	경과 시간.
	DWORD					m_dwCFlag;			//	콘트롤 플래그.	
	D3DXVECTOR3			m_vGenPos;			//	최초 생성위치.
	D3DXVECTOR3			m_vStartPos;			// 이동메시지 보낼때 시작위치
	float						m_fIdleTime;			//	휴식 시간.

//	Note : 스케쥴.
protected:
	GLMobSchedule*		m_pMobSchedule;

public:
	GLMobSchedule* GetMobSchedule ()			{ return m_pMobSchedule; }


public:
    virtual const DWORD GetCharID() const	override { return m_dwGaeaID; }

	virtual DWORD GETPARTYID () const				{ return 0; }
		
	virtual BOOL IsCtrlBlockBody () const { return FALSE; }
	virtual BOOL IsActionLimit( DWORD dwLimit ) const { return FALSE; } 
	
	virtual WORD GetBodyRadius () const				{ return m_pCrowData->m_sAction.m_wBodyRadius; }

	virtual DWORD GETHOLDBLOW () const				{ return 0; }
	virtual WORD GetLevel () const					{ return m_pCrowData->m_wLevel; }

	virtual DWORD GETHP () const					{ return 0; }	
	virtual DWORD GetNowHP () const					{ return 0; }
	virtual DWORD GetMaxHP () const					{ return 0; }
    virtual float GetMaxHpRate () const				{ return 0; }
    virtual void  SetMaxHpRate( float fRate )       {}

	virtual WORD GetNowMP () const					{ return 0; }
	virtual WORD GetMaxMP () const					{ return 0; }

	virtual WORD GetNowSP () const					{ return 0; }
	virtual WORD GetMaxSP () const					{ return 0; }

	virtual int				GetBasicOffenceHigh() const			{ return 0; }
	virtual int				GetBasicOffenceLow() const			{ return 0; }
	virtual int				GetBasicDefence() const				{ return 0; }
	virtual const SRESIST&	GetBasicRegist() const				{ return m_pCrowData->m_sResist; }
	virtual int				GetBasicHitRate() const				{ return 0; }
	virtual int				GetBasicAvoid() const				{ return 0; }
	virtual float			GetBasicHPRestorationRate() const	{ return 0; }
	virtual float			GetBasicMPRestorationRate() const	{ return 0; }
	virtual float			GetBasicSPRestorationRate() const	{ return 0; }

	virtual int				GetOffenceHigh () const				{ return 0; }
	virtual int				GetOffenceLow () const				{ return 0; }
	virtual int				GetDefense () const					{ return 0; }
	virtual const SRESIST&	GETRESIST () const					{ return m_pCrowData->m_sResist; }
	virtual int				GetHitRate () const					{ return 0; }
	virtual int				GetAvoid () const					{ return 0; }
	virtual float			GetHPRestorationRate() const		{ return 0; }
	virtual float			GetMPRestorationRate() const		{ return 0; }
	virtual float			GetSPRestorationRate() const		{ return 0; }

	virtual const float		GetMultipleHP() const				{ return 1.f; }
	virtual const float		GetMultipleAttack() const			{ return 1.f; }
	virtual const float		GetMultipleDefense() const			{ return 1.f; }
	virtual const float		GetMultipleExp() const				{ return 1.f; }
	virtual const float		GetMultipleDrop() const				{ return 1.f; }


	virtual DWORD GetBonusExp () const				{ return 0; }

	virtual EMBRIGHT GetBright () const				{ return BRIGHT_LIGHT; }
	virtual BOOL IsRunning () const					{ return false; }

	ServerActor*	GetTarget(const STARGETID& sTargetID)								{ return NULL; }
	bool			CheckStateToGo()													{ return true; }	
	bool	 		MoveTo( const D3DXVECTOR3* pTarPos, bool bJump, DWORD dwFlags )		{ return S_OK; }

	virtual const DWORD ReceiveDamage ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag ) override { return 0;}
	virtual const DWORD ReceiveHeal(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag){ return 0;}
	virtual void SendPushPull( const D3DXVECTOR3 &vMovePos, const float fPUSH_SPEED, const DWORD dwAniType, const bool bReActor = true ) {}

	virtual bool STATEBLOW ( const SSTATEBLOW &sStateBlow ) { return false; }
	virtual void DO_STUN_ACTION () {}
	virtual void CURE_STATEBLOW ( DWORD dwCUREFLAG ) {}
	virtual void SKILL_EFFECT_REMOVE ( DWORD dwRemoveFlag ){}

    virtual void VAR_BODY_POINT( 
        const EMCROW emActorCrow,
        const DWORD dwActorID,
        const DWORD dwVarTypeFlag,
        int& nVar_HP,
        int& nVar_MP, 
        int& nVar_SP ) override {}

	
	virtual const BOOL RECEIVE_SKILLFACT(
        const SNATIVEID& skill_id,
        const WORD wlevel,
        DWORD &dwSELECT,
        const STARGETID& sID,
        const SSKILLFACT::STIME sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME),
        const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT,
        EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NORMAL,
		const float* const pLaunchTime = 0, const float* const pLaunchValue = 0) override
    { 
        return false;
    }

	virtual const BOOL RECEIVE_SKILLFACTII(
		const SNATIVEID& skill_id,
		const WORD wlevel,
		DWORD &dwSELECT,
		const STARGETID& sID,
		const SSKILLFACT::STIME sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME),
		const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT,
		EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NORMAL,
		const float* const pLaunchTime = 0, const float* const pLaunchValue = 0) override
	{ 
		return false;
	}

    virtual const BOOL      REMOVE_SKILLFACT(const SNATIVEID& skill_id) override
    { return FALSE; };
    virtual const BOOL      REMOVE_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON) override
    { return FALSE; };

	virtual float GETSTATE_DAMAGE () const			{ return 0.0f; }

	virtual const DAMAGE_SPEC&	GetDamageSpec() const		{ return m_sDamageSpec; }
	virtual DAMAGE_SPEC*		GetDamageSpecPtr()			{ return &m_sDamageSpec; }
	virtual DEFENSE_SKILL GetDefenseSkill() const	{ return DEFENSE_SKILL(); }
	virtual void DamageReflectionProc(int nDAMAGE, const STARGETID& sACTOR) override {}
	virtual void DamageCurse(int nDAMAGE, const STARGETID& sACTOR, EMCROW emCrow) override {}
	virtual void DefenseSkill(const SNATIVEID& sNativeID, WORD wLevel, const STARGETID& sACTOR ) override {}

	/// 각 수치를 제어하는 값을 설정한다;
	// 기본 수치는 변경하지 않으면서 이 수치를 이용해 변경한다;
	// Material 에서는 사용되지 않는다;
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
	virtual void TurnAction(const EMACTIONTYPE toAction, const bool bMsg = true) override;

public:
	float GetAge() const { return m_fAge; }

protected:
    DWORD GenerateGathering( DWORD dwGaeaID, bool& bDrop );
    DWORD GenerateGathering_20060417( DWORD dwGaeaID, bool& bDrop );
    DWORD GenerateGathering_20110722( DWORD dwGaeaID, bool& bDrop );
	float CalculateItemGenRate( float fGenRate, PGLCHAR pChar, bool bParty );

public:
	void ReqNetMsg_Drop(GLMSG::SNETDROP_MATERIAL& Msg);

protected:
	typedef std::vector<DWORD>		VECPC;
	typedef VECPC::iterator			VECPC_ITER;

	DWORD	m_dwPC_AMOUNT;
	VECPC	m_vecPC_VIEW;

	void INSERT_PC_VIEW ( DWORD dwGAEAID );

public:
	HRESULT UpdateViewAround ();
	virtual HRESULT SendMsgViewAround(NET_MSG_GENERIC* pMsg);	
	virtual void SendToClient(NET_MSG_GENERIC* pMsg);	
	virtual void SendToClient(const DWORD dwClientSlot, NET_MSG_GENERIC* pMsg);

public:
	HRESULT UpateAnimation ( float fTime, float fElapsedTime );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT UpdateGathering( float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );

public:
	void RESET_DATA ();
	HRESULT CreateMaterial(GLMobSchedule *pMobSchedule, GLLandMan* pLandMan, const SNATIVEID& sCROWID, LPDIRECT3DDEVICEQ pd3dDevice=NULL, LPD3DXVECTOR3 pPos=NULL );

public:
	void Generate(GLChar* pChar, bool bParty = false);
	void GenerateItem(GLChar* pChar, SGENINFO::VEC_SPGENITEM& vecGenItem, OUT SGENRESULT& sResult, bool bParty = false);

	//발생한 보상 아이템
	typedef std::vector<SGENRESULT> VEC_RESULT;
    typedef VEC_RESULT::iterator    VEC_RESULT_ITER;
    VEC_RESULT m_vecResult;
};

typedef GLMaterial* PGLMATERIAL;

typedef sc::CGLLIST<PGLMATERIAL>	GLMATERIALLIST;
typedef sc::SGLNODE<PGLMATERIAL>	GLMATERIALNODE;

#endif // GLCROW_H_