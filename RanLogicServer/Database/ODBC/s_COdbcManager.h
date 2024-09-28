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
	COdbcSupervisor* m_pMalaysiaDB; //! 말레이시아 PC방 이벤트

public:
    virtual bool IsAlive() override;
    virtual bool Reconnect() override; //! 재접속 체크
    virtual bool CheckHdd() override; //! Hard disk 용량 체크
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
    //! 말레이시아 PC방 이벤트
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
	//! 사용자 인벤토리에서 데이타를 읽어온다.
	int	ReadUserInven(
			int SGNum,
			DWORD dwUserNum,
			se::ByteStream &ByteStream );

	//! 유저인벤토리가 있는지 체크한다.
	bool CheckInven( int SGNum, DWORD dwUserNum );
	int	MakeUserInven( int SGNum, DWORD dwUserNum );
	//! 사용자 인벤토리에 데이타를 저장한다.
	int	WriteUserInven( 
			int SGNum, 
			LONGLONG llMoney /* LONGLONG __int64 */,
			DWORD dwUserNum,
			BYTE* pData,
			int nSize);
	
	// 새로운 캐릭터를 생성한다.
	virtual int	CreateNewCharacter( SCHARDATA2* pCharData2 ) override;
	//mjeon.ado
	virtual int CreateNewCharacterExtraWithAdo(SCHARDATA2* pCharData2) {return 0;}

    //! ChaInfo Backup.
    virtual int CharacterInfoBackup(SCHARDATA2* pCharData2) override;


	// 대만,홍콩
	// 캐릭터 생성시 Temp Table에 캐릭터 레벨과 금액 정보를 암호화 해서 저장해 둔다.
	int	InsertCharInfoTemp(int nChaNum);

	virtual int ReadUserInven(SCHARDATA2* pChaData2);
	virtual int	ReadUserInvenNew( SCHARDATA2* pChaData2, int FirstLoad = 0 ) { return 0; }

	virtual int UpdateUserLockerOption( DWORD UserNum, int Option ) override { return 0; }

	//! 캐릭터를 저장한다
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
	* 캐릭터 정보를 가져온다 (바이너리 제거 데이타)
	* \param nUserNum 사용자번호
	* \param nChaNum 캐릭터번호
	* \return 캐릭터정보
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

	virtual int	Certification2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) override; // 2차비번
	virtual int	Update2ndPassword(int nUserNumber, int nChaNum, const char* sz2ndPass) override; // 2차비번
	
	//! 해당 사용자의 캐릭터 정보를 가져온다.
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

	// 실제 캐릭터 삭제 함수
	int	DeleteCharacter( int nUsrNum, int nChaNum );


	// 베트남 탐직방지 시스템의 추가 정보 불러오는 부분 추가
	int COdbcManager::GetVTCharInfo( int nChaNum, SCHARDATA2* pChaData2 );
	// 대만 해킹 문제 관련해서 최근 로그아웃 했을때의 정보를 DB에서 읽어옴.
	int COdbcManager::GetLastCharInfo( int nUserNum, int nChaNum,SCHARDATA2* pChaData2 );

	// 말레이시아/태국의 생성한 유저 수를 체크 한다.
	int	GetUserTempInfo( int nUserNum );
	int SetUserTempInfo( int nUserNum, int nCreateChar );

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

	//! 캐릭터의 친구정보를 가져온다
	virtual int GetChaFriend(int nChaNum, std::vector<CHA_FRIEND>& vFriend) override;
	virtual int	DelChaFriend(int nChaP, int nChaS) override;
	virtual int AddChaFriend(int nChaP, int nChaS) override;
	virtual int SetChaFriend(int nChaP, int nChaS, int nFlag) override;
    virtual int ChaFriendFlagSelect(int ChaP, int ChaS, int& Flag) override;

	//! 학교vs학교 파티대련결과저장
    virtual int	LogPartyMatch(int nSGNum, int nSvrNum, WORD wWin, WORD wLost) override;
    
    virtual int SetChaExp(int nChaNum, LONGLONG llExp) override;
    
    virtual LONGLONG GetChaExp(int nChaNum) override;
    
	virtual int SetChaStorageDate(int nUserNum, int nStorageNum, __time64_t tTime) override;
    
	virtual int SetChaInvenNum(int nChaNum, WORD wInvenLine=0) override;

    virtual int SetLastCallPos(int nChaNum, DWORD dwMapID, D3DXVECTOR3 vPos) override; 	
    virtual int CreateClub(const std::string& ClubName, DWORD ChaDbNum, wb::EM_COUNTRY Country) override;	
    virtual int DeleteClub(DWORD dwClub, DWORD dwChaNum) override;

	/**
	* 클럽랭킹을 세팅한다.
	* \param dwClub 클럽번호
	* \param dwRank 랭킹
	* \return 
	*/
    int SetClubRank(DWORD dwClub, DWORD dwRank);    

	/**
	* 클럽마크 이미지를 가져온다.
	* \param dwClub 클럽번호
	* \param &ByteStream 버퍼
	* \return 
	*/
    int ReadClubMarkImage(DWORD dwClub, se::ByteStream &ByteStream);    

	/**
	* 클럽마크 이미지를 저장한다.
	* \param dwClub 클럽번호
	* \param dwMarkVer 클럽마크버전
	* \param pData 클럽이미지 주소
	* \param nSize 클럽이미지 사이즈
	* \return 
	*/
    int	WriteClubMarkImage(
			DWORD dwClub,
			DWORD dwMarkVer,
			BYTE* pData,
			int nSize );
	
	/**
	* 해당클럽의 보유금액을 세팅한다. (Storage)
	* \param dwClub 클럽번호
	* \param llMoney 보유금액 (금액은 >= 0)
	* \return 
	*/
    int	SetClubMoney(DWORD dwClub, LONGLONG llMoney);    

	/**
	* 해당클럽의 보유금액을 가져온다. (Storage)
	* \param dwClub 클럽번호
	* \return 리턴값은 >= 0, 0 보다 작으면 ERROR
	*/
    LONGLONG GetClubMoney(DWORD dwClub);

	/**
	* 해당클럽의 수입금액을 세팅한다. (Income money)
	* \param dwClub 클럽번호
	* \param llMoney 수입금액 (금액은 >= 0)
	* \return 
	*/
    int SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney);    
    
	/**
	* 해당클럽의 수입금액을 가져온다. (Income money)
	* \param dwClub 클럽번호
	* \return >= 0, 0 보다 작으면 ERROR
	*/
    LONGLONG GetClubIncomeMoney(DWORD dwClub);    

	/**
	* 해당클럽의 클럽창고를 읽어온다.
	* \param dwClub 클럽번호
	* \param &ByteStream 버퍼
	* \return 
	*/
    int ReadClubStorage(DWORD dwClub, se::ByteStream &ByteStream);    
	int GetGuildMigrationState( DWORD dwClub );

	/**
	* 해당클럽의 클럽창고를 저장한다.
	* \param dwClub 클럽번호
	* \param pData 창고위치
	* \param nSize 버퍼 전체크기
	* \return 
	*/
    int	WriteClubStorage(DWORD dwClub, BYTE* pData, int nSize);

	/**
	* 클럽 해체시간을 설정한다.
	* \param dwClub 클럽번호
	* \param tDiss 해체시간
	* \return 
	*/
    int SetClubDissolutionTime(DWORD dwClub, __time64_t tDiss);

	/**
	* 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
	* \param dwClub 클럽번호
	* \param tSec 시간
	* \return 
	*/
	int SetClubAllianceSec(DWORD dwClub, __time64_t tSec);

	/**
	* 클럽의 동맹해산 시간을 정한다.
	* \param dwClub 클럽번호
	* \param tDis 해산시간
	* \return 
	*/
	int SetClubAllianceDis(DWORD dwClub, __time64_t tDis);

	/**
	* 클럽의 마스터위임 시간을 정한다.
	* \param dwClub 클럽번호
	* \param tDis 마스터 위임시간
	* \return 
	*/
	int SetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority);

	//! 서버의 모든 클럽정보를 가져온다.
	//! \param &vClub 클럽정보	
    virtual int GetClubInfo(std::vector<std::tr1::shared_ptr<GLClub> > &vClub) override;
    
    //! 캐릭터의 이름과 레벨을 가져온다.
    int GetChaNameLevel(DWORD ChaDbNum, std::string& ChaName, int& ChaLevel);
	
	//! 클럽의 공지사항을 세팅한다.
	//! \param dwClub 클럽번호
	//! \param szClubNotice 공지사항
	virtual int SetClubNotice(const std::string& ChaName, DWORD dwClub, const std::string& ClubNotice) override;

	//! 클럽의 선도클럽 인증 대리자를 세팅한다.
	//! \param dwClub 클럽번호
	//! \param dwDeputy 대리인증자 캐릭터번호
	virtual int SetClubDeputy(DWORD dwClub, DWORD dwDeputy) override;

	//! 클럽의 마스터 권한을 위임한다.
	//! \param dwClub 클럽번호
	//! \param dwMasterID 마스터 권한 위임할 케릭터
	virtual int SetClubAuthority (DWORD dwClub, DWORD dwMasterID) override;
    virtual int InsertClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) override;
    virtual int UpdateClubAuth(DWORD ClubDbNum, size_t AuthIndex, DWORD AuthFlag, const std::string& AuthName) override;
    virtual int ChangeClubAuthIndex(DWORD ClubDbNum, size_t AuthIndexA, size_t AuthIndexB) override;
    virtual int ChangeClubMemberGrade(DWORD ChaDbNum, int Grade) override;

	//! 해당 클럽의 멤버를 가져온다
	//! \param dwClub 클럽번호
	//! \param &vMember 클럽멤버
    virtual int GetClubMember(DWORD dwClub, std::vector<GLCLUBMEMBER> &vMember) override;

    virtual int GetClubAuth(DWORD ClubDbNum, std::vector<club::AUTH_GRADE_DB>& vAuth) override;    
    virtual int ResetClubPosition(DWORD ClubDbNum) override;
    
	//! 새로운 캐릭터를 클럽에 가입시킨다
	//! \param dwClub 클럽번호
	//! \param dwChaNum 가입시키려는 캐릭터번호
    virtual int AddClubMember(DWORD dwClub, DWORD dwChaNum) override;

	//! 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다
	//! \param dwChaNum 캐릭터번호
    virtual int DeleteClubMember(DWORD dwChaNum) override;

	virtual int DeleteClubMemberWithNoPenalty( DWORD dwChaNum ) override { return 0; }

	//! 캐릭터의 클럽번호를 가져온다.
	//! \param dwChaNum 캐릭터번호
    virtual int GetChaGuildNum(DWORD dwChaNum) override;

    virtual int SetClubRegion(DWORD dwRegionID, DWORD dwClub=0, float fTax=0) override;
    virtual int DelClubRegion(DWORD dwRegionID, DWORD dwClub=0) override;
    
	//! 현재 모든지역의 선도 클럽데이터를 가져온다.
	//! \n 주의:최초에는 가져올 값이 없다.
	//! \param &vGUID_DB 
    virtual int GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB) override;

	/**
	* 부 클럽장을 세팅한다
	* \param dwClub 클럽번호
	* \param dwChaNum 캐릭터번호
	* \param dwSubMasterFlags 플래그 (0 으로 넣으면 일반길드원이 된다)
	* \return 
	*/
	int	SetClubMasterFlags(DWORD dwClub, DWORD dwChaNum, DWORD dwSubMasterFlags);

	virtual int SetClubAlliance(DWORD dwClubP, DWORD dwClubS) override;

	/**
	* 동맹 클럽을 해체한다.
	* \param dwClubP 주 클럽번호 (동맹 클럽의 주)
	* \param dwClubS 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
	* \return 
	*/
	int	DelClubAlliance(DWORD dwClubP, DWORD dwClubS);

	/**
	* 동맹 클럽정보를 가져온다.
	* \n 아직 게임 Logic쪽에서 구현이 안된관계로 구현되어 있지 않다.
	* \return 
	*/
	int	GetClubAlliance ( std::vector< std::pair<DWORD,DWORD> > &vecAlliance );
	
	/**
	* 클럽 배틀을 신청한다.
	* \param dwClubP 주클럽번호( 클럽 배틀의 신청을 받은 주 )
	* \param dwClubS 보조 클럽번호( 클럽 배틀의 신청을 한 클럽 )
	* \return
	*/
	int SetClubBattle( DWORD dwClubP, DWORD dwClubS, DWORD dwEndTime, int nAlliance );

	/**
	* 클럽 배틀을 종료한다.
	* \param dwClubP 주클럽 번호( 클럽 배틀의 신청을 받은 주 )
	* \param dwClubS 보조 클럽 번호( 클럽 배틀의 신청을 한 클럽 )
	* \param nGuFlag 승패여부
	* \param nGuKillNum 클럽 배틀 킬수
	* \return
	*/
	int EndClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance );

	/**
	* 클럽 배틀정보를 저장한다,( 클럽 배틀이 종료되기전 서버재시작이 필요한경우 배틀 정보의 저장이 필요함 )
	* \param dwClubP 주클럽 번호( 클럽 배틀의 신청을 받은 주)
	* \param dwClubS 보조 클럽 번호( 클럽 배틀의 신청을 한 클럽 )
	* \param nGuKillNum 클럽 배틀 킬수
	* \return
	*/
	int SaveClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum );

	/**
	* 동맹배틀 전적을 초기화 시키긴다.
	* \param dwClub 클럽 번호
	* \return
	*/
	int ReSetAllianceBattle( DWORD dwClub );

	/**
	* 해당 클럽의 배틀중인 클럽 정보를 가져온다.
	* \param dwClub 클럽번호
	* \param &vMember 클럽배틀 정보
	* \return 
	*/
    int GetClubBattleInfo( DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo );
	
	virtual int UpdateClubName( DWORD dwClub, const std::string& ClubName ) override { return 0; }

	//! DB 에서 캐릭터를 온라인 상태로 만든다.
	//! \param nChaNum 캐릭터번호
	//! \return 
	virtual int SetCharacterOnline(int nChaNum) override;

	//! 캐릭터를 오프라인 상태로 만든다.
	//! \param nChaNum 캐릭터번호
	virtual int SetCharacterOffline(int nChaNum) override;

	virtual int SetAllCharacterOffline() override;

	virtual int SetChaHairColor(DWORD dwChaNum, int nHairColor) override;

	virtual int SetChaHairStyle(DWORD dwChaNum, int nHairStyle) override;

	//! 캐릭터의 얼굴스타일을 변경한다.
	virtual int SetChaFaceStyle(DWORD dwChaNum, int nFaceStyle) override;

	//! 캐릭터의 성별을 변경한다
	virtual int SetChaGenderChange(DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor) override;

	// 캐릭터 학원을 변경한다
	virtual int SetChaSchoolChange(DWORD dwChaNum, int nSchool) override;

	//----------------------------------------------------------------------------
	// PET
	//----------------------------------------------------------------------------
	
	// typedef std::pair < DWORD, PETSKILL> PETSKILL_PAIR;

	/**
	* 새로운 팻을 생성한다.
	* \param nChaNum  팻 소유자 캐릭터 번호
	* \param szPetName 팻이름
	* \param nPetType 팻의 타입
	* \param nPetMID 팻 MID
	* \param nPetSid 팻 SID
	* \param nPetStyle 팻의 스타일
	* \param nPetColor 팻의 컬러
	* \param m_nPetCardMID 팻 카드의 MID
	* \param m_nPetCardSID 팻 카드의 SID
	* \return sc::db::DB_ERROR 또는 성공했을때는 Pet 의 고유번호
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
	* 새로운 팻을 생성한다.
	* \param nPetNum 팻의 고유번호
	* \param szPetName 팻이름
	* \return sc::db::DB_ERROR, sc::db::DB_OK
	*/
	int RenamePet(int nChaNum, int nPetNum, const TCHAR* szPetName);

	/**
	* 팻의 소유권을 이전한다.
	* \param nChaNum 팻의 새로운 소유자
	* \param nPetNum 소유권을 이전할 팻 번호
	*/
	int ExchangePet(int nChaNum, int nPetNum);

	/**
	* 팻의 컬러를 변경한다.
	* \param nPetNum 컬러를 변경하려는 팻의 번호
	* \param nPetColor 팻의 컬러
	*/
	int SetPetColor(int nChaNum, int nPetNum, int nPetColor);

	/**
	* 팻의 스타일을 변경한다.
	* \param nPetNum 스타일을 변경하려는 팻의 번호
	* \param nPetStyle 팻의 스타일
	*
	*/
	int SetPetStyle(int nChaNum, int nPetNum, int nPetStyle);

	/**
	* 팻의 포만도를 설정한다. 포만도는 0-100 사이
	* \param nPetNum 포만도를 설정할 팻의 번호
	* \param nPetFull 포만도
	*/
	int SetPetFull( int nChaNum, int nPetNum, int nPetFull );

	/**
	* 팻의 인벤토리 업데이트
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
	* 팻의 인벤토리 업데이트
	*/
	int SetPetInven(
		int nPetChaNum,
		int nPetNum,
		se::ByteStream &ByteStream );

	virtual int SetPetMigrationState( int ChaNum, int PetNum, int MigrationState ) override { return 0; }
	virtual int SetVehicleMigrationState( int ChaNum, int VehicleNum, int MigrationState ) override { return 0; }

	/**
	* 팻의 스킬 슬롯 갯수 업데이트
	*/
	int SetPetSkillSlotNum(
		int nChaNum,
		int nPetNum,
		WORD wSkillSlotNum );

	/**
	* 팻을 삭제한다.
	* \param nPetNum 삭제할 팻의 번호
	*/
	int	DeletePet(int nChaNum, int nPetNum);
	
	virtual int GetPetSkill(int nChaNum, GLPET* pPet, DWORD dwPetNum) override;

	/**
	* 팻의 인벤토리 정보를 가져온다
	*/
	int GetPetInven( int nChaNum, GLPET* pPet, DWORD dwPetNum );

	/**
	* 팻의 정보를 가져온다.
	*/
	virtual int GetPet(GLPetField* pPet, DWORD dwPetNum, DWORD dwClientID) override;

	/**
	* 팻의 포만감을 가져온다
	* \param dwPetNum 팻 번호
	* \return 팻의 포만감 or sc::db::DB_ERROR
	*/
	int GetPetFull( int nChaNum, DWORD dwPetNum );
	
	virtual int GetRestorePetList(int nChaNum, std::vector<PET_RELIST> &vecPetReList) override;

	/**
	* 팻을 부활시킨다.
	* \param nPetNum 부활할 팻 번호
	* \param nChaNum 부활을 요구한 캐릭터 번호
	*/
	int RestorePet( DWORD dwPetNum, int nChaNum );

	/**
	* 팻의 스킨팩을 업데이트 한다.
	* \param nChaNum		팻소유 캐릭터 번호
	* \param dwPetNum		펫 번호
	* \param nPetSkinMID	펫 스킨 MID
	* \param nPetSkinSID	펫 스킨 SID
	* \param nPetSkinScale	펫 스킨 Scale
	* \param nPetSkinTime	펫 스킨 적용시간
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
	* 로그인하려는 사용자를 체크한다.	
	* \param szUsrID 사용자 ID
	* \param szPasswd 사용자 패스워드
	* \param szUsrIP 사용자의 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return DB 인증후 결과값
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
	* 로그인하려는 사용자를 체크한다.	
	* \param szUsrID 사용자 ID
	* \param szPasswd 사용자 패스워드
	* \param szUsrIP 사용자의 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return DB 인증후 결과값
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
	* \param szUsrIP 로그인하려는 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
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
	* \n 로그인하려는 사용자를 체크한다.
	* \param 
	* \param 
	* \param szUsrIP 로그인하려는 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
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
	* \n 로그인하려는 사용자를 체크한다.
	* \param szTerraLoginName Terra TLoginName varchar(15)
	* \param szTerraTID Web 에서 넘어온 문자열 (ENCODING 문자열)
	* \param szUsrIP 로그인하려는 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
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
	* \param szUserID 사용자ID
	* \param szUsrIP IP
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return 
	*/
	int ExciteUserCheck(
			const TCHAR* szUserID,
			const TCHAR* szUserIP,
			int nSvrGrp, 
			int nSvrNum );

	/**
	* Gonzo (Japan)
	* \param szUserID 사용자ID
	* \param szUsrIP IP
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return 
	*/
	int JapanUserCheck(
			const TCHAR* szUserID,
			const TCHAR* szUserIP,
			int nSvrGrp, 
			int nSvrNum,
			int nUserNum);


	/**
	* 글로벌 서비스
	* 로그인하려는 사용자를 체크한다.	
	* \param szUsrID 사용자 ID
	* \param szPasswd 사용자 패스워드
	* \param szUsrIP 사용자의 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return DB 인증후 결과값
	*/
	int	GsUserCheck(
		const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		int nSvrGrp,
		int nSvrNum);

	/**
	* EU 서비스
	* 로그인하려는 사용자를 체크한다.	
	* \param szUsrID 사용자 ID
	* \param szPasswd 사용자 패스워드
	* \param szUsrIP 사용자의 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return DB 인증후 결과값
	*/
	int	GmUserCheck(
		const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		int nSvrGrp,
		int nSvrNum );

	/**
	* US 서비스
	* 로그인하려는 사용자를 체크한다.	
	* \param szUsrID 사용자 ID
	* \param szPasswd 사용자 패스워드
	* \param szUsrIP 사용자의 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return DB 인증후 결과값
	*/
	int	UsUserCheck(
		const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		int nSvrGrp,
		int nSvrNum );


	/**
	* 해당사용자를 Logout 한다
	* \param szUsrID 사용자 ID
	* \param nUsrNum 사용자번호
	* \param nGameTime 게임플레이시간 
	* \param nChaNum 캐릭터번호
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
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
	* 해당사용자를 Logout 한다
	* \param szTID Terra User ID
	* \param nUsrNum 사용자번호
	* \param nGameTime 게임플레이시간
	* \param nChaNum 캐릭터번호
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
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
	* \n 사용자를 로그아웃 시킨다
	* \param szUsrID 사용자 ID
	* \return 
	*/
	int	UserLogoutSimple( const TCHAR* szUsrID );

	/**
	* Maya (Taiwan/Hongkong)
	* \n 사용자를 로그아웃 시킨다.
	* \param nUserNum 사용자번호
	* \return 
	*/
	int UserLogoutSimple2( int nUserNum );

	//! 해당서버의 사용자가 있을때만 로그아웃 시킨다
	//! \param szUsrID 사용자 ID
	//! \param nSvrGrp 서버 그룹
	virtual int UserLogoutSimple3(const TCHAR* szUsrID, int nSvrGrp) override;

    virtual int UserDbLogItemDataInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName) override;
	
	//! Feya (Taiwan/Hongkong)
	//! 해당 게임서버의 전체 유저를 로그아웃 시킨다.
	//! \param nSvrGrp 서버그룹
	//! \param nSvrNum 서버번호
	virtual int UserLogoutSvr(int nSvrGrp, int nSvrNum) override;
	
	/**
	* Feya (Taiwan/Hongkong)
	* \n DB 사용자 정보의 마지막 플레이한 캐릭터 이름을 세팅한다.
	* \param nUsrNum 사용자번호
	* \param szChaName 캐릭터이름
	* \return 
	*/
	int	UserUpdateCha(int nUsrNum, const TCHAR* szChaName);		
	
	/**	
	* 해당 사용자의 사용자 번호를 가져온다.
	* \param szUsrId 사용자 ID
	* \return 사용자번호
	*/
	int	GetUserNum(const TCHAR* szUsrId);

	int GetUserChaRemain(int nUserNum);
	int	GetUserChaTestRemain(int nUserNum);
	int	UserChaNumDecrease(int nUserNum);
	/*
	* TEST 서버 캐릭터 생성갯수 감소
	*/
	int	UserTestChaNumDecrease(int nUserNum);

	/**
	* 캐릭터 삭제, 캐릭터생성 갯수 증가
	*/
	int UserChaNumIncrease(int nUserNum);

	/**
	* TEST 서버 캐릭터 삭제, 갯수 증가
	*/
	int UserTestChaNumIncrease(int nUserNum);
	
	virtual int GetUserType(int nUserNum) override;
	virtual int GetUserCountryInfo ( IN int nUserNum, OUT Country::SCOUNTRY_INFO& sCountryInfo ) override;

	/**	
	* 해당 사용자의 프리미엄 기간을 가져온다.
	* \param nUserNum 사용자번호
	* \return 프리미엄기간(종료일)
	*/
    __time64_t GetPremiumTime(int nUserNum);	

	/**	
	* 해당 사용자의 계정 생성 날짜를 가져온다.
	* \param nUserNum 사용자번호
	* \return 계정 생성 시간(생성일)
	*/
	__time64_t GetCreateDate(const int nUserNum);	
    
	/**	
	* 해당 사용자의 프리미엄 기간을 세팅한다.
	* \param nUserNum 사용자번호
	* \param tPremiumTime 프리미엄 종료시간
	* \return 
	*/
    int SetPremiumTime(int nUserNum, __time64_t tPremiumTime);	

	/**
	* 해당 사용자의 채팅 블록시간을 가져온다
	* \param nUserNum 사용자번호
	* \return 채팅블록 만료시간
	*/
	__time64_t GetChatBlockTime(int nUserNum);

	//! 해당 사용자의 채팅 블록시간을 설정한다
	//! \param nUserNum 사용자번호
	//! \param tBlockTime 채팅블록 만료시간
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
	* 사용자 정보를 가져온다
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
	* 사용자 정보를 가져온다
	* Thailand
	* \param szUserID 
	* \param pMsg 
	* \return 
	*/
	int ThaiGetUserInfo( const TCHAR* szUserID, NET_LOGIN_FEEDBACK_DATA2* pMsg );

	/**
	* Terra 사용자 정보를 가져온다.
	* \param szTLoginName TLoginName varchar(15)
	* \param pMsg
	* \return 
	*/
	int TerraGetUserInfo( const TCHAR* szTLoginName, TERRA_NET_LOGIN_FEEDBACK_DATA2* pMsg );

	/**
	* GSP 사용자 정보를 가져온다.
	* \param szUserID UserID varchar(50)
	* \param pMsg
	* \return 
	*/
	int GspGetUserInfo( const TCHAR* szUserID, GSP_NET_LOGIN_FEEDBACK_DATA2* pMsg );
	
	//! Excite 사용자 정보를 가져온다.
	//! \param szUserID UserID varchar(20)
	//! \param pMsg
	virtual int ExciteGetUserInfo(const TCHAR* szUserID, EXCITE_NET_LOGIN_FEEDBACK_DATA2* pMsg) override;

	//! Japan(Gonzo) 사용자 정보를 가져온다.
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

	//! 글로벌서비스 사용자 정보를 가져온다.
	//! \param szUserID UserID varchar(16)
	//! \param pMsg	
	virtual int GsGetUserInfo(const TCHAR* szUserID, GS_NET_LOGIN_FEEDBACK_DATA2* pMsg) override;

	//! EU 사용자 정보를 가져온다.
	//! \param szUserID UserID varchar(16)
	//! \param pMsg	
	virtual int GmGetUserInfo( const TCHAR* szUserID, GM_NET_LOGIN_FEEDBACK_DATA2* pMsg ) override;

	//! US 사용자 정보를 가져온다.
	//! \param szUserID UserID varchar(16)
	//! \param pMsg	
	virtual int UsGetUserInfo( const TCHAR* szUserID, US_NET_LOGIN_FEEDBACK_DATA2* pMsg ) override;
	
	/**
	* 해당사용자를 블록시킨다.
	* \param nUserNum 사용자번호
	* \param nDay 블럭일수 1 - 15 까지
	* \return 
	*/
    int UserBlock( int nUserNum, int nDay );

	/**
	* 해당사용자를 블록시킨다.
	* \param nUserNum 사용자번호
	* \param nHour 시간
	* \return 
	*/
    int UserBlockDate( int nUserNum, __time64_t tBlockTime );
	
	/**
	* 해당 게임서버의 전체 유저를 로그아웃 시킨다.
	* \param nSvrGrp 
	* \return 
	*/
	int AllServerUserLogout(int nSvrGrp);

	/**
	* 전체 유저를 로그아웃 시킨다.
	* \return 
	*/
	int	AllUserLogout();

	/**
	* TERRA
	* 유저의 패스워드를 넣거나 체크한다.
	* \param szTID Terra UserID
	* \param szPasswd 사용자 패스워드
	* \return 
	*/
	int TerraUserPassCheck(
				const TCHAR* szTID,
				const TCHAR* szPasswd,
				int	nCheckFlag );

	/**
	* Daum
	* 유저의 패스워드를 넣거나 체크한다.
	* \param szDaumGID Daum UserGID
	* \param szDaumPass 사용자 패스워드
	* \return 
	*/
	int DaumUserPassCheck(
				const TCHAR* szDaumGID,
				const TCHAR* szDaumPasswd,
				int	nCheckFlag );


	/**
	* Excite
	* 유저의 패스워드를 넣거나 체크한다.
	* \param szUID Excite UserID
	* \param szUserID Excite 2차 UserID
	* \param szPasswd 사용자 패스워드
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
	* 사용자의 PC방 클래스를 체크한다.	
	* \param szUsrIP 사용자의 IP Address
	* \return DB 인증후 결과값
	*/
	int	MyUserClassCheck(
			const TCHAR* szUsrIP );

	/**
	* 필리핀
	* 사용자의 PC방 클래스를 체크한다.	
	* \param szUsrIP 사용자의 IP Address
	* \return DB 인증후 결과값
	*/
	int	PhUserClassCheck(
			const TCHAR* szUsrIP );
	
	/**
	* 해당 사용자의 오늘 게임플레이 시간(분)을 가져온다.
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
	* 아이템의 랜덤옵션 로그를 남긴다.
	* \param _sLOG 랜덤옵션 구조체
	*/
	int LogRandomItem( const LOG_RANDOM_OPTION &_sLOG );
	int LogItemCostumeStat(const LOG_COSTUME_STATS &_sLOG);

	virtual int LogAddonItem( const LOG_ADDON_OPTION &_sLOG ) override { return 0; }
	virtual int LogLinkSkillItem( const LOG_LINKSKILL_OPTION &_sLOG ) override { return 0; }
	virtual int LogBasicStatItem( const LOG_BASICSTAT_OPTION &_sLOG ) override { return 0; }

	/**
	* 펫의 액션로그를 남긴다.
	* \param _sLOG 펫액션로그 구조체
	*/
	int LogPetAction( const LOG_PET_ACTION &_sLOG );

	//! 여름방학 PC 방 프로모션 복권관련'선물펑펑!!' 이벤트
    /*
	int	LogLottery(
			CString strLottery, 
		    CString strIP, 
			CString strUserUID, 
			int nUserNum );
    */

	// 여름방학 PC 프로모션 아이템 '펑펑' 이벤트
    /*
	int LogPungPungCheck(
			CString strIP, 
		    int nUserNum );
    */    
	// 여름방학 PC 프로모션 아이템 '펑펑' 이벤트
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

	//! 게임내에서 진정이나 신고로 신고한 내용을 DB에 기록한다.
	//! \param nSGNum 서버그룹
	//! \param nChaNum 신고한 캐릭터번호
	//! \param strMsg 신고내용 (최대크기는 CHAT_MSG_SIZE)
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
	* 캐릭터의 친구정보를 가져온다.
	* \param nChaNum 친구정보를 가져올 캐릭터 번호
	* \param vFriend 친구정보를 담을 vector
	*/
	int	GetChaFriendSMS(
			int nChaNum,
			std::vector<CHA_FRIEND_SMS> &vFriend );

	/**************************************************************************
	* Shop Database
    */
    
    //! Item Shop 에서 구입한 아이템을 가져온다
    //! Shop 에서 구입한 아이템을 가져온다.
    //! 가져온 아이템은 실제로 가져 갈 수 있는 아이템이 아니다.
    //! 아이템을 가져갈때는 실제로 가져갈 수 있는지 다시 확인해야 한다. (SetPurchaseItem)
    //! 웹을 통한 구매와 취소 및 게임내 캐릭터의 동기화 문제.	
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
        ) override { return sc::db::DB_ERROR; };

    // 전장 점수 저장;
    virtual int CompetitionRatingUpdate(
        const std::string& strContentType,
        const DWORD charDbNum,
        const int Rating,
        const int WinningStreak,
        const int PlayCount,
        const __time64_t tLastPlayDate
        ) override { return sc::db::DB_ERROR; };

    // 전장 시즌 업데이트;
    virtual int CompetitionSeasonUpdate(
        const std::string& strContentType,
        const int seasonNum,
        const int baseRating
        ) override { return sc::db::DB_ERROR; };

    // 전장 로그 CompetitionLog


	// 인증 서버 로그 기록
	virtual int AddAuthServerState( int nCounrty, int nServerType, const TCHAR* szIP, int nPort, int nCurUser, int nMaxUser, const __time64_t& LogDate ) override;
	// 인증 절차 수행
	virtual int ProcessCertificationForAuth( int isSessionSvr, int nCounrty, int nServerType, const TCHAR* szIP, int nPort, std::string strUniqKey, int &certification, std::string &strNewUniqKey, int &nSessionSvrID ) override;
	// 이벤트 정보 기록
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

    //! ITEM ID를 통해 아이템을 가지고 온다. ( bDateCheck => 날짜(Date)검사 여부 )
    virtual int ConsignmentsSaleGet( sc::MGUID& Guid, private_market::ConsignmentSaleItem& SaleItem, bool bDateCheck = false  ) override;
    //! ChaNum을 통해 등록한 물품을 가지고 온다. 
    virtual int ConsignmentsSaleSelect( private_market::SALE_ITEMS_VECTOR& vecItem, const DWORD& dwChaNum = 0, const int& ServerGroup = 0 ) override;
    //! 남은 갯수를 바꾼다.
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

	// 필리핀 유저이전
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

    // GM LOG ( GM 아이템 넣어주기 )
    virtual int WriteGMLog(int nGmNum, std::string& strCmd, std::string& strUserIP) override { return 0; };

	// 란모바일
	virtual int RanMobileRequest(DWORD dwUserDBNum, DWORD dwChaDBNum, int nServerNum, int nCommand, int nValue, const std::string& strChaName, int nChaClass ) override;

	// Log Money;
	virtual int MoneyLogInsert ( const int _nMainType, const int _nSubType, const LONGLONG _llMoney ) { return sc::db::DB_ERROR; }

	//////////////////////////////////////////////////////////////////////////
	// 기간제 아이템 시간 연장
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
