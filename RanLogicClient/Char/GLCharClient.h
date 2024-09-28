#ifndef GLCHARCLIENT_H_
#define GLCHARCLIENT_H_

#pragma once

#include <string>
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../EngineLib/NaviMesh/Actor.h"

#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLogicEx.h"
#include "../../RanLogic/Market/GLPrivateMarket.h"
#include "../../RanLogic/Vehicle/GLVEHICLE.h"
#include "../../RanLogic/Skill/GLCountAttackSkill.h"
#include "../ClientActor.h"

#ifndef GLLandManClient
	class GLLandManClient;
	class GLClientNode;
	typedef SQUADNODE<GLClientNode> CLIENTQUADNODE;
#endif //GLLandMan

class GLGaeaClient;
class GLTransformSet;
struct ActorWorldLight;
struct ActorNameCollision;
class SkinCharMulti;

class GLCharClient : public GLCHARLOGIC, public ClientActor
{
public:
    GLCharClient(GLGaeaClient* pGaeaClient);
    virtual ~GLCharClient(void);

protected:    
	LPDIRECT3DDEVICEQ	m_pd3dDevice;	
	DxSkinChar*		m_pSkinChar;
	SkinCharMulti* m_pSkinCharMulti;
	GLTransformSet*	m_pTransformSet;
	BOOL m_bVisible; // 은신등으로 인해 플레이어(나)에게 보여지는지에 대한 플래그, false일 경우 렌더링/타겟팅 되지 않음;
	
	D3DXMATRIX			m_matBikeTrans;

	//	Note : 현재 상태.
	//
	float				m_fAge;		
	D3DXVECTOR3			m_vTarPos;

	float				m_fIdleTime;
	STARGETID			m_sTargetID;			//	목표 유닛.

	float				m_fMoveDelay;			//	이동 딜래이.

	DWORD				m_dwANISUBSELECT;
	DWORD				m_dwANISUBGESTURE;

	//	Note : AABB
	//
	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;

public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	float				m_fHeight;

	//	Note : 제어 코드.
	//
public:
	std::string			m_strName;	
	DWORD				m_dwCeID;				//	셀 ID.

	GLLandManClient*		m_pLandManClient;	//	맵 인스턴스 포인터.
	//sc::SGLNODE<GLCharClient*>*	m_pGlobNode;		//	맵 리스트 노드.
	//CLIENTQUADNODE*			m_pQuadNode;		//	쿼드 트리 노드.
	//sc::SGLNODE<GLCharClient*>*	m_pCellNode;		//	셀 리스트 노드.

	SUMMONIDLIST            m_dwSummonGUIDList; // 소환수 GUID 리스트

	//	Note : 기초 대이터.
	//
protected:
	EMANI_MAINTYPE		m_emANIMAINSKILL;
	EMANI_SUBTYPE		m_emANISUBSKILL;
	EMANI_MAINTYPE		m_emANIVehicleSKILL;

	EMANI_SUBTYPE		m_emANISUBTYPE;
	DWORD				m_dwRevData;
	SDROP_CHAR			m_CharData;

	VEC_LANDEFF_VALUE	m_vecLandEffect;				//  지형 효과들
	VEC_MAP_EFFECT		m_vecMapEffect;					//	맵에 적용되는 효과들

	float				m_fITEM_MOVE_R;					//  아이템 이동 속도 비율	
	float				m_fITEMATTVELO_R;				//  아이템 공격속도 비율
	float				m_fITEM_MOVE;					//  아이템 이동 속도 절대치	
	float				m_fITEMATTVELO;				//  아이템 공격속도 절대치

	WORD				m_wTARNUM;
	D3DXVECTOR3			m_vTARPOS;
	STARID				m_sTARIDS[EMTARGET_NET];
	CSkillCAIdx			m_sTARCA[EMTARGET_NET];

	WORD				m_wACTIVESKILL_LEVEL;
	SNATIVEID			m_idACTIVESKILL;

    char m_ClubName[CHAR_SZNAME]; //! 클럽이름

protected:
	float				m_fattTIMER;
	int					m_nattSTEP;

public:
	GLPrivateMarket		m_sPMarket;						// 개인 상점.

protected:
	GLVEHICLE			m_sVehicle;
	DWORD				m_dwVehiclePassengerID;	// Vehicle 내가 어느위치에 앉아 있는가~?
												// 난 다른 Vehicle 에 탈 수도 있으므로 GLVEHICLE 안에 들어가면 안된다.

public:
	bool				m_bItemShopOpen;

public:
    BYTE				m_byFB;					// Authed for Facebook ?
    BYTE				m_byTW;					// Authed for Twitter ?

private:
	EMANI_MAINTYPE		m_eMotionMID;			//! 모션 MID
	EMANI_SUBTYPE		m_eMotionSID;			//! 모션 SID
	FLOAT				m_fMotionSec;			//! 모션 플레이 시간 (초)

	SNATIVEID			m_sGeneralEffItemID;

	std::tr1::shared_ptr<ActorWorldLight>		m_spWorldLight;
	std::tr1::shared_ptr<ActorNameCollision>	m_spNameCollision;
	std::tr1::shared_ptr<ActorNameCollision>	m_spCollisionByObject;

protected:
	// Country
	Country::SCOUNTRY_INFO m_sCountryInfo;

public:
	// Country
	void SetCountryInfo ( const Country::SCOUNTRY_INFO& sCountryInfo ) { m_sCountryInfo = sCountryInfo; }
	const Country::SCOUNTRY_INFO& GetCountryInfo () const { return m_sCountryInfo; }

public:
	BOOL	IsVisibleDisplayName() const;

public:
	virtual WORD GetBodyRadius();

	// Vehicle. 내가 어느 위치에 앉아 있는가~?
	void SetVehiclePassengerID( DWORD dwID );
	DWORD GetVehiclePassengerID() const;

public:
    virtual const DWORD GetCharID() const	override    { return m_CharDbNum; } // m_CharData.m_Base.dwCharID;
	virtual const char* GetName() const	{ return m_strName.c_str(); }
	virtual WORD GetLevel() const		{ return m_CharData.m_Base.wLevel; }
	virtual const GLPartyID& GetPartyID() const	{ return m_CharData.m_Base.PartyID; }
	virtual DWORD GetClubID() const		{ return m_CharData.m_Base.dwGuild; }
	virtual const WORD GetCharSchool() const	{ return m_CharData.m_Base.wSchool; }
	virtual GLDWDATA GetHp() const		{ return m_CharData.m_Base.sHP; }
	virtual const EMCHARINDEX GetCharIndex() const { return m_CHARINDEX; }

	virtual void ReceiveDamage(const int nDamage, DWORD dwDamageFlag, WORD wSchool, const STARGETID& sACTOR) override;
	virtual void 	ReceiveHeal(const int nHeal, DWORD dwHealFlag, WORD wSchool, const STARGETID& sACTOR) override;
	virtual void ReceiveBlocking(const STARGETID& sACTOR) override;
	virtual void ReceiveAVoid( const STARGETID& sACTOR ) override;
	virtual void ReceiveSwing() override;
	virtual bool IsSafeZone() const		{ return m_bSafeZone; }
	virtual bool IsCDMSafeTime() const	{ return m_CharData.m_Base.m_fCDMSafeTime > 0.0f; }
	virtual const BOOL IsFeignDie() const;
	virtual void End_FeignDeath( BOOL bForce );

	virtual void Play_Disguise( int iIdx, BOOL bOn );
	virtual void Stop_Disguise();

	EMCHARCLASS GetClass() { return (EMCHARCLASS)m_CharData.m_Base.emClass; }

	const SITEMCLIENT& GET_SLOT_ITEM( EMSLOT _slot ) { return m_CharData.m_PutOnItems[_slot]; }
	BOOL VALID_SLOT_ITEM( EMSLOT _slot );

	SITEM* GET_SLOT_ITEMDATA( EMSLOT _slot );

	EMELEMENT GET_ITEM_ELMT();

	SNATIVEID	GetActiveSkill() const { return m_idACTIVESKILL; }   
	SSKILLFACT* GET_SKILLFACT(size_t Index)
    { 
        if (Index >=SKILLFACT_SIZE) 
            return NULL; 
        else
            return &m_sSKILLFACT[Index];
    }

	DWORD GETCLUBID()	  { return GetClubID(); }
	EMCROW GETCROW()	  { return GetCrow(); }
	const GLPartyID& GETPARTYID()	  { return GetPartyID(); }
	DWORD GETALLIANCEID() { return m_CharData.m_Base.dwAlliance; }

    void SetClubName(const std::string& ClubName);

public:	
	DxSkinChar* GetSkinChar() { return m_pSkinChar; }

    inline BOOL VehicleUseBoost() const { return m_sVehicle.UseBoost(); }
    inline BOOL VehicleOnceBoost() const { return m_sVehicle.OnceBoost(); }
    inline BOOL* VehicleOnceBoostRef() { return m_sVehicle.OnceBoostRef(); }
    inline void VehicleOnceBoostSet(BOOL bSet) { m_sVehicle.OnceBoostSet(bSet); }

    //! 오토바이 스킨
    DxVehicle* VehicleSkinChar() { return m_sVehicle.SkinChar(); }

    DWORD VehiclePassengerId(size_t Index) const { return m_sVehicle.PassengerId(Index); }
    void  VehiclePassengerIdSet(size_t Index, DWORD GaeaId) { m_sVehicle.PassengerIdSet(Index, GaeaId); }
    
public:
    virtual void FactEffectProcessing ( const EMFACTEFFPROCESSING emFactEffactCase );

public:	
//	EMACTIONTYPE m_Action;
//	virtual BOOL isAction( EMACTIONTYPE emCur )	const override { return m_Action == emCur; }

//	virtual BOOL IsValidBody() const override;
	virtual BOOL IsPartyMem()					{ return m_CharData.m_Base.PartyID.isValidParty(); }
	virtual BOOL IsPartyMaster()				{ return m_CharData.m_Base.dwPMasterID==m_CharData.m_Base.dwGaeaID; }
	virtual bool IsClubMaster()					{ return m_CharData.m_Base.dwGuildMaster==m_CharData.m_Base.dwCharID; }
	virtual BOOL IsAllianceMasterClub()			{ return m_CharData.m_Base.dwGuild == m_CharData.m_Base.dwAlliance ; }
	virtual BOOL IsAllianceMaster()				{ return IsClubMaster() && IsAllianceMasterClub(); }
    virtual const char* GetClubName()			{ return m_ClubName; }
	virtual BOOL IsDominated() const			{ return m_sDOMINATE.emController == EMSPEC_DOMINATE_TYPE_CASTER; }	
	virtual BOOL IsVisibleDetect(void) const	{ return m_bVisible; }
	
	//virtual const char* GetClubName()	{ return m_CharData.m_Base.szClubName; }
	BOOL IsCDCertify()					{ return m_CharData.m_Base.dwFLAGS & SDROP_CHAR_BASE::CLUB_CD; }

//	BOOL IsDie();	

	void UpdateSpecialSkill();
	void UpdateSkillEff();

private:
	const BOOL CalcVisibleDetect(const float fTime);
	const BOOL	CalcVisibleDetectAuto(const float fTime, const float fDistance, const D3DXVECTOR3& vDistance);	

protected:
	virtual float GetMoveVelo();	

public:
	void TurnAction( EMACTIONTYPE toAction );

public:
	const D3DXVECTOR3 &GetMaxOrg()     { return m_vMaxOrg; }
	const D3DXVECTOR3 &GetMinOrg()     { return m_vMinOrg; }
	float GetDirection();

	WORD GETBODYRADIUS()				{ return GLCONST_CHAR::wBODYRADIUS; }
	SDROP_CHAR& GetCharData()			{ return m_CharData; }
	DxSkinChar* GetCharSkin()			{ return m_pSkinChar; }

	virtual float GetBodyHeight()			{ return m_fHeight; }
	virtual D3DXVECTOR3 GetPosBodyHeight();

	void SetUseArmSub( BOOL bSub )		{ m_CharData.m_Base.m_bUseArmSub = bSub; }
	BOOL IsUseArmSub() const			{ return m_CharData.m_Base.m_bUseArmSub; }

	EMSLOT GetCurRHand();
	EMSLOT GetCurLHand();

	BOOL IsCurUseArm( EMSLOT emSlot );

	void SetMotion( EMANI_MAINTYPE eMID, EMANI_SUBTYPE eSID, FLOAT fSec );

public:
	void RESETSKEFF( int i )					{ m_sSKILLFACT[i].sNATIVEID = NATIVEID_NULL(); }
	void DISABLEBLOW( int i )					{ m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE; }

	virtual void UPDATE_DATA_FINISH(const DWORD dwSkillFactIndex) override { UPDATE_DATA_END(dwSkillFactIndex); }
	virtual void	UPDATE_DATA_END(const DWORD dwSkillfactIndex, BOOL bTimeDel = FALSE, BOOL bForceEnd = FALSE);
	void UPDATE_DATA( float fTime, float fElapsedTime, BOOL bClient=FALSE );

	void UPDATE_ITEM( );
	void DISABLEALLLANDEFF();
	void SETENABLEALLLUNCHBOX( const BOOL bEnable );
	void ADDLANDEFF( SLANDEFFECT_VALUE& landEffect );
	void DELLANDEFF( SLANDEFFECT_VALUE& landEffect );

	void DISABLEALLMAPEFF();
	void ADDMAPEFF( SMAP_EFFECT& mapEffect );
	void DELMAPEFF( SMAP_EFFECT& mapEffect );

	// 지형 효과 업데이트
	void UpdateLandEffect();
	void UpdateMapEffect();

	void UpdateGeneralEffect();

public:
	
	virtual float GETATTVELO() override;
	virtual float GETMOVEVELO() override;
	virtual float GETATT_ITEM() override;
	virtual float GETMOVE_ITEM() override;

	void Resurrect(const int nHP, const int nMP, const int nSP, D3DXVECTOR3 vPos);

public:
	HRESULT Create(GLLandManClient* pLandManClient, std::tr1::shared_ptr<SDROP_CHAR> spCharData, LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT UpdateSuit();
	HRESULT UpdateBoardSuit();
	HRESULT UpdateBaseSuit();
	HRESULT UpdateAllSuit();
	HRESULT UpdateSuit_Weapon();

	void ResetPiece( int i );

	HRESULT SetPiece( 
		const TCHAR* szFile,
		LPDIRECT3DDEVICEQ pd3dDevice,
		DWORD dwFlag, 
		int nLevel, 
		EMSCD_FLAGS emFlags,
		BOOL bAttachSub=FALSE,
		EMPIECECHAR* pOutPiecetype=NULL,
		const char* pSelfBodyEffect=NULL );

	HRESULT SetPiece( 
		EMPIECECHAR emPieceChar,
		const std::string& strFileOrigin,
		const std::string& strFileChange,
		LPDIRECT3DDEVICEQ pd3dDevice,
		DWORD dwFlag, 
		int nLevel, 
		EMSCD_FLAGS emFlags,
		BOOL bAttachSub=FALSE,
		EMPIECECHAR* pOutPiecetype=NULL,
		const char* pSelfBodyEffect=NULL );

	void SELECTANI( DxSkinChar* pRenderChar, const TCHAR* szAnim, DWORD dwFlags=NULL);
	void SELECTANI( DxSkinChar* pRenderChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0);
	void DEFAULTANI( DxSkinChar* pRenderChar );
	void TOSTARTTIME( DxSkinChar* pRenderChar );
	void TOENDTIME( DxSkinChar* pRenderChar );
	void SetPartRend( DxSkinChar* pRenderChar, EMPIECECHAR emPIECE, BOOL bRender );
	void SetHairColor( WORD wColor );
	BOOL PutPassiveEffect ( DxSkinChar *pSkinChar, const char* szEffFile, const D3DXVECTOR3 *pDir, float fTarDir=0.0f );
	//void CalculateAmbientColor( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, const D3DXVECTOR3& vPos );
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
	void ClearReserveActionForSkinChar( DxSkinChar *pSkinChar );	// 캐릭터를 안보이게 하거나 보드로 보이게 할경우 호출이 필요하다.

protected:
	void SummonStateVisibleUpdate(void) override {}

private:
	void UpdateCharPos_RenderChar( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE );
	void UpdateCharPos_RenderChar_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE );
	void UpdateCharPos_RenderChar_MaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE );
	void RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharBase* pRenderChar, BOOL bOpaqueState, BOOL bHALF_VISIBLE );
	void RenderMyCharMaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharBase* pRenderChar, BOOL bHALF_VISIBLE );

public:
	const BOOL IsNonTargetable(void) const override;// { return (m_sHALLUCINATE.bNonTargetable == true) || (m_bVisible == FALSE); }

	void RestoreActStateByInvisible(); // 상태 전환 및 스킬 관련 정리와 함께 정리 해야 함;

	void AddSkillFact_SpecialAddon_Client(const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF);

	void RECEIVE_SKILLFACT_Invisible(const float fVAR2);
	const BOOL RECEIVE_SKILLFACT_AIRBORNE(const SKILL::SSPEC& sSPEC, const SKILL::CDATA_LVL& sSKILL_DATA);

	const bool RECEIVE_SKILLFACT_Special(PGLSKILL pSkill, const WORD wlevel, const DWORD dwCount, const STARGETID& sID, SSKILLFACT& sSKILLEF );
	const bool RECEIVE_SKILLFACT_Addon( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF );
	const bool RECEIVE_SKILLFACT_Basic( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF);
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

	// MsgTateBlowBrd; 이대로 사용하기에는 문제가있다;
	void NewBlowEffect(SSTATEBLOW* const pStateBlows);

protected:
	void ReSelectAnimation();
	HRESULT UpdateAnimation( float fTime, float fElapsedTime );

protected:
	void StartAttackProc();
	BOOL AttackProc( float fElapsedTime );
	BOOL AttackVehicleProc( float fElapsedTime );
	void AttackEffect( const SANIMSTRIKE &sStrikeEff );

protected:
	void StartSkillProc();
	BOOL SkillProc( float fElapsedTime );

	// 스킬 시전 시 스킬에 숨겨진 무기가 있을 경우 무기를 세팅하고;
	// 스킬 애니메이션이 끝난 후 지운다;
	// 극강부용으로 쓰임;
	void SetHiddenWeaponPiece();
	void ResetHiddenWeaponPiece();

	void SKT_EFF_HOLDOUT( STARGETID sTarget, DWORD dwDamageFlag );

	void SK_EFF_TARG( const PGLSKILL pSkill, const SANIMSTRIKE &sStrikeEff, const STARGETID &sTarget );
	void SK_EFF_SELFZONE( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget=NULL );
	void SK_EFF_TARZONE( const STARGETID &sTarget, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF );
	void SkillEffect( const SANIMSTRIKE &sStrikeEff );

	void SkillEffectZone( STARGETID& sTarget, const SANIMSTRIKE &sStrikeEff );
	void SkillEffectSpecific(  STARGETID& sTarget, const SANIMSTRIKE &sStrikeEff );
	void SkillEffectUnit( const SANIMSTRIKE &sStrikeEff );
	void SkillEffectTarget( STARGETID& sTarget, const SANIMSTRIKE &sStrikeEff );
public:
	void SkillEffectDealyAction(  const SANIMSTRIKE &sStrikeEff, SNATIVEID sSkillId, SKILLTARGETDATA *sSkillTargetData  );
	void StartDealyAction( const SCHARSKILL& sSkill, std::tr1::shared_ptr<GLCharClient> pOwnerChar, const SKILLTARGETDATA* pTargetData, bool bSubSkill );


public:
		DWORD GET_PK_COLOR();

public:
	BOOL IsCollisionVolume();
	HRESULT FrameMove( const boost::shared_ptr<DxLandMan>& spLandMan, float fTime, float fElapsedTime );	

	HRESULT RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bWorldSystemOLD );
	HRESULT RenderShadow_SpotLight ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bUsedMaterialSystem, DWORD dwSlot );
	HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );
	HRESULT RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice );

    void    DisableDebuffSkillFact();
	void	DisableSkillFact();

    void    EndConfting();

private:
	void CalculateMatrix( const D3DXVECTOR3& vPosition );

public:
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects();

	//////////////////////////////////////////////////////////////////////////
	//						AnimationMove
private:
	float m_fAnimationMoveRotationY;
private:
	void SkinAniControl_AnimationMove();
	//						AnimationMove
	//////////////////////////////////////////////////////////////////////////

public:
	HRESULT MsgMoveState(NET_MSG_GENERIC* nmg);
	HRESULT MsgGoto(NET_MSG_GENERIC* nmg);
	void MsgAnimoveStartBrd(NET_MSG_GENERIC* nmg);

public:
	virtual void MsgProcess(NET_MSG_GENERIC* pMsg);

    void MsgUpdateFlags(NET_MSG_GENERIC* pMsg);
    void MsgPartyBrd(NET_MSG_GENERIC* pMsg);
    void MsgUpdatePassiveBrd(NET_MSG_GENERIC* pMsg);
    void MsgPrivateMarketOpenBrd(NET_MSG_GENERIC* pMsg);
    void MsgPrivateMarketCloseBrd(NET_MSG_GENERIC* pMsg);
    void MsgPrivateMarketItemUpdateBrd(NET_MSG_GENERIC* pMsg);
    void MsgPrivateMarketItemInfoBrd(NET_MSG_GENERIC* pMsg);
    void MsgClubInfoBrd(NET_MSG_GENERIC* pMsg);
    void MsgClubInfoMarkBrd(NET_MSG_GENERIC* pMsg);
    void MsgClubInfoNickBrd(NET_MSG_GENERIC* pMsg);
    void MsgClubDelBrd(NET_MSG_GENERIC* pMsg);
    void MsgClubAuthorityBrd(NET_MSG_GENERIC* pMsg);
    void MsgInvenHairChangeBrd(NET_MSG_GENERIC* pMsg);
    void MsgInvenHairColorChangeBrd(NET_MSG_GENERIC* pMsg);
    void MsgCostumeColorChangeBrd(NET_MSG_GENERIC* pMsg);
    void MsgVehicleColorChangeBrd(NET_MSG_GENERIC* pMsg);
    void MsgLearnBikeBoostBrd(NET_MSG_GENERIC* pMsg);
    void MsgUseBikeBoostBrd(NET_MSG_GENERIC* pMsg);
    void MsgUseBikeBoostCancelBrd(NET_MSG_GENERIC* pMsg);
    void MsgInvenFaceChangeBrd(NET_MSG_GENERIC* pMsg);
    void MsgInvenRenameBrd(NET_MSG_GENERIC* pMsg);
    void MsgReqGestureBrd(NET_MSG_GENERIC* pMsg);
    void MsgQuestionItemFactBrd(NET_MSG_GENERIC* pMsg);
    void MsgQuestionItemFactEndBrd(NET_MSG_GENERIC* pMsg);
    void MsgEventFactBrd(NET_MSG_GENERIC* pMsg);
    void MsgEventFactEndBrd(NET_MSG_GENERIC* pMsg);
    void MsgIPEventBrd(NET_MSG_GENERIC* pMsg);
    void MsgActiveVehicleBrd(NET_MSG_GENERIC* pMsg);
    void MsgActiveWithVehicleBrd(NET_MSG_GENERIC* pMsg);
    void MsgGetVehicleBrd(NET_MSG_GENERIC* pMsg);
    void MsgUngetVehicleBrd(NET_MSG_GENERIC* pMsg);
    void MsgVehicleAccessoryDeleteBrd(NET_MSG_GENERIC* pMsg);
    void MsgVehicleReqSlotExHoldBrd(NET_MSG_GENERIC* pMsg);
    void MsgVehicleRemoveSlotItemBrd(NET_MSG_GENERIC* pMsg);
    void MsgVehicleBrd(NET_MSG_GENERIC* pMsg);
    void MsgReqGatheringBrd(NET_MSG_GENERIC* pMsg);
    void MsgReqGatheringCancelBrd(NET_MSG_GENERIC* pMsg);
    void MsgGatheringResultBrd(NET_MSG_GENERIC* pMsg);
    void MsgUpdateCdmSafetimeBrd(NET_MSG_GENERIC* pMsg);
    void MsgActivityPopupComplate(NET_MSG_GENERIC* pMsg);
    void MsgTitleNotifyChange(NET_MSG_GENERIC* pMsg);
    void MsgUpdateStateBrd(NET_MSG_GENERIC* pMsg);
    void MsgUpdateBrightBrd(NET_MSG_GENERIC* pMsg);
    void MsgConfrontEndToCltBrd(NET_MSG_GENERIC* pMsg);
    void MsgMapEffectUpdateBrd(NET_MSG_GENERIC* pMsg);
    void MsgLandEffectUpdateBrd(NET_MSG_GENERIC* pMsg);
    void MsgPushPullBrd(NET_MSG_GENERIC* pMsg);
    void MsgCureStateBlowBrd(NET_MSG_GENERIC* pMsg);
    void MsgTateBlowBrd(NET_MSG_GENERIC* pMsg);
    void MsgSkillHOldRsBrd(NET_MSG_GENERIC* pMsg);
    void MsgSaveSkillFactBrd(NET_MSG_GENERIC* pMsg);
    void MsgSkillHoldBrd(NET_MSG_GENERIC* pMsg);
    void MsgSkillCancelBrd(NET_MSG_GENERIC* pMsg);
    void MsgReqSkillBrd(NET_MSG_GENERIC* pMsg);
	void MsgDealyActionSkillBrd(NET_MSG_GENERIC* pMsg);
	void MsgEndSkillBrd(NET_MSG_GENERIC* pMsg);
    void MsgLevelUpBrd(NET_MSG_GENERIC* pMsg);
    void MsgPutonChangeBrd(NET_MSG_GENERIC* pMsg);
    void MsgPutonUpdateBrd(NET_MSG_GENERIC* pMsg);
    void MsgPutonReleaseBrd(NET_MSG_GENERIC* pMsg);
    void MsgActionBrd(NET_MSG_GENERIC* pMsg);
    void MsgAttackDamageBrd(NET_MSG_GENERIC* pMsg);
    void MsgAttackAvoidBrd(NET_MSG_GENERIC* pMsg);
    void MsgAttackBlockingBrd(NET_MSG_GENERIC* pMsg);
    void MsgAttackCancelBrd(NET_MSG_GENERIC* pMsg);
    void MsgAttackBrd(NET_MSG_GENERIC* pMsg);
    void MsgJumpPosBrd(NET_MSG_GENERIC* pMsg);
    void MsgItemShopOpenBrd(NET_MSG_GENERIC* pMsg);
    void MsgSkillFactBrd(NET_MSG_GENERIC* pMsg);
	void MsgDrugFb(NET_MSG_GENERIC* pMsg);

	void MsgClubRenameBrd( NET_MSG_GENERIC* pMsg );

	//개인 상점 검색
	void MsgPrivateMarketSearch(NET_MSG_GENERIC* pMsg);

	// 스킬 효과 적용;
	void MsgApplySkillEffect ( NET_MSG_GENERIC* pMsg );
};

typedef GLCharClient* PGLCHARCLIENT;

typedef sc::CGLLIST<PGLCHARCLIENT>	GLCHARCLIENTLIST;
typedef sc::SGLNODE<PGLCHARCLIENT>	GLCHARCLIENTNODE;

#endif // GLCHARCLIENT_H_


