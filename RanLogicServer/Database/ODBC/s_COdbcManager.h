#ifndef _CODBCMANAGER_H_
#define _CODBCMANAGER_H_

#pragma once

#include <Windows.h>
#include <stdio.h>
#include <odbcinst.h>
#include <sqlext.h> // ODBC library

#include <vector>
#include <sstream>
#include <hash_map>

#include "../../../SigmaCore/Util/Lock.h"

#include "../../../RanLogic/Character/GLCharData.h"
#include "../../../RanLogic/Msg/GLContrlMsg.h"

#include "../../../RanLogic/Activity/ActivityBase.h"

#include "../../../RanLogic/Pet/GLPet.h"
#include "../../Character/GLCharAG.h"
#include "../../Guidance/GLGuidance.h"
#include "./s_COdbcSupervisor.h"
#include "../DbManager.h"


//! \date 2002-05-28
//! \author jgkim
//! \par license
//! Copyright(c) MIN COMMUNICATIONS, INC. All rights reserved. 
 
//
//mjeon.dbman
//

class COdbcManager : public db::IDbManager, public sc::DefaultLock
{
private:
    struct ODBC_CONNECTION_INFO
    {
        std::string OdbcName;
        std::string UserID;
        std::string UserPasswd;
        std::string DbName;
        int PoolSize;
        int DbTimeOut;
        bool bValid;
        ODBC_CONNECTION_INFO()
            : bValid(false)
        {
        }
        void SetData(
            const char* szOdbcName,
            const char* szUserId,
            const char* szPassword,
            const char* szDbName,
            int _PoolSize,
            int _DbTimeOut)
        {
            OdbcName = szOdbcName;
            UserID = szUserId;
            UserPasswd = szPassword;
            DbName = szDbName;
            PoolSize = _PoolSize;
            DbTimeOut = _DbTimeOut;
            bValid = true;
        }
    };
public:		
	//mjeon.Post
	typedef std::vector<SAPOST>	VPOST;
	typedef VPOST::iterator		VPOST_ITER;

	typedef std::vector<SAPOSTSTATE>	VPOSTSTATE;
	typedef VPOSTSTATE::iterator		VPOSTSTATE_ITER;

public:
	COdbcManager(NetServer* pServer, db::SERVER_TYPE ServerType, EMSERVICE_PROVIDER ServiceProvider, bool UseCustomPool);
	virtual ~COdbcManager();	

protected:
    bool m_UseCustomPool;

    ODBC_CONNECTION_INFO m_GameDbInfo;
   	COdbcSupervisor* m_pGameDB;

    ODBC_CONNECTION_INFO m_UserDbInfo; //! User Database
	COdbcSupervisor* m_pUserDB; //! User Database

    ODBC_CONNECTION_INFO m_BoardDbInfo;
	COdbcSupervisor* m_pBoardDB;

    ODBC_CONNECTION_INFO m_LogDbInfo;
	COdbcSupervisor* m_pLogDB;

    ODBC_CONNECTION_INFO m_ShopDbInfo;
	COdbcSupervisor* m_pShopDB;

    ODBC_CONNECTION_INFO m_TerraDbInfo;
	COdbcSupervisor* m_pTerraDB;

    ODBC_CONNECTION_INFO m_GspDbInfo;
	COdbcSupervisor* m_pGspDB;

    ODBC_CONNECTION_INFO m_ThaiDbInfo;
	COdbcSupervisor* m_pThaiDB;

    ODBC_CONNECTION_INFO m_KorDbInfo; //! Korea, Web User Database
	COdbcSupervisor* m_pKorDB; //! Korea, Web User Database
    
    ODBC_CONNECTION_INFO m_MalaysiaDbInfo;
	COdbcSupervisor* m_pMalaysiaDB; //! �����̽þ� PC�� �̺�Ʈ

public:
    virtual bool IsAlive() override;
    virtual bool Reconnect() override; //! ������ üũ
    virtual bool CheckHdd() override; //! Hard disk �뷮 üũ
    virtual bool IsAllDbJobCompleted() override;
    virtual void WaitAllDbJobCompleted(DWORD WaitLimitTime) override;

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
    virtual bool SetConnectionString(db::DB_TYPE Type, const std::string& Ip, const std::string& DbName, const std::string& Id, const std::string& Password, int AdoMaxPoolSize, int DatabasePacketSize) { return false; }

    void CloseGameDB();
    void CloseUserDB();
    void CloseBoardDB();	
	void CloseLogDB();
	void CloseShopDB();	
	void CloseTerraDB();
	void CloseGspDB();	
	void CloseThaiDB();
	void CloseKorDB();
	void CloseMyDB();

	//void Print(const CString& strMsg);
    void Print(const std::string& strMsg);
    std::string GetErrorString(SQLHSTMT hStmt, SQLSMALLINT fHandleType = SQL_HANDLE_STMT);
	//void CheckConnection();

	/**************************************************************************
	* Game Database
    */
	//! ����� �κ��丮���� ����Ÿ�� �о�´�.
	int	ReadUserInven(
			int SGNum,
			DWORD dwUserNum,
			se::ByteStream &ByteStream );

	//! �����κ��丮�� �ִ��� üũ�Ѵ�.
	bool CheckInven( int SGNum, DWORD dwUserNum );
	int	MakeUserInven( int SGNum, DWORD dwUserNum );
	//! ����� �κ��丮�� ����Ÿ�� �����Ѵ�.
	int	WriteUserInven( 
			int SGNum, 
			LONGLONG llMoney /* LONGLONG __int64 */,
			DWORD dwUserNum,
			BYTE* pData,
			int nSize);
	
	// ���ο� ĳ���͸� �����Ѵ�.
	virtual int	CreateNewCharacter( SCHARDATA2* pCharData2 ) override;
	//mjeon.ado
	virtual int CreateNewCharacterExtraWithAdo(SCHARDATA2* pCharData2) {return 0;}

    //! ChaInfo Backup.
    virtual int CharacterInfoBackup(SCHARDATA2* pCharData2) override;


	// �븸,ȫ��
	// ĳ���� ������ Temp Table�� ĳ���� ������ �ݾ� ������ ��ȣȭ �ؼ� ������ �д�.
	int	InsertCharInfoTemp(int nChaNum);

	virtual int ReadUserInven(SCHARDATA2* pChaData2);
	virtual int	ReadUserInvenNew( SCHARDATA2* pChaData2, int FirstLoad = 0 ) { return 0; }

	virtual int UpdateUserLockerOption( DWORD UserNum, int Option ) override { return 0; }

	//! ĳ���͸� �����Ѵ�
	virtual int	SaveCharacter( LPVOID _pbuffer, bool bAutoSave = false );
	//mjeon.ado
	virtual int SaveCharacterExtraWithAdo(LPVOID _pbuffer);

    virtual int SaveCharacterBase(SCHARDATA2* pCharData2, DWORD ChaDbNum) override;

	virtual int SaveInventoryAndGameMoney( LPVOID _pbuffer );

	virtual int SaveChaPos(
        DWORD dwChaNum,
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

	virtual int RenameCharacter(DWORD dwChaNum, TCHAR* szCharName) override;

	/**
	* ĳ���� ������ �����´� (���̳ʸ� ���� ����Ÿ)
	* \param nUserNum ����ڹ�ȣ
	* \param nChaNum ĳ���͹�ȣ
	* \return ĳ��������
	*/
	/*
	SCHARDATA2*	GetCharacter(
					int nUserNumber,
					int nChaNum );
	*/

	int GetChaPhoneNumber(int nChaNum, TCHAR* szPhoneNumber);

	virtual int GetCharacterInfo(int nUserNumber, int nChaNum, SCHARDATA2* pChaData2) override;

	//mjeon.ado
	int	GetCharacterInfoExtraWithAdo(int nUserNumber, int nChaNum, SCHARDATA2* pChaData2) {return 0;}

	virtual int GetCharacterInfo(
			int nUserNumber,
		    int nChaNum,
			GLCHARAG_DATA* pChaData) override;

	virtual int SetCharacterOption( int ChaNum, int CharacterOption ) override { return 0; };

	virtual int GetCharacterOption( int ChaNum ) override { return 0; };

	virtual int	Certification2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) override; // 2�����
	virtual int	Update2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) override; // 2�����
	
	//! �ش� ������� ĳ���� ������ �����´�.
	int	GetChaAllInfo( int nUsrNum );
	//int GetChaBAInfo(int nUsrNum, int nSvrGrp, NET_CHA_BBA_INFO_DH* ncbi);
    virtual int GetChaBAInfo(int UserNum, int SvrGrp, std::vector<int>& vChaNum) override;
	virtual int	GetChaBInfo(int nUserNum, int nChaNum, SCHARINFO_LOBBY* sci) override;
	virtual int	GetChaBInfoForMigration( int nUserNum, int nChaNum, SCHARINFO_LOBBY* sci ) override { return 0; } 
    virtual int GetChaBInfoAll(int UserDbNum, int ServerGroup, std::vector<SCHARINFO_LOBBY>& vCharInfo) override;
	virtual int GetChaBInfoAllForMigration( int UserDbNum, int ServerGroup, std::vector< SCHARINFO_LOBBY >& vCharInfo ) override;

	//mjeon.Post.ChaNum
	virtual int GetChaNum(const char *szCharName, int *pnChaNum, int *pnUserNum) override;
	virtual int PostGetData(int iRecver, VPOST &vPost, int &nRowCount, DWORD dwRecverUserNum) override;
	virtual int PostGetInfo(int iRecver, int &cntAllPost, int &cntNewPost) override;

    virtual int PostGetState(int iRecverNum, VPOSTSTATE &vPost, int &nRowCount) override;	//PostState
    virtual int PostGetSendState(int iRecverNum, VPOSTSTATE &vPost, int &nRowCount) override;	//PostSendState
    virtual int PostGetRecvState(int iRecverNum, VPOSTSTATE &vPost, int &nRowCount) override;	//PostRecvState

	virtual int PostOpen(LONGLONG llPostID) override;
	virtual int PostDelete(LONGLONG llPostID) override;
	virtual int PostReturn(LONGLONG llPostID) override;
	virtual int PostSendCharge(SAPOST &aPOST, DWORD dwOpenAndTakeUserNum) override;

	virtual int GetUserInvenOption( int UserNum, int& Option ) override { return 0; };

	//
	//mjeon.sns
	//
	virtual int SNSAuthedFB(SFACEBOOK &AuthFB)							override;
	virtual int SNSUnAuthedFB(int nChaNum)								override;
	virtual int SNSGetInfoFB(SFACEBOOK &AuthFB)							override;
	virtual int SNSGetUidFB(int nTargetChaNum, std::string &m_UID)		override;

	
	virtual int SNSAuthedTW(STWITTER &AuthTW)							override;
	virtual int SNSUnAuthedTW(int nChaNum)								override;
	virtual int SNSGetInfoTW(STWITTER &AuthTW)							override;
	virtual int SNSGetUidTW(int nTargetChaNum, std::string &m_UID)		override;

	virtual int SNSGetInfo(SFACEBOOK &AuthFB, STWITTER &AuthTW)			override;


	virtual int AttendanceLoad(OUT AttendanceManager &AttendanceMan, UINT nChaNum) override;
	virtual int AttendanceSave(IN  AttendanceManager &AttendanceMan, UINT nChaNum, LONGLONG nChaCurExp) override;
	virtual int AttendanceMaintenance() override;
    virtual int AttendanceLogInsert(int ChaNum, int Type, const std::string& LoadType, int DayN, int Status) override;
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
	virtual int ProvideRewardInitialize ( )																	override { return sc::db::DB_ERROR; }
	virtual int ProvideRewardInsert ( const ProvideRewardSystem::PROVIDE_REWARD_DATA& _ProvideRewardData )	override { return sc::db::DB_ERROR; }
	virtual int ProvideRewardSelect ( DWORD _dwChaDBNum, DWORD _dwServerGroupNum, OUT ProvideRewardSystem::VEC_PROVIDE_REWARD& _vecProvideRewardData )													override { return sc::db::DB_ERROR; }
	virtual int ProvideRewardClearDeleteFlag ( )															override { return sc::db::DB_ERROR; }
	virtual int ProvideRewardComplete ( DWORD _dwUniqueID )													override { return sc::db::DB_ERROR; }

	//
	// Country System
	//
	virtual int UpdateUserCountry ( IN DWORD dwUserID, IN Country::SCOUNTRY_INFO& sCountryInfo )			override { return sc::db::DB_ERROR; }
	virtual int LoadVictoriousCountry ( OUT Country::VCOUNTRY_VEC& vecVCountry )							override { return sc::db::DB_ERROR; }
	virtual int AddVictoriousCountry ( IN Country::SVICTORIOUS_COUNTRY& sVictoriousCountry )				override { return sc::db::DB_ERROR; }
	virtual int ResetVictoriousCountry ( IN Country::EMCOUNTRY_SYSTEM_FLAG emFlag )							override { return sc::db::DB_ERROR; }
	virtual int ConfirmStateVictoriousCountry ( IN Country::EMCOUNTRY_SYSTEM_FLAG emFlag, OUT Country::VCOUNTRY_VEC& vecVCountry )							override { return sc::db::DB_ERROR; }

	//
	// Lotto System
	//
	virtual int LottoSystemLoad( unsigned int systemId )								override { return sc::db::DB_ERROR; }

	virtual int LottoDrawingMachineNumSelect(
		unsigned int lottoNum, unsigned int systemId )									override { return sc::db::DB_ERROR; }
	virtual int LottoSystemCurTurnSelect( unsigned int lottoSystemId )					override { return sc::db::DB_ERROR; }
	virtual int LottoAccumulateMoneySelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override { return sc::db::DB_ERROR; }
	virtual int LottoWinNumSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override { return sc::db::DB_ERROR; }
	virtual int LottoCharBuyListSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override { return sc::db::DB_ERROR; }
	virtual int LottoCharNonConfirmBuyListSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int systemId, int confirmTicket )						override { return sc::db::DB_ERROR; }
	virtual int LottoWinManListSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override { return sc::db::DB_ERROR; }
	virtual int LottoInventWinNumSelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )					override { return sc::db::DB_ERROR; }

	virtual int LottoDrawingMachineNumSave(
		unsigned int lottoNum,
		unsigned int accumulateCount,
		unsigned int addCount, unsigned int systemId, bool bUpdate = true )				override { return sc::db::DB_ERROR; }
	virtual int LottoSystemTurnSave( unsigned int turnNum, unsigned int systemId )		override { return sc::db::DB_ERROR; }

	virtual int LottoCharBuyListInsert(
		LONGLONG accumulateMoney,
		LONGLONG charge,
		int buyType, const std::string& drawingNum,
		LONGLONG drawingNumInt, int confirmTicket,
		DWORD dwUserNum, DWORD dwDbNum, DWORD dwServerGroup, char* szChaName, unsigned int turnNum, unsigned int systemId )					override { return sc::db::DB_ERROR; }

	virtual int LottoWinManListUpdate(
		unsigned int ranking, LONGLONG winMoney, SNATIVEID sItemID, DWORD dwItemCount,
		unsigned int turnNum, unsigned int systemId )									override { return sc::db::DB_ERROR; }
	virtual int LottoWinManListInsert(
		unsigned int ranking, LONGLONG winNumInt,
		LONGLONG bonusNumInt, unsigned int systemId )									override { return sc::db::DB_ERROR; }

	virtual int LottoAccumulateMoneyUpdate(
		LONGLONG accumulateMoney,
		unsigned int turnNum, unsigned int systemId )									override { return sc::db::DB_ERROR; }
	virtual int LottoWinNumUpdate(
		const std::string& winNum, LONGLONG winNumInt,
		LONGLONG bonusNumInt, unsigned int turnNum, unsigned int systemId )				override { return sc::db::DB_ERROR; }
	virtual int LottoCharNonConfirmBuyListUpdate(
		DWORD dwDbNum, DWORD dwServerGroup, LONGLONG invenMoney, unsigned int systemId )						override { return sc::db::DB_ERROR; }

	virtual int LottoSystemNextTurn( unsigned int systemId )							override { return sc::db::DB_ERROR; }
	virtual int LottoConfirmWinMan( unsigned int systemId )								override { return sc::db::DB_ERROR; }
	virtual int LottoChangeInventWinNum( unsigned int turnNum, unsigned int systemId )	override { return sc::db::DB_ERROR; }
	virtual int LottoWinMoneyUpdate( unsigned int turnNum, unsigned int systemId )		override { return sc::db::DB_ERROR; }

	virtual int LottoSystemBuyLog( const LottoSystem::LOTTO_SYSTEM_BUY_LOG& logInfo )	override { return sc::db::DB_ERROR; }
	virtual int LottoSystemWinManLog( unsigned int turnNum, unsigned int systemId )		override { return sc::db::DB_ERROR; }
	virtual int LottoSystemGiveWinTicketLog( unsigned int systemId,
		DWORD dwTicketID, LONGLONG invenMoney )											override { return sc::db::DB_ERROR; }
	virtual int LottoSystemTurnInfoLog(
		const LottoSystem::LOTTO_SYSTEM_TURN_LOG& logInfo )								override { return sc::db::DB_ERROR; }

	virtual int DelCharacter(int nUsrNum, int nChaNum, const TCHAR* szPass2) override;
	virtual int DaumDelCharacter(int nUsrNum, int nChaNum, const char* szPass2) override;
	virtual int IdnDelCharacter(int nUsrNum, int nChaNum) override;
	virtual int TerraDelCharacter(int nUsrNum, int nChaNum) override;
	virtual int GspDelCharacter(int nUsrNum, int nChaNum) override;
	virtual int ExciteDelCharacter(int nUsrNum, int nChaNum) override;
	virtual int JapanDelCharacter(int nUsrNum, int nChaNum) override;
	virtual int GsDelCharacter(int nUsrNum, int nChaNum, const char* szPass2) override;

	// ���� ĳ���� ���� �Լ�
	int	DeleteCharacter( int nUsrNum, int nChaNum );


	// ��Ʈ�� Ž������ �ý����� �߰� ���� �ҷ����� �κ� �߰�
	int COdbcManager::GetVTCharInfo( int nChaNum, SCHARDATA2* pChaData2 );
	// �븸 ��ŷ ���� �����ؼ� �ֱ� �α׾ƿ� �������� ������ DB���� �о��.
	int COdbcManager::GetLastCharInfo( int nUserNum, int nChaNum,SCHARDATA2* pChaData2 );

	// �����̽þ�/�±��� ������ ���� ���� üũ �Ѵ�.
	int	GetUserTempInfo( int nUserNum );
	int SetUserTempInfo( int nUserNum, int nCreateChar );

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

	//! ĳ������ ģ�������� �����´�
	virtual int GetChaFriend(int nChaNum, std::vector<CHA_FRIEND>& vFriend) override;
	virtual int	DelChaFriend(int nChaP, int nChaS) override;
	virtual int AddChaFriend(int nChaP, int nChaS) override;
	virtual int SetChaFriend(int nChaP, int nChaS, int nFlag) override;
    virtual int ChaFriendFlagSelect(int ChaP, int ChaS, int& Flag) override;

	//! �б�vs�б� ��Ƽ��ð������
    virtual int	LogPartyMatch(int nSGNum, int nSvrNum, WORD wWin, WORD wLost) override;
    
    virtual int SetChaExp(int nChaNum, LONGLONG llExp) override;
    
    virtual LONGLONG GetChaExp(int nChaNum) override;
    
	virtual int SetChaStorageDate(int nUserNum, int nStorageNum, __time64_t tTime) override;
    
	virtual int SetChaInvenNum(int nChaNum, WORD wInvenLine=0) override;

    virtual int SetLastCallPos(int nChaNum, DWORD dwMapID, D3DXVECTOR3 vPos) override; 	
    virtual int CreateClub(const std::string& ClubName, DWORD ChaDbNum, wb::EM_COUNTRY Country) override;	
    virtual int DeleteClub(DWORD dwClub, DWORD dwChaNum) override;

	/**
	* Ŭ����ŷ�� �����Ѵ�.
	* \param dwClub Ŭ����ȣ
	* \param dwRank ��ŷ
	* \return 
	*/
    int SetClubRank(DWORD dwClub, DWORD dwRank);    

	/**
	* Ŭ����ũ �̹����� �����´�.
	* \param dwClub Ŭ����ȣ
	* \param &ByteStream ����
	* \return 
	*/
    int ReadClubMarkImage(DWORD dwClub, se::ByteStream &ByteStream);    

	/**
	* Ŭ����ũ �̹����� �����Ѵ�.
	* \param dwClub Ŭ����ȣ
	* \param dwMarkVer Ŭ����ũ����
	* \param pData Ŭ���̹��� �ּ�
	* \param nSize Ŭ���̹��� ������
	* \return 
	*/
    int	WriteClubMarkImage(
			DWORD dwClub,
			DWORD dwMarkVer,
			BYTE* pData,
			int nSize );
	
	/**
	* �ش�Ŭ���� �����ݾ��� �����Ѵ�. (Storage)
	* \param dwClub Ŭ����ȣ
	* \param llMoney �����ݾ� (�ݾ��� >= 0)
	* \return 
	*/
    int	SetClubMoney(DWORD dwClub, LONGLONG llMoney);    

	/**
	* �ش�Ŭ���� �����ݾ��� �����´�. (Storage)
	* \param dwClub Ŭ����ȣ
	* \return ���ϰ��� >= 0, 0 ���� ������ ERROR
	*/
    LONGLONG GetClubMoney(DWORD dwClub);

	/**
	* �ش�Ŭ���� ���Աݾ��� �����Ѵ�. (Income money)
	* \param dwClub Ŭ����ȣ
	* \param llMoney ���Աݾ� (�ݾ��� >= 0)
	* \return 
	*/
    int SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney);    
    
	/**
	* �ش�Ŭ���� ���Աݾ��� �����´�. (Income money)
	* \param dwClub Ŭ����ȣ
	* \return >= 0, 0 ���� ������ ERROR
	*/
    LONGLONG GetClubIncomeMoney(DWORD dwClub);    

	/**
	* �ش�Ŭ���� Ŭ��â�� �о�´�.
	* \param dwClub Ŭ����ȣ
	* \param &ByteStream ����
	* \return 
	*/
    int ReadClubStorage(DWORD dwClub, se::ByteStream &ByteStream);    
	int GetGuildMigrationState( DWORD dwClub );

	/**
	* �ش�Ŭ���� Ŭ��â�� �����Ѵ�.
	* \param dwClub Ŭ����ȣ
	* \param pData â����ġ
	* \param nSize ���� ��üũ��
	* \return 
	*/
    int	WriteClubStorage(DWORD dwClub, BYTE* pData, int nSize);

	/**
	* Ŭ�� ��ü�ð��� �����Ѵ�.
	* \param dwClub Ŭ����ȣ
	* \param tDiss ��ü�ð�
	* \return 
	*/
    int SetClubDissolutionTime(DWORD dwClub, __time64_t tDiss);

	/**
	* Ŭ���� ����Ż�� Ȥ�� ����ð��� ���Ѵ�.
	* \param dwClub Ŭ����ȣ
	* \param tSec �ð�
	* \return 
	*/
	int SetClubAllianceSec(DWORD dwClub, __time64_t tSec);

	/**
	* Ŭ���� �����ػ� �ð��� ���Ѵ�.
	* \param dwClub Ŭ����ȣ
	* \param tDis �ػ�ð�
	* \return 
	*/
	int SetClubAllianceDis(DWORD dwClub, __time64_t tDis);

	/**
	* Ŭ���� ���������� �ð��� ���Ѵ�.
	* \param dwClub Ŭ����ȣ
	* \param tDis ������ ���ӽð�
	* \return 
	*/
	int SetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority);

	//! ������ ��� Ŭ�������� �����´�.
	//! \param &vClub Ŭ������	
    virtual int GetClubInfo(std::vector<std::tr1::shared_ptr<GLClub> > &vClub) override;
    
    //! ĳ������ �̸��� ������ �����´�.
    int GetChaNameLevel(DWORD ChaDbNum, std::string& ChaName, int& ChaLevel);
	
	//! Ŭ���� ���������� �����Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param szClubNotice ��������
	virtual int SetClubNotice(const std::string& ChaName, DWORD dwClub, const std::string& ClubNotice) override;

	//! Ŭ���� ����Ŭ�� ���� �븮�ڸ� �����Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param dwDeputy �븮������ ĳ���͹�ȣ
	virtual int SetClubDeputy(DWORD dwClub, DWORD dwDeputy) override;

	//! Ŭ���� ������ ������ �����Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param dwMasterID ������ ���� ������ �ɸ���
	virtual int SetClubAuthority (DWORD dwClub, DWORD dwMasterID) override;
    virtual int InsertClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) override;
    virtual int UpdateClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) override;
    virtual int ChangeClubAuthIndex(DWORD ClubDbNum, size_t AuthIndexA, size_t AuthIndexB) override;
    virtual int ChangeClubMemberGrade(DWORD ChaDbNum, int Grade) override;

	//! �ش� Ŭ���� ����� �����´�
	//! \param dwClub Ŭ����ȣ
	//! \param &vMember Ŭ�����
    virtual int GetClubMember(DWORD dwClub, std::vector<GLCLUBMEMBER> &vMember) override;

    virtual int GetClubAuth(DWORD ClubDbNum, std::vector<club::AUTH_GRADE_DB>& vAuth) override;    
    virtual int ResetClubPosition(DWORD ClubDbNum) override;
    
	//! ���ο� ĳ���͸� Ŭ���� ���Խ�Ų��
	//! \param dwClub Ŭ����ȣ
	//! \param dwChaNum ���Խ�Ű���� ĳ���͹�ȣ
    virtual int AddClubMember(DWORD dwClub, DWORD dwChaNum) override;

	//! ���ԵǾ� �ִ� ĳ���͸� Ŭ������ Ż���Ų��
	//! \param dwChaNum ĳ���͹�ȣ
    virtual int DeleteClubMember(DWORD dwChaNum) override;

	virtual int DeleteClubMemberWithNoPenalty( DWORD dwChaNum ) override { return 0; }

	//! ĳ������ Ŭ����ȣ�� �����´�.
	//! \param dwChaNum ĳ���͹�ȣ
    virtual int GetChaGuildNum(DWORD dwChaNum) override;

    virtual int SetClubRegion(DWORD dwRegionID, DWORD dwClub=0, float fTax=0) override;
    virtual int DelClubRegion(DWORD dwRegionID, DWORD dwClub=0) override;
    
	//! ���� ��������� ���� Ŭ�������͸� �����´�.
	//! \n ����:���ʿ��� ������ ���� ����.
	//! \param &vGUID_DB 
    virtual int GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB) override;

	/**
	* �� Ŭ������ �����Ѵ�
	* \param dwClub Ŭ����ȣ
	* \param dwChaNum ĳ���͹�ȣ
	* \param dwSubMasterFlags �÷��� (0 ���� ������ �Ϲݱ����� �ȴ�)
	* \return 
	*/
	int	SetClubMasterFlags(DWORD dwClub, DWORD dwChaNum, DWORD dwSubMasterFlags);

	virtual int SetClubAlliance(DWORD dwClubP, DWORD dwClubS) override;

	/**
	* ���� Ŭ���� ��ü�Ѵ�.
	* \param dwClubP �� Ŭ����ȣ (���� Ŭ���� ��)
	* \param dwClubS ���� Ŭ����ȣ (�� Ŭ�� ������ ���� ���� Ŭ��)
	* \return 
	*/
	int	DelClubAlliance(DWORD dwClubP, DWORD dwClubS);

	/**
	* ���� Ŭ�������� �����´�.
	* \n ���� ���� Logic�ʿ��� ������ �ȵȰ���� �����Ǿ� ���� �ʴ�.
	* \return 
	*/
	int	GetClubAlliance ( std::vector< std::pair<DWORD,DWORD> > &vecAlliance );
	
	/**
	* Ŭ�� ��Ʋ�� ��û�Ѵ�.
	* \param dwClubP ��Ŭ����ȣ( Ŭ�� ��Ʋ�� ��û�� ���� �� )
	* \param dwClubS ���� Ŭ����ȣ( Ŭ�� ��Ʋ�� ��û�� �� Ŭ�� )
	* \return
	*/
	int SetClubBattle( DWORD dwClubP, DWORD dwClubS, DWORD dwEndTime, int nAlliance );

	/**
	* Ŭ�� ��Ʋ�� �����Ѵ�.
	* \param dwClubP ��Ŭ�� ��ȣ( Ŭ�� ��Ʋ�� ��û�� ���� �� )
	* \param dwClubS ���� Ŭ�� ��ȣ( Ŭ�� ��Ʋ�� ��û�� �� Ŭ�� )
	* \param nGuFlag ���п���
	* \param nGuKillNum Ŭ�� ��Ʋ ų��
	* \return
	*/
	int EndClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance );

	/**
	* Ŭ�� ��Ʋ������ �����Ѵ�,( Ŭ�� ��Ʋ�� ����Ǳ��� ����������� �ʿ��Ѱ�� ��Ʋ ������ ������ �ʿ��� )
	* \param dwClubP ��Ŭ�� ��ȣ( Ŭ�� ��Ʋ�� ��û�� ���� ��)
	* \param dwClubS ���� Ŭ�� ��ȣ( Ŭ�� ��Ʋ�� ��û�� �� Ŭ�� )
	* \param nGuKillNum Ŭ�� ��Ʋ ų��
	* \return
	*/
	int SaveClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum );

	/**
	* ���͹�Ʋ ������ �ʱ�ȭ ��Ű���.
	* \param dwClub Ŭ�� ��ȣ
	* \return
	*/
	int ReSetAllianceBattle( DWORD dwClub );

	/**
	* �ش� Ŭ���� ��Ʋ���� Ŭ�� ������ �����´�.
	* \param dwClub Ŭ����ȣ
	* \param &vMember Ŭ����Ʋ ����
	* \return 
	*/
    int GetClubBattleInfo( DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo );
	
	virtual int UpdateClubName( DWORD dwClub, const std::string& ClubName ) override { return 0; }

	//! DB ���� ĳ���͸� �¶��� ���·� �����.
	//! \param nChaNum ĳ���͹�ȣ
	//! \return 
	virtual int SetCharacterOnline(int nChaNum) override;

	//! ĳ���͸� �������� ���·� �����.
	//! \param nChaNum ĳ���͹�ȣ
	virtual int SetCharacterOffline(int nChaNum) override;

	virtual int SetAllCharacterOffline() override;

	virtual int SetChaHairColor(DWORD dwChaNum, int nHairColor) override;

	virtual int SetChaHairStyle(DWORD dwChaNum, int nHairStyle) override;

	//! ĳ������ �󱼽�Ÿ���� �����Ѵ�.
	virtual int SetChaFaceStyle(DWORD dwChaNum, int nFaceStyle) override;

	//! ĳ������ ������ �����Ѵ�
	virtual int SetChaGenderChange(DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor) override;

	// ĳ���� �п��� �����Ѵ�
	virtual int SetChaSchoolChange(DWORD dwChaNum, int nSchool) override;

	//----------------------------------------------------------------------------
	// PET
	//----------------------------------------------------------------------------
	
	// typedef std::pair < DWORD, PETSKILL> PETSKILL_PAIR;

	/**
	* ���ο� ���� �����Ѵ�.
	* \param nChaNum  �� ������ ĳ���� ��ȣ
	* \param szPetName ���̸�
	* \param nPetType ���� Ÿ��
	* \param nPetMID �� MID
	* \param nPetSid �� SID
	* \param nPetStyle ���� ��Ÿ��
	* \param nPetColor ���� �÷�
	* \param m_nPetCardMID �� ī���� MID
	* \param m_nPetCardSID �� ī���� SID
	* \return sc::db::DB_ERROR �Ǵ� ������������ Pet �� ������ȣ
	*/
	int CreatePet(
		int nChaNum,
		const TCHAR* szPetName,
		int nPetType,
		int nPetMID,
		int nPetSID,
		int nPetStyle,
		int nPetColor,
		int nPetCardMID,
		int nPetCardSID
		);

	/**
	* ���ο� ���� �����Ѵ�.
	* \param nPetNum ���� ������ȣ
	* \param szPetName ���̸�
	* \return sc::db::DB_ERROR, sc::db::DB_OK
	*/
	int RenamePet(int nChaNum, int nPetNum, const TCHAR* szPetName);

	/**
	* ���� �������� �����Ѵ�.
	* \param nChaNum ���� ���ο� ������
	* \param nPetNum �������� ������ �� ��ȣ
	*/
	int ExchangePet(int nChaNum, int nPetNum);

	/**
	* ���� �÷��� �����Ѵ�.
	* \param nPetNum �÷��� �����Ϸ��� ���� ��ȣ
	* \param nPetColor ���� �÷�
	*/
	int SetPetColor(int nChaNum, int nPetNum, int nPetColor);

	/**
	* ���� ��Ÿ���� �����Ѵ�.
	* \param nPetNum ��Ÿ���� �����Ϸ��� ���� ��ȣ
	* \param nPetStyle ���� ��Ÿ��
	*
	*/
	int SetPetStyle(int nChaNum, int nPetNum, int nPetStyle);

	/**
	* ���� �������� �����Ѵ�. �������� 0-100 ����
	* \param nPetNum �������� ������ ���� ��ȣ
	* \param nPetFull ������
	*/
	int SetPetFull( int nChaNum, int nPetNum, int nPetFull );

	/**
	* ���� �κ��丮 ������Ʈ
	*/
	int SetPetSkill(
		int nChaNum,
		int nPetNum,
		int nPetInvenType,
		int nPetInvenMID,
		int nPetInvenSID,
		int nPetInvenCMID,
		int nPetInvenCSID,
		int nPetInvenAvailable);

	/**
	* ���� �κ��丮 ������Ʈ
	*/
	int SetPetInven(
		int nPetChaNum,
		int nPetNum,
		se::ByteStream &ByteStream );

	virtual int SetPetMigrationState( int ChaNum, int PetNum, int MigrationState ) override { return 0; }
	virtual int SetVehicleMigrationState( int ChaNum, int VehicleNum, int MigrationState ) override { return 0; }

	/**
	* ���� ��ų ���� ���� ������Ʈ
	*/
	int SetPetSkillSlotNum(
		int nChaNum,
		int nPetNum,
		WORD wSkillSlotNum );

	/**
	* ���� �����Ѵ�.
	* \param nPetNum ������ ���� ��ȣ
	*/
	int	DeletePet(int nChaNum, int nPetNum);
	
	virtual int GetPetSkill(int nChaNum, GLPET* pPet, DWORD dwPetNum) override;

	/**
	* ���� �κ��丮 ������ �����´�
	*/
	int GetPetInven( int nChaNum, GLPET* pPet, DWORD dwPetNum );

	/**
	* ���� ������ �����´�.
	*/
	virtual int GetPet(GLPetField* pPet, DWORD dwPetNum, DWORD dwClientID) override;

	/**
	* ���� �������� �����´�
	* \param dwPetNum �� ��ȣ
	* \return ���� ������ or sc::db::DB_ERROR
	*/
	int GetPetFull( int nChaNum, DWORD dwPetNum );
	
	virtual int GetRestorePetList(int nChaNum, std::vector<PET_RELIST> &vecPetReList) override;

	/**
	* ���� ��Ȱ��Ų��.
	* \param nPetNum ��Ȱ�� �� ��ȣ
	* \param nChaNum ��Ȱ�� �䱸�� ĳ���� ��ȣ
	*/
	int RestorePet( DWORD dwPetNum, int nChaNum );

	/**
	* ���� ��Ų���� ������Ʈ �Ѵ�.
	* \param nChaNum		�ּ��� ĳ���� ��ȣ
	* \param dwPetNum		�� ��ȣ
	* \param nPetSkinMID	�� ��Ų MID
	* \param nPetSkinSID	�� ��Ų SID
	* \param nPetSkinScale	�� ��Ų Scale
	* \param nPetSkinTime	�� ��Ų ����ð�
	*/
	int UpdatePetSkin( int nChaNum,
					   DWORD dwPetNum,
					   int nPetSkinMID,
					   int nPetSkinSID,
					   int nPetSkinScale,
					   int nPetSkinTime );

	/**************************************************************************
	* User Database
	*/

	/**
	* Taiwan / HongKong 
	* �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
	* \param szUsrID ����� ID
	* \param szPasswd ����� �н�����
	* \param szUsrIP ������� IP Address
	* \param nSvrGrp �����׷�
	* \param nSvrNum ������ȣ
	* \return DB ������ �����
	*/
	int	UserCheck(
		const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		const TCHAR* szRandomPasswd,
		int nRandomNum,
		int nSvrGrp,
		int nSvrNum);


	int IdnUserCheck(int nUserNum, const std::string &strId, const std::string &strIP, int nSvrGrp, int nSvrNum);


	/**
	* Thailand
	* �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
	* \param szUsrID ����� ID
	* \param szPasswd ����� �н�����
	* \param szUsrIP ������� IP Address
	* \param nSvrGrp �����׷�
	* \param nSvrNum ������ȣ
	* \return DB ������ �����
	*/
	int	ThaiUserCheck(
			const TCHAR* szUsrID,
		    const TCHAR* szPasswd,
			const TCHAR* szUsrIP,
			int nSvrGrp,
			int nSvrNum);

	/**
	* Daum Game (Korea)	
	* \param 
	* \param 
	* \param szUsrIP �α����Ϸ��� IP Address
	* \param nSvrGrp �����׷�
	* \param nSvrNum ������ȣ
	* \return
	*/
	virtual int DaumUserCheck(
			TCHAR* szDaumGID,
		    const TCHAR* szUUID,
			const TCHAR* szUserIP,
			int nSvrGrp,
			int nSvrNum,
            int& Age OUT) override;

	/**
	* GSP
	* \n �α����Ϸ��� ����ڸ� üũ�Ѵ�.
	* \param 
	* \param 
	* \param szUsrIP �α����Ϸ��� IP Address
	* \param nSvrGrp �����׷�
	* \param nSvrNum ������ȣ
	* \return 
	*/
	int GspUserCheck(
			TCHAR* szGspUserID,
		    const TCHAR* szUUID,
			const TCHAR* szUserIP,
			int nSvrGrp,
			int nSvrNum );
	/**
	* Terra (Malaysia)
	* \n �α����Ϸ��� ����ڸ� üũ�Ѵ�.
	* \param szTerraLoginName Terra TLoginName varchar(15)
	* \param szTerraTID Web ���� �Ѿ�� ���ڿ� (ENCODING ���ڿ�)
	* \param szUsrIP �α����Ϸ��� IP Address
	* \param nSvrGrp �����׷�
	* \param nSvrNum ������ȣ
	* \return 
	*/
	int TerraUserCheck(
			TCHAR* szTerraLoginName,
		    const TCHAR* szTerraDecodedTID,
			const TCHAR* szUsrIP,
			int nSvrGrp,
			int nSvrNum );
	/**
	* Excite (Japan)
	* \param szUserID �����ID
	* \param szUsrIP IP
	* \param nSvrGrp �����׷�
	* \param nSvrNum ������ȣ
	* \return 
	*/
	int ExciteUserCheck(
			const TCHAR* szUserID,
			const TCHAR* szUserIP,
			int nSvrGrp, 
			int nSvrNum );

	/**
	* Gonzo (Japan)
	* \param szUserID �����ID
	* \param szUsrIP IP
	* \param nSvrGrp �����׷�
	* \param nSvrNum ������ȣ
	* \return 
	*/
	int JapanUserCheck(
			const TCHAR* szUserID,
			const TCHAR* szUserIP,
			int nSvrGrp, 
			int nSvrNum,
			int nUserNum);


	/**
	* �۷ι� ����
	* �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
	* \param szUsrID ����� ID
	* \param szPasswd ����� �н�����
	* \param szUsrIP ������� IP Address
	* \param nSvrGrp �����׷�
	* \param nSvrNum ������ȣ
	* \return DB ������ �����
	*/
	int	GsUserCheck(
		const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		int nSvrGrp,
		int nSvrNum);

	/**
	* EU ����
	* �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
	* \param szUsrID ����� ID
	* \param szPasswd ����� �н�����
	* \param szUsrIP ������� IP Address
	* \param nSvrGrp �����׷�
	* \param nSvrNum ������ȣ
	* \return DB ������ �����
	*/
	int	GmUserCheck(
		const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		int nSvrGrp,
		int nSvrNum );

	/**
	* US ����
	* �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
	* \param szUsrID ����� ID
	* \param szPasswd ����� �н�����
	* \param szUsrIP ������� IP Address
	* \param nSvrGrp �����׷�
	* \param nSvrNum ������ȣ
	* \return DB ������ �����
	*/
	int	UsUserCheck(
		const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		int nSvrGrp,
		int nSvrNum );


	/**
	* �ش����ڸ� Logout �Ѵ�
	* \param szUsrID ����� ID
	* \param nUsrNum ����ڹ�ȣ
	* \param nGameTime �����÷��̽ð� 
	* \param nChaNum ĳ���͹�ȣ
	* \param nSvrGrp �����׷�
	* \param nSvrNum ������ȣ
	* \return 
	*/
	int	UserLogout(
			const TCHAR* szUsrID,
			const TCHAR* szUsrIP, 
			int nUsrNum=0,
			int nGameTime=0,
			int nChaNum=0,
			int nSvrGrp=0,
			int nSvrNum=0,
			int nTotalGameTime=0,
			int nOfflineTime=0 );

	/**
	* Terra (Malaysia)
	* �ش����ڸ� Logout �Ѵ�
	* \param szTID Terra User ID
	* \param nUsrNum ����ڹ�ȣ
	* \param nGameTime �����÷��̽ð�
	* \param nChaNum ĳ���͹�ȣ
	* \param nSvrGrp �����׷�
	* \param nSvrNum ������ȣ
	* \return 
	*/
	int	TerraUserLogout(
			const TCHAR* szTID,
			int nUsrNum=0,
			int nGameTime=0,
			int nChaNum=0,
			int nSvrGrp=0,
			int nSvrNum=0 );

	/**
	* Maya (Taiwan/Hongkong)
	* \n ����ڸ� �α׾ƿ� ��Ų��
	* \param szUsrID ����� ID
	* \return 
	*/
	int	UserLogoutSimple( const TCHAR* szUsrID );

	/**
	* Maya (Taiwan/Hongkong)
	* \n ����ڸ� �α׾ƿ� ��Ų��.
	* \param nUserNum ����ڹ�ȣ
	* \return 
	*/
	int UserLogoutSimple2( int nUserNum );

	//! �ش缭���� ����ڰ� �������� �α׾ƿ� ��Ų��
	//! \param szUsrID ����� ID
	//! \param nSvrGrp ���� �׷�
	virtual int UserLogoutSimple3(const TCHAR* szUsrID, int nSvrGrp) override;

    virtual int UserDbLogItemDataInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName) override;
	
	//! Feya (Taiwan/Hongkong)
	//! �ش� ���Ӽ����� ��ü ������ �α׾ƿ� ��Ų��.
	//! \param nSvrGrp �����׷�
	//! \param nSvrNum ������ȣ
	virtual int UserLogoutSvr(int nSvrGrp, int nSvrNum) override;
	
	/**
	* Feya (Taiwan/Hongkong)
	* \n DB ����� ������ ������ �÷����� ĳ���� �̸��� �����Ѵ�.
	* \param nUsrNum ����ڹ�ȣ
	* \param szChaName ĳ�����̸�
	* \return 
	*/
	int	UserUpdateCha(int nUsrNum, const TCHAR* szChaName);		
	
	/**	
	* �ش� ������� ����� ��ȣ�� �����´�.
	* \param szUsrId ����� ID
	* \return ����ڹ�ȣ
	*/
	int	GetUserNum(const TCHAR* szUsrId);

	int GetUserChaRemain(int nUserNum);
	int	GetUserChaTestRemain(int nUserNum);
	int	UserChaNumDecrease(int nUserNum);
	/*
	* TEST ���� ĳ���� �������� ����
	*/
	int	UserTestChaNumDecrease(int nUserNum);

	/**
	* ĳ���� ����, ĳ���ͻ��� ���� ����
	*/
	int UserChaNumIncrease(int nUserNum);

	/**
	* TEST ���� ĳ���� ����, ���� ����
	*/
	int UserTestChaNumIncrease(int nUserNum);
	
	virtual int GetUserType(int nUserNum) override;
	virtual int GetUserCountryInfo ( IN int nUserNum, OUT Country::SCOUNTRY_INFO& sCountryInfo ) override;

	/**	
	* �ش� ������� �����̾� �Ⱓ�� �����´�.
	* \param nUserNum ����ڹ�ȣ
	* \return �����̾��Ⱓ(������)
	*/
    __time64_t GetPremiumTime(int nUserNum);	

	/**	
	* �ش� ������� ���� ���� ��¥�� �����´�.
	* \param nUserNum ����ڹ�ȣ
	* \return ���� ���� �ð�(������)
	*/
	__time64_t GetCreateDate(const int nUserNum);	
    
	/**	
	* �ش� ������� �����̾� �Ⱓ�� �����Ѵ�.
	* \param nUserNum ����ڹ�ȣ
	* \param tPremiumTime �����̾� ����ð�
	* \return 
	*/
    int SetPremiumTime(int nUserNum, __time64_t tPremiumTime);	

	/**
	* �ش� ������� ä�� ��Ͻð��� �����´�
	* \param nUserNum ����ڹ�ȣ
	* \return ä�ú�� ����ð�
	*/
	__time64_t GetChatBlockTime(int nUserNum);

	//! �ش� ������� ä�� ��Ͻð��� �����Ѵ�
	//! \param nUserNum ����ڹ�ȣ
	//! \param tBlockTime ä�ú�� ����ð�
	int SetChatBlockTime(int nUserNum, __time64_t tBlockTime);

	//int AddBlockIPList( TCHAR* szIP, TCHAR* szReason );
	virtual int AddLogServerState(const __time64_t& LogDate, int nSGNum=0, int nSvrNum=0, int nUserNum=0, int nUserMax=1) override;
	
	//
	//mjeon
	//Add PingLog
	//
	int AddLogPing( int nUserNum, int nRTT, const char* szIP, __time64_t tTime );

	//
	//mjeon.Post.Send
	//
	int PostSend(SPOSTINFO &PostInfo, SITEMCUSTOM &itemcustom);
	int PostSendRollBack(LONGLONG &llPostID);
	int PostOpenAndTake(LONGLONG llPostID, BOOL bMoneyAccept, BOOL m_bAttach01Accept, BYTE bChargePayment, BYTE bReturnedPost );

	//mjeon.Post.Maintenance
	int PostMaintenance();

	//mjeon.Post.Clean
	int PostCleanUp(char *pszCurrentDate, char *pszTargetDate);

	
	//int PostCheckAttachMoney(DWORD dwClientID, SPOSTINFO &PostInfo, SINVENITEM *pItem);

	USER_INFO_BASIC GetUserInfo( const TCHAR* szUsrId, const TCHAR* szPasswd );
	int DaumGetUserInfo( const TCHAR* szGID, DAUM_NET_LOGIN_FEEDBACK_DATA2* pMsg );
	
	/**
	* ����� ������ �����´�
	* Taiwan / HongKong
	* \param szUserID 
	* \param pMsg 
	* \return 
	*/
	int GetUserInfo( const TCHAR* szUserID, NET_LOGIN_FEEDBACK_DATA2* pMsg );


	//
	//mjeon.indonesia
	//
	int IdnGetUserInfo(const std::string &strId, NET_LOGIN_FEEDBACK_DATA2* pMsg);


	/**
	* ����� ������ �����´�
	* Thailand
	* \param szUserID 
	* \param pMsg 
	* \return 
	*/
	int ThaiGetUserInfo( const TCHAR* szUserID, NET_LOGIN_FEEDBACK_DATA2* pMsg );

	/**
	* Terra ����� ������ �����´�.
	* \param szTLoginName TLoginName varchar(15)
	* \param pMsg
	* \return 
	*/
	int TerraGetUserInfo( const TCHAR* szTLoginName, TERRA_NET_LOGIN_FEEDBACK_DATA2* pMsg );

	/**
	* GSP ����� ������ �����´�.
	* \param szUserID UserID varchar(50)
	* \param pMsg
	* \return 
	*/
	int GspGetUserInfo( const TCHAR* szUserID, GSP_NET_LOGIN_FEEDBACK_DATA2* pMsg );
	
	//! Excite ����� ������ �����´�.
	//! \param szUserID UserID varchar(20)
	//! \param pMsg
	virtual int ExciteGetUserInfo(const TCHAR* szUserID, EXCITE_NET_LOGIN_FEEDBACK_DATA2* pMsg) override;

	//! Japan(Gonzo) ����� ������ �����´�.
	//! \param szUserID UserID varchar(16)
	//! \param pMsg
	virtual int JapanGetUserInfo(const TCHAR* szUserID, JAPAN_NET_LOGIN_FEEDBACK_DATA2* pMsg) override;
	virtual int WorldBattleUserCheck(
		const std::string& UserId,
		const std::string& UserPass,
		const std::string& UserIp,
		int ServerGroup,
		int ServerNumber) override;
    virtual int WorldBattleGetUserInfo(const std::string& UserId, NET_LOGIN_FEEDBACK_DATA2& Msg) override;

	//! �۷ι����� ����� ������ �����´�.
	//! \param szUserID UserID varchar(16)
	//! \param pMsg	
	virtual int GsGetUserInfo(const TCHAR* szUserID, GS_NET_LOGIN_FEEDBACK_DATA2* pMsg) override;

	//! EU ����� ������ �����´�.
	//! \param szUserID UserID varchar(16)
	//! \param pMsg	
	virtual int GmGetUserInfo( const TCHAR* szUserID, GM_NET_LOGIN_FEEDBACK_DATA2* pMsg ) override;

	//! US ����� ������ �����´�.
	//! \param szUserID UserID varchar(16)
	//! \param pMsg	
	virtual int UsGetUserInfo( const TCHAR* szUserID, US_NET_LOGIN_FEEDBACK_DATA2* pMsg ) override;
	
	/**
	* �ش����ڸ� ��Ͻ�Ų��.
	* \param nUserNum ����ڹ�ȣ
	* \param nDay ���ϼ� 1 - 15 ����
	* \return 
	*/
    int UserBlock( int nUserNum, int nDay );

	/**
	* �ش����ڸ� ��Ͻ�Ų��.
	* \param nUserNum ����ڹ�ȣ
	* \param nHour �ð�
	* \return 
	*/
    int UserBlockDate( int nUserNum, __time64_t tBlockTime );
	
	/**
	* �ش� ���Ӽ����� ��ü ������ �α׾ƿ� ��Ų��.
	* \param nSvrGrp 
	* \return 
	*/
	int AllServerUserLogout(int nSvrGrp);

	/**
	* ��ü ������ �α׾ƿ� ��Ų��.
	* \return 
	*/
	int	AllUserLogout();

	/**
	* TERRA
	* ������ �н����带 �ְų� üũ�Ѵ�.
	* \param szTID Terra UserID
	* \param szPasswd ����� �н�����
	* \return 
	*/
	int TerraUserPassCheck(
				const TCHAR* szTID,
				const TCHAR* szPasswd,
				int	nCheckFlag );

	/**
	* Daum
	* ������ �н����带 �ְų� üũ�Ѵ�.
	* \param szDaumGID Daum UserGID
	* \param szDaumPass ����� �н�����
	* \return 
	*/
	int DaumUserPassCheck(
				const TCHAR* szDaumGID,
				const TCHAR* szDaumPasswd,
				int	nCheckFlag );


	/**
	* Excite
	* ������ �н����带 �ְų� üũ�Ѵ�.
	* \param szUID Excite UserID
	* \param szUserID Excite 2�� UserID
	* \param szPasswd ����� �н�����
	* \return 
	*/
	int ExciteUserPassCheck(
					const TCHAR* szUID,
					const TCHAR* szUserID,
					const TCHAR* szPasswd,
					int	nCheckFlag );
	
	virtual int ThaiUserClassCheck(const TCHAR* szUsrIP) override;

	/**
	* Malaysia
	* ������� PC�� Ŭ������ üũ�Ѵ�.	
	* \param szUsrIP ������� IP Address
	* \return DB ������ �����
	*/
	int	MyUserClassCheck(
			const TCHAR* szUsrIP );

	/**
	* �ʸ���
	* ������� PC�� Ŭ������ üũ�Ѵ�.	
	* \param szUsrIP ������� IP Address
	* \return DB ������ �����
	*/
	int	PhUserClassCheck(
			const TCHAR* szUsrIP );
	
	/**
	* �ش� ������� ���� �����÷��� �ð�(��)�� �����´�.
	* Thailand
	* \param nUserNum 
	* \return 
	*/
	WORD ThaiGetTodayPlayTime (int nUserNum);

	/**************************************************************************
	* Log Database
    */

	// int LogServerState(int nSGNum=0, int nSvrNum=0, int nUserNum=0, int nUserMax=0);
	int	LogChaDeleted(
			int nSGNum=0,
			int nSvrNum=0,
			int nUserNum=0,
			int nChaNum=0 );

	int	LogItemExchange(
			const LOG_ITEM_EXCHANGE &sEXCHANGE );

	int	LogMoneyExchange(
			const LOG_MONEY_EXCHANGE &_sLOG );

	int LogPointShopExchange(int nFromType, DWORD ChaDbNum, int nExchangeFlag, WORD ItemMid, WORD ItemSid, int nPoint, int nExchangeFlagDetail );

	int LogItemConversion(
			const LOG_ITEM_CONVERSION &_sLOG );

    int	LogAction(const LOG_ACTION& LogData);


	//
	//mjeon.CaptureTheField
	//
	virtual int CTFPlayLog(UINT nChaNum, WORD wSchool, int nTotalPoint, int nRewardPoint) override;
	virtual int GetCTFInfo(UINT rowcount ,std::vector<DWORD >& vAuthSchool) override;
	virtual int LogGuidance(const LOG_GUIDANCE &sLog) override;
	virtual int LogCDM(const LOG_CDM& sLog) override;	
	virtual int LogCDMReuslt(const LOG_CDM_RESULT& sLog) override;


	/**
	* �������� �����ɼ� �α׸� �����.
	* \param _sLOG �����ɼ� ����ü
	*/
	int LogRandomItem( const LOG_RANDOM_OPTION &_sLOG );
	int LogItemCostumeStat(const LOG_COSTUME_STATS &_sLOG);

	virtual int LogAddonItem( const LOG_ADDON_OPTION &_sLOG ) override { return 0; }
	virtual int LogLinkSkillItem( const LOG_LINKSKILL_OPTION &_sLOG ) override { return 0; }
	virtual int LogBasicStatItem( const LOG_BASICSTAT_OPTION &_sLOG ) override { return 0; }

	/**
	* ���� �׼Ƿα׸� �����.
	* \param _sLOG ��׼Ƿα� ����ü
	*/
	int LogPetAction( const LOG_PET_ACTION &_sLOG );

	//! �������� PC �� ���θ�� ���ǰ���'��������!!' �̺�Ʈ
    /*
	int	LogLottery(
			CString strLottery, 
		    CString strIP, 
			CString strUserUID, 
			int nUserNum );
    */

	// �������� PC ���θ�� ������ '����' �̺�Ʈ
    /*
	int LogPungPungCheck(
			CString strIP, 
		    int nUserNum );
    */    
	// �������� PC ���θ�� ������ '����' �̺�Ʈ
    /*
	int LogPungPungWrite(
			CString strIP, 
		    CString strUserUID, 
			int nUserNum, 
			int nPrize );
    */
    virtual LONGLONG GetItemMaxNum(
        int nSGNum, 
        int nSvrNum, 
        int nFldNum, 
        std::vector<VIEWLOGITEMEXCHANGEMAX>& v);
	
    virtual int GetLogItem(std::vector<DWORD>& LogItemData) override;

	//! ���ӳ����� �����̳� �Ű�� �Ű��� ������ DB�� ����Ѵ�.
	//! \param nSGNum �����׷�
	//! \param nChaNum �Ű��� ĳ���͹�ȣ
	//! \param strMsg �Ű��� (�ִ�ũ��� CHAT_MSG_SIZE)
    virtual int LogAppeal(int nSGNum, int nChaNum, CString strMsg) override;

	virtual int LogHackProgram(
		int nSGNum, 
		int nSvrNum,
		int nUserNum,
		int nChaNum,
		int nHackProgramNum,
		const TCHAR* szComment) override;

	virtual int LogSMS(
		int nSendChaNum,		
		int nReceiveChaNum,
		const TCHAR* szReceivePhone,
		const TCHAR* szSmsMsg) override;

	virtual int SetChaPhoneNumber(int nChaNum, const TCHAR* szPhoneNumber) override;

	/**
	* ĳ������ ģ�������� �����´�.
	* \param nChaNum ģ�������� ������ ĳ���� ��ȣ
	* \param vFriend ģ�������� ���� vector
	*/
	int	GetChaFriendSMS(
			int nChaNum,
			std::vector<CHA_FRIEND_SMS> &vFriend );

	/**************************************************************************
	* Shop Database
    */
    
    //! Item Shop ���� ������ �������� �����´�
    //! Shop ���� ������ �������� �����´�.
    //! ������ �������� ������ ���� �� �� �ִ� �������� �ƴϴ�.
    //! �������� ���������� ������ ������ �� �ִ��� �ٽ� Ȯ���ؾ� �Ѵ�. (SetPurchaseItem)
    //! ���� ���� ���ſ� ��� �� ���ӳ� ĳ������ ����ȭ ����.	
	virtual int GetPurchaseItem(const CString& strUID, std::vector<SHOPPURCHASE>& v) override;
    
	virtual int SetPurchaseItem(const CString& strPurKey, int nFlag=1) override;

	//! -----------------------------------------------------------------------
	//! Vehicle Database

	// VEHICLE
	//----------------------------------------------------------------------------	
	virtual int CreateVehicle(
        int nCharNum,
		const TCHAR* szVehicleName,
		int nVehicleType,
		int nVehicleCardMID,
		int nVehicleCardSID) override;

	virtual int SetVehicleBattery(int nVehicleNum, int nCharNum, int nVehicleBttery) override;
	virtual int GetVehicleBattery(int nVehicleNum, int nCharNum) override;	
	virtual int LogVehicleAction(const LOG_VEHICLE_ACTION &_sLOG) override;
	virtual int DeleteVehicle(int nVehicle, int nCharNum) override;
	virtual int RebuyVehicle(int nVehicle, int nCharNum) override;
	virtual int GetVehicle(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) override;
	//virtual int GetVehicleInven(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) override;
	virtual int SetVehicleInven(int nVehicle, int nCharNum, se::ByteStream &ByteStream) override;
	//virtual int GetVehicleColor(GLVEHICLE* pVehicle, int nVehicleNum, int nCharNum) override;
	virtual int SetVehicleColor(int nVehicle, int nCharNum, se::ByteStream &ByteStream) override;
	virtual int SetVehicleBooster(int nVehicleNum, int nCharNum, DWORD dwVehicleBooster) override;	

	virtual int LogUserAttend(int nUserNum, std::vector<USER_ATTEND_INFO> &vecUserAttend) override;

	virtual int InsertUserAttend(int nUserNum, CString strUserName, __time64_t tAttendTime, int nCount, int nAttendReward, int nItemMID, int nItemSID) override;

	int InsertShopItemMap( int nItemMID, int nItemSID );

	//	UserMoney Add
	int UpdateUserMoneyAdd( DWORD dwUserID, LONGLONG lnUserMoney );
	
	//	UserLastInfo Add
	int UpdateUserLastInfoAdd( DWORD dwUserID, LONGLONG lnUserMoney );

    // PointShop
    virtual int PointShopList(__time64_t& UpdateTime, std::vector<pointshop::SellItem>& ItemList, bool bStandAlone) override { return 0; }
    virtual int GetChaPoint(int ChaDbNum, __int64& PointRefundable, __int64& PointNotRefundable) override { return 0; }
    virtual int SetChaPoint(int ChaDbNum, const __int64& PointRefundable, const __int64& PointNotRefundable) override;
    virtual int LogPointShopInsert(DWORD ChaDbNum, const std::string& LogMessage) override { return 0; }
	virtual int LogPointShopInsert(
		DWORD ChaDbNum, int LogType, 
		__int64& BeforeRefundable, 
		__int64& BeforeNotRefundable, 
		__int64& AfterRefundable, 
		__int64& AfterNotRefundable, 
		WORD ItemMid, 
		WORD ItemSid) override { return 0; }
    virtual int ShopSaleLog(
        const SNATIVEID& ItemId,
        __int64 Price,
        db::EM_BILLING_SELL_TYPE Type,
        int UserDbNum,
        int ServerNum,
        int ChaDbNum,
        int ChaLevel) override 
    { 
        return 0;
    }

	//mjeon.CaptureTheField
	virtual int CTFStatLog(UINT *arrPlayerNum, UINT state)									override { return 0; }
	virtual int CTFResultLog(WORD wSchoolAuthed, CTF_REWARD_BUFF *arrReward, UINT nHowMany)	override { return 0; }
	virtual int LoadChaExtraInfo(UINT nChaNum, int &nContributionPoint)						override { return 0; }
	virtual int SaveChaExtraInfo(UINT nChaNum, int nContributionPoint)						override { return 0; }

    virtual int LogItemListInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName) override;
	virtual int LogQuestListInsertUpdate( int Mid, int Sid, const std::string& Name ) override { return 0; }
	virtual int LogSkillListInsertUpdate( int Mid, int Sid, const std::string& Name, int Grade ) override { return 0; };

	virtual int LogGmCommandInsert( int ChaNum, const std::string& Command, const std::string& Param ) override { return 0; }

	virtual int LogGMChat( int nChatType, int nGMType, const std::string& SendName, const std::string& RecvName, const std::string& ChatMsg ) override { return 0; }

    virtual int UserCountryUpdate(int UserDbNum, wb::EM_COUNTRY Country) override;

	/**************************************************************************
	* etc
    **************************************************************************/
	int	Command( TCHAR* szCmdStr );

    virtual int ExecuteSqlLogDb(const std::string& SqlStr) override;

    virtual int LogMiniGameOddEvenInsert(
        DWORD UserDbNum,
        DWORD CharDbNum,        
        BYTE  CurrentRound,
        BYTE  WinLost,
        __int64 BattingMoney,
        __int64 DividendMoney,
        __int64 ChaMoney) override;

    virtual int ChaFriendGroupSelect(int ChaDbNum, std::vector<FRIEND_GROUP>& vGroup) override;
    virtual int ChaFriendGroupInsert(int ChaDbNum, WORD GroupNum, const std::string& GroupName) override;
    virtual int ChaFriendGroupUpdate(int ChaDbNum, WORD GroupNum, const std::string& GroupName) override;
    virtual int ChaFriendGroupDelete(int ChaDbNum, WORD GroupNum) override;
    virtual int ChaFriendGroupSet(int ChaP, int ChaS, WORD GroupNum) override;

    virtual int ClubNickUpdate(DWORD ChaDbNum, const std::string& NickName) override;
    virtual int ClubPublicFlagUpdate(DWORD ClubDbNum, DWORD Flag) override;
    virtual int ClubAutoKickUpdate(DWORD ClubDbNum, size_t GradeIndex, WORD Day, bool AutoKickOut) override;
    virtual int ClubNewbieNoticeUpdate(DWORD ClubDbNum, const std::string& Notice) override;
    virtual int ChaClubSecedeTimeReset(DWORD ChaDbNum) override;

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
        ) override { return sc::db::DB_ERROR; };

    // ���� ���� ����;
    virtual int CompetitionRatingUpdate(
        const std::string& strContentType,
        const DWORD charDbNum,
        const int Rating,
        const int WinningStreak,
        const int PlayCount,
        const __time64_t tLastPlayDate
        ) override { return sc::db::DB_ERROR; };

    // ���� ���� ������Ʈ;
    virtual int CompetitionSeasonUpdate(
        const std::string& strContentType,
        const int seasonNum,
        const int baseRating
        ) override { return sc::db::DB_ERROR; };

    // ���� �α� CompetitionLog


	// ���� ���� �α� ���
	virtual int AddAuthServerState( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, int nCurUser, int nMaxUser, const __time64_t& LogDate ) override;
	// ���� ���� ����
	virtual int ProcessCertificationForAuth( int isSessionSvr, int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::string strUniqKey, int &certification, std::string &strNewUniqKey, int &nSessionSvrID ) override;
	// �̺�Ʈ ���� ���
	virtual int AddAuthEventInfo( G_AUTH_EVENT_INFO *eventInfo ) override;

	virtual int AddAuthServerConnectionState( int nSessionSvrID, int nCounrty, int nServerType, const TCHAR* szIP, int nPort ) override;
	virtual int AddAuthServerConnectionStateInit() override;
	virtual int AuthCountryAccessApproveSelect( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::vector<int>& vecIndex OUT ) override { return 0; };

	virtual int TexasHoldemChipMapSelect( std::vector< TexasHoldem::sChipPrice >& vecChip ) override { return 0; }
	virtual int TexasHoldemChipMapVersionSelect( LONGLONG& Version ) override { return 0; }
	virtual int	TexasHoldemChipSelect( SCHARDATA2* pChaData2 ) override { return 0; }
	virtual int	TexasHoldemChipUpdate( DWORD dwChaDbNum, LONGLONG& chip, bool bPlus = false ) override { return 0; }
	virtual int	TexasHoldemChipLogInsert( DWORD dwChaDbNum, LONGLONG& chip, LONGLONG& GameMoney, LONGLONG& Point ) override { return 0; }
	virtual int	TexasHoldemChipSelectFromWeb( int nServerGroup, DWORD dwChaDbNum, LONGLONG& chip ) override { return 0; };
	virtual int	TexasHoldemAuthKeyInsert( DWORD dwChaDbNum, const std::string& strGUID ) override { return 0; }
	virtual int	TexasHoldemGameTimeInsert( DWORD dwChaDbNum, DWORD dwUserNum, int nSecond ) override { return 0; }

    //! ITEM ID�� ���� �������� ������ �´�. ( bDateCheck => ��¥(Date)�˻� ���� )
    virtual int ConsignmentsSaleGet( sc::MGUID& Guid, private_market::ConsignmentSaleItem& SaleItem, bool bDateCheck = false  ) override;
    //! ChaNum�� ���� ����� ��ǰ�� ������ �´�. 
    virtual int ConsignmentsSaleSelect( private_market::SALE_ITEMS_VECTOR& vecItem, const DWORD& dwChaNum = 0, const int& ServerGroup = 0 ) override;
    //! ���� ������ �ٲ۴�.
    virtual int ConsignmentsUpdateTurnNum( sc::MGUID& Guid, int nTurnNum ) override;
    virtual int ConsignmentsSaleInsert( private_market::ConsignmentSaleItem& SaleItem ) override;
    virtual int ConsignmentsSaleDelete( sc::MGUID& Guid ) override;
    virtual int	ConfignmentSaleSlotExpansionDateSelect( SCHARDATA2* pChaData2 ) override;;
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
		WORD		wRemainingTime ) override { return 0; }
	virtual int MacroLogInsert(DWORD dwChaDbNum, WORD  wState) override { return 0; }

	virtual int ItemPeriodExLogInsert(DWORD  chaNum
		, WORD itemMid
		, WORD itemSid	
		, WORD remainNum
		, __time64_t previousPeriod
		, __time64_t currentPeriod) override { return 0; }


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
		int languageCode ) override { return 0; }

	virtual int	GetChaResetInfo( DWORD dwChaDbNum ) override { return 0; }

	// �ʸ��� ��������
	virtual int	IsMigrationTarget( DWORD dwChaDbNum ) override { return 0; }
	virtual int GetCharacterInfoForMigration( int nUserNumber, int nChaNum, SCHARDATA2* pChaData2 ) override { return 0; }
	virtual int SetMigrationTarget( DWORD dwChaDbNum ) override { return 0; }

	//////////////////////////////////////////////////////////////////////////
	//
	// TOURNAMENT BETTING SYSTEM
	//
	virtual int TournamentBettingSelect( DWORD ClientDBid, DWORD BettingStatus ) override;
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
    virtual int WriteGMLog(int nGmNum, std::string& strCmd, std::string& strUserIP) override { return 0; };

	// �������
	virtual int RanMobileRequest(DWORD dwUserDBNum, DWORD dwChaDBNum, int nServerNum, int nCommand, int nValue, const std::string& strChaName, int nChaClass ) override;

	// Log Money;
	virtual int MoneyLogInsert ( const int _nMainType, const int _nSubType, const LONGLONG _llMoney ) { return sc::db::DB_ERROR; }

	//////////////////////////////////////////////////////////////////////////
	// �Ⱓ�� ������ �ð� ����
	virtual int InspectionRewardPolicySelect() override { return 0; }
	virtual int InspectionRewardPolicyInsert( const __time64_t tStart, const __time64_t tEnd, const int nRewordMinute ) override { return 0; };
	virtual int InspectionRewardGetMinute( SCHARDATA2* pChaData2 ) override { return 0; };

	virtual int MoneyNotifyInsert( DWORD dwChaDbNum, __time64_t tDate, LONGLONG llOldMoney, LONGLONG llNewMoney, int nCause, int nChannel, int nField ) override { return 0; }

	// PostRenew
	// PostRenew
	virtual int PostRnSelectAll( std::vector<PostInfo>& vecList OUT, DWORD dwRecvChaNum ) override { return 0; };
	virtual int PostRnSetRead( DWORD dwPostID )		override { return 0; };
	virtual int PostRnDelAttach( DWORD dwPostID )	override { return 0; };
	virtual int PostRnDel( DWORD dwPostID )			override { return 0; };
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
		std::string strContent) override { return 0; }
	virtual int PostRnReturn( DWORD dwPostID, DWORD dwReturnChaNum, std::string strReturnChaName, DWORD dwReturnUserNum, DWORD dwRecvChaNum  )	override { return 0; }
	virtual int PostCharacterCheck(DWORD dwRecvChaDbNum) override { return 0; }
	virtual int GetinfofromCharDb(const char *szCharName, int *pnChaNum, int *pnUserNum, int *pClass, int *Lv) override { return 0; }

	virtual int TransDBDataToWorldBattle(SCHARDATA2* pCharData2, DWORD dwChaNum) override;
	virtual int CalculateExpCompressor( DWORD dwChaDbNum, __int64 nCurExp, __int64 nCompressedExp ) override;

	virtual int GetCharacterItemTransferTarget( int UserNum, std::vector< DWORD >& vecChaNum ) override { return 0; }

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

	// item
	virtual int ItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) override { return 0; }
	virtual int DeletedItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) override { return 0; }
	virtual int ItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) override { return 0; }
	virtual int EntireItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE* pItem, int CallType ) override { return 0; }
	virtual int ItemDelete( sc::MGUID& Guid, int State, int CallType ) override { return 0; }
	virtual int CompletelyItemDelete( sc::MGUID& Guid, int State, int CallType ) override { return 0; }
	virtual int ItemSaleToNpc( sc::MGUID& Guid, int State, int CallType ) override { return 0; }
	virtual int ItemSelect( DWORD ChaDbNum, INVEN_TYPE InvenType, std::vector< SINVENITEM_SAVE >& vecItems ) override { return 0; }
	virtual int ItemSelect( DWORD ChaDbNum, INVEN_TYPE InvenType, std::multimap< DWORD, SINVENITEM_SAVE >& mapItems ) override { return 0; }
	virtual int GetPutonItems( DWORD ChaDbNum, SCHARDATA2* pChaData2 ) override { return 0; }
	virtual int GetInvenItems( DWORD ChaDbNum, SCHARDATA2* pChaData2 ) override { return 0; }
	virtual int GetLockerItems( DWORD UserNum, SCHARDATA2* pChaData2 ) override { return 0; }
	virtual int SavePetInven( int nChaNum, int nPetNum, std::vector< SINVENITEM_SAVE >& vecItems ) override { return 0; }
	virtual int SaveVehicleInven( int nVehicle, int nCharNum, std::vector< SINVENITEM_SAVE >& vecItems ) override { return 0; }
	virtual int GetClubLockerItems( DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems ) override { return 0; }
	virtual int SaveClubLockerItems( DWORD ClubNum, std::vector< SINVENITEM_SAVE >& vecItems ) override { return 0; }

	virtual int TransferItems( DWORD UserNum, std::vector< DWORD >& vecChaNum ) override { return 0; }
	virtual int TransferClubLockerItems( DWORD ClubNum = 0 ) override { return 0; }
	virtual int GetGuildItemTransferStatus( DWORD ClubNum, int& Result, int& MigrationState ) override { return 0; }
	virtual int TransferLockerItems( DWORD UserNum = 0 ) override { return 0; }
	virtual int GetChaNumToTransfer( std::vector< DWORD >& vecChaNum ) { return 0; }
	virtual int TransferCharacterItems( DWORD ChaDbNum = 0 ) override { return 0; }

	virtual int ItemUpdatePos( DWORD ChaDbNum, INVEN_TYPE InvenType, sc::MGUID& Guid, WORD PosX, WORD PosY, int CallType ) override { return 0; }
	virtual int ItemUpdateTurnNum( sc::MGUID& Guid, WORD TurnNum, int CallType ) override { return 0; }
	virtual int ItemInsertError( sc::MGUID& Guid, int CallType, __time64_t& Date ) override { return 0; }

	virtual int GetItemTransferStatus( DWORD UserNum, int& Result ) override;
};

#endif // _CODBCMANAGER_H_
