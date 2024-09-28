#ifndef _GL_CHARACTER_H_
#define _GL_CHARACTER_H_

#pragma once

#include "../../SigmaCore/String/MinGuid.h"

#include "../../EngineLib/NaviMesh/Actor.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"

#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLogicEx.h"
#include "../../RanLogic/NpcTalk/NpcTalkControl.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../RanLogic/Market/GLPrivateMarket.h"
#include "../../RanLogic/GLChatLinkDef.h"
#include "../../RanLogic/Activity/TitleManager.h"
#include "../../RanLogic/FSM/GLCharStateDefine.h"
#include "../../RanLogic/Skill/GLSkillEffectAction.h"

#include "../InterfaceBaseType.h"
#include "../Trade/GLTradeClient.h"
#include "../Reservation/GLReservationAction.h"
#include "../ClientActor.h"
#include "./GLWorkingSkill.h"

#include "../Widget/GLWidgetScript.h"

using namespace NSSKILL_EFFSET;

class GLClubClient;
struct SKILLID;

enum EMFIND_TAR
{
	ENFIND_TAR_NULL		= 0,
	EMFIND_TAR_ANY		= 1,
	EMFIND_TAR_ENEMY	= 2,
	EMFIND_TAR_OUR		= 3,
	EMFIND_TAR_OUR_DIE	= 4,
	EMFIND_TAR_ANY_DIE	= 5,
	ENFIND_TAR_LAND		= 6,
};

enum EMFIND_OPTION
{
	EMFIND_OPTION_NULL		= 0x0000,
	EMFIND_OPTION_ONLYITEM	= 0x0001,
	EMFIND_OPTION_P2PMENU	= 0x0002,
};

enum DoActWait_TYPE
{
	DoActWait_Zero,
	DoActWait_Create,
	DoActWait_MoveActiveMap,
	DoActWait_MsgMoveActiveMap,
	DoActWait_MsgResurrect,
	DoActWait_ReLvMustLeaveFB,
	DoActWait_ReCallFB,
	DoActWait_TeleportFB,
};


enum ESlotHold
{
	Release = 0,
	Hold,
};

struct SACTIONTAR
{
	EMACTIONTAR		emACTAR;
	STARGETID		sTARID;

	SACTIONTAR() :
		emACTAR(EMACTAR_NULL)
	{
	}

	const bool operator==( SACTIONTAR& rhs ) const
	{
		if ( emACTAR != rhs.emACTAR )
		{
			return (sTARID == rhs.sTARID);
		}

		return true;
	}

	void RESET()
	{
		emACTAR = EMACTAR_NULL;
		sTARID.RESET();
	}
};

class GLCharClient;
class GLGaeaClient;
struct GLREACTION;
struct ActorWorldLight;
class GLWorkingSkill;
class GLTransformSet;
class SkinCharMulti;

typedef std::vector<USER_ATTEND_INFO>		VECATTEND;

typedef std::map< DWORD, DWORD >	MAPMARKETCLICK;
typedef MAPMARKETCLICK::iterator	MAPMARKETCLICK_ITER;

typedef std::map< DWORD, SENTRYLIMITINFO > MAPENTRYINFO;
typedef MAPENTRYINFO::iterator			   MAPENTRYINFO_ITER;

typedef std::vector<CString> MAPNAMEVECTOR;
typedef MAPNAMEVECTOR::const_iterator MAPNAMEVECTORCITER;
typedef MAPNAMEVECTOR::iterator MAPNAMEVECTORITER;

typedef std::vector< GLSKILL* >		VEC_GLSKILL;
typedef VEC_GLSKILL::iterator		VEC_GLSKILL_ITER;

//typedef SKILL_MAP::iterator					SKILL_MAP_ITER;
//typedef SKILL_MAP::const_iterator			SKILL_MAP_CITER;

class GLCharacter : public GLCHARLOGIC, public ClientActor
{
public:
    GLCharacter(GLGaeaClient* pGaeaClient, GLClubClient* pMyClub);
    virtual ~GLCharacter();

	static TARDISTMAP	ms_vecTAR_TEMP;

protected:
	const static float	m_fELAPS_MOVE;
    const static float  fLIMIT_TIME_ITEM_SORT;
    const static float	fMAX_ITEM_SORT_MSG_DELAY_TIME;
	
	enum { EMWAIT_COUNT = 10 };

protected:
    int FIRST_PRIORITY_ITEMSORT[ ITEM_NSIZE ];
    int SECOND_PRIORITY_ITEMSORT[ EMITEM_LEVEL_NSIZE ];
    int ADDTIONAL_PRIORITY_ITEMSORT[ SUIT_NSIZE ];

	SNATIVEID	m_sChangeQUICK[EMSKILLQUICK_SIZE];

	typedef std::vector<WORD>		vSortQuick;
	typedef vSortQuick::iterator	vSortQuick_iter;

	vSortQuick					   m_vQuickNum;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	DxSkinChar*	m_pSkinChar;
	SkinCharMulti* m_pSkinCharMulti;
	GLTransformSet*	m_pTransformSet; 
	GLWorkingSkill* m_pWorkingSkill; 

	CSkillSThrow	m_SkillSTrow;
	CSkillCameraHit m_SkillCHit;
	
	D3DXMATRIX			m_matBikeTrans;

	D3DXVECTOR3			m_vServerPos;

    EMSERVICE_PROVIDER m_ServiceProvider;

public:
	SUMMONIDLIST            m_dwSummonGUIDList; 

protected:
	EMACTIONTYPE		m_Action;				
	
	STARGETID			m_sActionID;
	STARGETID			m_sSelectID;
	STARGETID			m_sCursorID;
	
	SKILLTARGETDATA		m_sSkillTargetData;

	TARIDRC_SET			m_setTAR_SELECTED;			

	SNATIVEID			m_sRunSkill;				
	SNATIVEID			m_sActiveSkill;				
	SKILLID				m_sScopeSkill;				

	DWORD				m_dwANISUBCOUNT;
	DWORD				m_dwANISUBSELECT;

	DWORD				m_dwANISUBGESTURE;

	EMITEM_QUESTION		m_emOldQuestionType;	

	float				m_fQUEST_TIMER;				

protected:
	float				m_fLastMsgMoveSend;
	GLMSG::SNETPC_GOTO	m_sLastMsgMove;
	GLMSG::SNETPC_GOTO	m_sLastMsgMoveSend;

	GLReservationAction	m_ReservationAction;

	bool				m_bMiniMapMove;			// 미니맵을 통해서 이동 중이다
	float				m_fMiniMapMoveTime;

protected:
	float				m_fIdleTime;
	float				m_fattTIMER;
	int					m_nattSTEP;

	DWORD				m_dwWAIT;

protected:
	DWORD				m_dwNumStorageItem[EMSTORAGE_CHANNEL];	//	창고에 들어있는 아이템 갯수.
	SCONFTING_CLT		m_sCONFTING;							//	진행 대련 정보.

	
	DWORD				m_StorageSpecificItemCacheCount;
	std::vector<DWORD>  m_StorageSpecificItemCacheVec;
public:
	WORD				m_wPMPosX;								
	WORD				m_wPMPosY;
	GLPrivateMarket		m_sPMarket;								

	MAPMARKETCLICK		m_mapMarketClick;						

	BOOL				m_bCLUB_CERTIFY;						
	float				m_fCLUB_CERTIFY_TIMER;					
    bool				m_bForcedPKMode;						

protected:
	GLClubClient*		m_pMyClub;								

public:
	WORD				m_wInvenPosX1;				// Temp
	WORD				m_wInvenPosY1;
	WORD				m_wInvenPosX2;				// Temp
	WORD				m_wInvenPosY2;

	
	WORDWORD			m_wwInvenPos_FaceStyle;		// Temp
	WORDWORD			m_wwInvenPos_HairStyle;		// Temp
	WORDWORD			m_wwInvenPos_HairColor;		// Temp

	SITEMCUSTOM			m_sTempItemCustom;

	MAPENTRYINFO		m_mapEntryLimitInfo;

	
private:
	//BOOL			m_bEnableHairSytle;
	//BOOL			m_bEnableHairColor;
	//BOOL			m_bEnableFaceStyle;
	//WORD			m_wHairColorTEMP;
	//WORD			m_wHairStyleTEMP;
	//WORD			m_wFaceStyleTEMP;

	BOOL			m_bAttackable;							
	BOOL			m_bMiniGameOpen;

	float			m_fPartyRecruitTime;

    float           m_fCheckActivityItem;       

private:
	EMSLOT			m_emCostumSelectSlot;		

    float           m_fLimitTimeItemSort;
    float           m_fItemSortMsgDelayTime;
    float           m_bSendItemSortMsg;

    CHARACTER_POINT m_PointShopPoint;			//! PointShop Point

    RebuyItem::REBUY_VEC m_vecRebuy;

	EMANI_MAINTYPE		m_eMotionMID;			
	EMANI_SUBTYPE		m_eMotionSID;			
	FLOAT				m_fMotionSec;			

    CharState           m_bMovement;
	SNATIVEID			m_sGeneralEffItemID;

	
	int					m_nOpenCharSlot;

public:
    unsignedState       m_sResurrectAbleState;     

public:
    TitleManager    m_TitleManager;
    LONGLONG        m_llActivityPoint[ACTIVITY_CLASS_SIZE];

protected:
	// Country
	Country::SCOUNTRY_INFO	m_sCountryInfo;

public:
	// Country;
	void SetCountryInfo ( const Country::SCOUNTRY_INFO& sCountryInfo ) { m_sCountryInfo = sCountryInfo; }
	const Country::SCOUNTRY_INFO& GetCountryInfo () const { return m_sCountryInfo; }

private:
	std::tr1::shared_ptr<ActorWorldLight>		m_spWorldLight;

	// RequestLandIn Call this function after receive view around message already. 버그 찾기 위한 Data
	DoActWait_TYPE								m_emDoActWaitTypePrev;
	DoActWait_TYPE								m_emDoActWaitType;

private:
    void SetPriorityItemSort();

    void ResetLimitTimeItemSort();
    void SetEnableSendItemSortMsg();
    void SetDisableSendItemSortMsg();

public:
	std::tr1::shared_ptr<ActorWorldLight>	GetWorldLightForTool()	{ return m_spWorldLight; }

	inline SKILLTARGETDATA&	GetTargetData()			{ return m_sSkillTargetData; }
	BOOL					TryToUseSkill();
	ClientActor*			GetCrowOnLand( const EMCROW emCrow, const DWORD dwGaeaId );

	EMSLOT GetCostumSelectSlot() const { return m_emCostumSelectSlot; }
    bool IsEnalbeItemSort();

    //! PointShop Point
    inline void PointShopPointSet(const CHARACTER_POINT& Point) { m_PointShopPoint = Point; }
    inline __int64 PointShopPoint() const { return m_PointShopPoint.Total(); }

    //! 재매입
    const RebuyItem::REBUY_VEC& RebuyList() const;

    unsigned int GetCountItemInInven( EMITEM_TYPE emType );
	unsigned int GetCountItemInInven( EMITEM_TYPE emType, ITEM_VALUE typeValue, int _value );
    void GetItemPosInInven( EMITEM_TYPE emType, WORD& wOutPosX, WORD& wOutPosY );
	void GetItemPosInInven( EMITEM_TYPE emType, ITEM_VALUE typeValue, int _value, WORD& wOutPosX, WORD& wOutPosY );

	LuaTable GetItemPosInInvenByTable( DWORD dwType );
	LuaTable FindInvenItemPos( WORD wMID, WORD wSID, int nCountIndex );
	bool IsExistReturnSchoolCard(void);

public:	
	void SetServerPos( const D3DXVECTOR3& vPos ) { m_vServerPos = vPos; }

	const SNATIVEID& GetChangeQuick( int iNum );

public:
	BOOL IsOpenMiniGame() const { return m_bMiniGameOpen; }
	VOID OpenMiniGame()			{ m_bMiniGameOpen = TRUE; }
	VOID CloseMiniGame()		{ m_bMiniGameOpen = FALSE; }

public:
    bool IsOpenNPCMarket ();

public:
	//void HairStyleInitData();
	void HairStyleChange( WORD wStyle );
	//void HairStyleEnd()	{ m_bEnableHairSytle = FALSE; }
	
	//void HairColorInitData();
	void HairColorChange( WORD wColor );
	//void HairColorEnd()	{ m_bEnableHairColor = FALSE; }

	//void FaceStyleInitData();
	void FaceStyleChange( WORD wStyle );
	//void FaceStyleEnd()	{ m_bEnableFaceStyle = FALSE; }

private:
	void HairStyleUpdate( WORD wStyle );
	void HairColorUpdate( WORD wColor );
	void FaceStyleUpdate( WORD wStyle );

public:
	BOOL IsMarketClick( DWORD dwGaeaID );
	void InsertMarketClick( DWORD dwGaeaID );

public:
	//! AABB
	//
	D3DXVECTOR3		m_vMaxOrg;
	D3DXVECTOR3		m_vMinOrg;

public:
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
	float			m_fHeight;

protected:
	bool  m_bAwayGesture;

public:
	DWORD m_NpcLastClick; //! 가장 마지막에 클릭한 NPC ID
	float m_fKeyDownCheckTime;
	DWORD m_nAwayGestureLevel;

public:
	// PET
	//LONGLONG  m_llPetCardGenNum;	//! 활성화된 팻카드의 GenNum
	sc::MGUID m_llPetCardGenNum;
	SNATIVEID m_sPetCardNativeID;   //! 활성화된 팻카드의 Nativeid
	BYTE      m_cPetCardGenType;	//! 활성화된 팻카드의 GenType

	PETCARDINFO_MAP   m_mapPETCardInfo;     //! 팻카드 정보(팻의 상태가 변경될때마다 같이 변경해줘야함)
	PETCARDINFO_MAP   m_mapPETCardInfoTemp; //! 팻카드 정보(거래시 사용)
	PETREVIVEINFO_MAP m_mapPETReviveInfo;   //! 부활시키기 위한 팻의 정보

	BOOL m_bReceivedPetCardInfo; // 팻카드 정보 수신여부
	BOOL m_bIsPetActive;		 // 맵을 이동할때 팻이 활동중이었는지 여부

public:
	VEHICLEITEMINFO_MAP			m_mapVEHICLEItemInfo;					// 탈것 아이템의 정보( 탈것 상태 변경 )
	VEHICLEITEMINFO_MAP			m_mapVEHICLEItemInfoTemp;				// 탈것 아이템의 정보( 거래시 사용 )

protected:
	GLVEHICLE		m_sVehicle;				//! 탈것 구조체
	DWORD			m_dwVehiclePassengerID;	// Vehicle 내가 어느위치에 앉아 있는가~?
											// 난 다른 Vehicle 에 탈 수도 있으므로 GLVEHICLE 안에 들어가면 안된다.

public:
	BOOL			m_bReqSwapArms;			// 극강부 무기 교체 중이라면

	BOOL			m_bIsVehicleActive;		// 맵 이동중 탈것 타고 있었다면...
	BOOL			m_bRecivedVehicleItemInfo;  // 탈것 정보 수신 여부

	bool			m_bOldVisibleTracingUI[8];
	bool			m_bNewVisibleTracingUI[8];

	float			m_fPrintProcessInfoMsgGap;

	bool			m_bGarbageOpen;
	SINVEN_POS		m_sGarbageItem;			// 휴지통에 등록된 아이템

	bool			m_bItemShopOpen;		// ItemShopOpen( Japan ) 
	
	bool			m_bReqAttendList;		// 출석 리스트
	VECATTEND		m_vecAttend;			// 출석 리스트
	DWORD			m_dwComboAttend;		// 연속 출석 일수
	__time64_t		m_tAttendLogin;			// 로그인시간( 출석부에만 사용 ) 
	DWORD			m_dwAttendTime;			// 출석 가능한 게임 접속 시간

	CString			m_strRecordChat;
    bool			m_bRecordChat;
    //bool            m_bUseRemodelCard;
    bool            m_bUseStorageCard;
	CTime			m_recordStartTime;

	SINVEN_POS		m_sProductMaterialPos[ITEMMIX_ITEMNUM];

	bool			m_bCostumeStats;

private:
	EMTYPE_ITEM_REMODEL	m_emTypeItemRemodelState;

	float			m_fMacro_RecordUINotifyTime;// UI에 알린시간 기록, 분마다 갱신한다;

// 대련 신청에 쓰이는 변수들
private:
	DWORD			m_PtoPTargetID;
	DWORD			m_ConflictReqID; // = pNetMsg->dwID;
	EMCONFT_TYPE	m_emConflictReqType;	// = pNetMsg->emTYPE;
	SCONFT_OPTION	m_sReqConflictOption;	// = pNetMsg->sOption;

public:
    //! -----------------------------------------------------------------------
    //! 탈것

	// Vehicle. 내가 어느 위치에 앉아 있는가~?
	void SetVehiclePassengerID( DWORD dwID );
	DWORD GetVehiclePassengerID() const;

    VEHICLE_TYPE VehicleType() const { return m_sVehicle.Type(); }

    //! 소환을 요청한 탈것 아이템의 Mid/Sid
    SNATIVEID VehicleId() const { return m_sVehicle.Id(); }

    void VehicleActiveValueSet(bool bSet) { m_sVehicle.SetActiveValue(bSet); }
	bool VehicleActiveValue() const { return TRUE == m_sVehicle.IsActiveValue() ? true : false; }
    void VehicleReset() { m_sVehicle.RESET(); }

    inline BOOL VehicleUseBoost() const { return m_sVehicle.UseBoost(); }    
    
    inline BOOL VehicleOnceBoost() const { return m_sVehicle.OnceBoost(); }    
    inline BOOL* VehicleOnceBoostRef() { return m_sVehicle.OnceBoostRef(); }
    inline void VehicleOnceBoostSet(BOOL bSet) { m_sVehicle.OnceBoostSet(bSet); }
    
    DxVehicle* VehicleSkinChar() { return m_sVehicle.SkinChar(); }

    DWORD VehiclePassenger(size_t Index) const { return m_sVehicle.PassengerId(Index); }
    void  VehiclePassengerSet(size_t Index, DWORD GaeaId);

    void VehiclePutOnItemSet(VEHICLE_ACCESSORYTYPE Type, SNATIVEID ItemId) 
    { 
        m_sVehicle.PutOnItemSet(Type, ItemId);
    }

    SNATIVEID VehiclePutOnItem(VEHICLE_ACCESSORYTYPE Type) const
    {
        return m_sVehicle.PutOnItemId(Type);
    }

    const SITEMCUSTOM& VehiclePutOnItemRef(VEHICLE_ACCESSORYTYPE Type) const
    {
        return m_sVehicle.PutOnItemRef(Type);
    }

    BOOL VehicleCheckSlotItem(SNATIVEID sNativeID, EMSUIT emSUIT)
    {
        return m_sVehicle.CheckSlotItem(sNativeID, emSUIT);
    }
    
    int VehicleFull() const { return m_sVehicle.Full(); }

    float VehiclePutOnItemMoveSpeedTotal() const { return m_sVehicle.PutOnItemMoveSpeedTotal(); }

    DWORD VehicleBoosterId() const { return m_sVehicle.BoosterId(); }

    float VehicleBoostTime() const { return m_sVehicle.BoostTime(); }

    BOOL VehicleBoostUseDelay() const { return m_sVehicle.BoostUseDelay(); }
    float VehicleBoostDelay() const { return m_sVehicle.BoostDelay(); }

    DWORD VehicleColorMain(VEHICLE_ACCESSORYTYPE Type) const { return m_sVehicle.ColorMain(Type); }
    DWORD VehicleColorSub(VEHICLE_ACCESSORYTYPE Type) const { return m_sVehicle.ColorSub(Type); }

    int VehicleePassengerNum() const { return m_sVehicle.GetPassengerNum(); }    

	LuaTable GetPutOnVehicleInfo();
	float GetVehicleBoosterGauge();

	void UpdateSpecialSkill( float fElapsedTime );
	void UpdateLunchBox   ( float fElapsedTime );
	void StartDealyAction( const SCHARSKILL& sSkill, GLCharacter* pOwnerChar, const SKILLTARGETDATA* pTargetData, bool bSubSkill );
	void UpdateSkillEff(float fTime, float fElapsedTime );

public:
	BOOL 			ISCONFRONT_TAR( const STARGETID &sTargetID );
    BOOL 			ISCONFRONT_ALLIANCE(const DWORD dwGaeaID);
	void 			DELCONFRONT_MEMBER( DWORD dwGaeaID );

public:
	bool 			IsSchoolConfting();
	UINT 			GetConftMemberCount();

public:
	const bool		IsEmptyReservationAction() const { return m_ReservationAction.IsEmpty(); }	
	const bool		IsExistReservationAction(EMREACTION emFind);
	void			ClearReservationAction(void);
	void 			ResetReservationAction(bool bForced=false);
	void 			ResetSelectID();
	void 			ResetCursorID() { m_sCursorID.RESET(); }
	void			ResetActionID() { m_sActionID.RESET(); }
	void 			SetSelectID( const STARGETID& sTarget, const bool bResetSelected=true );
	void 			SetCursorID( const STARGETID& sTarget ) { m_sCursorID = sTarget; }
	STARGETID& 		GetSelectID() { return m_sSelectID; }
	STARGETID& 		GetActionID() { return m_sActionID; }
	STARGETID& 		GetCursorID() { return m_sCursorID; }	

	BOOL			SelectForcedTarget();

/**********************대련신청 UI용 함수********************/
public:
	void				SetAnsData(GLMSG::SNETPC_REQ_CONFRONT_TAR *_pNetMsg)
	{
		m_ConflictReqID = _pNetMsg->dwID;
		m_PtoPTargetID = _pNetMsg->dwID;
		m_emConflictReqType = _pNetMsg->emTYPE;
		m_sReqConflictOption = _pNetMsg->sOption;
	}
	void				SetReqConftID_ans(DWORD _ID)
	{
		m_ConflictReqID = _ID;
	}
	void				SetConftType_ans(EMCONFT_TYPE _Type)
	{
		m_emConflictReqType = _Type;
	}
	void				SetConftOption_ans(SCONFT_OPTION _Option)
	{
		m_sReqConflictOption = _Option;
	}

	DWORD			GetTarPtoP() { return m_PtoPTargetID; }
	std::string			GetConftMessage();
	LuaTable	GetHPRevoverNum();
	LuaTable	GetHPRate_Mine();
	LuaTable	GetHPRate_Target();	
	void				RequestConflict(LuaTable _lua_table); //상대방의 가이아ID를 입력
	void				AnswerConflict(bool _Ans);

	bool				HaveClubJoinAutority();
	bool				HaveClubAllianceAuthority();
	bool				HaveClubBattleAuthority();
	bool				HaveClubStorageAuthority();
	bool				IsAllianceMaster();
	bool				IsRideAVehicleNow();

	float GetCriticalRate() { return GETCRITICAL_TOTAL(); }
	float GetCrushingBlow() { return m_sSUMITEM.fIncR_CrushingBlow; }
	float GetMoveSpeed() { return GETMOVE_TOTAL(); }
	float GetAtkSpeed() { return GETATT_TOTAL(); }
	float GetExpRate() { return GETEXPVELO(); }
	float GetIncrHpTotal() { return m_fVariationRateHP; }
	float GetIncrMpTotal() { return m_fVariationRateMP; }
	float GetIncrSpTotal() { return m_fVariationRateSP; }
	int	GetReceiveCP() { return m_sSUMITEM.nReceive_CP_Inc; }
	int	GetPotionHpInc() { return m_sSUMITEM.nPotion_HP_Inc + m_nPotion_HP_Inc_SKILL; }
	int	GetPotionMpInc() { return m_sSUMITEM.nPotion_MP_Inc + m_nPotion_MP_Inc_SKILL; }
	int	GetPotionSpInc(){ return m_sSUMITEM.nPotion_SP_Inc + m_nPotion_SP_Inc_SKILL; }

	float	GetIncR_Ignore_BossDamage()			{ return m_sSUMITEM.fIncR_Ignore_BossDamage; }
	INT		GetInc_BossFixedDamage()			{ return m_sSUMITEM.nInc_BossFixedDamage; }
	float	GetIncR_Critical_EmergencyMob()		{ return m_sSUMITEM.fIncR_Critical_EmergencyMob; }
	float	GetIncR_Critical_EmergencyBoss()	{ return m_sSUMITEM.fIncR_Critical_EmergencyBoss; }
	float	GetDec_Damage_EmergencyState()		{ return m_sSUMITEM.fDecR_Damage_EmergencyState; }
	float	GetIncR_ExpAddEffect()			{ return m_sSUMITEM.fIncR_ExpAddEffect; }
	float	GetIncR_DropGenMoney()			{ return m_sSUMITEM.fIncR_DropGenMoney; }
	float	GetDecR_Damage_MeleeSkill()		{ return m_sSUMITEM.fDecR_Damage_MeleeSkill; }
	float	GetDecR_Damage_RangeSkill()		{ return m_sSUMITEM.fDecR_Damage_RangeSkill; }
	float	GetDecR_Damage_MagicSkill()		{ return m_sSUMITEM.fDecR_Damage_MagicSkill; }
	INT		GetInc_HP_SoloPlay()			{ return m_sSUMITEM.nInc_HP_SoloPlay; }
	INT		GetInc_HP_PartyPlay()			{ return m_sSUMITEM.nInc_HP_PartyPlay; }
	float	GetIncR_AtkSpeed_SoloPlay ()	{ return m_sSUMITEM.fIncR_AtkSpeed_SoloPlay ; }
	float	GetIncR_AtkSpeed_PartyPlay()	{ return m_sSUMITEM.fIncR_AtkSpeed_PartyPlay; }
	float	GetInc_SkillDurationTAdd(WORD wGrade)	{ return GLCHARLOGIC::GetInc_SkillDurationTAdd(wGrade); }
	float	GetIncR_SkillDamageAdd(WORD wGrade)	{ return GLCHARLOGIC::GetIncR_SkillDamageAdd(wGrade); }
	float	GetDecR_SkillDelayT(WORD wGrade)	{ return GLCHARLOGIC::GetDecR_SkillDelayT(wGrade); }
	float	GetIncR_CriticalDamage() { return GLCHARLOGIC::GetIncR_CriticalDamage(); }

	// 캐릭터 정보 디테일 가산 옵션 정보를 묶어줌;
	LuaTable GetCharInfoDetailAddonInfo();
	LuaTable GetCharInfoDetailAddonLimitInfo();
	LuaTable GetCharTempPuonInfoDetailAddonInfo(WORD wPosX, WORD wPosY, WORD wVirtualPow, WORD wVirtualDex,
												WORD wVirtualSpi, WORD wVirtualStr, WORD wVirtualSta);

public:
	bool 			IsReqSwapArms() 			{ return m_bReqSwapArms ? true : false; }
	bool 			IsMoveAble();
	
    // EMIMPACT_SIDE에 따른 행동을 할수 있는지 검사;
    const BOOL      IsReActionable( const STARGETID& _sTargetID, const EMIMPACT_SIDE emTargetSide);
    const BOOL      IsReActionable( const STARGETID& _sTargetID, const EMIMPACT_SIDE emTargetSide, const bool _bForceAction );
	const BOOL		IsReActionableOver( const STARGETID& _sTargetID, const EMIMPACT_SIDE emTargetSide, const bool _bForceAction );

	BOOL			IsSelectTargetLock();

private:
	BOOL			IsReActionableEnemyCrowPC( const ClientActor* pTARGET, const bool bEnableForcedPkMode = true );
	BOOL			IsReActionableEnemy( const ClientActor* pTARGET );
	BOOL			IsReActionableNoEnemyCrowPC( const ClientActor* pTARGET );
	BOOL			IsReActionableNoEnemy( const ClientActor* pTARGET );
    const BOOL      IsReActionableNonFaction( const STARGETID &sTargetID, const EMIMPACT_SIDE emTargetSide, const bool _bForceAction );    

public:
	SDOMINATE&	GetDominate()	{ return m_sDOMINATE; }

public:
	bool IsProducting();
	BOOL IsCtrlBlockBody();
	BOOL IsDie();

	const bool IsIdleCharacter();

    /**
        움직임(스킬포함) 가능 유무를 설정 한다.        

        이것은 서버기반으로 작동 되므로 클라에서는 값을 조작할 수 없도록 고안되었다.
        만약 움직임을 막고 싶다면 서버에 움직임을 막도록 요청하여 서버에 의해 처리 되어야 한다.
        클라 자체적으로 막고 싶다면(될 수 있으면 하지 않아야 한다 서버와 동기화가 안 맞으므로)
        SetLockMovement 를 사용 하도록 한다.

     */
    void SetMovement(const CharState state);
    const CharState IsMovement() const { return m_bMovement; } 

	BOOL IsRunMode() { return IsActState(EM_ACT_RUN); };

	BOOL IsActionLimit( DWORD dwLimit ) { return m_dwActionLimit&dwLimit; } 

	virtual const GLPartyID& GetPartyID(void) const;
	virtual BOOL IsPartyMaster();
	virtual const BOOL IsFeignDie() const;

	virtual BOOL IsPartyMem();

	virtual bool IsClubMaster();
	virtual bool IsHaveAllianceDisAuthority();
	virtual BOOL IsCDCertify();

	std::string GetClubName();

    bool IsAwayGesturing()  { return m_bAwayGesture; }
    void SetAwayGesture( bool _bAwayGesture ) { m_bAwayGesture = _bAwayGesture; }

public:
	void		BeginAction( EMACTIONTYPE emAction );
	void		EndAction( EMACTIONTYPE emAction );
	void		TurnAction( EMACTIONTYPE toAction );
	void		ResetAction();

public:
	virtual float GetMoveVelo();
	virtual void SetLockMovement(const bool bLock) override;
	BOOL ActionMoveTo( float fTime, D3DXVECTOR3 vFromPt, D3DXVECTOR3 vTargetPt, BOOL bContinue=FALSE, BOOL bREACT=FALSE, BOOL bMOBCOLL=TRUE );
	BOOL LargeMapMoveTo( D3DXVECTOR3* pPos, BOOL bContinue );
	BOOL	DominationMoveTo(float fTime, D3DXVECTOR3 vFromPt, D3DXVECTOR3 vTargetPt, BOOL bContinue=FALSE);

public:
	DxSkinChar* GetSkinChar()	{ return m_pSkinChar; }

public:
    virtual void FactEffectProcessing ( const EMFACTEFFPROCESSING emFactEffactCase );

	CNpcTalkControl::NPCMAP* FindDefaultTalk( const std::string& strTalkFile );

public:
	virtual const D3DXVECTOR3 &GetMaxOrg()     { return m_vMaxOrg; }
	virtual const D3DXVECTOR3 &GetMinOrg()     { return m_vMinOrg; }

	float GetDirection();

public:
	SCHARDATA2& GetCharData()			{ return(*this); }
	GLCHARLOGIC& GetCharLogic()		{ return(*this); }
	SNATIVEID GetskillRunSlot()		{ return m_sRunSkill; }
	
	void SetActiveSkillRun( SNATIVEID sSkillID ) { m_sActiveSkill = sSkillID; }
	SNATIVEID GetActiveSkillRun()				 { return m_sActiveSkill; }

	void SetScopeSkillRun( const SKILLID& sSkillID ) { m_sScopeSkill = sSkillID; }
	const SKILLID& GetScopeSkillRun()				 { return m_sScopeSkill; }
	
    //! 대련
    void EndConfting();
    bool IsConfting() const { return m_sCONFTING.IsCONFRONTING(); }
    EMCONFT_TYPE GetConftingType() const { return m_sCONFTING.emTYPE; }
    void ResetConfting() { m_sCONFTING.RESET(); }

	const WORD FindQuickSkillSlot( const SKILLID& sSkillID );

	float GetBuyRate();
	float GetSaleRate();

	void SetMotion( EMANI_MAINTYPE eMID, EMANI_SUBTYPE eSID, FLOAT fSec );

	void QuickChangeSkill();
	void QuickChangeReset();	

public:
	BOOL GET_PRETRADE_ITEM( SITEMCUSTOM& sItemCustom );
	BOOL IsVALID_STORAGE( DWORD dwChannel );

public:
	BOOL IsPostBoxWindowOpen();
    
    //! 개인상점검색 윈도우가 열려있는가?
    BOOL IsPrivateMarketSerchWindowOpen();

	BOOL IsPrivateMarketWindowOpen();

	void DoPrivateMarketClose();

public:
	DWORD DetectGate();
	void DetectGateToMapName(MAPNAMEVECTOR& mapNameVector);

	HRESULT UpdateSuit( BOOL bChangeHair = TRUE, BOOL bChangeFace = TRUE );
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


private:
	void UpdateCharPos_RenderChar( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE );
	void UpdateCharPos_RenderChar_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE );
	void UpdateCharPos_RenderChar_MaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE );
	void RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharBase* pRenderChar, BOOL bOpaqueState, BOOL bHALF_VISIBLE );
	void RenderMyCharMaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharBase* pRenderChar, BOOL bHALF_VISIBLE );
	void CalculateMatrix( const D3DXVECTOR3& vPosition );
	void UpdateSkillRandomBuild(float fTime, float fElapsedTime );
	void SetActiveAttack( float fTime );
	BOOL BuildRandomQuick();
	
public:

	virtual void Play_ScreenThrow( DWORD dwIndex, const STARGETID& sID );
	virtual void Stop_ScreenThrow( DWORD dwIndex );

	virtual void Play_CameraHit( WORD wEffNum );
	virtual void Stop_CameraHit( WORD wEffNum );

	virtual void End_FeignDeath( BOOL bForce );

	virtual void Play_Disguise( int iIdx, BOOL bOn );
	virtual void Stop_Disguise(  );

	virtual void SetGaeaID(const DWORD dwGaeaID) override
	{
		m_dwGlobalID = dwGaeaID;
		m_dwGaeaID = dwGaeaID;
	}
	virtual const DWORD GetCharID() const	override    { return m_CharDbNum; }
	virtual const char* GetName() const	{ return m_szName; }
	virtual WORD GetLevel() const		{ return m_wLevel; }
	virtual DWORD GetClubID() const;
	virtual bool CheckIsValidClub( int dbClubID );
	virtual bool IsValidClub() const;
	virtual GLDWDATA GetHp() const		{ return m_sHP; }
	virtual const EMCHARINDEX GetCharIndex() const { return m_CHARINDEX; }
	virtual const WORD GetCharSchool() const	{ return m_wSchool; }
	virtual const WORD GetChaRebornNum() const	{ return m_wRebornNum; }

	virtual WORD GetBodyRadius();
	virtual float GetBodyHeight()			{ return m_fHeight; }
	virtual D3DXVECTOR3 GetPosBodyHeight();

	virtual bool IsSafeZone() const { return m_bSafeZone; }
	virtual bool IsCDMSafeTime() const		{ return m_fCDMSafeTime > 0.0f; }

	virtual const BOOL IsNonTargetable(void) const override;// { return m_sHALLUCINATE.bNonTargetable; }

	void RestoreActStateByInvisible(); // 상태 전환 및 스킬 관련 정리와 함께 정리 해야 함;

    /**
        맵 입장 후 처리를 수행 한다.
        입장 후 서버에 정보를 요청 할 필요가 있을 때 사용할 수 있다.
        예를들어 맵 이동 후 1번 PVE 인던 정보를 요청하고 받아 정보를 갱신하고 있다.

     */
     void PostEnterMap();

protected:
	virtual void UPDATE_DATA_FINISH(const DWORD dwSkillFactIndex){ UPDATE_DATA_END(dwSkillFactIndex); }
	virtual void UPDATE_DATA_END(const DWORD dwSkillfactIndex, BOOL bTimeDel = FALSE, BOOL bForceEnd = FALSE, BOOL bOverLabEnd = FALSE);	
	virtual void UPDATE_DATA_Additional(const float fElapsedTime);

	void AddSkillFact_SpecialAddon_Client(const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF);

    void RECEIVE_SKILLFACT_Taunt( const STARGETID& sTargetID, const float fDistance );
	void RECEIVE_SKILLFACT_Invisible(const float fVAR2);
	void RECEIVE_SKILLFACT_Dominate(const SKILL::SSPEC& sSPEC);
	void RECEIVE_SKILLFACT_RandomExpRate(const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const float fLife, const WORD wLevel, SSKILLFACT& sSKILLEF, const float* const pLaunchTime, const float* const pLaunchValue);
	const BOOL RECEIVE_SKILLFACT_AIRBORNE(const SKILL::SSPEC& sSPEC, const SKILL::CDATA_LVL& sSKILL_DATA);
	const bool RECEIVE_SKILLFACT_Special(PGLSKILL pSkill, const WORD wlevel, const DWORD dwCount, const STARGETID& sID, SSKILLFACT& sSKILLEF, const SNATIVEID& skill_id, const float* const pLaunchTime = 0, const float* const pLaunchValue = 0);
	const bool RECEIVE_SKILLFACT_Addon( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF );
	const bool RECEIVE_SKILLFACT_Basic( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF);
	const bool RECEIVE_SKILLFACT_EntranceExit( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID );
	const bool RECEIVE_SKILLFACT_Distinguish( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID );
	const bool RECEIVE_SKILLFACT_Provoke( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID );
	
	const BOOL RECEIVE_SKILLFACT( const GLMSG::SNETPC_SKILLHOLD_BRD* pMsg );
	const BOOL RECEIVE_SKILLFACT( 
		const SNATIVEID& skill_id, 
		const WORD wlevel, 
		const DWORD dwSELECT, 
        const STARGETID& sID,
		const SSKILLFACT::STIME sTime=SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME), 
		const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT,
		const float* const pLaunchTime = 0, const float* const pLaunchValue = 0);

	const BOOL RECEIVE_SKILLCA(
		const GLMSG::SNETPC_SKILLHOLD_BRD* pMsg );

	virtual void ReceiveDamage(const int nDamage, DWORD dwDamageFlag, WORD wSchool, const STARGETID& sACTOR) override;
	virtual void ReceiveHeal(const int nHeal, DWORD dwHealFlag, WORD wSchool, const STARGETID& sACTOR) override;
	virtual void ReceiveBlocking(const STARGETID& sACTOR) override;
	virtual void ReceiveAVoid( const STARGETID& sACTOR ) override;
	virtual void ReceiveSwing() override;
	void ReceiveShock();

	void PrintTracingUserWindowInfo();
	void PrintTracingUserProcessInfo( float fElapsedTime );

public:	
    bool IsInsertToInven(std::tr1::shared_ptr<CItemClientDrop> spItemDrop);
	bool IsInsertToInven(const SITEM* pItem);
    //! 인벤토리 남은공간
    DWORD InvenAmountEmpty(bool bAllLine);
	DWORD GetAmountActionQ(WORD wSLOT);

public:
	bool IsInvenSplitItem( WORD wPosX, WORD wPosY, bool bVietnamInven = FALSE );
	bool IsInvenDecomposeItem( WORD wPosX, WORD wPosY );
	bool IsStorageSplitItem( DWORD dwChannel, WORD wPosX, WORD wPosY );
	bool IsNpcPileItem( SNATIVEID sNID_NPC, DWORD dwChannel, WORD wPosX, WORD wPosY );
	bool IsRestartPossible(); // 이 맵에서 재시작이 되는지
	BOOL IsVaildTradeInvenSpace();

public:
	void UpdateMacroState( float fElapsedTime );
	void FillMacroTime(float fTime);
	bool ReqMacroModeOnoff(bool bOn);
	bool PlayerGotoLocation(D3DXVECTOR3& vPosition);

	void ReqToggleRun();
	void ReqTogglePeaceMode();
	void ReqGateOut(const DWORD dwIndex = 0);

	void ReqVisibleNone();
	void ReqVisibleOff();
	void ReqVisibleOn();

	bool ReqGESTURE( int nMOTION, bool bCOMMAND=false );

	void SET_RECORD_CHAT();
	void UPDATE_RECORD_CHAT( bool bCloseClient = FALSE );


	bool GetLinkData( const SLINK_DATA_BASIC& sLinkDataBasic, CString& strLinkName, DWORD& dwColor ); 
	bool GetLinkDataName( const SLINK_TYPE& sLinkType, DWORD dwData, CString& strLinkName, DWORD& dwColor );

public:
	EMCONFT_TYPE GetConftType( DWORD dwID );

public:
	void InitAllSkillFactByMoveMap();

public:
	LONGLONG GetCalcTaxiCharge( int nSelectMap, int nSelectStop );

public:
	bool IsPOSTBOXUSEABLE( const EMPOSTBOXID_TYPE emIDType, const SNATIVEID& sPostBoxID );

public:
	bool IsCoolTime(const SNATIVEID& sNativeID);
	bool CheckCoolTime(const SNATIVEID& sNativeID);
	bool SetCoolTime( ITEM_COOLTIME& sCoolTime, EMCOOL_TYPE emCoolType );
	__time64_t GetMaxCoolTime(const SNATIVEID& sNativeID);

	const ITEM_COOLTIME& GetCoolTime(DWORD dwCoolID, EMCOOL_TYPE emCoolType) const;
	const ITEM_COOLTIME& GetCoolTime(const SNATIVEID& sNativeID) const;

public:
	inline const int GetOpenCharSlot ( void ) const { return m_nOpenCharSlot; }
	inline const EMTYPE_ITEM_REMODEL GetTypeItemRemodel ( void ) const { return m_emTypeItemRemodelState; }

public:
	virtual BOOL CHECK_ACTIVITY_POINT( const SITEM& sItem );
	virtual BOOL CHECK_CONTRIBUTION_POINT( const SITEM& sItem );
	
    // 인스턴스 던전 스크립용 NPC 클릭 이벤트시 메시지 전송;
    HRESULT ReqClickTrigger( const STARGETID& _target );

public:
	//! 채팅내용 전송
	HRESULT ReqSendChatMsg( DWORD dwChatType, const char* szName, const char* szMsg, const VEC_LINK_DATA_BASIC& vecLinkDataBasic );	

	//! 파티모집 링크 가입 지원
	HRESULT ReqPartyLinkJoin( DWORD dwMasterID, int nLinkIndex = -1);

	//! 파티모집 링크 수락 및 거절
	HRESULT ReqPartyLinkLure( bool bLure, const DWORD dwHashKey, const DWORD dwTimeKey, const DWORD dwGaeaID );

    //! 인벤 아이템 착용 요청
    HRESULT ReqInvenToSlot(WORD wPosX, WORD wPosY);

    //! 인벤 아이템 지정 슬롯으로 착용 요청
    HRESULT ReqInvenToSpecificSlot( EMSLOT emSlot, WORD wPosX, WORD wPosY );

    //! 물약 사용 요청.( 인벤 아이템으로. );
    HRESULT ReqInvenCure(WORD wPosX, WORD wPosY);

	//! 스킬 배우기 요청.( 인벤 아이템으로. )
	HRESULT ReqInvenSkill(WORD wPosX, WORD wPosY);

	//! 분해 요청;
	HRESULT ReqInvenItemDecompose(WORD wPosX, WORD wPosY);

	//! 스킬 배우기 요청.( 창고 아이템으로. )
	HRESULT ReqStorageSkill( SNATIVEID sNPCID, DWORD dwNPCGlobalID, DWORD dwChannel, WORD wPosX, WORD wPosY );

	//! 귀환 요청.
	HRESULT ReqReCall(WORD wPosX, WORD wPosY);

	//! 소환서 사용 요청.
	HRESULT ReqTeleport(WORD wPosX, WORD wPosY);

	//! 선물상자 열기.
	HRESULT ReqBoxOpen(WORD wPosX, WORD wPosY);

	//! 랜덤아이탬 박스 열기.
	HRESULT ReqRandumBoxOpen(WORD wPosX, WORD wPosY);

	//! 랜덤아이탬 박스 열기.
	HRESULT ReqDisJunction(WORD wPosX, WORD wPosY);

	//! 펫 랜덤 스킨 아이템 열기.
	HRESULT ReqPetSkinPackOpen(WORD wPosX, WORD wPosY);

	//! 기간 연장하기;
	HRESULT ReqPeriodExtend(WORD wPosX, WORD wPosY);

	//! 연마하기.
	HRESULT ReqGrinding(WORD wPosX, WORD wPosY);

	//! 일시강화
	HRESULT ReqTLGrinding(WORD wPosX, WORD wPosY);
	void    ReqTLGrinding();

	//! 코스툼 복장과 일반 복장과 결합.
	HRESULT ReqDisguise(WORD wPosX, WORD wPosY);

	//! 코스툼 복장과 일반 복장과 분리.
	HRESULT ReqCleanser(WORD wPosX, WORD wPosY);

    //! 아이템을 박스포장지로 씌움.
    HRESULT ReqBoxWrapping(WORD wPosX, WORD wPosY);

    //! 포장된 박스를 풂;
    HRESULT ReqBoxUnwrapping(WORD wPosX, WORD wPosY);

	//! 케릭터 카드 사용.
	HRESULT ReqCharCard(WORD wPosX, WORD wPosY);

	//! 창고 카드 사용.
	HRESULT ReqStorageCard( WORD wPosX, WORD wPosY, WORD wSTORAGE );

	//! 창고 카드 사용.
	HRESULT ReqInvenLineCard(WORD wPosX, WORD wPosY);

	//! 창고 연결 카드 사용 요청
	HRESULT ReqStorageOpenCard(WORD wPosX, WORD wPosY);

	//! 인벤토리에서 창고 오픈 요청
	bool ReqStorageOpenFromInven();

	//! 잠금 상자 오픈 요청;
	HRESULT ReqLockBoxUnlock(WORD wPosX, WORD wPosY);
	//! 랜덤아이탬 박스 열기;
	HRESULT ReqLockBoxOpen(WORD wPosX, WORD wPosY);

	//! 창고 연결 카드 사용 완료
	HRESULT ReqStorageCloseCard();

    HRESULT ReqPMarketSearchOpen(WORD wPosX, WORD wPosY);
    HRESULT ReqPMarketSearchClose();

	//! 프리미엄셋 사용.
	HRESULT ReqPremiumSet(WORD wPosX, WORD wPosY);
	
	//! 스킬및 스텟 리셋.
	HRESULT ResetSkillStats(WORD wPosX, WORD wPosY);
	HRESULT ReqResetSkillStats( WORD* pStats, const WORD wMaxAmount, const EMITEM_TYPE emItemType );	
	HRESULT RequestResetSkillStats( LuaTable tbStats, const WORD wMaxAmount, const EMITEM_TYPE emItemType );
	HRESULT RequestOpenResetSkillStats( const WORD wPosX, const WORD wPosY );
	HRESULT RequestResetSkillStatsBySkillUI();

	//! NPC 대화창 열기;
	HRESULT ReqDialogueWindowOpen(const SNATIVEID& sNPCID, const DWORD dwGaeaID, const EMCROW emCrow = CROW_NPC );

	// Note : 우편함 연결 카드 및 우체통 연결
	HRESULT ReqPostBoxOpenCard(WORD wPosX, WORD wPosY);
	HRESULT ReqPostBoxOpen	  ( const SNATIVEID& sTargetID );
	HRESULT	ReqPostBoxClose	  ();

	// 우편함 보내기우편 아이템;
	HRESULT	ReqPostBoxSendPageAttachItem();

	//! 헤어스타일 변경.
	HRESULT ReqInvenHairChange(WORD wPosX, WORD wPosY);

	HRESULT ReqInvenHairStyleChange( WORD wHairStyle );
	HRESULT InvenHairStyleChange(WORD wPosX, WORD wPosY);

	HRESULT ReqInvenHairColorChange( WORD wHairColor );
	HRESULT InvenHairColorChange(WORD wPosX, WORD wPosY);

	//! 코스튬 색깔 변경. by luxes.
	HRESULT ReqInvenCostumColorChange( DWORD dwMainColor, DWORD dwSubColor );
	HRESULT InvenCostumColorChange( EMSLOT emSlot, bool bRestrict = true ); // bRestrict 기능 제한 여부. default는 제한됨.

	//탈것 색상 변경
	HRESULT ReqInvenVehicleColorChange( DWORD dwMainColor[], DWORD dwSubColor[] );
	HRESULT InvenVehicleColorChange(WORD wPosX, WORD wPosY);

	//바이크 부스터 추가
	HRESULT ReqInvenBikeBoost(WORD wPosX, WORD wPosY);	

	//! 얼굴스타일 변경.
	HRESULT ReqInvenFaceChange(WORD wPosX, WORD wPosY);

	HRESULT ReqInvenFaceStyleChange( WORD wHairStyle );
	HRESULT InvenFaceStyleChange(WORD wPosX, WORD wPosY);

	//! 택시카드 사용
	HRESULT InvenUseTaxiCard(WORD wPosX, WORD wPosY);

	//! NPC 소환
	HRESULT InvenUseNpcRecall(WORD wPosX, WORD wPosY);

	// Note : 성별 변경
	HRESULT InvenGenderChange(WORD wPosX, WORD wPosY);
	HRESULT	ReqInvenGenderChange( WORD wFace, WORD wHair );

	HRESULT	ReqInvenSchoolChange( WORD wPosX, WORD wPosY );
	HRESULT	ReqInvenSchoolChange( WORD wSchool );

	// Note : 이름 변경.
	HRESULT ReqInvenRename( const char* szCharName );

	HRESULT InvenRename(WORD wPosX, WORD wPosY);

	//	Memo : 개조 기능
	HRESULT ReqRemodelOpenCard(WORD wPosX, WORD wPosY);

	//	Memo : 휴지통 기능
	HRESULT ReqGabargeOpenCard(WORD wPosX, WORD wPosY);

	//! 베트남 아이템, 경험치 획득 카드 사용
	HRESULT ReqInvenVietnamGet( WORD wPosX, WORD wPosY, bool bGetExp );

	// Note : 도시락(음식버프) 사용
	HRESULT ReqInvenLunchBox(WORD wPosX, WORD wPosY);

	// Note : 주사위 결정 번호 요청;
	HRESULT ReqDiceNumber(const DWORD _nDice);

	//! 스테이지 패스권
	HRESULT ReqStagePass(WORD wPosX, WORD wPosY);

	//! 인던 입장횟수 충전권
	HRESULT ReqEnterNumCharge(WORD wPosX, WORD wPosY);

    //! 판매 등록 확장 카드
    HRESULT ReqConsignmentSlotExpiration(WORD wPosX, WORD wPosY);

	//! 국가 변경 아이템 사용;
	HRESULT ReqInvenChangeCountry ( WORD wPosX, WORD wPosY );

	HRESULT	ReqJumpingLevelUp( int nExp );
	HRESULT	ReqJumpingLevelUp( WORD wPosX, WORD wPosY );

	// 경험치 압축 / 캡슐
	HRESULT	ReqInvenExpCompressor( WORD wPosX, WORD wPosY );
	HRESULT	ReqInvenExpCapsule( WORD wPosX, WORD wPosY );

	// 아이템 스킬 변경
	HRESULT ReqInvenChangeItemSkill ( WORD _wPosX, WORD _wPosY );

	// 아이템 기본 능력치 변경;
	HRESULT ReqInvenChangeBasicStats ( WORD _wPosX, WORD _wPosY );

	// 아이템 가산 능력치 변경;
	HRESULT ReqInvenChangeAddStats ( WORD _wPosX, WORD _wPosY );

	// 캐릭터 슬롯 락 해제;
	HRESULT ReqInvenUnlockCharacterSlot ( WORD _wPosX, WORD _wPosY );

	// Custom Message ( Instance System );
	void SendCustomMessage ( const DWORD _dwDestType, const DWORD _dwInstanceDungeonID, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4 );

public:
	//! 거래할 금액 넣기.
	//HRESULT ReqTradeMoney( LONGLONG lnMoney );

	//! 거래할 아이템 넣기/제거/교환.
	HRESULT ReqTradeBoxTo(WORD wPosX, WORD wPosY);

	// Note : 거래 잠금.
	HRESULT ReqTradeLock(); // by luxes.

	//! 거래 수락.
	HRESULT ReqTradeAgree();

	//! 거래 취소.
	HRESULT ReqTradeCancel();

	//! 대련 요청.
	HRESULT ReqConflict( DWORD dwID, const SCONFT_OPTION &sOption );

public:
	//! 창고 정보를 서버에 요청.
	HRESULT ReqGetStorage( DWORD dwChannel, DWORD dwNpcGlobID );
	bool ReqGetStorageForWidget( DWORD dwChannel );

	//! 특정 창고 아이템 정보를 서버에 요청.
	HRESULT ReqGetStorageSpecificItem(DWORD itemType);
	
	//! 창고 아이템 들때, 놓을때, 교환할때, 합칠때.
	HRESULT ReqStorageTo( SNATIVEID sNPCID, DWORD dwNPCGlobalID, DWORD dwChannel, WORD wPosX, WORD wPosY );

	//! 인벤토리 겹침 아이템 분리.
	HRESULT ReqStorageSplit( SNATIVEID sNPCID, DWORD dwNPCGlobalID, DWORD dwChannel, WORD wPosX, WORD wPosY, WORD wSplitNum );

	//! 창고 아이템 사용할때( 마시기, 스킬배우기 등 ).
	HRESULT ReqStorageDrug( SNATIVEID sNPCID, DWORD dwNPCGlobalID, DWORD dwChannel, WORD wPosX, WORD wPosY );

	//! 창고 돈 넣기.
	HRESULT ReqStorageSaveMoney( SNATIVEID sNPCID, DWORD dwNPCGlobalID, LONGLONG lnMoney );
	void ReqStorageSaveMoneyForWidget( LONGLONG lnMoney );

	//! 창고 돈 빼내기.
	HRESULT ReqStorageDrawMoney( SNATIVEID sNPCID, DWORD dwNPCGlobalID, LONGLONG lnMoney );
	void ReqStorageDrawMoneyForWidget( LONGLONG lnMoney );

	//! 상인 Npc에서 물품을 구입하거나 판매시.
	HRESULT ReqNpcTo( SNATIVEID sNPCID, DWORD dwNPCGlobalID, DWORD dwChannel, WORD wPosX, WORD wPosY, WORD wBuyNum=1, EMCROW emCrow = CROW_NPC );

    //! 상인 Npc에서 물품을 판매시.
    HRESULT ReqNpcSaleTo( SNATIVEID sNPCID, DWORD dwNPCGlobalID, WORD wPosX, WORD wPosY, EMCROW emCrow = CROW_NPC );

    //! 상인 Npc에서 물품을 우클릭으로 바로 판매시.
    HRESULT ReqNpcDirectSaleTo( SNATIVEID sNPCID, DWORD dwNPCGlobalID, WORD wPosX, WORD wPosY, EMCROW emCrow = CROW_NPC );

	//! 구입한 물품 꺼내오기.
	HRESULT ReqChargedItemTo(WORD wPosX, WORD wPosY);

	//! 구입한 이이템 정보 가져오기
	HRESULT ReqItemBankInfo(bool bFirstJoin);

    //! NPC 재매입 ui에서 소모품 아이템 체크.
    HRESULT ReqCheckRebuySupplies( bool bCheck );

	//! 상인 Npc에서 교환 물건 구매
	HRESULT ReqExChangeNpcTo( const GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC* pData );

public: 
    void    MsgActivityUpdateComplate(NET_MSG_GENERIC* nmg);
    void    MsgActivityUpdateValue(NET_MSG_GENERIC* nmg);
    void    MsgActivityNotifyComplate(NET_MSG_GENERIC* nmg);
    void    MsgActivityNotifyClosed(NET_MSG_GENERIC* nmg);
    void    MsgActivitySync(NET_MSG_GENERIC* nmg);
    void    MsgActivityRewardPoint(NET_MSG_GENERIC* nmg);
    void    MsgActivityRewardBadge(NET_MSG_GENERIC* nmg);
    void    MsgActivityChangeBadgeFB(NET_MSG_GENERIC* nmg);
	void    MsgActivityLoadCompletedFF(NET_MSG_GENERIC* nmg);

	void	MsgGetChaExtraInfo(NET_MSG_GENERIC* nmg);

	//! 필드 아이템(돈) 주을때.
	HRESULT ReqFieldTo( const STARGETID &sTargetID, bool bPet = false );

	//! 인벤토리 아이템 들때, 놓을때, 교환할때, 합칠때.
	HRESULT ReqInvenTo(WORD wPosX, WORD wPosY);
	//! 베트남 인벤토리 아이템 들때, 놓을때, 교환할때, 합칠때.
	HRESULT ReqVNInvenTo(WORD wPosX, WORD wPosY);
	//! 베트남 인벤토리의 아이템 전체 삭제
	HRESULT ReqVNInveReset();
	//! 베트남 인벤토리에서 오른쪽 버튼으로 아이템을 옮길 경우
	HRESULT ReqVietemInvenTo(WORD wPosX, WORD wPosY);

	//! 인벤토리 겹침 아이템 분리.
	HRESULT ReqInvenSplit( WORD wPosX, WORD wPosY, WORD wSplitNum );

	//! 인벤토리 아이템 사용할때( 마시기, 스킬배우기 등 ).
	ESlotHold ReqInvenDrug(WORD wPosX, WORD wPosY);

    //! 인벤토리 아이템 종류 확인
    EMITEM_TYPE GetInvenItemType( WORD wPosX, WORD wPosY );

    bool ReqInvenDrugDrug( EMITEM_DRUG DrugType );
    void ReqInvenDrugNotHoldItem( const SITEM* pItem, WORD wPosX, WORD wPosY, const SINVENITEM* pInvenItem );
    void ReqInvenDrugPrivateMarket( WORD wPosX, WORD wPosY );
    void ReqInvenDrugPetCard( const SINVENITEM* pInvenItem, WORD wPosX, WORD wPosY );
    void ReqInvenDrugLunchBox( WORD wPosX, WORD wPosY );
    ESlotHold ReqInvenDrugHoldItem( const SITEM* pHold, const SITEM* pItem, WORD wPosX, WORD wPosY );
    void ReqInvenDrugHoldPetRevive( const SITEM* pItem, WORD wPosX, WORD wPosY );
    //! 인벤토리 아이템 정렬.
    HRESULT ReqInvenItemSort();
    HRESULT MsgReqInvenItemSort(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqInvenItemSortStart(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqInvenItemSortData(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqInvenItemSortEnd(NET_MSG_GENERIC* nmg);

	// 아이템 내구도/부스트 , 수리;
	virtual void DurabilityDropHit( unsigned int nDamage, BOOL bPvp/* = FALSE  */);
	virtual void DurabilityDropUseSkill();
	virtual void DurabilityDropDeath( BOOL bPvp = FALSE );
	virtual void ItemRepairing( EMSLOT _slot );
	virtual void Durabilitiy_INIT_DATA( EMSLOT slot ); 
	
	// UI 재로드시 제대로 보이게하기위함;
	LuaTable GetWeaponBoostInitData(void);
	void SfSetWeaponBoostInfo(void);
private:
	bool GetWeaponDurabilityValue(float& fDurability, float& fMaxDurability);

public:
	//! 엑션 퀵슬롯에 있는 것을 쓰기.( 약품일 경우 마시기 ).
	void ReqActionQ(WORD wSLOT);

    void ReqActionQ_SlotDrug( SNATIVEID NativeId );
	//! 스킬 렙업 요청.
	HRESULT ReqSkillUp(const SNATIVEID& skill_id);
    HRESULT ReqRnSkillUp( std::vector<SNATIVEID>& vecSkillID );

	//! 사용할 스킬 설정.
	HRESULT ReqSkillRunSet( const WORD wSLOT );
	HRESULT ReqSkillRunReSet();

	//! 아이템 슬롯에 등록/제거.
	HRESULT ReqItemQuickSet( const WORD wSLOT );
	HRESULT ReqItemQuickReSet( const WORD wSLOT );

	//! 스킬 슬롯에 등록/제거.
	HRESULT ReqSkillQuickSet( const SNATIVEID& skill_id, const WORD wSLOT );
	HRESULT ReqSkillQuickReSet( const WORD wSLOT );

	//! 착용 인벤토리에 아이템 사용할때. by luxes.
	HRESULT ReqSlotConsume( EMSLOT emSlot );

	//! 아이템을 착용하거나 들어올림.
	HRESULT ReqSlotTo( EMSLOT emSlot );

	//! 극강부 무기를 메인과 서브 교체.
	HRESULT ReqSlotChange();

	//! 아이템을 구입하거나/팔아버림.
	//! 아이템 바닥에 버림.
	HRESULT ReqHoldToField( const D3DXVECTOR3 &vPos );
	void ReqHoldToFieldForWidget( const char* szSlotID, int nHoldItemPlace, 
		WORD wSlotPosX, WORD wSlotPosY, float fPosX, float fPosY, float fPosZ );

	//! 아이템 바닥에 버릴 수 있는지 체크. by luxes.
	HRESULT CheckHoldToField( const D3DXVECTOR3 &vPos );

	//! 폭죽을 바닥에 사용.
	HRESULT ReqHoldToFieldFireCracker( const D3DXVECTOR3 &vPos );

	//! 돈 바닥에 버림.
	HRESULT ReqMoneyToField( LONGLONG lnMoney );

	//! 부활 위치 지정 요청.
	HRESULT ReqReGenGate( DWORD dwNpcGlobID );

	//! 서버에 경험치 복구를 요청한다.(NPC)
	HRESULT ReqRecoveryExpNpc( DWORD dwNpcGlobID );
	
	//! 복구할 경험치를 묻는다.
	HRESULT ReqGetReExpNpc( DWORD dwNpcGlobID );	

	//! 치료 요청.
	HRESULT ReqCure( DWORD dwNpcGlobID, DWORD dwTalkID );
	HRESULT MsgCureFB(NET_MSG_GENERIC* nmg);

	//! 케릭터 정보 리셋.( stats, skill )
	HRESULT ReqCharReset( DWORD dwNpcGlobID );

	//! NPC와 item을 교환 A:npc에게 주는것, b:npc에게서 받는것.
	HRESULT ReqItemTrade(DWORD dwNpcGlobID, DWORD dwTalkID, std::tr1::shared_ptr<SNpcTalk> spNpcTalk);

	//! 소환수 고용 요청;
	HRESULT ReqHireSummon ( DWORD dwNpcGlobID, DWORD dwTalkID, std::tr1::shared_ptr<SNpcTalk> spNpcTalk );

	//! 친구에게 가기.
	HRESULT	Req2Friend( const char *szNAME );

	HRESULT ReqNpcTalkBasic( DWORD dwNpcGlobID, DWORD nAction, DWORD dwTalkID );
	HRESULT MsgNpcTalkBasic( DWORD dwNpcGlobID, DWORD nAction, DWORD dwTalkID );

public:

	//! 새로운 퀘스트 시작.
	HRESULT ReqQuestStart( DWORD dwNpcGlobID, DWORD dwTalkID, DWORD dwQUESTID );

	//! 퀘스트 진행.( npc와 대화 )
	HRESULT ReqQuestStepNpcTalk( DWORD dwNpcGlobID, DWORD dwTalkID, DWORD dwQUESTID, DWORD dwQUESTSTEP );

	//! 퀘스트 포기.
	HRESULT ReqQuestGiveUp( DWORD dwQUESTID );

	//! 퀘스트 읽어보기 요청( 읽었음을 응답. )
	HRESULT ReqQuestREADINGReset( DWORD dwQUESTID );

	//! 퀘스트 완료요청.
	HRESULT ReqQuestComplete( DWORD dwQUESTID );

	//! 버스 승차 요청.
	HRESULT ReqBusStation( DWORD dwNpcGlobID, DWORD dwSTATION, EMCROW eCrow = CROW_NPC );

	//! 택시 승차 요청.
	HRESULT ReqTaxiStation( WORD wPosX, WORD wPosY, int nSelectMap, int nSelectStop );
	
	//! 확성기 사용 요청.
	HRESULT ReqLoudSpeaker( const char* szChat, const VEC_LINK_DATA_BASIC& vecLinkDataBasic );

public:
	//! 개인상점의 타이틀 설정.
	HRESULT ReqPMarketTitle( const char* szTitle );

	//! 개인상점에 아이템을 등록.
	HRESULT ReqPMarketRegItem( WORD wPosX, WORD wPosY, LONGLONG llMoney, DWORD dwNum );

	//! 개인상점에 등록된 아이템을 제거.
	HRESULT ReqPMarketDisItem(WORD wPosX, WORD wPosY);

	//! 개인상점 열기.
	HRESULT ReqPMarketOpen( BOOL bOnlyUI );

	//! 개인상점 닫기.
	HRESULT ReqPMarketClose();

	//! 다른 사람의 개인상점의 정보 수신.
	HRESULT ReqPMarketInfo( DWORD dwGaeaID );
	HRESULT ReqPMarketInfoRelease( DWORD dwGaeaID );

	//! 다른 사람의 개인 상점에서 물품을 구입.
	HRESULT ReqPMarketBuy( DWORD dwGaeaID, WORD wPosX, WORD wPosY, DWORD dwNum );

public:    
	//! 클럽 생성.
	HRESULT ReqClubNew( DWORD dwNpcGlobID, const char* szClubName );
	void ReqClubNewForWidget( const char* szClubName );
	
	//! 선도지역 수수료 설정.
	HRESULT ReqGuidCommission( DWORD dwNpcGlobID, float fRATE );
	void ReqGuidCommissionForWidgt( float fRate );

	//! 출석 리스트 요청
	HRESULT	ReqAttendList( bool bDay = false );

	//! 출석 요청
	HRESULT	ReqAttendance();

	HRESULT ReqGathering( const STARGETID& sTargetID );
	HRESULT ReqCancelGathering();

public:
	//! 캐릭터의 전화번호 저장 / 업데이트
	HRESULT ReqSetPhoneNumber( const TCHAR * szPhoneNumber );
	HRESULT ReqSendSMS( int nCharID, const TCHAR * szPhoneNumber, const TCHAR * szSmsMsg );

public:
	//! 파티원에게 Qbox 옵션 상태를 보낸다.
	bool    ReqQBoxEnableState( bool bQboxEnable );

public :
	//! 코스튬 능력치 부여 관련 패킷 
	void	ReqCostumeStatCToS();
	void    ReqCostumeStat( WORD wPosX, WORD wPosY );
	void    ReqCostumeStatRelease( WORD wPosX, WORD wPosY );
	void	ReqCostumeStatReleaseOK( WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY );

	// 클럽 이름 변경
	HRESULT InvenClubRename( WORD wPosX, WORD wPosY );
	HRESULT ReqInvenClubRename( const char* szClubName );

public:
	// 캐릭터 슬롯 관련;
	void	ReqLockCharSlot ( const WORD _wCharSlotNum );
	void	ReqUnlockCharSlot ( const WORD _wCharSlotNum );
	void	ReqChangeCharSlot ( const WORD _wCharSlotNum );

	const bool IsChangeCharSlot ( const WORD _wCharSlotNum );

private :
	int m_nTitleSelectIndex;					// 선택한 타이틀 인덱스

	enum { SKILL_TAB_COUNT = 4 };
	VEC_GLSKILL m_vecSkill[ SKILL_TAB_COUNT ];	// 스킬 리스트

public :
	// 스킬 리스트 생성
	void GenerateSkillList();

public :
	// 타이틀 정보
	LuaTable GetTitle();
	// 선택한 타이틀 인덱스
	int GetTitleSelectIndex() { return m_nTitleSelectIndex; }
	// 물리 가산치
	int GetSumPowerAttribute( int nType );
	// 저항력
	short GetResistFire() { return m_sSUMRESIST_SKILL.nFire; }
	short GetResistIce() { return m_sSUMRESIST_SKILL.nIce; }
	short GetResistElectric() { return m_sSUMRESIST_SKILL.nElectric; }
	short GetResistPoison() { return m_sSUMRESIST_SKILL.nPoison; }
	short GetResistSpirit() { return m_sSUMRESIST_SKILL.nSpirit; }
	// 능력치 상승 값
	LuaTable GetChangeUpAbillity( WORD wPow, WORD wDex, WORD wSpi, WORD wStr, WORD wSta );
	// 스탯 포인트 사용
	void ApplyStatPoint( WORD wPow, WORD wDex, WORD wSpi, WORD wStr, WORD wSta );
	// 착용중인 아이템
	LuaTable GetPutOnItem( EMSLOT emSlot );
	LuaTable GetPutOnItems();
	// 착용중인 아이템 개수
	WORD GetPutOnItemCount( EMSLOT emSlot );

	//const SITEMCUSTOM* GetPutOnItemCustom( SNATIVEID nativId );
	// 착용중인 아이템 타입
	int GetPutOnItemType( EMSLOT emSlot );
	// 착용중인 무기 속성;
	int GetWeaponType(void);

	// 삭제 가능한 아이템인지 체크
	int CheckTrashItem();
	// 인벤토리 아이템 사용
	ESlotHold UseInvenItem( WORD wPosX, WORD wPosY );
	// 은행 아이템 정보 요청 시도
	bool TryRequestBankItemInfo();
	// 최근 사용 스킬 ID
	DWORD GetSkillRunSlotID() { return m_sRunSkill.dwID; }
	// 퀵슬롯 등록된 스킬
	DWORD GetSkillQuick( WORD wIndex );
	// 퀵슬롯 등록된 아이템
	DWORD GetItemQuick( WORD wIndex );

	// 채팅 메시지
	void SendChatMsg( DWORD dwType, const char* szName, const char* szMsg, LuaTable tbLinkData );

	// 스킬 인덱스 범위
	void GetSkillIndexRange( int& nBegin, int& nEnd );
	// 스킬 리스트
	LuaTable GetSkillList( int nPage );
	// 소모한 스킬 포인트
	DWORD GetUsingSkillPoint( LuaTable tbSkillList );
	// 스킬 투자
	LuaTable GetSkillInvest( DWORD dwID, WORD wLevel );
	//  배울 수 있는 최대 스킬 레벨
	LuaTable GetLearnSkillMaxLevel(DWORD dwID, DWORD dwLevel);

	LuaTable GetLearnSkillMinLevel(DWORD dwID, DWORD dwLevel);

	// 필요 스킬 포인트
	DWORD GetNeedSkillPoint( DWORD dwID, WORD wLevel );
	// 스킬 레벨 올리기
	void SkillLevelUp( LuaTable tbSkillUpData );
	// 스킬 배움 반환 금액
	std::string GetLearnedSkillCost();
	// 빈 스킬 퀵슬롯 인덱스 검색
	WORD GetEmptySkillQuick();
	// 보유중인 탈것 ID 들
	LuaTable GetHaveVehicleAll();
	// 맵 ID
	DWORD GetCurrentMapID();
	// 캐릭터 위치
	LuaTable GetPositionForLua();
	// 캐릭터 맵 위치
	LuaTable GetMapPosition();
	// 미니맵 위치 오프셋
	LuaTable GetMapPositionOffset();
	// 해당 미니맵 위치로 이동
	bool MoveToMap( DWORD dwMapID, int nPosX, int nPosY, bool bVehicle, bool bGMMove );

	void ReqBikePutOnWith();
	// 탈것 컬러 설정
	void SetVehicleColor( LuaTable tbColorList );
	// 탈것 컬러
	LuaTable GetVehicleColor();
	// 탈것 컬러 설정 확인
	void CommitVehicleColor( LuaTable tbColorList );

	// 코스튬 컬러 설정
	void SetCostumeColor( LuaTable tbColor );
	// 코스튬 컬러
	LuaTable GetCostumeColor();
	// 코스튬 컬러 설정 확인
	void CommitCostumeColor( LuaTable tbColor );

	// 캐릭터 시작 위치 맵ID
	DWORD GetStartMapID();
	// 부활 타입 결정을 위해 조건 확인
	LuaTable GetResurrectionCondition();
	// 경험치 회복 부활시 소모되는 경험치, 돈
	LuaTable GetRsurrectionExpRecovery();
	// 체크 포인트 부활 요청
	void ResurrectCheckPoint();
	// 경험치 회복 부활 요청
	void ResurrectExp();
	// 귀혼주 부활 요청
	void ResurrectCurPos();
	// 스킬 부활 요청
	void ResurrectSkill();
//퀘스트 테이블 값을 반환할 함수 구조에 필요한 것들이 상위단는 모두 없어서 여기에 위치해둠(상속이나 퀘스트 변경시 변경필요, Quest)
	LuaTable GetQuestListProc(); //영역 및 영역안의 퀘스트 제목 테이블 반환(진행중)
	LuaTable GetQuestDetailListProc(std::string _AreaName); // 인자로 받음 이름에 해당하는 영역안의 제목들 반환(진행중)
	LuaTable GetQuestListEnd(); //영역 및 영역안의 퀘스트 제목 테이블 반환(완료한)
	LuaTable GetQuestDetailListEnd(std::string _AreaName); // 인자로 받음 이름에 해당하는 영역안의 제목들 반환(완료한)
	LuaTable GettbQuestProc( DWORD dwQuestID); // 퀘스트 내용에 해당하는 데이터들을 반환(진행중)
	LuaTable GettbQuestEnd( DWORD dwQuestID);	// 퀘스트 내용에 해당하는 데이터들을 반환(완료한)
	LuaTable GettbQuestProcInfo( DWORD dwQuestID);	// 퀘스트 진행사항에 해당하는 데이터들을 반환(진행중)

	bool GetQuestComplete(DWORD _dwID, bool ProcOrEnd); //퀘스트 완료 여부 반환
	DWORD GetQuestStep(DWORD _dwID); //진행 단계 반환
	bool GetQuestCompleteCheck(DWORD _dwID); // 인벤토리가 꽉찬경우 완료버튼으로 획득하는데 그때 완료햇나 안햇나 여부
	bool GetQuestGiveUp(DWORD _dwID); //퀘스트 포기가능 여부
	void SetQuestHelpCheck(DWORD _dwID, bool _bCheck);

	// 락커 남은 시간
	LuaTable GetStorageTimeForWidget( DWORD dwChannel );
	// 프리미엄 남은 시간
	LuaTable GetPremium();

	// 인벤토리 아이템 임시 착용 능력치
	LuaTable TempPutOnStatInvenItem( WORD wPosX, WORD wPosY, WORD wVirtualPow, WORD wVirtualDex, 
		WORD wVirtualSpi, WORD wVirtualStr, WORD wVirtualSta );

	// 국가 선택 초기화 요청
	void ReqResetCountry( WORD wPosX, WORD wPosY );

	// 경험치 캡슐 사용 요청;
	void ReqUseExpCapsuleItem ( WORD wInvenPositionX, WORD wInvenPositionY );

	// 경험치 압축 사용 요청;
	void ReqUseExpCompressorItem ( WORD wInvenPositionX, WORD wInvenPositionY );

	// 캐릭터 슬롯 확장 아이템 사용 요청;
	void ReqUseUnlockCharacterSlotItem ( WORD wInvenPositionX, WORD wInvenPositionY );

	// 인벤토리 아이템 분해 가능한지 확인
	bool CheckInvenItemDecompose( WORD wPosX, WORD wPosY );

	// 장착중인 소모품
	LuaTable GetEquipConsume();

	// 수리 가능한 아이템
	LuaTable GetRepairableItem();
	// 아이템 수리 요청
	void ReqItemRepair( LuaTable tbSlot );
	// 아이템 수리 연결카드 사용
	void ReqItemRepairCardUse( WORD wPosX, WORD wPosY );

	// 인벤 아이템 개수
	DWORD GetInvenItemCount( WORD wMID, WORD wSID );
	// 인벤 아이템 착용 타입
	WORD GetInvenItemSuitType( WORD wPosX, WORD wPosY );
	// 착용 가능 체크;
	bool WearableCheck( WORD wPosX, WORD wPosY );

	// 인벤 아이템 TurnNum (수량.. ?) 을 가져오기
	DWORD GetSlotTurnNumCountItem(WORD wMID, WORD wSID);

	// 도시락 버프 가능한 맵인가?
	bool IsEnableLunchBox();

	// 코스튬 아이템 사용
	void UseDisguiseInvenItem( WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY );
	bool IsDisguiseInvenItem( WORD wPosX, WORD wPosY );

	// 개조가 가능한지 확인한다;
	const bool IsRebuildItem ( const DWORD _dwNPCGlobalID = MAXDWORD ) const;

public:
	HRESULT 				ReqRebuildOpen( SNATIVEID sNPCID, DWORD dwNPCGlobalID ); // 개조창 열기	// ITEMREBUILD_MARK
	HRESULT 				ReqRebuildMoveItem(const bool bMsg = true);	// 개조창과 손 사이의 개조 대상 아이템 이동 요청;
	HRESULT 				ReqRebuildMoveStamp(void);			// 인장창과 손 사이의 인장 아이템 이동 요청;
	HRESULT 				ReqRebuildInputMoney( LONGLONG i64InputMoney );	// 개조창에 입력한 금액 서버에 전송

	HRESULT					ReqRebuild_SelectCache(void);
	HRESULT 				ReqChangeRebuildType( DWORD dwNPCGlobalID, bool bKeepOrigin );
	HRESULT					ReqRebuildTry( SNATIVEID sNPCID, DWORD dwNPCGlobalID );				// 개조 시도;
	HRESULT 				ReqRebuildResult( SNATIVEID sNPCID, DWORD dwNPCGlobalID );			// 개조 요청;
	HRESULT 				ReqRebuildCancel( SNATIVEID sNPCID, DWORD dwNPCGlobalID );			// 개조 취소;
	HRESULT					ReqRebuildDirectResult( SNATIVEID sNPCID, DWORD dwNPCGlobalID );	// 개조 바로 요청;
	HRESULT 				ReqRebuildClose();								// 개조창 닫기

public:
	HRESULT 				ReqGarbageOpen();								// 휴지통 열기
	HRESULT 				ReqGarbageMoveItem();							// 휴지통에 아이템 이동 요청
	HRESULT 				ReqGarbageResult();								// 아이템 파쇄 요청
	HRESULT 				ReqGarbageClose();								// 휴지통 닫기

    HRESULT 				ReqChangeTitle( DWORD dwIndex, bool bSync = false );

	BOOL					GET_GARBAGE_ITEM( SITEMCUSTOM& sItemCustom );

	VOID					InitGarbageData();
	VOID					OpenGarbage()	   				{ m_bGarbageOpen = true; }
	VOID					CloseGarbage()	   				{ m_bGarbageOpen = false; }
	bool					ValidGarbageOpen() const		{ return m_bGarbageOpen; }

	bool					ValidItemMixOpen();
	const SITEMCUSTOM		GET_ITEM_MIX( int nIndex );
	void					SetItemMixMoveItem( int nIndex );
	void 					ReSetItemMixItem( int nIndex );
	void 					ResetItemMix();
	DWORD					GetItemMixMoney();
	HRESULT					ReqItemMix( DWORD dwNpcGlobID );

	bool					ValidWindowOpen();

	VOID					OpenCostumeStat()	   				{ m_bCostumeStats = true; }
	VOID					CloseCostumeStat();
	bool					ValidCostumeStatOpen();

public:
	// PET
	HRESULT 				ReqUsePetCard(WORD wPosX, WORD wPosY);
	HRESULT 				ReqReGenPet();
	HRESULT					ReqPetCardInfo();
	HRESULT 				ReqPetReviveInfo();
	HRESULT 				ReqPetRevive( DWORD dwPetID );

public:
    HRESULT                 ReqResurrect(const EMRESURRECTION_TYPE _resurrectType);  // 부활 요청;
	void 					ReqReBirth();						//! 부활 요청.
	void 					ReqLevelUp();						//! 렙업 요청.
	void 					ReqStatsUp( EMSTATS emStats );	//! 스텟 분배 요청.
	void					ReqStatsUp(WORD* emStatsArray); 
public:
	HRESULT 				SetVehicle( bool bActive );
	HRESULT 				ReqSetVehicle( bool bActive );	// 탈것 활성화/비활성화
	HRESULT 				ReqVehicleUpdate(); // 탈것 장착시 정보 요청
	void					ReqVehicleChangeAccessory( EMSUIT emSUIT ); // 탈것 장착 아이템 교체
	void					ReqVehicleRemoveSlotItem( EMSUIT emSUIT );	 // 탈것 장착 아이템 해제
	void					ReqVehicleGiveBattery(WORD wPosX, WORD wPosY);	// 탈것 연료 채우기
	void					ReqVehicleInvenUpdate();	// 탈것 정보 요청

	void					UseBikeBoost();

public:
	void					ReqItemShopOpen( bool bOpen );		// ItemShop Open/Close 통보

public:
	HRESULT 				ReqSkillHoldReset( SNATIVEID& sSkillID, WORD wSlot ); // 버프/도시락 제거 요청
	void					ReqSkillHoldResetForWidget( DWORD dwID, WORD wSlot );

	// 선택상자 관련 ;
private:
	INT		m_nSelFormBoxPosX;
	INT		m_nSelFormBoxPosY;
public:
	//! 선택상자 열기;
	HRESULT SelectiveformBoxOpen(WORD wPosX, WORD wPosY);
	//! 선택상자 아이템 선택;
	HRESULT ReqSelformBoxSelect(WORD wSelItemPos);
	void	MsgSelformBoxSelectOpenFb(NET_MSG_GENERIC* pMsg);

public:
	HRESULT 				DoActWait( DoActWait_TYPE emDoActWaitType );
	HRESULT 				Resurrect( const int nHP, const int nMP, const int nSP, D3DXVECTOR3 vPos, bool bWait );
	HRESULT 				MoveActiveMap( boost::shared_ptr<NavigationMesh> spNavi, const D3DXVECTOR3 &vPos, bool bWait=true );
	HRESULT 				PlayerUpdate( float fTime, float fElapsedTime );
	HRESULT 				SelectTargetUpdate( const bool bUsingSelectInfo );
	HRESULT 				ScopeSkillUpdate();
		
public:
	STARGETID*				FindForcedTarget( VEC_SK_TAR* pDetectMap );
	const STARGETID			GetSelectForcedTargetAround( const UINT nDegree = 360, const float fRange = 100.0f );
	const STARGETID 		GetSelectTargetAround( const EMFIND_TAR emFindTar, const D3DXVECTOR3& vPosition, const float fRange, const UINT nDegree );
	const STARGETID 		GetSelectTargetBodyAround( const EMFIND_TAR emFindTar, const float fRange, const UINT nDegree );
	const STARGETID			GetSelectTargetMouseAround( const EMFIND_TAR emFindTar, const float fRange );

public :
	virtual void ACCOUNTSKILL( const SCHARSKILL& sSkill, WORD wStrikeNum, bool bServer=false );

	void GetSkillCoolDownTime( const SNATIVEID& sID, float& fCoolDownTime, float& fCoolDownRatio );
	float GetSkillFactAge( DWORD dwSkillID );

	void GetItemCoolDownTime( const SNATIVEID& sID, float& fCoolDownTime, float& fCoolDownRatio );

	LuaTable GetBuffList();
	void RecvExperience( const SNATIVEID& sID, WORD wLevel, float fAge = 0.0f );

protected:
	void				ReSelectAnimation();
	HRESULT 		UpdateAnimation( float fTime, float fElapsedTime, const EMACTIONTYPE Action );
	//HRESULT 		UpdateMultiAnimation( float fTime, float fElapsedTime, const EMACTIONTYPE Action );
	HRESULT			UpdateAction( float fTime, float fElapsedTime );	
	void			UpdateActState(void);
	HRESULT 		UpdateClientState( GLMSG::SNETPC_UPDATE_STATE *pNetMsg );

	void 					NewConftBoundEffect( const EMCONFT_TYPE emCONFT );
	void 					DelConftBoundEffect();

protected:
	STARGETID				m_sOLD_TARMOB;

public:
	bool 					IsClubBattle_TAR(std::tr1::shared_ptr<GLCharClient> pCHAR);
	int						GetClubColorIndex(std::tr1::shared_ptr<GLCharClient> pCHAR);

protected:
	STARGETID* 				GetCONFT_TAR   ( VEC_SK_ITER_RANGE &pair );
	STARGETID* 				GetCONFT_SUMMON( VEC_SK_ITER_RANGE &pair );

public:
	SACTIONTAR 				FindActionTarget( const D3DXVECTOR3 &vTargetPt, const D3DXVECTOR3 &vFromPt, const DWORD dwFindOption, const EMFIND_TAR emFINDTAR );
	SACTIONTAR 				FindNearItem();

	void 					SK_EFF_TARG( const PGLSKILL pSkill, const SANIMSTRIKE &sStrikeEff, const STARGETID &sTarget, STARGETID *pResultTarget = NULL );
	void 					SK_EFF_SELFZONE( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget=NULL );
	void 					SK_EFF_TARZONE( const STARGETID &sTarget, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF );
	EMELEMENT				GET_ITEM_ELMT();
	void 					ACCOUNTBIKESKILL( const SCHARSKILL& sSkill, WORD wStrikeNum, bool bServer=false );

protected:	
	void 					StartAttackProc(void);
	BOOL 				AttackProc( float fElapsedTime );
	BOOL 				AttackVehicleProc( float fElapsedTime );
	void 					AttackEffect( const SANIMSTRIKE &sStrikeEff );

protected:
	void					StartSkillWaitProc();
	void					StartSkillProc();
	BOOL					IsExceedSkillHitFrameOfLast();
	BOOL					SkillProc( float fElapsedTime );

	// 스킬 시전 시 스킬에 숨겨진 무기가 있을 경우 무기를 세팅하고;
	// 스킬 애니메이션이 끝난 후 지운다;
	// 극강부용으로 쓰임;
	void					SetHiddenWeaponPiece();
	void					ResetHiddenWeaponPiece();

	void 					SKT_EFF_HOLDOUT( STARGETID sTarget, DWORD dwDamageFlag );

	void 					SkillEffect( const SANIMSTRIKE &sStrikeEff );
public:
	void 					SkillEffectDealyAction(  const SANIMSTRIKE &sStrikeEff, SNATIVEID sSkillId, SKILLTARGETDATA *sSkillTargetData  );

private:
	void 					PreLoadEffectFull();
	void 					PreLoadEffect( const PGLSKILL pSkill );

	virtual void			SetAction(const EMACTIONTYPE emAction);

protected:
	//! 범위 스킬관련 정보
	const bool SkillActivationInfoFanwise( IN const SKILLID& sSkillID, 
		IN const D3DXVECTOR3& vFromPt,
		IN const D3DXVECTOR3& vTargetPt,
		OUT D3DXVECTOR3& vOutDirection,
		OUT float& fOutRadius, 
		OUT float& fOutDegree );

	const bool SkillActivationInfoCircle( IN const SKILLID& sSkillID, 
		IN const D3DXVECTOR3& vFromPt,
		IN const D3DXVECTOR3& vTargetPt,
		OUT bool& bOutEnable, 
		OUT D3DXVECTOR3& vOutPoint,
		OUT float& fOutRadius );

public:	
	const bool SkillReaction( IN OUT GLREACTION& InReaction );

	void P2PReaction( IN OUT GLREACTION& InOutReaction );
	void PvPReaction( IN OUT GLREACTION& InOutReaction );

	void MobReaction( IN OUT GLREACTION& InOutReaction );
	void NpcReaction( IN OUT GLREACTION& InOutReaction );
	void SummonNpcReaction( IN OUT GLREACTION& InOutReaction );

	void ItemReaction( IN OUT GLREACTION& InOutReaction );
	void GatheringReaction( IN OUT GLREACTION& InOutReaction );

	void ReservedActionUpdate( float fTime );
	void DoReaction( float fTime, IN OUT GLREACTION& sReaction );
	void PreservedReaction( IN const GLREACTION& InReaction );
	GLREACTION_ITER BeginReservedSkillReaction(); // 예약스킬의 첫 반복자
	GLREACTION_ITER NextReservedSkillReaction( GLREACTION_ITER iterWhere ); // 다음 예약스킬의 반복자
	GLREACTION_ITER EndReactionIter(); // 리액션의 end

public:
	void ReqPickupNearItem();
	void ReqPickupItem( struct CROWREN* pDispName );
	void ReqSkillReaction( STARGETID sTID );
	void ReqPartyMemberSkillReaction( DWORD dwGaeaID );

public:
	SNATIVEID GetSkillIDQSlot(WORD wSlot) const;
	void AttackTargetAfterSkill( const STARGETID& sTargetID );
	void ReserveSkill( const SNATIVEID &sSkillID, GLREACTION_SKILL& sSkillReaction );
	bool SkillActiveSelfToSpec( const SNATIVEID &sSkillID, WORD wSkillQuickSlot = 0xFFFF );
	bool SkillActiveZone( const SNATIVEID &sSkillID, WORD wSkillQuickSlot );
	bool SkillAutoActiveZone( const SNATIVEID &sSkillID, WORD wSkillQuickSlot );

	bool SkillActive( const SNATIVEID &sSkillID, WORD wSkillQuickSlot = 0xFFFF, bool bAutoMatic = false );
	bool SkillActive( const WORD wSkillQuickSlot );
	void ScopeSkillActive();

	EMSKILLCHECK SkillCheck ( 
		const SNATIVEID& sSkillID, 
		const STARGETID& sTargetID,
		WORD wStrikeNum, 
		bool bPrintFail = false, 
		bool bCheckOnlyAvailability = false,  
		bool bNotLearn = false );

	EMSKILLCHECK RandomSkillCheck ( 
		const SNATIVEID& sSkillID, 
		const STARGETID& sTargetID,
		WORD wStrikeNum, 
		bool bCheckOnlyAvailability = false );

	void AutoArmSwap();

public:
	// 검사할 세트 아이디, 세트 효과를 받기위한 최소 개수, 해당 아이템 장착 개수;
	BOOL GetSetitemEquipInfo(const DWORD dwSetID, OUT DWORD& dwEffectMinTermCount, OUT DWORD& dwEquipCount);

public:
	HRESULT Create( boost::shared_ptr<NavigationMesh> spNavi, SCHARDATA2 *pCharData2, D3DXVECTOR3* pvPos, DWORD dwGaeaID, LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan );
	HRESULT SkinLoad( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan );
	HRESULT SkinLoadForTool( LPDIRECT3DDEVICEQ pd3dDevice, const char* szFile );
	void	DeleteSkin();

	// 맵을 이동하는 것이니 맵 참조값을 줄이는 방향으로 간다.
	void LandShredPtrRefSub();

public:
	void ResetData();

public:
	HRESULT FrameMove( float fTime, float fElapsedTime );
	HRESULT FrameMoveForTool( float fTime, float fElapsedTime );
	HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan );
	HRESULT RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWorldSystemOLD );
	HRESULT RenderShadow_SpotLight ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem, DWORD dwSlot );
	HRESULT RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice, float fWaterHeight );
	void	EventCalculate();
	void	VietnamCalculate();

	// 클라이언트에서 자체적으로 삭제할 일은 없다;
    //void	DisableDebuffSkillFact();
	// 지형 효과 업데이트
	void	UpdateLandEffect();
	void	UpdateMapEffect();

    void    UpdateActivityItem();

	//Rnattendance
	void	ResetRnAttendaceQuest();

	void UpdateGeneralEffect();

public:
	// 세션이 유효하지 않은 경우 여기서 처리해 준다;
	// 계정 동시 접속하여 서버와 연결 끊을 경우;
	void OffLine();

public:
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

    // Interface
public:
    void PrintMsgText(D3DCOLOR dwColor, const std::string& strText);
    void PrintConsoleText(const std::string& strText);
    void PrintMsgTextDlg(D3DCOLOR dwColor, const std::string& strText );
    void PrintConsoleTextDlg(const std::string& strText);
    void PrintMsgDlg(D3DCOLOR dwColor, const std::string& strText);

    // Callback
public:
    void CallbackHairColorCreate  ( UI::SPANNELGROUPINFOARRAY&                 OutPannelGroupInfoArray );
    void CallbackHairColorInit    ( UI::SPANNELGROUPINFOARRAY&                 OutPannelGroupInfoArray );
    void CallbackHairColorUpdate  ( const UI::SPANNELGROUPINFOARRAY&           InPannelGroupInfoArray  );
    void CallbackHairColorCommit  ( const UI::SPANNELGROUPINFOARRAY&           InPannelGroupInfoArray  );

    void CallbackCostumColorCreate( std::vector<UI::SCOLORPANNELGROUP_INFO>&   OutPannelGroupInfoArray );
    void CallbackCostumColorInit  ( std::vector<UI::SCOLORPANNELGROUP_INFO>&   OutPannelGroupInfoArray );
    void CallbackCostumColorUpdate( const UI::SPANNELGROUPINFOARRAY&           InPannelGroupInfoArray  );
    void CallbackCostumColorCommit( const UI::SPANNELGROUPINFOARRAY&           InPannelGroupInfoArray  );

    void CallbackVehicleColorCreate  ( std::vector<UI::SCOLORPANNELGROUP_INFO>&   OutPannelGroupInfoArray );
    void CallbackVehicleColorInit    ( std::vector<UI::SCOLORPANNELGROUP_INFO>&   OutPannelGroupInfoArray );
    void CallbackVehicleColorUpdate  ( const UI::SPANNELGROUPINFOARRAY&           InPannelGroupInfoArray  );
    void CallbackVehicleColorCommit  ( const UI::SPANNELGROUPINFOARRAY&           InPannelGroupInfoArray  );

	// 색상변경 카드 사용시 타입에 따라 텍스트 변경;
	bool VehicleColorChangeTextInit( std::vector<UI::SCOLORPANNELGROUP_INFO>& OutPannelGroupInfoArray );

protected:
	// 기존 소환수 슬롯 갱신시 매번 업데이트에서 체크한다;
	// 업데이트에서 체크하지않고 슬롯 변화->패시브체크 후;
	// 슬롯이 갱신되도록하기위함;
	void SummonStateVisibleUpdate(void) override;

public:
	LuaTable GetSummonHPInfo(void);
	const char* GetSummonName(void);

	//////////////////////////////////////////////////////////////////////////
	//						AnimationMove
private:
	BOOL m_bFirstAnimationMove;

private:
	void SkinAniControl_AnimationMove();
	//						AnimationMove
	//////////////////////////////////////////////////////////////////////////

public:
    struct GLCharacterPacketFunc
    {
       (void)(GLCharacter::*Msgfunc)(NET_MSG_GENERIC* pMsg);
        GLCharacterPacketFunc()
        {
            Msgfunc = NULL;
        }
    };
    GLCharacterPacketFunc m_GLCharacterMsgFunc[NET_MSG_GCTRL_MAX];

    void InitPacketFunc();

    virtual void MsgProcess(NET_MSG_GENERIC* pMsg);

    void MsgGetExpRecoveryNpcFb(NET_MSG_GENERIC* pMsg);
    void MsgEventFactInfo(NET_MSG_GENERIC* pMsg);
    void MsgEventFactEndBrd(NET_MSG_GENERIC* pMsg);
    void MsgEventFactBrd(NET_MSG_GENERIC* pMsg);
    void MsgQuestionItemFactEndBrd(NET_MSG_GENERIC* pMsg);
    void MsgIPEventBrd(NET_MSG_GENERIC* pMsg);
    void MsgQuestionItemFactBrd(NET_MSG_GENERIC* pMsg);
    void MsgInvenPostBoxOpenCardFb(NET_MSG_GENERIC* pMsg);
    void MsgPostBoxOpenFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenRenameFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenGenderChangeFb(NET_MSG_GENERIC* pMsg);
	void MsgInvenSchoolChangeFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenFaceChangeFb(NET_MSG_GENERIC* pMsg);
    void MsgUseBikeBoostCancelFb(NET_MSG_GENERIC* pMsg);
    void MsgUseBikeBoostFb(NET_MSG_GENERIC* pMsg);
    void MsgLearnBikeBoostFb(NET_MSG_GENERIC* pMsg);
    void MsgVehicleColorChangeFb(NET_MSG_GENERIC* pMsg);
    void MsgCostumeColorChangeFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenHairColorChangeFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenHairChangeFb(NET_MSG_GENERIC* pMsg);
	void MsgInvenItemDecomposeFb(NET_MSG_GENERIC* pMsg);
    void MsgPlayerKillingDel(NET_MSG_GENERIC* pMsg);
    void MsgPlayerKillingAdd(NET_MSG_GENERIC* pMsg);
    void MsgPMarketSearchItemResult(NET_MSG_GENERIC* pMsg);
    void MsgPMarketCloseBrd(NET_MSG_GENERIC* pMsg);
    void MsgPMarketItemUpdateBrd(NET_MSG_GENERIC* pMsg);
    void MsgPMarketBuyFb(NET_MSG_GENERIC* pMsg);
    void MsgPMarketOpenFb(NET_MSG_GENERIC* pMsg);
    void MsgPMarketRegItemFb(NET_MSG_GENERIC* pMsg);
    void MsgReviveFb(NET_MSG_GENERIC* pMsg);
    void MsgChargedItemDel(NET_MSG_GENERIC* pMsg);
    void MsgChargedItemGetFb(NET_MSG_GENERIC* pMsg);
    void MsgGetCharedItemFromDbFb(NET_MSG_GENERIC* pMsg);
    void MsgNpcItemTradeFb(NET_MSG_GENERIC* pMsg);
    void MsgPremiumState(NET_MSG_GENERIC* pMsg);
    void MsgFriendToFb(NET_MSG_GENERIC* pMsg);
    void MsgRebuildResult(NET_MSG_GENERIC* pMsg);
    void MsgGarbageResultFb(NET_MSG_GENERIC* pMsg);
    void MsgRebuildMoveItem(NET_MSG_GENERIC* pMsg);
    void MsgReBuildCostMoeny(NET_MSG_GENERIC* pMsg);
    void MsgReBuildInputMoney(NET_MSG_GENERIC* pMsg);
    void MsgUpdateLastCall(NET_MSG_GENERIC* pMsg);
    void MsgUpdateStartCall(NET_MSG_GENERIC* pMsg);
    void MsgSmsSendFb(NET_MSG_GENERIC* pMsg);
    void MsgSmsPhoneNumberFb(NET_MSG_GENERIC* pMsg);    
    void MsgVietnamGainType(NET_MSG_GENERIC* pMsg);
    void MsgVietnamGainMoney(NET_MSG_GENERIC* pMsg);
    void MsgVietnamGainExp(NET_MSG_GENERIC* pMsg);
    void MsgGmLimitEventBeginFb(NET_MSG_GENERIC* pMsg);
    void MsgGmLimitEventTimeReqFb(NET_MSG_GENERIC* pMsg);
    void MsgTradeProcess(NET_MSG_GENERIC* pMsg);
    void MsgActiveVehicleFb(NET_MSG_GENERIC* pMsg);
    void MsgLandEffectUpdateBrd(NET_MSG_GENERIC* pMsg);
    void MsgMapEffectUpdateBrd(NET_MSG_GENERIC* pMsg);
    void MsgGetVehicleFb(NET_MSG_GENERIC* pMsg);
    void MsgUngetVehicleFb(NET_MSG_GENERIC* pMsg);
    void MsgVehicleReqSlotExHoldFb(NET_MSG_GENERIC* pMsg);
    void MsgVehicleReqHoldToSlotFb(NET_MSG_GENERIC* pMsg);
    void MsgVehicleReqSlotToHoldFb(NET_MSG_GENERIC* pMsg);
    void MsgVehicleRemoveSlotItemFb(NET_MSG_GENERIC* pMsg);
    void MsgVehicleAccessoryDelete(NET_MSG_GENERIC* pMsg);
    void MsgVehicleUpdateClientBattery(NET_MSG_GENERIC* pMsg);
    void MsgVehicleReqGiveBatteryFb(NET_MSG_GENERIC* pMsg);
    void MsgVehicleReqItemInfoFb(NET_MSG_GENERIC* pMsg);
    void MsgWithVehicleFb(NET_MSG_GENERIC* pMsg);
    void MsgActiveWithVehicleFb(NET_MSG_GENERIC* pMsg);
    void MsgWithVehicleMoveMap(NET_MSG_GENERIC* pMsg);
    void MsgVehicleBrd(NET_MSG_GENERIC* pMsg);
    void MsgQboxOptionMember(NET_MSG_GENERIC* pMsg);
    void MsgUpdateTracingCharacter(NET_MSG_GENERIC* pMsg);
    void MsgReqAttendListFb(NET_MSG_GENERIC* pMsg);
    void MsgReqAttendanceFb(NET_MSG_GENERIC* pMsg);
    void MsgReqAttendRewardClt(NET_MSG_GENERIC* pMsg);
    void MsgNpcRecallFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenItemMixFb(NET_MSG_GENERIC* pMsg);
    void MsgReqGatheringFb(NET_MSG_GENERIC* pMsg);
    void MsgReqGatheringResult(NET_MSG_GENERIC* pMsg);
    void MsgUpdateCdmSafeTimeBrd(NET_MSG_GENERIC* pMsg);
	void MsgReqDialogueWindowOpenFb(NET_MSG_GENERIC* pMsg);
    void MsgReqNpcTalkBasicFb(NET_MSG_GENERIC* pMsg);
    void MsgChatPartyRecruitFb(NET_MSG_GENERIC* pMsg);
	void MsgChatFactionFb(NET_MSG_GENERIC* pMsg);
    void MsgChatAreaFb(NET_MSG_GENERIC* pMsg);
    void MsgChatLoudSpeakerFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenPremiumSetFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenStorageOpenFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenGarbageOpenFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenReModelOpenFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenPMarketSearchOpenFb(NET_MSG_GENERIC* pMsg);
	void MsgDominateMoveToFb(NET_MSG_GENERIC* pMsg);
	void MsgDominateMoveToFc(NET_MSG_GENERIC* pMsg);
	void MsgGoto(NET_MSG_GENERIC* pMsg);
    void MsgInvenInvenLineFb(NET_MSG_GENERIC* pMsg);
    void MsgStorageState(NET_MSG_GENERIC* pMsg);
    void MsgInvenStorageCardFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenCharCardFb(NET_MSG_GENERIC* pMsg);
    void MsgCureFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenResetSkstFb(NET_MSG_GENERIC* pMsg);
    void MsgVietnamAllInitTime(NET_MSG_GENERIC* pMsg);
    void MsgInvenVietnamExpGetFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenVietnamItemGetFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenVietnamItemGetNumUpdate(NET_MSG_GENERIC* pMsg);
    void MsgFireCrackerBrd(NET_MSG_GENERIC* pMsg);
    void MsgFireCrackerFb(NET_MSG_GENERIC* pMsg);
    void MsgCharResetFb(NET_MSG_GENERIC* pMsg);
	void MsgSelectTarget(NET_MSG_GENERIC* pMsg);
    void MsgRegenGateFb(NET_MSG_GENERIC* pMsg);
    void MsgPushPullBrd(NET_MSG_GENERIC* pMsg);
    void MsgInvenDelItemTimeLimit(NET_MSG_GENERIC* pMsg);
    void MsgInvenCleanserFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenDisguiseFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenDisjunctionFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenRandomBoxOpenFb(NET_MSG_GENERIC* pMsg);
	void MsgInvenLockBoxFB(NET_MSG_GENERIC* pMsg);
    void MsgInvenBoxOpenFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenBoxWrappingFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenBoxUnwrappingFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenItemUpdate(NET_MSG_GENERIC* pMsg);
    void MsgReqActionQFb(NET_MSG_GENERIC* pMsg);
    void MsgReqSkillQFb(NET_MSG_GENERIC* pMsg);
    void MsgStorageUpdateMoney(NET_MSG_GENERIC* pMsg);
    void MsgStorageDrugUpdate(NET_MSG_GENERIC* pMsg);    
	void MsgDefenseSkillActive(NET_MSG_GENERIC* pMsg);
	void MsgBeginSkill(NET_MSG_GENERIC* pMsg);
	void MsgEndSkill(NET_MSG_GENERIC* pMsg);	
	void MsgEndSkillBrd(NET_MSG_GENERIC* pMsg);
    void MsgInvenGrindingFb(NET_MSG_GENERIC* pMsg);
	void MsgInvenPeriodExtensionFb(NET_MSG_GENERIC* pMsg);
	void MsgInvenTLGrinding(NET_MSG_GENERIC* pMsg);
    void MsgStorageDelInsert(NET_MSG_GENERIC* pMsg);
    void MsgStorageItemUpdate(NET_MSG_GENERIC* pMsg);
    void MsgStorageDelete(NET_MSG_GENERIC* pMsg);
    void MsgStorageInsert(NET_MSG_GENERIC* pMsg);
    void MsgReqGetStorageItem(NET_MSG_GENERIC* pMsg);
    void MsgReqGetStorageFb(NET_MSG_GENERIC* pMsg);
	
	void MsgStorageSpecificItemCount(NET_MSG_GENERIC* pMsg);
	void MsgStorageSpecificItem(NET_MSG_GENERIC* pMsg);
    
	void MsgPutonDrugUpdate(NET_MSG_GENERIC* pMsg);
    void MsgCureStateBlowBrd(NET_MSG_GENERIC* pMsg);
    void MsgStateBlowBrd(NET_MSG_GENERIC* pMsg);
    void MsgSkillCancelBrd(NET_MSG_GENERIC* pMsg);
    void MsgSaveSkillFactBrd(NET_MSG_GENERIC* pMsg);
    void MsgSkillHoldRsBrd(NET_MSG_GENERIC* pMsg);
    void MsgSkillHoldBrd(NET_MSG_GENERIC* pMsg);
    void MsgSkillConsumeFb(NET_MSG_GENERIC* pMsg);
    void MsgUpdateSkillRevive(NET_MSG_GENERIC* pMsg);
    void MsgReqInvenLunchBoxFb(NET_MSG_GENERIC* pMsg);
    void MsgReqSkillHoldRsFb(NET_MSG_GENERIC* pMsg);
    void MsgReqSkillFb(NET_MSG_GENERIC* pMsg);
	void MsgSkillDealyAction(NET_MSG_GENERIC* pMsg);
    void MsgReqSkillUpFb(NET_MSG_GENERIC* pMsg);
	void MsgReqRnSkillUpComplete(NET_MSG_GENERIC* pMsg);
    void MsgReqLearnSkillFb(NET_MSG_GENERIC* pMsg);
    void MsgReqStatsUpFb(NET_MSG_GENERIC* pMsg);
	void MsgReqStatsUpExFb(NET_MSG_GENERIC* pMsg);
    void MsgReqLevelUpFb(NET_MSG_GENERIC* pMsg);
    void MsgPutonChange(NET_MSG_GENERIC* pMsg);
    void MsgPutonUpdate(NET_MSG_GENERIC* pMsg);
    void MsgPutonRelease(NET_MSG_GENERIC* pMsg);
    void MsgItemCoolTimeError(NET_MSG_GENERIC* pMsg);
    void MsgItemCoolTimeUpdate(NET_MSG_GENERIC* pMsg);
    void MsgReqVNInvenToInvenFb(NET_MSG_GENERIC* pMsg);
    void MsgInvenInsert(NET_MSG_GENERIC* pMsg);
    void MsgItemSortFail(NET_MSG_GENERIC* pMsg);
    void MsgReqItemSort(NET_MSG_GENERIC* pMsg);
	void MsgReqItemSortStart(NET_MSG_GENERIC* pMsg);
	void MsgReqItemSortData(NET_MSG_GENERIC* pMsg);
	void MsgReqItemSortEnd(NET_MSG_GENERIC* pMsg);
    void MsgReqHoldFb(NET_MSG_GENERIC* pMsg);
    void MsgUpdatePickupItem(NET_MSG_GENERIC* pMsg);
    void MsgPickUpMoney(NET_MSG_GENERIC* pMsg);
    void MsgUpdateStats(NET_MSG_GENERIC* pMsg);
    void MsgUpdateBright(NET_MSG_GENERIC* pMsg);
    void MsgUpdateSkillPoint(NET_MSG_GENERIC* pMsg);
    void MsgUpdateLp(NET_MSG_GENERIC* pMsg);
    void MsgUpdateCp(NET_MSG_GENERIC* pMsg);
    void MsgUpdateSp(NET_MSG_GENERIC* pMsg);
    void MsgUpdateMoney(NET_MSG_GENERIC* pMsg);
    void MsgUpdateExp(NET_MSG_GENERIC* pMsg);
    void MsgUpdateState(NET_MSG_GENERIC* pMsg);
	void MsgUpdateHireSummonState(NET_MSG_GENERIC* pMsg);
	void MsgUpdateCharacterSlotState(NET_MSG_GENERIC* pMsg);
    void MsgActionBrd(NET_MSG_GENERIC* pMsg);
    void MsgAttackDamage(NET_MSG_GENERIC* pMsg);
	void MsgAttackHeal(NET_MSG_GENERIC* pMsg);
    void MsgAttackAvoid(NET_MSG_GENERIC* pMsg);
    void MsgAttackBlocking(NET_MSG_GENERIC* pMsg);

    void MsgProcessAlliance(NET_MSG_GENERIC* nmg);
    void MsgProcessClub(NET_MSG_GENERIC* nmg);
    void MsgProcessQuest(NET_MSG_GENERIC* nmg);
    void MsgProcessConfront(NET_MSG_GENERIC* nmg);
    void MsgProcessConfront_FB(const GLMSG::SNETPC_REQ_CONFRONT_FB* pNetMsg);
    void MsgProcessConfront_FB_ReEnableLunch(const EMCONFT_TYPE emCONFT_TYPE);
    
    void MsgInvenDelInsert(NET_MSG_GENERIC* pMsg);
    void MsgInvenDelete(NET_MSG_GENERIC* pMsg);

    void MsgInvenDrugUpdate(NET_MSG_GENERIC* pMsg);
    void MsgSkillFactBrd(NET_MSG_GENERIC* pMsg);
    void MsgGmLimitEventEndFb(NET_MSG_GENERIC* pMsg);
    void MsgGmLimitEventApplyEnd(NET_MSG_GENERIC* pMsg);
    void MsgGmLimitEventApplyStart(NET_MSG_GENERIC* pMsg);
    void MsgVietnamTimeReqFb(NET_MSG_GENERIC* pMsg);
    void MsgChinaGainType(NET_MSG_GENERIC* pMsg);
    void MsgGmShowMeTheMoney(NET_MSG_GENERIC* pMsg);
    void MsgGmMoveToGateFb(NET_MSG_GENERIC* pMsg);	
    void MsgPMarketTitleFb(NET_MSG_GENERIC* pMsg);
    void MsgPMarketDisItemFb(NET_MSG_GENERIC* pMsg);
    void MsgRecoveryNpcFb(NET_MSG_GENERIC* pMsg);

	// 무한의 재단 빌링 아이템
	void MsgReqEnterNumChargeFb(NET_MSG_GENERIC* pMsg);

    // 재매입
    void MsgAddRebuyItem(NET_MSG_GENERIC* pMsg);
    void MsgBuyRebuyItem(NET_MSG_GENERIC* pMsg);
    void MsgCheckRebuySupplies(NET_MSG_GENERIC* pMsg);
	
	//개인상점검색
	void MsgPrivateMarketSearch(NET_MSG_GENERIC* pMsg);

	//Rnattendance Rn출석부
	void MsgProcessRnattendance(NET_MSG_GENERIC* nmg);
	
	//! 출석부 퀘스트 시작.
	HRESULT ReqAttendanceQuestStart( DWORD dwQUESTID );

	//!  스킬 쿨타임 ( 동기화 )
	void MsgSkillDelay(NET_MSG_GENERIC* pMsg);
	void MsgExchangeItemNpc(NET_MSG_GENERIC* pMsg);

	void MsgCostumeStatsFb(NET_MSG_GENERIC* pMsg);
	void MsgDrugFb(NET_MSG_GENERIC* pMsg);

	void MsgCostumeStatsReleaseFb(NET_MSG_GENERIC* pMsg);

	void MsgJumpingLevelUpFb(NET_MSG_GENERIC* pMsg);

	// 경험치 압축 / 캡슐
	void MsgExpCompressorAns(NET_MSG_GENERIC* pMsg);
	void MsgExpCapsuleAns(NET_MSG_GENERIC* pMsg);

	void MsgForceresetSkillStatFC ( NET_MSG_GENERIC* pMsg );

	// 아이템 스킬 변경;
	void MsgInvenChangeItemSkillFB ( NET_MSG_GENERIC* _pNetMsg );

	// 아이템 기본능력치 변경;
	void MsgInvenChangeBasicStatsFB ( NET_MSG_GENERIC* _pNetMsg );

	// 아이템 가산능력치 변경;
	void MsgInvenChangeAddStatsFB ( NET_MSG_GENERIC* _pNetMsg );
};

BOOL SelectSkillTarget(GLGaeaClient* const pGaeaClient, const SNATIVEID& sSkillID, const WORD wSkillLevel, const STARGETID& sTargetID, GLCharacter* pChar, const D3DXVECTOR3& vPos, SKILLTARGETDATA& sSkillTarget);

#endif // _GL_CHARACTER_H_
