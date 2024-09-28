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
    EMSERVICE_PROVIDER m_ServiceProvider; //! ���� ��������
    EM_DB_CONNECTION_TYPE m_ConnectionType; //! ���� ���

public:
    EM_DB_CONNECTION_TYPE GetConnectionType() const { return m_ConnectionType; }

	//
	//mjeon.ado
	//pSubDbMan: �� ���������� �ϳ�, ���� ODBC�� ó���ο� ������ ���� �����鼭 ADO �ڵ带 ���� �� �ֵ��� �ϱ� �����̴�.
	// ��, �ϳ��� DbAction���� ������ ODBCó���� ���Ҿ� �߰������� �����ϴ� ������ ADO�� ����ϰ� ���� ��쿡 �̸� ����ϸ� �ȴ�.
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
    virtual bool IsAlive() = 0; //! ������ ����� �Ǵ��� üũ
    virtual bool Reconnect() = 0; //! ������ üũ
    virtual bool CheckHdd() = 0; //! Hard disk �뷮 üũ
    virtual bool IsAllDbJobCompleted() = 0; //! ��� DB �۾��� �����°�?
    //! ��� DB �۾��� ������ ������ ����Ѵ�.
    //! WaitLimitTime : Milliseconds
    virtual void WaitAllDbJobCompleted(DWORD WaitLimitTime);

public:
    //! -----------------------------------------------------------------------
    //! ȣȯ�� ������ ���ؼ� �ӽ÷�...
    virtual int	OdbcOpenGameDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenUserDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenBoardDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenLogDB  (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int OdbcOpenShopDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenTerraDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenGspDB  (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenThaiDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    virtual int	OdbcOpenKorDB  (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) = 0;
    //! �����̽þ� PC�� �̺�Ʈ
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

    // ����
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


	//! ������ �α�
	virtual int LogGuidance(const LOG_GUIDANCE &sLog) = 0;

	//! CDM �α�
	virtual int LogCDM(const LOG_CDM &sLog) = 0;

	//! CDM �α�(����) 
	virtual int LogCDMReuslt(const LOG_CDM_RESULT &sLog) = 0;
	
	//! Shop ���� ������ �������� �����´�.
	//! \param strUID ����ID
	//! \param &v ��ǰ�� ���� ����
	//! \return 
    virtual int GetPurchaseItem(const CString& strUID, std::vector<SHOPPURCHASE> &v) = 0;
	
	//! ������ ������ �� �ִ� ��ǰ���� Ȯ���Ѵ�.
	//! \param strPurKey �����ڵ�
	//! \param nFlag(0 : ����, 1 : ���ſϷ�, 2 : ������ҽ�û, 3 : �������ó��)
	//! \return 1:�������� ���� �̿��ǰ�:�������� ����
    virtual int SetPurchaseItem(const CString& strPurKey, int nFlag=1) = 0;
	
	//! ĳ���� ����, ĳ���ͻ��� ���� ����
	virtual int UserChaNumIncrease(int nUserNum) = 0;

	//! �ش� ��Ŀ ����ð��� ���Ѵ�.
	//! \n 1 �� 5 �� ��Ŀ�� ���� �� ����.
	//! \n 2, 3, 4 �� ��Ŀ�� ����ð��� ���� �� �ִ�.
	//! \param nUserNum ����ڹ�ȣ
	//! \param nStorageNum ���丮����ȣ
	//! \param tTime ����ð�
	//! \return -1:sc::db::DB_ERROR  0:sc::db::DB_OK
    virtual int SetChaStorageDate(int nUserNum, int nStorageNum, __time64_t tTime) = 0;
    	
	//! �ش� ĳ������ �κ��丮 �߰� �ټ��� �����Ѵ�.
	//! \n ���� 0 ���� ���� �߰��� 1 �� �Է�
    //! \n ���� ���� ���¿��� �� ���� �߰��� 2 �� �Է�(���� �ټ��� �Է��Ѵ�)
    //! \n ���� ���� ���¿��� ���� �߰��� 3 �� �Է�
    //! \n �ִ� 3������ �Է°���.
	//! \param nChaNum ĳ���͹�ȣ
	//! \param wInvenLine �߰��ټ�
    virtual int SetChaInvenNum(int nChaNum, WORD wInvenLine) = 0;
    	
	//! �ش� ������� �����̾� �Ⱓ�� �����Ѵ�.
    virtual int SetPremiumTime(int nUserNum, __time64_t tPremiumTime) = 0;
    	    
	//! ������ȯ ī�� ����� ���� �������� �����Ѵ�.
    virtual int SetLastCallPos(int nChaNum, DWORD dwMapID, D3DXVECTOR3 vPos) = 0;    
	
	//! ���ο� ĳ���͸� �����Ѵ�.
    virtual int	CreateNewCharacter(SCHARDATA2* pCharData2) = 0;
	virtual int	CreateNewCharacterExtraWithAdo(SCHARDATA2* pCharData2) = 0;	//mjeon.ado

    //! ChaInfo Backup.
    virtual int CharacterInfoBackup(SCHARDATA2* pCharData2) = 0;
	
    
    //! ���ο� Ŭ���� �����Ѵ�.
    //! �Է�
    //!     strClubName : Ŭ���̸�
    //!     dwChaNum : Ŭ���� �����Ϸ��� ĳ���� ��ȣ(�� ĳ���Ͱ� Ŭ���� �����Ͱ� �ȴ�)
    //! ���
    //!     -1 : �̹� ��帶���ͷ� ��ϵǾ� ���� �����Ұ�.
    //!     -2 : �������� �����߻�(�ߺ��ȱ���̸�)
    //!      0 ���� Ŭ�� : ������ ����ȣ��
    virtual int CreateClub(const std::string& ClubName, DWORD ChaDbNum, wb::EM_COUNTRY Country) = 0;    
    
    //! Ŭ���� �����Ѵ�.
    //! dwClub : Ŭ����ȣ
    //! dwChaNum : Ŭ���� �����Ϸ��� ĳ���͹�ȣ(ĳ���� ��ȣ�� Ŭ�� �����Ͱ� �ƴϸ� ������ ���� �ʴ´�)
    virtual int DeleteClub(DWORD dwClub, DWORD dwChaNum) = 0;    
    
    //! Ŭ����ũ�� �����Ѵ�.
    //! dwClub : Ŭ����ȣ
    //! dwRank : ��ŷ
    virtual int SetClubRank(DWORD dwClub, DWORD dwRank) = 0;

    virtual int ReadClubMarkImage(DWORD dwClub, se::ByteStream &ByteStream) = 0;

    virtual int WriteClubMarkImage(DWORD dwClub, DWORD dwMarkVer, BYTE* pData, int nSize) = 0;

	//! Ŭ�� ��ü�ð��� �����Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param tDiss ��ü�ð�
    virtual int SetClubDissolutionTime(DWORD dwClub, __time64_t tDiss) = 0;
	
	//! Ŭ���� ����Ż�� Ȥ�� ����ð��� ���Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param tSec �ð�
	virtual int SetClubAllianceSec(DWORD dwClub, __time64_t tSec) = 0;
	
	//! Ŭ���� �����ػ� �ð��� ���Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param tDis �ػ�ð�
	virtual int SetClubAllianceDis(DWORD dwClub, __time64_t tDis) = 0;
	
	//! Ŭ���� ������ ���� �ð��� ���Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param tDis ������ ���ӽð�
	virtual int SetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority) = 0;
	
    //! ������ ��� Ŭ�������� �����´�
    //! \param vClub : Ŭ������
    virtual int GetClubInfo(std::vector<std::tr1::shared_ptr<GLClub> > &vClub) = 0;
    
    //! �ش� Ŭ���� ����� �����´�
    //! \param dwGuild : Ŭ����ȣ
    //! \param vMember : Ŭ�����
    virtual int GetClubMember(DWORD dwClub, std::vector<GLCLUBMEMBER>& vMember) = 0;

    //! �ش� Ŭ���� Ŭ�� ���� ������ �����´�.
    virtual int GetClubAuth(DWORD ClubDbNum, std::vector<club::AUTH_GRADE_DB>& vAuth) = 0;
    //! �ش� Ŭ���� ��� ĳ������ Ŭ�� ������ 0 ���� �����.
    virtual int ResetClubPosition(DWORD ClubDbNum) = 0;
        
    //! ���ο� ĳ���͸� Ŭ���� ���Խ�Ų��
    //! \param dwClub : Ŭ����ȣ
    //! \param dwChaNum : ���Խ�Ű���� ĳ���͹�ȣ
    virtual int AddClubMember(DWORD dwClub, DWORD dwChaNum) = 0;
        
    //! ���ԵǾ� �ִ� ĳ���͸� Ŭ������ Ż���Ų��
    //! \param dwChaNum : ĳ���͹�ȣ
    virtual int DeleteClubMember(DWORD dwChaNum) = 0;

	virtual int DeleteClubMemberWithNoPenalty( DWORD dwChaNum ) = 0;
	
	//! Ŭ�� ��Ʋ�� ��û�Ѵ�.
	//! \param dwClubP ��Ŭ����ȣ(Ŭ�� ��Ʋ�� ��û�� ���� �� )
	//! \param dwClubS ���� Ŭ����ȣ(Ŭ�� ��Ʋ�� ��û�� �� Ŭ�� )	
	virtual int SetClubBattle(DWORD dwClubP, DWORD dwClub, DWORD dwEndTime, int nAlliance) = 0;
	
	//! Ŭ�� ��Ʋ�� �����Ѵ�.
	//! \param dwClubP ��Ŭ�� ��ȣ(Ŭ�� ��Ʋ�� ��û�� ���� �� )
	//! \param dwClubS ���� Ŭ�� ��ȣ(Ŭ�� ��Ʋ�� ��û�� �� Ŭ�� )
	//! \param nGuFlag ���п���
	//! \param nGuKillNum Ŭ�� ��Ʋ ų��
	virtual int EndClubBattle(DWORD dwClubP, DWORD dwClubS, int nGuFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance) = 0;

	
    //! Ŭ�� ��Ʋ������ �����Ѵ�,(Ŭ�� ��Ʋ�� ����Ǳ��� ����������� �ʿ��Ѱ�� ��Ʋ ������ ������ �ʿ��� )
    //! \param dwClubP ��Ŭ�� ��ȣ(Ŭ�� ��Ʋ�� ��û�� ���� ��)
    //! \param dwClubS ���� Ŭ�� ��ȣ(Ŭ�� ��Ʋ�� ��û�� �� Ŭ�� )
    //! \param nGuKillNum Ŭ�� ��Ʋ ų��
    //! \return
	virtual int SaveClubBattle(DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum) = 0;
	
    //! ���͹�Ʋ ������ �ʱ�ȭ ��Ű���.
    //! \param dwClub Ŭ�� ��ȣ
	virtual int ReSetAllianceBattle(DWORD dwClub) = 0;

    //! �ش� Ŭ���� ��Ʋ���� Ŭ�� ������ �����´�.
    //! \param dwClub Ŭ����ȣ
    //! \param &vMember Ŭ����Ʋ ����
    virtual int GetClubBattleInfo(DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo) = 0;
    
	virtual int UpdateClubName( DWORD dwClub, const std::string& ClubName ) = 0;

    //! �ش� ĳ������ ����ġ�� �����Ѵ�.
    //! ĳ���͹�ȣ�� 0 ���� Ŀ���Ѵ�.
    //! ����ġ�� 0 ���� Ŀ���Ѵ�.
    //! \param  nChaNum : ĳ���͹�ȣ
    //! \param   llExp : ����ġ
    virtual int SetChaExp(int nChaNum, LONGLONG llExp) = 0;
    
    //! �ش� ĳ������ ����ġ�� �����´�.
    //! ĳ���͹�ȣ�� 0 ���� Ŀ���Ѵ�.
    //! ����ġ�� 0 ���� ������ �����̴�.
    //! \param nChaNum ĳ���͹�ȣ
    //!  LONGLONG : ����ġ
    virtual LONGLONG GetChaExp(int nChaNum) = 0;
        
    //! ���ӳ����� �����̳� �Ű�� �Ű��� ������ DB�� ����Ѵ�.    
    //! \param nSGNum �����׷�
    //! \param ChaNum �Ű��� ĳ���͹�ȣ
    //! \param strMsg �Ű���(�ִ�ũ��� CHAT_MSG_SIZE)
    virtual int LogAppeal(int nSGNum, int nChaNum, CString strMsg) = 0;
        
    //! ���� ��������� ���� Ŭ�������͸� �����´�.
    virtual int GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB) = 0;
        
    //! ������ �������� ������ Ŭ�� ������ ���Ѵ�.
    //! \param dwRegionID : ���� ID
    //! \param dwClub : Ŭ�� ID
    //! \param fTax : ����
    virtual int SetClubRegion(DWORD dwRegionID, DWORD dwClub=0, float fTax=0) = 0;
        
    //! �ش� ������ ���� ���¸� �����Ѵ�.
    //! \param dwRegionID ���� ID
    //! \param dwClub Ŭ�� ID(�⺻ 0 �̴�)
    virtual int DelClubRegion(DWORD dwRegionID, DWORD dwClub=0) = 0;
		
    //! �ش�Ŭ���� �����ݾ��� �����´�.(Storage)
    //! \param dwClub Ŭ����ȣ
    //! \param ���ϰ��� >= 0, 0 ���� ������ ERROR
    virtual LONGLONG GetClubMoney(DWORD dwClub) = 0;
	
    //! �ش�Ŭ���� �����ݾ��� �����Ѵ�.(Storage)
    //! \param dwClub : Ŭ����ȣ
    //! \param llMoney : �����ݾ�(�ݾ��� >= 0)
	virtual int	SetClubMoney(DWORD dwClub, LONGLONG llMoney) = 0;
	
    //! �ش�Ŭ���� ���Աݾ��� �����Ѵ�.(Income money)
    //! \param dwClub : Ŭ����ȣ
    //! \param llMoney : ���Աݾ�(�ݾ��� >= 0)
	virtual int	SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney) = 0;    
    
    //! �ش�Ŭ���� ���Աݾ��� �����´�.(Income money)
    //! \param dwClub : Ŭ����ȣ
    //! \return ���ϰ��� >= 0, 0 ���� ������ ERROR
	virtual LONGLONG GetClubIncomeMoney(DWORD dwClub) = 0;
        
    //! �ش�Ŭ���� Ŭ��â�� �о�´�.
    //! \param dwClub : Ŭ����ȣ
    //! \param ByteStream : ����
	virtual int ReadClubStorage(DWORD dwClub, se::ByteStream &ByteStream) = 0;    
	virtual int GetGuildMigrationState( DWORD dwClub ) = 0;
    
    //! �ش�Ŭ���� Ŭ��â�� �����Ѵ�.
    //! \param dwClub : Ŭ����ȣ
    //! \param pData : â����ġ
    //! \param nSize : ���� ��üũ��
	virtual int	WriteClubStorage(DWORD dwClub, BYTE* pData, int nSize) = 0;	
	
    //! �ҹ����α׷� ��볻���� ����Ѵ�.
    //! \param nSGNum �����׷�
    //! \param nSvrNum ������ȣ
    //! \param nUserNum ������ȣ
    //! \param nChaNum ĳ���͹�ȣ
    //! \param nHackProgramNum ��ŷ���α׷���ȣ
    //! \param szComment �޸�Ʈ
	virtual int LogHackProgram(
		int nSGNum, 
		int nSvrNum, 
		int nUserNum, 
		int nChaNum, 
		int nHackProgramNum,
		const TCHAR* szComment) = 0;
		
    //! �� Ŭ������ �����Ѵ�
    //! \param dwClub : Ŭ����ȣ
    //! \param dwChaNum : ĳ���͹�ȣ
    //! \param dwSubMasterFlags : �÷���(0 ���� ������ �Ϲݱ����� �ȴ�)
	virtual int	SetClubMasterFlags(
		DWORD dwClub,
		DWORD dwChaNum,
		DWORD dwSubMasterFlags) = 0;
	
    //! ���� Ŭ���� �Ἲ�Ѵ�.
    //! \ db �� Ŭ���� ���Ἲ�� üũ���� �ʴ´�.
    //! \ A �� B �� ������ ���¿��� 
    //! \ B �� C �� ������ ����
    //! \ C �� A �� ������ ���� ������ ü�� ������ �߻��Ѵ�.	
    //! \param dwClubP : �� Ŭ����ȣ(���� Ŭ���� ��)
    //! \param dwClubS : ���� Ŭ����ȣ(�� Ŭ�� ������ ���� ���� Ŭ��)
	virtual int	SetClubAlliance(DWORD dwClubP, DWORD dwClubS) = 0;
	
    //! ���� Ŭ���� ��ü�Ѵ�.
    //! \param dwClubP : �� Ŭ����ȣ(���� Ŭ���� ��)
    //! \param dwClubS : ���� Ŭ����ȣ(�� Ŭ�� ������ ���� ���� Ŭ��)
	virtual int	DelClubAlliance(DWORD dwClubP, DWORD dwClubS) = 0;

    //! ���� Ŭ�� ����Ʈ�� �����´�.
    //! <��Ŭ��,����Ŭ��>
	virtual int	GetClubAlliance(std::vector< std::pair<DWORD,DWORD> > &vecAlliance) = 0;
	
    //! �ش� ������� ä�� ��Ͻð��� �����´�
	virtual __time64_t  GetChatBlockTime(int nUserNum) = 0;
	
    //! �ش� ������� ä�� ��Ͻð��� �����Ѵ�
	virtual int SetChatBlockTime(int nUserNum, __time64_t tBlockTime) = 0;
		
    //! Ŭ���� ���������� �����Ѵ�.
    virtual int SetClubNotice(const std::string& ChaName, DWORD dwClub, const std::string& ClubNotice) = 0;
	
    //! Ŭ���� ����Ŭ�� ���� �븮�ڸ� �����Ѵ�.
	virtual int SetClubDeputy(DWORD dwClub, DWORD dwDeputy) = 0;
	
    //! Ŭ���� ������ ������ �����Ѵ�.
	virtual int SetClubAuthority(DWORD dwClub, DWORD dwMasterID) = 0;

    virtual int InsertClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) = 0;
    virtual int UpdateClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) = 0;
    virtual int ChangeClubAuthIndex(DWORD ClubDbNum, size_t AuthIndexA, size_t AuthIndexB) = 0;
    virtual int ChangeClubMemberGrade(DWORD ChaDbNum, int Grade) = 0;
	
    //! ĳ������ �̸��� �����Ѵ�.
    //! \param dwChaNum ĳ���͹�ȣ
    //! \param szCharName ������ ĳ�����̸�    
	virtual int RenameCharacter(DWORD dwChaNum, char* szCharName) = 0;
	
    //! ĳ������ �Ӹ����� �����Ѵ�.    
	virtual int SetChaHairColor(DWORD dwChaNum, int nHairColor) = 0;
	
    //! ĳ������ ��Ÿ���� �����Ѵ�.    
	virtual int SetChaHairStyle(DWORD dwChaNum, int nHairStyle) = 0;
	
    //! ĳ������ �󱼽�Ÿ���� �����Ѵ�.    
	virtual int SetChaFaceStyle(DWORD dwChaNum, int nFaceStyle) = 0;
	
    //! ĳ������ ������ �����Ѵ�.    
	virtual int SetChaGenderChange(DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor) = 0;

	// ĳ���� �п��� �����Ѵ�
	virtual int SetChaSchoolChange(DWORD dwChaNum, int nSchool) = 0;
	
    //! ĳ������ ��ġ������ �����Ѵ�.
    //! \param dwChaNum ĳ���� ��ȣ
    //! \param dwStartmapID �����ϴ� �� ��ȣ
    //! \param dwStartGate �����ϴ� ����Ʈ ��ȣ
    //! \param fStartPosX �����ϴ� X ��ǥ
    //! \param fStartPosY �����ϴ� Y ��ǥ
    //! \param fStartPosZ �����ϴ� Z ��ǥ
    //! \param dwSaveMapID ���������� �÷����� �� ��ȣ
    //! \param fSavePosX ���������� �÷����� X ��ǥ
    //! \param fSavePosY ���������� �÷����� Y ��ǥ
    //! \param fSavePosZ ���������� �÷����� Z ��ǥ
    //! \param dwLastCallMapID ���� ��ȯī�忡 ����� �� ��ȣ
    //! \param fLastCallPosX ���� ��ȯī�忡 ����� X ��ǥ
    //! \param fLastCallPosY ���� ��ȯī�忡 ����� Y ��ǥ
    //! \param fLastCallPosZ ���� ��ȯī�忡 ����� Z ��ǥ
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
	
    //! ���ο� ���� �����Ѵ�.
    //! \param nChaNum  �� ������ ĳ���� ��ȣ
    //! \param szPetName ���̸�
    //! \param nPetType ���� Ÿ��
    //! \param nPetMID �� MID
    //! \param nPetSid �� SID
    //! \param nPetStyle ���� ��Ÿ��
    //! \param nPetColor ���� �÷�
    //! \param m_nPetCardMID �� ī���� MID
    //! \param m_nPetCardSID �� ī���� SID
    //! \return sc::db::DB_ERROR �Ǵ� ������������ Pet �� ������ȣ
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

	
    //! ���� �̸��� �����Ѵ�.
    //! \param nChaNum ĳ���� ��ȣ
    //! \param nPetNum ���� ������ȣ
    //! \param szPetName ���̸�
    //! \return sc::db::DB_ERROR, sc::db::DB_OK
	virtual int RenamePet(int nChaNum, int nPetNum, const TCHAR* szPetName) = 0;

    //! ���� �������� �����Ѵ�.
    //! \param nChaNum ���� ���ο� ������
    //! \param nPetNum �������� ������ �� ��ȣ
	virtual int ExchangePet(int nChaNum, int nPetNum) = 0;

    //! ���� �÷��� �����Ѵ�.
    //! \param nChaNum ĳ���� ��ȣ
    //! \param nPetNum �÷��� �����Ϸ��� ���� ��ȣ
    //! \param nPetColor ���� �÷�
	virtual int SetPetColor(int nChaNum, int nPetNum, int nPetColor) = 0;

    //! ���� ��Ÿ���� �����Ѵ�.
    //! \param nPetNum ��Ÿ���� �����Ϸ��� ���� ��ȣ
    //! \param nPetStyle ���� ��Ÿ��
	virtual int SetPetStyle(int nChaNum, int nPetNum, int nPetStyle) = 0;
	
    //! ���� �������� �����Ѵ�. �������� 0-100 ����
    //! \param nPetNum �������� ������ ���� ��ȣ
    //! \param fPetFull ������
	virtual int SetPetFull(int nChaNum, int nPetNum, int nPetFull) = 0;
	
    //! ���� �κ��丮 ������Ʈ
	virtual int SetPetInven(int nChaNum, int nPetNum, se::ByteStream &ByteStream) = 0;
	
	virtual int SetPetMigrationState( int ChaNum, int PetNum, int MigrationState ) = 0;
	virtual int SetVehicleMigrationState( int ChaNum, int VehicleNum, int MigrationState ) = 0;

    //! ���� ��ų ������Ʈ
	virtual int SetPetSkill(
		int nChaNum,
		int nPetNum,
		int nPetInvenType,
		int nPetInvenMID,
		int nPetInvenSID,
		int nPetInvenCMID,
		int nPetInvenCSID,
		int nPetInvenAvailable) = 0;
	
    //! ���� ��ų���� ���� ������Ʈ
	virtual int SetPetSkillSlotNum(int nChaNum, int nPetNum, WORD wSkillSlotNum) = 0;
	
    //! ���� �����Ѵ�.
    //! \param nPetNum ������ ���� ��ȣ
	virtual int	DeletePet(int nChaNum, int nPetNum) = 0;	

	//! ���� ��ų�� �����´�.
	//! \param pPet ���� ������
	virtual int GetPetSkill(int nChaNum, GLPET* pPet, DWORD dwPetNum) = 0;
		
    //! ���� �κ��丮 ������ �����´�
	virtual int GetPetInven(int nChaNum, GLPET* pPet, DWORD dwPetNum) = 0;
		
    //! ���� ������ �����´�.
	virtual int GetPet(GLPetField* pPet, DWORD dwPetNum, DWORD dwClientID) = 0;
	
    //! ���� �������� �����´�
    //! \param dwPetNum �� ��ȣ
    //! \return ���� ������ or sc::db::DB_ERROR
	virtual int GetPetFull(int nChaNum, DWORD dwPetNum) = 0;

    //! �Ҹ�� ���� ����Ʈ�� �����´�.
	//! \param nChaNum �Ҹ�� �� ����Ʈ�� ����Ϸ��� ĳ���� ��ȣ
	//! \param vecPetReList �Ҹ�� �� ����Ʈ�� ���� ����
	virtual int GetRestorePetList(int nChaNum, std::vector<PET_RELIST>& vecPetReList) = 0;
	
    //! ���� ��Ȱ��Ų��.
    //! \param nPetNum ��Ȱ�� �� ��ȣ
    //! \param nChaNum ��Ȱ�� �䱸�� ĳ���� ��ȣ
	virtual int RestorePet(DWORD dwPetNum, int nChaNum) = 0;

    //! ���� ��Ų���� ������Ʈ �Ѵ�.
    //! \param nChaNum		�ּ��� ĳ���� ��ȣ
    //! \param dwPetNum		�� ��ȣ
    //! \param nPetSkinMID	�� ��Ų MID
    //! \param nPetSkinSID	�� ��Ų SID
    //! \param nPetSkinScale	�� ��Ų Scale
    //! \param nPetSkinTime	�� ��Ų ����ð�
	virtual int UpdatePetSkin(int nChaNum,
							   DWORD dwPetNum,
							   int nPetSkinMID,
							   int nPetSkinSID,
							   int nPetSkinScale,
							   int nPetSkinTime) = 0;

	
    //! SMS ���ڸ޽����� �߼��Ѵ�.
    //! \param nSendChaNum SMS �� �߼��ϴ� ĳ���� ��ȣ	
    //! \param nReceiveChaNum SMS �� ���� ĳ���� ��ȣ
    //! \param szReceivePhone SMS �� ���� ĳ������ ��ȭ��ȣ
    //! \param szSmsMsg SMS �޽��� ����
	virtual int LogSMS(
					int nSendChaNum,					
					int nReceiveChaNum,
					const TCHAR* szReceivePhone,
					const TCHAR* szSmsMsg) = 0;

	
    //! �������� �����ɼ� �α׸� �����.
    //! \param _sLOG �����ɼ� ����ü
	virtual int LogRandomItem(const LOG_RANDOM_OPTION &_sLOG) = 0;

	// �ڽ�Ƭ �ɷ�ġ �ο� �α�
	virtual int LogItemCostumeStat(const LOG_COSTUME_STATS &_sLOG) = 0;
	
	virtual int LogAddonItem( const LOG_ADDON_OPTION &_sLOG ) = 0;
	virtual int LogLinkSkillItem( const LOG_LINKSKILL_OPTION &_sLOG ) = 0;
	virtual int LogBasicStatItem( const LOG_BASICSTAT_OPTION &_sLOG ) = 0;

    //! ĳ������ �޴��� ��ȣ�� �����Ѵ�.
    //! \param nChaNum ĳ���� ��ȣ
    //! \param szPhoneNumber ��ȭ��ȣ NULL ���� ���� 14�� SMS_RECEIVER
	virtual int SetChaPhoneNumber(int nChaNum, const TCHAR* szPhoneNumber) = 0;
	
    //! ���� �׼� �α׸� �����.
    //! \param _sLOG ��׼Ƿα� ����ü
	virtual int LogPetAction(const LOG_PET_ACTION &_sLOG) = 0;
	
    //! Terra
    //! ������ �н����带 �ְų� üũ�Ѵ�.
    //! \param szTID Terra UserID
    //! \param szPasswd ����� �н�����
    //! \return 
	virtual int TerraUserPassCheck(const TCHAR* szTID, const TCHAR* szPasswd, int nCheckFlag) = 0;
	
    //! Daum
    //! ������ �н����带 �ְų� üũ�Ѵ�.
    //! \param szDaumGID Daum UserGID
    //! \param szDaumPass ����� �н�����
    //! \return 
	virtual int DaumUserPassCheck(const TCHAR* szDaumGID, const TCHAR* szDaumPasswd, int nCheckFlag) = 0;

    //! Excite
    //! ������ �н����带 �ְų� üũ�Ѵ�.
    //! \param szUID Excite UserID
    //! \param szPasswd ����� �н�����
    //! \return 
	virtual int ExciteUserPassCheck(
        const TCHAR* szUID, const TCHAR* szUserID, const TCHAR* szPasswd, int nCheckFlag) = 0;

    //! Thailand
    //! ������� PC�� Ŭ������ üũ�Ѵ�.	
    //! \param szUsrIP ������� IP Address
    //! \return DB ������ �����
	virtual int	ThaiUserClassCheck(const TCHAR* szUsrIP) = 0;
	 
    //! Malaysia
    //! ������� PC�� Ŭ������ üũ�Ѵ�.	
    //! \param szUsrIP ������� IP Address
    //! \return DB ������ �����
	virtual int	MyUserClassCheck(const TCHAR* szUsrIP) = 0;
	 
    //! �ʸ���
    //! ������� PC�� Ŭ������ üũ�Ѵ�.	
    //! \param szUsrIP ������� IP Address
    //! \return DB ������ �����
	virtual int	PhUserClassCheck(const TCHAR* szUsrIP) = 0;
		
    //! ���ο� Ż���� �����Ѵ�.
    //! \param nChaNum  Ż�� ������ ĳ���� ��ȣ
    //! \param szVehicleName Ż�� �̸�
    //! \param nVehicleType Ż���� Ÿ��
    //! \param nVehicleMID Ż�� MID
    //! \param nVehicleSID Ż�� SID
    //! \param nVehicleCardMID Ż���� ������ MID
    //! \param nVehicleCardSID Ż���� ������ SID
    //! \param nBoosterID Ż���� �ν��� ����
    //! \return sc::db::DB_ERROR �Ǵ� ������������ Ż���� ������ȣ
	virtual int CreateVehicle(
		int nCharNum,
		const TCHAR* szVehicleName,
		int nVehicleType,
		int nVehicleCardMID,
		int nVehicleCardSID) = 0;

    //! Ż���� ���͸��� �����Ѵ�. ���͸��� 0-1000 ����
    //! \param nVehicleBttery ���͸��� ������ Ż���� ��ȣ
    //! \param nVehicleBttery ���͸�
	virtual int SetVehicleBattery(
		int nVehicleNum,
		int nCharNum,
		int nVehicleBttery) = 0;
	
    //! Ż���� ���͸��� �����´�.
    //! \param nVehicleBttery ���͸��� ������ Ż���� ��ȣ
	virtual int GetVehicleBattery(int nVehicleNum, int nCharNum) = 0;
	
    //! Ż���� �����Ѵ�.
    //! \param nVehicle ������ Ż���� ��ȣ
	virtual int DeleteVehicle(int nVehicle, int nCharNum) = 0;

	virtual int RebuyVehicle(int nVehicle, int nCharNum) = 0;
	
	//! Ż���� ������ �����´�.	
	virtual int GetVehicle(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) = 0;
	
    //! Ż���� �κ��丮 ������ �����´�
	//virtual int GetVehicleInven(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) = 0;
	
	//! Ż�� �κ��丮 ������Ʈ
	virtual int SetVehicleInven(int nVehicle, int nCharNum, se::ByteStream& ByteStream) = 0;

    //! Ż���� �÷� ������ �����´�
	//virtual int GetVehicleColor(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) = 0;

	//! Ż�� �÷� ������Ʈ
	virtual int SetVehicleColor(int nVehicle, int nCharNum, se::ByteStream &ByteStream) = 0;
	
    //! Ż���� �ν��� ��ų ������Ʈ
    //! \param m_nVehicleBooster ��� �ν��� ����
	virtual int SetVehicleBooster(int nVehicleNum, int nCharNum, DWORD dwVehicleBooster) = 0;
	
    //! Ż���� �׼� �α׸� �����.
    //! \param _sLOG Ż�;׼Ƿα� ����ü
	virtual int LogVehicleAction(const LOG_VEHICLE_ACTION &_sLOG) = 0;

	// �⼮�� �̺�Ʈ 
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
	virtual int	Certification2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) = 0; // 2�����
	virtual int	Update2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) = 0; // 2�����

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

    //! Billing ���� Shop, Item �Ǹ� ���� �α�
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

	//! �ֽ� CTF ����(rowcount)��ŭ �����´�.
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
    // �δ��α� InstantDungeon Log
 
    // �α� Ÿ���� ����
    virtual int InstantDungeonLogTypeInsert( 
        const db::EM_INSTANT_LOG_TYPE& nLogType,
        const char* strParamName1=0,  const char* strParamName2=0,  const char* strParamName3=0,  const char* strParamName4=0,  const char* strParamName5=0,
        const char* strParamName6=0,  const char* strParamName7=0,  const char* strParamName8=0,  const char* strParamName9=0,  const char* strParamName10=0,
        const char* strParamName11=0, const char* strParamName12=0, const char* strParamName13=0, const char* strParamName14=0, const char* strParamName15=0,
        const char* strParamName16=0, const char* strParamName17=0, const char* strParamName18=0, const char* strParamName19=0, const char* strParamName20=0 ) = 0;

    // ���� �α� �Է�
    virtual int InstantDungeonLogInsert( 
        const db::EM_INSTANT_LOG_TYPE& nLogType,
        const char* strParam1=0,  const char* strParam2=0,  const char* strParam3=0,  const char* strParam4=0,  const char* strParam5=0,
        const char* strParam6=0,  const char* strParam7=0,  const char* strParam8=0,  const char* strParam9=0,  const char* strParam10=0,
        const char* strParam11=0, const char* strParam12=0, const char* strParam13=0, const char* strParam14=0, const char* strParam15=0,
        const char* strParam16=0, const char* strParam17=0, const char* strParam18=0, const char* strParam19=0, const char* strParam20=0 ) = 0;


    /////////////////////////////////////////////////////////////////////////
    // ���� ���� CompetitionRating

    // ���� ���� ����;
    virtual int CompetitionRatingGetInfo(
        const std::string& strContentType,
        const DWORD charDbNum,
        int& Rating,
        int& WinningStreak,
        int& PlayCount,
        __time64_t& LastPlayDate
        ) = 0 ;

    // ���� ���� ����;
    virtual int CompetitionRatingUpdate(
        const std::string& strContentType,
        const DWORD charDbNum,
        const int Rating,
        const int WinningStreak,
        const int PlayCount,
        const __time64_t tLastPlayDate
        ) = 0;

    // ���� ���� ������Ʈ;
    virtual int CompetitionSeasonUpdate(
        const std::string& strContentType,
        const int seasonNum,
        const int baseRating
        ) = 0;

    // ���� �α� CompetitionLog

    ////////////////////////////////////////////////////////////////////////
	// ���� ���� �α� ���
	virtual int AddAuthServerState( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, int nCurUser, int nMaxUser, const __time64_t& LogDate ) = 0;
	// ���� ���� ����
	virtual int ProcessCertificationForAuth( int isSessionSvr, int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::string strUniqKey, int &certification, std::string &strNewUniqKey, int &nSessionSvrID ) = 0;
	// �̺�Ʈ ���� ���
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

    //! ITEM ID�� ���� �������� ������ �´�. ( bDateCheck => ��¥(Date)�˻� ���� )
    virtual int ConsignmentsSaleGet( sc::MGUID& Guid, private_market::ConsignmentSaleItem& SaleItem, bool bDateCheck = false  ) = 0;
    //! ChaNum�� ���� ����� ��ǰ�� ������ �´�. 
    virtual int ConsignmentsSaleSelect( private_market::SALE_ITEMS_VECTOR& vecItem, const DWORD& dwChaNum = 0, const int& ServerGroup = 0 ) = 0;
    //! ���� ������ �ٲ۴�.
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

	// �ʸ��� ��������
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

    // GM LOG ( GM ������ �־��ֱ� )
    virtual int WriteGMLog(int nGmNum, std::string& strCmd, std::string& strUserIP) = 0;

	// �������
	virtual int RanMobileRequest(DWORD dwUserDBNum, DWORD dwChaDBNum, int nServerNum, int nCommand, int nValue, const std::string& strChaName, int nChaClass ) = 0;

	// Log Money;
	virtual int MoneyLogInsert ( const int _nMainType, const int _nSubType, const LONGLONG _llMoney ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// �Ⱓ�� ������ �ð� ����
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
     * redmine : #1161 Randombox Ȯ������ �ý��� ó��
     * created : sckim, 2015.12.17, Randombox Ȯ������ �׸� �Ϸ�α� ����
     * modified : 
     */
    virtual int LogRandomboxChanceComplete(const SNATIVEID& BoxId,  /* RandomBox ID */
                                           const SNATIVEID& ItemId, /* ��÷ Item ID */
                                           INT32 ReqCount,          /* ��ǥ ���� ���� */
                                           DWORD ChaNum,            /* ��ǥ ĳ���� ��ȣ */
                                           DWORD GMNum,             /* ��� GM Number */
                                           INT32 OpenCount,         /* ���� ���� ���� */
                                           DWORD Winner             /* ��÷�� */
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
