#ifndef _LS_DB_MANAGER_H_
#define _LS_DB_MANAGER_H_

#pragma once

#include "../../SigmaCore/Database/DbDefine.h"
#include "../../SigmaCore/String/MinGuid.h"
#include "../../RanLogic/Guidance/GLGuidanceDefine.h"
#include "../../RanLogic/Friend/FriendDefine.h"
#include "../../RanLogic/Activity/ActivityBase.h"
#include "../../RanLogic/Notify/NotifyDefine.h"
#include "../../RanLogic/Market/GLConsignmentSaleDefine.h"
#include "../../RanLogic/Post/PostDefine.h"

#include "./DBAction/DbAction.h"
#include "./DbDefine.h"

// Lotto System
#include "../LottoSystem/GLLottoDataTypeServer.h"

// Product
#include "../Product/GLProductDataTypeServer.h"

// Country System
#include "../Country/GLCountryManServer.h"

// Provide Reward System
#include "../ProvideRewardSystem/GLProvideRewardManager.h"

// Matching System
#include "../TournamentManager/MatchingJoinManager.h"

class NetServer;
class GLVEHICLE;
struct SCHARDATA2;
struct SPOSTINFO;
struct SITEMCUSTOM;
struct SHOPPURCHASE;
struct SINVENITEM;
struct SAPOST;
struct GLPET;
//struct GLCLUBINFO;
struct GLCLUBMEMBER;
struct GLCLUBBATTLE;
struct PET_RELIST;
struct SCHARINFO_LOBBY;
class GLCHARAG_DATA;
struct SAPOST;
struct SAPOSTSTATE;

struct LOG_ITEM_EXCHANGE;
struct LOG_MONEY_EXCHANGE;
struct LOG_ITEM_CONVERSION;
struct LOG_ACTION;
struct LOG_GUIDANCE;
struct LOG_CDM;
struct LOG_CDM_RESULT;
struct LOG_RANDOM_OPTION;
struct LOG_PET_ACTION;
struct LOG_VEHICLE_ACTION;
struct LOG_COSTUME_STATS;

struct LOG_ADDON_OPTION;
struct LOG_LINKSKILL_OPTION;
struct LOG_BASICSTAT_OPTION;

struct NET_LOGIN_FEEDBACK_DATA2;
struct DAUM_NET_LOGIN_FEEDBACK_DATA2;
struct GSP_NET_LOGIN_FEEDBACK_DATA2;
struct TERRA_NET_LOGIN_FEEDBACK_DATA2;
struct GS_NET_LOGIN_FEEDBACK_DATA2;
struct EXCITE_NET_LOGIN_FEEDBACK_DATA2;
struct JAPAN_NET_LOGIN_FEEDBACK_DATA2;
struct DAUM_NET_LOGIN_FEEDBACK_DATA2;
struct GM_NET_LOGIN_FEEDBACK_DATA2;
struct US_NET_LOGIN_FEEDBACK_DATA2;

struct SFACEBOOK;
struct STWITTER;

class Activity;
struct ACTIVITY_STATUS;
class TitleManager;

class AttendanceManager;

struct CTF_REWARD_BUFF;

class GLPetField;
class GLClub;

struct SINVENITEM_SAVE;

namespace se {
    class ByteStream;
}

namespace pointshop { 
    struct SellItem;
}

namespace wb {
    enum EM_COUNTRY;
}

namespace club {
    struct AUTH_GRADE_DB;
}

namespace gamelogic {
    class NotifyBase;
}

namespace sc {
    struct MGUID;
}

namespace TexasHoldem 
{ 
	struct sChipPrice;
}

namespace db
{

class DbExecuterMan;

enum EM_INSTANT_LOG_TYPE;

enum EM_DB_CONNECTION_TYPE
{
    ODBC,
    ADO,        
    EMULATOR,
};

//
//mjeon.dbman
//
class IDbManager
{
public:
    IDbManager(NetServer* pServer, db::SERVER_TYPE ServerType, EMSERVICE_PROVIDER ServiceProvider, EM_DB_CONNECTION_TYPE ConnectionType);
    virtual ~IDbManager();

protected:
    DbExecuterMan* m_pExecuter;
    NetServer* m_pServer;
    EMSERVICE_PROVIDER m_ServiceProvider; //! 서비스 제공업자
    EM_DB_CONNECTION_TYPE m_ConnectionType; //! 연결 방식

public:
    EM_DB_CONNECTION_TYPE GetConnectionType() const { return m_ConnectionType; }

	//
	//mjeon.ado
	//pSubDbMan: 좀 억지스럽긴 하나, 기존 ODBC쪽 처리부에 영향을 주지 않으면서 ADO 코드를 겸할 수 있도록 하기 위함이다.
	// 즉, 하나의 DbAction으로 기존의 ODBC처리와 더불어 추가적으로 구현하는 내용은 ADO를 사용하고 싶을 경우에 이를 사용하면 된다.
	//
    virtual void AddJob(db::DbActionPtr spJob, IDbManager *pSubDbMan = NULL);
    virtual void AddGameJob(db::DbActionPtr spJob, IDbManager *pSubDbMan = NULL);
    virtual void AddLogJob(db::DbActionPtr spJob, IDbManager *pSubDbMan = NULL);
    virtual void AddUserJob(db::DbActionPtr spJob, IDbManager *pSubDbMan = NULL);
    virtual void AddWebJob(db::DbActionPtr pJob, IDbManager *pSubDbMan = NULL);
    virtual void AddShopJob(db::DbActionPtr pJob, IDbManager *pSubDbMan = NULL);
    virtual void AddClubLogDbJob(DbActionPtr spJob, IDbManager *pSubDbMan = NULL);
	virtual void AddAuthJob(DbActionPtr spJob, IDbManager *pSubDbMan = NULL);
    virtual void ExecuteLastJob();
    virtual bool IsAlive() = 0; //! 접속이 제대로 되는지 체크
    virtual bool Reconnect() = 0; //! 재접속 체크
    virtual bool CheckHdd() = 0; //! Hard disk 용량 체크
    virtual bool IsAllDbJobCompleted() = 0; //! 모든 DB 작업이 끝났는가?
    //! 모든 DB 작업이 끝나기 전까지 대기한다.
    //! WaitLimitTime : Milliseconds
    virtual void WaitAllDbJobCompleted(DWORD WaitLimitTime);

public:
    //! -----------------------------------------------------------------------
    //! 호환성 유지를 위해서 임시로...
    virtual int	OdbcOpenGameDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenUserDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenBoardDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenLogDB  (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int OdbcOpenShopDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenTerraDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenGspDB  (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenThaiDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenKorDB  (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    //! 말레이시아 PC방 이벤트
    virtual int	OdbcOpenMyDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;

    //! Ado
    virtual bool SetConnectionString(DB_TYPE Type, const std::string& Ip, const std::string& DbName, const std::string& Id, const std::string& Password, int AdoMaxPoolSize, int DatabasePacketSize) = 0;

    //! -----------------------------------------------------------------------

	virtual int UpdateUserLockerOption( DWORD UserNum, int Option ) = 0;
	virtual int SaveCharacter( LPVOID _pbuffer, bool bAutoSave = false ) = 0;
    virtual int SaveCharacterBase(SCHARDATA2* pCharData2, DWORD ChaDbNum) = 0;
	virtual int SaveCharacterExtraWithAdo(LPVOID _pbuffer) = 0;	//mjeon.ado

	virtual int SaveInventoryAndGameMoney( LPVOID _pbuffer ) = 0;

	virtual int	ReadUserInven(SCHARDATA2* pChaData2) = 0;
	virtual int	ReadUserInvenNew( SCHARDATA2* pChaData2, int FirstLoad = 0 ) = 0;

    // 제조
    virtual int AddProductType(const DWORD dwChaNum, const WORD nProductType) = 0;
    virtual int GetProductType(const DWORD dwChaNum, std::vector<WORD>& vecProductType OUT) = 0;
    virtual int DelProductType(const DWORD dwChaNum, const WORD nProductType) = 0;

    virtual int AddProductIsGetExp(const DWORD dwChaNum, const DWORD dwProductID) = 0;
    virtual int GetProductIsGetExp(const DWORD dwChaNum, std::vector<WORD>& vecProductID OUT) = 0;
    virtual int DelProductIsGetExp(const DWORD dwChaNum, const PRODUCT_ITEM& productItem ) = 0;

    virtual int SaveProductBook(DWORD nChaNum, const std::vector<DWORD>& setLearnProductBook) = 0;
    virtual int LoadProductBook(DWORD nChaNum, std::vector<DWORD>& setLearnProductBook OUT) = 0;
    virtual int DeleteProductBook(DWORD nChaNum, const std::vector<PRODUCT_ITEM>& vecBook) = 0;

	virtual int CharProductLog( const CHAR_PRODUCT_LOG& productLog ) = 0;
	virtual int ProductExchangeLog( const PRODUCT_EXCHANGE_LOG& productLog ) = 0;

	virtual int DelChaFriend(int nChaP, int nChaS) = 0;
	virtual int AddChaFriend(int nChaP, int nChaS) = 0;
	virtual int SetChaFriend(int nChaP, int nChaS, int nFlag) = 0;
    virtual int ChaFriendFlagSelect(int ChaP, int ChaS, int& Flag) = 0;

	//mjeon.Ping
	virtual int AddLogPing(int nUserNum, int nRTT, const char* szIP, __time64_t tTime) = 0;
	
	//mjeon.Post
	virtual int PostSend(SPOSTINFO &PostInfo, SITEMCUSTOM &itemcustom) = 0;
	virtual int PostSendRollBack(LONGLONG &llPostID) = 0;
	virtual int PostOpenAndTake(LONGLONG llPostID, BOOL bMoneyAccept, BOOL bAttach01Accept, BYTE bChargePayment, BYTE bReturnedPost) = 0;
	virtual int PostSendCharge(SAPOST &aPOST, DWORD dwOpenAndTakeUserNum) = 0;

	//virtual int PostCheckAttachMoney(DWORD dwClientID, SPOSTINFO &PostInfo, SINVENITEM *pItem) = 0;

	virtual int LogPartyMatch(int nSGNum, int nSvrNum, WORD wWin, WORD wLost) = 0;

    virtual LONGLONG GetItemMaxNum(int nSGNum, int nSvrNum,	int nFldNum, std::vector<VIEWLOGITEMEXCHANGEMAX> &v) = 0;
    virtual int GetLogItem(std::vector<DWORD>& LogItemData) = 0;
	virtual int LogItemExchange(const LOG_ITEM_EXCHANGE &sEXCHANGE) = 0;
	virtual int LogMoneyExchange(const LOG_MONEY_EXCHANGE &_sLOG )	 = 0;
	virtual int LogPointShopExchange(int nFromType, DWORD ChaDbNum, int nExchangeFlag, WORD ItemMid, WORD ItemSid, int nPoint, int nExchangeFlagDetail ) = 0;
    virtual int LogItemConversion(const LOG_ITEM_CONVERSION &_sLOG) = 0;
    virtual int LogAction(const LOG_ACTION& LogData) = 0;


	//! 선도전 로그
	virtual int LogGuidance(const LOG_GUIDANCE &sLog) = 0;

	//! CDM 로그
	virtual int LogCDM(const LOG_CDM &sLog) = 0;

	//! CDM 로그(최종) 
	virtual int LogCDMReuslt(const LOG_CDM_RESULT &sLog) = 0;
	
	//! Shop 에서 구입한 아이템을 가져온다.
	//! \param strUID 유저ID
	//! \param &v 상품을 담을 벡터
	//! \return 
    virtual int GetPurchaseItem(const CString& strUID, std::vector<SHOPPURCHASE> &v) = 0;
	
	//! 실제로 가져갈 수 있는 상품인지 확인한다.
	//! \param strPurKey 구입코드
	//! \param nFlag(0 : 구매, 1 : 구매완료, 2 : 구매취소신청, 3 : 구매취소처리)
	//! \return 1:가져갈수 있음 이외의값:가져갈수 없음
    virtual int SetPurchaseItem(const CString& strPurKey, int nFlag=1) = 0;
	
	//! 캐릭터 삭제, 캐릭터생성 갯수 증가
	virtual int UserChaNumIncrease(int nUserNum) = 0;

	//! 해당 락커 만료시간을 정한다.
	//! \n 1 번 5 번 락커는 정할 수 없음.
	//! \n 2, 3, 4 번 락커만 만료시간을 정할 수 있다.
	//! \param nUserNum 사용자번호
	//! \param nStorageNum 스토리지번호
	//! \param tTime 만료시간
	//! \return -1:sc::db::DB_ERROR  0:sc::db::DB_OK
    virtual int SetChaStorageDate(int nUserNum, int nStorageNum, __time64_t tTime) = 0;
    	
	//! 해당 캐릭터의 인벤토리 추가 줄수를 세팅한다.
	//! \n 최초 0 에서 한줄 추가시 1 을 입력
    //! \n 현재 한줄 상태에서 또 한줄 추가시 2 를 입력(최종 줄수를 입력한다)
    //! \n 현재 두줄 상태에서 한줄 추가시 3 을 입력
    //! \n 최대 3까지만 입력가능.
	//! \param nChaNum 캐릭터번호
	//! \param wInvenLine 추가줄수
    virtual int SetChaInvenNum(int nChaNum, WORD wInvenLine) = 0;
    	
	//! 해당 사용자의 프리미엄 기간을 세팅한다.
    virtual int SetPremiumTime(int nUserNum, __time64_t tPremiumTime) = 0;
    	    
	//! 직전귀환 카드 사용을 위한 포지션을 저장한다.
    virtual int SetLastCallPos(int nChaNum, DWORD dwMapID, D3DXVECTOR3 vPos) = 0;    
	
	//! 새로운 캐릭터를 생성한다.
    virtual int	CreateNewCharacter(SCHARDATA2* pCharData2) = 0;
	virtual int	CreateNewCharacterExtraWithAdo(SCHARDATA2* pCharData2) = 0;	//mjeon.ado

    //! ChaInfo Backup.
    virtual int CharacterInfoBackup(SCHARDATA2* pCharData2) = 0;
	
    
    //! 새로운 클럽을 생성한다.
    //! 입력
    //!     strClubName : 클럽이름
    //!     dwChaNum : 클럽을 생성하려는 캐릭터 번호(이 캐릭터가 클럽의 마스터가 된다)
    //! 출력
    //!     -1 : 이미 길드마스터로 등록되어 있음 생성불가.
    //!     -2 : 길드생성중 에러발생(중복된길드이름)
    //!      0 보다 클때 : 생성된 길드번호임
    virtual int CreateClub(const std::string& ClubName, DWORD ChaDbNum, wb::EM_COUNTRY Country) = 0;    
    
    //! 클럽을 삭제한다.
    //! dwClub : 클럽번호
    //! dwChaNum : 클럽을 삭제하려는 캐릭터번호(캐릭터 번호가 클럽 마스터가 아니면 삭제가 되지 않는다)
    virtual int DeleteClub(DWORD dwClub, DWORD dwChaNum) = 0;    
    
    //! 클럽랭크를 세팅한다.
    //! dwClub : 클럽번호
    //! dwRank : 랭킹
    virtual int SetClubRank(DWORD dwClub, DWORD dwRank) = 0;

    virtual int ReadClubMarkImage(DWORD dwClub, se::ByteStream &ByteStream) = 0;

    virtual int WriteClubMarkImage(DWORD dwClub, DWORD dwMarkVer, BYTE* pData, int nSize) = 0;

	//! 클럽 해체시간을 설정한다.
	//! \param dwClub 클럽번호
	//! \param tDiss 해체시간
    virtual int SetClubDissolutionTime(DWORD dwClub, __time64_t tDiss) = 0;
	
	//! 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
	//! \param dwClub 클럽번호
	//! \param tSec 시간
	virtual int SetClubAllianceSec(DWORD dwClub, __time64_t tSec) = 0;
	
	//! 클럽의 동맹해산 시간을 정한다.
	//! \param dwClub 클럽번호
	//! \param tDis 해산시간
	virtual int SetClubAllianceDis(DWORD dwClub, __time64_t tDis) = 0;
	
	//! 클럽의 마스터 위임 시간을 정한다.
	//! \param dwClub 클럽번호
	//! \param tDis 마스터 위임시간
	virtual int SetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority) = 0;
	
    //! 서버의 모든 클럽정보를 가져온다
    //! \param vClub : 클럽정보
    virtual int GetClubInfo(std::vector<std::tr1::shared_ptr<GLClub> > &vClub) = 0;
    
    //! 해당 클럽의 멤버를 가져온다
    //! \param dwGuild : 클럽번호
    //! \param vMember : 클럽멤버
    virtual int GetClubMember(DWORD dwClub, std::vector<GLCLUBMEMBER>& vMember) = 0;

    //! 해당 클럽의 클럽 지위 정보를 가져온다.
    virtual int GetClubAuth(DWORD ClubDbNum, std::vector<club::AUTH_GRADE_DB>& vAuth) = 0;
    //! 해당 클럽의 모든 캐릭터의 클럽 지위를 0 으로 만든다.
    virtual int ResetClubPosition(DWORD ClubDbNum) = 0;
        
    //! 새로운 캐릭터를 클럽에 가입시킨다
    //! \param dwClub : 클럽번호
    //! \param dwChaNum : 가입시키려는 캐릭터번호
    virtual int AddClubMember(DWORD dwClub, DWORD dwChaNum) = 0;
        
    //! 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다
    //! \param dwChaNum : 캐릭터번호
    virtual int DeleteClubMember(DWORD dwChaNum) = 0;

	virtual int DeleteClubMemberWithNoPenalty( DWORD dwChaNum ) = 0;
	
	//! 클럽 배틀을 신청한다.
	//! \param dwClubP 주클럽번호(클럽 배틀의 신청을 받은 주 )
	//! \param dwClubS 보조 클럽번호(클럽 배틀의 신청을 한 클럽 )	
	virtual int SetClubBattle(DWORD dwClubP, DWORD dwClub, DWORD dwEndTime, int nAlliance) = 0;
	
	//! 클럽 배틀을 종료한다.
	//! \param dwClubP 주클럽 번호(클럽 배틀의 신청을 받은 주 )
	//! \param dwClubS 보조 클럽 번호(클럽 배틀의 신청을 한 클럽 )
	//! \param nGuFlag 승패여부
	//! \param nGuKillNum 클럽 배틀 킬수
	virtual int EndClubBattle(DWORD dwClubP, DWORD dwClubS, int nGuFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance) = 0;

	
    //! 클럽 배틀정보를 저장한다,(클럽 배틀이 종료되기전 서버재시작이 필요한경우 배틀 정보의 저장이 필요함 )
    //! \param dwClubP 주클럽 번호(클럽 배틀의 신청을 받은 주)
    //! \param dwClubS 보조 클럽 번호(클럽 배틀의 신청을 한 클럽 )
    //! \param nGuKillNum 클럽 배틀 킬수
    //! \return
	virtual int SaveClubBattle(DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum) = 0;
	
    //! 동맹배틀 전적을 초기화 시키긴다.
    //! \param dwClub 클럽 번호
	virtual int ReSetAllianceBattle(DWORD dwClub) = 0;

    //! 해당 클럽의 배틀중인 클럽 정보를 가져온다.
    //! \param dwClub 클럽번호
    //! \param &vMember 클럽배틀 정보
    virtual int GetClubBattleInfo(DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo) = 0;
    
	virtual int UpdateClubName( DWORD dwClub, const std::string& ClubName ) = 0;

    //! 해당 캐릭터의 경험치를 세팅한다.
    //! 캐릭터번호는 0 보다 커야한다.
    //! 경험치는 0 보다 커야한다.
    //! \param  nChaNum : 캐릭터번호
    //! \param   llExp : 경험치
    virtual int SetChaExp(int nChaNum, LONGLONG llExp) = 0;
    
    //! 해당 캐릭터의 경험치를 가져온다.
    //! 캐릭터번호는 0 보다 커야한다.
    //! 경험치가 0 보다 작으면 에러이다.
    //! \param nChaNum 캐릭터번호
    //!  LONGLONG : 경험치
    virtual LONGLONG GetChaExp(int nChaNum) = 0;
        
    //! 게임내에서 진정이나 신고로 신고한 내용을 DB에 기록한다.    
    //! \param nSGNum 서버그룹
    //! \param ChaNum 신고한 캐릭터번호
    //! \param strMsg 신고내용(최대크기는 CHAT_MSG_SIZE)
    virtual int LogAppeal(int nSGNum, int nChaNum, CString strMsg) = 0;
        
    //! 현재 모든지역의 선도 클럽데이터를 가져온다.
    virtual int GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB) = 0;
        
    //! 지역과 그지역을 소유한 클럽 세율을 정한다.
    //! \param dwRegionID : 지역 ID
    //! \param dwClub : 클럽 ID
    //! \param fTax : 세율
    virtual int SetClubRegion(DWORD dwRegionID, DWORD dwClub=0, float fTax=0) = 0;
        
    //! 해당 지역의 소유 상태를 리셋한다.
    //! \param dwRegionID 지역 ID
    //! \param dwClub 클럽 ID(기본 0 이다)
    virtual int DelClubRegion(DWORD dwRegionID, DWORD dwClub=0) = 0;
		
    //! 해당클럽의 보유금액을 가져온다.(Storage)
    //! \param dwClub 클럽번호
    //! \param 리턴값은 >= 0, 0 보다 작으면 ERROR
    virtual LONGLONG GetClubMoney(DWORD dwClub) = 0;
	
    //! 해당클럽의 보유금액을 세팅한다.(Storage)
    //! \param dwClub : 클럽번호
    //! \param llMoney : 보유금액(금액은 >= 0)
	virtual int	SetClubMoney(DWORD dwClub, LONGLONG llMoney) = 0;
	
    //! 해당클럽의 수입금액을 세팅한다.(Income money)
    //! \param dwClub : 클럽번호
    //! \param llMoney : 수입금액(금액은 >= 0)
	virtual int	SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney) = 0;    
    
    //! 해당클럽의 수입금액을 가져온다.(Income money)
    //! \param dwClub : 클럽번호
    //! \return 리턴값은 >= 0, 0 보다 작으면 ERROR
	virtual LONGLONG GetClubIncomeMoney(DWORD dwClub) = 0;
        
    //! 해당클럽의 클럽창고를 읽어온다.
    //! \param dwClub : 클럽번호
    //! \param ByteStream : 버퍼
	virtual int ReadClubStorage(DWORD dwClub, se::ByteStream &ByteStream) = 0;    
	virtual int GetGuildMigrationState( DWORD dwClub ) = 0;
    
    //! 해당클럽의 클럽창고를 저장한다.
    //! \param dwClub : 클럽번호
    //! \param pData : 창고위치
    //! \param nSize : 버퍼 전체크기
	virtual int	WriteClubStorage(DWORD dwClub, BYTE* pData, int nSize) = 0;	
	
    //! 불법프로그램 사용내역을 기록한다.
    //! \param nSGNum 서버그룹
    //! \param nSvrNum 서버번호
    //! \param nUserNum 유저번호
    //! \param nChaNum 캐릭터번호
    //! \param nHackProgramNum 해킹프로그램번호
    //! \param szComment 콤멘트
	virtual int LogHackProgram(
		int nSGNum, 
		int nSvrNum, 
		int nUserNum, 
		int nChaNum, 
		int nHackProgramNum,
		const TCHAR* szComment) = 0;
		
    //! 부 클럽장을 세팅한다
    //! \param dwClub : 클럽번호
    //! \param dwChaNum : 캐릭터번호
    //! \param dwSubMasterFlags : 플래그(0 으로 넣으면 일반길드원이 된다)
	virtual int	SetClubMasterFlags(
		DWORD dwClub,
		DWORD dwChaNum,
		DWORD dwSubMasterFlags) = 0;
	
    //! 동맹 클럽을 결성한다.
    //! \ db 는 클럽의 무결성을 체크하지 않는다.
    //! \ A 가 B 의 보조인 상태에서 
    //! \ B 가 C 의 보조로 들어가고
    //! \ C 가 A 의 보조로 들어가면 소유권 체인 문제가 발생한다.	
    //! \param dwClubP : 주 클럽번호(동맹 클럽의 주)
    //! \param dwClubS : 보조 클럽번호(주 클럽 밑으로 들어가는 하위 클럽)
	virtual int	SetClubAlliance(DWORD dwClubP, DWORD dwClubS) = 0;
	
    //! 동맹 클럽을 해체한다.
    //! \param dwClubP : 주 클럽번호(동맹 클럽의 주)
    //! \param dwClubS : 보조 클럽번호(주 클럽 밑으로 들어가는 하위 클럽)
	virtual int	DelClubAlliance(DWORD dwClubP, DWORD dwClubS) = 0;

    //! 동맹 클럽 리스트를 가져온다.
    //! <주클럽,보조클럽>
	virtual int	GetClubAlliance(std::vector< std::pair<DWORD,DWORD> > &vecAlliance) = 0;
	
    //! 해당 사용자의 채팅 블록시간을 가져온다
	virtual __time64_t  GetChatBlockTime(int nUserNum) = 0;
	
    //! 해당 사용자의 채팅 블록시간을 설정한다
	virtual int SetChatBlockTime(int nUserNum, __time64_t tBlockTime) = 0;
		
    //! 클럽의 공지사항을 세팅한다.
    virtual int SetClubNotice(const std::string& ChaName, DWORD dwClub, const std::string& ClubNotice) = 0;
	
    //! 클럽의 선도클럽 인증 대리자를 세팅한다.
	virtual int SetClubDeputy(DWORD dwClub, DWORD dwDeputy) = 0;
	
    //! 클럽의 마스터 권한을 위임한다.
	virtual int SetClubAuthority(DWORD dwClub, DWORD dwMasterID) = 0;

    virtual int InsertClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) = 0;
    virtual int UpdateClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) = 0;
    virtual int ChangeClubAuthIndex(DWORD ClubDbNum, size_t AuthIndexA, size_t AuthIndexB) = 0;
    virtual int ChangeClubMemberGrade(DWORD ChaDbNum, int Grade) = 0;
	
    //! 캐릭터의 이름을 변경한다.
    //! \param dwChaNum 캐릭터번호
    //! \param szCharName 변경할 캐릭터이름    
	virtual int RenameCharacter(DWORD dwChaNum, char* szCharName) = 0;
	
    //! 캐릭터의 머리색을 변경한다.    
	virtual int SetChaHairColor(DWORD dwChaNum, int nHairColor) = 0;
	
    //! 캐릭터의 헤어스타일을 변경한다.    
	virtual int SetChaHairStyle(DWORD dwChaNum, int nHairStyle) = 0;
	
    //! 캐릭터의 얼굴스타일을 변경한다.    
	virtual int SetChaFaceStyle(DWORD dwChaNum, int nFaceStyle) = 0;
	
    //! 캐릭터의 성별을 변경한다.    
	virtual int SetChaGenderChange(DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor) = 0;

	// 캐릭터 학원을 변경한다
	virtual int SetChaSchoolChange(DWORD dwChaNum, int nSchool) = 0;
	
    //! 캐릭터의 위치정보를 저장한다.
    //! \param dwChaNum 캐릭터 번호
    //! \param dwStartmapID 시작하는 맵 번호
    //! \param dwStartGate 시작하는 게이트 번호
    //! \param fStartPosX 시작하는 X 좌표
    //! \param fStartPosY 시작하는 Y 좌표
    //! \param fStartPosZ 시작하는 Z 좌표
    //! \param dwSaveMapID 마지막으로 플레이한 맵 번호
    //! \param fSavePosX 마지막으로 플레이한 X 좌표
    //! \param fSavePosY 마지막으로 플레이한 Y 좌표
    //! \param fSavePosZ 마지막으로 플레이한 Z 좌표
    //! \param dwLastCallMapID 시작 귀환카드에 저장된 맵 번호
    //! \param fLastCallPosX 시작 귀환카드에 저장된 X 좌표
    //! \param fLastCallPosY 시작 귀환카드에 저장된 Y 좌표
    //! \param fLastCallPosZ 시작 귀환카드에 저장된 Z 좌표
    //! \return sc::db::DB_OK, sc::db::DB_ERROR
	virtual int	SaveChaPos(DWORD dwChaNum,
				 DWORD dwStartMapID,
				 DWORD dwStartGate,
				 FLOAT fStartPosX,
				 FLOAT fStartPosY,
				 FLOAT fStartPosZ,
				 DWORD dwSaveMapID,
				 FLOAT fSavePosX,
				 FLOAT fSavePosY,
				 FLOAT fSavePosZ,
				 DWORD dwLastCallMapID,
				 FLOAT fLastCallPosX,
				 FLOAT fLastCallPosY,
				 FLOAT fLastCallPosZ) = 0;
	
    //! 새로운 팻을 생성한다.
    //! \param nChaNum  팻 소유자 캐릭터 번호
    //! \param szPetName 팻이름
    //! \param nPetType 팻의 타입
    //! \param nPetMID 팻 MID
    //! \param nPetSid 팻 SID
    //! \param nPetStyle 팻의 스타일
    //! \param nPetColor 팻의 컬러
    //! \param m_nPetCardMID 팻 카드의 MID
    //! \param m_nPetCardSID 팻 카드의 SID
    //! \return sc::db::DB_ERROR 또는 성공했을때는 Pet 의 고유번호
	virtual int CreatePet(
		int nChaNum,
		const TCHAR* szPetName,
		int nPetType,
		int nPetMID,
		int nPetSID,
		int nPetStyle,
		int nPetColor,
		int nPetCardMID,
	    int nPetCardSID
		) = 0;

	
    //! 펫의 이름을 변경한다.
    //! \param nChaNum 캐릭터 번호
    //! \param nPetNum 팻의 고유번호
    //! \param szPetName 팻이름
    //! \return sc::db::DB_ERROR, sc::db::DB_OK
	virtual int RenamePet(int nChaNum, int nPetNum, const TCHAR* szPetName) = 0;

    //! 팻의 소유권을 이전한다.
    //! \param nChaNum 팻의 새로운 소유자
    //! \param nPetNum 소유권을 이전할 팻 번호
	virtual int ExchangePet(int nChaNum, int nPetNum) = 0;

    //! 팻의 컬러를 변경한다.
    //! \param nChaNum 캐릭터 번호
    //! \param nPetNum 컬러를 변경하려는 팻의 번호
    //! \param nPetColor 팻의 컬러
	virtual int SetPetColor(int nChaNum, int nPetNum, int nPetColor) = 0;

    //! 팻의 스타일을 변경한다.
    //! \param nPetNum 스타일을 변경하려는 팻의 번호
    //! \param nPetStyle 팻의 스타일
	virtual int SetPetStyle(int nChaNum, int nPetNum, int nPetStyle) = 0;
	
    //! 팻의 포만도를 설정한다. 포만도는 0-100 사이
    //! \param nPetNum 포만도를 설정할 팻의 번호
    //! \param fPetFull 포만도
	virtual int SetPetFull(int nChaNum, int nPetNum, int nPetFull) = 0;
	
    //! 팻의 인벤토리 업데이트
	virtual int SetPetInven(int nChaNum, int nPetNum, se::ByteStream &ByteStream) = 0;
	
	virtual int SetPetMigrationState( int ChaNum, int PetNum, int MigrationState ) = 0;
	virtual int SetVehicleMigrationState( int ChaNum, int VehicleNum, int MigrationState ) = 0;

    //! 팻의 스킬 업데이트
	virtual int SetPetSkill(
		int nChaNum,
		int nPetNum,
		int nPetInvenType,
		int nPetInvenMID,
		int nPetInvenSID,
		int nPetInvenCMID,
		int nPetInvenCSID,
		int nPetInvenAvailable) = 0;
	
    //! 팻의 스킬슬롯 갯수 업데이트
	virtual int SetPetSkillSlotNum(int nChaNum, int nPetNum, WORD wSkillSlotNum) = 0;
	
    //! 팻을 삭제한다.
    //! \param nPetNum 삭제할 팻의 번호
	virtual int	DeletePet(int nChaNum, int nPetNum) = 0;	

	//! 팻의 스킬을 가져온다.
	//! \param pPet 펫의 포인터
	virtual int GetPetSkill(int nChaNum, GLPET* pPet, DWORD dwPetNum) = 0;
		
    //! 팻의 인벤토리 정보를 가져온다
	virtual int GetPetInven(int nChaNum, GLPET* pPet, DWORD dwPetNum) = 0;
		
    //! 팻의 정보를 가져온다.
	virtual int GetPet(GLPetField* pPet, DWORD dwPetNum, DWORD dwClientID) = 0;
	
    //! 팻의 포만감을 가져온다
    //! \param dwPetNum 팻 번호
    //! \return 팻의 포만감 or sc::db::DB_ERROR
	virtual int GetPetFull(int nChaNum, DWORD dwPetNum) = 0;

    //! 소멸된 팻의 리스트를 가져온다.
	//! \param nChaNum 소멸된 펫 리스트를 출력하려는 캐릭터 번호
	//! \param vecPetReList 소멸된 펫 리스트를 넣을 벡터
	virtual int GetRestorePetList(int nChaNum, std::vector<PET_RELIST>& vecPetReList) = 0;
	
    //! 팻을 부활시킨다.
    //! \param nPetNum 부활할 팻 번호
    //! \param nChaNum 부활을 요구한 캐릭터 번호
	virtual int RestorePet(DWORD dwPetNum, int nChaNum) = 0;

    //! 팻의 스킨팩을 업데이트 한다.
    //! \param nChaNum		팻소유 캐릭터 번호
    //! \param dwPetNum		펫 번호
    //! \param nPetSkinMID	펫 스킨 MID
    //! \param nPetSkinSID	펫 스킨 SID
    //! \param nPetSkinScale	펫 스킨 Scale
    //! \param nPetSkinTime	펫 스킨 적용시간
	virtual int UpdatePetSkin(int nChaNum,
							   DWORD dwPetNum,
							   int nPetSkinMID,
							   int nPetSkinSID,
							   int nPetSkinScale,
							   int nPetSkinTime) = 0;

	
    //! SMS 문자메시지를 발송한다.
    //! \param nSendChaNum SMS 를 발송하는 캐릭터 번호	
    //! \param nReceiveChaNum SMS 를 받을 캐릭터 번호
    //! \param szReceivePhone SMS 를 받을 캐릭터의 전화번호
    //! \param szSmsMsg SMS 메시지 내용
	virtual int LogSMS(
					int nSendChaNum,					
					int nReceiveChaNum,
					const TCHAR* szReceivePhone,
					const TCHAR* szSmsMsg) = 0;

	
    //! 아이템의 랜덤옵션 로그를 남긴다.
    //! \param _sLOG 랜덤옵션 구조체
	virtual int LogRandomItem(const LOG_RANDOM_OPTION &_sLOG) = 0;

	// 코스튬 능력치 부여 로그
	virtual int LogItemCostumeStat(const LOG_COSTUME_STATS &_sLOG) = 0;
	
	virtual int LogAddonItem( const LOG_ADDON_OPTION &_sLOG ) = 0;
	virtual int LogLinkSkillItem( const LOG_LINKSKILL_OPTION &_sLOG ) = 0;
	virtual int LogBasicStatItem( const LOG_BASICSTAT_OPTION &_sLOG ) = 0;

    //! 캐릭터의 휴대폰 번호를 저장한다.
    //! \param nChaNum 캐릭터 번호
    //! \param szPhoneNumber 전화번호 NULL 문자 포함 14자 SMS_RECEIVER
	virtual int SetChaPhoneNumber(int nChaNum, const TCHAR* szPhoneNumber) = 0;
	
    //! 팻의 액션 로그를 남긴다.
    //! \param _sLOG 펫액션로그 구조체
	virtual int LogPetAction(const LOG_PET_ACTION &_sLOG) = 0;
	
    //! Terra
    //! 유저의 패스워드를 넣거나 체크한다.
    //! \param szTID Terra UserID
    //! \param szPasswd 사용자 패스워드
    //! \return 
	virtual int TerraUserPassCheck(const TCHAR* szTID, const TCHAR* szPasswd, int nCheckFlag) = 0;
	
    //! Daum
    //! 유저의 패스워드를 넣거나 체크한다.
    //! \param szDaumGID Daum UserGID
    //! \param szDaumPass 사용자 패스워드
    //! \return 
	virtual int DaumUserPassCheck(const TCHAR* szDaumGID, const TCHAR* szDaumPasswd, int nCheckFlag) = 0;

    //! Excite
    //! 유저의 패스워드를 넣거나 체크한다.
    //! \param szUID Excite UserID
    //! \param szPasswd 사용자 패스워드
    //! \return 
	virtual int ExciteUserPassCheck(
        const TCHAR* szUID, const TCHAR* szUserID, const TCHAR* szPasswd, int nCheckFlag) = 0;

    //! Thailand
    //! 사용자의 PC방 클래스를 체크한다.	
    //! \param szUsrIP 사용자의 IP Address
    //! \return DB 인증후 결과값
	virtual int	ThaiUserClassCheck(const TCHAR* szUsrIP) = 0;
	 
    //! Malaysia
    //! 사용자의 PC방 클래스를 체크한다.	
    //! \param szUsrIP 사용자의 IP Address
    //! \return DB 인증후 결과값
	virtual int	MyUserClassCheck(const TCHAR* szUsrIP) = 0;
	 
    //! 필리핀
    //! 사용자의 PC방 클래스를 체크한다.	
    //! \param szUsrIP 사용자의 IP Address
    //! \return DB 인증후 결과값
	virtual int	PhUserClassCheck(const TCHAR* szUsrIP) = 0;
		
    //! 새로운 탈것을 생성한다.
    //! \param nChaNum  탈것 소유자 캐릭터 번호
    //! \param szVehicleName 탈것 이름
    //! \param nVehicleType 탈것의 타입
    //! \param nVehicleMID 탈것 MID
    //! \param nVehicleSID 탈것 SID
    //! \param nVehicleCardMID 탈것의 아이템 MID
    //! \param nVehicleCardSID 탈것의 아이템 SID
    //! \param nBoosterID 탈것의 부스터 종류
    //! \return sc::db::DB_ERROR 또는 성공했을때는 탈것의 고유번호
	virtual int CreateVehicle(
		int nCharNum,
		const TCHAR* szVehicleName,
		int nVehicleType,
		int nVehicleCardMID,
		int nVehicleCardSID) = 0;

    //! 탈것의 배터리를 설정한다. 배터리는 0-1000 사이
    //! \param nVehicleBttery 배터리를 설정할 탈것의 번호
    //! \param nVehicleBttery 배터리
	virtual int SetVehicleBattery(
		int nVehicleNum,
		int nCharNum,
		int nVehicleBttery) = 0;
	
    //! 탈것의 배터리를 가져온다.
    //! \param nVehicleBttery 배터리를 설정할 탈것의 번호
	virtual int GetVehicleBattery(int nVehicleNum, int nCharNum) = 0;
	
    //! 탈것을 삭제한다.
    //! \param nVehicle 삭제할 탈것의 번호
	virtual int DeleteVehicle(int nVehicle, int nCharNum) = 0;

	virtual int RebuyVehicle(int nVehicle, int nCharNum) = 0;
	
	//! 탈것의 정보를 가져온다.	
	virtual int GetVehicle(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) = 0;
	
    //! 탈것의 인벤토리 정보를 가져온다
	//virtual int GetVehicleInven(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) = 0;
	
	//! 탈것 인벤토리 업데이트
	virtual int SetVehicleInven(int nVehicle, int nCharNum, se::ByteStream& ByteStream) = 0;

    //! 탈것의 컬러 정보를 가져온다
	//virtual int GetVehicleColor(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) = 0;

	//! 탈것 컬러 업데이트
	virtual int SetVehicleColor(int nVehicle, int nCharNum, se::ByteStream &ByteStream) = 0;
	
    //! 탈것의 부스터 스킬 업데이트
    //! \param m_nVehicleBooster 배운 부스터 종류
	virtual int SetVehicleBooster(int nVehicleNum, int nCharNum, DWORD dwVehicleBooster) = 0;
	
    //! 탈것의 액션 로그를 남긴다.
    //! \param _sLOG 탈것액션로그 구조체
	virtual int LogVehicleAction(const LOG_VEHICLE_ACTION &_sLOG) = 0;

	// 출석부 이벤트 
	virtual int LogUserAttend(int nUserNum, std::vector<USER_ATTEND_INFO> &vecUserAttend) = 0;

	virtual int InsertUserAttend(int nUserNum, CString strUserName, __time64_t tAttendTime, int nCount,
								  int nAttendReward, int nItemMID, int nItemSID) = 0;

	virtual int InsertShopItemMap(int nItemMID, int nItemSID) = 0;

	//	UserMoney Add
	virtual int UpdateUserMoneyAdd(DWORD dwUserID, LONGLONG lnUserMoney) = 0;
	virtual int UpdateUserLastInfoAdd(DWORD dwUserID, LONGLONG lnUserMoney) = 0;

	//
	//mjeon.dbman.from odbcmanager
	//
	virtual	int		SetAllCharacterOffline()				 = 0;
	virtual int		AllServerUserLogout(int nSvrGrp)		 = 0;	

	//
	//mjeon.dbman.from DbLogicAction
	//

	//GameActions
	virtual int	PostMaintenance()											 = 0;
	virtual int	PostCleanUp(char *pszCurrentDate, char *pszTargetDate)		 = 0;
	virtual int	PostReturn(LONGLONG llPostID)								 = 0;
	virtual int	PostDelete(LONGLONG llPostID)								 = 0;
	virtual int	PostOpen(LONGLONG llPostID)									 = 0;
	virtual int	PostGetInfo(int iRecverNum, int &cntAllPost, int &cntNewPost) = 0;
	virtual int	PostGetData(int iRecverNum, std::vector<SAPOST>& vPost, int &nRowCount, DWORD dwRecverUserNum) = 0;
    virtual int	PostGetState(int iRecverNum, std::vector<SAPOSTSTATE>& vPost, int &nRowCount) = 0;
    virtual int	PostGetSendState(int iRecverNum, std::vector<SAPOSTSTATE>& vPost, int &nRowCount) = 0;
    virtual int	PostGetRecvState(int iRecverNum, std::vector<SAPOSTSTATE>& vPost, int &nRowCount) = 0;

	
	virtual int	GetChaNum(const char *szCharName, int *pnChaNum, int *pnUserNum) = 0;

	virtual int	DelCharacter(int nUsrNum, int nChaNum, const char* szPass2)	 = 0;
	virtual int	DaumDelCharacter(int nUsrNum, int nChaNum, const char* szPass2)					 = 0;
	virtual int	IdnDelCharacter(int nUsrNum, int nChaNum)					 = 0;
	virtual int	GspDelCharacter(int nUsrNum, int nChaNum)					 = 0;
	virtual int	TerraDelCharacter(int nUsrNum, int nChaNum)					 = 0;
	virtual int	ExciteDelCharacter(int nUsrNum, int nChaNum)				 = 0;
	virtual int	JapanDelCharacter(int nUsrNum, int nChaNum)					 = 0;
	virtual int	GsDelCharacter(int nUsrNum, int nChaNum, const char* szPass2)					 = 0;
	//virtual int GetChaBAInfo(int nUsrNum, int nSvrGrp, NET_CHA_BBA_INFO_DH* ncbi) = 0;
    virtual int GetChaBAInfo(int UserNum, int SvrGrp, std::vector<int>& vChaNum) = 0;
	virtual int	GetUserTempInfo(int nUserNum )								 = 0;
	virtual int	GetChaBInfo(int nUserNum, int nChaNum, SCHARINFO_LOBBY* sci) = 0;
	virtual int	GetChaBInfoForMigration( int nUserNum, int nChaNum, SCHARINFO_LOBBY* sci ) = 0;
    virtual int GetChaBInfoAll(int UserDbNum, int ServerGroup, std::vector<SCHARINFO_LOBBY>& vCharInfo) = 0;
	virtual int GetChaBInfoAllForMigration( int UserDbNum, int ServerGroup, std::vector< SCHARINFO_LOBBY >& vCharInfo ) = 0;
	virtual int	GetCharacterInfo(int nUserNumber, int nChaNum, GLCHARAG_DATA* pChaData) = 0;
	virtual int SetCharacterOption( int ChaNum, int CharacterOption ) = 0;
	virtual int GetCharacterOption( int ChaNum ) = 0;
	virtual int	GetCharacterInfo(int nUserNumber, int nChaNum, SCHARDATA2* pChaData2) = 0;
	virtual int	GetCharacterInfoExtraWithAdo(int nUserNumber, int nChaNum, SCHARDATA2* pChaData2) = 0;
	virtual int	Certification2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) = 0; // 2차비번
	virtual int	Update2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) = 0; // 2차비번

	virtual int	GetChaFriend(int nChaNum, std::vector<CHA_FRIEND> &vecFriend) = 0;
	virtual int	SetCharacterOnline(int nChaNum)								 = 0;
	virtual int	SetCharacterOffline(int nChaNum)							 = 0;
    virtual int GetChaFriendSMS(int nChaNum, std::vector<CHA_FRIEND_SMS> &vFriend) = 0;

	virtual int GetUserInvenOption( int UserNum, int& Option ) = 0;
	
	//UserActions
	virtual int GetUserCountryInfo ( IN int nUserNum, OUT Country::SCOUNTRY_INFO& sCountryInfo )		= 0;
	virtual int	GetUserType(int nUserNum)				 = 0;
	virtual int	UserTestChaNumDecrease(int nUserNum)	 = 0;
	virtual int	UserChaNumDecrease(int nUserNum)		 = 0;
	virtual int	GetChaGuildNum(DWORD nChaNum)			 = 0;
	virtual int	UserTestChaNumIncrease(int nUserNum)	 = 0;
	virtual int	UserUpdateCha(int nUsrNum, const TCHAR* szChaName) = 0;
	virtual int	UserLogoutSimple(const TCHAR* szUsrID)	 = 0;
	virtual int	UserLogoutSimple2(int nUserNum)			 = 0;
	virtual int	UserLogout(const TCHAR* szUsrID, 
					const TCHAR* szUsrIP, 
                    int nUsrNum, 
                    int nGameTime,
                    int nChaNum,
                    int nSvrGrp,
                    int nSvrNum, 
                    int nTotalGameTime,
                    int nOfflineTime )			 = 0;
	virtual int	ThaiUserCheck(const TCHAR* szUsrID,
                    const TCHAR* szPasswd,
                    const TCHAR* szUsrIP,
                    int nSvrGrp,
                    int nSvrNum )				 = 0;
	virtual int ThaiGetUserInfo(const TCHAR* szUserID, NET_LOGIN_FEEDBACK_DATA2* pMsg ) = 0;
	virtual int	UserCheck(
                    const TCHAR* szUsrID, 
                    const TCHAR* szPasswd, 
                    const TCHAR* szUsrIP,
                    const TCHAR* szRandomPasswd,
                    int nRandomNum,
                    int nSvrGrp,
                    int nSvrNum)				 = 0;

	virtual int IdnUserCheck(int nUserNum, const std::string &strId, const std::string &strIP, int nSvrGrp, int nSvrNum) = 0;

	virtual int	GetUserInfo(const TCHAR* szUserID, NET_LOGIN_FEEDBACK_DATA2* pMsg) = 0;

	virtual USER_INFO_BASIC GetUserInfo(const TCHAR* szUsrId, const TCHAR* szPasswd) = 0;

	virtual int IdnGetUserInfo(const std::string &strId, NET_LOGIN_FEEDBACK_DATA2* pMsg) = 0;

	virtual int	DaumUserCheck(
                TCHAR* szDaumGID,
                const TCHAR* szUUID,
                const TCHAR* szUserIP,
                int nSvrGrp,
                int nSvrNum,
                int& Age OUT) = 0;
	virtual int	DaumGetUserInfo(const TCHAR* szGID, DAUM_NET_LOGIN_FEEDBACK_DATA2* pMsg) = 0;
	
    virtual int	GspUserCheck(
        TCHAR* szGspUserID,
        const TCHAR* szUUID,
        const TCHAR* szUserIP,
        int nSvrGrp,
        int nSvrNum) = 0;

	virtual int GspGetUserInfo(const TCHAR* szUserID, GSP_NET_LOGIN_FEEDBACK_DATA2* pMsg) = 0;

	virtual int TerraUserCheck(
        TCHAR* szTerraLoginName,
        const TCHAR* szTerraDecodedTID,
        const TCHAR* szUsrIP,
        int nSvrGrp,
        int nSvrNum) = 0;
	
    virtual int TerraGetUserInfo(const TCHAR* szTLoginName, TERRA_NET_LOGIN_FEEDBACK_DATA2* pMsg) = 0;
	
    virtual int GsUserCheck(
        const TCHAR* szUsrID,
        const TCHAR* szPasswd,
        const TCHAR* szUsrIP,
        int nSvrGrp,
        int nSvrNum) = 0;
    virtual int UserDbLogItemDataInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName) = 0;
	
    virtual int GsGetUserInfo(const TCHAR* szUserID, GS_NET_LOGIN_FEEDBACK_DATA2* pMsg) = 0;

	virtual int GmUserCheck(
		const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		int nSvrGrp,
		int nSvrNum ) = 0;

	virtual int GmGetUserInfo( const TCHAR* szUserID, GM_NET_LOGIN_FEEDBACK_DATA2* pMsg ) = 0;

	virtual int UsUserCheck(
		const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		int nSvrGrp,
		int nSvrNum ) = 0;

	virtual int UsGetUserInfo( const TCHAR* szUserID, US_NET_LOGIN_FEEDBACK_DATA2* pMsg ) = 0;

	virtual int ExciteUserCheck(
        const TCHAR* szUserID,
        const TCHAR* szUserIP, 
        int nSvrGrp, 
        int nSvrNum) = 0;

	virtual int ExciteGetUserInfo(const TCHAR* szUserID, EXCITE_NET_LOGIN_FEEDBACK_DATA2* pMsg) = 0;

	virtual int JapanUserCheck(
        const TCHAR* szUserID,
        const TCHAR* szUserIP, 
        int nSvrGrp, 
        int nSvrNum,
        int nUserNum) = 0;
	virtual int JapanGetUserInfo(const TCHAR* szUserID, JAPAN_NET_LOGIN_FEEDBACK_DATA2* pMsg) = 0;

	virtual int WorldBattleUserCheck(
        const std::string& UserId,
        const std::string& UserPass,
        const std::string& UserIp,
        int ServerGroup,
        int ServerNumber) = 0;

    virtual int WorldBattleGetUserInfo(const std::string& UserId, NET_LOGIN_FEEDBACK_DATA2& Msg) = 0;

	virtual int UserLogoutSvr(int nSvrGrp, int nSvrNum) = 0;
	virtual int	UserBlock(int nUserNum, int nDay) = 0;
	virtual int	UserBlockDate(int nUserNum, __time64_t tBlockTime) = 0;
	
	virtual WORD ThaiGetTodayPlayTime(int nUserNum) = 0;
	virtual int	UserLogoutSimple3(const TCHAR* szUsrID, int nSvrGrp) = 0;
		
	//LogActions
	virtual int AddLogServerState(const __time64_t& LogDate, int nSGNum, int nSvrNum, int nUserNum, int nUserMax) = 0;
	//virtual int LogLottery(CString strLottery, CString strIP, CString strUserUID, int nUserNum) = 0;
	//virtual int LogPungPungWrite(CString strIP, CString strUserUID, int nUserNum, int nPrize) = 0;
	//virtual int LogPungPungCheck(CString strIP, int nUserNum) = 0;
	virtual int	AllUserLogout() = 0;
    
    //
    //mjeon.sns
    //
    virtual int SNSAuthedFB(SFACEBOOK &AuthFB) = 0;
    virtual int SNSUnAuthedFB(int nChaNum) = 0;
    virtual int SNSGetInfoFB(SFACEBOOK &AuthFB) = 0;
    virtual int SNSGetUidFB(int nTargetChaNum, std::string &m_UID) = 0;

    virtual int SNSAuthedTW(STWITTER &AuthTW) = 0;
    virtual int SNSUnAuthedTW(int nChaNum) = 0;
    virtual int SNSGetInfoTW(STWITTER &AuthTW) = 0;
    virtual int SNSGetUidTW(int nTargetChaNum, std::string &m_UID) = 0;

    virtual int SNSGetInfo(SFACEBOOK &AuthFB, STWITTER &AuthTW) = 0;

	//
	//mjeon.attendance
	//
	virtual int AttendanceLoad(OUT AttendanceManager &AttendanceMan, UINT nChaNum) = 0;
	virtual int AttendanceSave(IN  AttendanceManager &AttendanceMan, UINT nChaNum, LONGLONG nChaCurExp)		= 0;
	virtual int AttendanceMaintenance()													= 0;

	//
	//mjeon.activity
	//
	virtual int ActivityGetClosedActivityID(OUT ActivityBaseData &abd)					= 0;
	virtual int ActivityLoad(MMACTIVITIES &mmActivities, UINT nChaNum)					= 0;
	virtual int ActivitySave(MMACTIVITIES &mmActivities, UINT nChaNum, UINT nChaLevel)	= 0;
	virtual int ActivityOrdering(Activity &actv, UINT nChaNum, UINT nChaLevel)			= 0;
	virtual int ActivityGetPoint(OUT LONGLONG *pllRewardPoint, UINT nChaNum)			= 0;
	virtual int ActivityLoadBadge(UINT nChaNum, TitleManager &TitleMan)					= 0;
	virtual int ActivitySetPoint(IN LONGLONG *pllRewardPoint, UINT nChaNum)				= 0;
	virtual int ActivitySaveBadge(UINT nChaNum, TitleManager &TitleMan)					= 0;

	//
	// Provide Reward System
	//
	virtual int ProvideRewardInitialize ( )																	= 0;
	virtual int ProvideRewardInsert ( const ProvideRewardSystem::PROVIDE_REWARD_DATA& _ProvideRewardData )	= 0;
	virtual int ProvideRewardSelect ( DWORD _dwChaDBNum, DWORD _dwServerGroupNum, OUT ProvideRewardSystem::VEC_PROVIDE_REWARD& _vecProvideRewardData )													= 0;
	virtual int ProvideRewardClearDeleteFlag ( )															= 0;
	virtual int ProvideRewardComplete ( DWORD _dwUniqueID )													= 0;

	//
	// Country System
	//
	virtual int UpdateUserCountry ( IN DWORD dwUserID, IN Country::SCOUNTRY_INFO& sCountryInfo )			= 0;
	virtual int LoadVictoriousCountry ( OUT Country::VCOUNTRY_VEC& vecVCountry )							= 0;
	virtual int AddVictoriousCountry ( IN Country::SVICTORIOUS_COUNTRY& sVictoriousCountry )				= 0;
	virtual int ResetVictoriousCountry ( IN Country::EMCOUNTRY_SYSTEM_FLAG emFlag )							= 0;
	virtual int ConfirmStateVictoriousCountry ( IN Country::EMCOUNTRY_SYSTEM_FLAG emFlag, OUT Country::VCOUNTRY_VEC& vecVCountry )							= 0;

	//
	// Lotto System
	//
	virtual int LottoSystemLoad( unsigned int systemId )								= 0;

	virtual int LottoDrawingMachineNumSelect(
		unsigned int lottoNum, unsigned int systemId )									= 0;
	virtual int LottoSystemCurTurnSelect( unsigned int lottoSystemId )					= 0;
	virtual int LottoAccumulateMoneySelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					= 0;
	virtual int LottoWinNumSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					= 0;
	virtual int LottoCharBuyListSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					= 0;
	virtual int LottoCharNonConfirmBuyListSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int systemId, int confirmTicket )						= 0;
	virtual int LottoWinManListSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					= 0;
	virtual int LottoInventWinNumSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					= 0;

	virtual int LottoDrawingMachineNumSave(
		unsigned int lottoNum, unsigned int accumulateCount,
		unsigned int addCount, unsigned int systemId, bool bUpdate = true )				= 0;
	virtual int LottoSystemTurnSave( unsigned int turnNum, unsigned int systemId )		= 0;

	virtual int LottoCharBuyListInsert(
		LONGLONG accumulateMoney,
		LONGLONG charge,
		int buyType, const std::string& drawingNum,
		LONGLONG drawingNumInt, int confirmTicket,
		DWORD dwUserNum, DWORD dwDbNum, DWORD dwServerGroup, char* szChaName, unsigned int turnNum, unsigned int systemId )					= 0;

	virtual int LottoWinManListUpdate(
		unsigned int ranking, LONGLONG winMoney, SNATIVEID sItemID, DWORD dwItemCount,
		unsigned int turnNum, unsigned int systemId )									= 0;
	virtual int LottoWinManListInsert(
		unsigned int ranking, LONGLONG winNumInt,
		LONGLONG bonusNumInt, unsigned int systemId )									= 0;

	virtual int LottoAccumulateMoneyUpdate( LONGLONG accumulateMoney,
		unsigned int turnNum, unsigned int systemId )									= 0;
	virtual int LottoWinNumUpdate( const std::string& winNum, LONGLONG winNumInt,
		LONGLONG bonusNumInt, unsigned int turnNum, unsigned int systemId )				= 0;
	virtual int LottoCharNonConfirmBuyListUpdate(
		DWORD dwDbNum, DWORD dwServerGroup, LONGLONG invenMoney, unsigned int systemId )						= 0;

	virtual int LottoSystemNextTurn( unsigned int systemId )							= 0;
	virtual int LottoConfirmWinMan( unsigned int systemId )								= 0;
	virtual int LottoChangeInventWinNum( unsigned int turnNum, unsigned int systemId )	= 0;
	virtual int LottoWinMoneyUpdate( unsigned int turnNum, unsigned int systemId )		= 0;

	virtual int LottoSystemBuyLog( const LottoSystem::LOTTO_SYSTEM_BUY_LOG& logInfo )	= 0;
	virtual int LottoSystemWinManLog( unsigned int turnNum, unsigned int systemId )		= 0;
	virtual int LottoSystemGiveWinTicketLog( unsigned int systemId,
		DWORD dwTicketID, LONGLONG invenMoney )											= 0;
	virtual int LottoSystemTurnInfoLog(
		const LottoSystem::LOTTO_SYSTEM_TURN_LOG& logInfo )								= 0;

    // PointShop
    virtual int PointShopList(__time64_t& UpdateTime, std::vector<pointshop::SellItem>& ItemList, bool bStandAlone) = 0;
    virtual int GetChaPoint(int ChaDbNum, __int64& PointRefundable, __int64& PointNotRefundable) = 0;
    virtual int SetChaPoint(int ChaDbNum, const __int64& PointRefundable, const __int64& PointNotRefundable) = 0;
    virtual int LogPointShopInsert(DWORD ChaDbNum, const std::string& LogMessage) = 0;
	virtual int LogPointShopInsert(
		DWORD ChaDbNum, int LogType, 
		__int64& BeforeRefundable, 
		__int64& BeforeNotRefundable, 
		__int64& AfterRefundable, 
		__int64& AfterNotRefundable, 
		WORD ItemMid, 
		WORD ItemSid) = 0;

    //! Billing 관련 Shop, Item 판매 정보 로그
    virtual int ShopSaleLog(
        const SNATIVEID& ItemId,
        __int64 Price,
        EM_BILLING_SELL_TYPE Type,
        int UserDbNum,
        int ServerNum,
        int ChaDbNum,
        int ChaLevel) = 0;
	
	//mjeon.CaptureTheField
	virtual int CTFStatLog(UINT *arrPlayerNum, UINT state)									= 0;
	virtual int CTFResultLog(WORD wSchoolAuthed, CTF_REWARD_BUFF *arrReward, UINT nHowMany)	= 0;
	virtual int CTFPlayLog(UINT nChaNum, WORD wSchool, int nTotalPoint, int nRewardPoint)	= 0;

	//! 최신 CTF 갯수(rowcount)만큼 가져온다.
	virtual int GetCTFInfo(UINT rowcount ,std::vector<DWORD >& vAuthSchool) = 0;

	virtual int LoadChaExtraInfo(UINT nChaNum, int &nContributionPoint)						= 0;
	virtual int SaveChaExtraInfo(UINT nChaNum, int nContributionPoint)						= 0;

    //! Log Database
    virtual int LogItemListInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName) = 0;
	virtual int LogQuestListInsertUpdate( int Mid, int Sid, const std::string& Name ) = 0;
	virtual int LogSkillListInsertUpdate( int Mid, int Sid, const std::string& Name, int Grade ) = 0;

	virtual int LogGmCommandInsert( int ChaNum, const std::string& Command, const std::string& Param ) = 0;

	virtual int LogGMChat( int nChatType, int nGMType, const std::string& SendName, const std::string& RecvName, const std::string& ChatMsg ) = 0;

    virtual int ExecuteSqlLogDb(const std::string& SqlStr) = 0;

    virtual int LogMiniGameOddEvenInsert(
        DWORD UserDbNum,
        DWORD CharDbNum,        
        BYTE  CurrentRound,
        BYTE  WinLost,
        __int64 BattingMoney,
        __int64 DividendMoney,
        __int64 ChaMoney) = 0;

    virtual int ChaFriendGroupSelect(int ChaDbNum, std::vector<FRIEND_GROUP>& vGroup) = 0;
    virtual int ChaFriendGroupInsert(int ChaDbNum, WORD GroupNum, const std::string& GroupName) = 0;
    virtual int ChaFriendGroupUpdate(int ChaDbNum, WORD GroupNum, const std::string& GroupName) = 0;
    virtual int ChaFriendGroupDelete(int ChaDbNum, WORD GroupNum) = 0;
    virtual int ChaFriendGroupSet(int ChaP, int ChaS, WORD GroupNum) = 0;
    virtual int UserCountryUpdate(int UserDbNum, wb::EM_COUNTRY Country) = 0;

    virtual int ClubNickUpdate(DWORD ChaDbNum, const std::string& NickName) = 0;
    virtual int ClubPublicFlagUpdate(DWORD ClubDbNum, DWORD Flag) = 0;
    virtual int ClubAutoKickUpdate(DWORD ClubDbNum, size_t GradeIndex, WORD Day, bool AutoKickOut) = 0;
    virtual int ClubNewbieNoticeUpdate(DWORD ClubDbNum, const std::string& Notice) = 0;
    virtual int ChaClubSecedeTimeReset(DWORD ChaDbNum) = 0;

    virtual int NotifySelect(int ChaDbNum, std::vector<std::tr1::shared_ptr<gamelogic::NotifyBase> >& vNotify) = 0;
    virtual int NotifyInsert(int ChaDbNum, gamelogic::notify::NOTIFY_TYPE Type, sc::MGUID& Guid, const std::string& Data) = 0;
    virtual int NotifyDelete(sc::MGUID& Guid) = 0;

    virtual int AttendanceLogInsert(int ChaNum, int Type, const std::string& LoadType, int DayN, int Status) = 0;
    virtual int AttendanceTaskInsert(
        int ChaNum, LONGLONG ChaCurExp, int TaskN, int Type, int Status, int Value,
        int ValueBase, int RewardPoint, int RewardExp, int RewardMoney, int RewardItems,
        int RewardItem1, int RewardItem2, int RewardItem3, int TookRewards) =0;

    //////////////////////////////////////////////////////////////////////////
    // 인던로그 InstantDungeon Log
 
    // 로그 타입을 정의
    virtual int InstantDungeonLogTypeInsert( 
        const db::EM_INSTANT_LOG_TYPE& nLogType,
        const char* strParamName1=0,  const char* strParamName2=0,  const char* strParamName3=0,  const char* strParamName4=0,  const char* strParamName5=0,
        const char* strParamName6=0,  const char* strParamName7=0,  const char* strParamName8=0,  const char* strParamName9=0,  const char* strParamName10=0,
        const char* strParamName11=0, const char* strParamName12=0, const char* strParamName13=0, const char* strParamName14=0, const char* strParamName15=0,
        const char* strParamName16=0, const char* strParamName17=0, const char* strParamName18=0, const char* strParamName19=0, const char* strParamName20=0 ) = 0;

    // 실제 로그 입력
    virtual int InstantDungeonLogInsert( 
        const db::EM_INSTANT_LOG_TYPE& nLogType,
        const char* strParam1=0,  const char* strParam2=0,  const char* strParam3=0,  const char* strParam4=0,  const char* strParam5=0,
        const char* strParam6=0,  const char* strParam7=0,  const char* strParam8=0,  const char* strParam9=0,  const char* strParam10=0,
        const char* strParam11=0, const char* strParam12=0, const char* strParam13=0, const char* strParam14=0, const char* strParam15=0,
        const char* strParam16=0, const char* strParam17=0, const char* strParam18=0, const char* strParam19=0, const char* strParam20=0 ) = 0;


    /////////////////////////////////////////////////////////////////////////
    // 전장 점수 CompetitionRating

    // 전장 점수 얻어옴;
    virtual int CompetitionRatingGetInfo(
        const std::string& strContentType,
        const DWORD charDbNum,
        int& Rating,
        int& WinningStreak,
        int& PlayCount,
        __time64_t& LastPlayDate
        ) = 0 ;

    // 전장 점수 저장;
    virtual int CompetitionRatingUpdate(
        const std::string& strContentType,
        const DWORD charDbNum,
        const int Rating,
        const int WinningStreak,
        const int PlayCount,
        const __time64_t tLastPlayDate
        ) = 0;

    // 전장 시즌 업데이트;
    virtual int CompetitionSeasonUpdate(
        const std::string& strContentType,
        const int seasonNum,
        const int baseRating
        ) = 0;

    // 전장 로그 CompetitionLog

    ////////////////////////////////////////////////////////////////////////
	// 인증 서버 로그 기록
	virtual int AddAuthServerState( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, int nCurUser, int nMaxUser, const __time64_t& LogDate ) = 0;
	// 인증 절차 수행
	virtual int ProcessCertificationForAuth( int isSessionSvr, int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::string strUniqKey, int &certification, std::string &strNewUniqKey, int &nSessionSvrID ) = 0;
	// 이벤트 정보 기록
	virtual int AddAuthEventInfo( G_AUTH_EVENT_INFO *eventInfo ) = 0;

	virtual int AddAuthServerConnectionState( int nSessionSvrID, int nCounrty, int nServerType, const TCHAR* szIP, int nPort ) = 0;
	virtual int AddAuthServerConnectionStateInit() = 0;
	virtual int AuthCountryAccessApproveSelect( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::vector<int>& vecIndex OUT ) = 0;

	virtual int TexasHoldemChipMapSelect( std::vector< TexasHoldem::sChipPrice >& vecChip ) = 0;
	virtual int TexasHoldemChipMapVersionSelect( LONGLONG& Version ) = 0;
	virtual int	TexasHoldemChipSelect( SCHARDATA2* pChaData2 ) = 0;
	virtual int	TexasHoldemChipUpdate( DWORD dwChaDbNum, LONGLONG& chip, bool bPlus = false ) = 0;
	virtual int	TexasHoldemChipLogInsert( DWORD dwChaDbNum, LONGLONG& chip, LONGLONG& GameMoney, LONGLONG& Point ) = 0;
	virtual int	TexasHoldemChipSelectFromWeb( int nServerGroup, DWORD dwChaDbNum, LONGLONG& chip ) = 0;
	virtual int	TexasHoldemAuthKeyInsert( DWORD dwChaDbNum, const std::string& strGUID ) = 0;
	virtual int	TexasHoldemGameTimeInsert( DWORD dwChaDbNum, DWORD dwUserNum, int nSecond ) = 0;

    //! ITEM ID를 통해 아이템을 가지고 온다. ( bDateCheck => 날짜(Date)검사 여부 )
    virtual int ConsignmentsSaleGet( sc::MGUID& Guid, private_market::ConsignmentSaleItem& SaleItem, bool bDateCheck = false  ) = 0;
    //! ChaNum을 통해 등록한 물품을 가지고 온다. 
    virtual int ConsignmentsSaleSelect( private_market::SALE_ITEMS_VECTOR& vecItem, const DWORD& dwChaNum = 0, const int& ServerGroup = 0 ) = 0;
    //! 남은 갯수를 바꾼다.
    virtual int ConsignmentsUpdateTurnNum( sc::MGUID& Guid, int nTurnNum ) = 0;
    virtual int ConsignmentsSaleInsert( private_market::ConsignmentSaleItem& SaleItem ) = 0;
    virtual int ConsignmentsSaleDelete( sc::MGUID& Guid ) = 0;
    virtual int	ConfignmentSaleSlotExpansionDateSelect( SCHARDATA2* pChaData2 ) = 0;
    virtual int	ConfignmentSaleSlotExpansionDateSet( DWORD dwChaDbNum, __time64_t tExpansionDate ) = 0;
	virtual int ConsignmentSaleLogInsert( 
		sc::MGUID&	Guid,
		DWORD		dwChaDbNum,
		WORD		wType,
		WORD		wState,
		LONGLONG	llPrice,
		LONGLONG	llFee,
		WORD		wItemMid,
		WORD		wItemSid,
		LONGLONG    llItemMakeNum,
		int			nItemAmount,
		WORD		TradeCount,
		WORD		wRemainingTime ) = 0;
	virtual int MacroLogInsert(DWORD dwChaDbNum, WORD  wState) = 0;

	virtual int ItemPeriodExLogInsert(DWORD  chaNum
		, WORD itemMid
		, WORD itemSid	
		, WORD remainNum
		, __time64_t previousPeriod
		, __time64_t currentPeriod) = 0;


	virtual int	UserSystemInfoInsert( 
		DWORD dwUserNum, 
		DWORD dwCpuCoreNumber, 
		DWORD dwCpuClock, 
		DWORDLONG dwMemorySize,
		DWORD dwResolutionX,
		DWORD dwResolutionY,
		DWORD dwGpuVendor,
		DWORD dwGpuModel,
		DWORD dwShaderModel,
		int languageCode ) = 0;

	virtual int	GetChaResetInfo( DWORD dwChaDbNum ) = 0;

	// 필리핀 유저이전
	virtual int	IsMigrationTarget( DWORD dwChaDbNum ) = 0;
	virtual int GetCharacterInfoForMigration( int nUserNumber, int nChaNum, SCHARDATA2* pChaData2 ) = 0;
	virtual int SetMigrationTarget( DWORD dwChaDbNum ) = 0;

	virtual int TransDBDataToWorldBattle(SCHARDATA2* pCharData2, DWORD dwChaNum) = 0;
	virtual int CalculateExpCompressor( DWORD dwChaDbNum, __int64 nCurExp, __int64 nCompressedExp ) = 0;

public:
	//////////////////////////////////////////////////////////////////////////
	//
	// TOURNAMENT BETTING SYSTEM
	//
	virtual int TournamentBettingSelect( DWORD ClientDBid, DWORD BettingStatus ) = 0;
	virtual int TournamentBettingInsert( DWORD ChaNum,LONGLONG BettingCount, DWORD BettingGameSciptid,DWORD BettingGroup) = 0;
	virtual int TournamentBettingUpdate( DWORD ChaNum,DWORD BettingGameSciptid,DWORD BettingStatus) = 0;
	virtual int TournamentBettingDelete( DWORD ChaNum) = 0;

	//////////////////////////////////////////////////////////////////////////
	//
	// MATCHING SYSTEM
	//

	virtual int MatchingOnload( std::string Matching_Content_Type,
		std::string Betting_Script,
		std::string Room_Admin_Script,
		std::string Matching_Script,
		std::string Lobby_Field_Manager ) = 0;
	virtual int MatchingGroup( DWORD GroupID,
		DWORD GameScriptID,
		bool  IsDeleted) = 0;
	virtual int MatchingGroupUser( DWORD GroupID,
		DWORD GameScriptID,
		DWORD UserDBNum,
		bool  IsDeleted) = 0;
	virtual int MatchingQueueIn( DWORD ClientID,
		DWORD GameScriptID,
		bool  IsDeleted) = 0;
	virtual int MatchingInstanceIn( DWORD GroupID,
		DWORD GameScriptID,
		DWORD  MatchingScriptID) = 0;
	virtual int MatchingInstanceJoin( DWORD GroupID,
		DWORD MapID,
		DWORD GameScriptID,
		DWORD  MatchingScriptID) = 0;
	virtual int MatchingInstanceOnStart( DWORD GroupReadySize,
		DWORD GameScriptID,
		DWORD MatchingScriptID) = 0;
	virtual int MatchingInstanceOut( DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID) = 0;
	virtual int MatchingInstanceResualt( DWORD Faction,
		bool IsLose,
		DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID) = 0;
	virtual int MatchingRoomDestroy( DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID) = 0;
	virtual int MatchingRoomReq( DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID) = 0;
	virtual int MatchingFinalWinner( DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID) = 0;
	virtual int MatchingInstanceTournament( int   tournamentPosition,
		std::string GroupName,
		int   GroupIndexPosition,
		int   GroupSize,
		int   GroupMaxSize,
		int   GroupHeadLevel,
		DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID) = 0;

	virtual int MatchingJoinCountLoad ( MatchingSystem::MAP_CHAR_JOIN_COUNT_LIST& _refMapCharJoinCountList ) = 0;
	virtual int MatchingJoinCountReset ( ) = 0;
	virtual int MatchingJoinCountUpdate ( const DWORD _dwCharDBNum, const MatchingSystem::SJOIN_COUNT _sJoinCount ) = 0;

    // GM LOG ( GM 아이템 넣어주기 )
    virtual int WriteGMLog(int nGmNum, std::string& strCmd, std::string& strUserIP) = 0;

	// 란모바일
	virtual int RanMobileRequest(DWORD dwUserDBNum, DWORD dwChaDBNum, int nServerNum, int nCommand, int nValue, const std::string& strChaName, int nChaClass ) = 0;

	// Log Money;
	virtual int MoneyLogInsert ( const int _nMainType, const int _nSubType, const LONGLONG _llMoney ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// 기간제 아이템 시간 연장
	virtual int InspectionRewardPolicySelect() = 0;
	virtual int InspectionRewardPolicyInsert( const __time64_t tStart, const __time64_t tEnd, const int nRewordMinute ) = 0;
	virtual int InspectionRewardGetMinute( SCHARDATA2* pChaData2 ) = 0;

	virtual int MoneyNotifyInsert( DWORD dwChaDbNum, __time64_t tDate, LONGLONG llOldMoney, LONGLONG llNewMoney, int nCause, int nChannel, int nField ) = 0;

	// PostRenew
	// PostRenew
	virtual int PostRnSelectAll( std::vector<PostInfo>& vecList OUT, DWORD dwRecvChaNum )= 0;
	virtual int PostRnSetRead( DWORD dwPostID )		= 0;
	virtual int PostRnDelAttach( DWORD dwPostID )	= 0;
	virtual int PostRnDel( DWORD dwPostID )			= 0;
	virtual int PostRnSend(
		DWORD dwSendChaDBNum, 
		std::string strSendChaName, 
		DWORD dwSendUserNum, 
		DWORD dwRecvChaDbNum, 
		int nStyle, 
		int wMailType,
		int wAttachType,
		int wMoneyType,
		LONGLONG llMoney, 
		SITEMCUSTOM sItemCustom,
		std::string strTitle, 
		std::string strContent ) = 0;
	virtual int PostRnReturn( DWORD dwPostID, DWORD dwReturnChaNum, std::string strReturnChaName, DWORD dwReturnUserNum, DWORD dwRecvChaNum  )	= 0;
	virtual int PostCharacterCheck(DWORD dwRecvChaDbNum) = 0;
	virtual int GetinfofromCharDb(const char *szCharName, int *pnChaNum, int *pnUserNum, int *pClass, int *Lv) = 0;

	virtual int GetCharacterItemTransferTarget( int UserNum, std::vector< DWORD >& vecChaNum ) = 0;

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.17, Randombox 확률제어 항목 완료로그 저장
     * modified : 
     */
    virtual int LogRandomboxChanceComplete(const SNATIVEID& BoxId,  /* RandomBox ID */
                                           const SNATIVEID& ItemId, /* 당첨 Item ID */
                                           INT32 ReqCount,          /* 목표 오픈 수량 */
                                           DWORD ChaNum,            /* 목표 캐릭터 번호 */
                                           DWORD GMNum,             /* 등록 GM Number */
                                           INT32 OpenCount,         /* 실제 오픈 수량 */
                                           DWORD Winner             /* 당첨자 */
                                          ) = 0;

	// item
	virtual int ItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) = 0;
	virtual int DeletedItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) = 0;
	virtual int ItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) = 0;
	virtual int EntireItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) = 0;
	virtual int ItemDelete( sc::MGUID& Guid, int State, int CallType ) = 0;
	virtual int CompletelyItemDelete( sc::MGUID& Guid, int State, int CallType ) = 0;
	virtual int ItemSaleToNpc( sc::MGUID& Guid, int State, int CallType ) = 0;
	virtual int ItemSelect( DWORD ChaDbNum, INVEN_TYPE InvenType, std::vector< SINVENITEM_SAVE >& vecItems ) = 0;
	virtual int ItemSelect( DWORD ChaDbNum, INVEN_TYPE InvenType, std::multimap< DWORD, SINVENITEM_SAVE >& mapItems ) = 0;
	virtual int GetPutonItems( DWORD ChaDbNum, SCHARDATA2* pChaData2 ) = 0;
	virtual int GetInvenItems( DWORD ChaDbNum, SCHARDATA2* pChaData2 ) = 0;
	virtual int GetLockerItems( DWORD UserNum, SCHARDATA2* pChaData2 ) = 0;
	virtual int SavePetInven( int nChaNum, int nPetNum, std::vector< SINVENITEM_SAVE >& vecItems ) = 0;
	virtual int SaveVehicleInven( int nVehicle, int nCharNum, std::vector< SINVENITEM_SAVE >& vecItems ) = 0;
	virtual int GetClubLockerItems( DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems ) = 0;
	virtual int SaveClubLockerItems( DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems ) = 0;

	virtual int TransferItems( DWORD UserNum, std::vector< DWORD >& vecChaNum ) = 0;
	virtual int TransferClubLockerItems( DWORD ClubNum = 0 ) = 0;
	virtual int GetGuildItemTransferStatus( DWORD ClubNum, int& Result, int& MigrationState ) = 0;
	virtual int TransferLockerItems( DWORD UserNum = 0 ) = 0;
	virtual int GetChaNumToTransfer( std::vector< DWORD >& vecChaNum ) = 0;
	virtual int TransferCharacterItems( DWORD ChaDbNum = 0 ) = 0;

	virtual int ItemUpdatePos( DWORD ChaDbNum, INVEN_TYPE InvenType, sc::MGUID& Guid, WORD PosX, WORD PosY, int CallType ) = 0;
	virtual int ItemUpdateTurnNum( sc::MGUID& Guid, WORD TurnNum, int CallType ) = 0;
	virtual int ItemInsertError( sc::MGUID& Guid, int CallType, __time64_t& Date ) = 0;

	virtual int GetItemTransferStatus( DWORD UserNum, int& Result ) = 0;
};

} // namespace db
#endif // _LS_DB_MANAGER_H_
