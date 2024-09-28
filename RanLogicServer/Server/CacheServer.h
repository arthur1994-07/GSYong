#ifndef _LS_CACHE_SERVER_H_
#define _LS_CACHE_SERVER_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include <boost/tr1/unordered_map.hpp>
#include "../../SigmaCore/Net/NetDefine.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "../../SigmaCore/Json/json_spirit_value.h"
#include "./NetServer.h"

// 개인상점 검색시스템 테스트
#include "../../RanLogic/Msg/GLContrlPrivateMarket.h"

#include "../Database/DBAction/DbAction.h"

struct PRIVAIETEMDATA
{
	DWORD dwCharDbNum;
	char Itemname[MAP_NAME_LENGTH+1];
	SSALEITEM sItemdata;

	PRIVAIETEMDATA() 
		: dwCharDbNum(0)
	{
		memset(Itemname, 0, sizeof(char) * (MAP_NAME_LENGTH+1));
	}

	void SetName(const char* Name)
	{
		if (Name)
			StringCchCopy(Itemname, MAP_NAME_LENGTH+1, Name);
	}
};

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.14, 신규등록
 * modified : 
 */
class CacheRandomboxChance;

class CacheUserLobbyMan;
class CacheServerConfigXml;
class NetCacheClientMan;
class PostRenewCache;
struct NET_MSG_GENERIC;
namespace db {
    class IDbManager;
    enum EM_BILLING_SELL_TYPE;
}

namespace pointshop {
    class PointShopCache;
}

namespace club {
    class LogManServer;
}

namespace private_market
{
	class GLPrivateMarketSearchMan;
    class GLConsignmentSaleMan;
}

namespace sc {
	class CGlobalAuthClientLogic;
}

namespace Money
{
	class CMoneyManagerCache;
}

class GLCharCache;
#ifndef spCharCache
typedef std::tr1::shared_ptr<GLCharCache> spCharCache;
#endif

typedef std::tr1::shared_ptr<PostRenewCache> spPostRenewCache;

class CacheServer : public NetServer
{
public:
    CacheServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider);
    virtual ~CacheServer();

private:
    CacheServerConfigXml* m_pConfig;
    NetCacheClientMan* m_pClientManager;
    db::IDbManager* m_pDbManager; //! mjeon.dbman
    DWORD m_HeartbeatTimer;
    DWORD m_SessionCheckTimer;
    bool m_bHeartBeat;
    DWORD m_SessionSlot;

	sc::CGlobalAuthClientLogic* m_pAuthManager; // for global auth
    //EMSERVICE_PROVIDER m_ServiceProvider;

    //! 캐릭터 DB 번호, 캐릭터 Pointer
    typedef std::tr1::unordered_map<DWORD, spCharCache> CACHE_CHAR;
    typedef CACHE_CHAR::iterator                        CACHE_CHAR_ITER;
    typedef CACHE_CHAR::const_iterator                  CACHE_CHAR_CITER;
    typedef CACHE_CHAR::value_type                      CACHE_CAHR_VALUE;

    CACHE_CHAR m_CacheChar;
    pointshop::PointShopCache* m_pPointShop;

    CacheUserLobbyMan* m_pUserLobbyMan;

    club::LogManServer* m_pClubLog;

	spPostRenewCache m_spPostCache;

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.14, 신규등록
     * modified : 
     */
    CacheRandomboxChance* m_pRandomboxMan;

public:
    private_market::GLPrivateMarketSearchMan* GetPrivateMarketSearch() { return m_pPrivateMarketSearch; };
    
private:    
    // 개인상점 검색
	private_market::GLPrivateMarketSearchMan* m_pPrivateMarketSearch;
    private_market::GLConsignmentSaleMan* m_pConsignmentSale;     // 위탁 판매 ( 란포인트 거래 )

	Money::CMoneyManagerCache* m_pMoneyMan;

public:

    //! -----------------------------------------------------------------------
    //! NetServer override functions 
    //! -----------------------------------------------------------------------
    virtual int	Start() override;
    virtual int	Stop() override;	
	virtual void ForceStop( DWORD Reason ) override;
    //virtual	int	Pause() override;
    //virtual	int Resume() override;
    //virtual	int	ReStart() override;

    virtual int StartClientManager() override;
    virtual int StartWorkThread(ServerConfigXml* pConfig) override;
    virtual int StartUpdateThread() override;

    virtual int WorkProc() override;
    virtual int UpdateProc() override;
    virtual int ListenProc() override;
	virtual int	RegularScheduleProc() override;
            
    //! Local Message process function (직접 내부 메시지 전송)
    virtual int InsertMsg(int nClient, void* pMsg) override;
	virtual int	InsertMsg(int nClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);

    // I/O operation memory
    virtual PER_IO_OPERATION_DATA* GetFreeOverIO(EMMSG_IO_TYPE nType=NET_SEND_POSTED) override;
    virtual void ReleaseOperationData(PER_IO_OPERATION_DATA* pData) override;
    
    //! -----------------------------------------------------------------------

    int SendClient(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);
    int SendClient(DWORD ClientSlot, EMNET_MSG Type, const msgpack::sbuffer& SendBuffer);

    void SendAllClient(NET_MSG_GENERIC* pMsg);
    void SendAllClient(EMNET_MSG Type, const msgpack::sbuffer& SendBuffer);
	void SendAllClientWithoutSession(NET_MSG_GENERIC* pMsg);
	void SendToSession(NET_MSG_GENERIC* pMsg);

    int SessionConnect();
    void SessionSndSvrInfo();
	int SessionCloseConnect();
    void DatabaseCheck();
    void ClubLogCheck();

    club::LogManServer* GetClibLogMan() { return m_pClubLog; }

private:
    //! -----------------------------------------------------------------------
    //! 설정파일을 읽는다.
    bool ReadConfig();

    //! Database Manager
    bool StartDbManager();

    //! Close Client Connection
    //! bForce:강제종료
    void CloseClient( DWORD dwClient );

    void RecvMessageProcess();
    void MessageProcess(MSG_LIST* pMsg);
    void HeartbeatCheck();
    void FrameCheck();
    void ConcurrentCheck();
    void HeartbeatSend();
    void HeartbeatRecvCheck();	
    void SessionServerConnectionCheck();

    void FrameMove( float fElapsTime );

	// 서버 시작할 때 디비액션 거치지 않고 바로 포인트샵 정보 읽어오는 용도
	void PointShopDirectLoad();

	// 서버 시작할 때 디비액션 거치지 않고 바로 개인상점(위탁판매) 정보 읽어오는 용도
	void PrivateMarketDirectLoad();

public:
    //cwback
    void AddGameAdoJob(db::DbActionPtr spJob);
    void AddLogAdoJob(db::DbActionPtr spJob);
    void AddUserAdoJob(db::DbActionPtr spJob);
    void AddShopAdoJob(db::DbActionPtr spJob);

private:
    spCharCache GetChar(DWORD ChaDbNum);

public:
	bool IsOnline( DWORD dwChaDbNum );

	bool IsIntegrationServer();

private:
    struct CachePacketFunc
    {
        (void) (CacheServer::*Msgfunc) (DWORD dwClient, NET_MSG_GENERIC* pMsg);
        CachePacketFunc()
        {
            Msgfunc = NULL;
        }
    };

    CachePacketFunc m_MsgFunc[NET_MSG_GCTRL_MAX];
    void InitPacketFunc();

    void MsgJson(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    bool MsgJsonParser(DWORD ClientSlot, const std::string& JsonStr);
    void MsgHeartbeatReq(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgHeartbeatAns(DWORD dwClient, NET_MSG_GENERIC* pMsg);

    //! -----------------------------------------------------------------------
    //! Game
    //! 캐릭터 게임 조인
    void MsgGameJoinAH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgGameCloseAH(DWORD dwClient, NET_MSG_GENERIC* pMsg);

    //! Agent->Cache:캐릭터 리스트 요청
    void MsgChaListAH(DWORD dwClient, NET_MSG_GENERIC* pMsg);

    //! DB->Cache Server:Character Point 정보
    void MsgCharPointDH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    //! Field->Cache:Character Point 정보 요청
    void MsgCharReqPointFH(DWORD dwClient, NET_MSG_GENERIC* pMsg);

    //! Database->Cache:User Character List
    void MsgChaBaInfoDH(DWORD dwClient, NET_MSG_GENERIC* pMsg);

    //! Agent->Cache:Character Lobby display info request
    void MsgChaLobbyInfoAH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
	void MsgChaLobbyInfoAHEnd(DWORD dwClient, NET_MSG_GENERIC* pMsg);

    //! DB->Cache:Character Lobby display info request
    void MsgChaLobbyInfoDH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    //! DB->Cache:신규 캐릭터 Character Lobby display info request
    void MsgChaLobbyNewInfoDH(DWORD dwClient, NET_MSG_GENERIC* pMsg);

    void MsgLobbyUserInfoDH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgLobbyCharInfoDH2(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgLobbyUserInfoEndDH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    
    //! -----------------------------------------------------------------------
    //! Log
    void MsgLogItemExchange(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgLogMoneyExchange(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgLogItemConversion(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgLogAction(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgLogItemRandomOption(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgLogPetAction(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgLogVehicleAction(DWORD dwClient, NET_MSG_GENERIC* pMsg);
	void MsgCTFStatLogAH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
	void MsgCTFResultLogAH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
	void MsgLogCostumeStat(DWORD dwClient, NET_MSG_GENERIC* pMsg);

	void MsgLogItemAddonOption( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgLogItemLinkSkillOption( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgLogItemBasicStatOption( DWORD dwClient, NET_MSG_GENERIC* pMsg );

    //! Server->Cache Club log
    void MsgClubLog(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);
    //! Agent->Cache Club log request
    void MsgClubLogAH(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);

    void MsgClubLogDh(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);
    void MsgClubLogDhEnd(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);

    void MsgLogAttendanceSH(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);
    void MsgLogAttendanceTaskSH(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);

	//
	// Game
	//
	void MsgGetChaExtraInfoFH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
	void MsgGetChaExtraInfoDH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
	void MsgSetChaExtraInfoFH(DWORD dwClient, NET_MSG_GENERIC* pMsg);	
	

    //! -----------------------------------------------------------------------
    //! Point Shop
    void MsgPsDataStartDS(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgPsDataEndDS(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgPsDataDS(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgPsReqDataSH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void PointShopDataSend(DWORD dwClient, bool bReload);
    //! Session->Cache Server:Point Shop reload
    void MsgPsReloadSS(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    //! Field->Cache:Character Point 변경됨
    void MsgCharPsPointChangeFH(DWORD dwClient, NET_MSG_GENERIC* pMsg);

	// Cache의 Point를 갱신한다;
	// 위의 함수는 아이템과 종속되어 있기 때문에 사용하지 않는다;
	void MsgCharChangePointFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );

    //! -----------------------------------------------------------------------
	//! 개인상점검색 시스템
	void FrameMovePrivateMarket();
	void MsgPrivateMarketSellerRegisterFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketItemRegisterFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );    
	void MsgPrivateMarketItemSearchBasicFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketItemBuyFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketItemPayMoneyFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketItemBuyRollbackFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketItemBuyResultFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketItemIsHoldFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketLogFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketStateFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketItemSearchNameAH( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketOptionFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPrivateMarketCloseFH( DWORD dwClient, NET_MSG_GENERIC* pMsg );


	//! -----------------------------------------------------------------------
	//! 기간제 아이템 시간 연장
	void MsgInspectionRewardSet( DWORD dwClient, NET_MSG_GENERIC* pMsg );

	//! -----------------------------------------------------------------------
	//! 우편 코드 개선
	void MsgPostRenewListReq( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPostRenewListDBAck( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPostRenewSendReq( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPostRenewSendDBAck( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPostRenewDelReq( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPostRenewAttachReq( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	//void MsgPostRenewAttachDBAck( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPostRenewReadReq( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPostRenewReturnReq( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPostRenewReturnDBAck( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgPostRenewAttachRollbackReq( DWORD dwClient, NET_MSG_GENERIC* pMsg );


	void MsgPmarketRegitemFH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
	void MsgPmarketSearchReqAH(DWORD dwClient, NET_MSG_GENERIC* pMsg);

    void MsgCharWebShopLogFH(DWORD dwClient, NET_MSG_GENERIC* pMsg);

    void SaleLog(
        const SNATIVEID& ItemId,
        __int64 Price,
        db::EM_BILLING_SELL_TYPE BillingType,
        DWORD UserDbNum,
        int ServerNum,
        DWORD ChaDbNum,
        int ChaLevel);

    void SessionMsgVersionSS(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void DoNotiong(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);

    void MsgOddEvenLogFH(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);

    //! 위탁판매 ConsignmentSale
    //void MsgConsignmentSaleGetListAck( DWORD dwClient, NET_MSG_GENERIC* pMsg );
    void MsgConsignmentSaleRegisterReq( DWORD dwClient, NET_MSG_GENERIC* pMsg );
    void MsgConsignmentSaleSendbackReq( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgConsignmentSalePurchaseFail( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgConsignmentSalePayMoneyAck( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgConsignmentSaleUpdateItemAck( DWORD dwClient, NET_MSG_GENERIC* pMsg );

public:
	//! Test Code
	void AdoTest1();
    void AdoTest2();
    //! 개인상점 테스트

	std::vector<PRIVAIETEMDATA> m_vecPrivateItemData;

    DWORD m_dwFrameMoveTimer;

public:
	int SendAuthInfoToSessionServer(); // 세션서버로 인정 정보 전송. 세션에서 인증서버로 릴레이
	void MsgAuthCertificationResponse(DWORD dwClient, NET_MSG_GENERIC* pMsg);
	
/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.15
 * modified : 
 */
public:
    void MsgGmSetRandomboxChanceReqAH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgGmGetRandomboxChanceListAH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgGmDelRandomboxChanceAH(DWORD dwClient, NET_MSG_GENERIC* pMsg);

    void MsgRandomboxOpenChanceReqFH(DWORD dwClient, NET_MSG_GENERIC* pMsg);
    void MsgRandomboxOpenChanceEndFH(DWORD dwClient, NET_MSG_GENERIC* pMsg);



public:
	void MsgLogMoneyFH ( DWORD _dwClient, NET_MSG_GENERIC* _pMsg );
};

#endif // _LS_CACHE_SERVER_H_
