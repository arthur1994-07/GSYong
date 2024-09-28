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
// �κ��丮 ���ӸӴ� ��ȭ ������ ���� ���� ��� �Ʒ��� �߰��Ѵ�. �߰��� �����ϸ� �ȵ�
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

//Serialization - �ϴ� �� ����

//! ĳ����.
class GLChar : public GLCHARLOGIC, public ServerActor
{
public:
	
	// �� Flag�� ����� ��� ���ڿ� �´� ��쿡 ������ �����ϰ� �ȴ�;
	// ������ ����� �Ŀ��� ������ ��������;
	// ( Reserve Action )
	enum EMRESERVED_ACTION
	{
		EMRESERVED_ACTION_CTF_QUIT,				// CTF QUIT ����;
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
// 	D3DXVECTOR3			m_vPosition;					//	ĳ���� ��ġ
// 	D3DXVECTOR3			m_vBackupPosition;			//	ĳ���� ��ġ ���

	DWORD				m_dwLastPostSend;		// Time when the character sent the last post.
	std::set<LONGLONG>	m_setAttachTakenPostID;	//mjeon.�ε��׽þ� ������ ���繮���� �������� FieldServer���� 2�� üũ�� �����Ѵ�.
												//���� ÷�� �����۰� ÷�� ���ӸӴϴ� ���ÿ� ���ϰ� �Ǿ� �����Ƿ� �ϳ��� ���͸� ����ϸ� �ȴ�.

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
	STARGETID			m_sNPCID;				// �ֱٿ� ���õ� NPC
	WORD				m_wTARNUM;
	STARID				m_sTARIDS[EMTARGET_NET<<1];	
	DWORD				m_dwANISUBSELECT;
	GLWorkingSkill_s	m_WorkingSkill;			//  �������� ���� ��ų

	CSkillDistinguish	m_SkillDistinguish;

protected:
	float				m_fPingTraceTimer;			//  mjeon.Add PingTrace timer
	float				m_fIdleTime;				//	�޽� �ð�.
	float				m_fMBR_TIMER;				//	��Ƽ ������� ������ ���� �ð�.
	float				m_fSTATE_TIMER;				//	���� ���� ���� �ð�.
	float				m_fPERIOD_TIMER;			//	Period �ð� ����.
	float				m_fQUEST_TIMER;				//	����Ʈ ���� ����.
	float				m_fattTIMER;				//	���� ������.
	float				m_fMoveDelay;				//	�̵� ������.
	float				m_fITEM_TIMER;				//	������ ���ѽð� ����.
	float				m_fSAVEDB_TIMER;			//	DB ���� �ð�.
	float				m_fBRIGHT_TIMER;			//	�Ӽ���ġ ���� �ð�.
	float				m_fSkillDelay;				//  ��ų ������(������� ������)
    float               m_PositionSendTimer;	    // ��ġ���� ����
	float				m_fPremiumExitCheckTimer;	// �����̾� �� �������� üũ �ð�

	bool				m_bReservedActionFlags[ EMRESERVED_SIZE ];

protected:
	STARGETID			m_sAssault;				//	���� �ֱٿ� �ڽ��� ��������.
	STARGETID			m_sHeal;					// ���� �ֱٿ� �ڽ��� ġ������.
	LONGLONG			m_lnLastSendExp;
	LONGLONG			m_lnLastSendVNExp;		//  �������� ���� ��Ʈ�� ���� ����ġ

	WORD				m_wLastSendCP;			//  ���������� ���� CP

	WORD				m_ItemRemodelPosX;		//	����ī�� ���� ������ ��ġ ����
	WORD				m_ItemRemodelPosY;		
	WORD				m_ItemStampPosX;			// ����ī�� ���� ������ ��ġ ����;
	WORD				m_ItemStampPosY;
	WORD				m_ItemStampFixMax;

private:
	void _SetStampInfo(SINVENITEM* pInvenItem);

protected:
	GLARoundSlot		m_ARoundSlot;			//	�ǰ��ݽÿ� �ڽ��� �ֺ��� ������� �ִ� ��ġ�� �ִ��� ����.

	DAMAGELOG			m_cDamageLog;			//	����� �α�.

	WORD				m_wHealingNum;			//	���� ��.


    // PVE�δ��� �̵� ���� ����;
    GLStateMiniFSM      m_cMovement;            // ������(��ų ����)

    float				m_fAge;					//	��� �ð�.	

	bool				m_bRanMobileRecognized;	// �� ����� ���� ����
	int					m_nRanMobilePoint;		// �� ����� ����Ʈ
	__time64_t			m_tRanMobileRewardDate;	// �� ����� ���� ���� ���� ��¥

public:
	__time64_t			m_tLoginTime;			//! ���� �ð�;
	DWORD				m_dwViewFrame;			//	����ȯ�� ������Ʈ ������.

private:
	//* ���� ���� �����Ͽ� �������� ����! *//
	unsigned int		m_uintViewAround;					//	���� ȯ�� ������Ʈ ���. ����
	float				m_fViewAroundSettingReleaseTimer;   //  Ȥ�� �Ǽ��� ��Ǯ���� ����Ͽ� �ڵ����� Ǯ�����
	//* ����ߴµ� �ȵǴ°�� *//
	//1. View Around�� �ſ� ���ɿ� �ΰ��Ͽ� ���̵�("���� ��ġ�� �ٲ����� ����Ʈ ���� ����"�� ����) �� 
	//    �����ð�(m_fViewAroundSettingReleaseTimer)�� ���� Ǯ����. (��� �����ϰ� �ϸ� �����ȴ�)
	//2. MAX_VIEWRANGE ���� ������� �������� �ʴ´�. (�ǵ��� ������)
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
//---------- �ɸ��� ��Ȱ resurrection -----------;
	
    unsignedState       m_sResurrectAbleState;          // ��Ȱ ������ ��Ŀ� ���� flag;
    __int64             m_lnDeathPenaltyEXP;            // ����� ��Ȱ�� ����ġ ���� ��ġ;
    bool                m_bSetDeathPenalty;             // ��� �г�Ƽ ��������;

    float					m_fForcedResurrectTime;			// ���� ��Ȱ ��� �ð�;
    SNATIVEID			m_sForcedResurrect_MapID;		// ���� ��Ȱ�� ��Ȱ ��;
    DWORD				m_dwForcedResurrect_GateID;	// ���� ��Ȱ�� ��Ȱ ���� ����ƮID;
	D3DXVECTOR3		m_vForcedResurrect_position;	// ���� ��Ȱ�� ��Ȱ ���� ����ƮID;
    bool					m_bForcedResurrect_Instance;	// ���� ��Ȱ�� ��Ȱ������ �δ����ΰ�? (�� ��� �ش� BaseMapID�� �δ��� ����,���� �� ��Ȱ);
    float					m_fForcedResurrect_Restore;		// ���� ��Ȱ�� HP,MP,SP ȸ����;

    float               m_fSkillResurrect_Restore;      // ��ų ��Ȱ�� HP,MP,SP ȸ����;    

    float               m_fGenAge;				        // ��Ȱ�� ��� �ð�.;
private:
    void _sendResurrectMsg(GLLandMan* pLandMan, const float _fValue = 100.0f);  // �ɸ��� ��Ȱ;

public:

	BOOL IsSkillLoopEnd() { return m_WorkingSkill.IsApplyEnd(); }
	BOOL IsSkillEnd( SNATIVEID sSkill ) { return m_WorkingSkill.IsApplyEnd(sSkill); }
	BOOL IsSkillEffectFrame();

	bool IsAlive ();	// ĳ���Ͱ� ����ִ°�?;
    void SetAlive(const float _fValue = 100.0f);  // �����ϰ� �ɸ��͸� �ǻ츮�� �۾��� ����;

    void ResurrectMoveMapInstance(const SNATIVEID& _moveMapID, const DWORD _dwGateID, const D3DXVECTOR3& _position, const float _fRestore = 100.0f);  //�δ� �� ���̵��� ���� ��Ȱ;
    void ResurrectMoveMap(const SNATIVEID& _moveMapID, const DWORD _dwGateID, const float _fRestore = 100.0f);  //���̵��� ���� ��Ȱ;
    void ResurrectStartMap(const float _fValue = 100.0f);  // ������������ ��Ȱ;
    void ResurrectCurrentPos(const float _fValue = 100.0f);  // ���� ��ġ���� ��Ȱ;

    void Resurrect(const EMRESURRECTION_TYPE _emResurrectionType);  // �÷��̾� ��Ȱ (��ȿ�� �˻�, ��ũ��Ʈ ����);
	void ResurrectImmediatelyForced();
    void ResurrectProc(const EMRESURRECTION_TYPE _emResurrectionType);  // ���� �÷��̾� ��Ȱó��;

    //void ResurrectDropChar();

    void SetDefaultResurrection();
    void SetResurrectionNomal();  // ��Ȱ ���� ���� ( �Ϲ����� ��Ȳ );
    void SetResurrectionItem();  // ��Ȱ ���� ���� ( ��ȥ�� ����� );
    void SetResurrectionSkill(const float _fResurrectionValue);  // ��Ȱ ���� ���� ( ��ų ��Ȱ ��Ȳ );
    void SetResurrectionForced(const bool _bInstance, const SNATIVEID& _sStartMapID, const DWORD _dwGateID, const D3DXVECTOR3& _position, const float _fForcedResurrectTime, const float _fResurrectionValue = 100.0f );  // ��Ȱ ���� ���� ( ���� ��Ȱ );

    void SetResurrectionDisable(const bool _bDisableSkill);  // ��Ȱ �Ұ��� ���� ( ��ų ��Ȱ ���ɼ����� SetResurrectionSkillDisable ��� );
    void SetResurrectionDisableSkill(const bool _bDisableSkill);  // ��ų ��Ȱ �Ұ��� ���� ( ��Ȱ ���ɼ����� SetResurrectionDisable ��� );

    void Receive_ResurrectSkill(const float _fResurrectionValue);  // ��Ȱ ��ų ����;

    void UpdateForcedResurrect( const float _fElpasedTime ); // ���� ��Ȱ ������Ʈ;

    void DoDeathPenaltyExp();
    LONGLONG	GetRestorableExp();
    LONGLONG	GetEXPRestoreCost( LONGLONG nReExp );

    BOOL CheckResurrectStartMap();

	virtual const bool IsNonTargetable() const;

//-----------------------------------------------;

protected:
	DWORD				m_ClientSlot;			//	Ŭ���̾�Ʈ ��� ���� 

private:
	EMANI_MAINTYPE		m_SKILLMTYPE;
	EMANI_SUBTYPE		m_SKILLSTYPE;

protected:		
	GLItemLimit*		m_pItemLimit;

public:
	MapID			m_mapIDPrevious;			//  ���� �� ID. 

	sc::SGLNODE<GLChar*>*	m_pCharNodeInGaea;			//	���� ����Ʈ ���.
	sc::SGLNODE<GLChar*>*	m_pCharNodeInLand;			// �� ĳ���Ͱ� ����	LandMan�� �����ϰ� �ִ� ĳ���� ���� Node �� �� ĳ������ Node�� ���� ������

	LANDQUADNODE*		m_pQuadNode;			//	���� Ʈ�� ���.( ���� ������ ���� �����ϱ� ������. )
	sc::SGLNODE<GLChar*>* m_pCellNode;			//	�� ����Ʈ ���.

	GLTrade				m_sTrade;				//	�ŷ� ����.

public:	// FlyCamera;
	bool				m_bFlyCameraUse;
	D3DXVECTOR3			m_vFlyCameraPos;

protected:
	PrivateMarketField*	m_pPrivateMarket;	//!	���� ����
    VECSEARCH_RESULT	m_PrivateMarketSearchResult; //! �������� ã�� �����۵� 

public:
	SCONFTING			m_sCONFTING;			// ���� ��� ����.
	
public:
	bool				m_bUsedStorageCard;		// ���â����ī�� ��뿩��
	// PET
SERIALIZE	DWORD		m_dwPetGUID;			// �ָ޸��ε���
SERIALIZE	DWORD		m_PetDbNum;				//! Pet DB ���� ��ȣ
	bool				m_bGetPetFromDB;		// DB���� ���� ������ �������� ������...

	bool				m_bEntryFailed;			// ���Խ���

	bool				m_bEventStart;
	bool				m_bEventApply;

    bool                m_bAcquireEXP;          // GM��ɾ� /acquire_exp ���� �� ����

	BYTE				m_ChinaGainType;	    // �߱� Ž�й��� �ý��� 255: default, 0: 1�ð� ������, 1: ���� 50%, 2: ���� 0%
	CTime				m_China15Time;			// �߱� Ž�й��� �ý���  15�� ���� �������� �ð�

	GAIN_TYPE			m_dwVietnamGainType;	    // ��Ʈ�� Ž�й��� �ý���  255: default, 0: 3�ð� ������ ���� 50%, 1: 0%

	//CTime				m_China1Hour;			// 1�ð� ������ �� ȣ���

	DWORD				m_dwLastPremiumExitNotifyTime;

protected:
	GLVEHICLE*  m_pVehicle;				// Ż�� ����ü

public:
	bool  m_bGetVehicleFromDB; //! true:DB���� ���� �������� ��, false:DB���� ���� �������� ����
	float m_fVehicleTimer;
	
	bool m_bItemShopOpen;		// �����ۼ� �̿������� Ȯ��( Japan )
	bool m_bDefenseSkill;		// �ߵ���ų�� ���������� Ȯ��

    float m_fLimitTimeItemSort;

    float m_fLimitTimeSummonPet; // pet ��ȯ �ð� ����.

private:
	EMANI_MAINTYPE		m_eMotionMID;			//! ��� MID
	EMANI_SUBTYPE		m_eMotionSID;			//! ��� SID
	FLOAT				m_fMotionSec;			//! ��� �÷��� �ð� (��)
	FunctionVoid		m_fnOnMotionFinish;		//! ����� �ߴܵǰų� �������� ȣ���Լ�

	bool		m_bReferCharPublic;		// ĳ���� ���� ��ȸ ���
	FLOAT				m_fReferCharDelay;		// ĳ���� ���� ��ȸ ������

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
    wb::EM_COUNTRY m_Country; //! ��������

protected:
	GLProductField*		m_pProductField;		//���� Ŭ����//hsshin

protected:
	CRYPT_KEY m_cKey;	// ClientManager �� ���� �׸��� ������ ���� ������� �ʰ� �ְ�, �����ʿ��� ����Ϸ��� ���⿡ �߰��Ѵ�.

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
	BOOL m_bUsedPMarketSeacrhCard;	// ���λ��� �˻� ī�� ��뿩��

//Rnattendance
public:
	std::vector<DWORD>	m_vecAttendanceSelectQuest; //�⼮�� ����
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
    
    //! ĳ������ PointShop Point �� �߰��Ѵ�.
    //! PointRefundable ȯ�Ұ��� ����Ʈ
    //! PointNotRefundable ȯ�ҺҰ� ����Ʈ
    bool PointShopPointAdd(__int64 PointRefundable, __int64 PointNotRefundable, pointshop::EM_POINT_CHANGE_TYPE Type);

	//! PointShop ����Ʈ ����
	void PointShopPointSub( const SNATIVEID& BuyItem, __int64 ItemPrice, int nTraceType = 0 );
	void PointShopPointSubByHoldem( const SNATIVEID& Chip, __int64 ChipPrice );
	void PointShopPointSub( __int64 point, pointshop::EM_POINT_CHANGE_TYPE ePointChangeType, int nTraceType = 0 );
private:
	// ����� ����Ʈ ���� �Լ����� ����ϸ� ���������� �Ʒ��� �Լ����� ����ؼ� ����Ʈ ���, DB ���� �� ĳ���� ������ �αױ��� �����
    void PointShopPointSub( __int64 point, const SNATIVEID& Item, pointshop::EM_POINT_CHANGE_TYPE ePointChangeType, int nTraceType = 0 );
    CHARACTER_POINT PointShopPointSub( __int64 point, int nTraceType = 0 ); 
public:

	//! PointShop ������ ����
    //! �ݵ�� �����ؾ� �Ѵ�. �Լ��� ȣ���ϱ� ���� ���Ἲ�� üũ�ؾ� �Ѵ�.
    void PointShopBuyItem( const SNATIVEID& BuyItem, __int64 ItemPrice, const SITEMCUSTOM& ItemCustom );

    //! ��������
    wb::EM_COUNTRY GetCountry() const { return m_Country; }

private:
	// ����Ʈ�� ���� ��ų �� ĳ�ü����� ĳ���� Point ������ �����Ѵ�;
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

	// ������ ��ȿ�� �˻�;
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
		m_dwGlobalID = dwGaeaID; // GLCROWLOGIC���� ��� ���̶� �ϴ� ���ܵ�; // ���������δ� �����ؾ���;
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
	/// ����� ������� �ʴ´�, �Ŀ� ���氡�������� �Ѵٸ� �̿��Ѵ�;
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

	virtual void			SetMultipleHP ( float fMultiple ) { }
	virtual void			SetMultipleAttack ( float fMultiple ) { }
	virtual void			SetMultipleDefense ( float fMultiple ) { }
	virtual void			SetMultipleExp ( float fMultiple ) { }
	virtual void			SetMultipleDrop ( float fMultiple ) { }

private:
	/// ����� ������� �ʴ´�, �Ŀ� ���氡�������� �Ѵٸ� �̿��Ѵ�;
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

public:
    // bExp_ShowUser�� ���ؼ� GM�� �ƴϴ��� ����ġ�� ä��â�� ǥ���� �� �ִ�.
	// - ����ġ, �������� �˸�, ����ġ �߰�ȹ�� ȿ�� ���� ����;
	void ReceiveExp( float fGenExp, bool bExp_ShowUser, bool bExp_Addition = true); 
    void ReceiveExpApply( float fGenExp, bool bExp_ShowUser = false, bool bExp_Addition = true );    // ������ ����ġ�� �޴´�.
	// �̺�Ʈ�� Ž�� ���� �ý��ۿ� ���� ����� ����ġ�� ���Ѵ�.
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
    //! Ŭ���̾�Ʈ ��� ����
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

	void RestoreActStateByInvisible(); // ���� ��ȯ �� ��ų ���� ������ �Բ� ���� �ؾ� ��;
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

    void DoFalling_Proc_By_PVP();  // �÷��̾ ���� �׾��� ���� ���� ó��;
    void DoFalling_Proc_Log();  // �׾��� ���� �α� ����� ó��;

    void DieProc();  // ���� �״� ������ ���� ó��;
    void DieProc_CDM();

    void DeathPenaltyDefault();  // ����� �г�Ƽ ó�� (����ġ �϶�, ������ ���);

public:
    void DeathPenalty(const unsigned int _nPenaltyExp, const unsigned int _nDropItem, const float _fDropRate);
    void SetDeathPenalty_Exp(const __int64 _penaltyExp = 0);  // ����� ����ġ �϶�ġ ����;
    void RestoreDeathPenalty_Exp(const __int64 _penaltyExp);

public:
	bool IsConflictTarget( ServerActor* pTARGET );
    const EMREACTIONABLE IsReActionable ( ServerActor* pTARGET, const EMIMPACT_SIDE _emTargetSide);
    void ReActionByForcedAction( ServerActor* const pTarget, const EMIMPACT_SIDE _emTargetSide );

	// ��ȯ������ Ÿ�ٰ��� �Ѽ� üũ�� �ʿ��ϸ� ���� �Ѽ��� �������� üũ�Ѵ�;
	// ��ȯ�� �̿ܿ��� �����������;
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
	//	���� ���μ���.
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
        �̵� ���� ������ ���� �� �� �ִ�.
        �ܼ� �̵� �� �ƴ϶� ��ų�� ������� ���ϴ� �����̴�.
        true : �̵� ����, false : �̵� �Ұ���

     */
    const CharState IsMovement() const { return m_cMovement.GetState(); }
    void SetMovement(const CharStateID id, const CharState state);

    /**
        ���¸� ���� �Ѵ�.
        �ߺ� �ڵ带 ���� �Ѵ�.

     */
    void SetState(const CharStateID id, const CharState state, GLStateMiniFSM& fsm);

    /**
        ������ ĳ���� ���¸� Ŭ���̾�Ʈ�� ����ȭ �����.
        �̰��� ���� ��Ȱ�� �̵��� �������� ��� �ȴ�.
        ���� ���� �߰��ÿ��� ����� �� �ִ�.

     */
    void SyncClientState(const CharStateIndex idx, const CharState state);

    /**
        ��� ������ �� ���� ��� �Ѵ�.
        �Ϲ�, ��ų, ��ȯ�� �������� ��� �̰��� ��� �Ѵ�.
        ���� �̺�Ʈ�� ���� ������ ���� ��� �ȴ�.
        ���� �̿� �����ϰ� �ʿ��ϴٸ� ��� ���� �ϴ�.
        CTF �� ���� �����ڵ尡 ����������� �ʹ� CTF�� �ϵ��ڵ��Ǿ� �־� ���� �����.

     */
    void ToAllDamage_PVE(const DWORD dwDamage);
    void ToAllHeal_PVE(const DWORD dwHeal);

    /**
        ���� ĳ���͸� ���� ��ġ�� ��� �ش�.
        ����ġ �ս� ���� ���ڸ� ��Ȱ ��ų ��� Ȱ�� �ȴ�.

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

	//	��ų ���μ���.	
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

	//	��ũ ���μ���.
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

	HRESULT UpdateDurDamage( float fTime, float fElapsedTime );	//	���� ������ ó��( ������ �ش�. ) 

	//! ���ӸӴϰ� ������Ʈ �Ǿ��� ��� üũ�ϴ� ����
	BOOL CheckMoneyUpdate(const LONGLONG lnCurrentMoney, const LONGLONG lnUpdateMoney, const bool bPlus, const char* msg);

    //! ���ӸӴ�(�κ��丮) �������� �˸�
    void AddInvenMoneyAndUpdate( LONGLONG llMoney, int nCause );
    void SubtractInvenMoneyAndUpdate( LONGLONG llMoney, int nCause );
    void SendInvenMoneyUpdate();
    //! ���ӸӴϸ� ��ȭ��Ű�� �� ����� Ŭ���̾�Ʈ�� ȹ�� �뺸��
    void AddInvenMoneyAndPickupMessage( LONGLONG llGetMoney, int nCause );

	// ������ �ð� üũ
	// ���Ǵ� �κ� Ȯ���ϴ� ������ ������ ���۷����� �ѱ�� �ִ�. �Լ��ȿ��� ���� �����۵� �����ǰ� �ִ�.
	// �ڵ�ȿ� �ڽ�Ƭ Ȯ���ϴ� �κ����� ������ ���� �ڵ尡 �ִµ�, ���� �����Ǹ� �ڽ�Ƭ Ȯ���ϴ� �κк��� �ƹ� �ҿ��� ���Եȴ�.
	// �Լ� �ȿ��� ���� �������� �����ؼ� ����ϵ��� �����Ѵ�.
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
    //! Pet DB ���� ��ȣ
    inline DWORD GetPetDbNum() const { return m_PetDbNum; }
    inline void SetPetDbNum(DWORD PetDbNum) { m_PetDbNum=PetDbNum; }

    std::tr1::shared_ptr<GLClubField> GetMyClub();
    std::tr1::shared_ptr<GLClubField> GetClub(DWORD ClubDbNum);

	//! �ֺ� �ɸ��� �����Ҷ�
	bool CheckBelongViewAround( GLChar* pTarChar );
	virtual HRESULT SendMsgViewAround(NET_MSG_GENERIC* nmg);
	HRESULT SendMsgViewAround(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);	
    virtual HRESULT SendMsgViewAroundRelation(const Faction::EMRELATION_FLAG_TYPE emRelation, NET_MSG_GENERIC* nmg);
    HRESULT SendMsgViewAroundRelation(const Faction::EMRELATION_FLAG_TYPE emRelation, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);	

    //! ������ �����Ҷ�
    virtual void SendToClient(NET_MSG_GENERIC* pMsg);
    void SendToClient(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false);
    void SendToClient(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	//! �ٸ� Character ���� �����Ҷ�
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
    
    //! ���� Party ���� ����
    void SendToParty(NET_MSG_GENERIC* pMsg);
    //! �ٸ� Party ���� ����
    void SendToParty(const GLPartyID& PartyID, NET_MSG_GENERIC* pMsg);

    //! ���� Club ���� ����
    void SendToClub(NET_MSG_GENERIC* pMsg);
    //! �ٸ� Club���� ����
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
	// �� �Լ��� ������ ChaInfo��, ���Ӱ� �߰��� Character ���� �������� ������ �����ϱ� �����̴�.
	// CTF�� �⿩��(����) ���� ���⿡�� �����Ѵ�.
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
    //���� ĳ���Ϳ� ���� �⺻ �� ���� ����
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
	// ����Ʈ ���࿡ ���� ����Ǵ� �̺�Ʈ�� 0 : ���۽�, 1 : �Ϸ��, 2 : ���н�
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

	// �κ��丮�� �������� �ִ´�. PosX, PosY �� �ִ� �Լ��� ���� ��ġ�� �˰� ���� ��, ���� �Լ��� �𸦶� ���
	// Item : ���� ������, guid �� ������ ��� ���� �߻��� �� ����
	// bSendClient : Ŭ���̾�Ʈ�� �������� �־��� ���� �˷��� �� true, InvenInsertSend ȣ���
	// CallType : �� �Լ��� ȣ���� �� ��𿡼� ȣ���� ������ �����ϱ� ����, ���� ��񿡼�
	// ������ �߻����� �� �αװ� ���Ե�, enum CALL_TYPE �� �߰��ؼ� ����ϸ� ��
	// DbUpdate : ��� ������Ʈ�� �ʿ��� ��� true, �ӽ÷� �޸� �󿡼��� �ʿ��� ��� ���
	// �����ϰ� ���� ��� ��� true �� �ѱ��. ������ ��� ���¿� ���� ������ �ٸ���.(insert or update)
	// Force : �κ��丮�� �������� ���� �� ���� ������� �������� ���� �� �ִ� ������ �ִ�.
	// �ű⿡ ������ �������� ���� ��쿡 ����Ѵ�.
	// true : ����, false : ����
    bool InvenInsertItem( const SITEMCUSTOM& Item, bool bSendClient, int CallType, 
		bool DbUpdate, bool Force = false, bool bNew = false );
    bool InvenInsertItem( const SITEMCUSTOM& Item, WORD PosX, WORD PosY, bool bSendClient, int CallType,
		bool DbUpdate, bool Force = false, bool bNew = false );

	// �κ��丮 ������ ��� ������Ʈ�� �� ��� ȣ��
	// JustInvenTypeUpdate ��ǥ�� ������Ʈ �Ǵ� ��� true, false �� pInvenItem �������� ������Ʈ ����
	void InvenUpdateItem( SINVENITEM* pInvenItem, bool JustInvenTypeUpdate = false );

	// �κ��丮�� �������� ���� �� Ŭ���̾� �˷��ִ� �뵵�� ����Ѵ�. ���� �ܵ����� ���Ǵ� ���� ����.
	// InvenInsertItem �Լ��� ȣ��� ��(InvenInsertItem ȣ���� �� bSendClient �� false ��, �ƴ� �޽��� �ߺ�) ȣ�� �Ǵ� ��찡 ����.
	void InvenInsertSend( const SINVENITEM* pInsertItem, BOOL bVietnamInven = FALSE, bool bAllLine = false, bool bNew = false );

	SINVENITEM* InvenGetItem( const WORD PosX, const WORD PosY );

	SINVENITEM* InvenFindPosItem( const WORD wPosX, const WORD wPosY );

	DWORD InvenGetAmountEmpty( bool bAllLine );

	GLInventory& GetInventory();
    
	// �κ��丮 �������� �����Ѵ�.
	// ���� ��񿡼��� �������� �ʰ�, �κ�Ÿ���� ������ �������̶�� Ÿ������ update �� �ȴ�.
	// �������ν��� sp_ItemDel ����
	// PosX, PosY ������ �κ��丮 ��ǥ
	// bSendClient : Ŭ���̾�Ʈ�� �������� ������ ���� �˷��� �� true, InvenDeleteSend ȣ���
	// CallType : �� �Լ��� ȣ���� �� ��𿡼� ȣ���� ������ �����ϱ� ����, ���� ��񿡼�
	// ������ �߻����� �� �αװ� ���Ե�, enum CALL_TYPE �� �߰��ؼ� ����ϸ� ��
	// DbUpdate : ��� ������Ʈ�� �ʿ��� ��� true, �ӽ÷� �޸� �󿡼��� �ʿ��� ��� ���
	// �����ϰ� ���� ��� ��� true �� �ѱ��.
    bool InvenDeleteItem( WORD PosX, WORD PosY, bool bSendClient, BOOL bVietnam, int CallType, bool DbUpdate );

	// ���� �������� �����ص� �κ�Ÿ�Ը� �����ϴ� ����̳� �� �Լ��� ���� ���̺��� �����Ѵ�.
	// bInsertDeletedItemTable : ������ ������ ���̺� ������ ���ΰ�? true �� �Ѿ�� ��� ItemDel ���̺� ������ �Ǿ�
	// �, ���������� ������ �� �� �ִ�.
	bool InvenCompletelyDeleteItem( WORD PosX, WORD PosY, bool bSendClient, BOOL bVietnam, int CallType, bool bInsertDeletedItemTable = true );

	// Item �� ���� id �� �������� iCount ��ŭ �����Ѵ�.
	// ���ο��� InvenDeleteItem �� ȣ��ȴ�. 
	// bSendClient, bVietnam �� �ٸ� �Լ��� ���� ������ �Ѵ�.
	bool InvenCountDeleteItem( const SITEMCUSTOM& Item, int iCount, bool bSendClient, BOOL bVietnam );

	// ������ ������ �˷��ش�. InvenInsertSend �� ����ϴ�.
    void InvenDeleteSend( WORD PosX, WORD PosY, BOOL bVietnamInven );
	
	// �������� �κ��丮�� �ֿ� �� �ִ��� Ȯ���Ѵ�. 
	// Force : ��Ȱ��ȭ�Ȱ��̶� �������� �ִ��� Ȯ��
    bool IsInsertToInven( CItemDrop* pItemDrop, bool Force = false );
	bool IsInsertToInven( const SITEMCUSTOM& Item, bool Force = false );

	// �̸� �˰� �ִ� �κ��丮 wPosX, wPosY ��ǥ�� �������� ���� �� �ִ��� Ȯ���Ѵ�.
	BOOL InvenIsInsertable( const WORD wPosX, const WORD wPosY, bool bLOAD = false );

	bool InvenSwapTestInvenItem( SINVENITEM* pInvenItem, SINVENITEM* pHoldInvenItem );

	// �κ��丮�� ������ ���� ������ �ֳ�?
	// ������ TRUE ����, wPosX, wPosY �� �� �� �ִ� ��ǥ�� ����ִ�.
	BOOL InvenFindInsertable( WORD& wPosX, WORD& wPosY, bool bAllLine = false );

	// ����â���� �����Ѵ�. DbUpdate �� true �� ��� ��񿡼��� �����ȴ�. ����
	// Ż���� �������� �ٷ� �����Ǵ� ��� true �� �Ѵ�.(�ð��������� �ٷ� �����Ǵ� ���)
	void RELEASE_SLOT_ITEM_SERVER( EMSLOT _slot, bool DbUpdate = false );

	// ����â���� �ִ´�. DbUpdate �� true �� ��� ��� �ٷ� ����
	// �ð� ���Ѿ������� �����Ǹ鼭 �ű⿡ �پ��ִ� �ڽ�Ƭ�� �ٷ� ����â���� ����.
	// �̷� ��� ��� �ٷ� �����Ѵ�.
	void SLOT_ITEM_SERVER( SITEMCUSTOM& sItemCustom, EMSLOT _slot, bool DbUpdate = false );

	void SlotUpdateItem( SITEMCUSTOM& sItemCustom, EMSLOT _slot, bool JustInvenTypeUpdate = false );
	    
	//! ������ ������ȣ�� �����Ѵ�
// 	void RegItemGen( __int64& GenNum, const SNATIVEID &sNID, EMITEMGEN emTYPE );
// 	void RegItemGen( sc::MGUID& guid );

	// InsertToInvenEx �� �������� Force ���� true �ѱ�� InsertToInvenEx ������ �Ѵ�.
	bool InsertToInven( CItemDrop* pItemDrop, bool bParty, bool bPickMsg, bool bDrop, bool Force = false, bool bNew = false );

	// �̺�Ʈ�� Ž�� ���� �ý��ۿ� ���� ����� ���� ���� ���Ѵ�.
	LONGLONG CalculateInsertmoney( LONGLONG lnAmount );

	void InsertMoney(LONGLONG lnAmount);
	bool InsertToInven( CMoneyDrop* pMoneyDrop );

	// ��ĥ �� �ִ� �������� �־��ִ� �Լ�
	// �����ۿ� ���� ����Ű�� �ִ�. sItemCustom �� �Ѱ��� �� guid �� �����ؼ� �ѱ��� �ʴ´�.
	// ��ħ ������ ���� ���� �������� ��������� ��� �� �Լ��ȿ��� guid �� �����ȴ�.
	// vecInsertedItems �� �����۷α� ����� ���� �뵵
	// ������ �α� �۾� �ٽ� �ؾ��Ѵ�. ���߿� �˻��ؼ� �ٽ� �־�� �Ѵ�.
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

	// â�� ������ ��� ������Ʈ�� �� ��� ȣ��
	// JustInvenTypeUpdate ��ǥ�� ������Ʈ �Ǵ� ��� true, false �� pInvenItem �������� ������Ʈ ����
	void UserStorageUpdateItem( DWORD Channel, SINVENITEM* pInvenItem, bool JustInvenTypeUpdate = false );

	// ������ ���� ó��, �ٴڿ� ������ ����� �� ���Ǹ�, �����δ� ��񿡼� ������ ���� ��ġ�� ����ȴ�.
	// ���� ����ϴ� �������� ����� �� ����ϴ°� �ƴ�, ĳ���� ������ �̵�(�κ�->�κ�, �κ�<->�κ�, ������Ŀ<->�κ� ���)�� ��
	// ���ܰ� �߻����� �� �ش� �������� �ٴڿ� ����ϴ� �뵵�� ���ȴ�.
	bool DropItem( SITEMCUSTOM& Item, const D3DXVECTOR3& Position, bool Drop = true );

	bool ItemSaleToNpc( WORD PosX, WORD PosY, bool bSendClient, BOOL bVietnam, bool DbUpdate );

    //! ������ �ŷ� �α�
	// ��� �� �α� ����
    void LogItemExchange(
        const SITEMCUSTOM& sITEM,
        gamelog::EMIDTYPE FromType, DWORD FromDbNum, // �����
        gamelog::EMIDTYPE ToType, DWORD ToDbNum, // ������
        EMITEM_ROUTE RouteType,
        int nNum, // �ŷ� ����
		const MapID &sMapID, 
		EMITEM_ROUTE_DETAIL emFlagDetail,
		bool bForceLog );
        //bool bForceLog = true );

    //! ���ӸӴ� �ŷ��α�
	// ��� �� �α� ����
    void LogMoneyExchange(
        gamelog::EMIDTYPE FromType, DWORD FromDbNum, // �����
        gamelog::EMIDTYPE ToType, DWORD ToDbNum, // ������
        LONGLONG lnPrice, EMITEM_ROUTE Flag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail);

	//! ���ӸӴ� �ŷ��α�
	// ��� �� �α� ����
	void LogMoneyExchangeWithItemGuid(
		gamelog::EMIDTYPE FromType, DWORD FromDbNum, 
		gamelog::EMIDTYPE ToType, DWORD ToDbNum, 
		LONGLONG lnPrice, EMITEM_ROUTE Flag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail, sc::MGUID& guid );

    //! ������ ��ȯ�α�
    void LogItemConversion(const SITEMCUSTOM& sITEM, gamelog::EMIDTYPE FromType, DWORD FromDbNum, EMITEM_ROUTE emFlag, EMITEM_ROUTE_DETAIL emFlagDetail);

    //! ������ �����ɼ� �α�
    void LogItemRandomOption(const SITEMCUSTOM& sITEM);
	void LogItemCostumeState(int iCharDbNum, const SITEMCUSTOM& sITEM);

	void LogItemAddonOption( const SITEMCUSTOM& sITEM );
	void LogItemLinkSkillOption( const SITEMCUSTOM& sITEM );
	void LogItemBasicStatOption( const SITEMCUSTOM& sITEM );

    //! ĳ���� Action Log
	// ��� �� �α� ����
    void LogCharAction(
        DWORD dwCI, gamelog::EMACTION emACT, 
        gamelog::EMIDTYPE emTAR, DWORD dwTAR, 
        __int64 nEXP, int nBRIGHT, int nLIFE, int nMONEY, const MapID &sMapID);

    void LogPetAction(int nPetNum, SNATIVEID nItemID, pet::EM_ACTION emAction, int nPetFull);

    void LogVehicleAction(int nVehicleNum, SNATIVEID nItemID, EMVEHICLE_ACTION emAction, int nVehicleFull);

	const ITEM_COOLTIME& GetCoolTime(DWORD dwCoolID, EMCOOL_TYPE emCoolType) const;

	// ���� ���ð� ���Ҵ���
    bool IsCoolTime(const SNATIVEID& sNativeID) const;
	bool CheckCoolTime(const SNATIVEID& sNativeID );		// ������ ���� ��Ÿ�� üũ
	bool SetCoolTime( SNATIVEID sNativeID , EMCOOL_TYPE emCoolType );	//	��Ÿ�� �ϰ� ����
	__time64_t	GetMaxCoolTime(const SNATIVEID& sNativeID) const;

	bool UpdateNpcCommission( LONGLONG lnCommission );
	
	//
	//mjeon.CaptureTheField
	//
	BOOL IsCTFPlayer();
	BOOL IsCTFPlayerNotCheckMap();
	BOOL IsCTFMap();
	DWORD GetCTFRebirthTime();

	// ���������� 
	BOOL	IsPrisonMap();

	//
	// Recall ó���� �Ϸ�Ǳ� ����, ��, Ÿ�� Field���� �̵��� �Ϸ�(�ش� �ʵ忡 GLChar ���� �Ϸ�)�Ǿ�����, ���� �߰����� �̵�ó���� �̷������ ������ ó���� �κ�.
	//
	void ToDoRightBeforeRecallDone( const GLLandMan* const pEnterLand, const MapID& mapIDTo/*������ MapID, �� method�� ȣ���ϴ� �������� m_sMapID�� �������� �̵��ϱ� ���� MapID�� ����Ų��.*/);
	//
	// Recall ó���� �Ϸ�Ǵ� ����, ��, Ÿ�� Field���� �̵��� �Ϸ�Ǿ ViewAround�� �����ϴ� ������ Ư���� ó�� �ؾ��� ������ �ִٸ�, ���⿡�� �ϵ��� ����.
	//
	void ToDoRightAfterRecallDone();

	const bool CheckOverStatPoint ();
	const bool CheckEquipedItems ( const bool _bPost = true );

	// ������� �ʴ� Ȧ������� ���� ��Ŷ�� ���� �����ؼ� ������� ���ϴ� �κ��丮 ���ο� �������� �־�� ĳ���͵��� ��������
	// �ٴڿ� �����Ų��.
	const bool CheckInvalidInventoryLine();

	void ReSetStartMap();

	virtual void Play_Disguise( int iIdx, BOOL bOn );
	virtual void Stop_Disguise( );

	virtual void Play_ActiveDamage();
	virtual void Stop_ActiveDamage() {};

	void UpdateSkillEff( float fElapsedTime );

public:
	// ĳ���� ����;
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
    
    //! ��ȯ�� ��û�� Ż�� �������� Mid/Sid
    SNATIVEID VehicleId() const;
    void VehicleIdSet(const SNATIVEID& VehicleId);
    
    DWORD VehicleDbNum() const;
    void  VehicleDbNumSet(DWORD DbNum);

    void VehicleUpdateTimeLmtItem(GLChar* pChar);
    
    const SITEMCUSTOM& VehiclePutOnItemRef(VEHICLE_ACCESSORYTYPE Type) const;
	
	// Ż�� ��������� ����
	// DbUpdate true �� �ٷ� ��񿡼� �����ȴ�.
    void VehiclePutOnItemReset( VEHICLE_ACCESSORYTYPE Type, bool DbUpdate = false );

	// ��� ������Ʈ(insert or update)
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

    //! true:DB���� ���� �������� ��, false:DB���� ���� �������� ����
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

    //! NPC �κ��� ������ ����
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
	
    //! �������� ������ ������ ��������
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

	//! Char Inven -> Locker, �� �ֱ�
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

	//! ĳ���� ��Ȱ ��û
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
     * redmine : #1161 Randombox Ȯ������ �ý��� ó��
     * created : sckim, 2015.12.16, Randombox Open�� Ȯ������ ���� ���� ��û ����
     * modified : sckim, 2016.09.20, Redmine #4544 Ȯ��/������ ����Ʈ ���� ��� ����(MsgReqInvenRandomBoxOpenFB �Լ� �Ķ���� ����)
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
	
	//! Ŭ��������û
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
    
    //! �տ� ����ִ� ������ Ŭ��â�� ����
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

	HRESULT MsgReqGarbageResult( NET_MSG_GENERIC* nmg );	// ������ ���

	//HRESULT MsgReqPhoneNumber(NET_MSG_GENERIC* nmg);
	//HRESULT MsgPhoneNumber(NET_MSG_GENERIC* nmg);
	HRESULT MsgSendSMS( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqSendSMS( NET_MSG_GENERIC* nmg );

	HRESULT MsgItemShopOpen(NET_MSG_GENERIC* nmg);	// ItemShopOpen
	HRESULT MsgReqItemMix( NET_MSG_GENERIC* nmg );	// ItemMIx

	HRESULT MsgReqDiceDetNumber(NET_MSG_GENERIC* nmg); // �ֻ��� ����;

	void CallbackProduct();

	HRESULT MsgReqDialogueWindowOpen(NET_MSG_GENERIC* nmg);	
	HRESULT MsgReqNpcTalkBasic(NET_MSG_GENERIC* nmg);

	// PET
	HRESULT MsgReqGetRightOfItem( NET_MSG_GENERIC* nmg );
	HRESULT	MsgGiveFood( NET_MSG_GENERIC* nmg );
	HRESULT MsgGetPetFullFromDB( NET_MSG_GENERIC* nmg );
	HRESULT	MsgPetReviveInfo( NET_MSG_GENERIC* nmg );

	// �ڽ�Ƭ ����. by luxes.
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

    // �����.
    HRESULT MsgBuyRebuyItem( const SITEMCUSTOM& sItemCustom, LONGLONG Price );

	void SendTriggerMessage(TriggerSystem::SMessage* msg);
	void PostTriggerMessage(TriggerSystem::SpMessage msg);

	// PVE ������ ���
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

	// ����ġ ���� / ĸ��
	int MsgReqExpCompressor(NET_MSG_GENERIC* nmg, int &nType, SNATIVEID &sCapsuleItem);
	HRESULT MsgReqExpCapsule(NET_MSG_GENERIC* nmg);

	// Macro;
	HRESULT MsgReqMacroModeOnoffCF(NET_MSG_GENERIC* nmg);
	void	MacroOnOff(bool bOn);
	void	UpdateMacroState( float fElapsedTime );
	
	DWORD	GetNearestEnemy(void);
	BOOL LandNodeCheckHierarchy(LANDQUADNODE* pQuadNode, GLCROWLIST* crowList);
	BOOL LandNodeCheckLowChunk (LANDQUADNODE* pQuadNode, GLCROWLIST* crowList);//, GLCROWLIST& crowList);
	
	// ��ųui���� ��ų�ʱ�ȭ ��ư ������ ��
	HRESULT MsgReqResetSkillStatsBySkillUI( NET_MSG_GENERIC* nmg );
public:
	//HRESULT MsgProcess(NET_MSG_GENERIC* nmg);	

protected:
	void UpdateActState(void);
	HRESULT UpdateClientState( float fElapsedTime );
	HRESULT UpdateVehicle( float fElapsedTime );
	HRESULT UpdateBooster( float fElapsedTime );
	// ������ ���� �̻�ȿ�� 
	void	UpdateLandEffect();
	void	UpdateMapEffect();

public:
	// ���� �׼� ���� ( Reserve Action );
	void	ReserveAction( EMRESERVED_ACTION emReservedAction );
	void	CancelReserveAction( EMRESERVED_ACTION emReservedAction );
	void	CancelAllReserveAction();

public:
	BOOL			ModifyExpiredTimeHostile( const DWORD dwCharID, const float fExtendTime );
	void			RemoveOverTimeHostile( const float fElapsedTime );
	void			Confronting( const float fElapsedTime );
	BOOL			CheckConfronting();

	// ���� �׼� ���� ( Reserve Action );
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

    // �������, �о�� Ȯ�� ��� �� �Ÿ� Ȯ��
    const bool CheckHit(const STARGETID& cTargetID, const BOOL bLowSP, const DWORD dwApplyFlag);

    //! ĳ���Ͱ� ���� ���������� ������ ��� ����
	DWORD CALCDAMAGE(
		int &rResultDAMAGE,
		const DWORD	dwGaeaID,
		const STARGETID &cTargetID,
		const GLLandMan* pLandMan,
		const GLSKILL* pSkill=NULL,
		const DWORD dwskill_lev=0,
		const DWORD dwWeatherFlag=NULL,
		const DWORD dwDivCount=0);

	// ����
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

    //! Private Market/���λ���/���� ����
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

	// �������
	void RanMobileCharacterConnectionFee( LONGLONG Price , bool bExtend );
	void RanMobileDailyBonus( const SNATIVEID& ItemID );
	bool RanMobileDeleteInvenItem( int nType, const SNATIVEID& ItemID, int nCount ); // ���� (1) �ƴϸ� ����� (2)
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
