#ifndef _GL_AGENT_SERVER_H_
#define _GL_AGENT_SERVER_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <map>
#include <deque>
#include <hash_set>

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/G-Logic/DxMsgServer.h"
#include "../../EngineLib/G-Logic/DxConsoleMsg.h"
#include "../../RanLogic/PVE/PVEInDunID.h"

#include "../../RanLogic/Guidance/GLGuidanceDefine.h"
#include "../Land/GLMapListServer.h"
#include "../Land/GLAGLandMan.h"
#include "../Character/GLCharAG.h"
#include "../Period/GLAGPeriod.h"
#include "../Database/DbManager.h"
#include "../InstanceSystem/LogicAgent/InstanceSystemAgent.h"
#include "../Event/EventAgent/EventAgent.h"

#include "../../RanLogic/Network/NetLogicDefine.h"
#include "../../RanLogic/Event/GLIPEvent.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

class GLClubManAgent;
class GLClubAgent;
class GLGuidanceAgentMan;
class GLClubDeathMatchAgentMan;
class GLGuidance;
class GLPartyManagerAgent;
struct GLPartyAgent;
class GLExpeditionAgent;
class GLFreePK;
class GLAutoLevelMan;
class GLMapLayerMan;
class GLSchoolFreePKAgent;
class GLItemLimit;
class CaptureTheFieldManAgt;
class ChatManager;

struct CHAR_BASE_CACHE_DATA;

namespace InstanceSystem
{
	class ManagerAgent;
}
namespace pointshop {
    class PointShopAgent;
}

namespace buffmanager {
    class GLBuffManagerAG;
}

namespace private_market
{
	class GLSearchDelayMgr;
}
namespace MatchingSystem
{
	class MatchingCore;
}
namespace StateFieldServer
{
	class Manager;
}

namespace TexasHoldem
{
	class GLTexasHoldemAgent;
}

namespace TexasHoldem
{
	class GLTexasHoldemAgent;
}

class GLJackpotMgr;

typedef std::set<DWORD>	   MAPIDSET;
typedef MAPIDSET::iterator MAPIDSET_ITER;

typedef std::set<WORD>				SET_INSTANCE_SID;
typedef std::set<WORD>::iterator	SET_INSTANCE_SID_ITER;

typedef std::vector<DWORD>				MAP_PLAYER_NUM_VEC;
typedef MAP_PLAYER_NUM_VEC::iterator	MAP_PLAYER_NUM_VEC_ITER;

typedef std::map<DWORD, GLAGLandManager*>				MAP_LANDMANAG;
typedef std::map<DWORD, GLAGLandManager*>::iterator		MAP_LANDMANAG_ITER;
typedef std::map<DWORD, GLAGLandManager*>::value_type	MAP_LANDMANAG_VALUETYPE;

struct SFIELDSERVER_INFO
{
	int		nChannelNum;
	int		nFieldNum;

	SFIELDSERVER_INFO ()
		: nChannelNum ( MAX_CHANNEL_NUMBER )
		, nFieldNum ( FIELDSERVER_MAX )
	{

	}

	const bool IsValid () const
	{
		if ( nChannelNum == MAX_CHANNEL_NUMBER || nFieldNum == FIELDSERVER_MAX )
			return false;

		return true;
	}
};

typedef std::vector< SFIELDSERVER_INFO >			VEC_FIELDSERVER_INFO;
typedef VEC_FIELDSERVER_INFO::const_iterator		VEC_FIELDSERVER_INFO_CITER;

struct STestInstantMapCharInfo 
{
	DWORD		dwGaeaID;
	SNATIVEID	sBaseMapID;
	float		fElapsedTime;
	float		fCreateTime;

	STestInstantMapCharInfo() 
		: dwGaeaID(0)
		, sBaseMapID(NATIVEID_NULL())
		, fElapsedTime(0.0f)
		, fCreateTime(0.0f)
	{
	}

};

typedef std::queue<DWORD> FreeIDQueue;

enum EVENT_TYPE
{
	EVENT_EXP = 0,
	EVENT_ITEM = 1,
	EVENT_MONEY = 2,
	EVENT_EXTYPE = 3,
	EVENT_LIMIT = 4,
	EVENT_CLASS_EXP = 5,
	EVENT_CLASS_ITEM = 6,
	EVENT_CLASS_MONEY = 7,
	EVENT_GRADE = 8,
    EVENT_IP = 9,
	EVENT_TYPE_SIZE = 10
};

enum RESERVED_TASK_TYPE
{
	RESERVED_TASK_PARTY_CLEAR, 
	RESERVED_TASK_SIZE, 
};

struct SWorkingEvent
{

	GLMSG::NET_GM_EVENT_EXP_CAF		 exp_Event;
	GLMSG::SNET_GM_EVEN_ITEM_GEN	 item_Event;
	GLMSG::SNET_GM_EVEN_MONEY_GEN    money_Event;
	GLMSG::SNET_GM_CLASS_EVENT		 class_Event[3];
	GLMSG::SNET_GM_EVENT_EX			 ex_Event;	
	GLMSG::SNET_GM_LIMIT_EVENT_BEGIN limit_Event;
	GLMSG::SNET_GM_EVENT_GRADE		 grade_Event;
    GLMSG::SNETPC_GM_SET_IPEVENT ip_Event;
	bool bWorkEvent[EVENT_TYPE_SIZE];

	SWorkingEvent::SWorkingEvent()
	{
		Init();
	}

	void SWorkingEvent::Init()
	{
		memset( bWorkEvent, 0, sizeof ( bool ) * EVENT_TYPE_SIZE );	
	}

	void operator=(G_AUTH_EVENT_INFO& rhs)
    {
		rhs.eventExp = exp_Event.m_Scale;
		rhs.eventItem = item_Event.fRATE;
		rhs.eventMoney = money_Event.fRATE;
		rhs.eventGrade = grade_Event.fRATE;

		if (ex_Event.emType & EMGM_EVENT_SPEED)
			rhs.eventSpeedEx = ex_Event.wValue;
		if (ex_Event.emType & EMGM_EVENT_ASPEED)
			rhs.eventAtkSpeedEx = ex_Event.wValue;
		if (ex_Event.emType & EMGM_EVENT_ATTACK)
			rhs.eventAtkRateEx = ex_Event.wValue;

		rhs.limitStartLv = limit_Event.start_Lv;
		rhs.limitEndLv = limit_Event.end_Lv;
		rhs.limitPlayTime = limit_Event.play_Time;
		rhs.limitBusterTime = limit_Event.buster_Time;
		rhs.limitItemGainRate = limit_Event.itemGain_Rate;
		rhs.limitExpGainRate = limit_Event.expGain_Rate;
		rhs.limitEventMinute = limit_Event.dwEventMinute;

		rhs.eventClassExp = class_Event[0].dwClassIndex;
		rhs.eventClassExpRate = class_Event[0].fEventRate;
		rhs.eventClassExpStartLv = class_Event[0].start_Lv;
		rhs.eventClassExpEndLv = class_Event[0].end_Lv;

		rhs.eventClassItem = class_Event[1].dwClassIndex;
		rhs.eventClassItemRate = class_Event[1].fEventRate;
		rhs.eventClassItemStartLv = class_Event[1].start_Lv;
		rhs.eventClassItemEndLv = class_Event[1].end_Lv;

		rhs.eventClassMoney = class_Event[2].dwClassIndex;
		rhs.eventClassMoneyRate = class_Event[2].fEventRate;
		rhs.eventClassMoneyStartLv = class_Event[2].start_Lv;
		rhs.eventClassMoneyEndLv = class_Event[2].end_Lv;

		rhs.ipEventAttackValue = ran_event::GLIPEvent::Instance()->GetAttackValue();
		rhs.ipEventAttackSpeed = ran_event::GLIPEvent::Instance()->GetAttackSpeed();
		rhs.ipEventMoveSpeed = ran_event::GLIPEvent::Instance()->GetMoveSpeed();
		rhs.ipEventItemDrop = ran_event::GLIPEvent::Instance()->GetItempDrop();
		rhs.ipEventMoneyDrop = ran_event::GLIPEvent::Instance()->GetMoneyDrop(); 
		rhs.ipEventExp =ran_event::GLIPEvent::Instance()->GetExp();
		rhs.ipEventHPRecovery = ran_event::GLIPEvent::Instance()->GetHPRecovery();
		rhs.ipEventMPRecovery = ran_event::GLIPEvent::Instance()->GetMPRecovery();
		rhs.ipEventSPRecovery = ran_event::GLIPEvent::Instance()->GetSPRecovery();
		rhs.ipEventAllRecovery = ran_event::GLIPEvent::Instance()->GetAllRecovery();
		rhs.ipEventCriticalRate = ran_event::GLIPEvent::Instance()->GetCriticalRate();
    }
};

struct EncryptPack
{
	DWORD dwMasterClientSlot;
	DWORD dwTimeKey;
	DWORD dwTargetClientSlot;
};

class AgentCharMan;

class GLAgentServer
{
public:
	typedef std::deque<DWORD>	PCGID;
	typedef PCGID::iterator		PCGID_ITER;

    typedef boost::unordered_map<DWORD,DWORD> CLIENTMAP;
	typedef CLIENTMAP::iterator				  CLIENTMAP_ITER;
    typedef CLIENTMAP::const_iterator		  CLIENTMAP_CITER;
    typedef CLIENTMAP::value_type             CLIENTMAP_VALUE;

	typedef std::map<std::string, GLCharAG*>	CHAR_MAP;
	typedef CHAR_MAP::iterator					CHAR_MAP_ITER;

	typedef std::vector<DWORD>					VEC_INSTANT_MAPID;
	typedef VEC_INSTANT_MAPID::iterator			VEC_INSTANT_MAPID_ITER;

	typedef std::list<SENTRYLIMITINFO>			LIST_ENTRYINFO;
	typedef LIST_ENTRYINFO::iterator			LIST_ENTRYINFO_ITER;

	typedef std::map<DWORD, LIST_ENTRYINFO> MAP_ENTRYLIMIT;
	typedef MAP_ENTRYLIMIT::iterator		MAP_ENTRYLIMIT_ITER;

	typedef std::map<DWORD, __time64_t>		 MAP_PARTY_RECRUIT_TIME;
	typedef MAP_PARTY_RECRUIT_TIME::iterator MAP_PARTY_RECRUIT_TIME_ITER;

    typedef boost::unordered_map<DWORD, GLCharAG*> AGENT_CHAR_DATA;
    typedef AGENT_CHAR_DATA::iterator              AGENT_CHAR_DATA_ITER;
    typedef AGENT_CHAR_DATA::const_iterator        AGENT_CHAR_DATA_CITER;
    typedef AGENT_CHAR_DATA::value_type            AGENT_CHAR_DATA_VALUE;

    typedef std::map<DWORD, GLAGLandManager*> LandManagerMap;
    typedef LandManagerMap::const_iterator LandManagerMapCIter;
    typedef LandManagerMap::iterator LandManagerMapIter;
    typedef std::pair<DWORD, GLAGLandManager*> LandManagerPair;

public:
    GLAgentServer(const bool bEmulator = false);
    ~GLAgentServer();

private:
    GLGuidanceAgentMan*     m_pGuidance;
	DxMsgServer*			m_pMsgServer;
	//DxConsoleMsg*			m_pConsoleMsg;
	db::IDbManager*			m_pDBMan;
	db::IDbManager*			m_pAdoMan;
    GLClubDeathMatchAgentMan* m_pClubDeathMatchMan;

	CRITICAL_SECTION		m_CSPCLock;

	DWORD					m_dwMaxClient;
	//PGLCHARAG*			m_PCArray;				//!	PC 배열(삭제, m_CharData 로 변경) 2012-05-08 jgkim
    AGENT_CHAR_DATA         m_CharData;             //! Characters
	PCGID					m_FreePCGIDs;			//	미사용된 PC GlobID 들.
	DWORD					m_dwBaseGaeaId;			// 이 서버군의 기준(시작) GaeaId

	//AGCHARLIST			m_PCList;				//	활동중인 케릭터 리스트.(삭제, m_CharData 로 변경) 2012-05-08 jgkim

	SWorkingEvent			m_WorkEvent;			//  현재 적용중인 이벤트들

    AgentCharMan*           m_pCharMan;
	CHAR_MAP				m_CharNameMap;			//! 캐릭터이름/캐릭터 Pointer
	
	//mjeon.groupchat
	ChatManager				*m_pGroupChatMan;			//GroupChat mananger

	
	PCGID					m_RegDropOutPC;

	InstanceSystem::ManagerAgent* m_pInstanceManager; //! 인스턴스 던전 관리;

	MatchingSystem::MatchingCore* m_pMatchingCore; // 메칭 관리

	GLEvent::ManagerAgent* m_pEventManager; //! 이벤트 매니저;

	GLPartyManagerAgent*  			m_pPartyManager;	
	GLClubManAgent*   		m_pClubMan;
	GLFreePK*				m_pFreePK;				// 성향 이벤트용
	GLAGPeriod				m_cAGPeriod;			// 주기/이벤트 용
    GLAutoLevelMan*		m_pAutoLevel;
	GLMapLayerMan*		m_pMapLayer;
    GLSchoolFreePKAgent*    m_pSchoolFreePk;
    GLItemLimit*            m_pItemLimit;
    pointshop::PointShopAgent* m_pPointShop;
	private_market::GLSearchDelayMgr* m_pPrivateMarketSearchDelayMgr;
	private_market::GLSearchDelayMgr* m_pPrivateMarketLogDelayMgr;
	StateFieldServer::Manager* m_pStateFieldServer;
	TexasHoldem::GLTexasHoldemAgent* m_pTexasHoldemMgr;
	GLJackpotMgr* m_pJackpot;

	float					m_fPERIOD_TIMER;	
	CTime					m_PresetTime;
	SEventState				m_sEventState;
	MAP_ENTRYLIMIT			m_mapEntryLimitInfo;
	MAP_PARTY_RECRUIT_TIME	m_mapPartyRecruitTime;	//	파티 모집 채팅 제한 시간 
	float					m_fPartyRecruitTime;	//	파티 모집 채팅 제한 갱신 시간
	bool					m_bClubMasterMandate;	//  클럽마스터 위임 on/off
	bool					m_pFreePkGMCommand;
	bool					m_bEmulator;

	std::list<STestInstantMapCharInfo> m_listTestCreateInstMapInfo;
	EMSERVICE_PROVIDER		m_ServiceProvider;
	CaptureTheFieldManAgt	*m_pCTFMan;

	DWORD					m_dwSvrGroupNum;

    //
    //bjju.buffmanager
    //
    buffmanager::GLBuffManagerAG* m_pBuffMan;


	struct SPartyInfoBoardData
	{
		SPartyInfoBoardData()
		{
			m_MapID = (DWORD)-1;
		}

		DWORD		m_MapID;
		std::string m_MasterName;
		std::string m_Title;
	};

	typedef boost::multi_index_container<
		SPartyInfoBoardData,
		boost::multi_index::indexed_by<
		boost::multi_index::ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(SPartyInfoBoardData,DWORD,m_MapID)>,
		boost::multi_index::ordered_unique<BOOST_MULTI_INDEX_MEMBER(SPartyInfoBoardData,std::string,m_MasterName)> 
		>
	> PartyInfoBoardMultiIndex;

	typedef PartyInfoBoardMultiIndex::nth_index<0>::type  PartyInfoBoardIndexMapID;
	typedef PartyInfoBoardMultiIndex::nth_index<1>::type  PartyInfoBoardIndexName;

	PartyInfoBoardMultiIndex m_PartyInfoBoardTable;

	std::vector< RESERVED_TASK_TYPE > m_ReservedTask;	// 예약된 작업 리스트

	std::vector< int > m_AutoJoinClub;

private:
	// 정적 Map Data;
	GLMapListServer			m_sMapList;

	// Agent 에서 관리되는 Land 정보;
	// 동적 Land 와 정적 Land 가 같이 관리되고 있기 때문에 사용에 주의가 필요하다;
	// 이전작업자가 관리하기 편하도록 이렇게 해놓은것같다;
    LandManagerMap	        m_mapLandMan;

	// 인던 맵 아이디 벡터;
    VEC_INSTANT_MAPID		m_vecInstantMapId;

private:
    DWORD m_dwFIELDSVR_NUM;
	DWORD m_dwFIELDSVR_NUM_PER_CHANNEL[MAX_CHANNEL_NUMBER];
    bool m_bFIELDSVR[MAX_CHANNEL_NUMBER][FIELDSERVER_MAX];
	bool m_bFIELDSVR_ONLINE[MAX_CHANNEL_NUMBER][FIELDSERVER_MAX];

	VEC_FIELDSERVER_INFO	m_vecCreatableInstanceField;

private:
	bool StartPartyConfront(const GLPartyID PartyID_A, const GLPartyID PartyID_B,
		const SNATIVEID &sMapID, const SCONFT_OPTION &sOption,
		const DWORD* pMBR_A, const DWORD* pMBR_B, const D3DXVECTOR3& vPos );

    //void DeleteAllCharacterData();
    //void RegistCharacterData(GAEAID GaeaId, GLCharAG* pChar);
    //void UnRegistCharacterData(GAEAID GaeaId);

public:
	void InsertAutoJoinClub( int ClubNum );
	bool IsAutoJoinClub( int ClubNum );

public:
    void AddGameDbJob(db::DbActionPtr spJob);
	//mjeon.ado
	void AddGameAdoJob(db::DbActionPtr spJob);
	void AddLogAdoJob(db::DbActionPtr spJob);
	void AddUserAdoJob(db::DbActionPtr spJob);
	void AddShopAdoJob(db::DbActionPtr spJob);

	HRESULT StartClubBattle( DWORD dwClubID, GLCLUBBATTLE sClubBattle );
    GLGuidanceAgentMan* Guidance();
    bool GuidanceIsBattle() const;
    GLGuidance* GuidanceFindByClubID(DWORD ClubId);
    DWORD GuidanceId(DWORD ClubId) const;
    bool ClubDeathMatchIsBattle() const;
	const bool IsClubDeathMatchMap(const SNATIVEID& _baseMapID);

    GLItemLimit* GetItemLimit() { return m_pItemLimit; }

	bool IsAgentOnline ( DWORD dwAgentNum );

	const bool IsSubServer ();

	// 어떤 Client Slot 인가?;
	const bool IsAgentClientSlot ( DWORD _dwClientSlotID );
	const bool IsFieldClientSlot ( DWORD _dwClientSlotID );

	// Agent 서버군에 따른 Client Slot ID 정보;
	const DWORD GetServerGroup_ClientSlotID ( DWORD _dwClientSlotID );
	const DWORD GetClientSlotID_ServerGroup ( DWORD _dwServerGroup );

	bool IsFieldOnline(DWORD Channel, DWORD FieldNum);
    void SetFieldOnline(DWORD Channel, DWORD FieldNum, bool Set);
	void ChangeChannelUser( DWORD dwClient, int nPreChannel, int nAfterChannel );
	const int GetServerGroup(void) const { return m_pMsgServer->GetServerGroup(); }

    //! 오프라인 친구 요청
    void FriendOfflineRequest(const std::string& ReqCharName, const std::string& TargetCharName );
	void SetClubMasterMandate( bool bBool );
	bool IsClubMasterMandate();
	void SetFreePkGMCommand( bool bBool );
	bool IsFreePkGmCommand();
	void SetFreePkMode( bool bBool );

	void registTimerEvent(const callbackTimerFunction& _functionCallbackTimer, Timer::Parameter& _parameter, const float _fTimeInterval);
	void registTimerEvent(const unsigned int _dbNum, const callbackTimerFunction& _functionCallbackTimer, Timer::Parameter& _parameter, const float _fTimeInterval);

	void ChaSaveUserNumAdd( DWORD dwUserNum );
	void ChaSaveUserNumDel( DWORD dwUserNum );

public:
	DxMsgServer* GetMsgServer ()	{ return m_pMsgServer; }
	EMSERVICE_PROVIDER GetServiceProvider() {return m_ServiceProvider;}

	DWORD GetSvrGroupNum() {return m_dwSvrGroupNum;}

	const VEC_FIELDSERVER_INFO& GetVector_CreatableInstanceField () { return m_vecCreatableInstanceField; }

public:
	void SENDTOCLIENT(NET_MSG_GENERIC* nmg, DWORD dwChaNum);	//mjeon.AF
    void SENDTOCLIENT(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, DWORD dwChaNum, bool UseCrc=false);
    void SENDTOCLIENT(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, DWORD dwChaNum, bool UseCrc=false);

	void SENDTOCLIENT(DWORD dwClientID, NET_MSG_GENERIC* nmg);
    void SENDTOCLIENT(DWORD dwClientID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
    void SENDTOCLIENT(DWORD dwClientID, EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);
	
	void SENDTOAGENT(NET_MSG_GENERIC* pMsg);
	void SENDTOAGENT(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
	void SENDTOAGENT(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	void SENDTOAGENT(DWORD dwClientID, NET_MSG_GENERIC* pMsg);
	void SENDTOAGENT(DWORD dwClientID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
	void SENDTOAGENT(DWORD dwClientID, EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	int SENDTOAGENT_SERVERGROUP(DWORD dwServerGroup, NET_MSG_GENERIC* pMsg);
	int SENDTOAGENT_SERVERGROUP(DWORD dwServerGroup, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
	int SENDTOAGENT_SERVERGROUP(DWORD dwServerGroup, EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	int SENDTOWORLDBATTLE ( NET_MSG_GENERIC* pMsg );
	int SENDTOWORLDBATTLE ( EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false );
	int SENDTOWORLDBATTLE ( EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false );

	void SENDTOFIELD(DWORD dwClientID, NET_MSG_GENERIC* nmg);
    void SENDTOFIELD(DWORD dwClientID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
    void SENDTOFIELD(DWORD dwClientID, EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	void SENDTOFIELD(DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* nmg);	//mjeon.SendField를 사용하도록 구현 : 메시지 압축을 하지 않는다.
    void SENDTOFIELD(DWORD dwChannel, DWORD dwFieldSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
    void SENDTOFIELD(DWORD dwChannel, DWORD dwFieldSvrNum, EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	void SENDTOFIELDSVR(int nChannel, int nField, NET_MSG_GENERIC* nmg); //SendClient를 사용: 메시지에 대해 압축을 수행한다.
    void SENDTOFIELDSVR(int nChannel, int nField, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
    void SENDTOFIELDSVR(int nChannel, int nField, EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	void SENDTOCHANNEL(NET_MSG_GENERIC* nmg, int nChannel);
    void SENDTOCHANNEL(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nChannel, bool UseCrc=false);
    void SENDTOCHANNEL(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, int nChannel, bool UseCrc=false);

	int SENDTOALLCHANNEL(NET_MSG_GENERIC* nmg);
    int SENDTOALLCHANNEL(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
    int SENDTOALLCHANNEL(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	void SENDTOALLCLIENT(NET_MSG_GENERIC* nmg);
    void SENDTOALLCLIENT(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
    void SENDTOALLCLIENT(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

	void SENDTOALLCLIENT(NET_MSG_GENERIC* nmg, int nChannel);
    void SENDTOALLCLIENT(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nChannel, bool UseCrc=false);
    void SENDTOALLCLIENT(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, int nChannel, bool UseCrc=false);

	void SENDTOPARTY(const GLPartyID& PartyID, NET_MSG_GENERIC* nmg);    
    void SENDTOPARTY(const GLPartyID& PartyID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
	void SENDTOPARTY(DWORD dwGaeaID, const GLPartyID& PartyID, NET_MSG_GENERIC* nmg);
    
	void SENDTOCLUBCLIENT(DWORD dwClubID, NET_MSG_GENERIC* nmg);
    void SENDTOCLUBCLIENT(DWORD dwClubID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);

	void SENDTOALLIANCECLIENT(DWORD dwClubID, NET_MSG_GENERIC* nmg);
    void SENDTOALLIANCECLIENT(DWORD dwClubID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);

    void SENDTOCACHE(NET_MSG_GENERIC* pMsg);
    void SENDTOCACHE(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
    void SENDTOCACHE(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc=false);

    void SENDCLUBLOG(DWORD ClubDbNum, club::EM_LOG_TYPE LogType, const std::string& LogMsg);

	void SENDTOMATCH(NET_MSG_GENERIC *msg);
    void SENDTOMATCH(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);

	void SENDTOMYSELF(NET_MSG_GENERIC *msg);
	void SENDTOMYSELF(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);

	/* ---------------------------------------------------------------------------------------------------- */
	// Agent Relay Message 관련 ( Agent 통신을 이용하는 Message 의 경우 사용해야한다 );
	void SENDTOCLIENT_RELAY ( DWORD _dwDbNum, DWORD _dwClientID, NET_MSG_GENERIC_RELAY_AGENT* _pMsg );
	void SENDTOCLIENT_RELAY_BYSVRGROUP ( DWORD _dwDbNum, DWORD _dwServerGroup, NET_MSG_GENERIC_RELAY_AGENT* _pMsg );

	void SENDTOFIELD_RELAY ( DWORD _dwDbNum, DWORD _dwClientID, NET_MSG_GENERIC_RELAY_AGENT* _pMsg );
	void SENDTOFIELD_RELAY_BYSVRGROUP ( DWORD _dwDbNum, DWORD _dwServerGroup, NET_MSG_GENERIC_RELAY_AGENT* _pMsg );

	void SENDTOALLCLIENT_RELAY ( NET_MSG_GENERIC_RELAY_AGENT* _pMsg );

	/* ---------------------------------------------------------------------------------------------------- */

	// 배틀 진행시 party 멤버 확인
	void ClubBattlePartyCheck( DWORD dwClubID, DWORD dwClubIDTar );
	void AllianceBattlePartyCheck( DWORD dwClubID, DWORD dwClubIDTar );

	BOOL SendToGMMsgLink( PGLCHARAG pChar, GLMSG::NET_CHAT_LINK_FB* pNetChatFB );

public:
	bool IS_FIELDSERVER_ONLINE ( const UINT32 nChannel, const UINT32 nFieldServer ) { return m_bFIELDSVR[nChannel][nFieldServer]; }

public:
    //! Gaea ID 로 캐릭터를 찾는다
	GLCharAG* GetChar(const GAEAID dwGaeaID);
    inline GLCharAG* GetCharByGaeaID(const GAEAID dwGaeaID) 
    {
        return GetChar(dwGaeaID);
    }

    //! 캐릭터 이름으로 캐릭터를 찾는다
	GLCharAG* GetChar(const char* szName);
    GLCharAG* GetChar(const std::string& ChaName);

    //! 사용자 ID 로 캐릭터를 찾는다
    GLCharAG* GetCharByUserAccount(const std::string& UserId);

    //! 캐릭터 DB 번호로 캐릭터를 찾는다
	GLCharAG* GetCharByDbNum(const DWORD ChaDbNum);
	GLCharAG* GetCharByUserDbNum(const DWORD UserDbNum);

    //! 캐릭터 이름/캐릭터 DB 번호, 캐릭터 DB 번호/캐릭터 이름
    void ChaNameDbNumAdd(
        const std::string& ChaName,
        DWORD ChaDbNum,
        DWORD UserDbNum,
        EMUSERTYPE UserType);

	void ChaNameDbNumAdd(
		const std::string& ChaName,
		DWORD ChaDbNum,
		DWORD UserDbNum,
		EMUSERTYPE UserType,
		int iClass,
		int iLv);


    //! 캐릭터 이름/캐릭터 DB 번호
    DWORD GetChaDbNumByChaName(const std::string& ChaName);
    
    //! 캐릭터 DB 번호/캐릭터 이름
    std::string GetChaNameByDbNum(DWORD ChaDbNum);

    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> GetCharCacheData(const std::string& ChaName);
    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> GetCharCacheData(DWORD ChaDbNum);
    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> GetCharCacheData(const std::string& ReqCharName, const std::string& ChaName);


	Country::SCOUNTRY_INFO GetCountry( DWORD ChaDbNum ) const;

	DWORD GetFieldServer ( const SNATIVEID &sMapID );
	GLAGLandMan* GetLand(const MapID& sMapID);
    GLAGLandMan* GetLand(const SNATIVEID& sMapID);
	GLAGLandManager* GetLandManager(const MapID& sMapID);
    GLAGLandManager* GetLandManager(const SNATIVEID& sMapID);
    GLAGLandManager* GetLandManager(const DWORD dwMapID);
	db::IDbManager* GetDBMan() { return m_pDBMan; }
	db::IDbManager* GetAdoMan() { return m_pAdoMan; }	
	const char* GetMapName(SNATIVEID sMapID)	{ return m_sMapList.GetMapName(sMapID); }
    std::string GetMapNameStr(const SNATIVEID& MapID) { return m_sMapList.GetMapNameStr(MapID); }
	const SNATIVEID GetToMapIDByGateID(const DWORD dwGaeaID, const DWORD dwGateID);
	SMAPNODE* const FindMapNode(const SNATIVEID& MapID) { return m_sMapList.FindMapNode(MapID); }
	SMAPNODE* const FindMapNode(const MapID& sMapID) { return m_sMapList.FindMapNode( sMapID.getBaseMapID() ); }

	StateFieldServer::Manager* const getStateFieldServer(void) const { return m_pStateFieldServer; }

	//! Instance -------------------------------------------------------------
	const InstanceSystem::ManagerAgent* const GetInstanceManager(void) const { return m_pInstanceManager; }
	InstanceSystem::ManagerAgent* const getInstanceManager(void) { return m_pInstanceManager; }

	//! Event ----------------------------------------------------------------
	const GLEvent::ManagerAgent* const getEventManager(void) const { return m_pEventManager; }
	GLEvent::ManagerAgent* const getEventManager(void) { return m_pEventManager; }

	//! Party -----------------------------------------------------------------
    const GLPartyAgent* const GetParty(const GLPartyID& PartyID) const;
	const GLExpeditionAgent* const GetExpedition(const GLPartyID& PartyID) const;
	const bool OfflinePartyMember(const DWORD dwGaeaID, const DWORD dwDbNum, const GLPartyID& PartyID);	
	BOOL CompareTargetSlot( DWORD dwClientID, DWORD dwTargetGaeaID );

    BOOL JoinParty(const DWORD dwMasterGaeaID, const DWORD dwTargetGaeaID, const SPartyOption& PartyOption, const bool bUseScript = true);
    void QuitParty(GLCharAG *pCharAG, const bool bUseScript = true, EMPARTY_ACTION_TYPE eType = PARTY_ACTION_NON_TYPE);
	const bool DeleteParty(const GLPartyID _partyID, const bool bUseScript = true);
    
    void QuitPartyMap( GLCharAG* pChar );    
    
    BOOL MsgPartyLureTarAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgPartyLinkLure(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
		
	//  [1/11/2013 gbgim];
	// - 파티장 초대를 통한 가입 경로;
	BOOL MsgPartyLure(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgPartyLureWithCharNum(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	// - 링크를 통한 가입 경로;
	BOOL MsgPartyLinkJoin(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);    
	// - 기존에는 위의 두가지 타입에 따라 모두 예외처리를 따로해주엇는데;
	//   불편해서 하나로 묶은 함수가 바로 MsgPartyInviteRequest;
		BOOL MsgPartyInviteRequest(
			const DWORD dwMasterGaeaID,
			const DWORD dwCharTarGaeaID,
			const SPartyOption& PartyOption,
			const bool bClient2Master = true);

    BOOL MsgPartyLinkRefuse(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgPartySecede(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgPartyAuthority(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgPartyDissolve(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgPartyInfoReqCA(NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgExpeditionNew(NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgPartyChangeOption(NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgExpeditionCheckReadyRequest(NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgExpeditionCheckReadyAnswer(NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgExpeditionSwapSlot(NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID);	


    //! Club ------------------------------------------------------------------
	GLClubManAgent* GetClubMan();
    std::tr1::shared_ptr<GLClubAgent> GetClub(DWORD ClubId);
    //! 주의:속도느림
    std::tr1::shared_ptr<GLClubAgent> GetClub(const std::string& ClubName);

	DWORD GetFieldNum() const { return m_dwFIELDSVR_NUM; }
	DWORD GetFieldNum(const unsigned int _nChannel) const 
	{
		if ( _nChannel < MAX_CHANNEL_NUMBER )
			return m_dwFIELDSVR_NUM_PER_CHANNEL[_nChannel];
		return 0;
	}
    void SetCurrentField(GLCharAG* pChar, DWORD dwSvr, const MapID& mapID);

    void RegisterCharacter(DWORD GaeaId, GLCharAG* pChar);
    void UnregisterCharacter(DWORD GaeaId);

public:
	//! 캐릭터 생성
    GLCharAG* CreateChar(
        GLCHARAG_DATA* pCharData,
        DWORD dwClientID,
        int nChannel,
        const SChinaTime& chinaTime,
        EMIP_BONUS_CLASS emIPBonus,
        wb::EM_COUNTRY Country,
		CRYPT_KEY cKey );

    void CraeteCharRegist(GLCharAG* pChar);
    bool CreateCharCheck(GLCHARAG_DATA* pCharData, DWORD ClientSlot);

    //! 캐릭터 생성실패
    GLCharAG* CreateCharFail(GLCharAG* pChar, DWORD dwClientID);
    //! 캐릭터를 제거 합니다
    HRESULT DropOutChar(DWORD GaeaID);
    void SendClubInfo(DWORD ClientSlot, DWORD ClubDbNum);

public:
	BOOL ClearReservedDropOutPC ();
	void ChangeNameMap(GLCharAG* pChar, const char* pszOldName, const char* pszNewName);
	void ChangeNameMap(GLCharAG* pChar, const TCHAR* pszPhoneNumber);

	BOOL ChatMsgProc(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL ChatMsgLinkProc(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    void ChatPartyRecruit(NET_MSG_GENERIC* pMsg);
    void ChatAlliance(NET_MSG_GENERIC* pMsg);
    void ChatGuild(NET_MSG_GENERIC* pMsg);
    void ChatParty(NET_MSG_GENERIC* pMsg);
    void ChatPrivate(NET_MSG_GENERIC* pMsg);
    void ChatGlobal(NET_MSG_GENERIC* pMsg);

    BOOL ChatMsgLoudSpeakerAgt(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);


	void SetEventExp( bool bEvent, float fScale = 0.0f );
	void SetEventItem( bool bEvent, float fScale = 0.0f );
	void SetEventMoney( bool bEvent, float fScale = 0.0f );
	void SetEventEx( bool bEvent, EMGM_EVENT_TYPE type, WORD wValue = 0  );
	void SetEventLimit ( bool bEvent, SEventState sEventState );
	void SetEventGrade( bool bEvent, float fScale = 0.0f );

public:
	BOOL ReserveDropOutPC ( DWORD dwGaeaID );

public:
	void ResetFieldInfo ();

public:
	void ReserveTask( RESERVED_TASK_TYPE eTaskType );
	void ExecuteReservedTask();

public:
	struct GLAgentPacketFunc
	{
		( BOOL ) ( GLAgentServer::*Msgfunc )(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
		GLAgentPacketFunc()
		{
			Msgfunc = NULL;
		}
	};
	GLAgentPacketFunc m_MsgFunc[NET_MSG_GCTRL_MAX];

	void initPacketFunc();

public:
	HRESULT FrameMove(float fTime, float fElapsedTime, const CTime& CurrentTime);

	void	FrameMoveCTF(float fElapsedTime);
	
    void    FrameMoveGuidance(float fElapsedTime);
    void    FrameMoveClubDeathMatch(float fElapsedTime);

	//! Lotto System ---------------------------------------------------------- ;
	void FrameMoveLottoSystem( float fElapsedTime, const CTime& currentTime );

public:

	void CTFQuit(DWORD dwChaNum, WORD wSchool);
	void CTFSyncState(DWORD dwClientID);	
	UINT CTFGetChannel();

	CTime GetCTFReadyTime();

    void AddCTFRewardBuff(SNATIVEID id, WORD wLevel);
    void ClearCTFRewardBuff();
	bool IsRegisterCTFMember(DWORD dwCharDbNum);

	//mjeon.groupchat
	void GroupChatSyncConfig(DWORD dwClientID);

public:
	//! Lotto System ---------------------------------------------------------- ;
	//void LottoSystemSyncState( DWORD dwClientID, DWORD dwChaNum );

	//! Country System -------------------------------------------------------- ;
	void CountrySystemSyncState ( DWORD dwClientID );

	//! Synchronize Use Features ---------------------------------------------- ;
	void SynchronizeUseFeatures ( DWORD dwClientID );

	void SynchronizeUseFeaturesJoinWorldBattle ( DWORD dwClientID );

	//! Matching System ------------------------------------------------------- ;
	void MatchingSystemSyncState ( const DWORD dwClientID, const DWORD dwChaNum );

public:
    //! 필드 서버 변경시 클라이언트 정보 변경 사항 반영
    //! 부활시, gate를 통해 맵 이동시 호출됨
	BOOL ReActionMoveFieldSvr(DWORD dwClientID, DWORD dwGaeaID );

	BOOL GameJoinToFieldSvr(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

public:
	BOOL PostSendSystemMail( 
		DWORD dwRecverDbNum, 
		std::string& strRecverName, 
		SITEMCUSTOM& sItem, 
		LONGLONG llAttachedMoney, 
		UINT iDesign, 
		UINT nSenderSystem, 
		std::string &strSenderName, 
		std::string &strTitle, 
		std::string &strContent, 
		bool bAttachMoneyLimit /*= true*/ );

public:
	BOOL CheckStartMap( DWORD dwGaeaID );
	// 특정 맵으로 이동, 필드 서버 변경 필요여부 반환
	//BOOL MoveToMap( GLCharAG* pChar, SNATIVEID& sMapID, DWORD dwGenGate );

private:
	BOOL MsgReqFieldSvrCharChkFb(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqGenItemAgt(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgServerCloseAgent ( NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgServerCloseField(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MessageInstanceSystemProc ( NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MessageInstanceSystem(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MessageInstanceSystemMsgPack(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MessageInstanceSystemContents(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MessageInstanceSystemContentsMsgPack(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MessageMatchingSystem(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgReqCreateInstantMap(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	//BOOL MsgReqTestCreateInstantMap(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqDeleteInstantMap(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);	
	BOOL MsgReqReEntryRemainTime(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgReqGateOut(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);	
	BOOL MsgReqReBirth(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgReqResurrectMoveMap(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgResurrectMoveMapFB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqReGenGate(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	//BOOL MsgFieldSvrOutFb(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgReCallAg(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgTeleportAg(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgLvUpMustLeaveMap(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL Msg2FriendAG(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	
	BOOL MsgMove2Char(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgMove2CharAG(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    BOOL MsgGetWhisperMsg(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgReqGateOutFB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	
	BOOL MsgReqCreateInstantMapFB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqCreateTutorialMapFB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID); // by luxes.

	BOOL TestCreateInstantMap( float fElpasedTime );

	BOOL MsgReqConfront(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgAnsConfront(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgAnsConfrontFB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgConfrontEnd2Agt(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	//! Lotto System ---------------------------------------------------------- ;
	BOOL LottoSystemMsgProc( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

	BOOL MsgPrisonAg(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

public:
	void MsgClubNew2Agt(
        DWORD ClientSlot,
        DWORD ChaDbNum,
        wb::EM_COUNTRY Country,
        const char* ClubName,
        const std::string& ChaName,
        bool bLobby);
    BOOL MsgClubNewDb2Agt(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

private:	
	BOOL MsgClubDissolution(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubMemberDb2Del(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubCertified(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubIncomeMoney(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	//BOOL MsgClubIncomeMoneyUp(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	//BOOL MsgClubIncomeMoneyFb(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgServerCtrlWeather(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgServerCtrlGenHold(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgServerCtrlTime(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgServerCtrlMonth(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgBlockDetected(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmKickUser(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

public:
	BOOL MsgServerGeneralChat(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

private:
	BOOL MsgConfrontPtyCheckMber(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgConfrontClbCheckMber(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	//mjeon
	//Process Ping messages.
	BOOL ProcessPingMsg(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL ProcessPingTraceMsg( NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID );	

	//
	//mjeon.Post
	//
public:
	BOOL MsgPostNotify(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

private:
	BOOL MsgPostOpenAndTakeFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	//
	//mjeon.Activity
	//
public:
	BOOL MsgActivitySyncFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgActivityPointSyncFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgActivityBadgeSyncFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgActivityCompletedFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgAttendanceSyncFAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgAttendanceResetFAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgAttendanceUpgradeFAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);


	//
	//mjeon.CaptureTheField
	//
public:
	BOOL MsgCTFJoinFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgCTFPrepareAF_FB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);	
	BOOL MsgCTFQuitCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgCTFEndFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgCTFDoneFA_FB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgCTFRankingFAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgCTFMoveStateByGMCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	
	//mjeon.instance
	BOOL MsgMatchRequestCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgMatchCancelCAM(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgMatchCancelCAM_FB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgMatchResultMAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgTransferStartMA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);	

	//BOOL MsgTransferTestCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgTransferChar(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgTransferCharDoneFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);



	BOOL MsgChatInviteCAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgChatInviteCAC_FB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgChatQuitCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgChatKickCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgChatMessageCAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);



	BOOL MsgGetChaExtraInfoFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
		

	//////////////////////////////////////////////////////////////////////////
	BOOL MsgMapSearchName(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgMapSearchNameCF(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgMapSearchNameDetailInfoCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgMapSearchNameDetailInfoFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgMapSearchNamePositionCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgMapSearchNamePositionFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    BOOL MsgAddRebuyItem(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    BOOL MsgCharPosFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	

	//////////////////////////////////////////////////////////////////////////
	BOOL MsgMapCreatePartyInfoCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgMapDeletePartyInfoCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgMapRefreshPartyInfoCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	void MapRefreshPartyInfoHelper(DWORD mapID,DWORD dwClientID, DWORD dwGaeaID);

	bool MapDeletePartyInfoHelper(const std::string& name);
	BOOL MsgMapPartyInfoDetailCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgMapPartyMemberUpdateFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	//////////////////////////////////////////////////////////////////////////
private:
	BOOL MsgGmNonConfrontMode	    (NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmNonConfrontModeViewReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgGmEventItemGen(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmEventItemGenEnd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmEventGrade(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmEventGradeEnd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmEventMoneyGen(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmEventMoneyGenEnd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmEventEx(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmEventExEnd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgGmEventExp(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
public:
    void EventExpSA(NET_MSG_GENERIC* pMsg);
    void EventExpEndSA(NET_MSG_GENERIC* pMsg);

private:
    void EventExp(DWORD ChaDbNum, float Scale);
    void EventExpEnd();

    BOOL MsgGmEventExpEnd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmViewWorkEvent(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgGmEventClassEvent(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgGmLimitEventBegin(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmLimitEventEnd  (NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmLimitEventRestart  (NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgGmLimitEventTimeReq  (NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgCyberCafeClassUpdate(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgGmBigHead(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmBigHand(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgGmChatBlockUA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmChatBlockCN(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmChatBlockCI(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgGmCharInfo4NAME(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmCharInfo4CID(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmCharInfoFldFB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgUserCharInfo4NAME(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgUserCharInfoFldFB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgGmFreePK(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgFreePkGmCommand(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmViewAllPlayer(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID); 

	BOOL MsgGmGenChar(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID); 
	BOOL MsgGmGenMob(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmGenMobEx(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmDelMobEx(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmDelMob(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);	
	BOOL MsgGmMobLevel(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID); 
	BOOL MsgGmMobLevelClear(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID); 
	BOOL MsgGmWhereNPC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID); 
	BOOL MsgGmWhereNPCALL(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID); 
	BOOL MsgGMMove2Gate(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGMMove2Pos(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);	
	BOOL MsgGmWarningMSG(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID); 
	BOOL MsgGmShowMeTheMoney(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID); 
    BOOL MsgGmWhySoSerious(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID); 
	BOOL MsgGmWhySoSeriousFB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgLogToDB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmInstantMapNum(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID); 
	BOOL MsgGmAutoLevel(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmMapLayer(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgGmPrintCrowList(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID); 

	BOOL MsgClub_GuidBattle_RemainTime(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubCDCertifyIngBrd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubCDCertifyBrd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgClubDeathMatchRemainTime(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubDeathMatchResult(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgGmClubDeathMatchStartEndCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgMarketOpenCheck (NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgClubBattleKillUpdate(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubBattleLastKillUpdate(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgReqTaxi(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqTaxiNpcPos(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgNpcCommission(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	//! Character msg -----------------------------------------------------------------
	BOOL MsgActState(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqFriendList(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqFriendAdd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqFriendAddAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqFriendDel(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqFriendBlock(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubMemberReqAgt(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubMemberReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubMemberAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgGMFlyCameraControl(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
    BOOL MsgGmRandomboxNotifyItemReloadReqCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);


public:
    bool MsgClubMemberAddLobby(
        DWORD ClubDbNum,
        DWORD ClientSlot,
        DWORD ChaDbNum,
        const std::string& ChaName,
        EMCHARCLASS ChaClass);

private:
    bool ClubMemberAdd(
        const std::string& FromChaName,
        std::tr1::shared_ptr<GLClubAgent> spClub,
        DWORD ClientSlot,
        const std::string& TargetChaName,
        DWORD TarChaDbNum,
        WORD TarChaLevel,
        bool bLobby,
        const __time64_t& LastLoginTime,
        EMCHARCLASS ChaClass);
    
    BOOL MsgClubMemberDel(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubMemberSecede(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubAuthorityReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);	
	BOOL MsgClubAuthorityReqAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);	
	BOOL MsgClubMarkInfo(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubMarkChange(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubRank2Agt(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubGuidCommission(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubNoticeReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	//BOOL MsgClubSubMaster(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	
    //! 클럽 연합(동맹) 가입 요청
    BOOL MsgClubAllianceReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    //! 클럽 엽합(동맹) 가입 요청 응답
	BOOL MsgClubAllianceReqAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgClubAllianceDelReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubAllianceSecReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubAllianceDisReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubBattleReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubBattleReqAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubBattleReqAnsAlliance(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubBattleArmisticeReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubBattleArmisticeReqAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubBattleSubmissionReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);	
	BOOL MsgAllianceBattleReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgAllianceBattleReqAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgAllianceBattleArmisticeReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgAllianceBattleArmisticeReqAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgAllianceBattleSubmissionReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgRenameBrd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	//BOOL MsgPhoneNumberBrd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgUpdateStartMap(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgSetServerDelayTime(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgSetSkipPacketNum(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgSetServerCrashTime(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgChangeArm(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	//BOOL MsgFriendClubWindowOpen(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqQboxOption(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqAttendList(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgReqAttendance(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgWhisperState(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgBuyRebuyItem(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgBuyRebuyItemFB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgLevelUpFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    //! Lobby 에서 Club 에 가입요청
    void ClubMemberAddLobby(DWORD ClientSlot, DWORD ChaDbNum, const char* szChaName);
    //! 클럽 공지 변경
    bool ClubNoticeChange(DWORD ClubDbNum, DWORD ChaDbNum, DWORD ClientSlot, const std::string& ChaName, const std::string& Notice);
    BOOL MsgCheckRebuySupplies(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgClubId2MasterNameReqCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

public:
	BOOL MsgChaSaveFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgChaSaveEndFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	bool IsChaSaving( DWORD dwUserNum );

	BOOL MsgChaValidataionFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

    //! 클럽 계급 이름 변경
    void ClubGradeNameChange(GLCharAG* pChar, size_t GradeIndex, const std::string& GradeName);

public:
	// 클럽 자동 가입
	bool AutoClubJoin( DWORD ClubDbNum, GLCharAG* pChar, DWORD ClientSlot );
	//BOOL TraceFilter(EMNET_MSG nType);	//mjeon.AF.추적용 필터 (임시)

	// 필리핀 용도 클럽 자동 가입
	void AutoClubJoinForMigration( GLCharAG* pChar, DWORD ClientSlot );

	BOOL MsgProcess(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    const DWORD CreateInstantMap(const SNATIVEID& baseMapID, const DWORD dwFieldChannel, const DWORD dwFieldID);

	// Agent 의 인던정보만을 삭제하는 함수이다;
	// 사용에 주의를 해야한다;
    BOOL DeleteInstantMap ( const DWORD instanceMapID);

private:
	HRESULT InsertMap(GLAGLandManager* pNewLandMan);

public:
	void CheckAgentServer ( DWORD dwAgentNum, A_SERVER_INFO sAgentServerInfo );

public:
	HRESULT Create(
        DWORD dwMaxClient,
        DxMsgServer *pMsgServer, /*DxConsoleMsg *pConsoleMsg,*/ 
        db::IDbManager* pDBMan,
		db::IDbManager* pAdoMan,
        char* szMapList,
        F_SERVER_INFO* pFieldInfo,
        EMSERVICE_PROVIDER ServiceProvider,
        bool bToolMode);
	HRESULT CleanUp();

    void ReloadLogItem();

public:
	//! Lotto System ---------------------------------------------------------- ;
	void InitializeLottoSystem();
	void DestroyLottoSystem();

public:
    //! Point Shop ------------------------------------------------------------
    void PointShopClear(__time64_t UpdateTime);
    void PointShopDataEnd(bool bReload);
    void PointShopDataInsert(const pointshop::SellItem& Item);
    BOOL MsgPointShopUpdateDateCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
	BOOL MsgPointShopCharPointFA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    void SendPointShopInfoToClient(DWORD ClientSlot, bool bAllClient);

public:
	//! ReferChar
	BOOL MsgReferCharRequest( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgReferCharRelay( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

public:
	//! Event;
	BOOL MsgEvent(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

public:
    //! Buff Manager
    buffmanager::GLBuffManagerAG* GetBuffMan() { return m_pBuffMan; }

    void TxtLogToDb();

public:
    //! World battle ----------------------------------------------------------
    BOOL MsgClubCountryCountCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgClubCountryCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    BOOL MsgClubIncomeReNewCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgClubGetStorageCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    void MsgClubStorage(std::tr1::shared_ptr<GLClubAgent> pClub, DWORD ClientSlot);
    BOOL MsgClubStorageGetDb( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgClubStorageGetDbEnd( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
    BOOL MsgClubStorageToInvenCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
    BOOL MsgClubStorageRestoreFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
    BOOL MsgClubStorageExInvenCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
    BOOL MsgClubStorageExClubStorageCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
    BOOL MsgClubStorageToClubStorageCA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
    BOOL MsgClubStorageExHoldFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
    BOOL MsgClubInvenToStorageFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
    //BOOL MsgClubStorageSplit(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgClubStorageDrawMoneyCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgClubStorageDrawMoneyFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
    
    //! 캐릭터머니->클럽창고
    BOOL MsgClubStorageSaveMoneyFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

    BOOL MsgReqDropCharClubNameCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    void SaveClubStorage2DbAll();

    //! Club member grade change
    void ClubMemberGradeChange(GLCharAG* pChar, DWORD MemberDbNum, int GradeIndex);

    //! Club cd certifier change
    void ClubCdCertifyChange(GLCharAG* pChar, DWORD MemberDbNum, bool bCd);

    //! 클럽 권한 변경 Client->Agent
    BOOL MsgClubGradeFlagChangeCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    
    //! 클럽 공개 설정 변경 Client->Agent
    BOOL MsgClubPublicFlagChangeCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgClubAutoKickCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    //! 클럽 신입생 소개글
    BOOL MsgClubNewbieNoticeCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    //! 클럽 가입시간 리셋
    BOOL MsgClubJoinTimeResetCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    //! 클럽 동맹 제한시간 리셋
    BOOL MsgClubAllianceTimeResetCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	//! 클럽 마스터 위임 ON/OFF
	BOOL MsgClubMasterMandate(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgClubStorageUserReset( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgClubStorageMoneyUserReset( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

	BOOL MsgClubNameChangeFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

    //! Log -------------------------------------------------------------------
	// 통계 및 로그 수정
    void LogMoneyExchange(
        gamelog::EMIDTYPE FromType, DWORD FromDbNum, // 출발지
        gamelog::EMIDTYPE ToType, DWORD ToDbNum, // 목적지
        LONGLONG lnPrice, EMITEM_ROUTE Flag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail);

    //! 캐릭터 Action Log
	// 통계 및 로그 수정
    void LogCharAction(
        DWORD dwCI, gamelog::EMACTION emACT, 
        gamelog::EMIDTYPE emTAR, DWORD dwTAR, 
        __int64 nEXP, int nBRIGHT, int nLIFE, int nMONEY, const MapID &sMapID);

    void ConvertWorldToMap(const MapID& mapID, const D3DXVECTOR3& vPos, int& nPosX, int& nPosY);
    void ConvertMapToWorld(const MapID& mapID, int PosX, int PosY, D3DXVECTOR3& vPos);

public:
    //! Product (제조) --------------------------------------------------------
    BOOL MsgProductReqCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgProductLearnBookCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);    
    BOOL MsgProductBookListCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);   
    BOOL MsgProductTypeListCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgProductTypeAddCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgProductTypeDelCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgProductIsGetExpListCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    BOOL MsgProductReqFA(NET_MSG_GENERIC* nmg);
    BOOL MsgProductIsLearnBookFA(NET_MSG_GENERIC* nmg); 
    BOOL MsgProductTypeDelFA(NET_MSG_GENERIC* nmg);

    BOOL MsgProductTypeListDA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgProductBookListDA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgProductIsGetExpListDA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgProductExchangeLogFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

public:
    //! Friend ----------------------------------------------------------------
    BOOL MsgFriendGroupNewCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgFriendGroupDelCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgFriendSetGroupCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgFriendGroupNameChangeCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgFriendGroupReqCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgFriendStateReqCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD dwGaeaID);
    BOOL MsgFriendStateReqDA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD dwGaeaID);
	BOOL Msg2FriendReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

public:
    //! Notify ----------------------------------------------------------------
    BOOL MsgNotifyReqCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

public:
    //! 캐릭터 이름으로 캐릭터 DB 번호를 요청
    BOOL MsgChaDbNumReqCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    BOOL MsgCharDbNumReqDA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);

public:
    //! Guidance
    BOOL MsgGuidanceMapInfoCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    BOOL MsgGmGuidanceInfoCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    BOOL MsgGmGuidanceChangeCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);

	BOOL MsgGmGuidanceBattleStartEndCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);

public:
	BOOL MsgGmNetMsgDelayCA( NET_MSG_GENERIC* nmg, DWORD ClientSlot, DWORD GaeaID );


public:
	//! 개인상점 검색 ----------------------------------------------------------------
	BOOL MsgPrivateMarketItemSearchBasicCA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );
	BOOL MsgPrivateMarketItemNumTotalHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );
	BOOL MsgPrivateMarketItemNumHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );
	BOOL MsgPrivateMarketItemHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );
	BOOL MsgPrivateMarketItemEndHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );
	BOOL MsgPrivateMarketLogCA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );
	BOOL MsgPrivateMarketLogNumHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );
	BOOL MsgPrivateMarketLogHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );
	BOOL MsgPrivateMarketLogEndHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );
	BOOL MsgPrivateMarketStateCA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );
	BOOL MsgPrivateMarketStateHA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );
	BOOL MsgPrivateMarketItemSearchNameCA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );
	BOOL MsgPrivateMarketOptionCA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );

    BOOL MsgPrivateMarketSearchOpenFA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );

    // PVE
    BOOL MsgPVEReqMapTableCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    BOOL MsgPVEReqEntranceCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    BOOL MsgPVEReqEntranceConfirmFA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    BOOL MsgPVEReqClearOfBelongingCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    BOOL MsgPVEAckCreateInstantMapFA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    BOOL MsgPVEAckEntranceInstantMapFA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    BOOL MsgPVEReqMoveFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgPVEReqRetryFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgPVEReqReturnFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgPVEStageResultFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgPVEReqReloadTriggerCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgPVEAckReloadTriggerFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    BOOL ChangeMap(DWORD dwGaeaID, const DWORD dwInstanceMapID, const DWORD dwGateID, const SPVEInDunID indunID);
    BOOL ChangeFieldSvr(const DWORD dwNextFieldSvr, DWORD dwGaeaID, const DWORD dwBaseMapID, const DWORD dwGaeaMapID, const DWORD dwGateID);
    const DWORD CreatePVEInstantMap(SNATIVEID baseMapID, DWORD dwGaeaID, const GLPartyID& PartyID, const DWORD dwFieldChannel, const DWORD dwFieldID);
	BOOL MsgPVEReqEnterNumCharge(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);

    // IP event
    BOOL MsgGmSetIPEvent(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

    BOOL MsgGmAcquireEXP(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
	BOOL MsgMsgTournamentAgent(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);
    BOOL MsgMsgPackTournamentAgent(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	BOOL MsgFieldServerStateFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

	// User System Info
	BOOL MsgUserSystemInfoCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID);

public:
	SWorkingEvent *GetWorkingEvent() { return &m_WorkEvent; }
	void GetWorkingEventInfo(G_AUTH_EVENT_INFO &eventInfo) { m_WorkEvent = eventInfo; }
	SEventState *GetEventState() { return &m_sEventState; }
	void GetEventState(G_AUTH_EVENT_INFO &eventInfo) { m_sEventState = eventInfo; }
	// TexasHoldem
	BOOL MsgTexasHolem( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD dwGaeaID );

public:
	BOOL MsgPostGetDataChangeDB( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgPostGetStateChangeDB( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgPostGetSendStateChangeDB( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgPostGetRecvStateChangeDB( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

public:
	BOOL MsgCharMoneyNotifyFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgCountRecorderInfoReq( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgCountRecorderInfoFB_FA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

public:
	BOOL MsgVCountryAddFAC( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgVCountryAddDA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgVCountryAddAA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgVCountryResetFAC( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgVCountryResetDA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgVCountryResetAA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

public:
	BOOL MsgGmSetNotifyMoneyCA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );

	BOOL MsgGmNetworkProfileReq( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );	
	BOOL MsgGmChangeJoinWorldBattle( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );	
	// WB 서버 접속 가능여부 확인 및 접속
	BOOL MsgTransDB2WorldBattle( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );	

	// 란모바일
	BOOL MsgGmRanMobileCommandRequest( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );	
	BOOL MsgGmRanMobileCommandResponse( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID );	
	
/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.14
 * modified : 
 */
public:
    // 등록 요청
    BOOL MsgGmSetRandomboxChanceReqCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    BOOL MsgGmSetRandomboxChanceReqHA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);

    // 조회 요청
    BOOL MsgGmGetRandomboxChanceListCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    BOOL MsgGmGetRandomboxChanceListHA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);

    // 삭제 요청
    BOOL MsgGmDelRandomboxChanceCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);
    BOOL MsgGmDelRandomboxChanceHA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID);

    // Randombox Open시 당첨확률이 5%미만의 경우 공지 메시지를 전달
    BOOL MsgRandomboxWinnerNotifyFA(NET_MSG_GENERIC* nmg, DWORD ClientSlot, DWORD GaeaID);

public:
	// 아이템 관련 처리 도중 에러가 발생해서 롤백까지 했으나 제대로 되지 않을 경우
	// 아이템 guid 와 원인을 parameter 로 넘겨준다.
	// 내부 동작은 디비에 특정 테이블에 아이템 guid, 원인, 발생 시간 이렇게 저장이 된다.
	// 예외가 발생한 아이템은 결국 디비에 있기 때문에 로그 확인 및 아이템 테이블과
	// 가칭 에러 아이템 테이블에서 guid 로 아이템을 찾을 수 있고 후 처리가 가능할 것 이다.
	// 가칭 에러 아이템 테이블에 guid 는 중복해서 들어갈 수 있도록 테이블 설계
	void InsertErrorItem( sc::MGUID& guid, int CallType );

	// db action 제작 필요 제작했다. 디비 함수 제작 및 프로시저 남았음
	// guid, 변경될 인벤타입, 좌표 필요하다.
	// sItemCustom 의 인벤타입enum INVEN_TYPE 참조
	// 아이템 세부 정보 변경없이, 단지 위치만 변경되는 경우에 사용된다. 예를 들어 창고에 있던 아이템이 인벤토리로 이동한 경우
	// 장착창에서 인벤토리로 이동한 경우, 인벤토리에서 인벤토리로 이동한 경우등
	// 디비에서 저장 위치와, 좌표값만 변경한다. 아이템 세부항목 전체 업데이트 비용을 줄이기 위한 용도이다.
	void UpdateItemInvenType( DWORD Owner, const SITEMCUSTOM& sItemCustom, INVEN_TYPE InvenType, WORD PosX, WORD PosY, DWORD Channel = 0 );

public:
	BOOL MsgJackpotNotify( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	BOOL MsgJackpotOption( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

};

#endif // _GL_AGENT_SERVER_H_
