#pragma once

#include "../../EngineLib/NaviMesh/Actor.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../RanLogic/Pet/GLPet.h"
#include "../../RanLogicClient/InterfaceBaseType.h"

#include "../Char/GLCharacter.h"

enum EM_FIELDITEM_STATE
{
	EM_FIELDITEM_STATE_REMOVED     = 0, // 제거됐음
	EM_FIELDITEM_STATE_INSERTOK    = 1, // 인벤에 삽입가능
	EM_FIELDITEM_STATE_INSERTFAIL  = 2, // 인벤에 삽입불가
	EM_FIELDITEM_STATE_GENERALFAIL  = 3, // 오류
};

class GLGaeaClient;

class GLPetClient : public GLPET
{
public:
    GLPetClient(GLGaeaClient* pGaeaClient);
    virtual ~GLPetClient(void);

protected:
    GLGaeaClient* m_pGaeaClient;

    LPDIRECT3DDEVICEQ	m_pd3dDevice;
	Actor				m_actorMove;
	DxSkinChar*			m_pSkinChar;

	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDirOrig;
	D3DXVECTOR3			m_vPos;
	D3DXMATRIX			m_matTrans;

	GLCharacter*		m_pOwner;

	bool				m_bValid;
	bool				m_bReadyActive;			//	펫 활성화 준비중일때

	STARGETID			m_sSkillTARGET;

	DWORD				m_dwActionFlag;
	VEC_SK_TAR			m_vecDroppedItems;

	PETDELAY_MAP		m_SKILLDELAY;

	float				m_fIdleTimer;
	float				m_fAttackTimer;
    float               m_fPickItemTimer;
	float				m_fQBoxLockNotifyTimer;
	float				m_fQboxPickableTimer;
	WORD				m_wAniSub[3];

public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	float				m_fHeight;

	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;


public:
	SNATIVEID			m_sPetCrowID;
	WORD				m_wColorTEMP;
	WORD				m_wStyleTEMP;

	BOOL				m_bEnableSytle;
	BOOL				m_bEnableColor;

	D3DXVECTOR3			m_vRandPos;

public:
	PETACTIVESKILLLIST_VEC m_vecActiveSkillInfo; // 활성화 된 스킬에 대한 정보

private:
	std::tr1::shared_ptr<ActorWorldLight>		m_spWorldLight;
	std::tr1::shared_ptr<ActorNameCollision>	m_spNameCollision;

public:	
	DxSkinChar* GetSkinChar ()							{ return m_pSkinChar; }

	BOOL	IsVisibleDisplayName() const;

	void SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget );

	bool	IsVALID () { return m_bValid; }
	bool	IsReady () { return m_bReadyActive; }
	void	SetReady( bool bActive ) { m_bReadyActive = bActive; }

	D3DXVECTOR3 GetPosition () { return m_vPos; }

	BOOL IsSTATE ( DWORD dwStateFlag )		 			{ return m_dwActionFlag&dwStateFlag; }
	void SetSTATE ( DWORD dwStateFlag )					{ m_dwActionFlag |= dwStateFlag; }
	void ReSetSTATE ( DWORD dwStateFlag )				{ m_dwActionFlag &= ~dwStateFlag; }
	void ReSetAllSTATE ();

	void SetMoveState ( BOOL bRun );
	void SetAttackState( const D3DXVECTOR3& vTargetPos );

	bool IsUsePetSkinPack() { return m_sPetSkinPackState.bUsePetSkinPack; }

	D3DXVECTOR3 GetRandomOwnerTarget ();

	EM_FIELDITEM_STATE	IsInsertableInven ( STARGETID sTar );

	bool	CheckSkill ( SNATIVEID sSkillID, WORD wSlot );
	void	AccountSkill ( SNATIVEID sSkillID );
	void	UpdateSkillDelay ( float fElapsedTime );
	float	GetSkillDelayPercent ( SNATIVEID sSkillID );

	// 현재 활성화된 스킬을 정리한다
	PETACTIVESKILLLIST_VEC& UpdateActiveSkillInfo ();

	// 형상과 정보를 생성한다.
	HRESULT Create ( const PGLPET pPetData, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, boost::shared_ptr<NavigationMesh> spNavi, LPDIRECT3DDEVICEQ pd3dDevice );

	// 정보만 없앤다.
	void	Delete();

	// 형상만 생성한다.
	HRESULT SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice );

	// 형상만 없앤다.
	void	DeleteSkin();

	// 맵을 이동하는 것이니 맵 참조값을 줄이는 방향으로 간다.
	void LandShredPtrRefSub();

	HRESULT UpdateAnimation ( float fTime, float fElapsedTime );
	HRESULT	UpdatePetState ( float fTime, float fElapsedTime );
	HRESULT UpdateSuit ( BOOL bChangeStyle );

	// Default FrameWork
	HRESULT FrameMove( GLLandManClient* pLandManClient, float fTime, float fElapsedTime );
	HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWorldSystemOLD );
	HRESULT RenderShadow_SpotLight ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem, DWORD dwSlot );
	HRESULT RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice, float fWaterHeight );

private:
	void RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, BOOL bOpaqueState, BOOL bHALF_VISIBLE );

public:
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();
	
public:
	//const char* GetName() const { return m_szName; }	//GLPet
	//inline int Satiation() const { return m_nFull; }  //GLPet
	lua_tinker::table GetPetCardID();
	lua_tinker::table GetPetItemList();
	int GetType();
	std::string GetTypeString();
	const WORD  GetMaxSkillSlot() const;
	lua_tinker::table GetPetSkillList( int nIndex );
	lua_tinker::table GetPetSkillListAll();
	void SetActivatedSkill( lua_tinker::table tbActivatedSkill );

	WORD ColorDWORDtoWORD( DWORD dwColor );

	DWORD GetColor();
	void SetColor( DWORD dwColor );
	void CommitColor( DWORD dwColor );

	void MsgProcess(NET_MSG_GENERIC* nmg);

protected:
	void MakeAniSubKey ( float fTime );

private:
	bool IsActiveSkill(SKILL::EMTYPES emSkillType);

public:
	bool IsQBoxLockMessageNotifyTime(void);
	bool IsQBoxPickableTime(void);

public:
	void	ReqGoto ( D3DXVECTOR3 vTarPos, bool bToPickUp );
	void	ReqStop ( bool bStopAttack = false );
	void	ReqUpdateVelocity ();
	void	ReqUpdateState ();
	void	ReqUpdateSkillState ();
	void	ReqGiveFood ( WORD wPosX, WORD wPosY );
	void	ReqInputName ( WORD wPosX, WORD wPosY );
	void	ReqRename ( const char* szCharName );
	void	ReqInputColor ( WORD wPosX, WORD wPosY );
	void	ReqChangeColor ( WORD wColor );
	void	ReqInputStyle ( WORD wPosX, WORD wPosY );
	void	ReqChangeStyle ( WORD wStyle );
	void	ReqChangeActiveSkill ( SNATIVEID sSkillID, WORD wSlot );
	void	ReqChangeAccessory ( EMSUIT emSUIT );
	void	ReqLearnSkill ( WORD wPosX, WORD wPosY );
	void    ReqLearnSkill ( DWORD dwChannel, WORD wPosX, WORD wPosY );
	void	ReqRemoveSlotItem ( EMSUIT emType );
	void    ReqLearnDualSkill ( WORD wPosX, WORD wPosY );
	void	ReqGetItem (  DWORD dwGetItemFlag );


	void	MsgGetRightOfItem(NET_MSG_GENERIC* nmg);
	void	MsgUpdateFull(NET_MSG_GENERIC* nmg);
	void	MsgRename(NET_MSG_GENERIC* nmg);
	void	MsgChangeColor(NET_MSG_GENERIC* nmg);
	void	MsgChangeStyle(NET_MSG_GENERIC* nmg);
	void	MsgSlotExHold(NET_MSG_GENERIC* nmg);
	void	MsgHoldToSlot(NET_MSG_GENERIC* nmg);
	void	MsgSlotToHold(NET_MSG_GENERIC* nmg);
	void	MsgSkillChange(NET_MSG_GENERIC* nmg);
	void	MsgLearnSkill(NET_MSG_GENERIC* nmg);
	void	MsgRemoveSlotItem(NET_MSG_GENERIC* nmg);
	void	MsgAttack(NET_MSG_GENERIC* nmg);
	void	MsgSad(NET_MSG_GENERIC* nmg);
	void	MsgAccessoryDelete(NET_MSG_GENERIC* nmg);
	void	MsgRemoveSkillFact(NET_MSG_GENERIC* nmg);
	void	MsgSkillSlotExpansion(NET_MSG_GENERIC* nmg);
	void	MsgJumpPos(NET_MSG_GENERIC* nmg);

	// Skill Functions
	void	ProceedSkill ();
	void	StartSkillProc ();

	// 스타일 & 컬러
	void StyleInitData();
	void StyleChange( WORD wStyle );
	void StyleEnd()						{ m_bEnableSytle = FALSE; }
	void ColorInitData();
	void ColorChange( WORD wColor );
	void ColorEnd()						{ m_bEnableColor = FALSE; }
	void StyleUpdate();
	void ColorUpdate();

    // Callback
    void CallbackPetColorCreate    ( UI::SPANNELGROUPINFOARRAY&        OutPannelGroupInfoArray );
    void CallbackPetColorInit      ( UI::SPANNELGROUPINFOARRAY&        OutPannelGroupInfoArray );
    void CallbackPetColorUpdate    ( const UI::SPANNELGROUPINFOARRAY&  InPannelGroupInfoArray  );
    void CallbackPetColorCommit    ( const UI::SPANNELGROUPINFOARRAY&  InPannelGroupInfoArray  );

};

typedef GLPetClient* PGLPETCLIENT;


