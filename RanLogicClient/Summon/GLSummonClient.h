#pragma once

#include "../../SigmaCore/Container/GLList.h"

#include "../../EngineLib/NaviMesh/Actor.h"
#include "../../EngineLib/G-Logic/GLQuadTree.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"

#include "../../RanLogic/Summon/GLSummon.h"
#include "../../RanLogic/Skill/GLSkill.h"

#include "../ClientActor.h"

class GLLandManClient;
class GLTransformSet;

class GLSummonClient : public GLSUMMON, public ClientActor
{
public:
    GLSummonClient(GLGaeaClient* pGaeaClient);
    virtual ~GLSummonClient(void);

protected:    

	LPDIRECT3DDEVICEQ	m_pd3dDevice;	
	DxSkinChar*			m_pSkinChar;
	SkinCharMulti*		m_pSkinCharMulti;
	GLTransformSet*		m_pTransformSet;
    BOOL                m_bVisible; // 은신등으로 인해 플레이어(나)에게 보여지는지에 대한 플래그, false일 경우 렌더링/타겟팅 되지 않음;

	STARGETID			m_sTargetID;

	WORD				m_wTARNUM;				//	타겟 숫자.
	STARID				m_sTARIDS[EMTARGET_NET];//	타겟들.
	D3DXVECTOR3			m_vTARPOS;				//	타겟 위치.

	BOOL				m_bBeginAlpha;			//  사망시 서서히 사라짐 발동 여부

public:
	GLLandManClient*		    m_pLandManClient;	//	맵 인스턴스 포인터.

public:	
//	EMACTIONTYPE		m_Action;				//	행동.
	WORD				m_wAniSub;

	SCROWATTACK*		m_pAttackProp;			//	공격 속성.

public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	float				m_fHeight;

	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;

protected:
	float m_fattTIMER;
	int   m_nattSTEP;

private:
	std::tr1::shared_ptr<ActorWorldLight>		m_spWorldLight;
	std::tr1::shared_ptr<ActorNameCollision>	m_spNameCollision;
	std::tr1::shared_ptr<ActorNameCollision>	m_spCollisionByObject;

protected:
	// Move
	void    MoveProc          ( float fElpaseTime        );
	void    Move              ( const D3DXVECTOR3& vPos  );
	void    Move_TargetTracing( const STARGETID& sTarget );

	// Attack
	void    StartAttackProc();
	BOOL    AttackProc( float fElapsedTime );
	void    AttackEffect( const SANIMSTRIKE &sStrikeEff );

	// Skill
	void    StartSkillProc();
	BOOL    SkillProc( float fElapsedTime );

	void    SKT_EFF_HOLDOUT( STARGETID sTarget, DWORD dwDamageFlag );
	void    SK_EFF_TARG( const PGLSKILL pSkill, const SANIMSTRIKE &sStrikeEff, const STARGETID &sTarget );
	void    SK_EFF_SELFZONE( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget=NULL );
	void    SK_EFF_TARZONE( const STARGETID &sTarget, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF );
	void    SkillEffect( const SANIMSTRIKE &sStrikeEff );

	// SkillFact
	void AddSkillFact_SpecialAddon_Client(const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF);

	//void RECEIVE_SKILLFACT_Invisible(const float fVAR2);
	const BOOL RECEIVE_SKILLFACT_AIRBORNE(const SKILL::SSPEC& sSPEC, const SKILL::CDATA_LVL& sSKILL_DATA);

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

public:
	const BOOL IsNonTargetable(void) const override;

public:
    virtual void FactEffectProcessing ( const EMFACTEFFPROCESSING emFactEffactCase );

public:
	void ASSIGN( PSDROPSUMMON pDropSummon );

	BOOL	IsVisibleDisplayName() const;

	DxSkinChar* GetSkinChar()							{ return m_pSkinChar; }

//	virtual BOOL IsValidBody () const override;

	float GetDirection();
	virtual float GetMoveVelo();

	HRESULT Create( GLLandManClient *pLandManClient, PSDROPSUMMON pDropSummon, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT SkinLoad( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual void UPDATE_DATA_FINISH(const DWORD dwSkillFactIndex) override { UPDATE_DATA_END( dwSkillFactIndex ); }

	virtual void UPDATE_DATA_END ( const DWORD dwSkillFactIndex );	

	HRESULT UpdateAnimation( float fTime, float fElapsedTime );
	HRESULT	UpdateTurnAction( float fTime, float fElapsedTime );
	void    UpdateLandEffect();
	void	UpdateMapEffect();

	void TurnAction( EMACTIONTYPE toAction );


//	BOOL IsActState( DWORD dwStateFlag )		 			{ return m_dwActionFlag&dwStateFlag; }
//	void SetActState( DWORD dwStateFlag )					{ m_dwActionFlag |= dwStateFlag; }
//	void ReSetActState( DWORD dwStateFlag )				{ m_dwActionFlag &= ~dwStateFlag; }	

	void MsgGoto(NET_MSG_GENERIC* nmg);
	void MsgUpdateEffect(NET_MSG_GENERIC* nmg);
	void MsgUpdateCurSkillPage(NET_MSG_GENERIC* nmg);
	void MsgUpdateState(NET_MSG_GENERIC* nmg);
	void MsgSkillFact(NET_MSG_GENERIC* nmg);
	void MsgAttack(NET_MSG_GENERIC* nmg);
	void MsgSkill(NET_MSG_GENERIC* nmg);
	void MsgSkillEnd(NET_MSG_GENERIC* nmg);

	void MsgApplySkillEffect ( NET_MSG_GENERIC* pMsg );

	void SELECTANI( DxSkinChar* pRenderChar, const TCHAR* szAnim, DWORD dwFlags=NULL);
	void SELECTANI( DxSkinChar* pRenderChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0);
	void DEFAULTANI( DxSkinChar* pRenderChar );
	void TOSTARTTIME( DxSkinChar* pRenderChar );
	void TOENDTIME( DxSkinChar* pRenderChar );
	BOOL PutPassiveEffect ( DxSkinChar *pSkinChar, const char* szEffFile, const D3DXVECTOR3 *pDir, float fTarDir=0.0f );
	void OutEffect ( DxSkinChar *pSkinChar, std::string strEffFile );
	//void CalculateAmbientColor( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, const D3DXVECTOR3& vPos );	// Ambient Color를 계산한다.
	void SELECTSKILLANI (  DxSkinChar *pSkinChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE );
	// None const
	BOOL IsANI_SELECTANI ( DxSkinChar *pSkinChar, const TCHAR* szAnim, DWORD dwFlags=NULL );
	BOOL IsANI_SELECTANI( DxSkinChar *pSkinChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0 );
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
	HRESULT UpdateSuit( BOOL bChangeHair, BOOL bChangeFace );

	// Default FrameWork
	HRESULT FrameMove( const boost::shared_ptr<DxLandMan>& spLandMan, float fTime, float fElapsedTime );
	HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );
	HRESULT RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	HRESULT RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	HRESULT RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bWorldSystemOLD );
	HRESULT RenderShadow_SpotLight( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bUsedMaterialSystem, DWORD dwSlot );
	HRESULT RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderAniEff( LPDIRECT3DDEVICEQ pd3dDevice );

	void RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar* pRenderChar, BOOL bOpaqueState, BOOL bHALF_VISIBLE );

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

	void MsgProcess(NET_MSG_GENERIC* nmg);

//	virtual BOOL isAction( EMACTIONTYPE emCur ) const override { return m_Action == emCur; }

	virtual void SetGaeaID ( const DWORD dwGaeaID ) override
	{
		m_dwGlobalID = dwGaeaID;
		m_dwGaeaID = dwGaeaID;
	}
    virtual const DWORD GetCharID() const	override { return m_dwGaeaID; }

	virtual WORD              GetBodyRadius();
	virtual float             GetBodyHeight()			{ return m_fHeight; }
	virtual D3DXVECTOR3       GetPosBodyHeight()		{ return GetPosition() + D3DXVECTOR3(0.0f, m_fHeight + m_sAirborne.getHeight(), 0.0f); }
	
	virtual const char*       GetName() const	        { return m_pSummonCrowData->GetName(); }
	virtual const SNATIVEID GetCrowID(void) const { return m_sSummonID; }
	virtual GLDWDATA          GetHp() const				{ return GLDWDATA(GETHP(),GETMAXHP()); }

	virtual const D3DXVECTOR3 &GetMaxOrg()             { return m_vMaxOrg; }
	virtual const D3DXVECTOR3 &GetMinOrg()             { return m_vMinOrg; }

	virtual void ReceiveDamage(const int nDamage, DWORD dwDamageFlag, WORD wSchool, const STARGETID& sACTOR) override;
	virtual void ReceiveHeal(const int nHeal, DWORD dwHealFlag, WORD wSchool, const STARGETID& sACTOR) override;
	virtual void ReceiveBlocking(const STARGETID& sACTOR) override;
	virtual void ReceiveAVoid( const STARGETID& sACTOR ) override;
    virtual void ReceiveSwing() override;

	virtual BOOL IsDominated() const { return m_sDOMINATION.emController == EMSPEC_DOMINATE_TYPE_CASTER; }
	BOOL IsVisibleDetect(void) const override;
	BOOL IsCollisionVolume();		

	//	virtual const D3DXVECTOR3 &GetPosition()		{ return m_vPos; }

	const STARGETID& GetTargetID() const { return m_sTargetID; }

	bool IsSummonNpcActionable( const D3DXVECTOR3& vReqPos, const float fRadius,	int nNpcAction = -1 )
	{ 
		return ISSummonNPC_INTERACTIONABLE(vReqPos, GetPosition(), fRadius, nNpcAction);
	}

private:
	const BOOL CalcVisibleDetect ( const float fTime );
	const BOOL CalcVisibleDetectAuto (
		const float fTime,
		const float fDistance,
		const D3DXVECTOR3& vDistance );
};

typedef GLSummonClient*          PGLSUMMONCLIENT;
typedef sc::CGLLIST<PGLSUMMONCLIENT> GLSUMMONCLIENTLIST;
typedef sc::SGLNODE<PGLSUMMONCLIENT> GLSUMMONCLIENTNODE;



