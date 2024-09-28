#ifndef _GL_CHAR_H_
#define _GL_CHAR_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include <boost/function.hpp>
#include <hash_map>
#include <deque>

#include <boost/serialization/shared_ptr.hpp>

#include "../../SigmaCore/String/MinGuid.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../../EngineLib/NaviMesh/Actor.h"

#include "../../RanLogic/Activity/ActivityBase.h"
#include "../../RanLogic/Activity/TitleManager.h"
#include "../../RanLogic/GLogicEx.h"
#include "../../RanLogic/Market/MarketDefine.h"
#include "../../RanLogic/PointShop/PointShopDefine.h"
#include "../../RanLogic/Trade/GLTrade.h"
#include "../../RanLogic/FSM/GLCharStateDefine.h"
#include "../../RanLogic/TriggerSystem/TriggerSystemType.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/Skill/GLSkillEffectAction.h"
#include "../../RanLogic/Money/MoneyLogDefine.h"

#include "../Activity/ActivityManager.h"
#include "../Attendance/AttendanceManager.h"

#include "../Database/DBAction/DbAction.h"
#include "../AroundSlot/GLARoundSlot.h"
#include "../LandEvent/GLLandEventServer.h"
#include "../ServerActor.h"
#include "./GLWorkingSkill_s.h"
#include "../Product/GLProductField.h"

#ifndef GLLandMan
	class GLLandMan;
	class GLLandNode;
	typedef SQUADNODE<GLLandNode> LANDQUADNODE;
#endif //GLLandMan

typedef GLCrow* PGLCROW;
typedef sc::CGLLIST<PGLCROW>	GLCROWLIST;
typedef sc::SGLNODE<PGLCROW>	GLCROWNODE;

#define MAX_QUEST_PACKET_SIZE 900 // QUEST_BUFFER_OVER

struct SFIELDCROW;
typedef SFIELDCROW* PSFIELDCROW;

struct SFIELDLANDEVENT;
typedef SFIELDLANDEVENT* PSFIELDLANDEVENT;

struct GLPartyField;
class GLClubField;
class GLQUEST;
class GLGaeaServer;
//class GLVehicleField;
class GLPetField;
class PrivateMarketField;
struct SSKILLACTEX;
struct SSKILLACT;
class GLProductField;
/*
// 인벤토리 게임머니 변화 원인이 새로 생긴 경우 아래에 추가한다. 중간에 삽입하면 안됨
enum GAME_MONEY_CHANGE
{
	INVEN_MONEY_DEFAULT, // default
	SUBTRACT_INVEN_MONEY_TRADE, 
	SUBTRACT_INVEN_MONEY_CLUB_CREATE, 
	SUBTRACT_INVEN_MONEY_CLUB_STORAGE_SAVE, 
	SUBTRACT_INVEN_MONEY_ITEM_BUY_NPC, 
	SUBTRACT_INVEN_MONEY_ITEM_REBUY_NPC, 
	SUBTRACT_INVEN_MONEY_DROP_GROUND, 
	SUBTRACT_INVEN_MONEY_ITEM_REBUILD, 
	SUBTRACT_INVEN_MONEY_ITEM_GARBAGE, 
	SUBTRACT_INVEN_MONEY_ITEM_MIX, 
	SUBTRACT_INVEN_MONEY_EXP_RECOVERY_NPC, 
	SUBTRACT_INVEN_MONEY_EXP_RECOVERY_SPECIAL_REBIRTH, 
	SUBTRACT_INVEN_MONEY_ITEM_BUY_PRIVATE_MARKET_DIRECT, 
	SUBTRACT_INVEN_MONEY_ITEM_BUY_PRIVATE_MARKET_SEARCH, 
	SUBTRACT_INVEN_MONEY_QUEST_START, 
	SUBTRACT_INVEN_MONEY_SKILL_LEARN, 
	SUBTRACT_INVEN_MONEY_USER_STORAGE_SAVE, 
	SUBTRACT_INVEN_MONEY_AREA_CHAT_COST, 
	SUBTRACT_INVEN_MONEY_PARTY_RECRUIT_CHAT_COST, 
	SUBTRACT_INVEN_MONEY_CLUB_RANK, 
	SUBTRACT_INVEN_MONEY_TAXI,
	SUBTRACT_INVEN_MONEY_ITEM_DECOMPOSE, 
	SUBTRACT_INVEN_MONEY_ITEM_REPAIR, 
	SUBTRACT_INVEN_MONEY_POST_CHARGE, 
	SUBTRACT_INVEN_MONEY_POST_FEE_AND_ATTACHED_MONEY, 
	SUBTRACT_INVEN_MONEY_POST_ATTACHED_MONEY, 
	SUBTRACT_INVEN_MONEY_LOTTO_BUY, 
	SUBTRACT_INVEN_MONEY_MATCHING_BETTING_COST, 
	SUBTRACT_INVEN_MONEY_MATCHING_FEE, 
	SUBTRACT_INVEN_MONEY_ODDEVEN_COST, 
	SUBTRACT_INVEN_MONEY_TEXASHOLDEM_CHIP_BUY, 
	SUBTRACT_INVEN_MONEY_INSTANCE_DUNGEON_RETRY_COST, 
	SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_REGISTRATION, 
	SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY, 
	ADD_INVEN_MONEY_TRADE, 
	ADD_INVEN_MONEY_CLUB_STORAGE_SAVE_ROLLBACK, 
	ADD_INVEN_MONEY_CLUB_STORAGE_DRAW, 
	ADD_INVEN_MONEY_PARTY_MONEY_DISTRIBUTION, 
	ADD_INVEN_MONEY_GROUND_MONEY_PICK_UP, 
	ADD_INVEN_MONEY_ITEM_SELL_TO_NPC, 
	ADD_INVEN_MONEY_IVEN_VIETNAM_GET, 
	ADD_INVEN_MONEY_PARTY_RECRUIT_CHAT_COST_BACK, 
	ADD_INVEN_MONEY_ITEM_SELL_TO_PRIVATE_MARKET_DIRECT, 
	ADD_INVEN_MONEY_ITEM_SELL_TO_PRIVATE_MARKET_SEARCH, 
	ADD_INVEN_MONEY_QUEST_REWARD, 
	ADD_INVEN_MONEY_USER_STORAGE_DRAW, 
	ADD_INVEN_MONEY_SHOWMETHEMONEY, 
	ADD_INVEN_MONEY_SHOWMETHEMONEY_EMUL, 
	ADD_INVEN_MONEY_POST_ATTACHED_MONEY, 
	ADD_INVEN_MONEY_POST_CHARGE_ROLLBACK, 
	ADD_INVEN_MONEY_LOTTO_WIN, 
	ADD_INVEN_MONEY_MATCHING_BETTING_RESTORE, 
	ADD_INVEN_MONEY_MATCHING_BETTING_INCOME, 
	ADD_INVEN_MONEY_INSTANCE_DUNGEON_REWARD,

};
*/
struct UPDATED_ITEM 
{
	SNATIVEID sPosition;
	WORD wCount;

	UPDATED_ITEM()
		: sPosition( false )
		, wCount( 0 )
	{
	}

	UPDATED_ITEM( SNATIVEID Pos, WORD Count )
		: sPosition( Pos.dwID )
		, wCount( Count )
	{
	}
};

//Serialization - 일단 다 넣자

//! 캐릭터.
class GLChar : public GLCHARLOGIC, public ServerActor
{
public:
	
	// 각 Flag는 예약된 경우 각자에 맞는 경우에 예약을 수행하게 된다;
	// 예약이 수행된 후에는 예약이 없어진다;
	// ( Reserve Action )
	enum EMRESERVED_ACTION
	{
		EMRESERVED_ACTION_CTF_QUIT,				// CTF QUIT 예약;
		EMRESERVED_SIZE
	};

public:	
	GLChar(DWORD MaxClient, GLGaeaServer* pGaeaServer, wb::EM_COUNTRY Country);
	virtual ~GLChar();

private:
	typedef sc::CGLLIST<SFIELDCROW*> FIELDCROWLIST;
	typedef sc::SGLNODE<SFIELDCROW*> FIELDCROWNODE;

	typedef sc::CGLLIST<SFIELDLANDEVENT*> FIELDLANDEVENTLIST;
	typedef sc::SGLNODE<SFIELDLANDEVENT*> FIELDLANDEVENTNODE;

	typedef std::vector<SSEARCHITEMESULT> VECSEARCH_RESULT;
	typedef VECSEARCH_RESULT::iterator	  VECSEARCH_RESULT_ITER;

	typedef boost::function< void() > FunctionVoid;

public:
    __time64_t			m_CurrentTime;

// 	Actor				m_actorMove;
// 	D3DXVECTOR3			m_vDirect;
// 	D3DXVECTOR3			m_vPosition;					//	캐릭터 위치
// 	D3DXVECTOR3			m_vBackupPosition;			//	캐릭터 위치 백업

	DWORD				m_dwLastPostSend;		// Time when the character sent the last post.
	std::set<LONGLONG>	m_setAttachTakenPostID;	//mjeon.인도네시아 아이템 복사문제를 막기위해 FieldServer에서 2차 체크를 수행한다.
												//현재 첨부 아이템과 첨부 게임머니는 동시에 취하게 되어 있으므로 하나의 필터만 사용하면 된다.

	//
	//mjeon.sns
	//
SERIALIZE	SFACEBOOK	m_AUTHFB;				// updated by AgentServer
SERIALIZE	STWITTER	m_AUTHTW;				// updated by AgentServer

	//
	//mjeon.activity
	//

	//
	//this smart-pointer points to an ActivityManager that will alive until the last DbAction(CActivitySave) has done
	//  even though GLChar object that owns it is destroyed.
	//
SERIALIZE	std::tr1::shared_ptr<ActivityManager>	m_pActivityMan;
SERIALIZE	std::tr1::shared_ptr<TitleManager>		m_pTitleMan;

	//
	//AttendanceManager
	//
SERIALIZE	std::tr1::shared_ptr<AttendanceManager>	m_pAttendanceMan;

protected:
    const static float  fLIMIT_TIME_ITEM_SORT;
    const static float fLIMIT_TIME_SUMMON_PET;

protected:
	D3DXVECTOR3		m_vBackupPosition;
	STARGETID			m_TargetID;
	STARGETID			m_sNPCID;				// 최근에 선택된 NPC
	WORD				m_wTARNUM;
	STARID				m_sTARIDS[EMTARGET_NET<<1];	
	DWORD				m_dwANISUBSELECT;
	GLWorkingSkill_s	m_WorkingSkill;			//  현재진행 중인 스킬

	CSkillDistinguish	m_SkillDistinguish;

protected:
	float				m_fPingTraceTimer;			//  mjeon.Add PingTrace timer
	float				m_fIdleTime;				//	휴식 시간.
	float				m_fMBR_TIMER;				//	파티 멤버에게 보내는 갱신 시간.
	float				m_fSTATE_TIMER;				//	상태 정보 갱신 시간.
	float				m_fPERIOD_TIMER;			//	Period 시간 갱신.
	float				m_fQUEST_TIMER;				//	퀘스트 정보 갱신.
	float				m_fattTIMER;				//	공격 프래임.
	float				m_fMoveDelay;				//	이동 딜래이.
	float				m_fITEM_TIMER;				//	아이템 제한시간 점검.
	float				m_fSAVEDB_TIMER;			//	DB 저장 시간.
	float				m_fBRIGHT_TIMER;			//	속성수치 갱신 시간.
	float				m_fSkillDelay;				//  스킬 딜레이(오토공격 방지용)
    float               m_PositionSendTimer;	    // 위치정보 전송
	float				m_fPremiumExitCheckTimer;	// 프리미엄 맵 강제퇴장 체크 시간

	bool				m_bReservedActionFlags[ EMRESERVED_SIZE ];

protected:
	STARGETID			m_sAssault;				//	가장 최근에 자신을 공격한자.
	STARGETID			m_sHeal;					// 가장 최근에 자신을 치유한자.
	LONGLONG			m_lnLastSendExp;
	LONGLONG			m_lnLastSendVNExp;		//  마지막에 보낸 베트남 누적 경험치

	WORD				m_wLastSendCP;			//  마지막으로 보낸 CP

	WORD				m_ItemRemodelPosX;		//	개조카드 사용시 아이템 위치 저장
	WORD				m_ItemRemodelPosY;		
	WORD				m_ItemStampPosX;			// 인장카드 사용시 아이템 위치 저장;
	WORD				m_ItemStampPosY;
	WORD				m_ItemStampFixMax;

private:
	void _SetStampInfo(SINVENITEM* pInvenItem);

protected:
	GLARoundSlot		m_ARoundSlot;			//	피공격시에 자신의 주변에 몰려들수 있는 위치가 있는지 점검.

	DAMAGELOG			m_cDamageLog;			//	대미지 로그.

	WORD				m_wHealingNum;			//	힐링 수.


    // PVE인던용 이동 관련 변수;
    GLStateMiniFSM      m_cMovement;            // 움직임(스킬 포함)

    float				m_fAge;					//	경과 시간.	

	bool				m_bRanMobileRecognized;	// 란 모바일 연동 여부
	int					m_nRanMobilePoint;		// 란 모바일 포인트
	__time64_t			m_tRanMobileRewardDate;	// 란 모바일 일일 보상 받은 날짜

public:
	__time64_t			m_tLoginTime;			//! 접속 시간;
	DWORD				m_dwViewFrame;			//	주위환경 업데이트 프레임.

private:
	//* 직접 변수 접근하여 수정하지 말것! *//
	unsigned int		m_uintViewAround;					//	주위 환경 업데이트 방경. 설정
	float				m_fViewAroundSettingReleaseTimer;   //  혹시 실수로 안풀때를 대비하여 자동으로 풀어버림
	//* 사용했는데 안되는경우 *//
	//1. View Around는 매우 성능에 민감하여 맵이동("맵의 위치가 바꼈을때 퀘스트 실패 여부"와 같음) 과 
	//    안전시간(m_fViewAroundSettingReleaseTimer)을 통해 풀린다. (계속 설정하게 하면 유지된다)
	//2. MAX_VIEWRANGE 보다 작을경우 동작하지 않는다. (의도된 동작임)
public:
	void SetViewAround(unsigned int _ViewAround);
	void SetViewAround(unsigned int _ViewAround,float ViewAroundDuration);
	bool GetViewAroundUsed();
	int GetViewAroundValue() { return m_uintViewAround; }

	// FlyCamera
	void SetFlyCameraUse(const BOOL bUse) { m_bFlyCameraUse = bUse; }
	void SetFlyCameraPos(const D3DXVECTOR3 vPos);
	const D3DXVECTOR3& GetViewPosition(void) const;

	const __time64_t& GetLoginTime(void) const { return m_tLoginTime; }
    
private:
//---------- 케릭터 부활 resurrection -----------;
	
    unsignedState       m_sResurrectAbleState;          // 부활 가능한 방식에 대한 flag;
    __int64             m_lnDeathPenaltyEXP;            // 사망후 부활시 경험치 감소 수치;
    bool                m_bSetDeathPenalty;             // 사망 패널티 설정여부;

    float					m_fForcedResurrectTime;			// 강제 부활 대기 시간;
    SNATIVEID			m_sForcedResurrect_MapID;		// 강제 부활시 부활 맵;
    DWORD				m_dwForcedResurrect_GateID;	// 강제 부활시 부활 지점 게이트ID;
	D3DXVECTOR3		m_vForcedResurrect_position;	// 강제 부활시 부활 지점 게이트ID;
    bool					m_bForcedResurrect_Instance;	// 강제 부활시 부활지점이 인던맵인가? (이 경우 해당 BaseMapID로 인던을 생성,입장 후 부활);
    float					m_fForcedResurrect_Restore;		// 강제 부활시 HP,MP,SP 회복률;

    float               m_fSkillResurrect_Restore;      // 스킬 부활시 HP,MP,SP 회복률;    

    float               m_fGenAge;				        // 부활후 경과 시간.;
private:
    void _sendResurrectMsg(GLLandMan* pLandMan, const float _fValue = 100.0f);  // 케릭터 부활;

public:

	BOOL IsSkillLoopEnd() { return m_WorkingSkill.IsApplyEnd(); }
	BOOL IsSkillEnd( SNATIVEID sSkill ) { return m_WorkingSkill.IsApplyEnd(sSkill); }
	BOOL IsSkillEffectFrame();

	bool IsAlive ();	// 캐릭터가 살아있는가?;
    void SetAlive(const float _fValue = 100.0f);  // 순수하게 케릭터를 되살리는 작업만 수행;

    void ResurrectMoveMapInstance(const SNATIVEID& _moveMapID, const DWORD _dwGateID, const D3DXVECTOR3& _position, const float _fRestore = 100.0f);  //인던 내 맵이동을 통한 부활;
    void ResurrectMoveMap(const SNATIVEID& _moveMapID, const DWORD _dwGateID, const float _fRestore = 100.0f);  //맵이동을 통한 부활;
    void ResurrectStartMap(const float _fValue = 100.0f);  // 시작지점에서 부활;
    void ResurrectCurrentPos(const float _fValue = 100.0f);  // 현재 위치에서 부활;

    void Resurrect(const EMRESURRECTION_TYPE _emResurrectionType);  // 플레이어 부활 (유효성 검사, 스크립트 제어);
	void ResurrectImmediatelyForced();
    void ResurrectProc(const EMRESURRECTION_TYPE _emResurrectionType);  // 실제 플레이어 부활처리;

    //void ResurrectDropChar();

    void SetDefaultResurrection();
    void SetResurrectionNomal();  // 부활 상태 설정 ( 일반적인 상황 );
    void SetResurrectionItem();  // 부활 상태 설정 ( 귀혼주 착용시 );
    void SetResurrectionSkill(const float _fResurrectionValue);  // 부활 상태 설정 ( 스킬 부활 상황 );
    void SetResurrectionForced(const bool _bInstance, const SNATIVEID& _sStartMapID, const DWORD _dwGateID, const D3DXVECTOR3& _position, const float _fForcedResurrectTime, const float _fResurrectionValue = 100.0f );  // 부활 상태 설정 ( 강제 부활 );

    void SetResurrectionDisable(const bool _bDisableSkill);  // 부활 불가능 설정 ( 스킬 부활 가능설정은 SetResurrectionSkillDisable 사용 );
    void SetResurrectionDisableSkill(const bool _bDisableSkill);  // 스킬 부활 불가능 설정 ( 부활 가능설정은 SetResurrectionDisable 사용 );

    void Receive_ResurrectSkill(const float _fResurrectionValue);  // 부활 스킬 적용;

    void UpdateForcedResurrect( const float _fElpasedTime ); // 강제 부활 업데이트;

    void DoDeathPenaltyExp();
    LONGLONG	GetRestorableExp();
    LONGLONG	GetEXPRestoreCost( LONGLONG nReExp );

    BOOL CheckResurrectStartMap();

	virtual const bool IsNonTargetable() const;

//-----------------------------------------------;

protected:
	DWORD				m_ClientSlot;			//	클라이언트 통신 슬롯 

private:
	EMANI_MAINTYPE		m_SKILLMTYPE;
	EMANI_SUBTYPE		m_SKILLSTYPE;

protected:		
	GLItemLimit*		m_pItemLimit;

public:
	MapID			m_mapIDPrevious;			//  전의 맵 ID. 

	sc::SGLNODE<GLChar*>*	m_pCharNodeInGaea;			//	전역 리스트 노드.
	sc::SGLNODE<GLChar*>*	m_pCharNodeInLand;			// 이 캐릭터가 속한	LandMan이 관리하고 있는 캐릭터 관리 Node 중 이 캐릭터의 Node에 대한 포인터

	LANDQUADNODE*		m_pQuadNode;			//	쿼드 트리 노드.( 직접 포인터 형을 지정하기 난감함. )
	sc::SGLNODE<GLChar*>* m_pCellNode;			//	셀 리스트 노드.

	GLTrade				m_sTrade;				//	거래 정보.

public:	// FlyCamera;
	bool				m_bFlyCameraUse;
	D3DXVECTOR3			m_vFlyCameraPos;

protected:
	PrivateMarketField*	m_pPrivateMarket;	//!	개인 상점
    VECSEARCH_RESULT	m_PrivateMarketSearchResult; //! 상점에서 찾은 아이템들 

public:
	SCONFTING			m_sCONFTING;			// 진행 대련 정보.
	
public:
	bool				m_bUsedStorageCard;		// 긴급창고연결카드 사용여부
	// PET
SERIALIZE	DWORD		m_dwPetGUID;			// 팻메모리인덱스
SERIALIZE	DWORD		m_PetDbNum;				//! Pet DB 고유 번호
	bool				m_bGetPetFromDB;		// DB에서 팻의 정보를 가져오는 중인지...

	bool				m_bEntryFailed;			// 진입실패

	bool				m_bEventStart;
	bool				m_bEventApply;

    bool                m_bAcquireEXP;          // GM명령어 /acquire_exp 적용 중 여부

	BYTE				m_ChinaGainType;	    // 중국 탐닉방지 시스템 255: default, 0: 1시간 지났음, 1: 수익 50%, 2: 수익 0%
	CTime				m_China15Time;			// 중국 탐닉방지 시스템  15분 마다 보내지는 시간

	GAIN_TYPE			m_dwVietnamGainType;	    // 베트남 탐닉방지 시스템  255: default, 0: 3시간 지났음 수익 50%, 1: 0%

	//CTime				m_China1Hour;			// 1시간 지났을 때 호출됨

	DWORD				m_dwLastPremiumExitNotifyTime;

protected:
	GLVEHICLE*  m_pVehicle;				// 탈것 구조체

public:
	bool  m_bGetVehicleFromDB; //! true:DB에서 정보 가져오는 중, false:DB에서 정보 가져오지 않음
	float m_fVehicleTimer;
	
	bool m_bItemShopOpen;		// 아이템샵 이용중인지 확인( Japan )
	bool m_bDefenseSkill;		// 발동스킬이 정상적인지 확인

    float m_fLimitTimeItemSort;

    float m_fLimitTimeSummonPet; // pet 소환 시간 제한.

private:
	EMANI_MAINTYPE		m_eMotionMID;			//! 모션 MID
	EMANI_SUBTYPE		m_eMotionSID;			//! 모션 SID
	FLOAT				m_fMotionSec;			//! 모션 플레이 시간 (초)
	FunctionVoid		m_fnOnMotionFinish;		//! 모션이 중단되거나 끝났을때 호출함수

	bool		m_bReferCharPublic;		// 캐릭터 정보 조회 허용
	FLOAT				m_fReferCharDelay;		// 캐릭터 정보 조회 딜레이

    typedef std::map<DWORD, SFIELDLANDEVENT*> LAND_EVENT_MAP;
    typedef LAND_EVENT_MAP::iterator          LAND_EVENT_MAP_ITER;
    typedef LAND_EVENT_MAP::const_iterator    LAND_EVENT_MAP_CITER;
    typedef LAND_EVENT_MAP::value_type        LAND_EVENT_MAP_VALUE;

    LAND_EVENT_MAP m_LandEventMap;

public:
    typedef std::pair<EMCROW, DWORD> CROW_PAIR;
    typedef std::map<CROW_PAIR, SFIELDCROW*> VIEW_CROW_MAP;
    typedef VIEW_CROW_MAP::iterator          VIEW_CROW_MAP_ITER;
    typedef VIEW_CROW_MAP::const_iterator    VIEW_CROW_MAP_CITER;
    typedef VIEW_CROW_MAP::value_type        VIEW_CROW_MAP_VALUE;

private:
	VIEW_CROW_MAP m_ViewCrow;

	EMSERVICE_PROVIDER m_ServiceProvider;

protected:
    wb::EM_COUNTRY m_Country; //! 국가정보

protected:
	GLProductField*		m_pProductField;		//제작 클래스//hsshin

protected:
	CRYPT_KEY m_cKey;	// ClientManager 에 같은 항목이 있으나 지금 사용하지 않고 있고, 로직쪽에서 사용하려고 여기에 추가한다.

protected:
	// Country;
	Country::SCOUNTRY_INFO m_sCountryInfo;
	
	// Change Character Slot CoolTime;
	float				m_fCoolTime_ChangeCharSlot;

public:
	// Country;
	void SetCountryInfo ( const Country::SCOUNTRY_INFO& sCountryInfo ) { m_sCountryInfo = sCountryInfo; }
	const Country::SCOUNTRY_INFO& GetCountryInfo () const { return m_sCountryInfo; }

public:
	void SetCryptKey( CRYPT_KEY cKey ) { m_cKey = cKey; }
	CRYPT_KEY GetCryptKey() { return m_cKey; }

public:
	BOOL m_bUsedPMarketSeacrhCard;	// 개인상점 검색 카드 사용여부

//Rnattendance
public:
	std::vector<DWORD>	m_vecAttendanceSelectQuest; //출석부 과제
	HRESULT GLChar::MsgReqRnattendanceQuestStart ( NET_MSG_GENERIC* nmg );
	HRESULT StartAttendancAdditionalQuest( DWORD dwQUESTID );

public:
	PrivateMarketField* GetPrivateMarket() { return m_pPrivateMarket; }

    void Online();
    void Offline();
	void ReportPositionToAgent(void);

    //! PointShop point
    void PointShopPointSet(const CHARACTER_POINT& Point);
    void PointShopPointDbSave(__int64 PointRefundable, __int64 PointNotRefundable, int nType = 0, __int64 OriPointRefundable = 0, __int64 OriPointNotRefundable = 0, __int64 SubValue = 0 );
    void PointShopPointLog(
        const CHARACTER_POINT& OldPoint,
        const CHARACTER_POINT& NewPoint,
        pointshop::EM_POINT_CHANGE_TYPE Type,
        const SNATIVEID& ItemId,
        __int64 ItemPrice);
    
    //! 캐릭터의 PointShop Point 를 추가한다.
    //! PointRefundable 환불가능 포인트
    //! PointNotRefundable 환불불가 포인트
    bool PointShopPointAdd(__int64 PointRefundable, __int64 PointNotRefundable, pointshop::EM_POINT_CHANGE_TYPE Type);

	//! PointShop 포인트 감소
	void PointShopPointSub( const SNATIVEID& BuyItem, __int64 ItemPrice, int nTraceType = 0 );
	void PointShopPointSubByHoldem( const SNATIVEID& Chip, __int64 ChipPrice );
	void PointShopPointSub( __int64 point, pointshop::EM_POINT_CHANGE_TYPE ePointChangeType, int nTraceType = 0 );
private:
	// 상단의 포인트 감소 함수들을 사용하면 내부적으로 아래의 함수들을 사용해서 포인트 계산, DB 저장 및 캐쉬로 보내서 로그까지 남긴다
    void PointShopPointSub( __int64 point, const SNATIVEID& Item, pointshop::EM_POINT_CHANGE_TYPE ePointChangeType, int nTraceType = 0 );
    CHARACTER_POINT PointShopPointSub( __int64 point, int nTraceType = 0 ); 
public:

	//! PointShop 아이템 구매
    //! 반드시 성공해야 한다. 함수롤 호출하기 전에 무결성을 체크해야 한다.
    void PointShopBuyItem( const SNATIVEID& BuyItem, __int64 ItemPrice, const SITEMCUSTOM& ItemCustom );

    //! 국가정보
    wb::EM_COUNTRY GetCountry() const { return m_Country; }

private:
	// 포인트를 감소 시킬 때 캐시서버의 캐릭터 Point 정보를 갱신한다;
	void PointShopPointUpdateCache( const CHARACTER_POINT& sUpdatePoint );

    bool RegisterViewCrow(EMCROW emCrow, DWORD Id, SFIELDCROW* pCrow);
    //void UnregisterViewCrow(EMCROW emCrow, DWORD Id);

	void SETFIELDLANDEVENT(SFIELDLANDEVENT* pFIELDLANDEVENT);

	void RESETFIELDLANDEVENT( DWORD dwID );

    SFIELDCROW* 		GetViewCrow( EMCROW emCrow, DWORD GaeaId );	
    SFIELDLANDEVENT*	GetLandEvent(DWORD Id);

public:
	void RegisterViewFlyCamChar(EMCROW emCrow, DWORD Id);

public:
    VIEW_CROW_MAP_CITER GetViewCrowBegin();
    VIEW_CROW_MAP_CITER GetViewCrowEnd();

	const BOOL AddFieldCrow(const EMCROW emCrow, const DWORD dwGUID);
	const BOOL AddFieldLandEvent(const DWORD dwGUID);
    void AddGameDbJob(db::DbActionPtr spJob);
	//mjeon.ado
	void AddGameAdoJob(db::DbActionPtr spJob);
	void AddLogAdoJob(db::DbActionPtr spJob);
    void AddShopAdoJob(db::DbActionPtr spJob);

public:	
	SCHARDATA2& GetCharData2()				{ return *this; }

	GLARoundSlot& GetARoundSlot()			{ return m_ARoundSlot; }
	BOOL IsFreeARSlot()					{ return m_ARoundSlot.GetAmount()!=GLARoundSlot::SLOT_SIZE; }
	
    EMSERVICE_PROVIDER GetServiceProvider() const;

public:
	void RESET_TIMELMT_ITEM();
	void RESET_CHECK_ITEM();

	// 아이템 유효성 검사;
	const bool ConfirmItemValidation ();

public:
	HRESULT CreateChar(GLLandMan* pLandMan, D3DXVECTOR3 &vPos, PCHARDATA2 pCharData, BOOL bNEW);
	BOOL	InitSerializedChar(GLLandMan* pLandMan, BOOL bNEW);
	HRESULT SetNavi(boost::shared_ptr<NavigationMesh> spNavi, const D3DXVECTOR3 &vPos);
	HRESULT SetData(PCHARDATA2 pCharData, BOOL bNEW);

	HRESULT SavePosition();
	HRESULT SaveLastCall();
	HRESULT SaveSkillFact();

	HRESULT LoadStorageFromDB();
	
public:
	LONGLONG AddInvenMoneyServer( LONGLONG llMoney, int nCause = INVEN_MONEY_DEFAULT );
	LONGLONG SubtractInvenMoneyServer( LONGLONG llMoney, int nCause = INVEN_MONEY_DEFAULT );
	void MoneyNotify( LONGLONG OldMoney, LONGLONG NewMoney, int nCause );

public:
	
	inline BOOL IsRunning() const						{ return isAction(GLAT_MOVE) && IsActState(EM_ACT_RUN); };

	const BOOL IsFeignDieDetect() const;

    void ResetLimitTimeItemSort()    { m_fLimitTimeItemSort = fLIMIT_TIME_ITEM_SORT; }
    float GetRemainTimeItemSort()   { return m_fLimitTimeItemSort; }

    void ResetLimitTimeSummonPet() { m_fLimitTimeSummonPet = fLIMIT_TIME_SUMMON_PET; }
    float GetRemainTimeSummonPet() { return m_fLimitTimeSummonPet; }

	bool IsProducting();	
	GLProductField* GetProduct() { return m_pProductField; }	//hsshin

public:
	BOOL IsVaildTradeData();
	BOOL IsVaildTradeInvenSpace( GLInventory &sTradeBox );
	BOOL DoTrade( GLTrade& sTarTrade, DWORD TarCharDbNum );

public:
	void RemoveAllPlayHostiles();
	void RemovePlayHostile( DWORD dwCharID );
	void RemovePlayHostileEachOther( DWORD dwCharID );
	void RemovePlayHostileClub( DWORD dwClubID );

public:
	//void SummonAllRemove();
	//void SummonAllKill  ();

    void SummonGateAllRemove();

public:
    void RegisterLandEvent(const DWORD dwGUID)    { m_dwLandEventList.insert(m_dwLandEventList.end(), dwGUID); }
    void RemoveLandEvent  (const DWORD dwGUID)    { m_dwLandEventList.remove(dwGUID); }

public:
	virtual float GetMoveVelo();

public:
	virtual void TurnAction(const EMACTIONTYPE toAction, const bool bMsg = true) override;

	virtual void SetAction ( const EMACTIONTYPE emAction ) override;

public:
	void ResetAction() { TurnAction( GLAT_IDLE ); }

	void SetMotion( EMANI_MAINTYPE eMID, EMANI_SUBTYPE eSID, FLOAT fSec, FunctionVoid fnCallBack );
    const bool IsMotion(EMANI_MAINTYPE eMID, EMANI_SUBTYPE eSID) const;
    void EventMotionFinish();

	void SendMsgReferChar( bool bToClient, DWORD nRequestClientSlot, DWORD nRequestCharDbNum );

	void SetReferCharPublic( bool bPublic ) { m_bReferCharPublic = bPublic; }
	bool IsReferCharPublic() { return m_bReferCharPublic; }

	void SetReferCharDelay( FLOAT fDelay ) { m_fReferCharDelay = fDelay; }
	FLOAT GetReferCharDelay() { return m_fReferCharDelay; }
	
public:
	void ResetTargetID(void) { this->m_TargetID = STARGETID(); }
	virtual const STARGETID&	GetTargetID()	const		{ return m_TargetID; }
	const STARID*		GetTarIds()		const		{ return m_sTARIDS; }
	EMANI_SUBTYPE		GetAniSubType();

public:	
	void ResetCP()					 { m_sCP.SetData( 0, MAX_CP ); }    

public:
	DWORD SELECT_SKILLSLOT( SNATIVEID skill_id, EMSKILLFACT_TYPE emSkillFactType );

	virtual void SetGaeaID(const DWORD dwGaeaID) override
	{
		m_dwGlobalID = dwGaeaID; // GLCROWLOGIC에서 사용 중이라 일단 남겨둠; // 최종적으로는 제거해야함;
		m_dwGaeaID = dwGaeaID;
	}	
	virtual const DWORD GetCharID() const	override{ return m_CharDbNum; }
	virtual WORD GetSchool() const	{ return m_wSchool; }	
	
	virtual BOOL IsCtrlBlockBody() const;
	virtual BOOL IsActionLimit( DWORD dwLimit ) const { return m_dwActionLimit&dwLimit; } 
	virtual bool IsSafeTime() const				    { return( IsBasiceSafeTime() || IsCDMSafeTime() ); }
	virtual bool IsBasiceSafeTime() const			{ return m_fMoveDelay > 0.0f; }
	virtual bool IsCDMSafeTime() const				{ return m_fCDMSafeTime > 0.0f; }
	virtual bool IsSafeZone() const				    { return m_bSafeZone; }

	virtual bool IsCheckedSkillFlagSpecial ( DWORD dwFlag )	const;
	
	virtual void SetPosition(const D3DXVECTOR3& vPosition) override;

	virtual WORD GetBodyRadius() const				{ return GETBODYRADIUS(); }
	virtual GLARoundSlot* GETSLOT()			    	{ return &m_ARoundSlot; }
	virtual SCONFTING* GETCONFTING()				{ return &m_sCONFTING; }
    void    EndConfting();

	virtual DWORD GETHOLDBLOW() const				{ return m_dwHOLDBLOW; }
	virtual WORD GetLevel() const					{ return GETLEVEL(); }
	virtual const WORD	GetGradeAttack(void) const override;

	virtual DWORD GetNowHP() const					{ return GETHP(); }
	virtual DWORD GetMaxHP() const					{ return GETMAXHP(); }
    virtual float GetMaxHpRate() const				{ return GETMAXHPRATE(); }
    virtual void  SetMaxHpRate( float fRate )       {  SETMAXHPRATE( fRate ); }

	virtual WORD GetNowMP() const					{ return static_cast<WORD>(GETMP()); };
	virtual WORD GetMaxMP() const					{ return static_cast<WORD>(GETMAXMP()); };

	virtual WORD GetNowSP() const					{ return static_cast<WORD>(GETSP()); };
	virtual WORD GetMaxSP() const					{ return static_cast<WORD>(GETMAXSP()); };


	virtual WORD GetMaxLevel();

	virtual int				GetBasicOffenceHigh() const			{ return GETFORCE_HIGH(); }
	virtual int				GetBasicOffenceLow() const			{ return GETFORCE_LOW(); }
	virtual int				GetBasicDefence() const				{ return GETDEFENSE(); }
	virtual const SRESIST&	GetBasicRegist() const				{ return m_sSUMRESIST_SKILL; }
	virtual int				GetBasicHitRate() const				{ return GETHIT(); }
	virtual int				GetBasicAvoid()	const				{ return GETAVOID(); }

	virtual float			GetBasicHPRestorationRate() const	{ return 0.f; }
	virtual float			GetBasicMPRestorationRate() const	{ return 0.f; }
	virtual float			GetBasicSPRestorationRate() const	{ return 0.f; }

	virtual int				GetOffenceHigh() const				{ return GETFORCE_HIGH(); }
	virtual int				GetOffenceLow() const				{ return GETFORCE_LOW(); }
	virtual int				GetDefense() const					{ return GETDEFENSE(); }
	virtual const SRESIST&	GETRESIST() const					{ return m_sSUMRESIST_SKILL; }
	virtual int				GetHitRate() const					{ return GETHIT(); }
	virtual int				GetAvoid() const					{ return GETAVOID(); }
	virtual float			GetHPRestorationRate() const		{ return 0.f; }
	virtual float			GetMPRestorationRate() const		{ return 0.f; }
	virtual float			GetSPRestorationRate() const		{ return 0.f; }

	virtual const float		GetMultipleHP() const				{ return 1.f; }
	virtual const float		GetMultipleAttack() const			{ return 1.f; }
	virtual const float		GetMultipleDefense() const			{ return 1.f; }
	virtual const float		GetMultipleExp() const				{ return 1.f; }
	virtual const float		GetMultipleDrop() const				{ return 1.f; }

	virtual int GetBodyDefense() const				{ return m_nDEFENSE_BODY; }
	virtual int GetItemDefense() const				{ return m_sSUMITEM.nDefense; }

	virtual float GetIgnoreBossDamageR() const		{ return m_sSUMITEM.fIncR_Ignore_BossDamage; }
	virtual int GetBossFixedDamage() const			{ return m_sSUMITEM.nInc_BossFixedDamage; }
	virtual float GetDecRDamage_EmergencyState() const	{ return m_sSUMITEM.fDecR_Damage_EmergencyState; }
	virtual float GetIncRCritical_EmergencyMob() const	{ return m_sSUMITEM.fIncR_Critical_EmergencyMob; }
	virtual float GetIncRCritical_EmergencyBoss()const	{ return m_sSUMITEM.fIncR_Critical_EmergencyBoss; }
	virtual float GetIncR_ExpAddEffect() const			{ return m_sSUMITEM.fIncR_ExpAddEffect; }
	virtual float GetIncR_DropGenMoney() const			{ return m_sSUMITEM.fIncR_DropGenMoney; }
	virtual float GetDecR_DamageMeleeSkill() const	{ return m_sSUMITEM.fDecR_Damage_MeleeSkill; }
	virtual float GetDecR_DamageRangeSkill() const	{ return m_sSUMITEM.fDecR_Damage_RangeSkill; }
	virtual float GetDecR_DamageMagicSkill() const	{ return m_sSUMITEM.fDecR_Damage_MagicSkill; }
	virtual DWORD GetBonusExp() const;

	virtual EMBRIGHT GetBright() const				{ return GETBRIGHT(); }

	virtual bool	 		MoveTo( const D3DXVECTOR3* pTarPos, bool bJump, DWORD dwFlags );
	void					ReactDamage( ServerActor* pAttacker, DWORD dwDamageFlags );

	virtual const DWORD ReceiveDamage( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag ) override;
    const DWORD ReceiveDamage_Procedure( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag );

    virtual const DWORD ReceiveHeal(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag) override;
    const DWORD ReceiveHeal_Procedure(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag);

	virtual void SendPushPull( const D3DXVECTOR3 &vMovePos, const float fPUSH_SPEED, const DWORD dwAniType, const bool bReActor = true );

	virtual float GETSTATE_DAMAGE() const			{ return m_fSTATE_DAMAGE; }
	
	virtual const DAMAGE_SPEC& GetDamageSpec() const	{ return m_sDamageSpec;}
	virtual DAMAGE_SPEC* GetDamageSpecPtr()				{ return &m_sDamageSpec;}
	virtual DEFENSE_SKILL GetDefenseSkill() const	{ return m_sDefenseSkill; }
	virtual SINCREASEEFF* const GetIncreaseEff() { return &m_sIncreaseEff; }
	virtual SIGNORED_DAMAGE* const GetIgnoredDamage(void) override { return &m_sIgnoredDamage; }
	virtual const EMSPEC_DOMINATE_TYPE GetController(void) const { return m_sDOMINATE.emController; }
	virtual void	SetController(const EMSPEC_DOMINATE_TYPE emDOMINATION_TYPE){m_sDOMINATE.emController = emDOMINATION_TYPE; }

    virtual void HOLD_ITEM(const SITEMCUSTOM& sItemCustom) override;

	virtual void End_FeignDeath( BOOL bForce );

public:
	/// 현재는 사용하지 않는다, 후에 변경가능해져야 한다면 이용한다;
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

	virtual void			SetMultipleHP ( float fMultiple ) { }
	virtual void			SetMultipleAttack ( float fMultiple ) { }
	virtual void			SetMultipleDefense ( float fMultiple ) { }
	virtual void			SetMultipleExp ( float fMultiple ) { }
	virtual void			SetMultipleDrop ( float fMultiple ) { }

private:
	/// 현재는 사용하지 않는다, 후에 변경가능해져야 한다면 이용한다;
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

public:
    // bExp_ShowUser를 통해서 GM이 아니더라도 경험치를 채팅창에 표시할 수 있다.
	// - 경험치, 유저에게 알림, 경험치 추가획득 효과 적용 여부;
	void ReceiveExp( float fGenExp, bool bExp_ShowUser, bool bExp_Addition = true); 
    void ReceiveExpApply( float fGenExp, bool bExp_ShowUser = false, bool bExp_Addition = true );    // 무조건 경험치를 받는다.
	// 이벤트나 탐닉 방지 시스템에 의해 변경된 경험치를 구한다.
	int	 CalculateReceveExp( int nGenExp );

public:
	void ReceiveKillExp( const STARGETID &cTargetID, bool bConftSchool=false, bool bSummonReceive=false, bool bBoss=false );
		void ReceiveKillExpParty(const STARGETID &TargetID, const bool bConftSchool, bool bBoss=false);
	void ReceiveAttackExp( const STARGETID &cTargetID, DWORD dwDamage, BOOL bPartySkill, bool bConftSchool=false, bool bSummonReceive=false, bool bBoss=false );
	void ReceiveKillCP( const STARGETID &cTargetID );
	void ReceiveCP( const STARGETID &cTargetID, const WORD wRecvCP );
	void ReceivePartyExp( const float fGenExp, const STARGETID &TargetID );
	void ReceiveLivingPoint(const int nLP);
	BOOL ReceiveQItemFact( const SNATIVEID& nidITEM );
	void ReceiveEventEx( const EMGM_EVENT_TYPE emType, const WORD wValue );
	void ReceiveBeginEvent( const SEventState emType );
	void ReceiveVietnam( const __time64_t initTime );
    void ReceiveIPEvent( bool _Enable );

public:
    //! 클라이언트 통신 슬롯
    __forceinline DWORD ClientSlot() const { return m_ClientSlot; }
    __forceinline void  SetClientSlot(DWORD Slot) { m_ClientSlot = Slot; }

	virtual DWORD        GETCLIENTID()			{ return m_ClientSlot; }
	virtual SSKILLFACT* GETSKILLFACT() override { return m_sSKILLFACT; }
	virtual int GETSKILLFACTSIZE() const override { return SKILLFACT_SIZE; }

	virtual bool STATEBLOW( const SSTATEBLOW &sStateBlow );
	virtual void CURE_STATEBLOW( DWORD dwCUREFLAG );
	virtual void SKILL_EFFECT_REMOVE( DWORD dwRemoveFlag );
	virtual void DO_STUN_ACTION();
    virtual void VAR_BODY_POINT(
        const EMCROW emActorCrow, 
        const DWORD dwActorID, 
        const DWORD dwVarTypeFlag, 
        int& nVar_HP,
        int& nVar_MP, 
        int& nVar_SP ) override;

	virtual void	UPDATE_DATA_END(const DWORD dwSkillfactIndex, BOOL bTimeDel = FALSE, BOOL bForceEnd = FALSE, BOOL bOverLabEnd = FALSE );
	virtual void	UPDATE_DATA_Additional(const float fElapsedTime);
		void UPDATE_DATA_Additional_Stigma(SSKILLFACT& sSKEFF, const DWORD dwStigmaIndex, const DWORD dwSkillfactIndex);
		void UPDATE_DATA_Additional_Domination(SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex);
        void UPDATE_DATA_Additional_Taunt(SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex);
		void UPDATE_DATA_Additional_Provoke(SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex);

	void RestoreActStateByInvisible(); // 상태 전환 및 스킬 관련 정리와 함께 정리 해야 함;
	virtual void RECEIVE_DAMAGE_AFTER(void);

	void AddSkillFact_SpecialAddon_Server(const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF);
	void RECEIVE_SKILLFACT_SpecialProvocation( const STARGETID& sID, const float fDistance );			
	const bool RECEIVE_SKILLFACT_Domination(const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel);
	void RECEIVE_SKILLFACT_Invisible(const float fVAR2);
	void RECEIVE_SKILLFACT_Stigma(const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel);
	void RECEIVE_SKILLFACT_RandomExpRate(const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const float fLife, const WORD wLevel, SSKILLFACT& sSKILLEF, const float* const pLaunchTime, const float* const pLaunchValue);
	void RECEIVE_SKILLFACT_ReleasePet(void);
	const BOOL RECEIVE_SKILLFACT_Special( PGLSKILL pSkill, const SNATIVEID& sSkillID, const WORD wlevel, const DWORD dwCount, SSKILLFACT& sSKILLEF, const STARGETID sID, const float* const pLaunchTime , const float* const pLaunchValue );
	const BOOL RECEIVE_SKILLFACT_Addon( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF );
	const BOOL RECEIVE_SKILLFACT_Basic( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF );

	const bool RECEIVE_SKILLFACT_ScreenThrow(const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel);
	const bool RECEIVE_SKILLFACT_EntranceExit( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID );
	const bool RECEIVE_SKILLFACT_FeignDeath( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID );
	const bool RECEIVE_SKILLFACT_CameraHit(const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel);
	const bool RECEIVE_SKILLFACT_Provoke(const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID);
	
	virtual const BOOL RECEIVE_SKILLFACT(
        const SNATIVEID& skill_id,
        const WORD wlevel,
        DWORD& dwSELECT,
        const STARGETID& sID,
        const SSKILLFACT::STIME sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME),
        const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT,
        EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NORMAL,
		const float* const pLaunchTime = 0, const float* const pLaunchValue = 0) override;

	virtual const BOOL RECEIVE_SKILLFACTII(
		const SNATIVEID& skill_id,
		const WORD wlevel,
		DWORD& dwSELECT,
		const STARGETID& sID,
		const SSKILLFACT::STIME sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME),
		const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT,
		EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NORMAL,
		const float* const pLaunchTime = 0, const float* const pLaunchValue = 0) override;

	virtual const BOOL RECEIVE_SKILLCA(
		 const SSKILLACTEX &sACTEX ) override;

    virtual const BOOL REMOVE_SKILLFACT(const SNATIVEID& skill_id) override;
    virtual const BOOL REMOVE_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON) override;

    virtual BOOL RESET_SKILLFACT( const SNATIVEID& skill_id, DWORD& dwOutSlot, EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NONE ) override;

	virtual BOOL RECEIVE_DRUGFACT( EMITEM_DRUG emDRUG, WORD wCURE, BOOL bRATIO );

public:
	DWORD ToDamage(const STARGETID &sTargetID, const int nDamage, const DWORD dwDamageFlag);
	DWORD ToHeal(const STARGETID& sTargetID, const int nHeal, const DWORD dwHealFlag);
	
	void NotifyKillSomeone(DWORD dwGaeaID);

public:
	void DoFalling();

private:
	//void GenerateReward( bool bCONFT=false, bool bCONFT_SCHOOL=false );	
    void DoFalling_Proc();
    void DoFalling_Proc_Confront();

    void DoFalling_Proc_By_PVP();  // 플레이어에 의해 죽었을 때에 대한 처리;
    void DoFalling_Proc_Log();  // 죽었을 때에 로그 남기는 처리;

    void DieProc();  // 실제 죽는 행위에 대한 처리;
    void DieProc_CDM();

    void DeathPenaltyDefault();  // 사망시 패널티 처리 (경험치 하락, 아이템 드랍);

public:
    void DeathPenalty(const unsigned int _nPenaltyExp, const unsigned int _nDropItem, const float _fDropRate);
    void SetDeathPenalty_Exp(const __int64 _penaltyExp = 0);  // 사망시 경험치 하락치 설정;
    void RestoreDeathPenalty_Exp(const __int64 _penaltyExp);

public:
	bool IsConflictTarget( ServerActor* pTARGET );
    const EMREACTIONABLE IsReActionable ( ServerActor* pTARGET, const EMIMPACT_SIDE _emTargetSide);
    void ReActionByForcedAction( ServerActor* const pTarget, const EMIMPACT_SIDE _emTargetSide );

	// 소환수에서 타겟과의 팩션 체크가 필요하면 주인 팩션을 기준으로 체크한다;
	// 소환수 이외에선 사용하지말자;
    const EMREACTIONABLE IsReActionableFaction ( ServerActor* pTARGET, const EMIMPACT_SIDE bTargetSide );
    const EMREACTIONABLE IsReActionableNonFaction ( ServerActor* pTARGET, const EMIMPACT_SIDE bTargetSide );    

public:
	void AddPlayHostileEachOther( const DWORD dwAttackedCharID, const BOOL bClubBattle, const float fTime );
	void AddPlayHostile( const DWORD dwCHARID, const BOOL bBAD, const BOOL bClubBattle, const float fTime );

private:
	void StigmaProc_Skill(const SSKILLACT* pSkillAct, BOOL bStigmaEffect = FALSE);
	void StigmaProc_Normal(const bool bBroad);
	void StigmaProc_SendMessage(const STARGETID& sTargetID, const float fRate, const bool bBroad);	

private:
	//	공격 프로세스.
	BOOL	AttackProcess( float fElapsedTime );
	void	AvoidProc(const SNATIVEID& skill_id=NATIVEID_NULL(), const WORD wSkillLevel = 1, const BOOL bLowSP=FALSE );
	void	DamageProc( int nDAMAGE, DWORD dwDamageFlag, const BOOL bLowSP );	
	void	Damage_SendMessage(const int nDAMAGE, const DWORD dwDamageFlag, const STARGETID& sTargetID, const bool bBroad);
	void	Heal_SendMessage(const int nHEAL, const DWORD dwHealFlag, const STARGETID& sTargetID, const bool bBroad);
	void	BlockProc ( const STARGETID& sTargetID, const SNATIVEID& skill_id=NATIVEID_NULL(), const WORD wSkillLevel = 1, const BOOL bLowSP=FALSE );

public:
	virtual void DamageReflectionProc(int nDAMAGE, const STARGETID& sACTOR) override;
	virtual void DamageCurse(int nDAMAGE, const STARGETID& sACTOR, EMCROW emCrow) override;
	virtual void DefenseSkill(const SNATIVEID& sNativeID, WORD wLevel, const STARGETID& sACTOR) override;

	virtual const bool IsImmuneToSkill ( BYTE iIndex ) const;

    /**
        이동 가능 유무를 설정 할 수 있다.
        단순 이동 뿐 아니라 스킬도 사용하지 못하는 상태이다.
        true : 이동 가능, false : 이동 불가능

     */
    const CharState IsMovement() const { return m_cMovement.GetState(); }
    void SetMovement(const CharStateID id, const CharState state);

    /**
        상태를 설정 한다.
        중복 코드를 방지 한다.

     */
    void SetState(const CharStateID id, const CharState state, GLStateMiniFSM& fsm);

    /**
        서버의 캐릭터 상태를 클라이언트와 동기화 맞춘다.
        이것은 위의 부활과 이동에 공통으로 사용 된다.
        향후 상태 추가시에도 사용할 수 있다.

     */
    void SyncClientState(const CharStateIndex idx, const CharState state);

    /**
        모든 데미지 및 힐이 통과 한다.
        일반, 스킬, 소환수 데미지가 모두 이곳을 통과 한다.
        현재 이벤트에 의한 데미지 계산시 사용 된다.
        향후 이와 유사하게 필요하다면 사용 가능 하다.
        CTF 에 의해 유사코드가 만들어졌지만 너무 CTF에 하드코딩되어 있어 새로 만든다.

     */
    void ToAllDamage_PVE(const DWORD dwDamage);
    void ToAllHeal_PVE(const DWORD dwHeal);

    /**
        죽은 캐릭터를 현재 위치에 살려 준다.
        경험치 손실 없이 제자리 부활 시킬 경우 활용 된다.

     */
    void SetAlive_PVE();

	void SetMoveDelay(const float delay) { m_fMoveDelay = delay; } 
	const float GetMoveDelay() const { return m_fMoveDelay; }

	// Summon Crow;
	virtual void SummonCrow (
		const SNATIVEID& sFamiliarID,
		float fRemainedTime = 0.f,
		DWORD dwFamiliarType = 0,
		bool bEventSkill = true,
		DWORD dwMaxSummon = 1,
		WORD wSkillLevel = 0 );
	virtual void ChangeSummonSkillPage ( UINT iPage, SNATIVEID sSkillID );
	virtual void UseSummonEventSkill ( UINT iIdx, SNATIVEID sSkillID );
	virtual void HireBasicSummon ( WORD wMainID, WORD wSubID );
	virtual void SummonTargeting ( const STARGETID& sTARID, SNATIVEID sSkillID );
	virtual void SummonRunAway ( SNATIVEID sSkillID );

	//	스킬 프로세스.	
	BOOL	CheckSkillToStart( const SNATIVEID& idSkill, EMSKILLCHECK& sSkill );
	BOOL	GetSkillRange( const SNATIVEID& idSkill, float& fRange );
	
	SANIATTACK*		GetAniAttack( EMANI_MAINTYPE eSkillMType, EMANI_SUBTYPE eSkillSType );
	SANIATTACK*		GetAniAttack();

	CEVENT_NODE_SERVER_SUMMONGATE* GetSummonEventInfo( PGLSKILL pSkill, const SCHARSKILL& sSkill, const D3DXVECTOR3& vEventPos);
	void 	SkillSummon( PGLSKILL pSkill, const SCHARSKILL& sSkill );
	void	SkillApplyProc_Land( PGLSKILL pSkill, const WORD wSKILL_LVL, const SCHARSKILL& sSkill );
	void 	SkillBasicProc_Land( PGLSKILL pSkill, const SCHARSKILL& sSkill );
	void	SkillBasicProc( PGLSKILL pSkill, const SCHARSKILL& sSkill, const WORD& wTARNUM, const STARID* sTARIDS );
	void	ApplySkillTarget_HP( PGLSKILL pSkill, WORD wSKILL_LVL, SKILL::CDATA_LVL& sSKILL_DATA, ServerActor* pACTOR, WORD	wAttackDivCount, const STARGETID&	sTARID, int& nHP, DWORD& dwDamageFlag );
	void	ApplySkillTarget_MP( PGLSKILL pSkill, SKILL::CDATA_LVL& sSKILL_DATA, ServerActor* pACTOR, int& nMP );
	void	ApplySkillTarget_SP( PGLSKILL pSkill, SKILL::CDATA_LVL& sSKILL_DATA, ServerActor* pACTOR, int& nMP );
	void	ApplySkillTarget_SpecToMob( const EMSPEC_ADDON& emSpec, const SKILL::SSPEC& sSKILL_SPEC, ServerActor* pACTOR, DWORD& dwDamageFlag, int* nGather, int* Vars );
	void	ApplySkillTarget_SpecToMobElse( const EMSPEC_ADDON& emSpec, const SKILL::SSPEC& sSKILL_SPEC, ServerActor* pACTOR, DWORD& dwDamageFlag, int* nGather, int* Vars );
	void	ApplySkillTarget_Spec( PGLSKILL pSkill, WORD wSKILL_LVL, ServerActor* pACTOR, DWORD& dwDamageFlag, int* nGather, int* Vars );
	void	ApplySkillSummons( PGLSKILL pSkill, const SCHARSKILL& sSkill, const STARID& _sTARID, ServerActor* pACTOR );
	void	ApplySkillTarget_Blow( PGLSKILL pSkill, const WORD wSKILL_LVL, ServerActor*	pACTOR, SSKILLACTEX& sSKILLACTEX );
	BOOL	ApplySkillTarget( PGLSKILL pSkill, const SCHARSKILL& sSkill, WORD wAttackDivCount, SKILL::CDATA_LVL& sSKILL_DATA, BOOL bLowSP, const STARGETID& sSelectTargetID, const WORD& wTARNUM, const STARID* sTARIDS );
	void	ApplySkillTarget_Special( PGLSKILL pSkill, const SCHARSKILL& sSkill, const STARGETID& sSelectTARID, const STARGETID& sTARID, ServerActor* pTarget, BOOL& bCrushBlowOne, SSKILLACTEX& sSKILLACTEX, BOOL& bStigmaEffect  );
	void	ApplySkillTarget_CASpecial( PGLSKILL pSkill, const SCHARSKILL& sSkill, const STARGETID& sSelectTARID, const STARGETID& sTARID, ServerActor* pTarget, BOOL& bCrushBlowOne, SSKILLACTEX& sSKILLACTEX, SSKILLACTEX& sCASKILLACTEX, BOOL& bStigmaEffect );

	bool	GetPullSpeed( ServerActor* pTarget, const SKILL::SSPEC& sSKILL_SPEC, float& fSpeed, float& fDist );
	void	ApplySkillTargetCrushingBlow( const SKILL::SSPEC& sSKILL_SPEC, ServerActor* pACTOR, BOOL& bCrushBlowOne, SSKILLACTEX& sSKILLACTEX );
	void	ApplySkillTargetRebirth( ServerActor* pTarget, const SKILL::SSPEC& sSKILL_SPEC );
	void	ApplySkillTargetStun( ServerActor* pACTOR );	
	void	ApplySkillTargetStigma(const STARGETID& sTARID, const SKILL::SSPEC& sSPEC, const PGLSKILL pSkill);
	void	ApplySkillTargetSkillLink(const STARGETID& sSelectTARID, const STARGETID& sTARGETID, const SKILL::SSPEC& sSPEC, const SCHARSKILL& sSkill);
	
	void	ApplySkillSelf_Special( PGLSKILL pSkill, const WORD wSKILL_LVL, const STARGETID& TargetID, const WORD& wTARNUM, const STARID* sTARIDS );
	void	ApplySkillSelf( PGLSKILL pSkill, const WORD wSKILL_LVL, const STARGETID& TargetID, const WORD& wTARNUM, const STARID* sTARIDS );

	BOOL	SkillProcess( float fElapsedTime );
	BOOL	SkillProc( const SCHARSKILL& sSkill, BOOL bLowSP, const STARGETID& TargetID, WORD wTARNUM, const STARID* sTARIDS, WORD wAttackDiv );
	void	HitDamage( const SSTRIKE& sStrike );
	BOOL	ApplySkill( const SCHARSKILL& sSkill, const SSTRIKE& sStrike, const STARGETID& TargetID, const WORD& wTARNUM, const STARID* sTARIDS, WORD wAttackDiv );
	void	ApplySkillDealy( const SCHARSKILL& sSkill, const STARGETID& TargetID, const WORD& wTARNUM, const STARID* sTARIDS, WORD wAttackDiv );
	void	DelayActionSkillStart( STARGETID sCasterID, SNATIVEID sSkillID, WORD wLevel );
	void	InvokeActionSkillStart( STARGETID sTargetID, SNATIVEID sSkillID, WORD wLevel  );

	void	SelfSkillStart( SNATIVEID sSkillID, WORD wLevel );

	void	ACCOUNTBIKESKILL( SNATIVEID skill_id, WORD wStrikeNum, bool bServer = false );

	//	쇼크 프로세스.
	BOOL	ShockProcess( float fElapsedTime );

	BOOL	ApplyItemSkillRun( const SCHARSKILL& sSkill, PGLSKILL pSkill, const STARGETID& TargetID, const WORD& wTARNUM, const STARID* sTARIDS, std::vector<sItemSkill>& info );

public:
    virtual BOOL CHECK_ACTIVITY_POINT( const SITEM& sItem );
	virtual BOOL CHECK_CONTRIBUTION_POINT( const SITEM& sItem );

public:
	HRESULT GetViewAround();
    void GetViewAroundLandEvent(GLLandNode * pLandNode);
    void GetViewAroundMoney(GLLandNode * pLandNode);
    void GetViewAroundItem(GLLandNode * pLandNode);
    void GetViewAroundMaterial(GLLandNode * pLandNode);
    void GetViewAroundCrow(GLLandNode * pLandNode);
    void GetViewAroundChar(GLLandNode * pLandNode);
    void GetViewAroundPet(GLLandNode * pLandNode);
    void GetViewAroundSummon(GLLandNode * pLandNode);

    HRESULT UpdateViewAround();
    void UpdateViewAroundLandEvent(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect);
    void UpdateViewAroundMoney(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect);
    void UpdateViewAroundItem(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect);
    void UpdateViewAroundMaterial(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect);
    void UpdateViewAroundCrow(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect);
    void UpdateViewAroundChar(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect, const BOUDRECT* pRectOnlySee);
    void UpdateViewAroundPet(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect);
    void UpdateViewAroundSummon(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect);

    HRESULT ResetViewAround();
	void ActiveSkillClear() {m_WorkingSkill.Clear();}

	HRESULT UpdateDurDamage( float fTime, float fElapsedTime );	//	지속 데미지 처리( 데미지 준다. ) 

	//! 게임머니가 업데이트 되었을 경우 체크하는 변수
	BOOL CheckMoneyUpdate(const LONGLONG lnCurrentMoney, const LONGLONG lnUpdateMoney, const bool bPlus, const char* msg);

    //! 게임머니(인벤토리) 보유량을 알림
    void AddInvenMoneyAndUpdate( LONGLONG llMoney, int nCause );
    void SubtractInvenMoneyAndUpdate( LONGLONG llMoney, int nCause );
    void SendInvenMoneyUpdate();
    //! 게임머니를 변화시키고 그 사실을 클라이언트에 획득 통보함
    void AddInvenMoneyAndPickupMessage( LONGLONG llGetMoney, int nCause );

	// 아이템 시간 체크
	// 사용되는 부분 확인하니 아이템 원본을 레퍼런스로 넘기고 있다. 함수안에서 실제 아이템도 삭제되고 있다.
	// 코드안에 코스튬 확인하는 부분위에 아이템 삭제 코드가 있는데, 실제 삭제되면 코스튬 확인하는 부분부터 아무 소용이 없게된다.
	// 함수 안에서 원본 아이템을 복사해서 사용하도록 수정한다.
    HRESULT CheckItemDuration( SINVENITEM& sInvenItemOriginal );

public:
	HRESULT MsgGameJoin();
	//Rnattendance
	bool ResetRnattendanceQuest(GLQUESTPROG* pQUEST_PROG, GLQuestPlay::MAPQUEST_ITER& Iter);
	bool IsPlayRnattendanceQuest( GLQUESTPROG* pQUEST_PROG );

    //void MsgGameJoinSendClubInfo();
    //HRESULT MsgClubInfo();

	//HRESULT MsgClubStorage();
	HRESULT MsgFieldMoveReset();

	BOOL IsInViewAround( DWORD dwGAEAID );

    DWORD GetMaxClient() const;
    bool IsBRIGHTEVENT() const;
    bool SchoolFreePkOn() const;
    bool IsPKServer() const;
    bool IsReserveServerStop() const;

    SFIELDCROW* NEW_FIELDCROW();
    void        RELEASE_FIELDCROW(SFIELDCROW* pFIELDCROW);

    int   GetServerChannel() const;
    DWORD GetFieldSvrID() const;

    ServerActor* GetTarget(const STARGETID& sTargetID);
    ServerActor* GetTarget(const GLLandMan* pLandMan, const STARGETID& sTargetID);

	GLChar* GetChar64(DWORD dwSvrGroupNum, DWORD dwChaNum);
    GLChar* GetChar(DWORD dwGaeaId);
    GLChar* GetCharByDbNum(DWORD ChaDbNum);

	virtual void SetPartyID(const GLPartyID& PartyID);
	virtual void ResetPartyID(void);
    GLPartyField* const GetMyParty(void) const;
    GLPartyField* const GetParty(const DWORD dwPartyID) const;
	GLPartyField* const GetParty(const GLPartyID& PartyID) const;

    //! Pet -------------------------------------------------------------------
    GLPetField* GetMyPet();
    GLPetField* GetPet(DWORD PetId);
    //! Pet DB 고유 번호
    inline DWORD GetPetDbNum() const { return m_PetDbNum; }
    inline void SetPetDbNum(DWORD PetDbNum) { m_PetDbNum=PetDbNum; }

    std::tr1::shared_ptr<GLClubField> GetMyClub();
    std::tr1::shared_ptr<GLClubField> GetClub(DWORD ClubDbNum);

	//! 주변 케릭에 전송할때
	bool CheckBelongViewAround( GLChar* pTarChar );
	virtual HRESULT SendMsgViewAround(NET_MSG_GENERIC* nmg);
	HRESULT SendMsgViewAround(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);	
    virtual HRESULT SendMsgViewAroundRelation(const Faction::EMRELATION_FLAG_TYPE emRelation, NET_MSG_GENERIC* nmg);
    HRESULT SendMsgViewAroundRelation(const Faction::EMRELATION_FLAG_TYPE emRelation, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);	

    //! 나에게 전송할때
    virtual void SendToClient(NET_MSG_GENERIC* pMsg);
    void SendToClient(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
    void SendToClient(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	//! 다른 Character 에게 전송할때
	virtual void SendToClient(const DWORD dwClientSlot, NET_MSG_GENERIC* pMsg);
	void SendToClient(const DWORD dwClientSlot, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
    void SendToClient(const DWORD dwClientSlot, EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);
    
    void SendToAgent(NET_MSG_GENERIC* pMsg);
    void SendToAgent(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
    void SendToAgent(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

    void SendToCache(NET_MSG_GENERIC* pMsg);
    void SendToCache(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
    void SendToCache(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	void SendToIntegrationCache( NET_MSG_GENERIC* pMsg );
	void SendToIntegrationCache( EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc = false );
	void SendToIntegrationCache( EMNET_MSG MsgType, const std::vector< std::tr1::shared_ptr< msgpack::sbuffer > >& vecSpBuffer, bool UseCrc = false );
    
    //! 나의 Party 에게 전달
    void SendToParty(NET_MSG_GENERIC* pMsg);
    //! 다른 Party 에게 전달
    void SendToParty(const GLPartyID& PartyID, NET_MSG_GENERIC* pMsg);

    //! 나의 Club 에게 전달
    void SendToClub(NET_MSG_GENERIC* pMsg);
    //! 다른 Club에게 전달
    void SendToClub(DWORD ClubDbNum, NET_MSG_GENERIC* pMsg);

    void SENDCLUBLOG(club::EM_LOG_TYPE LogType, const std::string& LogMsg);
    	
	void SendMsgSkillSucessResult(const SNATIVEID& idSkill, const WORD wLevel, const D3DXVECTOR3& vTARPOS, const STARGETID& targetID);   
    

	HRESULT CheckInstanceItem();

public:
	//
	//mjeon.activity
	//

	void ActivityLoad(BOOL bTransfer = FALSE)
	{
		m_pActivityMan->Load(this, bTransfer);
	}

	void ActivitySave()
	{
		m_pActivityMan->Save(this);
	}
	
	DWORD ActivityCheckLevel()
	{
		return m_pActivityMan->CheckLevel(GetLevel());
	}

	DWORD ActivityCheckMobKill(GLChar* pKillChar, DWORD dwTargetID)
	{
		return m_pActivityMan->CheckMobKill(pKillChar, this, dwTargetID);
	}	

	DWORD ActivityCheckPK()
	{
		return m_pActivityMan->CheckPK(m_mapID.getBaseMapID().Id());
	}

	DWORD ActivityCheckMap()
	{
		return m_pActivityMan->CheckMap(m_mapID.getBaseMapID().Id());
	}

	DWORD ActivityCheckItemUse(DWORD dwTargetID)
	{
		return m_pActivityMan->CheckItemUse(dwTargetID);
	}

	DWORD ActivityCheckItemTake(DWORD dwTargetID, DWORD dwActivityID)
	{
		return m_pActivityMan->CheckItemTake(dwTargetID, dwActivityID);
	}


public:
	//
	//mjeon.attendance
	//
	void AttendanceLoad(BOOL bTransfer = FALSE)
	{
		m_pAttendanceMan->Load(this, bTransfer);
	}

	void AttendanceSave(BOOL bLastOne4Maintenance = FALSE)
	{
		m_pAttendanceMan->Save(this, bLastOne4Maintenance);
	}

	void AttendanceSetReady(BOOL bReady)
	{
		m_pAttendanceMan->SetReady(bReady);
	}

	BOOL AttendanceIsReady()
	{
		return m_pAttendanceMan->GetReady();
	}

	void AttendanceCheckConnection()
	{
		m_pAttendanceMan->CheckTaskConnection();
	}

	void AttendanceCheckSparingSolo()
	{
		m_pAttendanceMan->CheckTaskSparingSolo();
	}

	void AttendanceCheckSparingParty()
	{
		m_pAttendanceMan->CheckTaskSparingParty();
	}

	/*
	void AttendanceCheckSparingClub()
	{
		m_pAttendanceMan->CheckTaskSparingClub();
	}
	*/

	void AttendanceCheckWhisper()
	{
		m_pAttendanceMan->CheckTaskWhisper();
	}

	void AttendanceCheckParty(UINT nPartySize)
	{
		m_pAttendanceMan->CheckTaskParty(nPartySize);
	}

	void AttendanceCheckEnchantW()
	{
		m_pAttendanceMan->CheckTaskEnchantW();
	}

	void AttendanceCheckEnchantWC(BOOL bSuccess)
	{
		m_pAttendanceMan->CheckTaskEnchantWC(bSuccess);
	}

	void AttendanceCheckEnchantA()
	{
		m_pAttendanceMan->CheckTaskEnchantA();
	}

	void AttendanceCheckEnchantAC(BOOL bSuccess)
	{
		m_pAttendanceMan->CheckTaskEnchantAC(bSuccess);
	}

	void AttendanceCheckPointItem()
	{
		m_pAttendanceMan->CheckTaskPointItem();
	}

	void AttendanceCheckPostSend()
	{
		m_pAttendanceMan->CheckTaskPostSend();
	}  

	//
	//mjeon.CaptureTheField
	//
	// 이 함수는 기존의 ChaInfo에, 새롭게 추가된 Character 관련 정보들을 별도로 관리하기 위함이다.
	// CTF의 기여도(점수) 또한 여기에서 관리한다.
	//
	void	LoadExtraInfo();
	void	SaveExtraInfo();

	void	LoadContributionPoint();
	void	SaveContributionPoint();
	
	int		GetContributionPoint()				{return m_nContributionPoint;}
	
	void	UpdateContributionPoint(int nExtraPoint, int nMax)
	{
		m_nRewardContribution = m_PvPPerformance.CalculateContributionPoint(nExtraPoint, nMax);
		m_nContributionPoint += m_nRewardContribution;
	}

	void	CTFResetCounters()					{m_PvPPerformance.Reset();}
	void	CTFKill()							{m_PvPPerformance.Kill();}
	void	CTFDeath()							{m_PvPPerformance.Death();}
	void	CTFRebirthTo()							{m_PvPPerformance.RebirthTo();}
	void	CTFDamageTo(int nDamage)			{m_PvPPerformance.DamageTo(nDamage);}
	void	CTFHealTo(int nHeal)				{m_PvPPerformance.HealTo(nHeal);}

	BOOL	IsInInstance();

	BOOL	ToggleVehicle(bool bActive);	

public:
	const SCONFTING& GETCONFRONTING() { return m_sCONFTING; }
	void ResetConfront( EMCONFRONT_END emEND );

public:
    //주위 캐릭터에 대한 기본 및 외형 정보
    GLMSG::SNETDROP_PC_BASE         ReqNetMsg_Drop_Base();
    GLMSG::SNETDROP_PC_STATEBLOW_SKILLFACT    ReqNetMsg_Drop_StateBlow_SkillFact();
    GLMSG::SNETDROP_PC_PUT_ON_ITEMS ReqNetMsg_Drop_PutOnItems();

	void ReqNetMsg_PMarketItem( DWORD dwClientID );

	void MsgSendPeriod();
	void MsgSendUpdateState();
	void MsgSendUpdateHireSummonState( DWORD dwHireSummonGaeaID, SNATIVEID sBasicSummonID );
	void MsgSendUpdateCharacterSlotState ( void );

	void DoConftStateBackup();
	void DoConftStateRestore();

protected:
	bool CheckQuestItemReward( GLQUEST* pQUEST, GLQUESTPROG* pQUEST_PROG = NULL );
	// 퀘스트 진행에 따라 실행되는 이벤트를 0 : 시작시, 1 : 완료시, 2 : 실패시
	bool CheckQuestProgressEvent( BYTE eventType, GLQUEST* pQUEST );
	bool QuestMoveProgressEvent(const MapID& mapID, DWORD wPosX, DWORD wPosY, DWORD dwGateID, bool bStartMove = FALSE);

	//Rnattendance
	int GetAttendanceCompletedDay( GLQUEST* pQUEST, GLQUESTPROG* pQUEST_PROG );

public:
	bool DoQuestProgress( GLQUESTPROG* pQUEST_PROG );
	void DoQuestGenerateReward( GLQUEST* pQUEST );
	void DoQuestProgMob( DWORD dwMOBID, bool bMySelf, bool bPlayerKill );
	void DoQuestProgQItem( DWORD dwMOBID, bool bMySelf );
	void DoQuestMobKill( PGLCHAR pKillChar, SNATIVEID nidMOB, bool bMySelf, bool bPlayerKill = false );
	void DoQuestItemUse( SNATIVEID nidITEM );
	void DoQuestProgItemUse( DWORD dwITEMID );
	void DoQuestItemGrinding( SNATIVEID nidITEM, EMGRINDING_CLASS emGrindingClass, bool IsSuccess );
	void DoQuestProgItemGrinding( DWORD dwITEMID, EMGRINDING_CLASS emGrindingClass, bool IsSuccess );
	void DoQuestLevelUp();
	void DoQuestReachZone();
	void DoQuestConfting( EMCONFT_TYPE emConftType, bool bSCHOOL );

	void MsgSendQuestTime();
	void DoQuestCheckLimitTime( float fTime, float fElapsedTime );
	void DoQuestCheckLimitDie();
	void DoQuestCheckLeaveMap();
	void DoQuestLevel();

public:
	void ClubBattleSetPoint( DWORD dwClub_P, DWORD dwClub_S, bool bAlliancBattle );

public:
	bool IsInsertToQuestInven( GLInventory &_sINVENTORY, SITEMCUSTOM &sCUSTOM );
	HRESULT InsertToQuestInven( DWORD dwClientID, DWORD dwQID, GLInventory &_sINVENTORY, SITEMCUSTOM &sCUSTOM );

public:

	// 인벤토리에 아이템을 넣는다. PosX, PosY 가 있는 함수는 넣을 위치를 알고 있을 때, 없는 함수는 모를때 사용
	// Item : 넣을 아이템, guid 가 없으면 디비 에러 발생할 수 있음
	// bSendClient : 클라이언트에 아이템이 넣어진 것을 알려줄 때 true, InvenInsertSend 호출됨
	// CallType : 이 함수를 호출할 때 어디에서 호출한 것인지 구분하기 위함, 실제 디비에서
	// 에러가 발생했을 때 로그가 남게됨, enum CALL_TYPE 에 추가해서 사용하면 됨
	// DbUpdate : 디비에 업데이트가 필요할 경우 true, 임시로 메모리 상에서만 필요한 몇가지 경우
	// 제외하고 거의 모든 경우 true 로 넘긴다. 아이템 디비 상태에 따라 동작이 다르다.(insert or update)
	// Force : 인벤토리에 아이템을 넣을 때 실제 사용하지 못하지만 넣을 수 있는 공간이 있다.
	// 거기에 강제로 아이템을 넣을 경우에 사용한다.
	// true : 성공, false : 실패
    bool InvenInsertItem( const SITEMCUSTOM& Item, bool bSendClient, int CallType, 
		bool DbUpdate, bool Force = false, bool bNew = false );
    bool InvenInsertItem( const SITEMCUSTOM& Item, WORD PosX, WORD PosY, bool bSendClient, int CallType,
		bool DbUpdate, bool Force = false, bool bNew = false );

	// 인벤토리 아이템 디비 업데이트만 할 경우 호출
	// JustInvenTypeUpdate 좌표만 업데이트 되는 경우 true, false 면 pInvenItem 정상적인 업데이트 진행
	void InvenUpdateItem( SINVENITEM* pInvenItem, bool JustInvenTypeUpdate = false );

	// 인벤토리에 아이템이 들어갔을 때 클라이언에 알려주는 용도로 사용한다. 실제 단독으로 사용되는 경우는 없다.
	// InvenInsertItem 함수가 호출된 후(InvenInsertItem 호출할 때 bSendClient 는 false 로, 아님 메시지 중복) 호출 되는 경우가 많다.
	void InvenInsertSend( const SINVENITEM* pInsertItem, BOOL bVietnamInven = FALSE, bool bAllLine = false, bool bNew = false );

	SINVENITEM* InvenGetItem( const WORD PosX, const WORD PosY );

	SINVENITEM* InvenFindPosItem( const WORD wPosX, const WORD wPosY );

	DWORD InvenGetAmountEmpty( bool bAllLine );

	GLInventory& GetInventory();
    
	// 인벤토리 아이템을 삭제한다.
	// 실제 디비에서는 삭제되지 않고, 인벤타입이 삭제된 아이템이라는 타입으로 update 만 된다.
	// 저장프로시저 sp_ItemDel 참조
	// PosX, PosY 삭제할 인벤토리 좌표
	// bSendClient : 클라이언트에 아이템이 삭제된 것을 알려줄 때 true, InvenDeleteSend 호출됨
	// CallType : 이 함수를 호출할 때 어디에서 호출한 것인지 구분하기 위함, 실제 디비에서
	// 에러가 발생했을 때 로그가 남게됨, enum CALL_TYPE 에 추가해서 사용하면 됨
	// DbUpdate : 디비에 업데이트가 필요할 경우 true, 임시로 메모리 상에서만 필요한 몇가지 경우
	// 제외하고 거의 모든 경우 true 로 넘긴다.
    bool InvenDeleteItem( WORD PosX, WORD PosY, bool bSendClient, BOOL bVietnam, int CallType, bool DbUpdate );

	// 원래 아이템을 삭제해도 인벤타입만 변경하는 방식이나 이 함수는 실제 테이블에서 삭제한다.
	// bInsertDeletedItemTable : 삭제된 아이템 테이블에 삽입할 것인가? true 로 넘어가면 디비에 ItemDel 테이블에 삽입이 되어
	// 운영, 유지보수에 도움이 될 수 있다.
	bool InvenCompletelyDeleteItem( WORD PosX, WORD PosY, bool bSendClient, BOOL bVietnam, int CallType, bool bInsertDeletedItemTable = true );

	// Item 과 같은 id 의 아이템을 iCount 만큼 삭제한다.
	// 내부에서 InvenDeleteItem 이 호출된다. 
	// bSendClient, bVietnam 는 다른 함수와 같은 동작을 한다.
	bool InvenCountDeleteItem( const SITEMCUSTOM& Item, int iCount, bool bSendClient, BOOL bVietnam );

	// 삭제된 아이템 알려준다. InvenInsertSend 와 비슷하다.
    void InvenDeleteSend( WORD PosX, WORD PosY, BOOL bVietnamInven );
	
	// 아이템을 인벤토리에 넣울 수 있는지 확인한다. 
	// Force : 비활성화된곳이라도 막넣을수 있는지 확인
    bool IsInsertToInven( CItemDrop* pItemDrop, bool Force = false );
	bool IsInsertToInven( const SITEMCUSTOM& Item, bool Force = false );

	// 미리 알고 있는 인벤토리 wPosX, wPosY 좌표에 아이템을 넣을 수 있는지 확인한다.
	BOOL InvenIsInsertable( const WORD wPosX, const WORD wPosY, bool bLOAD = false );

	bool InvenSwapTestInvenItem( SINVENITEM* pInvenItem, SINVENITEM* pHoldInvenItem );

	// 인벤토리에 아이템 넣을 공간이 있나?
	// 있으면 TRUE 리턴, wPosX, wPosY 에 들어갈 수 있는 좌표가 들어있다.
	BOOL InvenFindInsertable( WORD& wPosX, WORD& wPosY, bool bAllLine = false );

	// 장착창에서 삭제한다. DbUpdate 가 true 인 경우 디비에서도 삭제된다. 주의
	// 탈것이 장착에서 바로 삭제되는 경우 true 로 한다.(시간제한으로 바로 삭제되는 경우)
	void RELEASE_SLOT_ITEM_SERVER( EMSLOT _slot, bool DbUpdate = false );

	// 장착창에서 넣는다. DbUpdate 가 true 인 경우 디비에 바로 적용
	// 시간 제한아이템이 삭제되면서 거기에 붙어있던 코스튬이 바로 장착창으로 들어간다.
	// 이런 경우 디비에 바로 적용한다.
	void SLOT_ITEM_SERVER( SITEMCUSTOM& sItemCustom, EMSLOT _slot, bool DbUpdate = false );

	void SlotUpdateItem( SITEMCUSTOM& sItemCustom, EMSLOT _slot, bool JustInvenTypeUpdate = false );
	    
	//! 아이템 고유번호를 생성한다
// 	void RegItemGen( __int64& GenNum, const SNATIVEID &sNID, EMITEMGEN emTYPE );
// 	void RegItemGen( sc::MGUID& guid );

	// InsertToInvenEx 는 없어지고 Force 값을 true 넘기면 InsertToInvenEx 동작을 한다.
	bool InsertToInven( CItemDrop* pItemDrop, bool bParty, bool bPickMsg, bool bDrop, bool Force = false, bool bNew = false );

	// 이벤트나 탐닉 방지 시스템에 의해 변경된 돈의 양을 구한다.
	LONGLONG CalculateInsertmoney( LONGLONG lnAmount );

	void InsertMoney(LONGLONG lnAmount);
	bool InsertToInven( CMoneyDrop* pMoneyDrop );

	// 겹칠 수 있는 아이템을 넣어주는 함수
	// 아이템에 이제 고유키가 있다. sItemCustom 을 넘겨줄 때 guid 를 생성해서 넘기지 않는다.
	// 겹침 개수에 따라 새로 아이템이 만들어지는 경우 이 함수안에서 guid 를 생성된다.
	// vecInsertedItems 는 아이템로그 남기기 위한 용도
	// 아이템 로그 작업 다시 해야한다. 나중에 검색해서 다시 넣어야 한다.
	bool InsertPileItem( const SITEMCUSTOM& sItemCustom, WORD wREQINSRTNUM, std::vector< UPDATED_ITEM >& vecInsertedItems, bool bNew = false );
	bool DeletePileItem( SNATIVEID sNID, WORD wREQDELNUM );
	
	bool ConsumeInvenItem( WORD wPosX, WORD wPosY, bool b2AGENT = false, WORD wNum = 1, bool bTurnNumCheck = false, bool DbUpdate = true );
	bool ConsumeStorageItem( DWORD dwChannel, WORD wPosX, WORD wPosY );
	bool ConsumeSlotItem( EMSLOT emSLOT, WORD wNum = 1 );

	SINVENITEM* UserStorageGetItem( const DWORD Channel, const WORD PosX, const WORD PosY );
	SINVENITEM* UserStorageFindPosItem( const DWORD Channel, const WORD PosX, const WORD PosY );

	BOOL UserStorageIsInsertable( const DWORD dwChannel, const WORD wPosX, const WORD wPosY );

	void UserStorageInsertSend( DWORD dwChannel, const SINVENITEM* pInsertItem );

	bool UserStorageDeleteItem( DWORD dwChannel, WORD PosX, WORD PosY, bool bSendClient, int CallType, bool DbUpdate );
	void UserStorageDeleteSend( DWORD dwChannel, WORD PosX, WORD PosY );

	bool UserStorageInsertItem( DWORD dwChannel, const SITEMCUSTOM& Item, WORD PosX, WORD PosY, bool bSendClient, int CallType, bool DbUpdate );

	// 창고 아이템 디비 업데이트만 할 경우 호출
	// JustInvenTypeUpdate 좌표만 업데이트 되는 경우 true, false 면 pInvenItem 정상적인 업데이트 진행
	void UserStorageUpdateItem( DWORD Channel, SINVENITEM* pInvenItem, bool JustInvenTypeUpdate = false );

	// 아이템 삭제 처리, 바닥에 아이템 드랍할 때 사용되며, 실제로는 디비에서 아이템 저장 위치가 변경된다.
	// 몹이 드랍하는 아이템을 드랍할 때 사용하는게 아닌, 캐릭터 아이템 이동(인벤->인벤, 인벤<->인벤, 유저락커<->인벤 등등)할 때
	// 예외가 발생했을 때 해당 아이템을 바닥에 드랍하는 용도로 사용된다.
	bool DropItem( SITEMCUSTOM& Item, const D3DXVECTOR3& Position, bool Drop = true );

	bool ItemSaleToNpc( WORD PosX, WORD PosY, bool bSendClient, BOOL bVietnam, bool DbUpdate );

    //! 아이템 거래 로그
	// 통계 및 로그 수정
    void LogItemExchange(
        const SITEMCUSTOM& sITEM,
        gamelog::EMIDTYPE FromType, DWORD FromDbNum, // 출발지
        gamelog::EMIDTYPE ToType, DWORD ToDbNum, // 목적지
        EMITEM_ROUTE RouteType,
        int nNum, // 거래 개수
		const MapID &sMapID, 
		EMITEM_ROUTE_DETAIL emFlagDetail,
		bool bForceLog );
        //bool bForceLog = true );

    //! 게임머니 거래로그
	// 통계 및 로그 수정
    void LogMoneyExchange(
        gamelog::EMIDTYPE FromType, DWORD FromDbNum, // 출발지
        gamelog::EMIDTYPE ToType, DWORD ToDbNum, // 목적지
        LONGLONG lnPrice, EMITEM_ROUTE Flag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail);

	//! 게임머니 거래로그
	// 통계 및 로그 수정
	void LogMoneyExchangeWithItemGuid(
		gamelog::EMIDTYPE FromType, DWORD FromDbNum, 
		gamelog::EMIDTYPE ToType, DWORD ToDbNum, 
		LONGLONG lnPrice, EMITEM_ROUTE Flag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail, sc::MGUID& guid );

    //! 아이템 변환로그
    void LogItemConversion(const SITEMCUSTOM& sITEM, gamelog::EMIDTYPE FromType, DWORD FromDbNum, EMITEM_ROUTE emFlag, EMITEM_ROUTE_DETAIL emFlagDetail);

    //! 아이템 랜덤옵션 로그
    void LogItemRandomOption(const SITEMCUSTOM& sITEM);
	void LogItemCostumeState(int iCharDbNum, const SITEMCUSTOM& sITEM);

	void LogItemAddonOption( const SITEMCUSTOM& sITEM );
	void LogItemLinkSkillOption( const SITEMCUSTOM& sITEM );
	void LogItemBasicStatOption( const SITEMCUSTOM& sITEM );

    //! 캐릭터 Action Log
	// 통계 및 로그 수정
    void LogCharAction(
        DWORD dwCI, gamelog::EMACTION emACT, 
        gamelog::EMIDTYPE emTAR, DWORD dwTAR, 
        __int64 nEXP, int nBRIGHT, int nLIFE, int nMONEY, const MapID &sMapID);

    void LogPetAction(int nPetNum, SNATIVEID nItemID, pet::EM_ACTION emAction, int nPetFull);

    void LogVehicleAction(int nVehicleNum, SNATIVEID nItemID, EMVEHICLE_ACTION emAction, int nVehicleFull);

	const ITEM_COOLTIME& GetCoolTime(DWORD dwCoolID, EMCOOL_TYPE emCoolType) const;

	// 재사용 대기시간 남았는지
    bool IsCoolTime(const SNATIVEID& sNativeID) const;
	bool CheckCoolTime(const SNATIVEID& sNativeID );		// 아이템 사용시 쿨타임 체크
	bool SetCoolTime( SNATIVEID sNativeID , EMCOOL_TYPE emCoolType );	//	쿨타임 일괄 셋팅
	__time64_t	GetMaxCoolTime(const SNATIVEID& sNativeID) const;

	bool UpdateNpcCommission( LONGLONG lnCommission );
	
	//
	//mjeon.CaptureTheField
	//
	BOOL IsCTFPlayer();
	BOOL IsCTFPlayerNotCheckMap();
	BOOL IsCTFMap();
	DWORD GetCTFRebirthTime();

	// 감옥맵인지 
	BOOL	IsPrisonMap();

	//
	// Recall 처리가 완료되기 직전, 즉, 타겟 Field로의 이동은 완료(해당 필드에 GLChar 생성 완료)되었으나, 아직 추가적인 이동처리가 이루어지기 직전에 처리할 부분.
	//
	void ToDoRightBeforeRecallDone( const GLLandMan* const pEnterLand, const MapID& mapIDTo/*목적지 MapID, 이 method를 호출하는 시점에서 m_sMapID는 목적지로 이동하기 전의 MapID를 가르킨다.*/);
	//
	// Recall 처리가 완료되는 시점, 즉, 타겟 Field로의 이동이 완료되어서 ViewAround를 설정하는 시점에 특별히 처리 해야할 사항이 있다면, 여기에서 하도록 하자.
	//
	void ToDoRightAfterRecallDone();

	const bool CheckOverStatPoint ();
	const bool CheckEquipedItems ( const bool _bPost = true );

	// 사용하지 않는 홀드아이템 관련 패킷을 임의 전송해서 사용하지 못하는 인벤토리 라인에 아이템을 넣어둔 캐릭터들의 아이템을
	// 바닥에 드랍시킨다.
	const bool CheckInvalidInventoryLine();

	void ReSetStartMap();

	virtual void Play_Disguise( int iIdx, BOOL bOn );
	virtual void Stop_Disguise( );

	virtual void Play_ActiveDamage();
	virtual void Stop_ActiveDamage() {};

	void UpdateSkillEff( float fElapsedTime );

public:
	// 캐릭터 슬롯;
	void LockCharSlot ( const WORD _wCharSlotNum );
	void UnlockCharSlot ( const WORD _wCharSlotNum );
	void ChangeCharSlot ( const WORD _wCharSlotNum );


public:
	void QuestStartFromGetITEM( SNATIVEID nidITEM );
	void QuestStartFromGetSKILL( SNATIVEID nidSKILL );
	void QuestStartFromGetLEVEL( WORD wLEVEL );
	void QuestStartFromCharJOIN( int nFlag );
	void SetAvailableQuest( GLQuestMan::QUESTGEN_RANGE range );

protected:
	HRESULT TouchQItem( DWORD dwGlobID );
	bool IsKEEP_CLUB_STORAGE( DWORD dwCHANNEL );
	void DROP_PUTONITEM( DWORD _dwNUM, float _fRATE );

public:
	bool IsCHATBLOCK();

	void SENDBOXITEMINFO(bool bFirstJoin);

public:
	void PostSetFlagTakenAttach(LONGLONG llPostID);
	BOOL PostGetFlagTakenAttach(LONGLONG llPostID);

	bool IsPOSTBOXUSEABLE( const EMPOSTBOXID_TYPE emIDType, const SNATIVEID& sPostBoxID );

public:
    //! -----------------------------------------------------------------------
    //! Vehicle
    const GLVEHICLE* VehicleConst() const { return m_pVehicle; }
    GLVEHICLE*       Vehicle() { return m_pVehicle; }

    BOOL  VehicleActiveValue() const;
    void  VehicleActiveValueSet(bool bActiveValue);
    BOOL  VehicleIsNotEnoughFull() const;
    
    DWORD VehiclePassengerId(size_t Index) const;
    void  VehiclePassengerIdSet(size_t Index, DWORD GaeaId);
    void  VehiclePassengerIdReset(size_t Index);

    void  VehicleAssign(const GLVEHICLE& sVehicleData);

    DWORD VehicleOwnerDbNum() const;
    void  VehicleOwnerDbNumSet(DWORD ChaDbNum);
    
    //! 소환을 요청한 탈것 아이템의 Mid/Sid
    SNATIVEID VehicleId() const;
    void VehicleIdSet(const SNATIVEID& VehicleId);
    
    DWORD VehicleDbNum() const;
    void  VehicleDbNumSet(DWORD DbNum);

    void VehicleUpdateTimeLmtItem(GLChar* pChar);
    
    const SITEMCUSTOM& VehiclePutOnItemRef(VEHICLE_ACCESSORYTYPE Type) const;
	
	// 탈것 착용아이템 해제
	// DbUpdate true 면 바로 디비에서 삭제된다.
    void VehiclePutOnItemReset( VEHICLE_ACCESSORYTYPE Type, bool DbUpdate = false );

	// 디비 업데이트(insert or update)
	void VehiclePutOnItemUpdate( EMSUIT emSuit, bool JustInvenTypeUpdate = false );
	void VehiclePutOnItemUpdate( VEHICLE_ACCESSORYTYPE Type, bool JustInvenTypeUpdate = false );

    SITEMCUSTOM VehiclePutOnItem(VEHICLE_ACCESSORYTYPE Type) const;
    SNATIVEID VehiclePutOnItemId(VEHICLE_ACCESSORYTYPE Type) const;

    SVEHICLE_COLOR VehicleColor(VEHICLE_ACCESSORYTYPE Type) const;
    
    void VehicleItemUpdate();

    float VehicleSpeedRate() const;
    float VehicleSpeedVol() const;

    VEHICLE_TYPE VehicleType() const;
    void         VehicleTypeSet(VEHICLE_TYPE emType);

    int VehicleFull() const;
    DWORD VehicleBoosterId() const;    

	void    ActiveVehicle(bool bActive, bool bMoveMap);
    void    ActiveVehicleTrue();
    void    ActiveVehicleFalse(bool bMoveMap);    
    HRESULT	ActiveWithVehicle( bool bActive, bool bMoveMap );

    //! true:DB에서 정보 가져오는 중, false:DB에서 정보 가져오지 않음
    bool VehicleFromDb() const { return m_bGetVehicleFromDB; }
    void VehicleFromDbSet(bool bSet) { m_bGetVehicleFromDB=bSet; }

	void		SendJumpMsg( const D3DXVECTOR3& vTarPos, bool bGate, DWORD dwFlags );
	void		SendGoingMsg( const D3DXVECTOR3& vTarPos );
	bool		CheckStateToGo();

public:
	BOOL ISBOTH_HANDHELD (const SITEM* pITEM ) const;

	HRESULT MsgReady(NET_MSG_GENERIC* nmg);
	HRESULT MsgActState(NET_MSG_GENERIC* nmg);
	HRESULT MsgMoveState(NET_MSG_GENERIC* nmg);
	HRESULT MsgGoto(NET_MSG_GENERIC* nmg);
	HRESULT MsgDominateMoveTo(NET_MSG_GENERIC* nmg);
	void	MsgAniMoveStartCF(NET_MSG_GENERIC* nmg);
	void	MsgAniMoveEndCF(NET_MSG_GENERIC* nmg);
	HRESULT MsgAttack(NET_MSG_GENERIC* nmg);
	HRESULT MsgAttackCancel(NET_MSG_GENERIC* nmg);

	HRESULT MsgSetTarget(NET_MSG_GENERIC* nmg);
	
	HRESULT MsgReqFieldToInven( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqFieldToHold( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqInvenItemSort( NET_MSG_GENERIC* nmg );
	int MsgReqInvenItemSortStart( NET_MSG_GENERIC* nmg );
	int MsgReqInvenItemSortData( NET_MSG_GENERIC* nmg );
	int MsgReqInvenItemSortEnd( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqInvenToHold(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqInvenExInven( NET_MSG_GENERIC* nmg );
    HRESULT MsgReqStorageExInven( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenToSlot( NET_MSG_GENERIC* nmg );
    HRESULT MsgReqSlotToInven( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqVNGainToHold( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqVNGainExHold( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqHoldToVNGain( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqVNGainInvenReset( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqVNInvenToInven( NET_MSG_GENERIC* nmg );
	
	HRESULT MsgReqInvenSplit( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqSlotToHold( NET_MSG_GENERIC* nmg);

	HRESULT MsgReqItemSort( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqItemSortStart( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqItemSortData( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqItemSortEnd( NET_MSG_GENERIC* nmg );

    HRESULT MsgReqHoldToInven( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenToInven( NET_MSG_GENERIC* nmg );
    HRESULT MsgReqStorageToInven( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenToField( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqHoldToSlot( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqSlotChange( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqMoneyToField( NET_MSG_GENERIC* nmg );

    //! NPC 로부터 아이템 구입
	HRESULT MsgReqBuyFromNpc( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqSaleToNpc( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqExchangeBuyFromNpc( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqReBirth(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqLevelUp(NET_MSG_GENERIC* nmg);

    HRESULT ReqLevelUpValid();
    HRESULT ReqLevelUpInvalid();

	HRESULT MsgReqJumpingLevelUp(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqJumpingLevelUpCA(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqJumpingLevelUpByExp(NET_MSG_GENERIC* nmg);
	HRESULT ReqJumpingLevelUpValid();


    HRESULT MsgReqStatsUp(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqStatsUpEx(NET_MSG_GENERIC* nmg);

	//HRESULT MsgReqInvenDrug( NET_MSG_GENERIC* nmg );
    HRESULT MsgReqInvenDrug( WORD PosX, WORD PosY, bool bPetSkill );
	HRESULT MsgReqInvenBoxOpen( NET_MSG_GENERIC* nmg );
    HRESULT MsgReqInvenBoxInfo( NET_MSG_GENERIC* nmg );

    HRESULT MsgReqInvenDisguise( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenCleanser( NET_MSG_GENERIC* nmg );
	
    //! 빌링으로 구매한 아이템 가져오기
	HRESULT MsgReqInvenChargedItem( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqSkill_Targets( GLMSG::SNETPC_REQ_SKILL* pNetMsg, PGLSKILL	pSkill, WORD wLevel, WORD wSkillRange, WORD& wTARNUM, STARID* sTARIDS );
	HRESULT MsgReqSkill_CheckToStartSkill( GLMSG::SNETPC_REQ_SKILL* pNetMsg, PGLSKILL pSkill, EMSKILLCHECK emCHECK );	
	
	HRESULT MsgReqSkill(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqSkillCancel(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqSkillHoldReset(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqLearnSkill(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqSkillUp(NET_MSG_GENERIC* nmg);
    HRESULT MsgReqRnSkillUp(NET_MSG_GENERIC* nmg);

	//HRESULT MsgReqSkillQSetActive(NET_MSG_GENERIC* nmg);
    HRESULT MsgReqSkillQSetActive(WORD Slot);
	HRESULT MsgReqSkillQSet(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqSkillQReSet(NET_MSG_GENERIC* nmg);

	HRESULT MsgReqActionQSet(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqActionQReSet(NET_MSG_GENERIC* nmg);

	HRESULT MsgReqGetStorage( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqStorageDrug( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqStorageSkill( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqStorageToHold( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenExStorage(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqInvenToStorage( NET_MSG_GENERIC* nmg );
    HRESULT MsgReqStorageToStorage( NET_MSG_GENERIC* nmg );


	HRESULT MsgReqGetStorageSpecificItem( NET_MSG_GENERIC* nmg );

	//! Char Inven -> Locker, 돈 넣기
    HRESULT MsgReqStorageSaveMoney( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqStorageDrawMoney( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqStorageSplit( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqInvenGrinding( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenTLGrinding( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenLockBoxUnLock( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenLockBoxOpen( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenResetSkSt( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqInvenPeriodExtend( NET_MSG_GENERIC* nmg );

    HRESULT MsgReqInvenBoxWrapping( NET_MSG_GENERIC* nmg );
    HRESULT MsgReqInvenBoxUnwrapping (NET_MSG_GENERIC* nmg );

	HRESULT MsgInvenInitializeCountry( NET_MSG_GENERIC* nmg, DWORD dwClientID );

	HRESULT MsgReqPostBoxOpen( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqPostBoxOpenCard( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqPostBoxClose( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqReGenGate(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqCure(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqRevive(NET_MSG_GENERIC* nmg);
    HRESULT MsgReqFriendFld(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqMove2CharFld(NET_MSG_GENERIC* nmg);

	//! 캐릭터 부활 요청
    HRESULT MsgReqRecovery();
	HRESULT MsgReqGetExpRecoveryNpc(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqRecoveryNpc(NET_MSG_GENERIC* nmg);
	
	HRESULT MsgReqCharReset(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqCharCard( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqStorageCard( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenLine( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenStorageOpen( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenStorageClose( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenRemodelOpen( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenGarbageOpen( NET_MSG_GENERIC* nmg );
    HRESULT MsgReqInvenPMarketSearchOpen( NET_MSG_GENERIC* nmg );
    HRESULT MsgReqInvenPMarketSearchOpenFB( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenPremiumSet( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqFireCracker(NET_MSG_GENERIC* nmg);
    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.16, Randombox Open시 확률제어 적용 여부 요청 응답
     * modified : sckim, 2016.09.20, Redmine #4544 확률/아이템 리스트 제어 기능 적용(MsgReqInvenRandomBoxOpenFB 함수 파라미터 변경)
     */
	HRESULT MsgReqInvenRandomBoxOpen( DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
    HRESULT MsgReqInvenRandomBoxOpenFB( NET_MSG_GENERIC* nmg, float& ItemRate, SITEMCUSTOM& sItemBox, SITEMCUSTOM& sItemInBox );

	HRESULT MsgReqInvenSelformBoxSelectOpen( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenDisJunction( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqInvenHairChange( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenHairStyleChange( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenHairColorChange( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenFaceChange( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenFaceStyleChange( NET_MSG_GENERIC* nmg );
	HRESULT	MsgReqInvenGenderChange( NET_MSG_GENERIC* nmg );
	HRESULT	MsgReqInvenSchoolChange( NET_MSG_GENERIC* nmg );

	HRESULT	MsgReqInvenLunchBox( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqInvenRename( NET_MSG_GENERIC* nmg );
	HRESULT MsgInvenRename( NET_MSG_GENERIC* nmg );

	HRESULT MsgInvenVietnamGet( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqConFrontStart(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqConFrontEnd(NET_MSG_GENERIC* nmg);

	HRESULT MsgReqGesture(NET_MSG_GENERIC* nmg);

	HRESULT MsgReqNpcItemTrade( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqNpcQuestStart(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqNpcQuestStartWithoutNPC(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqNpcQuestTalk(NET_MSG_GENERIC* nmg);

	HRESULT MsgReqQuestGiveUp(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqQuestReadReset(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqQuestComplete(NET_MSG_GENERIC* nmg);

	HRESULT MsgLoudSpeaker( NET_MSG_GENERIC* nmg );
    HRESULT MsgChatPartyRecruitFB(EMCHAT_PARTY_RECRUIT emFB, float fPartyRecruitTime);
	
	HRESULT MsgPMarketTitle(NET_MSG_GENERIC* nmg);
	HRESULT MsgPMarketReqItem( NET_MSG_GENERIC* nmg );
	HRESULT MsgPMarketDisItem(NET_MSG_GENERIC* nmg);
	HRESULT MsgPMarketOpen(NET_MSG_GENERIC* nmg);
	HRESULT MsgPMarketClose(NET_MSG_GENERIC* nmg);
	HRESULT MsgPMarketItemInfo(NET_MSG_GENERIC* nmg);
	HRESULT MsgPMarketBuy(NET_MSG_GENERIC* nmg);

	int MsgPrivateMarketItemBuyHF( NET_MSG_GENERIC* nmg );
	
	//! 클럽생성요청
    HRESULT MsgClubNew(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubRank(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubNew2FLD(NET_MSG_GENERIC* nmg);
	//HRESULT MsgClubMemberReq(NET_MSG_GENERIC* nmg);
	//HRESULT MsgClubMemberAns(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubMemberNick(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubCDCertify(NET_MSG_GENERIC* nmg);

	//HRESULT MsgReqClubGetStorage(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqClubStorageToInven( DWORD ClubDbNum, const SITEMCUSTOM& Item, WORD PosX, WORD PosY );
	HRESULT MsgReqClubStorageExInven( DWORD ClubDbNum, const SITEMCUSTOM& ClubItem, WORD Channel, WORD ClubStoragePosX, WORD ClubStoragePosY, WORD InvenPosX, WORD InvenPosY );
    
    //! 손에 들고있던 아이템 클럽창고에 넣음
	bool MsgReqClubInvenToStorage( DWORD Channel, WORD PosX, WORD PosY, WORD HoldPosX, WORD HoldPosY );
    //bool HoldItemRestore(const SITEMCUSTOM& Item);
	//HRESULT MsgReqClubStorageSplit(NET_MSG_GENERIC* nmg);

	//
	//mjeon.activity
	//	
	HRESULT MsgActivityDoneItemTake( NET_MSG_GENERIC* nmg );
	HRESULT MsgActivityChangeBadge(NET_MSG_GENERIC* nmg);
	HRESULT MsgActivityCompletedFF(NET_MSG_GENERIC* nmg);
	HRESULT MsgActivityLoadCompletedFF(NET_MSG_GENERIC* nmg);

	//
	//mjeon.attendance
	//
	HRESULT MsgAttendanceSyncFF(NET_MSG_GENERIC* nmg);
	HRESULT MsgAttendanceResetFF(NET_MSG_GENERIC* nmg);
	HRESULT MsgAttendanceUpgradeFF(NET_MSG_GENERIC* nmg);
	HRESULT MsgAttendanceTakeRewardCF(NET_MSG_GENERIC* nmg);
	HRESULT MsgAttendanceDonePointItemCF(NET_MSG_GENERIC* nmg);
	HRESULT MsgAttendanceWhisperDoneAF(NET_MSG_GENERIC* nmg);

	//
	//mjeon.CaptureTheField
	//
	HRESULT MsgChaExtraInfoHF(NET_MSG_GENERIC* nmg);

	HRESULT MsgReqClubStorageSaveMoney( NET_MSG_GENERIC* nmg );
    void MsgReqClubStorageSaveMoneyAF( LONGLONG Money );
	HRESULT MsgReqClubStorageDrawMoney(DWORD ClubDbNum, LONGLONG Money);
	//HRESULT MsgReqClubInComeReNew(NET_MSG_GENERIC* nmg);

private:
	HRESULT _RebuildRandomOption(GLMSG::SNET_REBUILD_RESULT* pNetMsg);
	HRESULT _RebuildAddon(GLMSG::SNET_REBUILD_RESULT* pNetMsg);
	HRESULT _RebuildBasicStats(GLMSG::SNET_REBUILD_RESULT* pNetMsg);
	HRESULT _RebuildLinkSkill(GLMSG::SNET_REBUILD_RESULT* pNetMsg);

public:
	HRESULT MsgReqRebuildResult( NET_MSG_GENERIC* nmg );	// ITEMREBUILD_MARK
	HRESULT MsgReqRebuildMoveItem( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqRebuildCostMoney();
	HRESULT MsgReqRebuildInputMoney(NET_MSG_GENERIC* nmg);

	HRESULT MsgReqGarbageResult( NET_MSG_GENERIC* nmg );	// 휴지통 사용

	//HRESULT MsgReqPhoneNumber(NET_MSG_GENERIC* nmg);
	//HRESULT MsgPhoneNumber(NET_MSG_GENERIC* nmg);
	HRESULT MsgSendSMS( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqSendSMS( NET_MSG_GENERIC* nmg );

	HRESULT MsgItemShopOpen(NET_MSG_GENERIC* nmg);	// ItemShopOpen
	HRESULT MsgReqItemMix( NET_MSG_GENERIC* nmg );	// ItemMIx

	HRESULT MsgReqDiceDetNumber(NET_MSG_GENERIC* nmg); // 주사위 게임;

	void CallbackProduct();

	HRESULT MsgReqDialogueWindowOpen(NET_MSG_GENERIC* nmg);	
	HRESULT MsgReqNpcTalkBasic(NET_MSG_GENERIC* nmg);

	// PET
	HRESULT MsgReqGetRightOfItem( NET_MSG_GENERIC* nmg );
	HRESULT	MsgGiveFood( NET_MSG_GENERIC* nmg );
	HRESULT MsgGetPetFullFromDB( NET_MSG_GENERIC* nmg );
	HRESULT	MsgPetReviveInfo( NET_MSG_GENERIC* nmg );

	// 코스튬 염색. by luxes.
	bool	CheckCostumColorRestrict( int nColor );
    bool	CheckCostumColorChange_Old( DWORD dwMainColor, DWORD dwSubColor );
    bool	CheckCostumColorChange( DWORD dwMainColor, DWORD dwSubColor );
	HRESULT MsgReqInvenCostumColorChange( NET_MSG_GENERIC* nmg );

	// vehicle 
	HRESULT	MsgVehicleInvenExSlot( NET_MSG_GENERIC* nmg );
	HRESULT	MsgVehicleInvenToSlot( NET_MSG_GENERIC* nmg );
	HRESULT	MsgVehicleSlotToInven( NET_MSG_GENERIC* nmg );
	HRESULT	MsgVehicleRemoveSlot( NET_MSG_GENERIC* nmg );
	HRESULT	MsgVehicleGiveBattery( NET_MSG_GENERIC* nmg );
	HRESULT	MsgGetVehicleFullFromDB( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenVehicleColorChange( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqVehicleGetOff( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqLearnBikeBoost( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqUseBikeBoost( NET_MSG_GENERIC* nmg );

	// summon
	//HRESULT	MsgSummonSlotExHold(NET_MSG_GENERIC* nmg);
	//HRESULT	MsgSummonHoldToSlot(NET_MSG_GENERIC* nmg);
	//HRESULT	MsgSummonRemoveSlot(NET_MSG_GENERIC* nmg);

	// vietNam GainType System
	HRESULT MsgReqVietnamGainType(NET_MSG_GENERIC* nmg);

	// ETC
	HRESULT MsgSetServerDelayTime(NET_MSG_GENERIC* nmg);
	HRESULT MsgSetSkipPacketNum(NET_MSG_GENERIC* nmg);
	HRESULT MsgSetServerCrashTime(NET_MSG_GENERIC* nmg);	

	HRESULT	MsgGathering(NET_MSG_GENERIC* nmg);
	HRESULT MsgGatheringCancel(NET_MSG_GENERIC* nmg);

    // 재매입.
    HRESULT MsgBuyRebuyItem( const SITEMCUSTOM& sItemCustom, LONGLONG Price );

	void SendTriggerMessage(TriggerSystem::SMessage* msg);
	void PostTriggerMessage(TriggerSystem::SpMessage msg);

	// PVE 무한의 재단
	HRESULT MsgReqInvenStagePass( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenStagePassOk(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqInvenEnterNumCharge( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenEnterNumChargeOk( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqInvenCostumeStats( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqInvenCostumeStats_Release( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqInvenChangeItemSkill ( NET_MSG_GENERIC* _pMsg );
	HRESULT MsgReqInvenChangeBasicStats ( NET_MSG_GENERIC* _pMsg );
	HRESULT MsgReqInvenChangeAddStats ( NET_MSG_GENERIC* _pMsg );
	HRESULT MsgReqInvenUnlockCharacterSlot ( NET_MSG_GENERIC* _pMsg );

	HRESULT	MsgTransDB2WorldBattle( NET_MSG_GENERIC* nmg );
	HRESULT MsgTransDB2WorldBattleAns( NET_MSG_GENERIC* nmg );

	// 경험치 압축 / 캡슐
	int MsgReqExpCompressor(NET_MSG_GENERIC* nmg, int &nType, SNATIVEID &sCapsuleItem);
	HRESULT MsgReqExpCapsule(NET_MSG_GENERIC* nmg);

	// Macro;
	HRESULT MsgReqMacroModeOnoffCF(NET_MSG_GENERIC* nmg);
	void	MacroOnOff(bool bOn);
	void	UpdateMacroState( float fElapsedTime );
	
	DWORD	GetNearestEnemy(void);
	BOOL LandNodeCheckHierarchy(LANDQUADNODE* pQuadNode, GLCROWLIST* crowList);
	BOOL LandNodeCheckLowChunk (LANDQUADNODE* pQuadNode, GLCROWLIST* crowList);//, GLCROWLIST& crowList);
	
	// 스킬ui에서 스킬초기화 버튼 눌렀을 때
	HRESULT MsgReqResetSkillStatsBySkillUI( NET_MSG_GENERIC* nmg );
public:
	//HRESULT MsgProcess(NET_MSG_GENERIC* nmg);	

protected:
	void UpdateActState(void);
	HRESULT UpdateClientState( float fElapsedTime );
	HRESULT UpdateVehicle( float fElapsedTime );
	HRESULT UpdateBooster( float fElapsedTime );
	// 지형에 대한 이상효과 
	void	UpdateLandEffect();
	void	UpdateMapEffect();

public:
	// 예약 액션 관련 ( Reserve Action );
	void	ReserveAction( EMRESERVED_ACTION emReservedAction );
	void	CancelReserveAction( EMRESERVED_ACTION emReservedAction );
	void	CancelAllReserveAction();

public:
	BOOL			ModifyExpiredTimeHostile( const DWORD dwCharID, const float fExtendTime );
	void			RemoveOverTimeHostile( const float fElapsedTime );
	void			Confronting( const float fElapsedTime );
	BOOL			CheckConfronting();

	// 예약 액션 수행 ( Reserve Action );
	void			FrameMove_ReservedAction();

	void			FrameMoveTAIWAN_HONGKONG();
	void			FrameMoveAction( const float fElapsedTime );
	void			FrameMoveVehicle();
	void			FrameMoveForcedExitMap();
	HRESULT 		FrameMove(float fTime, float fElapsedTime, __time64_t CurrentTime);
    void    		CheckOneSec(__time64_t CurrentTime);
	void			ChinaGainCalculate();
	void			VietnamGainCalculate();
	void			EventCalculate();

    void    		DisableDebuffSkillFact();
	void			DisableSkillFact();

	DWORD GetChatLinkData(SLINK_DATA_BASIC& sLinkDataBasic, BYTE* pBuffer, size_t BufferSize);
    int GetChatLinkDataParty(SLINK_DATA_BASIC &sLinkDataBasic, BYTE* pBuffer, size_t BufferSize);
    int GetChatLinkDataItemLink(SLINK_DATA_BASIC &sLinkDataBasic, BYTE* pBuffer, size_t BufferSize);

    
    HRESULT	AutoPotionDrug();

    float			GET_PK_ITEMDROP_RATE();   

    // 끌어오기, 밀어내기 확률 계산 및 거리 확인
    const bool CheckHit(const STARGETID& cTargetID, const BOOL bLowSP, const DWORD dwApplyFlag);

    //! 캐릭터가 몹을 공격했을때 데미지 계산 공식
	DWORD CALCDAMAGE(
		int &rResultDAMAGE,
		const DWORD	dwGaeaID,
		const STARGETID &cTargetID,
		const GLLandMan* pLandMan,
		const GLSKILL* pSkill=NULL,
		const DWORD dwskill_lev=0,
		const DWORD dwWeatherFlag=NULL,
		const DWORD dwDivCount=0);

	// 힐량
	DWORD CALCHEAL(
		int &rResultDAMAGE,
		const DWORD	dwGaeaID,
		const STARGETID &cTargetID,
		const GLLandMan* pLandMan,
		const GLSKILL* pSkill=NULL,
		const DWORD dwskill_lev=0,
		const DWORD dwWeatherFlag=NULL,
		const DWORD dwDivCount=0);

    void LogMonsterKill(const SNATIVEID& MonsterID, const std::string& MonsterName);
    void LogPlayerKill(const std::string& KillCharName, const std::string& DeadCharName, bool bKill);

    //! Private Market/개인상점/개인 상점
    bool IsPrivateMarketOpen() const;
    void PrivateMarketClose();
    void PrivateMarketFindItem(const SSEARCHITEMDATA& sSearchData, std::vector<SFINDRESULT>& vecFindResult);
    const std::string& GetPrivateMarektTitle() const;
    const MAPSEARCH& GetPrivateMarketSearchItemList() const;
    size_t GetPrivateSearchResultSize() const { return m_PrivateMarketSearchResult.size(); }
    void PrivateMarketSearchResultClear();
    SSEARCHITEMESULT GetPrivateMarketSearchResult(size_t Index);
    void AddPrivateMarketSearchResult(const SSEARCHITEMESULT& Data);
	DWORD GetPrivateMarketItemTurnNum( const UUID& sItemGUID );
	SSALEITEM* GetPrivateMarketItem( SNATIVEID sSALEPOS );

	void Decompose_GenerateItem(SGENINFO::VEC_SPGENITEM& vecGenItem, OUT SGENRESULT& sResult);

	void LogRandomOptionAll();

	// 란모바일
	void RanMobileCharacterConnectionFee( LONGLONG Price , bool bExtend );
	void RanMobileDailyBonus( const SNATIVEID& ItemID );
	bool RanMobileDeleteInvenItem( int nType, const SNATIVEID& ItemID, int nCount ); // 보석 (1) 아니면 연장권 (2)
	void RanMobileBuyMPointItem( const SNATIVEID& ItemID );

	void SetRanMobileRecognized( bool bValue ) { m_bRanMobileRecognized = bValue; }
	bool GetRanMobileRecognized() { return m_bRanMobileRecognized; }

	void SetRanMobilePoint( int nValue ) { m_nRanMobilePoint = nValue; }
	int GetRanMobilePoint() { return m_nRanMobilePoint; }

	void SetRanMobileRewardDate( __time64_t tDate ) { m_tRanMobileRewardDate= tDate; }
	__time64_t GetRanMobileRewardDate() { return m_tRanMobileRewardDate; }

protected:
	void SummonStateVisibleUpdate(void) override {}

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<GLCHARLOGIC>(*this);

		ar & m_Country;	//world battle country

		ar & m_AUTHFB;
		ar & m_AUTHTW;
		
		//
		//mjeon.instance.transfer.serialization
		//  pointer serialization includes the object's allocation and all we need is just copying of it's content.
		//  The allocation will be done by GLCharacter's constructor which is called by NEW_CHAR().
		//
		ar & *m_pActivityMan;
		ar & *m_pTitleMan;
		ar & *m_pAttendanceMan;

		//ar & m_dwPetGUID;
		//ar & m_PetDbNum;

		/*			
		ar & m_pVehicle;
		*/
	}
};


struct GLSkillTargetArgv
{
	GLChar* const pChar;
	GLMSG::SNETPC_REQ_SKILL* const pNetMsg;
	const GLSKILL* const pSkill;
	const STARGETID& TargetID;	
	const WORD wSkillLevel;
	
	const GLSkillTargetArgv& operator =(const GLSkillTargetArgv&){}
	GLSkillTargetArgv(GLChar* const _pChar, GLMSG::SNETPC_REQ_SKILL* const _pNetMsg, const GLSKILL* const _pSkill, const STARGETID& _TargetID, const WORD _wSkillLevel)
		: pChar(_pChar)
		, pNetMsg(_pNetMsg)
		, pSkill(_pSkill)
		, TargetID(_TargetID)		
		, wSkillLevel(_wSkillLevel)		
	{
	}
};

const bool setTarget_Self(GLSkillTargetArgv& SkillTargetArgv);
const bool setTarget_Spec(GLSkillTargetArgv& SkillTargetArgv);
const bool setTarget_SelfToSpec(GLSkillTargetArgv& SkillTargetArgv);
const bool setTarget_Zone(GLSkillTargetArgv& SkillTargetArgv);
const bool setTarget_Land(GLSkillTargetArgv& SkillTargetArgv);

typedef GLChar*	PGLCHAR;
typedef sc::CGLLIST<GLChar*> GLCHARLIST;
typedef sc::SGLNODE<GLChar*> GLCHARNODE;

typedef std::map<std::string, GLChar*>	GLCHAR_MAP;
typedef std::pair<std::string, GLChar*>	GLCHAR_MAP_PAIR;
typedef GLCHAR_MAP::iterator GLCHAR_MAP_ITER;

#endif // _GL_CHAR_H_
