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
    virtual bool Reconnect() override; //! ������ üũ
    virtual bool CheckHdd() override; //! Hard disk �뷮 üũ
    virtual bool IsAllDbJobCompleted() override { return true; }
    virtual void WaitAllDbJobCompleted(DWORD WaitLimitTime) override;

public:
    //! -----------------------------------------------------------------------
    //! ȣȯ�� ������ ���ؼ� �ӽ÷�...
    
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
    //! �����̽þ� PC�� �̺�Ʈ
    virtual int	OdbcOpenMyDB(const TCHAR* szOdbcName, const TCHAR* szUsrID, const TCHAR* szUsrPasswd, const TCHAR* szDbName, bool UseCustomPool, int nPoolSize = sc::db::DB_POOL_SIZE, int nDBTimeOut = sc::db::DB_RESPONSE_TIME) override;
    
    //! Ado
    virtual bool SetConnectionString(DB_TYPE Type, const std::string& Ip, const std::string& DbName, const std::string& Id, const std::string& Password, int AdoMaxPoolSize, int DatabasePacketSize) override;

    //! -----------------------------------------------------------------------

    //! ���� üũ
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
	
	//DbManager�� �Ϲ������� ���Ǵ� �����Լ��� �ƴϴ�. ...ExtraWithAdo�� ���� ȣ��Ǵ� �Լ�.
	//virtual int SaveProductBook(UINT nChaNum, ProductBookContainer& setLearnProductBook);
	//virtual int LoadProductBook(UINT nChaNum, ProductBookContainer& setLearnProductBook);

    // ����
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
    
    //! �б�vs�б� ��Ƽ��ð������
    virtual int LogPartyMatch(int nSGNum, int nSvrNum, WORD wWin, WORD wLost) override;

    virtual LONGLONG GetItemMaxNum(int nSGNum, int nSvrNum,	int nFldNum, std::vector<VIEWLOGITEMEXCHANGEMAX> &v) override;
    virtual int GetLogItem(std::vector<DWORD>& LogItemData) override;
    virtual int LogItemExchange( const LOG_ITEM_EXCHANGE& sEXCHANGE ) override;
    virtual int LogMoneyExchange( const LOG_MONEY_EXCHANGE& _sLOG )	 override;
	virtual int LogPointShopExchange( int nFromType, DWORD ChaDbNum, int nExchangeFlag, WORD ItemMid, WORD ItemSid, int nPoint, int nExchangeFlagDetail ) override;
    virtual int LogItemConversion(const LOG_ITEM_CONVERSION &_sLOG) override;
    virtual int LogAction(const LOG_ACTION& LogData) override;
//     int LogItemExchangeInsert(
//         // �ֹ�ȣ, �ι�ȣ
//         int	Mid, int Sid,
//         // �����׷�, ������ȣ, �ʵ��ȣ
//         int ServerGroup, int ServerNum, int FieldNum,
//         // ����Ÿ��, ������ȣ
//         int	MakeType, LONGLONG lnMakeNum,
//         // from�� ���� (����, �ɸ���, Ŭ��), ������ �����
//         int FromType, int ItemFrom, 
//         // to�� ���� (����, �ɸ���, Ŭ��), ������ ������
//         int ToType, int ItemTo, 
//         // ��ȯ Ÿ��, ����
//         int ExchangeFlag, int CountNum, 
//         // �ڽ���
//         int CostumeMid, int CostumeSid,
//         // ���� ����
//         short Damage, short Defense, short Fire, short Ice, short Poison, short Electric, short Spirit,        
//         // ��ȯ����
//         LONGLONG TradePrice,
// 		int nBaseMainMapID, int nBaseSubMapID, int nGaeaMainMapID, int nGaeaSubMapID, 
// 		short nOriNIDMain, short nOriNIDSub,
// 		int nExchangeFlagDetail,
//         const __time64_t& LogDate);

    //! ������ �α�
    virtual int LogGuidance(const LOG_GUIDANCE &sLog) override;

    //! CDM �α�
    virtual int LogCDM(const LOG_CDM &sLog) override;

    //! CDM �α�(����) 
    virtual int LogCDMReuslt(const LOG_CDM_RESULT &sLog) override;

    //! Shop ���� ������ �������� �����´�.
    //! \param strUID ����ID
    //! \param &v ��ǰ�� ���� ����
    //! \return 
    virtual int GetPurchaseItem(const CString& strUID, std::vector<SHOPPURCHASE> &v) override;

    //! ������ ������ �� �ִ� ��ǰ���� Ȯ���Ѵ�.
    //! \param strPurKey �����ڵ�
    //! \param nFlag(0 : ����, 1 : ���ſϷ�, 2 : ������ҽ�û, 3 : �������ó��)
    //! \return 1:�������� ���� �̿��ǰ�:�������� ����
    virtual int SetPurchaseItem(const CString& strPurKey, int nFlag=1) override;

    //! ĳ���� ����, ĳ���ͻ��� ���� ����
    virtual int UserChaNumIncrease(int nUserNum) override;

    //! �ش� ��Ŀ ����ð��� ���Ѵ�.
    //! \n 1 �� 5 �� ��Ŀ�� ���� �� ����.
    //! \n 2, 3, 4 �� ��Ŀ�� ����ð��� ���� �� �ִ�.
    //! \param nUserNum ����ڹ�ȣ
    //! \param nStorageNum ���丮����ȣ
    //! \param tTime ����ð�
    //! \return -1:sc::db::DB_ERROR  0:sc::db::DB_OK
    virtual int SetChaStorageDate(int nUserNum, int nStorageNum, __time64_t tTime) override;

    //! �ش� ĳ������ �κ��丮 �߰� �ټ��� �����Ѵ�.
    //! \n ���� 0 ���� ���� �߰��� 1 �� �Է�
    //! \n ���� ���� ���¿��� �� ���� �߰��� 2 �� �Է�(���� �ټ��� �Է��Ѵ�)
    //! \n ���� ���� ���¿��� ���� �߰��� 3 �� �Է�
    //! \n �ִ� 3������ �Է°���.
    //! \param nChaNum ĳ���͹�ȣ
    //! \param wInvenLine �߰��ټ�
    virtual int SetChaInvenNum(int nChaNum, WORD wInvenLine) override;

    //! �ش� ������� �����̾� �Ⱓ�� �����Ѵ�.
    virtual int SetPremiumTime(int nUserNum, __time64_t tPremiumTime) override;

    //! ������ȯ ī�� ����� ���� �������� �����Ѵ�.
    virtual int SetLastCallPos(int nChaNum, DWORD dwMapID, D3DXVECTOR3 vPos) override;    

    //! ���ο� ĳ���͸� �����Ѵ�.
    virtual int	CreateNewCharacter(SCHARDATA2* pCharData2) override;
	virtual int	CreateNewCharacterExtraWithAdo(SCHARDATA2* pCharData2) override;	//mjeon.ado	

    //! ChaInfo Backup.
    virtual int CharacterInfoBackup(SCHARDATA2* pCharData2) override;

    // �븸,ȫ��
    // ĳ���� ������ Temp Table�� ĳ���� ������ �ݾ� ������ ��ȣȭ �ؼ� ������ �д�.
    int	InsertCharInfoTemp(int nChaNum);

    virtual int CreateClub(const std::string& ClubName, DWORD ChaDbNum, wb::EM_COUNTRY Country) override;

    //! Ŭ���� �����Ѵ�.
    //! dwClub : Ŭ����ȣ
    //! dwChaNum : Ŭ���� �����Ϸ��� ĳ���͹�ȣ(ĳ���� ��ȣ�� Ŭ�� �����Ͱ� �ƴϸ� ������ ���� �ʴ´�)
    virtual int DeleteClub(DWORD dwClub, DWORD dwChaNum) override;    

    //! Ŭ����ũ�� �����Ѵ�.
    //! dwClub : Ŭ����ȣ
    //! dwRank : ��ŷ
    virtual int SetClubRank(DWORD dwClub, DWORD dwRank) override;

    virtual int ReadClubMarkImage(DWORD dwClub, se::ByteStream &ByteStream) override;

    virtual int WriteClubMarkImage(DWORD dwClub, DWORD dwMarkVer, BYTE* pData, int nSize) override;

    //! Ŭ�� ��ü�ð��� �����Ѵ�.
    //! \param dwClub Ŭ����ȣ
    //! \param tDiss ��ü�ð�
    virtual int SetClubDissolutionTime(DWORD dwClub, __time64_t tDiss) override;

    //! Ŭ���� ����Ż�� Ȥ�� ����ð��� ���Ѵ�.
    //! \param dwClub Ŭ����ȣ
    //! \param tSec �ð�
    virtual int SetClubAllianceSec(DWORD dwClub, __time64_t tSec) override;

    //! Ŭ���� �����ػ� �ð��� ���Ѵ�.
    //! \param dwClub Ŭ����ȣ
    //! \param tDis �ػ�ð�
    virtual int SetClubAllianceDis(DWORD dwClub, __time64_t tDis) override;

    //! Ŭ���� ������ ���� �ð��� ���Ѵ�.
    //! \param dwClub Ŭ����ȣ
    //! \param tDis ������ ���ӽð�
    virtual int SetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority) override;

    //! ������ ��� Ŭ�������� �����´�
    //! \param vClub : Ŭ������
    virtual int GetClubInfo(std::vector<std::tr1::shared_ptr<GLClub> > &vClub) override;

    //! �ش� Ŭ���� ����� �����´�
    //! \param dwGuild : Ŭ����ȣ
    //! \param vMember : Ŭ�����
    virtual int GetClubMember(DWORD dwClub, std::vector<GLCLUBMEMBER>& vMember) override;

    //! Ŭ���� ���� ������ �����´�.
    virtual int GetClubAuth(DWORD ClubDbNum, std::vector<club::AUTH_GRADE_DB>& vAuth) override;
    //! �ش� Ŭ���� ��� ĳ������ Ŭ�� ������ 0 ���� �����.
    virtual int ResetClubPosition(DWORD ClubDbNum) override;

    //! ���ο� ĳ���͸� Ŭ���� ���Խ�Ų��
    //! \param dwClub : Ŭ����ȣ
    //! \param dwChaNum : ���Խ�Ű���� ĳ���͹�ȣ
    virtual int AddClubMember(DWORD dwClub, DWORD dwChaNum) override;

    //! ���ԵǾ� �ִ� ĳ���͸� Ŭ������ Ż���Ų��
    //! \param dwChaNum : ĳ���͹�ȣ
    virtual int DeleteClubMember(DWORD dwChaNum) override;

	//! ���ԵǾ� �ִ� ĳ���͸� Ŭ������ Ż���Ų��(Ż�� �г�Ƽ X)
	//! \param dwChaNum : ĳ���͹�ȣ
	virtual int DeleteClubMemberWithNoPenalty( DWORD dwChaNum ) override;

    //! Ŭ�� ��Ʋ�� ��û�Ѵ�.
    //! \param dwClubP ��Ŭ����ȣ(Ŭ�� ��Ʋ�� ��û�� ���� �� )
    //! \param dwClubS ���� Ŭ����ȣ(Ŭ�� ��Ʋ�� ��û�� �� Ŭ�� )	
    virtual int SetClubBattle(DWORD dwClubP, DWORD dwClub, DWORD dwEndTime, int nAlliance) override;

    //! Ŭ�� ��Ʋ�� �����Ѵ�.
    //! \param dwClubP ��Ŭ�� ��ȣ(Ŭ�� ��Ʋ�� ��û�� ���� �� )
    //! \param dwClubS ���� Ŭ�� ��ȣ(Ŭ�� ��Ʋ�� ��û�� �� Ŭ�� )
    //! \param nGuFlag ���п���
    //! \param nGuKillNum Ŭ�� ��Ʋ ų��
    virtual int EndClubBattle(DWORD dwClubP, DWORD dwClubS, int nGuFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance) override;


    //! Ŭ�� ��Ʋ������ �����Ѵ�,(Ŭ�� ��Ʋ�� ����Ǳ��� ����������� �ʿ��Ѱ�� ��Ʋ ������ ������ �ʿ��� )
    //! \param dwClubP ��Ŭ�� ��ȣ(Ŭ�� ��Ʋ�� ��û�� ���� ��)
    //! \param dwClubS ���� Ŭ�� ��ȣ(Ŭ�� ��Ʋ�� ��û�� �� Ŭ�� )
    //! \param nGuKillNum Ŭ�� ��Ʋ ų��
    //! \return
    virtual int SaveClubBattle(DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum) override;

    //! ���͹�Ʋ ������ �ʱ�ȭ ��Ű���.
    //! \param dwClub Ŭ�� ��ȣ
    virtual int ReSetAllianceBattle(DWORD dwClub) override;

    //! �ش� Ŭ���� ��Ʋ���� Ŭ�� ������ �����´�.
    //! \param dwClub Ŭ����ȣ
    //! \param &vMember Ŭ����Ʋ ����
    virtual int GetClubBattleInfo(DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo) override;

	virtual int UpdateClubName( DWORD dwClub, const std::string& ClubName ) override;

    //! �ش� ĳ������ ����ġ�� �����Ѵ�.
    //! ĳ���͹�ȣ�� 0 ���� Ŀ���Ѵ�.
    //! ����ġ�� 0 ���� Ŀ���Ѵ�.
    //! \param  nChaNum : ĳ���͹�ȣ
    //! \param   llExp : ����ġ
    virtual int SetChaExp(int nChaNum, LONGLONG llExp) override;

    //! �ش� ĳ������ ����ġ�� �����´�.
    //! ĳ���͹�ȣ�� 0 ���� Ŀ���Ѵ�.
    //! ����ġ�� 0 ���� ������ �����̴�.
    //! \param nChaNum ĳ���͹�ȣ
    //!  LONGLONG : ����ġ
    virtual LONGLONG GetChaExp(int nChaNum) override;

    //! ���ӳ����� �����̳� �Ű�� �Ű��� ������ DB�� ����Ѵ�.    
    //! \param nSGNum �����׷�
    //! \param ChaNum �Ű��� ĳ���͹�ȣ
    //! \param strMsg �Ű���(�ִ�ũ��� CHAT_MSG_SIZE)
    virtual int LogAppeal(int nSGNum, int nChaNum, CString strMsg) override;

    //! ���� ��������� ���� Ŭ�������͸� �����´�.
    virtual int GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB) override;

    //! ������ �������� ������ Ŭ�� ������ ���Ѵ�.
    //! \param dwRegionID : ���� ID
    //! \param dwClub : Ŭ�� ID
    //! \param fTax : ����
    virtual int SetClubRegion(DWORD dwRegionID, DWORD dwClub=0, float fTax=0) override;

    //! �ش� ������ ���� ���¸� �����Ѵ�.
    //! \param dwRegionID ���� ID
    //! \param dwClub Ŭ�� ID(�⺻ 0 �̴�)
    virtual int DelClubRegion(DWORD dwRegionID, DWORD dwClub=0) override;

    //! �ش�Ŭ���� �����ݾ��� �����´�.(Storage)
    //! \param dwClub Ŭ����ȣ
    //! \param ���ϰ��� >= 0, 0 ���� ������ ERROR
    virtual LONGLONG GetClubMoney(DWORD dwClub) override;

    //! �ش�Ŭ���� �����ݾ��� �����Ѵ�.(Storage)
    //! \param dwClub : Ŭ����ȣ
    //! \param llMoney : �����ݾ�(�ݾ��� >= 0)
    virtual int	SetClubMoney(DWORD dwClub, LONGLONG llMoney) override;

    //! �ش�Ŭ���� ���Աݾ��� �����Ѵ�.(Income money)
    //! \param dwClub : Ŭ����ȣ
    //! \param llMoney : ���Աݾ�(�ݾ��� >= 0)
    virtual int	SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney) override;    

    //! �ش�Ŭ���� ���Աݾ��� �����´�.(Income money)
    //! \param dwClub : Ŭ����ȣ
    //! \return ���ϰ��� >= 0, 0 ���� ������ ERROR
    virtual LONGLONG GetClubIncomeMoney(DWORD dwClub) override;

    //! �ش�Ŭ���� Ŭ��â�� �о�´�.
    //! \param dwClub : Ŭ����ȣ
    //! \param ByteStream : ����
    virtual int ReadClubStorage(DWORD dwClub, se::ByteStream &ByteStream) override;    
	virtual int GetGuildMigrationState( DWORD dwClub ) override;

    //! �ش�Ŭ���� Ŭ��â�� �����Ѵ�.
    //! \param dwClub : Ŭ����ȣ
    //! \param pData : â����ġ
    //! \param nSize : ���� ��üũ��
    virtual int	WriteClubStorage(DWORD dwClub, BYTE* pData, int nSize) override;	

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
        const TCHAR* szComment) override;

    //! �� Ŭ������ �����Ѵ�
    //! \param dwClub : Ŭ����ȣ
    //! \param dwChaNum : ĳ���͹�ȣ
    //! \param dwSubMasterFlags : �÷���(0 ���� ������ �Ϲݱ����� �ȴ�)
    virtual int	SetClubMasterFlags(
        DWORD dwClub,
        DWORD dwChaNum,
        DWORD dwSubMasterFlags) override;

    //! ���� Ŭ���� �Ἲ�Ѵ�.
    //! \ db �� Ŭ���� ���Ἲ�� üũ���� �ʴ´�.
    //! \ A �� B �� ������ ���¿��� 
    //! \ B �� C �� ������ ����
    //! \ C �� A �� ������ ���� ������ ü�� ������ �߻��Ѵ�.	
    //! \param dwClubP : �� Ŭ����ȣ(���� Ŭ���� ��)
    //! \param dwClubS : ���� Ŭ����ȣ(�� Ŭ�� ������ ���� ���� Ŭ��)
    virtual int	SetClubAlliance(DWORD dwClubP, DWORD dwClubS) override;

    //! ���� Ŭ���� ��ü�Ѵ�.
    //! \param dwClubP : �� Ŭ����ȣ(���� Ŭ���� ��)
    //! \param dwClubS : ���� Ŭ����ȣ(�� Ŭ�� ������ ���� ���� Ŭ��)
    virtual int	DelClubAlliance(DWORD dwClubP, DWORD dwClubS) override;

    //! ���� Ŭ�� ����Ʈ�� �����´�.
    //! <��Ŭ��,����Ŭ��>
    virtual int	GetClubAlliance(std::vector< std::pair<DWORD,DWORD> > &vecAlliance) override;

    //! �ش� ������� ä�� ��Ͻð��� �����´�
    virtual __time64_t  GetChatBlockTime(int nUserNum) override;

    //! �ش� ������� ä�� ��Ͻð��� �����Ѵ�
    virtual int SetChatBlockTime(int nUserNum, __time64_t tBlockTime) override;

    //! Ŭ���� ���������� �����Ѵ�.
    virtual int SetClubNotice(const std::string& ChaName, DWORD dwClub, const std::string& ClubNotice) override;

    //! Ŭ���� ����Ŭ�� ���� �븮�ڸ� �����Ѵ�.
    virtual int SetClubDeputy(DWORD dwClub, DWORD dwDeputy) override;

    //! Ŭ���� ������ ������ �����Ѵ�.
    virtual int SetClubAuthority(DWORD dwClub, DWORD dwMasterID) override;
    virtual int InsertClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) override;
    virtual int UpdateClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) override;
    virtual int ChangeClubAuthIndex(DWORD ClubDbNum, size_t AuthIndexA, size_t AuthIndexB) override;
    virtual int ChangeClubMemberGrade(DWORD ChaDbNum, int Grade) override;

    //! ĳ������ �̸��� �����Ѵ�.
    //! \param dwChaNum ĳ���͹�ȣ
    //! \param szCharName ������ ĳ�����̸�
    //! \return sc::db::DB_OK, sc::db::DB_ERROR
    virtual int RenameCharacter(DWORD dwChaNum, char* szCharName) override;

    //! ĳ������ �Ӹ����� �����Ѵ�.
    //! \return sc::db::DB_OK, sc::db::DB_ERROR
    virtual int SetChaHairColor(DWORD dwChaNum, int nHairColor) override;

    //! ĳ������ ��Ÿ���� �����Ѵ�.
    //! \return sc::db::DB_OK, sc::db::DB_ERROR	
    virtual int SetChaHairStyle(DWORD dwChaNum, int nHairStyle) override;

    //! ĳ������ �󱼽�Ÿ���� �����Ѵ�.
    //! \return sc::db::DB_OK, sc::db::DB_ERROR	
    virtual int SetChaFaceStyle(DWORD dwChaNum, int nFaceStyle) override;

    //! ĳ������ ������ �����Ѵ�.
    //! \return sc::db::DB_OK, sc::db::DB_ERROR	
    virtual int SetChaGenderChange(DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor) override;

	// ĳ���� �п��� �����Ѵ�
	virtual int SetChaSchoolChange(DWORD dwChaNum, int nSchool) override;

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
        FLOAT fLastCallPosZ) override;

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
        ) override;


    //! ���� �̸��� �����Ѵ�.
    //! \param nChaNum ĳ���� ��ȣ
    //! \param nPetNum ���� ������ȣ
    //! \param szPetName ���̸�
    //! \return sc::db::DB_ERROR, sc::db::DB_OK
    virtual int RenamePet(int nChaNum, int nPetNum, const TCHAR* szPetName) override;

    //! ���� �������� �����Ѵ�.
    //! \param nChaNum ���� ���ο� ������
    //! \param nPetNum �������� ������ �� ��ȣ
    virtual int ExchangePet(int nChaNum, int nPetNum) override;

    //! ���� �÷��� �����Ѵ�.
    //! \param nChaNum ĳ���� ��ȣ
    //! \param nPetNum �÷��� �����Ϸ��� ���� ��ȣ
    //! \param nPetColor ���� �÷�
    virtual int SetPetColor(int nChaNum, int nPetNum, int nPetColor) override;

    //! ���� ��Ÿ���� �����Ѵ�.
    //! \param nPetNum ��Ÿ���� �����Ϸ��� ���� ��ȣ
    //! \param nPetStyle ���� ��Ÿ��
    virtual int SetPetStyle(int nChaNum, int nPetNum, int nPetStyle) override;

    //! ���� �������� �����Ѵ�. �������� 0-100 ����
    //! \param nPetNum �������� ������ ���� ��ȣ
    //! \param fPetFull ������
    virtual int SetPetFull(int nChaNum, int nPetNum, int nPetFull) override;

    //! ���� �κ��丮 ������Ʈ
    virtual int SetPetInven(int nChaNum, int nPetNum, se::ByteStream &ByteStream) override;

	virtual int SetPetMigrationState( int ChaNum, int PetNum, int MigrationState ) override;
	virtual int SetVehicleMigrationState( int ChaNum, int VehicleNum, int MigrationState ) override;

    //! ���� ��ų ������Ʈ
    virtual int SetPetSkill(
        int nChaNum,
        int nPetNum,
        int nPetInvenType,
        int nPetInvenMID,
        int nPetInvenSID,
        int nPetInvenCMID,
        int nPetInvenCSID,
        int nPetInvenAvailable) override;

    //! ���� ��ų���� ���� ������Ʈ
    virtual int SetPetSkillSlotNum(int nChaNum, int nPetNum, WORD wSkillSlotNum) override;

    //! ���� �����Ѵ�.
    //! \param nPetNum ������ ���� ��ȣ
    virtual int	DeletePet(int nChaNum, int nPetNum) override;	

    //! ���� ��ų�� �����´�.
    //! \param pPet ���� ������
    virtual int GetPetSkill(int nChaNum, GLPET* pPet, DWORD dwPetNum) override;

    //! ���� �κ��丮 ������ �����´�
    virtual int GetPetInven(int nChaNum, GLPET* pPet, DWORD dwPetNum) override;

    //! ���� ������ �����´�.
    virtual int GetPet(GLPetField* pPet, DWORD dwPetNum, DWORD dwClientID) override;

    //! ���� �������� �����´�
    //! \param dwPetNum �� ��ȣ
    //! \return ���� ������ or sc::db::DB_ERROR
    virtual int GetPetFull(int nChaNum, DWORD dwPetNum) override;

    //! �Ҹ�� ���� ����Ʈ�� �����´�.
    //! \param nChaNum �Ҹ�� �� ����Ʈ�� ����Ϸ��� ĳ���� ��ȣ
    //! \param vecPetReList �Ҹ�� �� ����Ʈ�� ���� ����
    virtual int GetRestorePetList(int nChaNum, std::vector<PET_RELIST>& vecPetReList) override;

    //! ���� ��Ȱ��Ų��.
    //! \param nPetNum ��Ȱ�� �� ��ȣ
    //! \param nChaNum ��Ȱ�� �䱸�� ĳ���� ��ȣ
    virtual int RestorePet(DWORD dwPetNum, int nChaNum) override;

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
        int nPetSkinTime) override;

    //! SMS ���ڸ޽����� �߼��Ѵ�.
    //! \param nSendChaNum SMS �� �߼��ϴ� ĳ���� ��ȣ	
    //! \param nReceiveChaNum SMS �� ���� ĳ���� ��ȣ
    //! \param szReceivePhone SMS �� ���� ĳ������ ��ȭ��ȣ
    //! \param szSmsMsg SMS �޽��� ����
    virtual int LogSMS(
        int nSendChaNum,					
        int nReceiveChaNum,
        const TCHAR* szReceivePhone,
        const TCHAR* szSmsMsg) override;

    //! �������� �����ɼ� �α׸� �����.
    //! \param _sLOG �����ɼ� ����ü
    virtual int LogRandomItem(const LOG_RANDOM_OPTION &_sLOG) override;

	virtual int LogAddonItem( const LOG_ADDON_OPTION &_sLOG ) override;
	virtual int LogLinkSkillItem( const LOG_LINKSKILL_OPTION &_sLOG ) override;
	virtual int LogBasicStatItem( const LOG_BASICSTAT_OPTION &_sLOG ) override;

	virtual int LogItemCostumeStat(const LOG_COSTUME_STATS &_sLOG) override;

    //! ĳ������ �޴��� ��ȣ�� �����Ѵ�.
    //! \param nChaNum ĳ���� ��ȣ
    //! \param szPhoneNumber ��ȭ��ȣ NULL ���� ���� 14�� SMS_RECEIVER
    virtual int SetChaPhoneNumber(int nChaNum, const TCHAR* szPhoneNumber) override;

    //! ���� �׼� �α׸� �����.
    //! \param _sLOG ��׼Ƿα� ����ü
    virtual int LogPetAction(const LOG_PET_ACTION &_sLOG) override;

    //! Terra
    //! ������ �н����带 �ְų� üũ�Ѵ�.
    //! \param szTID Terra UserID
    //! \param szPasswd ����� �н�����
    //! \return 
    virtual int TerraUserPassCheck(const TCHAR* szTID, const TCHAR* szPasswd, int nCheckFlag) override;

    //! Daum
    //! ������ �н����带 �ְų� üũ�Ѵ�.
    //! \param szDaumGID Daum UserGID
    //! \param szDaumPass ����� �н�����
    //! \return 
    virtual int DaumUserPassCheck(const TCHAR* szDaumGID, const TCHAR* szDaumPasswd, int nCheckFlag) override;

    //! Excite
    //! ������ �н����带 �ְų� üũ�Ѵ�.
    //! \param szUID Excite UserID
    //! \param szPasswd ����� �н�����
    //! \return 
    virtual int ExciteUserPassCheck(
        const TCHAR* szUID, const TCHAR* szUserID, const TCHAR* szPasswd, int nCheckFlag) override;

    //! Thailand
    //! ������� PC�� Ŭ������ üũ�Ѵ�.	
    //! \param szUsrIP ������� IP Address
    //! \return DB ������ �����
    virtual int	ThaiUserClassCheck(const TCHAR* szUsrIP) override;

    //! Malaysia
    //! ������� PC�� Ŭ������ üũ�Ѵ�.	
    //! \param szUsrIP ������� IP Address
    //! \return DB ������ �����
    virtual int	MyUserClassCheck(const TCHAR* szUsrIP) override;

    //! �ʸ���
    //! ������� PC�� Ŭ������ üũ�Ѵ�.	
    //! \param szUsrIP ������� IP Address
    //! \return DB ������ �����
    virtual int	PhUserClassCheck(const TCHAR* szUsrIP) override;

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
        int nVehicleCardSID) override;

    //! Ż���� ���͸��� �����Ѵ�. ���͸��� 0-1000 ����
    //! \param nVehicleBttery ���͸��� ������ Ż���� ��ȣ
    //! \param nVehicleBttery ���͸�
    virtual int SetVehicleBattery(
        int nVehicleNum,
        int nCharNum,
        int nVehicleBttery) override;

    //! Ż���� ���͸��� �����´�.
    //! \param nVehicleBttery ���͸��� ������ Ż���� ��ȣ
    virtual int GetVehicleBattery(int nVehicleNum, int nCharNum) override;

    //! Ż���� �����Ѵ�.
    //! \param nVehicle ������ Ż���� ��ȣ
    virtual int DeleteVehicle(int nVehicle, int nCharNum) override;

	virtual int RebuyVehicle(int nVehicle, int nCharNum) override;

    //! Ż���� ������ �����´�.	
    virtual int GetVehicle(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) override;

    //! Ż���� �κ��丮 ������ �����´�
    //virtual int GetVehicleInven(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) override;

    //! Ż�� �κ��丮 ������Ʈ
    virtual int SetVehicleInven(int nVehicle, int nCharNum, se::ByteStream& ByteStream) override;

    //! Ż���� �÷� ������ �����´�
    //virtual int GetVehicleColor(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) override;

    //! Ż�� �÷� ������Ʈ
    virtual int SetVehicleColor(int nVehicle, int nCharNum, se::ByteStream &ByteStream) override;

    //! Ż���� �ν��� ��ų ������Ʈ
    //! \param m_nVehicleBooster ��� �ν��� ����
    virtual int SetVehicleBooster(int nVehicleNum, int nCharNum, DWORD dwVehicleBooster) override;

    //! Ż���� �׼� �α׸� �����.
    //! \param _sLOG Ż�;׼Ƿα� ����ü
    virtual int LogVehicleAction(const LOG_VEHICLE_ACTION &_sLOG) override;

    // �⼮�� �̺�Ʈ 
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
	virtual int Certification2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) override; // 2�����
	virtual int Update2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) override; // 2�����

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

    // ���λ��� ��Ź�Ǹ� ( ������Ʈ �ŷ� ) �߰���.
    int GetCharacterInfoConsignmentSlotExpansionDate(int nChaNum, SCHARDATA2* pChaData2);

    //! ��Ʈ�� Ž������ �ý����� �߰� ���� �ҷ����� �κ� �߰�
    int GetVTCharInfo(int nChaNum, SCHARDATA2* pChaData2);
    //! �븸 ��ŷ ���� �����ؼ� �ֱ� �α׾ƿ� �������� ������ DB���� �о��.
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

    //! Character �� PointShop Point �� �����´�
    //! ChaDbNum : ĳ���� DB ��ȣ
    //! PointRefundable : ȯ�Ұ��� ����Ʈ
    //! PointNotRefundable : ȯ�ҺҰ� ����Ʈ
    virtual int GetChaPoint(int ChaDbNum, __int64& PointRefundable, __int64& PointNotRefundable) override;

    //! Character �� PointShop Point �� �����Ѵ�
    //! ChaDbNum : ĳ���� DB ��ȣ
    //! PointRefundable : ȯ�Ұ��� ����Ʈ
    //! PointNotRefundable : ȯ�ҺҰ� ����Ʈ
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
    //! Billing ���� Shop, Item �Ǹ� ���� �α�
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
    // �δ��α� InstantDungeon Log
    
    // �α� Ÿ���� ����
    virtual int InstantDungeonLogTypeInsert( 
		const db::EM_INSTANT_LOG_TYPE& nLogType,
        const char* strParamName1=0,  const char* strParamName2=0,  const char* strParamName3=0,  const char* strParamName4=0,  const char* strParamName5=0,
        const char* strParamName6=0,  const char* strParamName7=0,  const char* strParamName8=0,  const char* strParamName9=0,  const char* strParamName10=0,
        const char* strParamName11=0, const char* strParamName12=0, const char* strParamName13=0, const char* strParamName14=0, const char* strParamName15=0,
        const char* strParamName16=0, const char* strParamName17=0, const char* strParamName18=0, const char* strParamName19=0, const char* strParamName20=0 ) override;

    // ���� �α� �Է�
    virtual int InstantDungeonLogInsert( 
		const db::EM_INSTANT_LOG_TYPE& nLogType,
        const char* strParam1=0,  const char* strParam2=0,  const char* strParam3=0,  const char* strParam4=0,  const char* strParam5=0,
        const char* strParam6=0,  const char* strParam7=0,  const char* strParam8=0,  const char* strParam9=0,  const char* strParam10=0,
        const char* strParam11=0, const char* strParam12=0, const char* strParam13=0, const char* strParam14=0, const char* strParam15=0,
        const char* strParam16=0, const char* strParam17=0, const char* strParam18=0, const char* strParam19=0, const char* strParam20=0 ) override;


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
        ) override ;

    // ���� ���� ����;
    virtual int CompetitionRatingUpdate(
        const std::string& strContentType,
        const DWORD charDbNum,
        const int Rating,
        const int WinningStreak,
        const int PlayCount,
        const __time64_t tLastPlayDate
        ) override;

    // ���� ���� ������Ʈ;
    virtual int CompetitionSeasonUpdate(
        const std::string& strContentType,
        const int seasonNum,
        const int baseRating
        ) override;

    // ���� �α� CompetitionLog



	/////////////////////////////////////////////////
	// ���� ���� �α� ���
	virtual int AddAuthServerState( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, int nCurUser, int nMaxUser, const __time64_t& LogDate ) override;
	// ���� ���� ����
	virtual int ProcessCertificationForAuth( int isSessionSvr, int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::string strUniqKey, int &certification, std::string &strNewUniqKey, int &nSessionSvrID ) override;
	// �̺�Ʈ ���� ���
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
    // RanPoint Trade ������Ʈ �ŷ� ( ��Ź�Ǹ� ) ConsignmentsSale

    //! ITEM ID�� ���� �������� ������ �´�. ( bDateCheck => ��¥(Date)�˻� ���� )
    virtual int ConsignmentsSaleGet( sc::MGUID& Guid, private_market::ConsignmentSaleItem& SaleItem, bool bDateCheck = false ) override;
    //! ChaNum�� ������ ĳ������ ��ǰ�� ���� ������� ChaNum�� ���� ������ ������ ��Ź�Ǹ� ��ų ��ǰ�� ���� �´�.
    virtual int ConsignmentsSaleSelect( private_market::SALE_ITEMS_VECTOR& vecItem, const DWORD& dwChaNum = 0, const int& ServerGroup = 0 ) override;
    //! ���� ������ �ٲ۴�.
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


    // ���� ���� ����
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

	// �ʸ��� ��������
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


    // GM LOG ( GM ������ �־��ֱ� )
    virtual int WriteGMLog(int nGmNum, std::string& strCmd, std::string& strUserIP) override;

	// �������
	virtual int RanMobileRequest(DWORD dwUserDBNum, DWORD dwChaDBNum, int nServerNum, int nCommand, int nValue, const std::string& strChaName, int nChaClass ) override;

	// Log Money;
	virtual int MoneyLogInsert ( const int _nMainType, const int _nSubType, const LONGLONG _llMoney ) override;

	//////////////////////////////////////////////////////////////////////////
	// �Ⱓ�� ������ �ð� ����
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
                                          ) override;

}; // class AdoManager

} // namespace db
#endif // _DB_ADO_MANAGER_H_
