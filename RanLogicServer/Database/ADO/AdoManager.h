#ifndef _DB_ADO_MANAGER_H_
#define _DB_ADO_MANAGER_H_

#pragma once

#include "../../SigmaCore/Util/Lock.h"
//#include "../../SigmaCore/Database/Ado/AdoClass.h"
#include "../../SigmaCore/String/MinGuid.h"
#include "../../RanLogic/Character/GLCharData.h"
#include "./AdoExt.h"
#include "../DbManager.h"

struct CTF_REWARD_BUFF;

namespace db
{

class AdoManager : public IDbManager, private sc::DefaultLock
{
public:
    AdoManager(NetServer* pServer, SERVER_TYPE ServerType, EMSERVICE_PROVIDER ServiceProvider);
    virtual ~AdoManager();

private:    
    std::string m_UserDBConnString;
    std::string m_GameDBConnString;
    std::string m_LogDBConnString;
    std::string m_ShopDBConnString;
	std::string m_TexasHoldemDBConnString;
	std::string m_GlobalAuthDBConnString;
	std::string m_WorldBattleDBConnString;
	std::string m_RanMobileDBConnString;

    sc::db::AdoExt m_adoUser;
    sc::db::AdoExt m_adoGame;
    sc::db::AdoExt m_adoLog;
    sc::db::AdoExt m_adoShop;
	sc::db::AdoExt m_adoTexasHoldem;
	sc::db::AdoExt m_adoGlobalAuth;
	sc::db::AdoExt m_adoWorldBattle;
	sc::db::AdoExt m_adoRanMobile;

private:
    int ExecuteStoredProcedure(const std::string& SpName, sc::db::AdoExt& Ado);
    int ExecuteStoredProcedureIntReturn(const std::string& SpName, sc::db::AdoExt& Ado, const std::string& OutPutParamName);
    LONGLONG ExecuteStoredProcedureLonglongReturn(const std::string& SpName, sc::db::AdoExt& Ado, const std::string& OutPutParamName);

public:
    virtual bool IsAlive() override;
    virtual bool Reconnect() override; //! 재접속 체크
    virtual bool CheckHdd() override; //! Hard disk 용량 체크
    virtual bool IsAllDbJobCompleted() override { return true; }
    virtual void WaitAllDbJobCompleted(DWORD WaitLimitTime) override;

public:
    //! -----------------------------------------------------------------------
    //! 호환성 유지를 위해서 임시로...
    
    //! Odbc
    virtual int	OdbcOpenGameDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override;
    virtual int	OdbcOpenUserDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override;
    virtual int	OdbcOpenBoardDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override;
    virtual int	OdbcOpenLogDB  (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override;
    virtual int OdbcOpenShopDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override;
    virtual int	OdbcOpenTerraDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override;
    virtual int	OdbcOpenGspDB  (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override;
    virtual int	OdbcOpenThaiDB (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override;
    virtual int	OdbcOpenKorDB  (const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override;
    //! 말레이시아 PC방 이벤트
    virtual int	OdbcOpenMyDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override;
    
    //! Ado
    virtual bool SetConnectionString(DB_TYPE Type, const std::string& Ip, const std::string& DbName, const std::string& Id, const std::string& Password, int AdoMaxPoolSize, int DatabasePacketSize) override;

    //! -----------------------------------------------------------------------

    //! 버전 체크
    bool CheckVersion(DB_TYPE Type);

	virtual int UpdateUserLockerOption( DWORD UserNum, int Option ) override;
    virtual int SaveCharacter( LPVOID _pbuffer, bool bAutoSave = false ) override;
    virtual int SaveCharacterBase(SCHARDATA2* pCharData2, DWORD ChaDbNum) override;
	virtual int SaveCharacterExtraWithAdo(LPVOID _pbuffer) override;	//mjeon.ado

	virtual int SaveInventoryAndGameMoney( LPVOID _pbuffer ) override;

    int SaveCharacterSkill(SCHARDATA2* pChar);
    int SaveCharacterSkillQuickSlot(SCHARDATA2* pChar);
    int SaveCharacterActionQuickSlot(SCHARDATA2* pChar);
    int SaveCharacterPutOnItems(SCHARDATA2* pChar);
    int SaveCharacterInventory(SCHARDATA2* pChar);
    int SaveCharacterInventoryVietnam(SCHARDATA2* pChar);
    int SaveCharacterTaiwan(SCHARDATA2* pChar);
    int SaveCharacterQuest(SCHARDATA2* pChar);
    int SaveCharacterCoolTime(SCHARDATA2* pChar);
    int SaveCharacterSkillFact(SCHARDATA2* pChar);
	int SaveCharacterHireSummon(SCHARDATA2* pChar);
	int SaveCharacterSlots(SCHARDATA2* pChar);
    int SaveUserInven(SCHARDATA2* pChar);

	int SaveCharacterPutOnItemsNew( SCHARDATA2* pChar );
	int SaveCharacterInventoryNew( SCHARDATA2* pChar );
	int SaveUserInvenNew( SCHARDATA2* pChar );
	int SaveCharacterQuestItems( SCHARDATA2* pChar );

	int GetPetPutonItems( GLPetField* pPet, DWORD dwPetNum );
	int GetVehiclePutonItems( GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum );
	int GetAllPetPutonItems( DWORD ChaDbNum, std::vector< GLPET >& vecPets );
	int GetAllVehiclePutonItems( DWORD ChaDbNum, std::vector< GLVEHICLE >& vecVehicles );
	
	//DbManager의 일반적으로 사용되는 가상함수가 아니다. ...ExtraWithAdo에 의해 호출되는 함수.
	//virtual int SaveProductBook(UINT nChaNum, ProductBookContainer& setLearnProductBook);
	//virtual int LoadProductBook(UINT nChaNum, ProductBookContainer& setLearnProductBook);

    // 제조
    virtual int AddProductType(const DWORD dwChaNum, const WORD nProductType) override;
    virtual int GetProductType(const DWORD dwChaNum, std::vector<WORD>& vecProductType OUT) override;
    virtual int DelProductType(const DWORD dwChaNum, const WORD nProductType) override;

    virtual int AddProductIsGetExp(const DWORD dwChaNum, const DWORD dwProductID) override;
    virtual int GetProductIsGetExp(const DWORD dwChaNum, std::vector<WORD>& vecProductID OUT) override;
    virtual int DelProductIsGetExp(const DWORD dwChaNum, const PRODUCT_ITEM& productItem) override;

    virtual int SaveProductBook(DWORD nChaNum, const std::vector<DWORD>& setLearnProductBook) override;
    virtual int LoadProductBook(DWORD nChaNum, std::vector<DWORD>& setLearnProductBook OUT) override;
    virtual int DeleteProductBook(DWORD nChaNum, const std::vector<PRODUCT_ITEM>& vecBook) override;
    
	virtual int CharProductLog( const CHAR_PRODUCT_LOG& productLog ) override;
	virtual int ProductExchangeLog( const PRODUCT_EXCHANGE_LOG& productLog ) override;
	
    virtual int	ReadUserInven(SCHARDATA2* pChaData2) override;
	virtual int	ReadUserInvenNew( SCHARDATA2* pChaData2, int FirstLoad = 0 ) override;
    bool CheckInven(int SGNum, DWORD dwUserNum);
    int MakeUserInven(int SGNum, DWORD dwUserNum);
    int	ReadUserInven(int SGNum, DWORD dwUserNum, SCHARDATA2* pChaData2);
	int	ReadUserInvenForItemTransfer( int SGNum, DWORD dwUserNum, SCHARDATA2* pChaData2 );

    virtual int DelChaFriend(int nChaP, int nChaS) override;
    virtual int AddChaFriend(int nChaP, int nChaS) override;
    virtual int SetChaFriend(int nChaP, int nChaS, int nFlag) override;
    virtual int ChaFriendFlagSelect(int ChaP, int ChaS, int& Flag OUT) override;

    //mjeon.Ping
    virtual int AddLogPing(int nUserNum, int nRTT, const char* szIP, __time64_t tTime) override;

    //mjeon.Post
    virtual int PostSend(SPOSTINFO &PostInfo, SITEMCUSTOM &itemcustom) override;
	virtual int PostSendRollBack(LONGLONG &llPostID) override;
    virtual int PostOpenAndTake(LONGLONG llPostID, BOOL bMoneyAccept, BOOL bAttach01Accept, BYTE bChargePayment, BYTE bReturnedPost) override;
    virtual int PostSendCharge(SAPOST &aPOST, DWORD dwOpenAndTakeUserNum) override;

    //virtual int PostCheckAttachMoney(DWORD dwClientID, SPOSTINFO &PostInfo, SINVENITEM *pItem) override;
    
    //! 학교vs학교 파티대련결과저장
    virtual int LogPartyMatch(int nSGNum, int nSvrNum, WORD wWin, WORD wLost) override;

    virtual LONGLONG GetItemMaxNum(int nSGNum, int nSvrNum,	int nFldNum, std::vector<VIEWLOGITEMEXCHANGEMAX> &v) override;
    virtual int GetLogItem(std::vector<DWORD>& LogItemData) override;
    virtual int LogItemExchange( const LOG_ITEM_EXCHANGE& sEXCHANGE ) override;
    virtual int LogMoneyExchange( const LOG_MONEY_EXCHANGE& _sLOG )	 override;
	virtual int LogPointShopExchange( int nFromType, DWORD ChaDbNum, int nExchangeFlag, WORD ItemMid, WORD ItemSid, int nPoint, int nExchangeFlagDetail ) override;
    virtual int LogItemConversion(const LOG_ITEM_CONVERSION &_sLOG) override;
    virtual int LogAction(const LOG_ACTION& LogData) override;
//     int LogItemExchangeInsert(
//         // 주번호, 부번호
//         int	Mid, int Sid,
//         // 서버그룹, 서버번호, 필드번호
//         int ServerGroup, int ServerNum, int FieldNum,
//         // 생성타입, 생성번호
//         int	MakeType, LONGLONG lnMakeNum,
//         // from의 종류 (유저, 케릭터, 클럽), 아이템 출발지
//         int FromType, int ItemFrom, 
//         // to의 종류 (유저, 케릭터, 클럽), 아이템 목적지
//         int ToType, int ItemTo, 
//         // 교환 타입, 개수
//         int ExchangeFlag, int CountNum, 
//         // 코스툼
//         int CostumeMid, int CostumeSid,
//         // 연마 정보
//         short Damage, short Defense, short Fire, short Ice, short Poison, short Electric, short Spirit,        
//         // 교환가격
//         LONGLONG TradePrice,
// 		int nBaseMainMapID, int nBaseSubMapID, int nGaeaMainMapID, int nGaeaSubMapID, 
// 		short nOriNIDMain, short nOriNIDSub,
// 		int nExchangeFlagDetail,
//         const __time64_t& LogDate);

    //! 선도전 로그
    virtual int LogGuidance(const LOG_GUIDANCE &sLog) override;

    //! CDM 로그
    virtual int LogCDM(const LOG_CDM &sLog) override;

    //! CDM 로그(최종) 
    virtual int LogCDMReuslt(const LOG_CDM_RESULT &sLog) override;

    //! Shop 에서 구입한 아이템을 가져온다.
    //! \param strUID 유저ID
    //! \param &v 상품을 담을 벡터
    //! \return 
    virtual int GetPurchaseItem(const CString& strUID, std::vector<SHOPPURCHASE> &v) override;

    //! 실제로 가져갈 수 있는 상품인지 확인한다.
    //! \param strPurKey 구입코드
    //! \param nFlag(0 : 구매, 1 : 구매완료, 2 : 구매취소신청, 3 : 구매취소처리)
    //! \return 1:가져갈수 있음 이외의값:가져갈수 없음
    virtual int SetPurchaseItem(const CString& strPurKey, int nFlag=1) override;

    //! 캐릭터 삭제, 캐릭터생성 갯수 증가
    virtual int UserChaNumIncrease(int nUserNum) override;

    //! 해당 락커 만료시간을 정한다.
    //! \n 1 번 5 번 락커는 정할 수 없음.
    //! \n 2, 3, 4 번 락커만 만료시간을 정할 수 있다.
    //! \param nUserNum 사용자번호
    //! \param nStorageNum 스토리지번호
    //! \param tTime 만료시간
    //! \return -1:sc::db::DB_ERROR  0:sc::db::DB_OK
    virtual int SetChaStorageDate(int nUserNum, int nStorageNum, __time64_t tTime) override;

    //! 해당 캐릭터의 인벤토리 추가 줄수를 세팅한다.
    //! \n 최초 0 에서 한줄 추가시 1 을 입력
    //! \n 현재 한줄 상태에서 또 한줄 추가시 2 를 입력(최종 줄수를 입력한다)
    //! \n 현재 두줄 상태에서 한줄 추가시 3 을 입력
    //! \n 최대 3까지만 입력가능.
    //! \param nChaNum 캐릭터번호
    //! \param wInvenLine 추가줄수
    virtual int SetChaInvenNum(int nChaNum, WORD wInvenLine) override;

    //! 해당 사용자의 프리미엄 기간을 세팅한다.
    virtual int SetPremiumTime(int nUserNum, __time64_t tPremiumTime) override;

    //! 직전귀환 카드 사용을 위한 포지션을 저장한다.
    virtual int SetLastCallPos(int nChaNum, DWORD dwMapID, D3DXVECTOR3 vPos) override;    

    //! 새로운 캐릭터를 생성한다.
    virtual int	CreateNewCharacter(SCHARDATA2* pCharData2) override;
	virtual int	CreateNewCharacterExtraWithAdo(SCHARDATA2* pCharData2) override;	//mjeon.ado	

    //! ChaInfo Backup.
    virtual int CharacterInfoBackup(SCHARDATA2* pCharData2) override;

    // 대만,홍콩
    // 캐릭터 생성시 Temp Table에 캐릭터 레벨과 금액 정보를 암호화 해서 저장해 둔다.
    int	InsertCharInfoTemp(int nChaNum);

    virtual int CreateClub(const std::string& ClubName, DWORD ChaDbNum, wb::EM_COUNTRY Country) override;

    //! 클럽을 삭제한다.
    //! dwClub : 클럽번호
    //! dwChaNum : 클럽을 삭제하려는 캐릭터번호(캐릭터 번호가 클럽 마스터가 아니면 삭제가 되지 않는다)
    virtual int DeleteClub(DWORD dwClub, DWORD dwChaNum) override;    

    //! 클럽랭크를 세팅한다.
    //! dwClub : 클럽번호
    //! dwRank : 랭킹
    virtual int SetClubRank(DWORD dwClub, DWORD dwRank) override;

    virtual int ReadClubMarkImage(DWORD dwClub, se::ByteStream &ByteStream) override;

    virtual int WriteClubMarkImage(DWORD dwClub, DWORD dwMarkVer, BYTE* pData, int nSize) override;

    //! 클럽 해체시간을 설정한다.
    //! \param dwClub 클럽번호
    //! \param tDiss 해체시간
    virtual int SetClubDissolutionTime(DWORD dwClub, __time64_t tDiss) override;

    //! 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
    //! \param dwClub 클럽번호
    //! \param tSec 시간
    virtual int SetClubAllianceSec(DWORD dwClub, __time64_t tSec) override;

    //! 클럽의 동맹해산 시간을 정한다.
    //! \param dwClub 클럽번호
    //! \param tDis 해산시간
    virtual int SetClubAllianceDis(DWORD dwClub, __time64_t tDis) override;

    //! 클럽의 마스터 위임 시간을 정한다.
    //! \param dwClub 클럽번호
    //! \param tDis 마스터 위임시간
    virtual int SetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority) override;

    //! 서버의 모든 클럽정보를 가져온다
    //! \param vClub : 클럽정보
    virtual int GetClubInfo(std::vector<std::tr1::shared_ptr<GLClub> > &vClub) override;

    //! 해당 클럽의 멤버를 가져온다
    //! \param dwGuild : 클럽번호
    //! \param vMember : 클럽멤버
    virtual int GetClubMember(DWORD dwClub, std::vector<GLCLUBMEMBER>& vMember) override;

    //! 클럽의 권한 설정을 가져온다.
    virtual int GetClubAuth(DWORD ClubDbNum, std::vector<club::AUTH_GRADE_DB>& vAuth) override;
    //! 해당 클럽의 모든 캐릭터의 클럽 지위를 0 으로 만든다.
    virtual int ResetClubPosition(DWORD ClubDbNum) override;

    //! 새로운 캐릭터를 클럽에 가입시킨다
    //! \param dwClub : 클럽번호
    //! \param dwChaNum : 가입시키려는 캐릭터번호
    virtual int AddClubMember(DWORD dwClub, DWORD dwChaNum) override;

    //! 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다
    //! \param dwChaNum : 캐릭터번호
    virtual int DeleteClubMember(DWORD dwChaNum) override;

	//! 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다(탈퇴 패널티 X)
	//! \param dwChaNum : 캐릭터번호
	virtual int DeleteClubMemberWithNoPenalty( DWORD dwChaNum ) override;

    //! 클럽 배틀을 신청한다.
    //! \param dwClubP 주클럽번호(클럽 배틀의 신청을 받은 주 )
    //! \param dwClubS 보조 클럽번호(클럽 배틀의 신청을 한 클럽 )	
    virtual int SetClubBattle(DWORD dwClubP, DWORD dwClub, DWORD dwEndTime, int nAlliance) override;

    //! 클럽 배틀을 종료한다.
    //! \param dwClubP 주클럽 번호(클럽 배틀의 신청을 받은 주 )
    //! \param dwClubS 보조 클럽 번호(클럽 배틀의 신청을 한 클럽 )
    //! \param nGuFlag 승패여부
    //! \param nGuKillNum 클럽 배틀 킬수
    virtual int EndClubBattle(DWORD dwClubP, DWORD dwClubS, int nGuFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance) override;


    //! 클럽 배틀정보를 저장한다,(클럽 배틀이 종료되기전 서버재시작이 필요한경우 배틀 정보의 저장이 필요함 )
    //! \param dwClubP 주클럽 번호(클럽 배틀의 신청을 받은 주)
    //! \param dwClubS 보조 클럽 번호(클럽 배틀의 신청을 한 클럽 )
    //! \param nGuKillNum 클럽 배틀 킬수
    //! \return
    virtual int SaveClubBattle(DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum) override;

    //! 동맹배틀 전적을 초기화 시키긴다.
    //! \param dwClub 클럽 번호
    virtual int ReSetAllianceBattle(DWORD dwClub) override;

    //! 해당 클럽의 배틀중인 클럽 정보를 가져온다.
    //! \param dwClub 클럽번호
    //! \param &vMember 클럽배틀 정보
    virtual int GetClubBattleInfo(DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo) override;

	virtual int UpdateClubName( DWORD dwClub, const std::string& ClubName ) override;

    //! 해당 캐릭터의 경험치를 세팅한다.
    //! 캐릭터번호는 0 보다 커야한다.
    //! 경험치는 0 보다 커야한다.
    //! \param  nChaNum : 캐릭터번호
    //! \param   llExp : 경험치
    virtual int SetChaExp(int nChaNum, LONGLONG llExp) override;

    //! 해당 캐릭터의 경험치를 가져온다.
    //! 캐릭터번호는 0 보다 커야한다.
    //! 경험치가 0 보다 작으면 에러이다.
    //! \param nChaNum 캐릭터번호
    //!  LONGLONG : 경험치
    virtual LONGLONG GetChaExp(int nChaNum) override;

    //! 게임내에서 진정이나 신고로 신고한 내용을 DB에 기록한다.    
    //! \param nSGNum 서버그룹
    //! \param ChaNum 신고한 캐릭터번호
    //! \param strMsg 신고내용(최대크기는 CHAT_MSG_SIZE)
    virtual int LogAppeal(int nSGNum, int nChaNum, CString strMsg) override;

    //! 현재 모든지역의 선도 클럽데이터를 가져온다.
    virtual int GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB) override;

    //! 지역과 그지역을 소유한 클럽 세율을 정한다.
    //! \param dwRegionID : 지역 ID
    //! \param dwClub : 클럽 ID
    //! \param fTax : 세율
    virtual int SetClubRegion(DWORD dwRegionID, DWORD dwClub=0, float fTax=0) override;

    //! 해당 지역의 소유 상태를 리셋한다.
    //! \param dwRegionID 지역 ID
    //! \param dwClub 클럽 ID(기본 0 이다)
    virtual int DelClubRegion(DWORD dwRegionID, DWORD dwClub=0) override;

    //! 해당클럽의 보유금액을 가져온다.(Storage)
    //! \param dwClub 클럽번호
    //! \param 리턴값은 >= 0, 0 보다 작으면 ERROR
    virtual LONGLONG GetClubMoney(DWORD dwClub) override;

    //! 해당클럽의 보유금액을 세팅한다.(Storage)
    //! \param dwClub : 클럽번호
    //! \param llMoney : 보유금액(금액은 >= 0)
    virtual int	SetClubMoney(DWORD dwClub, LONGLONG llMoney) override;

    //! 해당클럽의 수입금액을 세팅한다.(Income money)
    //! \param dwClub : 클럽번호
    //! \param llMoney : 수입금액(금액은 >= 0)
    virtual int	SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney) override;    

    //! 해당클럽의 수입금액을 가져온다.(Income money)
    //! \param dwClub : 클럽번호
    //! \return 리턴값은 >= 0, 0 보다 작으면 ERROR
    virtual LONGLONG GetClubIncomeMoney(DWORD dwClub) override;

    //! 해당클럽의 클럽창고를 읽어온다.
    //! \param dwClub : 클럽번호
    //! \param ByteStream : 버퍼
    virtual int ReadClubStorage(DWORD dwClub, se::ByteStream &ByteStream) override;    
	virtual int GetGuildMigrationState( DWORD dwClub ) override;

    //! 해당클럽의 클럽창고를 저장한다.
    //! \param dwClub : 클럽번호
    //! \param pData : 창고위치
    //! \param nSize : 버퍼 전체크기
    virtual int	WriteClubStorage(DWORD dwClub, BYTE* pData, int nSize) override;	

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
        const TCHAR* szComment) override;

    //! 부 클럽장을 세팅한다
    //! \param dwClub : 클럽번호
    //! \param dwChaNum : 캐릭터번호
    //! \param dwSubMasterFlags : 플래그(0 으로 넣으면 일반길드원이 된다)
    virtual int	SetClubMasterFlags(
        DWORD dwClub,
        DWORD dwChaNum,
        DWORD dwSubMasterFlags) override;

    //! 동맹 클럽을 결성한다.
    //! \ db 는 클럽의 무결성을 체크하지 않는다.
    //! \ A 가 B 의 보조인 상태에서 
    //! \ B 가 C 의 보조로 들어가고
    //! \ C 가 A 의 보조로 들어가면 소유권 체인 문제가 발생한다.	
    //! \param dwClubP : 주 클럽번호(동맹 클럽의 주)
    //! \param dwClubS : 보조 클럽번호(주 클럽 밑으로 들어가는 하위 클럽)
    virtual int	SetClubAlliance(DWORD dwClubP, DWORD dwClubS) override;

    //! 동맹 클럽을 해체한다.
    //! \param dwClubP : 주 클럽번호(동맹 클럽의 주)
    //! \param dwClubS : 보조 클럽번호(주 클럽 밑으로 들어가는 하위 클럽)
    virtual int	DelClubAlliance(DWORD dwClubP, DWORD dwClubS) override;

    //! 동맹 클럽 리스트를 가져온다.
    //! <주클럽,보조클럽>
    virtual int	GetClubAlliance(std::vector< std::pair<DWORD,DWORD> > &vecAlliance) override;

    //! 해당 사용자의 채팅 블록시간을 가져온다
    virtual __time64_t  GetChatBlockTime(int nUserNum) override;

    //! 해당 사용자의 채팅 블록시간을 설정한다
    virtual int SetChatBlockTime(int nUserNum, __time64_t tBlockTime) override;

    //! 클럽의 공지사항을 세팅한다.
    virtual int SetClubNotice(const std::string& ChaName, DWORD dwClub, const std::string& ClubNotice) override;

    //! 클럽의 선도클럽 인증 대리자를 세팅한다.
    virtual int SetClubDeputy(DWORD dwClub, DWORD dwDeputy) override;

    //! 클럽의 마스터 권한을 위임한다.
    virtual int SetClubAuthority(DWORD dwClub, DWORD dwMasterID) override;
    virtual int InsertClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) override;
    virtual int UpdateClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) override;
    virtual int ChangeClubAuthIndex(DWORD ClubDbNum, size_t AuthIndexA, size_t AuthIndexB) override;
    virtual int ChangeClubMemberGrade(DWORD ChaDbNum, int Grade) override;

    //! 캐릭터의 이름을 변경한다.
    //! \param dwChaNum 캐릭터번호
    //! \param szCharName 변경할 캐릭터이름
    //! \return sc::db::DB_OK, sc::db::DB_ERROR
    virtual int RenameCharacter(DWORD dwChaNum, char* szCharName) override;

    //! 캐릭터의 머리색을 변경한다.
    //! \return sc::db::DB_OK, sc::db::DB_ERROR
    virtual int SetChaHairColor(DWORD dwChaNum, int nHairColor) override;

    //! 캐릭터의 헤어스타일을 변경한다.
    //! \return sc::db::DB_OK, sc::db::DB_ERROR	
    virtual int SetChaHairStyle(DWORD dwChaNum, int nHairStyle) override;

    //! 캐릭터의 얼굴스타일을 변경한다.
    //! \return sc::db::DB_OK, sc::db::DB_ERROR	
    virtual int SetChaFaceStyle(DWORD dwChaNum, int nFaceStyle) override;

    //! 캐릭터의 성별을 변경한다.
    //! \return sc::db::DB_OK, sc::db::DB_ERROR	
    virtual int SetChaGenderChange(DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor) override;

	// 캐릭터 학원을 변경한다
	virtual int SetChaSchoolChange(DWORD dwChaNum, int nSchool) override;

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
        FLOAT fLastCallPosZ) override;

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
        ) override;


    //! 펫의 이름을 변경한다.
    //! \param nChaNum 캐릭터 번호
    //! \param nPetNum 팻의 고유번호
    //! \param szPetName 팻이름
    //! \return sc::db::DB_ERROR, sc::db::DB_OK
    virtual int RenamePet(int nChaNum, int nPetNum, const TCHAR* szPetName) override;

    //! 팻의 소유권을 이전한다.
    //! \param nChaNum 팻의 새로운 소유자
    //! \param nPetNum 소유권을 이전할 팻 번호
    virtual int ExchangePet(int nChaNum, int nPetNum) override;

    //! 팻의 컬러를 변경한다.
    //! \param nChaNum 캐릭터 번호
    //! \param nPetNum 컬러를 변경하려는 팻의 번호
    //! \param nPetColor 팻의 컬러
    virtual int SetPetColor(int nChaNum, int nPetNum, int nPetColor) override;

    //! 팻의 스타일을 변경한다.
    //! \param nPetNum 스타일을 변경하려는 팻의 번호
    //! \param nPetStyle 팻의 스타일
    virtual int SetPetStyle(int nChaNum, int nPetNum, int nPetStyle) override;

    //! 팻의 포만도를 설정한다. 포만도는 0-100 사이
    //! \param nPetNum 포만도를 설정할 팻의 번호
    //! \param fPetFull 포만도
    virtual int SetPetFull(int nChaNum, int nPetNum, int nPetFull) override;

    //! 팻의 인벤토리 업데이트
    virtual int SetPetInven(int nChaNum, int nPetNum, se::ByteStream &ByteStream) override;

	virtual int SetPetMigrationState( int ChaNum, int PetNum, int MigrationState ) override;
	virtual int SetVehicleMigrationState( int ChaNum, int VehicleNum, int MigrationState ) override;

    //! 팻의 스킬 업데이트
    virtual int SetPetSkill(
        int nChaNum,
        int nPetNum,
        int nPetInvenType,
        int nPetInvenMID,
        int nPetInvenSID,
        int nPetInvenCMID,
        int nPetInvenCSID,
        int nPetInvenAvailable) override;

    //! 팻의 스킬슬롯 갯수 업데이트
    virtual int SetPetSkillSlotNum(int nChaNum, int nPetNum, WORD wSkillSlotNum) override;

    //! 팻을 삭제한다.
    //! \param nPetNum 삭제할 팻의 번호
    virtual int	DeletePet(int nChaNum, int nPetNum) override;	

    //! 팻의 스킬을 가져온다.
    //! \param pPet 펫의 포인터
    virtual int GetPetSkill(int nChaNum, GLPET* pPet, DWORD dwPetNum) override;

    //! 팻의 인벤토리 정보를 가져온다
    virtual int GetPetInven(int nChaNum, GLPET* pPet, DWORD dwPetNum) override;

    //! 팻의 정보를 가져온다.
    virtual int GetPet(GLPetField* pPet, DWORD dwPetNum, DWORD dwClientID) override;

    //! 팻의 포만감을 가져온다
    //! \param dwPetNum 팻 번호
    //! \return 팻의 포만감 or sc::db::DB_ERROR
    virtual int GetPetFull(int nChaNum, DWORD dwPetNum) override;

    //! 소멸된 팻의 리스트를 가져온다.
    //! \param nChaNum 소멸된 펫 리스트를 출력하려는 캐릭터 번호
    //! \param vecPetReList 소멸된 펫 리스트를 넣을 벡터
    virtual int GetRestorePetList(int nChaNum, std::vector<PET_RELIST>& vecPetReList) override;

    //! 팻을 부활시킨다.
    //! \param nPetNum 부활할 팻 번호
    //! \param nChaNum 부활을 요구한 캐릭터 번호
    virtual int RestorePet(DWORD dwPetNum, int nChaNum) override;

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
        int nPetSkinTime) override;

    //! SMS 문자메시지를 발송한다.
    //! \param nSendChaNum SMS 를 발송하는 캐릭터 번호	
    //! \param nReceiveChaNum SMS 를 받을 캐릭터 번호
    //! \param szReceivePhone SMS 를 받을 캐릭터의 전화번호
    //! \param szSmsMsg SMS 메시지 내용
    virtual int LogSMS(
        int nSendChaNum,					
        int nReceiveChaNum,
        const TCHAR* szReceivePhone,
        const TCHAR* szSmsMsg) override;

    //! 아이템의 랜덤옵션 로그를 남긴다.
    //! \param _sLOG 랜덤옵션 구조체
    virtual int LogRandomItem(const LOG_RANDOM_OPTION &_sLOG) override;

	virtual int LogAddonItem( const LOG_ADDON_OPTION &_sLOG ) override;
	virtual int LogLinkSkillItem( const LOG_LINKSKILL_OPTION &_sLOG ) override;
	virtual int LogBasicStatItem( const LOG_BASICSTAT_OPTION &_sLOG ) override;

	virtual int LogItemCostumeStat(const LOG_COSTUME_STATS &_sLOG) override;

    //! 캐릭터의 휴대폰 번호를 저장한다.
    //! \param nChaNum 캐릭터 번호
    //! \param szPhoneNumber 전화번호 NULL 문자 포함 14자 SMS_RECEIVER
    virtual int SetChaPhoneNumber(int nChaNum, const TCHAR* szPhoneNumber) override;

    //! 팻의 액션 로그를 남긴다.
    //! \param _sLOG 펫액션로그 구조체
    virtual int LogPetAction(const LOG_PET_ACTION &_sLOG) override;

    //! Terra
    //! 유저의 패스워드를 넣거나 체크한다.
    //! \param szTID Terra UserID
    //! \param szPasswd 사용자 패스워드
    //! \return 
    virtual int TerraUserPassCheck(const TCHAR* szTID, const TCHAR* szPasswd, int nCheckFlag) override;

    //! Daum
    //! 유저의 패스워드를 넣거나 체크한다.
    //! \param szDaumGID Daum UserGID
    //! \param szDaumPass 사용자 패스워드
    //! \return 
    virtual int DaumUserPassCheck(const TCHAR* szDaumGID, const TCHAR* szDaumPasswd, int nCheckFlag) override;

    //! Excite
    //! 유저의 패스워드를 넣거나 체크한다.
    //! \param szUID Excite UserID
    //! \param szPasswd 사용자 패스워드
    //! \return 
    virtual int ExciteUserPassCheck(
        const TCHAR* szUID, const TCHAR* szUserID, const TCHAR* szPasswd, int nCheckFlag) override;

    //! Thailand
    //! 사용자의 PC방 클래스를 체크한다.	
    //! \param szUsrIP 사용자의 IP Address
    //! \return DB 인증후 결과값
    virtual int	ThaiUserClassCheck(const TCHAR* szUsrIP) override;

    //! Malaysia
    //! 사용자의 PC방 클래스를 체크한다.	
    //! \param szUsrIP 사용자의 IP Address
    //! \return DB 인증후 결과값
    virtual int	MyUserClassCheck(const TCHAR* szUsrIP) override;

    //! 필리핀
    //! 사용자의 PC방 클래스를 체크한다.	
    //! \param szUsrIP 사용자의 IP Address
    //! \return DB 인증후 결과값
    virtual int	PhUserClassCheck(const TCHAR* szUsrIP) override;

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
        int nVehicleCardSID) override;

    //! 탈것의 배터리를 설정한다. 배터리는 0-1000 사이
    //! \param nVehicleBttery 배터리를 설정할 탈것의 번호
    //! \param nVehicleBttery 배터리
    virtual int SetVehicleBattery(
        int nVehicleNum,
        int nCharNum,
        int nVehicleBttery) override;

    //! 탈것의 배터리를 가져온다.
    //! \param nVehicleBttery 배터리를 설정할 탈것의 번호
    virtual int GetVehicleBattery(int nVehicleNum, int nCharNum) override;

    //! 탈것을 삭제한다.
    //! \param nVehicle 삭제할 탈것의 번호
    virtual int DeleteVehicle(int nVehicle, int nCharNum) override;

	virtual int RebuyVehicle(int nVehicle, int nCharNum) override;

    //! 탈것의 정보를 가져온다.	
    virtual int GetVehicle(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) override;

    //! 탈것의 인벤토리 정보를 가져온다
    //virtual int GetVehicleInven(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) override;

    //! 탈것 인벤토리 업데이트
    virtual int SetVehicleInven(int nVehicle, int nCharNum, se::ByteStream& ByteStream) override;

    //! 탈것의 컬러 정보를 가져온다
    //virtual int GetVehicleColor(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) override;

    //! 탈것 컬러 업데이트
    virtual int SetVehicleColor(int nVehicle, int nCharNum, se::ByteStream &ByteStream) override;

    //! 탈것의 부스터 스킬 업데이트
    //! \param m_nVehicleBooster 배운 부스터 종류
    virtual int SetVehicleBooster(int nVehicleNum, int nCharNum, DWORD dwVehicleBooster) override;

    //! 탈것의 액션 로그를 남긴다.
    //! \param _sLOG 탈것액션로그 구조체
    virtual int LogVehicleAction(const LOG_VEHICLE_ACTION &_sLOG) override;

    // 출석부 이벤트 
    virtual int LogUserAttend(int nUserNum, std::vector<USER_ATTEND_INFO> &vecUserAttend) override;

    virtual int InsertUserAttend(int nUserNum, CString strUserName, __time64_t tAttendTime, int nCount,
        int nAttendReward, int nItemMID, int nItemSID) override;

    virtual int InsertShopItemMap(int nItemMID, int nItemSID) override;

    // UserMoney Add
    virtual int UpdateUserMoneyAdd(DWORD dwUserID, LONGLONG lnUserMoney) override;
    virtual int UpdateUserLastInfoAdd(DWORD dwUserID, LONGLONG lnUserMoney) override;

    //mjeon.dbman.from odbcmanager
    virtual	int SetAllCharacterOffline() override;
    virtual int	AllServerUserLogout(int nSvrGrp) override;
    
    //mjeon.dbman.from DbLogicAction
    
    //GameActions
    virtual int PostMaintenance()											 override;
    virtual int PostCleanUp(char *pszCurrentDate, char *pszTargetDate)		 override;
    virtual int PostReturn(LONGLONG llPostID)								 override;
    virtual int PostDelete(LONGLONG llPostID)								 override;
    virtual int PostOpen(LONGLONG llPostID)									 override;
    virtual int PostGetInfo(int iRecverNum, int &cntAllPost, int &cntNewPost) override;
    virtual int PostGetData(int iRecverNum, std::vector<SAPOST>& vPost, int &nRowCount, DWORD dwRecverUserNum) override;
    virtual int PostGetState(int iRecverNum, std::vector<SAPOSTSTATE>& vPost, int &nRowCount) override;
    virtual int PostGetSendState(int iRecverNum, std::vector<SAPOSTSTATE>& vPost, int &nRowCount) override;
    virtual int PostGetRecvState(int iRecverNum, std::vector<SAPOSTSTATE>& vPost, int &nRowCount) override;
    virtual int GetChaNum(const char *szCharName, int *pnChaNum, int *pnUserNum) override;

    virtual int DelCharacter(int nUsrNum, int nChaNum, const char* szPass2)	 override;
    int DeleteCharacter(int nUsrNum, int nChaNum);

    virtual int DaumDelCharacter(int nUsrNum, int nChaNum, const char* szPass2)					 override;
	virtual int	IdnDelCharacter(int nUsrNum, int nChaNum)					 override;
    virtual int GspDelCharacter(int nUsrNum, int nChaNum)					 override;
    virtual int TerraDelCharacter(int nUsrNum, int nChaNum)					 override;
    virtual int ExciteDelCharacter(int nUsrNum, int nChaNum)				 override;
    virtual int JapanDelCharacter(int nUsrNum, int nChaNum)					 override;
    virtual int GsDelCharacter(int nUsrNum, int nChaNum, const char* szPass2)					 override;
    //virtual int GetChaBAInfo(int nUsrNum, int nSvrGrp, NET_CHA_BBA_INFO_DH* ncbi) override;
    virtual int GetChaBAInfo(int UserNum, int SvrGrp, std::vector<int>& vChaNum) override;
    virtual int GetUserTempInfo(int nUserNum )								 override;
    virtual int GetChaBInfo(int nUserNum, int nChaNum, SCHARINFO_LOBBY* sci) override;
	virtual int GetChaBInfoForMigration( int nUserNum, int nChaNum, SCHARINFO_LOBBY* sci ) override;
    virtual int GetChaBInfoAll(int UserDbNum, int ServerGroup, std::vector<SCHARINFO_LOBBY>& vCharInfo) override;
	virtual int GetChaBInfoAllForMigration( int UserDbNum, int ServerGroup, std::vector< SCHARINFO_LOBBY >& vCharInfo ) override;
    virtual int GetCharacterInfo(int nUserNumber, int nChaNum, GLCHARAG_DATA* pChaData) override;
	virtual int SetCharacterOption( int ChaNum, int CharacterOption ) override;
	virtual int GetCharacterOption( int ChaNum ) override;
    virtual int GetCharacterInfo(int nUserNumber, int nChaNum, SCHARDATA2* pChaData2) override;
    //virtual int GetCharacterInfo(int nUserNumber, int nChaNum, GLCHARAG_DATA* pChaData, bool& bNewCha) override;
    //virtual int GetCharacterInfo( int nUserNumber, int nChaNum, SCHARDATA2* pChaData2 ) override;
	virtual int Certification2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) override; // 2차비번
	virtual int Update2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) override; // 2차비번

	virtual int GetUserInvenOption( int UserNum, int& Option ) override;

    int GetCharacterInfoBase      (int nUserNumber, int nChaNum, SCHARDATA2* pChaData2);
	int GetCharacterInfoStats     (int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoSkill     (int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoSkillQSlot(int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoActionSlot(int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoQuest     (int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoPutOnItems(int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoInven     (int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoVTAddInven(int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoCoolTime  (int nChaNum, SCHARDATA2* pChaData2);
    int GetCharacterInfoSkillFact (int nChaNum, SCHARDATA2* pChaData2);
	int GetCharacterInfoHireSummon(int nChaNum, SCHARDATA2* pChaData2);
	int GetCharacterInfoSlots     (int nChaNum, SCHARDATA2* pChaData2);

	virtual int GetCharacterItemTransferTarget( int UserNum, std::vector< DWORD >& vecChaNum ) override;

	virtual int GetinfofromCharDb(const char *szCharName, int *pnChaNum, int *pnUserNum, int *pClass, int *Lv) override;

	int GetCharacterInfoQuestNew( int nChaNum, SCHARDATA2* pChaData2 );

    // 개인상점 위탁판매 ( 란포인트 거래 ) 추가됨.
    int GetCharacterInfoConsignmentSlotExpansionDate(int nChaNum, SCHARDATA2* pChaData2);

    //! 베트남 탐직방지 시스템의 추가 정보 불러오는 부분 추가
    int GetVTCharInfo(int nChaNum, SCHARDATA2* pChaData2);
    //! 대만 해킹 문제 관련해서 최근 로그아웃 했을때의 정보를 DB에서 읽어옴.
    int GetLastCharInfo(int nUserNum, int nChaNum, SCHARDATA2* pChaData2);

	virtual int	GetCharacterInfoExtraWithAdo(int nUserNumber, int nChaNum, SCHARDATA2* pCharData2) override;
    virtual int GetChaFriend(int nChaNum, std::vector<CHA_FRIEND> &vecFriend) override;
    virtual int SetCharacterOnline(int nChaNum)								 override;
    virtual int SetCharacterOffline(int nChaNum)							 override;
    virtual int GetChaFriendSMS(int nChaNum, std::vector<CHA_FRIEND_SMS> &vFriend) override;

    //UserActions
    virtual int GetUserType(int nUserNum)				 override;
	virtual int GetUserCountryInfo ( IN int nUserNum, OUT Country::SCOUNTRY_INFO& sCountryInfo )		override;
    virtual int UserTestChaNumDecrease(int nUserNum)	 override;
    virtual int UserChaNumDecrease(int nUserNum)		 override;
    virtual int GetChaGuildNum(DWORD nChaNum)			 override;
    virtual int UserTestChaNumIncrease(int nUserNum)	 override;
    virtual int UserUpdateCha(int nUsrNum, const TCHAR* szChaName) override;
    virtual int UserLogoutSimple(const TCHAR* szUsrID)	 override;
    virtual int UserLogoutSimple2(int nUserNum)			 override;
    virtual int UserLogout(const TCHAR* szUsrID, 
		const TCHAR* szUsrIP, 
        int nUsrNum, 
        int nGameTime,
        int nChaNum,
        int nSvrGrp,
        int nSvrNum, 
        int nTotalGameTime,
        int nOfflineTime )			 override;
    virtual int ThaiUserCheck(const TCHAR* szUsrID, const TCHAR* szPasswd, const TCHAR* szUsrIP, int nSvrGrp, int nSvrNum) override;
    virtual int ThaiGetUserInfo(const TCHAR* szUserID, NET_LOGIN_FEEDBACK_DATA2* pMsg) override;
    virtual int UserCheck(
        const TCHAR* szUsrID, 
        const TCHAR* szPasswd, 
        const TCHAR* szUsrIP,
        const TCHAR* szRandomPasswd,
        int nRandomNum,
        int nSvrGrp,
        int nSvrNum) override;

	virtual int IdnUserCheck(int nUserNum, const std::string &strId, const std::string &strIP, int nSvrGrp, int nSvrNum) override;
    
    virtual int GetUserInfo(const TCHAR* szUserID,  NET_LOGIN_FEEDBACK_DATA2* pMsg)		override;

	virtual int IdnGetUserInfo(const std::string &strId, NET_LOGIN_FEEDBACK_DATA2* pMsg)	override;

    virtual USER_INFO_BASIC GetUserInfo(const TCHAR* szUsrId, const TCHAR* szPasswd)	override;
    
    virtual int DaumUserCheck(
        TCHAR* szDaumGID,
        const TCHAR* szUUID,
        const TCHAR* szUserIP,
        int nSvrGrp,
        int nSvrNum,
        int& Age OUT) override;

    virtual int DaumGetUserInfo(const TCHAR* szGID, DAUM_NET_LOGIN_FEEDBACK_DATA2* pMsg) override;

    virtual int GspUserCheck(TCHAR* szGspUserID,
        const TCHAR* szUUID,
        const TCHAR* szUserIP,
        int nSvrGrp,
        int nSvrNum) override;

    virtual int GspGetUserInfo(const TCHAR* szUserID,
        GSP_NET_LOGIN_FEEDBACK_DATA2* pMsg) override;
    virtual int TerraUserCheck(TCHAR* szTerraLoginName,
        const TCHAR* szTerraDecodedTID,
        const TCHAR* szUsrIP,
        int nSvrGrp,
        int nSvrNum )				 override;
    virtual int TerraGetUserInfo(const TCHAR* szTLoginName, 
        TERRA_NET_LOGIN_FEEDBACK_DATA2* pMsg) override;
    virtual int GsUserCheck(const TCHAR* szUsrID,
        const TCHAR* szPasswd,
        const TCHAR* szUsrIP,
        int nSvrGrp,
        int nSvrNum)				 override;
    virtual int GsGetUserInfo(const TCHAR* szUserID,
        GS_NET_LOGIN_FEEDBACK_DATA2* pMsg) override;
	virtual int GmUserCheck( const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		int nSvrGrp,
		int nSvrNum )				 override;
	virtual int GmGetUserInfo( const TCHAR* szUserID,
		GM_NET_LOGIN_FEEDBACK_DATA2* pMsg ) override;
	virtual int UsUserCheck( const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		int nSvrGrp,
		int nSvrNum )				 override;
	virtual int UsGetUserInfo( const TCHAR* szUserID,
		US_NET_LOGIN_FEEDBACK_DATA2* pMsg ) override;
    virtual int ExciteUserCheck(
        const TCHAR* szUserID,
        const TCHAR* szUserIP, 
        int nSvrGrp, 
        int nSvrNum)				 override;
    virtual int ExciteGetUserInfo(
        const TCHAR* szUserID,
        EXCITE_NET_LOGIN_FEEDBACK_DATA2* pMsg) override;
    virtual int JapanUserCheck(
        const TCHAR* szUserID,
        const TCHAR* szUserIP, 
        int nSvrGrp, 
        int nSvrNum,
        int nUserNum)				 override;
    virtual int JapanGetUserInfo(const TCHAR* szUserID, JAPAN_NET_LOGIN_FEEDBACK_DATA2* pMsg) override;
	
    virtual int WorldBattleUserCheck(
		const std::string& UserId,
		const std::string& UserPass,
		const std::string& UserIp,
		int ServerGroup,
		int ServerNumber) override;
    
    virtual int WorldBattleGetUserInfo(const std::string& UserId, NET_LOGIN_FEEDBACK_DATA2& Msg) override;
    //int ExtreamClassCheck(int UserDbNum, int& ExtreamM, int& ExtreamW);

    virtual int UserLogoutSvr(int nSvrGrp, int nSvrNum)	 override;
    virtual int UserBlock(int nUserNum, int nDay )		 override;
    virtual int UserBlockDate(int nUserNum, __time64_t tBlockTime) override;

    virtual WORD ThaiGetTodayPlayTime(int nUserNum ) override;
    virtual int UserLogoutSimple3(const TCHAR* szUsrID, int nSvrGrp) override;
    virtual int UserDbLogItemDataInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName) override;

    //LogActions
    virtual int AddLogServerState(const __time64_t& LogDate, int nSGNum, int nSvrNum, int nUserNum, int nUserMax) override;
    //virtual int	LogLottery(CString strLottery, CString strIP, CString strUserUID, int nUserNum) override;
    //virtual int	LogPungPungWrite(CString strIP, CString strUserUID, int nUserNum, int nPrize) override;
    //virtual int	LogPungPungCheck(CString strIP, int nUserNum) override;
    virtual int	AllUserLogout() override;

    //
    //mjeon.sns
    //
    virtual int SNSAuthedFB(SFACEBOOK &AuthFB) override;
    virtual int SNSUnAuthedFB(int nChaNum) override;
    virtual int SNSGetInfoFB(SFACEBOOK &AuthFB) override;
    virtual int SNSGetUidFB(int nTargetChaNum, std::string &m_UID) override;

    virtual int SNSAuthedTW(STWITTER &AuthTW) override;
    virtual int SNSUnAuthedTW(int nChaNum) override;
    virtual int SNSGetInfoTW(STWITTER &AuthTW) override;
    virtual int SNSGetUidTW(int nTargetChaNum, std::string &m_UID) override;

    virtual int SNSGetInfo(SFACEBOOK &AuthFB, STWITTER &AuthTW) override;

		
	//
	//mjeon.attendance
	//
	virtual int AttendanceLoad(OUT AttendanceManager &AttendanceMan, UINT nChaNum)		override;
	virtual int AttendanceSave(IN  AttendanceManager &AttendanceMan, UINT nChaNum, LONGLONG nChaCurExp)		override;
	virtual int AttendanceMaintenance()													override;
    virtual int AttendanceLogInsert(
        int ChaNum,
        int Type,
        const std::string& LoadType,
        int DayN,
        int Status) override;
    virtual int AttendanceTaskInsert(
        int ChaNum, LONGLONG ChaCurExp, int TaskN, int Type, int Status, int Value,
        int ValueBase, int RewardPoint, int RewardExp, int RewardMoney, int RewardItems,
        int RewardItem1, int RewardItem2, int RewardItem3, int TookRewards) override;

	//
	//mjeon.activity
	//
	virtual int ActivityGetClosedActivityID(OUT ActivityBaseData &abd)					override;
	virtual int ActivityLoad(MMACTIVITIES &mmActivities, UINT nChaNum)					override;
	virtual int ActivitySave(MMACTIVITIES &mmActivities, UINT nChaNum, UINT nChaLevel)	override;
	virtual int ActivityOrdering(Activity &actv, UINT nChaNum, UINT nChaLevel)			override;
	virtual int ActivityGetPoint(OUT LONGLONG *pllRewardPoint, UINT nChaNum)			override;
	virtual int ActivityLoadBadge(UINT nChaNum, TitleManager &TitleMan)					override;
	virtual int ActivitySetPoint(IN LONGLONG *pllRewardPoint, UINT nChaNum)				override;
	virtual int ActivitySaveBadge(UINT nChaNum, TitleManager &TitleMan)					override;

	//
	// Provide Reward System
	//
	virtual int ProvideRewardInitialize ( )																	override;
	virtual int ProvideRewardInsert ( const ProvideRewardSystem::PROVIDE_REWARD_DATA& _ProvideRewardData )	override;
	virtual int ProvideRewardSelect ( DWORD _dwChaDBNum, DWORD _dwServerGroupNum, OUT ProvideRewardSystem::VEC_PROVIDE_REWARD& _vecProvideRewardData )													override;
	virtual int ProvideRewardClearDeleteFlag ( )															override;
	virtual int ProvideRewardComplete ( DWORD _dwUniqueID )													override;

	//
	// Country System
	//
	virtual int UpdateUserCountry ( IN DWORD dwUserID, IN Country::SCOUNTRY_INFO& sCountryInfo )			override;
	virtual int LoadVictoriousCountry ( OUT Country::VCOUNTRY_VEC& vecVCountry )							override;
	virtual int AddVictoriousCountry ( IN Country::SVICTORIOUS_COUNTRY& sVictoriousCountry )				override;
	virtual int ResetVictoriousCountry ( IN Country::EMCOUNTRY_SYSTEM_FLAG emFlag )							override;
	virtual int ConfirmStateVictoriousCountry ( IN Country::EMCOUNTRY_SYSTEM_FLAG emFlag, OUT Country::VCOUNTRY_VEC& vecVCountry )							override;

	//
	// Lotto System
	//
	virtual int LottoSystemLoad( unsigned int systemId )								override;

	virtual int LottoDrawingMachineNumSelect(
		unsigned int lottoNum, unsigned int systemId )									override;
	virtual int LottoSystemCurTurnSelect( unsigned int lottoSystemId )					override;
	virtual int LottoAccumulateMoneySelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override;
	virtual int LottoWinNumSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override;
	virtual int LottoCharBuyListSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override;
	virtual int LottoCharNonConfirmBuyListSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int systemId, int confirmTicket )						override;
	virtual int LottoWinManListSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override;
	virtual int LottoInventWinNumSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override;

	virtual int LottoDrawingMachineNumSave(
		unsigned int lottoNum,
		unsigned int accumulateCount,
		unsigned int addCount, unsigned int systemId, bool bUpdate = true )				override;
	virtual int LottoSystemTurnSave( unsigned int turnNum, unsigned int systemId )		override;

	virtual int LottoCharBuyListInsert(
		LONGLONG accumulateMoney,
		LONGLONG charge,
		int buyType, const std::string& drawingNum,
		LONGLONG drawingNumInt, int confirmTicket,
		DWORD dwUserNum, DWORD dwDbNum, DWORD dwServerGroup, char* szChaName, unsigned int turnNum, unsigned int systemId )					override;

	virtual int LottoWinManListUpdate(
		unsigned int ranking, LONGLONG winMoney, SNATIVEID sItemID, DWORD dwItemCount,
		unsigned int turnNum, unsigned int systemId )									override;
	virtual int LottoWinManListInsert(
		unsigned int ranking, LONGLONG winNumInt,
		LONGLONG bonusNumInt, unsigned int systemId )									override;
	virtual int LottoAccumulateMoneyUpdate(
		LONGLONG accumulateMoney,
		unsigned int turnNum, unsigned int systemId )									override;
	virtual int LottoWinNumUpdate(
		const std::string& winNum, LONGLONG winNumInt,
		LONGLONG bonusNumInt, unsigned int turnNum, unsigned int systemId )				override;
	virtual int LottoCharNonConfirmBuyListUpdate(
		DWORD dwDbNum, DWORD dwServerGroup, LONGLONG invenMoney, unsigned int systemId )						override;

	virtual int LottoSystemNextTurn( unsigned int systemId )							override;
	virtual int LottoConfirmWinMan( unsigned int systemId )								override;
	virtual int LottoChangeInventWinNum( unsigned int turnNum, unsigned int systemId )	override;
	virtual int LottoWinMoneyUpdate( unsigned int turnNum, unsigned int systemId )		override;

	virtual int LottoSystemBuyLog( const LottoSystem::LOTTO_SYSTEM_BUY_LOG& logInfo )	override;
	virtual int LottoSystemWinManLog( unsigned int turnNum, unsigned int systemId )		override;
	virtual int LottoSystemGiveWinTicketLog( unsigned int systemId,
		DWORD dwTicketID, LONGLONG invenMoney )											override;
	virtual int LottoSystemTurnInfoLog(
		const LottoSystem::LOTTO_SYSTEM_TURN_LOG& logInfo )								override;

    //! -----------------------------------------------------------------------
    //! PointShop
    int PointShopUpdateDate(__time64_t& UpdateTime, bool bStandAlone);
    virtual int PointShopList(__time64_t& UpdateTime, std::vector<pointshop::SellItem>& ItemList, bool bStandAlone) override;

    //! Character 의 PointShop Point 를 가져온다
    //! ChaDbNum : 캐릭터 DB 번호
    //! PointRefundable : 환불가능 포인트
    //! PointNotRefundable : 환불불가 포인트
    virtual int GetChaPoint(int ChaDbNum, __int64& PointRefundable, __int64& PointNotRefundable) override;

    //! Character 의 PointShop Point 를 설정한다
    //! ChaDbNum : 캐릭터 DB 번호
    //! PointRefundable : 환불가능 포인트
    //! PointNotRefundable : 환불불가 포인트
    virtual int SetChaPoint(int ChaDbNum, const __int64& PointRefundable, const __int64& PointNotRefundable) override;
    virtual int LogPointShopInsert(DWORD ChaDbNum, const std::string& LogMessage) override;
	virtual int LogPointShopInsert(
		DWORD ChaDbNum, int LogType, 
		__int64& BeforeRefundable, 
		__int64& BeforeNotRefundable, 
		__int64& AfterRefundable, 
		__int64& AfterNotRefundable, 
		WORD ItemMid, 
		WORD ItemSid) override;
    //! Billing 관련 Shop, Item 판매 정보 로그
    virtual int ShopSaleLog(
        const SNATIVEID& ItemId,
        __int64 Price,
        EM_BILLING_SELL_TYPE Type,
        int UserDbNum,
        int ServerNum,
        int ChaDbNum,
        int ChaLevel) override;

	//mjeon.CaptureTheField
	virtual int CTFStatLog(UINT *arrPlayerNum, UINT state)										override;
	virtual int CTFResultLog(WORD wSchoolAuthed, CTF_REWARD_BUFF *arrReward, UINT nHowMany)		override;
	virtual int CTFPlayLog(UINT nChaNum, WORD wSchool, int nTotalPoint, int nRewardPoint)		override;
	virtual int GetCTFInfo(UINT rowcount ,std::vector<DWORD >& vAuthSchool)						override;
	virtual int LoadChaExtraInfo(UINT nChaNum, int &nContributionPoint)							override;
	virtual int SaveChaExtraInfo(UINT nChaNum, int nContributionPoint)							override;

    virtual int LogItemListInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName) override;
	virtual int LogQuestListInsertUpdate( int Mid, int Sid, const std::string& Name ) override;
	virtual int LogSkillListInsertUpdate( int Mid, int Sid, const std::string& Name, int Grade ) override;

	virtual int LogGmCommandInsert( int ChaNum, const std::string& Command, const std::string& Param ) override;

	virtual int LogGMChat( int nChatType, int nGMType, const std::string& SendName, const std::string& RecvName, const std::string& ChatMsg ) override;

    virtual int UserCountryUpdate(int UserDbNum, wb::EM_COUNTRY Country) override;

    virtual int ExecuteSqlLogDb(const std::string& SqlStr) override;

    virtual int LogMiniGameOddEvenInsert(
        DWORD UserDbNum,
        DWORD CharDbNum,        
        BYTE  CurrentRound,
        BYTE  WinLost,
        __int64 BattingMoney,
        __int64 DividendMoney,
        __int64 ChaMoney);

    virtual int ClubNickUpdate(DWORD ChaDbNum, const std::string& NickName) override;
    virtual int ClubPublicFlagUpdate(DWORD ClubDbNum, DWORD Flag) override;
    virtual int ClubAutoKickUpdate(DWORD ClubDbNum, size_t GradeIndex, WORD Day, bool AutoKickOut) override;
    virtual int ClubNewbieNoticeUpdate(DWORD ClubDbNum, const std::string& Notice) override;
    virtual int ChaClubSecedeTimeReset(DWORD ChaDbNum) override;

    virtual int ChaFriendGroupSelect(int ChaDbNum, std::vector<FRIEND_GROUP>& vGroup) override;
    virtual int ChaFriendGroupInsert(int ChaDbNum, WORD GroupNum, const std::string& GroupName) override;
    virtual int ChaFriendGroupUpdate(int ChaDbNum, WORD GroupNum, const std::string& GroupName) override;
    virtual int ChaFriendGroupDelete(int ChaDbNum, WORD GroupNum) override;
    virtual int ChaFriendGroupSet(int ChaP, int ChaS, WORD GroupNum) override;

    virtual int NotifySelect(int ChaDbNum, std::vector<std::tr1::shared_ptr<gamelogic::NotifyBase> >& vNotify) override;
    virtual int NotifyInsert(int ChaDbNum, gamelogic::notify::NOTIFY_TYPE Type, sc::MGUID& Guid, const std::string& Data) override;
    virtual int NotifyDelete(sc::MGUID& Guid) override;

    //////////////////////////////////////////////////////////////////////////
    // 인던로그 InstantDungeon Log
    
    // 로그 타입을 정의
    virtual int InstantDungeonLogTypeInsert( 
		const db::EM_INSTANT_LOG_TYPE& nLogType,
        const char* strParamName1=0,  const char* strParamName2=0,  const char* strParamName3=0,  const char* strParamName4=0,  const char* strParamName5=0,
        const char* strParamName6=0,  const char* strParamName7=0,  const char* strParamName8=0,  const char* strParamName9=0,  const char* strParamName10=0,
        const char* strParamName11=0, const char* strParamName12=0, const char* strParamName13=0, const char* strParamName14=0, const char* strParamName15=0,
        const char* strParamName16=0, const char* strParamName17=0, const char* strParamName18=0, const char* strParamName19=0, const char* strParamName20=0 ) override;

    // 실제 로그 입력
    virtual int InstantDungeonLogInsert( 
		const db::EM_INSTANT_LOG_TYPE& nLogType,
        const char* strParam1=0,  const char* strParam2=0,  const char* strParam3=0,  const char* strParam4=0,  const char* strParam5=0,
        const char* strParam6=0,  const char* strParam7=0,  const char* strParam8=0,  const char* strParam9=0,  const char* strParam10=0,
        const char* strParam11=0, const char* strParam12=0, const char* strParam13=0, const char* strParam14=0, const char* strParam15=0,
        const char* strParam16=0, const char* strParam17=0, const char* strParam18=0, const char* strParam19=0, const char* strParam20=0 ) override;


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
        ) override ;

    // 전장 점수 저장;
    virtual int CompetitionRatingUpdate(
        const std::string& strContentType,
        const DWORD charDbNum,
        const int Rating,
        const int WinningStreak,
        const int PlayCount,
        const __time64_t tLastPlayDate
        ) override;

    // 전장 시즌 업데이트;
    virtual int CompetitionSeasonUpdate(
        const std::string& strContentType,
        const int seasonNum,
        const int baseRating
        ) override;

    // 전장 로그 CompetitionLog



	/////////////////////////////////////////////////
	// 인증 서버 로그 기록
	virtual int AddAuthServerState( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, int nCurUser, int nMaxUser, const __time64_t& LogDate ) override;
	// 인증 절차 수행
	virtual int ProcessCertificationForAuth( int isSessionSvr, int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::string strUniqKey, int &certification, std::string &strNewUniqKey, int &nSessionSvrID ) override;
	// 이벤트 정보 기록
	virtual int AddAuthEventInfo( G_AUTH_EVENT_INFO *eventInfo ) override;

	virtual int AddAuthServerConnectionState( int nSessionSvrID, int nCounrty, int nServerType, const TCHAR* szIP, int nPort ) override;
	virtual int AddAuthServerConnectionStateInit() override;
	virtual int AuthCountryAccessApproveSelect( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::vector<int>& vecIndex OUT ) override;


	virtual int TexasHoldemChipMapSelect( std::vector< TexasHoldem::sChipPrice >& vecChip ) override;
	virtual int TexasHoldemChipMapVersionSelect( LONGLONG& Version ) override;
	virtual int	TexasHoldemChipSelect( SCHARDATA2* pChaData2 ) override;
	virtual int	TexasHoldemChipUpdate( DWORD dwChaDbNum, LONGLONG& chip, bool bPlus = false ) override;
	virtual int	TexasHoldemChipLogInsert( DWORD dwChaDbNum, LONGLONG& chip, LONGLONG& GameMoney, LONGLONG& Point ) override;
	virtual int	TexasHoldemChipSelectFromWeb( int nServerGroup, DWORD dwChaDbNum, LONGLONG& chip ) override;
	virtual int	TexasHoldemAuthKeyInsert( DWORD dwChaDbNum, const std::string& strGUID ) override;
	virtual int	TexasHoldemGameTimeInsert( DWORD dwChaDbNum, DWORD dwUserNum, int nSecond ) override;


    //////////////////////////////////////////////////////////////////////////
    // RanPoint Trade 란포인트 거래 ( 위탁판매 ) ConsignmentsSale

    //! ITEM ID를 통해 아이템을 가지고 온다. ( bDateCheck => 날짜(Date)검사 여부 )
    virtual int ConsignmentsSaleGet( sc::MGUID& Guid, private_market::ConsignmentSaleItem& SaleItem, bool bDateCheck = false ) override;
    //! ChaNum을 넣으면 캐릭터의 물품을 전부 갖고오고 ChaNum을 넣지 않으면 서버에 위탁판매 시킬 물품을 갖고 온다.
    virtual int ConsignmentsSaleSelect( private_market::SALE_ITEMS_VECTOR& vecItem, const DWORD& dwChaNum = 0, const int& ServerGroup = 0 ) override;
    //! 남은 갯수를 바꾼다.
    virtual int ConsignmentsUpdateTurnNum( sc::MGUID& Guid, int nTurnNum ) override;
    virtual int ConsignmentsSaleInsert( private_market::ConsignmentSaleItem& SaleItem ) override;
    virtual int ConsignmentsSaleDelete( sc::MGUID& Guid ) override;
    virtual int	ConfignmentSaleSlotExpansionDateSelect( SCHARDATA2* pChaData2 ) override;
    virtual int	ConfignmentSaleSlotExpansionDateSet( DWORD dwChaDbNum, __time64_t tExpansionDate ) override;
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
		WORD		wRemainingTime ) override ;

	virtual int MacroLogInsert(
		DWORD dwChaDbNum,
		WORD  wState) override;

	virtual int ItemPeriodExLogInsert(DWORD  chaNum
		, WORD itemMid
		, WORD itemSid	
		, WORD remainNum
		, __time64_t previousPeriod
		, __time64_t currentPeriod) override;


    // 유저 정보 수집
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
		int languageCode ) override;

	virtual int	GetChaResetInfo( DWORD dwChaDbNum ) override;

	// 필리핀 유저이전
	virtual int	IsMigrationTarget( DWORD dwChaDbNum ) override;
	virtual int GetCharacterInfoForMigration( int nUserNumber, int nChaNum, SCHARDATA2* pChaData2 ) override;
	int GetCharacterInfoQuestForMigration( int nChaNum, SCHARDATA2* pChaData2 );
	int GetCharacterInfoPutOnItemsForMigration( int nChaNum, SCHARDATA2* pChaData2 );
	int GetCharacterInfoInvenForMigration( int nChaNum, SCHARDATA2* pChaData2 );
	virtual int SetMigrationTarget( DWORD dwChaDbNum ) override;


	//////////////////////////////////////////////////////////////////////////
	//
	// TOURNAMENT BETTING SYSTEM
	//
	virtual int TournamentBettingSelect( DWORD ClientDBid, DWORD BettingStatus )								override;
	virtual int TournamentBettingInsert( DWORD ChaNum,LONGLONG BettingCount, DWORD BettingGameSciptid,DWORD BettingGroup)override;
	virtual int TournamentBettingUpdate( DWORD ChaNum,DWORD BettingGameSciptid,DWORD BettingStatus)override;
	virtual int TournamentBettingDelete( DWORD ChaNum)override;

	//////////////////////////////////////////////////////////////////////////
	//
	// MATCHING SYSTEM
	//

	virtual int MatchingOnload( std::string Matching_Content_Type,
		std::string Betting_Script,
		std::string Room_Admin_Script,
		std::string Matching_Script,
		std::string Lobby_Field_Manager )override;
	virtual int MatchingGroup( DWORD GroupID,
		DWORD GameScriptID,
		bool  IsDeleted)override;
	virtual int MatchingGroupUser( DWORD GroupID,
		DWORD GameScriptID,
		DWORD UserDBNum,
		bool  IsDeleted)override;
	virtual int MatchingQueueIn( DWORD ClientID,
		DWORD GameScriptID,
		bool  IsDeleted)override;
	virtual int MatchingInstanceIn( DWORD GroupID,
		DWORD GameScriptID,
		DWORD  MatchingScriptID)override;
	virtual int MatchingInstanceJoin( DWORD GroupID,
		DWORD MapID,
		DWORD GameScriptID,
		DWORD  MatchingScriptID)override;
	virtual int MatchingInstanceOnStart( DWORD GroupReadySize,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override;
	virtual int MatchingInstanceOut( DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override;
	virtual int MatchingInstanceResualt( DWORD Faction,
		bool IsLose,
		DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override;
	virtual int MatchingRoomDestroy( DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override;
	virtual int MatchingRoomReq( DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override;
	virtual int MatchingFinalWinner( DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override;
	virtual int MatchingInstanceTournament( int   tournamentPosition,
		std::string GroupName,
		int   GroupIndexPosition,
		int   GroupSize,
		int   GroupMaxSize,
		int   GroupHeadLevel,
		DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)override;

	virtual int MatchingJoinCountLoad ( MatchingSystem::MAP_CHAR_JOIN_COUNT_LIST& _refMapCharJoinCountList ) override;
	virtual int MatchingJoinCountReset ( ) override;
	virtual int MatchingJoinCountUpdate ( const DWORD _dwCharDBNum, const MatchingSystem::SJOIN_COUNT _sJoinCount ) override;


    // GM LOG ( GM 아이템 넣어주기 )
    virtual int WriteGMLog(int nGmNum, std::string& strCmd, std::string& strUserIP) override;

	// 란모바일
	virtual int RanMobileRequest(DWORD dwUserDBNum, DWORD dwChaDBNum, int nServerNum, int nCommand, int nValue, const std::string& strChaName, int nChaClass ) override;

	// Log Money;
	virtual int MoneyLogInsert ( const int _nMainType, const int _nSubType, const LONGLONG _llMoney ) override;

	//////////////////////////////////////////////////////////////////////////
	// 기간제 아이템 시간 연장
	virtual int InspectionRewardPolicySelect() override;
	virtual int InspectionRewardPolicyInsert( const __time64_t tStart, const __time64_t tEnd, const int nRewordMinute ) override;
	virtual int InspectionRewardGetMinute( SCHARDATA2* pChaData2 ) override;

	virtual int MoneyNotifyInsert( DWORD dwChaDbNum, __time64_t tDate, LONGLONG llOldMoney, LONGLONG llNewMoney, int nCause, int nChannel, int nField ) override;

	// PostRenew
	virtual int PostRnSelectAll( std::vector<PostInfo>& vecList OUT, DWORD dwRecvChaNum ) override;
	virtual int PostRnSetRead( DWORD dwPostID )		override;
	virtual int PostRnDelAttach( DWORD dwPostID )	override;
	virtual int PostRnDel( DWORD dwPostID )			override;
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
		std::string strContent ) override;
	virtual int PostRnReturn( DWORD dwPostID, DWORD dwReturnChaNum, std::string strReturnChaName, DWORD dwReturnUserNum, DWORD dwRecvChaNum  )	override;
	virtual int PostCharacterCheck(DWORD dwRecvChaDbNum) override;
	
	
	virtual int TransDBDataToWorldBattle(SCHARDATA2* pCharData2, DWORD dwChaNum) override;
	virtual int CalculateExpCompressor( DWORD dwChaDbNum, __int64 nCurExp, __int64 nCompressedExp ) override;
	// item
	virtual int ItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) override;
	virtual int DeletedItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) override;
	virtual int ItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) override;
	virtual int EntireItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) override;
	virtual int ItemDelete( sc::MGUID& Guid, int State, int CallType ) override;
	virtual int CompletelyItemDelete( sc::MGUID& Guid, int State, int CallType ) override;
	virtual int ItemSaleToNpc( sc::MGUID& Guid, int State, int CallType ) override;
	virtual int ItemSelect( DWORD ChaDbNum, INVEN_TYPE InvenType, std::vector< SINVENITEM_SAVE >& vecItems ) override;
	virtual int ItemSelect( DWORD ChaDbNum, INVEN_TYPE InvenType, std::multimap< DWORD, SINVENITEM_SAVE >& mapItems ) override;
	virtual int GetPutonItems( DWORD ChaDbNum, SCHARDATA2* pChaData2 ) override;
	virtual int GetInvenItems( DWORD ChaDbNum, SCHARDATA2* pChaData2 ) override;
	virtual int GetLockerItems( DWORD UserNum, SCHARDATA2* pChaData2 ) override;
	virtual int SavePetInven( int nChaNum, int nPetNum, std::vector< SINVENITEM_SAVE >& vecItems ) override;
	virtual int SaveVehicleInven( int nVehicle, int nCharNum, std::vector< SINVENITEM_SAVE >& vecItems ) override;
	virtual int GetClubLockerItems( DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems ) override;
	virtual int SaveClubLockerItems( DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems ) override;

	virtual int TransferItems( DWORD UserNum, std::vector< DWORD >& vecChaNum ) override;
	virtual int TransferClubLockerItems( DWORD ClubNum = 0 ) override;
	virtual int GetGuildItemTransferStatus( DWORD ClubNum, int& Result, int& MigrationState ) override;
	virtual int TransferLockerItems( DWORD UserNum = 0 ) override;
	virtual int GetChaNumToTransfer( std::vector< DWORD >& vecChaNum ) override;
	virtual int TransferCharacterItems( DWORD ChaDbNum = 0 ) override;

	virtual int ItemUpdatePos( DWORD ChaDbNum, INVEN_TYPE InvenType, sc::MGUID& Guid, WORD PosX, WORD PosY, int CallType ) override;
	virtual int ItemUpdateTurnNum( sc::MGUID& Guid, WORD TurnNum, int CallType ) override;
	virtual int ItemInsertError( sc::MGUID& Guid, int CallType, __time64_t& Date ) override;

	virtual int GetItemTransferStatus( DWORD UserNum, int& Result ) override;

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
                                          ) override;

}; // class AdoManager

} // namespace db
#endif // _DB_ADO_MANAGER_H_
